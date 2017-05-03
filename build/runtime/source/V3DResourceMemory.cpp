#include "V3DResourceMemory.h"
#include "V3DDevice.h"
#include "V3DBuffer.h"
#include "V3DImage.h"
#include "V3DAdapter.h"

/******************************/
/* public - V3DResourceMemory */
/******************************/

V3DResourceMemory* V3DResourceMemory::Create()
{
	return V3D_NEW_T(V3DResourceMemory);
}

V3D_RESULT V3DResourceMemory::Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint64_t size)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(propertyFlags != 0);
	V3D_ASSERT(size != 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_Source.memoryPropertyFlags = ToVkMemoryPropertyFlags(propertyFlags);

	// ----------------------------------------------------------------------------------------------------
	// メモリを確保
	// ----------------------------------------------------------------------------------------------------

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = m_pDevice->GetInternalAdapterPtr()->Vulkan_GetMemoryTypeIndex(m_Source.memoryPropertyFlags);

	VkResult vkResult = vkAllocateMemory(m_pDevice->GetSource().device, &allocInfo, nullptr, &m_Source.deviceMemory);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Source.memoryMappedRange.memory = m_Source.deviceMemory;

	// ----------------------------------------------------------------------------------------------------
	// 記述を設定
	// ----------------------------------------------------------------------------------------------------

	m_Desc.propertyFlags = propertyFlags;
	m_Desc.size = size;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT V3DResourceMemory::Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint32_t resourceCount, IV3DResource** ppResources)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(propertyFlags != 0);
	V3D_ASSERT(resourceCount != 0);
	V3D_ASSERT(ppResources != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	// ----------------------------------------------------------------------------------------------------
	// リソースをアライメントの大きい順にソート
	// ----------------------------------------------------------------------------------------------------

	STLVector<IV3DResource*> resources;
	resources.reserve(resourceCount);

	for (uint32_t i = 0; i < resourceCount; i++)
	{
#ifdef _DEBUG
		switch (ppResources[i]->GetResourceDesc().type)
		{
		case V3D_RESOURCE_TYPE_BUFFER:
			if (static_cast<V3DBuffer*>(ppResources[i])->CheckBindMemory() == true)
			{
				V3D_LOG_ERROR(Log_Error_AlreadyBindResourceMemory, V3D_LOG_TYPE(ppResources), i);
				return V3D_ERROR_FAIL;
			}
			break;
		case V3D_RESOURCE_TYPE_IMAGE:
			if (static_cast<V3DImage*>(ppResources[i])->CheckBindMemory() == true)
			{
				V3D_LOG_ERROR(Log_Error_AlreadyBindResourceMemory, V3D_LOG_TYPE(ppResources), i);
				return V3D_ERROR_FAIL;
			}
			break;
		}
#endif //_DEBUG

		resources.push_back(ppResources[i]);
	}

	std::sort(resources.begin(), resources.end(), [](const IV3DResource* lh, const IV3DResource* rh) { return lh->GetResourceDesc().memoryAlignment > rh->GetResourceDesc().memoryAlignment; });

	// ----------------------------------------------------------------------------------------------------
	// アライメントを気にしつつ、メモリのサイズを求める
	// ----------------------------------------------------------------------------------------------------

	uint64_t vkMinAlignment = m_pDevice->GetSource().deviceProps.limits.minMemoryMapAlignment;
	VkDeviceSize vkAllocSize = 0;

	STLVector<VkDeviceSize> vkOffsets;
	vkOffsets.resize(resourceCount);

	for (uint32_t i = 0; i < resourceCount; i++)
	{
		const V3DResourceDesc& resourceDesc = ppResources[i]->GetResourceDesc();

		VkDeviceSize vkAlignment = V3D_MAX(vkMinAlignment, resourceDesc.memoryAlignment);
		if (vkAllocSize % vkAlignment)
		{
			vkAllocSize = (vkAllocSize / vkAlignment) * vkAlignment + vkAlignment;
		}

		vkOffsets[i] = vkAllocSize;
		vkAllocSize += resourceDesc.memorySize;
	}

	// ----------------------------------------------------------------------------------------------------
	// メモリを作成
	// ----------------------------------------------------------------------------------------------------

	m_Source.memoryPropertyFlags = ToVkMemoryPropertyFlags(propertyFlags);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = vkAllocSize;
	allocInfo.memoryTypeIndex = m_pDevice->GetInternalAdapterPtr()->Vulkan_GetMemoryTypeIndex(m_Source.memoryPropertyFlags);

	VkResult vkResult = vkAllocateMemory(m_pDevice->GetSource().device, &allocInfo, nullptr, &m_Source.deviceMemory);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Source.memoryMappedRange.memory = m_Source.deviceMemory;

	// ----------------------------------------------------------------------------------------------------
	// 記述を設定
	// ----------------------------------------------------------------------------------------------------

	m_Desc.propertyFlags = propertyFlags;
	m_Desc.size = vkAllocSize;

	// ----------------------------------------------------------------------------------------------------
	// リソースをバインド
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = V3D_ERROR_FAIL;

	for (uint32_t i = 0; i < resourceCount; i++)
	{
		IV3DResource* pResource = ppResources[i];

		switch (pResource->GetResourceDesc().type)
		{
		case V3D_RESOURCE_TYPE_BUFFER:
			result = static_cast<V3DBuffer*>(pResource)->BindMemory(this, vkOffsets[i]);
			if (result != V3D_OK)
			{
				return result;
			}
			break;

		case V3D_RESOURCE_TYPE_IMAGE:
			result = static_cast<V3DImage*>(pResource)->BindMemory(this, vkOffsets[i]);
			if (result != V3D_OK)
			{
				return result;
			}
			break;
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DResourceMemory::Source& V3DResourceMemory::GetSource() const
{
	return m_Source;
}

V3D_RESULT V3DResourceMemory::Map(uint64_t offset, uint64_t size, void** ppMemory)
{
	if (m_Desc.size < (offset + size))
	{
		return V3D_ERROR_FAIL;
	}

	if (m_pMemory != nullptr)
	{
		*ppMemory = m_pMemory + offset;
		return V3D_OK;
	}

	if (m_Source.memoryMappedRange.size != 0)
	{
		return V3D_ERROR_FAIL;
	}

	VkResult vkResult = vkMapMemory(m_pDevice->GetSource().device, m_Source.deviceMemory, offset, size, 0, ppMemory);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Source.memoryMappedRange.offset = offset;
	m_Source.memoryMappedRange.size = size;

	if ((m_Source.memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	{
		vkResult = vkInvalidateMappedMemoryRanges(m_pDevice->GetSource().device, 1, &m_Source.memoryMappedRange);
		if (vkResult != VK_SUCCESS)
		{
			return ToV3DResult(vkResult);
		}
	}

	return V3D_OK;
}

V3D_RESULT V3DResourceMemory::Unmap()
{
	if (m_pMemory != nullptr)
	{
		return V3D_OK;
	}

	if (m_Source.memoryMappedRange.size == 0)
	{
		return V3D_ERROR_FAIL;
	}

	V3D_RESULT result = V3D_OK;

	if ((m_Source.memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	{
		VkResult vkResult = vkFlushMappedMemoryRanges(m_pDevice->GetSource().device, 1, &m_Source.memoryMappedRange);
		if (vkResult != VK_SUCCESS)
		{
			result = ToV3DResult(vkResult);
		}
	}

	m_Source.memoryMappedRange.offset = 0;
	m_Source.memoryMappedRange.size = 0;

	vkUnmapMemory(m_pDevice->GetSource().device, m_Source.deviceMemory);

	return result;
}

#ifdef _DEBUG

bool V3DResourceMemory::Debug_CheckMemory(uint64_t offset, uint64_t size)
{
	return (m_Desc.size >= (offset + size));
}

#endif //_DEBUG

/****************************************/
/* public override - IV3DResourceMemory */
/****************************************/

const V3DResourceMemoryDesc& V3DResourceMemory::GetDesc() const
{
	return m_Desc;
}

V3D_RESULT V3DResourceMemory::BeginMap()
{
	if (m_Source.memoryMappedRange.size != 0)
	{
		return V3D_ERROR_FAIL;
	}

	VkResult vkResult = vkMapMemory(m_pDevice->GetSource().device, m_Source.deviceMemory, 0, m_Desc.size, 0, reinterpret_cast<void**>(&m_pMemory));
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Source.memoryMappedRange.offset = 0;
	m_Source.memoryMappedRange.size = m_Desc.size;

	if ((m_Source.memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	{
		vkResult = vkInvalidateMappedMemoryRanges(m_pDevice->GetSource().device, 1, &m_Source.memoryMappedRange);
		if (vkResult != VK_SUCCESS)
		{
			return ToV3DResult(vkResult);
		}
	}

	return V3D_OK;
}

V3D_RESULT V3DResourceMemory::EndMap()
{
	if (m_Source.memoryMappedRange.size == 0)
	{
		return V3D_ERROR_FAIL;
	}

	V3D_RESULT result = V3D_OK;

	if ((m_Source.memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
	{
		VkResult vkResult = vkFlushMappedMemoryRanges(m_pDevice->GetSource().device, 1, &m_Source.memoryMappedRange);
		if (vkResult != VK_SUCCESS)
		{
			result = ToV3DResult(vkResult);
		}
	}

	m_Source.memoryMappedRange.offset = 0;
	m_Source.memoryMappedRange.size = 0;

	vkUnmapMemory(m_pDevice->GetSource().device, m_Source.deviceMemory);

	m_pMemory = nullptr;

	return result;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DResourceMemory::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DResourceMemory::GetRefCount() const
{
	return m_RefCounter;
}

void V3DResourceMemory::AddRef()
{
	++m_RefCounter;
}

void V3DResourceMemory::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DResourceMemory);
	}
}

/*******************************/
/* private - V3DResourceMemory */
/*******************************/

V3DResourceMemory::V3DResourceMemory() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Desc({}),
	m_Source({}),
	m_pMemory(nullptr)
{
	m_Source.deviceMemory = VK_NULL_HANDLE;
	m_Source.memoryMappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	m_Source.memoryMappedRange.pNext = nullptr;
}

V3DResourceMemory::~V3DResourceMemory()
{
	if (m_pDevice != nullptr)
	{
		m_pDevice->NotifyReleaseResourceMemory();
	}

	if (m_Source.deviceMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(m_pDevice->GetSource().device, m_Source.deviceMemory, nullptr);
	}

	V3D_RELEASE(m_pDevice);
}
