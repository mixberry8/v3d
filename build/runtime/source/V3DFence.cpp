#include "V3DFence.h"
#include "V3DDevice.h"

/*********************/
/* public - V3DFence */
/*********************/

V3DFence* V3DFence::Create()
{
	return V3D_NEW_T(V3DFence);
}

V3D_RESULT V3DFence::Initialize(IV3DDevice* pDevice, bool initialState, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	VkFenceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = (initialState == true)? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VkResult vkResult = vkCreateFence(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.fence);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.fence, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DFence::Source& V3DFence::GetSource() const
{
	return m_Source;
}

/*******************************/
/* public override - IV3DFence */
/*******************************/

bool V3DFence::GetState() const
{
	return (vkGetFenceStatus(m_pDevice->GetSource().device, m_Source.fence) == VK_SUCCESS);
}

V3D_RESULT V3DFence::Reset()
{
	VkResult vkResult = vkResetFences(m_pDevice->GetSource().device, 1, &m_Source.fence);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DFence::Wait(uint64_t timeOut)
{
	VkResult vkResult = vkWaitForFences(m_pDevice->GetSource().device, 1, &m_Source.fence, VK_TRUE, timeOut);

	return ToV3DResult(vkResult);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DFence::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DFence::GetRefCount() const
{
	return m_RefCounter;
}

void V3DFence::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DFence::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DFence);
	}
}

/**********************/
/* private - V3DFence */
/**********************/

V3DFence::V3DFence() :
	m_RefCounter(1),
	m_pDevice(nullptr)
{
	m_Source.fence = VK_NULL_HANDLE;
}

V3DFence::~V3DFence()
{
	if (m_pDevice != nullptr)
	{
		if (m_Source.fence != VK_NULL_HANDLE)
		{
			vkDestroyFence(m_pDevice->GetSource().device, m_Source.fence, nullptr);
			V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.fence);
		}
	}

	V3D_RELEASE(m_pDevice);
}
