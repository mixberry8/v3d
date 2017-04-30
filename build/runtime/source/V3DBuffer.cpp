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

V3D_RESULT V3DBuffer::Initialize(IV3DDevice* pDevice, uint32_t subresourceCount, const V3DBufferSubresourceDesc* pSubresources)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(subresourceCount > 0);
	V3D_ASSERT(pSubresources != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	// ----------------------------------------------------------------------------------------------------
	// サブリソースのレイアウトを決める
	// ----------------------------------------------------------------------------------------------------

	VkDeviceSize vkSize = 0;
	V3DFlags usageFlags = 0;

	const VkPhysicalDeviceLimits& vkPhysicalDeviceLimits = m_pDevice->GetSource().deviceProps.limits;

	const V3DBufferSubresourceDesc* pSrcSubresource = &pSubresources[0];
	const V3DBufferSubresourceDesc* pSrcSubresourceEnd = pSrcSubresource + subresourceCount;

	m_Subresources.resize(subresourceCount);
	V3DBuffer::Subresource* pDstSubresource = m_Subresources.data();
	uint32_t subresourceIndex = 0;

	while (pSrcSubresource != pSrcSubresourceEnd)
	{
		uint64_t memoryAlignment = 0;

		if (pSrcSubresource->usageFlags & V3DBuffer::StandardBufferUsageMask) { memoryAlignment = V3D_MAX(memoryAlignment, vkPhysicalDeviceLimits.minMemoryMapAlignment); }
		if (pSrcSubresource->usageFlags & V3DBuffer::TexelBufferUsageMask) { memoryAlignment = V3D_MAX(memoryAlignment, vkPhysicalDeviceLimits.minTexelBufferOffsetAlignment); }
		if (pSrcSubresource->usageFlags & V3DBuffer::UniformBufferUsageMask) { memoryAlignment = V3D_MAX(memoryAlignment, vkPhysicalDeviceLimits.minUniformBufferOffsetAlignment); }
		if (pSrcSubresource->usageFlags & V3DBuffer::StorageBufferUsageMask) { memoryAlignment = V3D_MAX(memoryAlignment, vkPhysicalDeviceLimits.minStorageBufferOffsetAlignment); }

		memoryAlignment = V3D_MAX(vkPhysicalDeviceLimits.minMemoryMapAlignment, memoryAlignment);

		V3DFlags subresourceUsageFlags = pSrcSubresource->usageFlags;
		usageFlags |= subresourceUsageFlags & ~usageFlags; // 必要なフラグだけ立てる

		pDstSubresource->index = subresourceIndex++;
		pDstSubresource->memoryAlignment = memoryAlignment;
		pDstSubresource->layout.usageFlags = subresourceUsageFlags;
		pDstSubresource->layout.offset = 0; // ソート後に求める
		pDstSubresource->layout.size = pSrcSubresource->size;

		pDstSubresource++;
		pSrcSubresource++;
	}

	// サブリソースをアラインメントの大きい順にソート
	std::sort(m_Subresources.begin(), m_Subresources.end(), [](const V3DBuffer::Subresource& lh, const V3DBuffer::Subresource& rh) { return lh.memoryAlignment > rh.memoryAlignment; });

	// サブリソースのレイアウトの参照配列を作成
	V3DBuffer::Subresource* pSrcSubresourceIndex = m_Subresources.data();
	V3DBuffer::Subresource* pSrcSubresourceIndexEnd = pSrcSubresourceIndex + m_Subresources.size();

	m_SubresourcesIndices.resize(subresourceCount);
	uint32_t* pDstSubresourceIndex = m_SubresourcesIndices.data();

	while (pSrcSubresourceIndex != pSrcSubresourceIndexEnd)
	{
		*pDstSubresourceIndex++ = (*pSrcSubresourceIndex++).index;
	}

	// サブリソースのメモリオフセットを求める
	V3DBuffer::Subresource* pSubresource = m_Subresources.data();
	V3DBuffer::Subresource* pSubresourceEnd = pSubresource + m_Subresources.size();

	while (pSubresource != pSubresourceEnd)
	{
		uint64_t memoryAlignment = pSubresource->memoryAlignment;

		// メモリのオフセット
		pSubresource->layout.offset = ((vkSize / memoryAlignment) * memoryAlignment) + (((vkSize % memoryAlignment) != 0)? memoryAlignment : 0);
		// 確保するメモリのサイズ
		vkSize = pSubresource->layout.offset + pSubresource->layout.size;

		pSubresource++;
	}

	// ----------------------------------------------------------------------------------------------------
	// バッファを作成
	// ----------------------------------------------------------------------------------------------------

	const V3DDevice::Source& deviceSource = m_pDevice->GetSource();

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = vkSize;
	bufferCreateInfo.usage = ToVkBufferUsageFlags(usageFlags);
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult vkResult = vkCreateBuffer(deviceSource.device, &bufferCreateInfo, nullptr, &m_Source.buffer);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// 記述を設定
	// ----------------------------------------------------------------------------------------------------

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(deviceSource.device, m_Source.buffer, &memReqs);

	m_ResourceDesc.memoryTypeBits = memReqs.memoryTypeBits;
	m_ResourceDesc.memorySize = memReqs.size;
	m_ResourceDesc.memoryAlignment = memReqs.alignment;

	m_UsageFlags = usageFlags;

	m_Source.memorySize = vkSize;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DBuffer::Subresource* V3DBuffer::GetSubresourcesPtr() const
{
	return m_Subresources.data();
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

	VkResult vkResult = vkBindBufferMemory(m_pDevice->GetSource().device, m_Source.buffer, m_pMemory->GetSource().deviceMemory, memoryOffset);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	return V3D_OK;
}

/********************************/
/* public override - IV3DBuffer */
/********************************/

V3DFlags V3DBuffer::GetUsageFlags() const
{
	return m_UsageFlags;
}

uint32_t V3DBuffer::GetSubresourceCount() const
{
	return static_cast<uint32_t>(m_Subresources.size());
}

const V3DBufferSubresourceLayout& V3DBuffer::GetSubresourceLayout(uint32_t layer) const
{
	return m_Subresources[m_SubresourcesIndices[layer]].layout;
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
	if ((m_ResourceDesc.memorySize <= offset) || ((size != V3D_WHOLE_SIZE) && (m_ResourceDesc.memorySize < (offset + size))) || (ppMemory == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	if (m_pMemory == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	return m_pMemory->Map(m_Source.memoryOffset + offset, (size == V3D_WHOLE_SIZE)? m_ResourceDesc.memorySize : size, ppMemory);
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
	++m_RefCounter;
}

void V3DBuffer::Release()
{
	if (--m_RefCounter == 0)
	{
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
	m_UsageFlags(0),
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
	}

	V3D_RELEASE(m_pMemory);
	V3D_RELEASE(m_pDevice);
}
