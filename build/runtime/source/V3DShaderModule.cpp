#include "V3DShaderModule.h"
#include "V3DDevice.h"

/****************************/
/* public - V3DShaderModule */
/****************************/

V3DShaderModule* V3DShaderModule::Create()
{
	return V3D_NEW_T(V3DShaderModule);
}

V3D_RESULT V3DShaderModule::Initialize(IV3DDevice* pDevice, size_t codeSize, const void* pCode)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(codeSize > 0);
	V3D_ASSERT((codeSize % sizeof(uint32_t)) == 0);
	V3D_ASSERT(pCode != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = codeSize;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(pCode);

	VkResult vkResult = vkCreateShaderModule(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.shaderModule);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	return V3D_OK;
}

const V3DShaderModule::Source& V3DShaderModule::GetSource() const
{
	return m_Source;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DShaderModule::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DShaderModule::GetRefCount() const
{
	return m_RefCounter;
}

void V3DShaderModule::AddRef()
{
	++m_RefCounter;
}

void V3DShaderModule::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DShaderModule);
	}
}

/*****************************/
/* private - V3DShaderModule */
/*****************************/

V3DShaderModule::V3DShaderModule() :
	m_RefCounter(1),
	m_pDevice(nullptr)
{
	m_Source.shaderModule = VK_NULL_HANDLE;
}

V3DShaderModule::~V3DShaderModule()
{
	if (m_Source.shaderModule != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(m_pDevice->GetSource().device, m_Source.shaderModule, nullptr);
	}

	V3D_RELEASE(m_pDevice);
}