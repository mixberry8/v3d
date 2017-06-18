#include "V3DBaseDescriptorSetLayout.h"
#include "V3DDevice.h"

/***************************************/
/* public - V3DBaseDescriptorSetLayout */
/***************************************/

V3D_RESULT V3DBaseDescriptorSetLayout::Initialize(V3DDevice* pDevice, uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, bool pushable, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(descriptorCount > 0);
	V3D_ASSERT(descriptors != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	V3D_DEBUG_CODE(m_DebugName = V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトの作成に必要な情報を収集
	// ----------------------------------------------------------------------------------------------------

	uint32_t maxBinding = 0;

	for (uint32_t i = 0; i < descriptorCount; i++)
	{
		maxBinding = V3D_MAX(maxBinding, descriptors[i].binding);
	}

	m_Source.references.resize(maxBinding + 1, ~0U);

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトの作成情報を作成
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkDescriptorSetLayoutBinding> vkDescriptorSetLayoutBindings;
	vkDescriptorSetLayoutBindings.resize(descriptorCount);

	m_Source.descriptors.resize(descriptorCount, V3DBaseDescriptorSetLayout::DescriptorInit);

	for (uint32_t i = 0; i < descriptorCount; i++)
	{
		const V3DDescriptorDesc& src = descriptors[i];

		VkDescriptorSetLayoutBinding& dst1 = vkDescriptorSetLayoutBindings[i];
		dst1.binding = src.binding;
		dst1.descriptorType = ToVkDescriptorType(src.type);
		dst1.descriptorCount = 1;
		dst1.stageFlags = ToVkShaderStageFlags(src.stageFlags);
		dst1.pImmutableSamplers = nullptr;

		V3DBaseDescriptorSetLayout::Descriptor& dst2 = m_Source.descriptors[i];
		dst2.binding = src.binding;
		dst2.type = dst1.descriptorType;
		dst2.write = i;

		switch (dst1.descriptorType)
		{
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			dst2.resourceFlags = V3DBaseDescriptorSetLayout::RESOURCE_BUFFER;
			dst2.resource = m_Source.bufferCount;
			m_Source.bufferCount++;
			break;

		case V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			dst2.resourceFlags = V3DBaseDescriptorSetLayout::RESOURCE_BUFFER_VIEW;
			dst2.resource = m_Source.bufferViewCount;
			m_Source.bufferViewCount++;
			break;

		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:       // image -------
			dst2.resourceFlags = V3DBaseDescriptorSetLayout::RESOURCE_IMAGE;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		case VK_DESCRIPTOR_TYPE_SAMPLER:                // ----  sampler
			dst2.resourceFlags = V3DBaseDescriptorSetLayout::RESOURCE_SAMPLER;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // image sampler
			dst2.resourceFlags = V3DBaseDescriptorSetLayout::RESOURCE_IMAGE | V3DBaseDescriptorSetLayout::RESOURCE_SAMPLER;
			dst2.resource = m_Source.imageViewCount;
			m_Source.imageViewCount++;
			break;
		}

		m_Source.references[src.binding] = i;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo;
	descSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutCreateInfo.pNext = nullptr;
	descSetLayoutCreateInfo.flags = (pushable == true)? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(vkDescriptorSetLayoutBindings.size());
	descSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.data();

	VkResult vkResult = vkCreateDescriptorSetLayout(m_pDevice->GetSource().device, &descSetLayoutCreateInfo, nullptr, &m_Source.descriptorSetLayout);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.descriptorSetLayout, m_DebugName.c_str());

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DBaseDescriptorSetLayout::Source& V3DBaseDescriptorSetLayout::GetSource() const
{
	return m_Source;
}

const V3DBaseDescriptorSetLayout::Descriptor* V3DBaseDescriptorSetLayout::GetDescriptor(uint32_t binding, V3DFlags resourceFlags) const
{
	if ((m_Source.references.size() <= binding) || (m_Source.references[binding] == 0xFFFFFFFF))
	{
		return nullptr;
	}

	const V3DBaseDescriptorSetLayout::Descriptor& descriptor = m_Source.descriptors[m_Source.references[binding]];
	if ((descriptor.resourceFlags & resourceFlags) != resourceFlags)
	{
		return nullptr;
	}

	V3D_ASSERT(descriptor.binding != ~0U);
	V3D_ASSERT(descriptor.resource != ~0U);

	return &descriptor;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DBaseDescriptorSetLayout::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DBaseDescriptorSetLayout::GetRefCount() const
{
	return m_RefCounter;
}

void V3DBaseDescriptorSetLayout::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DBaseDescriptorSetLayout::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DBaseDescriptorSetLayout);
	}
}

/****************************************/
/* private - V3DBaseDescriptorSetLayout */
/****************************************/

V3DBaseDescriptorSetLayout::V3DBaseDescriptorSetLayout() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Source({})
{
	m_Source.descriptorSetLayout = VK_NULL_HANDLE;
}

V3DBaseDescriptorSetLayout::~V3DBaseDescriptorSetLayout()
{
	if (m_Source.descriptorSetLayout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetSource().device, m_Source.descriptorSetLayout, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.descriptorSetLayout);
	}

	V3D_RELEASE(m_pDevice);
}
