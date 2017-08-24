#include "V3DSemaphore.h"
#include "V3DDevice.h"
#include "V3DBuffer.h"

/**************************/
/* public - V3DSemaphore */
/**************************/

V3DSemaphore* V3DSemaphore::Create()
{
	return V3D_NEW_T(V3DSemaphore);
}

V3D_RESULT V3DSemaphore::Initialize(IV3DDevice* pDevice, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	V3D_ADD_DEBUG_MEMORY_OBJECT(this, V3D_DEBUG_OBJECT_TYPE_SEMAPHORE, V3D_SAFE_NAME(this, pDebugName));

	VkSemaphoreCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;

	VkResult vResult = vkCreateSemaphore(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.semaphore);
	if (vResult != VK_SUCCESS)
	{
		return ToV3DResult(vResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.semaphore, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DSemaphore::Source& V3DSemaphore::GetSource() const
{
	return m_Source;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DSemaphore::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DSemaphore::GetRefCount() const
{
	return m_RefCounter;
}

void V3DSemaphore::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DSemaphore::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DSemaphore);
	}
}

/***************************/
/* private - V3DSemaphore */
/***************************/

V3DSemaphore::V3DSemaphore() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Source({})
{
	m_Source.semaphore = VK_NULL_HANDLE;
}

V3DSemaphore::~V3DSemaphore()
{
	if (m_Source.semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(m_pDevice->GetSource().device, m_Source.semaphore, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.semaphore);
	}

	V3D_REMOVE_DEBUG_MEMORY_OBJECT(this);

	V3D_RELEASE(m_pDevice);
}
