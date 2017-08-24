#include "V3DBuffer.h"
#include "V3DDevice.h"
#include "V3DResourceMemory.h"

/**********************/
/* public - V3DBuffer */
/**********************/

V3DBuffer* V3DBuffer::Create()
{
	return V3D_NEW_T(V3DBuffer);
}

V3D_RESULT V3DBuffer::Initialize(IV3DDevice* pDevice, const V3DBufferDesc& desc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(desc.usageFlags != 0);
	V3D_ASSERT(desc.size != 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	V3D_DEBUG_CODE(m_DebugName = V3D_SAFE_NAME(this, pDebugName));

	V3D_ADD_DEBUG_MEMORY_OBJECT(this, V3D_DEBUG_OBJECT_TYPE_BUFFER, m_DebugName.c_str());

	// ----------------------------------------------------------------------------------------------------
	// バッファを作成
	// ----------------------------------------------------------------------------------------------------

	const V3DDevice::Source& deviceSource = m_pDevice->GetSource();

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = desc.size;
	bufferCreateInfo.usage = ToVkBufferUsageFlags(desc.usageFlags);
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult vkResult = vkCreateBuffer(deviceSource.device, &bufferCreateInfo, nullptr, &m_Source.buffer);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.buffer, m_DebugName.c_str());
	V3D_SET_DEBUG_MARKER_OBJECT_NAME(m_pDevice, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, m_Source.buffer, V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// 記述を設定
	// ----------------------------------------------------------------------------------------------------

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(deviceSource.device, m_Source.buffer, &memReqs);

	m_ResourceDesc.memoryTypeBits = memReqs.memoryTypeBits;
	m_ResourceDesc.memorySize = memReqs.size;
	m_ResourceDesc.memoryAlignment = memReqs.alignment;

	m_Desc = desc;

	m_Source.memorySize = desc.size;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DBuffer::Source& V3DBuffer::GetSource() const
{
	return m_Source;
}

bool V3DBuffer::CheckBindMemory() const
{
	return (m_pMemory != nullptr);
}

V3D_RESULT V3DBuffer::BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset)
{
	V3D_ASSERT(pMemory != nullptr);

	m_pMemory = V3D_TO_ADD_REF(pMemory);
	m_Source.memoryOffset = memoryOffset;

#ifdef V3D_DEBUG
	if (m_pMemory->Debug_CheckMemory(memoryOffset, m_ResourceDesc.memorySize) == false)
	{
		return V3D_ERROR_FAIL;
	}
#endif //V3D_DEBUG

	VkResult vkResult = vkBindBufferMemory(m_pDevice->GetSource().device, m_Source.buffer, m_pMemory->GetSource().deviceMemory, memoryOffset);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	return V3D_OK;
}

#ifdef V3D_DEBUG

const wchar_t* V3DBuffer::GetDebugName() const
{
	return m_DebugName.c_str();
}

#endif //V3D_DEBUG

/********************************/
/* public override - IV3DBuffer */
/********************************/

const V3DBufferDesc& V3DBuffer::GetDesc() const
{
	return m_Desc;
}

/**********************************/
/* public override - IV3DResource */
/**********************************/

const V3DResourceDesc& V3DBuffer::GetResourceDesc() const
{
	return m_ResourceDesc;
}

V3D_RESULT V3DBuffer::GetResourceMemory(IV3DResourceMemory** ppMemory)
{
	(*ppMemory) = V3D_TO_ADD_REF(m_pMemory);

	return V3D_OK;
}

V3D_RESULT V3DBuffer::Map(uint64_t offset, uint64_t size, void** ppMemory)
{
	if ((m_ResourceDesc.memorySize <= offset) || ((size != 0) && (m_ResourceDesc.memorySize < (offset + size))) || (ppMemory == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	if (m_pMemory == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	return m_pMemory->Map(m_Source.memoryOffset + offset, (size == 0)? (m_ResourceDesc.memorySize - offset) : size, ppMemory);
}

V3D_RESULT V3DBuffer::Unmap()
{
	return m_pMemory->Unmap();
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DBuffer::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DBuffer::GetRefCount() const
{
	return m_RefCounter;
}

void V3DBuffer::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DBuffer::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DBuffer);
	}
}

/***********************/
/* private - V3DBuffer */
/***********************/

V3DBuffer::V3DBuffer() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pMemory(nullptr),
	m_ResourceDesc({}),
	m_Desc({}),
	m_Source({})
{
	m_ResourceDesc.type = V3D_RESOURCE_TYPE_BUFFER;
	m_Source.buffer = VK_NULL_HANDLE;
}

V3DBuffer::~V3DBuffer()
{
	if (m_Source.buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(m_pDevice->GetSource().device, m_Source.buffer, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.buffer);
	}

	V3D_RELEASE(m_pMemory);

	V3D_REMOVE_DEBUG_MEMORY_OBJECT(this);

	V3D_RELEASE(m_pDevice);
}
