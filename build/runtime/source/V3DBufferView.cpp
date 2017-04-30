#include "V3DBufferView.h"
#include "V3DDevice.h"
#include "V3DBuffer.h"

/**************************/
/* public - V3DBufferView */
/**************************/

V3DBufferView* V3DBufferView::Create()
{
	return V3D_NEW_T(V3DBufferView);
}

V3D_RESULT V3DBufferView::Initialize(IV3DDevice* pDevice, IV3DBuffer* pBuffer, uint32_t bufferSubresource, V3D_FORMAT format)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pBuffer != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pBuffer = V3D_TO_ADD_REF(static_cast<V3DBuffer*>(pBuffer));

	const V3DBufferSubresourceLayout& subresourceLayout = m_pBuffer->GetSubresourceLayout(bufferSubresource);
	const V3DBuffer::Source& bufferSource = m_pBuffer->GetSource();

	// ----------------------------------------------------------------------------------------------------

	if (subresourceLayout.usageFlags & (V3D_BUFFER_USAGE_UNIFORM_TEXEL | V3D_BUFFER_USAGE_STORAGE_TEXEL))
	{
		if (format == V3D_FORMAT_UNDEFINED)
		{
			return V3D_ERROR_FAIL;
		}

		VkBufferViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.buffer = bufferSource.buffer;
		createInfo.format = ToVkFormat(format);
		createInfo.offset = subresourceLayout.offset;
		createInfo.range = subresourceLayout.size;

		VkResult vkResult = vkCreateBufferView(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.bufferView);
		if(vkResult != VK_SUCCESS)
		{
			return ToV3DResult(vkResult);
		}
	}
	else if(format != V3D_FORMAT_UNDEFINED)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------

	m_Desc.usageFlags = subresourceLayout.usageFlags;
	m_Desc.format = format;
	m_Desc.offset = subresourceLayout.offset;
	m_Desc.size = subresourceLayout.size;

	m_Source.buffer = bufferSource.buffer;
	m_Source.offset = m_Desc.offset;
	m_Source.size = m_Desc.size;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DBufferView::Source& V3DBufferView::GetSource() const
{
	return m_Source;
}

/************************************/
/* public override - IV3DBufferView */
/************************************/

const V3DBufferViewDesc& V3DBufferView::GetDesc() const
{
	return m_Desc;
}

void V3DBufferView::GetBuffer(IV3DBuffer** ppBuffer)
{
	(*ppBuffer) = V3D_TO_ADD_REF(m_pBuffer);

}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DBufferView::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DBufferView::GetRefCount() const
{
	return m_RefCounter;
}

void V3DBufferView::AddRef()
{
	++m_RefCounter;
}

void V3DBufferView::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DBufferView);
	}
}

/***************************/
/* private - V3DBufferView */
/***************************/

V3DBufferView::V3DBufferView() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pBuffer(nullptr),
	m_Desc({}),
	m_Source({})
{
	m_Source.buffer = VK_NULL_HANDLE;
	m_Source.bufferView = VK_NULL_HANDLE;
}

V3DBufferView::~V3DBufferView()
{
	if (m_Source.bufferView != VK_NULL_HANDLE)
	{
		vkDestroyBufferView(m_pDevice->GetSource().device, m_Source.bufferView, nullptr);
	}

	V3D_RELEASE(m_pBuffer);
	V3D_RELEASE(m_pDevice);
}
