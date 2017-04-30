#include "V3DBackBuffer.h"
#include "V3DDevice.h"
#include "V3DResourceMemory.h"
#include "V3DAdapter.h"

/************************/
/* public V3DBackBuffer */
/************************/

V3DBackBuffer* V3DBackBuffer::Create()
{
	return V3D_NEW_T(V3DBackBuffer);
}

V3D_RESULT V3DBackBuffer::Initialize(IV3DDevice* pDevice, VkImage image, VkFormat format, uint32_t width, uint32_t height)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(image != VK_NULL_HANDLE);
	V3D_ASSERT(width > 0);
	V3D_ASSERT(height > 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	VkMemoryRequirements vkMemReq;
	vkGetImageMemoryRequirements(m_pDevice->GetSource().device, image, &vkMemReq);

	m_Source.image = image;

	m_Desc.format = ToV3DFormat(format);
	m_Desc.width = width;
	m_Desc.height = height;

	m_ResourceDesc.memoryTypeBits = vkMemReq.memoryTypeBits;
	m_ResourceDesc.memorySize = vkMemReq.size;
	m_ResourceDesc.memoryAlignment = vkMemReq.alignment;

	return V3D_OK;
}

/***********************************/
/* public override - IV3DImageBase */
/***********************************/

const IV3DImageBase::Source& V3DBackBuffer::GetSource() const
{
	return m_Source;
}

bool V3DBackBuffer::CheckBindMemory() const
{
	return true;
}

V3D_RESULT V3DBackBuffer::BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset)
{
	return V3D_ERROR_FAIL;
}

/*******************************/
/* public override - IV3DImage */
/*******************************/

const V3DImageDesc& V3DBackBuffer::GetDesc() const
{
	return m_Desc;
}

const V3DImageSubresourceLayout& V3DBackBuffer::GetSubresourceLayout(uint32_t level, uint32_t layer) const
{
	return V3DBackBuffer::DummySubresource;
}

/**********************************/
/* public override - IV3DResource */
/**********************************/

const V3DResourceDesc& V3DBackBuffer::GetResourceDesc() const
{
	return m_ResourceDesc;
}

V3D_RESULT V3DBackBuffer::GetResourceMemory(IV3DResourceMemory** ppMemory)
{
	return V3D_ERROR_FAIL;
}

V3D_RESULT V3DBackBuffer::Map(uint64_t offset, uint64_t size, void** ppMemory)
{
	return V3D_ERROR_FAIL;
}

V3D_RESULT V3DBackBuffer::Unmap()
{
	return V3D_ERROR_FAIL;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DBackBuffer::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DBackBuffer::GetRefCount() const
{
	return m_RefCounter;
}

void V3DBackBuffer::AddRef()
{
	++m_RefCounter;
}

void V3DBackBuffer::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DBackBuffer);
	}
}

/*************************/
/* private V3DBackBuffer */
/*************************/

V3DBackBuffer::V3DBackBuffer() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_ResourceDesc({}),
	m_Desc({}),
	m_Source({})
{
	m_ResourceDesc.type = V3D_RESOURCE_TYPE_IMAGE;

	m_Desc.type = V3D_IMAGE_TYPE_2D;
	m_Desc.format = V3D_FORMAT_UNDEFINED;
	m_Desc.depth = 1;
	m_Desc.levelCount = 1;
	m_Desc.layerCount = 1;
	m_Desc.samples = V3D_SAMPLE_COUNT_1;
	m_Desc.tiling = V3D_IMAGE_TILING_OPTIMAL;
	m_Desc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_SRC | V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;

	m_Source.image = VK_NULL_HANDLE;
	m_Source.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
}

V3DBackBuffer::~V3DBackBuffer()
{
	//m_Source.image はスワップチェイン側で破棄される

	V3D_RELEASE(m_pDevice);
}
