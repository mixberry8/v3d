#include "V3DDescriptorSetLayout.h"
#include "V3DDevice.h"
#include "ScopedLock.h"
#include <algorithm>

struct V3DDescriptorSetLayout::HandleT
{
	uint32_t maxCount;
	uint32_t count;

	VkDescriptorPool pool;

#ifdef _DEBUG
	void* pDebugPtr;
#endif //_DEBUG

	V3D_DECLARE_ALLOCATOR
};

/***********************************/
/* public - V3DDescriptorSetLayout */
/***********************************/

V3DDescriptorSetLayout* V3DDescriptorSetLayout::Create()
{
	return V3D_NEW_T(V3DDescriptorSetLayout);
}

V3D_RESULT V3DDescriptorSetLayout::Initialize(V3DDevice* pDevice, uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(descriptorCount > 0);
	V3D_ASSERT(descriptors != nullptr);
	V3D_ASSERT(poolSize > 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトの作成に必要な情報を収集
	// ----------------------------------------------------------------------------------------------------

	uint32_t maxBinding = 0;

	m_Source.poolSizes.resize(VK_DESCRIPTOR_TYPE_RANGE_SIZE);
	memcpy_s(m_Source.poolSizes.data(), sizeof(V3DDescriptorSetLayout::DescriptorPoolSizeInitList),
		V3DDescriptorSetLayout::DescriptorPoolSizeInitList, sizeof(V3DDescriptorSetLayout::DescriptorPoolSizeInitList));

	V3DFlags vkPoolSizeFlags = 0;
	uint32_t vkPoolSizeCount = 0;

	for (uint32_t i = 0; i < descriptorCount; i++)
	{
		const V3DDescriptorDesc& src = descriptors[i];

		maxBinding = V3D_MAX(maxBinding, src.binding);

		VkDescriptorType vkDescType = ToVkDescriptorType(src.type);

		m_Source.poolSizes[vkDescType].descriptorCount++;

		V3D_ASSERT(32 > vkDescType);
		uint32_t poolSizeBit = 1 << vkDescType;
		if ((vkPoolSizeFlags & poolSizeBit) == 0)
		{
			vkPoolSizeCount++;
			vkPoolSizeFlags |= poolSizeBit;
		}
	}

	m_Source.references.resize(maxBinding + 1, ~0U);

	std::sort(m_Source.poolSizes.begin(), m_Source.poolSizes.end(), [](const VkDescriptorPoolSize& lh, const VkDescriptorPoolSize& rh) { return lh.descriptorCount > rh.descriptorCount; });
	m_Source.poolSizes.resize(vkPoolSizeCount);
	m_Source.poolSizes.shrink_to_fit();

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトの作成情報を作成
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
	vkDescriptorSetLayoutBindings.resize(descriptorCount);

	m_Source.descriptors.resize(descriptorCount, V3DDescriptorSetLayout::DescriptorInit);

	for (uint32_t i = 0; i < descriptorCount; i++)
	{
		const V3DDescriptorDesc& src = descriptors[i];

		VkDescriptorSetLayoutBinding& dst1 = vkDescriptorSetLayoutBindings[i];
		dst1.binding = src.binding;
		dst1.descriptorType = ToVkDescriptorType(src.type);
		dst1.descriptorCount = 1;
		dst1.stageFlags = ToVkShaderStageFlags(src.stageFlags);
		dst1.pImmutableSamplers = nullptr;

		V3DDescriptorSetLayout::Descriptor& dst2 = m_Source.descriptors[i];
		dst2.binding = src.binding;
		dst2.type = dst1.descriptorType;
		dst2.write = i;

		switch (dst1.descriptorType)
		{
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			dst2.resourceFlags = V3DDescriptorSetLayout::RESOURCE_BUFFER;
			dst2.resource = m_Source.bufferCount;
			m_Source.bufferCount++;
			break;

		case V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			dst2.resourceFlags = V3DDescriptorSetLayout::RESOURCE_BUFFER_VIEW;
			dst2.resource = m_Source.bufferViewCount;
			m_Source.bufferViewCount++;
			break;

		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:       // image -------
			dst2.resourceFlags = V3DDescriptorSetLayout::RESOURCE_IMAGE;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		case VK_DESCRIPTOR_TYPE_SAMPLER:                // ----  sampler
			dst2.resourceFlags = V3DDescriptorSetLayout::RESOURCE_SAMPLER;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // image sampler
			dst2.resourceFlags = V3DDescriptorSetLayout::RESOURCE_IMAGE | V3DDescriptorSetLayout::RESOURCE_SAMPLER;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		}

		m_Source.references[src.binding] = i;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo{};
	descSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutCreateInfo.pNext = nullptr;
	descSetLayoutCreateInfo.flags = 0;
	descSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
	descSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.data();

	VkResult vkResult = vkCreateDescriptorSetLayout(m_pDevice->GetSource().device, &descSetLayoutCreateInfo, nullptr, &m_Source.descriptorSetLayout);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタプールを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorPoolCreateInfo& poolCreateInfo = m_Source.poolCreateInfo;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolCreateInfo.maxSets = poolSize;
	poolCreateInfo.poolSizeCount = vkPoolSizeCount;
	poolCreateInfo.pPoolSizes = m_Source.poolSizes.data();

	V3DDescriptorSetLayout::Handle dummyHandle;
	V3D_RESULT result = AddHandle(&dummyHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	poolCreateInfo.maxSets = poolResizeStep;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DDescriptorSetLayout::Source& V3DDescriptorSetLayout::GetSource() const
{
	return m_Source;
}

const V3DDescriptorSetLayout::Descriptor* V3DDescriptorSetLayout::GetDescriptor(uint32_t binding, V3DFlags resourceFlags) const
{
	if ((m_Source.references.size() <= binding) || (m_Source.references[binding] == 0xFFFFFFFF))
	{
		return nullptr;
	}

	const V3DDescriptorSetLayout::Descriptor& descriptor = m_Source.descriptors[m_Source.references[binding]];
	if ((descriptor.resourceFlags & resourceFlags) != resourceFlags)
	{
		return nullptr;
	}

	V3D_ASSERT(descriptor.binding != ~0U);
	V3D_ASSERT(descriptor.resource != ~0U);

	return &descriptor;
}

V3D_RESULT V3DDescriptorSetLayout::Vulkan_CreateDescriptorSet(V3DDescriptorSetLayout::Handle* pHandle, VkDescriptorSet* pDescriptorSet)
{
	ScopedLock lock(m_CriticalSection);

	// ----------------------------------------------------------------------------------------------------
	// ハンドルを取得
	// ----------------------------------------------------------------------------------------------------

	V3DDescriptorSetLayout::HandleT* pHandleT = m_Handles.back();

	if (pHandleT->count == pHandleT->maxCount)
	{
		V3D_ASSERT(m_Statistics.setCount == m_Statistics.setCapacity);

		V3D_RESULT result = AddHandle(&pHandleT);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorSetAllocateInfo descSetAllocInfo{};
	descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descSetAllocInfo.pNext = nullptr;
	descSetAllocInfo.descriptorPool = pHandleT->pool;
	descSetAllocInfo.descriptorSetCount = 1;
	descSetAllocInfo.pSetLayouts = &m_Source.descriptorSetLayout;

	VkDescriptorSet descriptorSet;
	VkResult vkResult = vkAllocateDescriptorSets(m_pDevice->GetSource().device, &descSetAllocInfo, &descriptorSet);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	pHandleT->count++;
	m_Statistics.setCount++;

	if ((pHandleT->count == pHandleT->maxCount) && (m_Statistics.setCount < m_Statistics.setCapacity))
	{
		// いっぱいになったのでソート
		std::sort(m_Handles.begin(), m_Handles.end(), [](const V3DDescriptorSetLayout::HandleT* lh, const V3DDescriptorSetLayout::HandleT* rh) { return lh->count > rh->count; });
	}

	*pHandle = pHandleT;
	*pDescriptorSet = descriptorSet;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void V3DDescriptorSetLayout::Vulkan_DestroyDescriptorSet(V3DDescriptorSetLayout::Handle handle, VkDescriptorSet descriptorSet)
{
	V3D_ASSERT(handle != nullptr);
	V3D_ASSERT(descriptorSet != VK_NULL_HANDLE);

	V3D_ASSERT(handle->pDebugPtr == this);
	V3D_ASSERT(handle->count > 0);
	V3D_ASSERT(m_Statistics.setCount > 0);

	ScopedLock lock(m_CriticalSection);

	vkFreeDescriptorSets(m_pDevice->GetSource().device, handle->pool, 1, &descriptorSet);

	handle->count--;
	if ((handle->count == 0) || (m_Statistics.setCount == m_Statistics.setCapacity))
	{
		// からっぽになった、前回までいっぱいだったが空きができたのでソート
		std::sort(m_Handles.begin(), m_Handles.end(), [](const V3DDescriptorSetLayout::HandleT* lh, const V3DDescriptorSetLayout::HandleT* rh) { return lh->count > rh->count; });
	}

	m_Statistics.setCount--;
}

/*********************************************/
/* public override - IV3DDescriptorSetLayout */
/*********************************************/

const V3DDescriptorSetLayoutStatistics& V3DDescriptorSetLayout::GetStatistics() const
{
	return m_Statistics;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DDescriptorSetLayout::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DDescriptorSetLayout::GetRefCount() const
{
	return m_RefCounter;
}

void V3DDescriptorSetLayout::AddRef()
{
	++m_RefCounter;
}

void V3DDescriptorSetLayout::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DDescriptorSetLayout);
	}
}

/***********************/
/* private - V3DDescriptorSetLayout */
/***********************/

V3DDescriptorSetLayout::V3DDescriptorSetLayout() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Statistics({}),
	m_Source({})
{
	m_Source.descriptorSetLayout = VK_NULL_HANDLE;
}

V3DDescriptorSetLayout::~V3DDescriptorSetLayout()
{
	if (m_Handles.empty() == false)
	{
		VkDevice vkDevice = m_pDevice->GetSource().device;

		STLVector<V3DDescriptorSetLayout::HandleT*>::iterator it_begin = m_Handles.begin();
		STLVector<V3DDescriptorSetLayout::HandleT*>::iterator it_end = m_Handles.end();
		STLVector<V3DDescriptorSetLayout::HandleT*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3DDescriptorSetLayout::HandleT* pHandle = (*it);
			vkDestroyDescriptorPool(vkDevice, pHandle->pool, nullptr);
			V3D_DELETE_T(pHandle, HandleT);
		}
	}

	if (m_Source.descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetSource().device, m_Source.descriptorSetLayout, nullptr);
	}

	V3D_RELEASE(m_pDevice);
}

V3D_RESULT V3DDescriptorSetLayout::AddHandle(V3DDescriptorSetLayout::HandleT** ppHandle)
{
	VkDescriptorPool vkDescriptorPool;

	VkResult vkResult = vkCreateDescriptorPool(m_pDevice->GetSource().device, &m_Source.poolCreateInfo, nullptr, &vkDescriptorPool);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3DDescriptorSetLayout::HandleT* pHandle = V3D_NEW_T(V3DDescriptorSetLayout::HandleT);
	if (pHandle == nullptr)
	{
		vkDestroyDescriptorPool(m_pDevice->GetSource().device, vkDescriptorPool, nullptr);
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

#ifdef _DEBUG
	pHandle->pDebugPtr = this;
#endif //_DEBUG
	pHandle->pool = vkDescriptorPool;
	pHandle->maxCount = m_Source.poolCreateInfo.maxSets;
	pHandle->count = 0;

	m_Handles.push_back(pHandle);
	m_Statistics.setCapacity += pHandle->maxCount;
	m_Statistics.poolCount++;

	(*ppHandle) = pHandle;

	return V3D_OK;
}
