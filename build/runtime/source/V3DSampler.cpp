#include "V3DSampler.h"
#include "V3DDevice.h"
#include "V3DAdapter.h"

/**********************/
/* public - V3DSampler */
/**********************/

V3DSampler* V3DSampler::Create()
{
	return V3D_NEW_T(V3DSampler);
}

V3D_RESULT V3DSampler::Initialize(IV3DDevice* pDevice, const V3DSamplerDesc& desc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_Desc = desc;

	// ----------------------------------------------------------------------------------------------------

	VkSamplerCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.magFilter = ToVkFilter(desc.magFilter);
	createInfo.minFilter = ToVkFilter(desc.minFilter);
	createInfo.mipmapMode = ToVkSamplerMipmapMode(desc.mipmapMode);
	createInfo.addressModeU = ToVkSamplerAddressMode(desc.addressModeU);
	createInfo.addressModeV = ToVkSamplerAddressMode(desc.addressModeV);
	createInfo.addressModeW = ToVkSamplerAddressMode(desc.addressModeW);
	createInfo.mipLodBias = desc.mipLodBias;
	createInfo.anisotropyEnable = ToVkBool32(desc.anisotropyEnable);
	createInfo.maxAnisotropy = desc.maxAnisotropy;
	createInfo.compareEnable = desc.compareEnable;
	createInfo.compareOp = ToVkCompareOp(desc.compareOp);
	createInfo.minLod = desc.minLod;
	createInfo.maxLod = desc.maxLod;
	createInfo.borderColor = ToVkBorderColor(desc.borderColor);
	createInfo.unnormalizedCoordinates = VK_FALSE;

	VkResult vkResult = vkCreateSampler(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.sampler);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.sampler, V3D_DEBUG_SAFE_NAME(this, pDebugName));
	V3D_SET_DEBUG_MARKER_OBJECT_NAME(m_pDevice, VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT, m_Source.sampler, V3D_DEBUG_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DSampler::Source& V3DSampler::GetSource() const
{
	return m_Source;
}

/*********************************/
/* public override - IV3DSampler */
/*********************************/

const V3DSamplerDesc& V3DSampler::GetDesc() const
{
	return m_Desc;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DSampler::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DSampler::GetRefCount() const
{
	return m_RefCounter;
}

void V3DSampler::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DSampler::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DSampler);
	}
}

/***********************/
/* private - V3DSampler */
/***********************/

V3DSampler::V3DSampler() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Desc({})
{
	m_Source.sampler = VK_NULL_HANDLE;
}

V3DSampler::~V3DSampler()
{
	if (m_pDevice != nullptr)
	{
		m_pDevice->NotifyReleaseSampler();
	}

	if (m_Source.sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(m_pDevice->GetSource().device, m_Source.sampler, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.sampler);
	}

	V3D_RELEASE(m_pDevice);
}
