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

V3D_RESULT V3DBufferView::Initialize(IV3DDevice* pDevice, IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pBuffer != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pBuffer = V3D_TO_ADD_REF(static_cast<V3DBuffer*>(pBuffer));

	const V3DBuffer::Source& bufferSource = m_pBuffer->GetSource();

	// ----------------------------------------------------------------------------------------------------

	VkBufferViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.buffer = bufferSource.buffer;
	createInfo.format = ToVkFormat(desc.format);
	createInfo.offset = desc.offset;
	createInfo.range = desc.size;

	VkResult vkResult = vkCreateBufferView(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.bufferView);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.bufferView, pDebugName);

	// ----------------------------------------------------------------------------------------------------

	m_Desc = desc;

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
	V3D_REF_INC(m_RefCounter);
}

void V3DBufferView::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
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
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.bufferView);
	}

	V3D_RELEASE(m_pBuffer);
	V3D_RELEASE(m_pDevice);
}
