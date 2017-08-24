#include "V3DEvent.h"
#include "V3DDevice.h"

/*********************/
/* public - V3DEvent */
/*********************/

V3DEvent* V3DEvent::Create()
{
	return V3D_NEW_T(V3DEvent);
}

V3D_RESULT V3DEvent::Initialize(IV3DDevice* pDevice, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	V3D_ADD_DEBUG_MEMORY_OBJECT(this, V3D_DEBUG_OBJECT_TYPE_EVENT, V3D_SAFE_NAME(this, pDebugName));

	VkEventCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkResult vkResult = vkCreateEvent(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.event);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.event, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DEvent::Source& V3DEvent::GetSource() const
{
	return m_Source;
}

/*******************************/
/* public override - IV3DEvent */
/*******************************/

bool V3DEvent::GetState() const
{
	return (vkGetEventStatus(m_pDevice->GetSource().device, m_Source.event) == VK_SUCCESS);
}

V3D_RESULT V3DEvent::Reset()
{
	VkResult vkResult = vkResetEvent(m_pDevice->GetSource().device, m_Source.event);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DEvent::Set()
{
	VkResult vkResult = vkSetEvent(m_pDevice->GetSource().device, m_Source.event);

	return ToV3DResult(vkResult);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DEvent::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DEvent::GetRefCount() const
{
	return m_RefCounter;
}

void V3DEvent::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DEvent::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DEvent);
	}
}

/**********************/
/* private - V3DEvent */
/**********************/

V3DEvent::V3DEvent() :
	m_RefCounter(1),
	m_pDevice(nullptr)
{
	m_Source.event = VK_NULL_HANDLE;
}

V3DEvent::~V3DEvent()
{
	if (m_pDevice != nullptr)
	{
		if (m_Source.event != VK_NULL_HANDLE)
		{
			vkDestroyEvent(m_pDevice->GetSource().device, m_Source.event, nullptr);
			V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.event);
		}
	}

	V3D_REMOVE_DEBUG_MEMORY_OBJECT(this);

	V3D_RELEASE(m_pDevice);
}
