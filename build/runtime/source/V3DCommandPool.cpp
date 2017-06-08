#include "V3DCommandPool.h"
#include "V3DDevice.h"

/********************************/
/* public - V3DCommandPool */
/********************************/

V3DCommandPool* V3DCommandPool::Create()
{
	return V3D_NEW_T(V3DCommandPool);
}

V3D_RESULT V3DCommandPool::Initialize(IV3DDevice* pDevice, const V3DCommandPoolDesc& desc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_Desc = desc;

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.queueFamilyIndex = m_Desc.queueFamily;
	if (m_Desc.propertyFlags & V3D_COMMAND_POOL_PROPERTY_TRANSIENT) { createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; }
	if (m_Desc.propertyFlags & V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER) { createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; }

	VkResult vkResult = vkCreateCommandPool(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.commandPool);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.commandPool, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DCommandPool::Source& V3DCommandPool::GetSource() const
{
	return m_Source;
}

/******************************/
/* override - IV3DCommandPool */
/******************************/

const V3DCommandPoolDesc& V3DCommandPool::GetDesc() const
{
	return m_Desc;
}

V3D_RESULT V3DCommandPool::Reset(V3DFlags resetFlags)
{
	uint32_t vkResetFlags = 0;

	if (resetFlags & V3D_COMMAND_ALLOCATOR_RESET_RELEASE_RESOURCES)
	{
		vkResetFlags |= VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;
	}

	VkResult vkResult = vkResetCommandPool(m_pDevice->GetSource().device, m_Source.commandPool, vkResetFlags);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	return V3D_OK;
}

/******************************/
/* override - IV3DDeviceChild */
/******************************/

void V3DCommandPool::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/*************************/
/* override - IV3DObject */
/*************************/

int64_t V3DCommandPool::GetRefCount() const
{
	return m_RefCounter;
}

void V3DCommandPool::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DCommandPool::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DCommandPool);
	}
}

/*********************************/
/* private - V3DCommandPool */
/*********************************/

V3DCommandPool::V3DCommandPool() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Desc({}),
	m_Source({})
{
	m_Source.commandPool = VK_NULL_HANDLE;
}

V3DCommandPool::~V3DCommandPool()
{
	if (m_pDevice != nullptr)
	{
		vkDestroyCommandPool(m_pDevice->GetSource().device, m_Source.commandPool, nullptr);

		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.commandPool);
	}

	V3D_RELEASE(m_pDevice);
}
