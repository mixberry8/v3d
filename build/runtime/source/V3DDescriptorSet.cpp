#include "V3DDescriptorSet.h"
#include "V3DDevice.h"
#include "V3DImageView.h"
#include "V3DSampler.h"
#include "V3DBufferView.h"

/**********************/
/* public - V3DDescriptorSet */
/**********************/

V3DDescriptorSet* V3DDescriptorSet::Create()
{
	return V3D_NEW_T(V3DDescriptorSet);
}

V3D_RESULT V3DDescriptorSet::Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pLayout != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pLayout = V3D_TO_ADD_REF(static_cast<V3DDescriptorSetLayout*>(pLayout));

	// ----------------------------------------------------------------------------------------------------
	// �f�X�N���v�^�Z�b�g���쐬
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = m_pLayout->Vulkan_CreateDescriptorSet(&m_PoolHandle, &m_Source.descriptorSet);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// �f�X�N���v�^�Z�b�g�̏������ݏ����쐬
	// ----------------------------------------------------------------------------------------------------

	const V3DDescriptorSetLayout::Source& descSetLayoutSource = m_pLayout->GetSource();

	m_Source.descriptorCount = static_cast<uint32_t>(descSetLayoutSource.descriptors.size());
	m_Source.pWriteDescriptors = static_cast<VkWriteDescriptorSet*>(V3D_MALLOC(sizeof(VkWriteDescriptorSet) * m_Source.descriptorCount));
	if (m_Source.pWriteDescriptors == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	m_ImageViews.resize(descSetLayoutSource.imageViewCount, nullptr);
	m_Samplers.resize(descSetLayoutSource.imageViewCount, nullptr);
	m_BufferViews.resize(descSetLayoutSource.bufferViewCount, nullptr);

	m_Source.descriptorImageInfos.resize(descSetLayoutSource.imageInfoCount, VkDescriptorImageInfo{});
	m_Source.descriptorBufferInfos.resize(descSetLayoutSource.bufferInfoCount, VkDescriptorBufferInfo{});

	const V3DDescriptorSetLayout::Descriptor* pSrcDescriptor = descSetLayoutSource.descriptors.data();
	const V3DDescriptorSetLayout::Descriptor* pSrcDescriptorEnd = pSrcDescriptor + m_Source.descriptorCount;

	VkWriteDescriptorSet* pDstDescriptor = &m_Source.pWriteDescriptors[0];
	VkDescriptorImageInfo* pDstImageInfo = m_Source.descriptorImageInfos.data();
	VkDescriptorBufferInfo* pDstBufferInfo = m_Source.descriptorBufferInfos.data();

	while (pSrcDescriptor != pSrcDescriptorEnd)
	{
		pDstDescriptor->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		pDstDescriptor->pNext = nullptr;
		pDstDescriptor->dstSet = m_Source.descriptorSet;
		pDstDescriptor->dstBinding = pSrcDescriptor->binding;
		pDstDescriptor->dstArrayElement = 0;
		pDstDescriptor->descriptorCount = 1;
		pDstDescriptor->descriptorType = pSrcDescriptor->type;
		pDstDescriptor->pImageInfo = nullptr;
		pDstDescriptor->pBufferInfo = nullptr;
		pDstDescriptor->pTexelBufferView = nullptr;

		if ((pSrcDescriptor->resourceFlags & V3DDescriptorSetLayout::RESOURCE_IMAGE) ||
			(pSrcDescriptor->resourceFlags & V3DDescriptorSetLayout::RESOURCE_SAMPLER))
		{
			pDstDescriptor->pImageInfo = pDstImageInfo++;
		}
		else if (pSrcDescriptor->resourceFlags & V3DDescriptorSetLayout::RESOURCE_BUFFER)
		{
			pDstDescriptor->pBufferInfo = pDstBufferInfo++;
		}
		else if (pSrcDescriptor->resourceFlags & V3DDescriptorSetLayout::RESOURCE_TEXEL_BUFFER_VIEW)
		{
			// �e�N�Z���o�b�t�@�[�r���[�� VkWriteDescriptorSet �ɒ��ڐݒ肷��
			;
		}

		pDstDescriptor++;
		pSrcDescriptor++;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DDescriptorSet::Source& V3DDescriptorSet::GetSource() const
{
	return m_Source;
}

/***************************************/
/* public override - IV3DDescriptorSet */
/***************************************/

void V3DDescriptorSet::GetLayout(IV3DDescriptorSetLayout** ppLayout)
{
	(*ppLayout) = m_pLayout;
}

V3D_RESULT V3DDescriptorSet::GetImageView(uint32_t binding, IV3DImageView** ppImageView)
{
	if (ppImageView == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_IMAGE);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	IV3DImageView* pImageView = m_ImageViews[pDescriptor->resource];
	if (pImageView == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	(*ppImageView) = V3D_TO_ADD_REF(pImageView);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout)
{
	if (pImageLayout == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_IMAGE);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	*pImageLayout = ToV3DImageLayout(m_Source.descriptorImageInfos[pDescriptor->info].imageLayout);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::SetImageView(uint32_t binding, IV3DImageView* pImageView)
{
	if (pImageView == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_IMAGE);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	V3DImageView* pInternalImageView = static_cast<V3DImageView*>(pImageView);
	const V3DImageView::Source& imageViewSource = pInternalImageView->GetSource();

	m_Source.descriptorImageInfos[pDescriptor->info].imageView = imageViewSource.imageView;

	V3D_RELEASE(m_ImageViews[pDescriptor->resource]);
	m_ImageViews[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalImageView);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout)
{
	if (pImageView == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_IMAGE);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	V3DImageView* pInternalImageView = static_cast<V3DImageView*>(pImageView);
	const V3DImageView::Source& imageViewSource = pInternalImageView->GetSource();

	VkDescriptorImageInfo& vkImageInfo = m_Source.descriptorImageInfos[pDescriptor->info];
	vkImageInfo.imageView = imageViewSource.imageView;
	vkImageInfo.imageLayout = ToVkImageLayout(imageLayout);

	V3D_RELEASE(m_ImageViews[pDescriptor->resource]);
	m_ImageViews[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalImageView);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::GetSampler(uint32_t binding, IV3DSampler** ppSampler)
{
	if (ppSampler == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_SAMPLER);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	IV3DSampler* pSampler = m_Samplers[pDescriptor->resource];
	if (pSampler == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	(*ppSampler) = V3D_TO_ADD_REF(pSampler);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::SetSampler(uint32_t binding, IV3DSampler* pSampler)
{
	if (pSampler == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_SAMPLER);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	V3DSampler* pInternalSampler = static_cast<V3DSampler*>(pSampler);

	const V3DSampler::Source& samplerSource = pInternalSampler->GetSource();
	m_Source.descriptorImageInfos[pDescriptor->info].sampler = samplerSource.sampler;

	V3D_RELEASE(m_Samplers[pDescriptor->resource]);
	m_Samplers[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalSampler);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler)
{
	if (pSampler == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_IMAGE_SAMPLER);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	V3DImageView* pInternalImageView = static_cast<V3DImageView*>(pImageView);
	const V3DImageView::Source& imageViewSource = pInternalImageView->GetSource();

	V3DSampler* pInternalSampler = static_cast<V3DSampler*>(pSampler);
	const V3DSampler::Source& samplerSource = pInternalSampler->GetSource();

	VkDescriptorImageInfo& vkImageInfo = m_Source.descriptorImageInfos[pDescriptor->info];
	vkImageInfo.sampler = samplerSource.sampler;
	vkImageInfo.imageView = imageViewSource.imageView;
	vkImageInfo.imageLayout = ToVkImageLayout(imageLayout);

	V3D_RELEASE(m_ImageViews[pDescriptor->resource]);
	m_ImageViews[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalImageView);

	V3D_RELEASE(m_Samplers[pDescriptor->resource]);
	m_Samplers[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalSampler);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView)
{
	if (ppBufferView == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_BUFFER);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	IV3DBufferView* pBufferView = m_BufferViews[pDescriptor->resource];
	if (pBufferView == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	(*ppBufferView) = V3D_TO_ADD_REF(pBufferView);

	return V3D_OK;
}

V3D_RESULT V3DDescriptorSet::SetBufferView(uint32_t binding, IV3DBufferView* pBufferView)
{
	if (pBufferView == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DDescriptorSetLayout::Descriptor* pDescriptor = m_pLayout->GetDescriptor(binding, V3DDescriptorSetLayout::RESOURCE_BUFFER);
	if (pDescriptor == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	V3DBufferView* pInternalBufferView = static_cast<V3DBufferView*>(pBufferView);
	const V3DBufferView::Source& bufferViewSource = pInternalBufferView->GetSource();

	switch (pDescriptor->type)
	{
	case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
		if (bufferViewSource.bufferView != VK_NULL_HANDLE)
		{
			m_Source.pWriteDescriptors[pDescriptor->write].pTexelBufferView = &bufferViewSource.bufferView;
		}
		else
		{
			return V3D_ERROR_FAIL;
		}
		break;

	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
	case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
	case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
		if (bufferViewSource.bufferView == VK_NULL_HANDLE)
		{
			VkDescriptorBufferInfo* pInfo = &m_Source.descriptorBufferInfos[pDescriptor->info];
			pInfo->buffer = bufferViewSource.buffer;
			pInfo->offset = bufferViewSource.offset;
			pInfo->range = bufferViewSource.size;
		}
		else
		{
			return V3D_ERROR_FAIL;
		}
		break;
	}

	V3D_RELEASE(m_BufferViews[pDescriptor->resource]);
	m_BufferViews[pDescriptor->resource] = V3D_TO_ADD_REF(pInternalBufferView);

	return V3D_OK;
}

void V3DDescriptorSet::Update()
{
#ifdef _DEBUG
	uint32_t debugCount = 0;

	for (uint32_t i = 0; i < m_Source.descriptorCount; i++)
	{
		const VkWriteDescriptorSet& writeDescSet = m_Source.pWriteDescriptors[i];

		switch (writeDescSet.descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:          // image -------
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:       // image -------
			if (writeDescSet.pImageInfo->imageView == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, writeDescSet.dstBinding, L"Image");
				debugCount++;
			}
			break;
		case VK_DESCRIPTOR_TYPE_SAMPLER:                // ----  sampler
			if (writeDescSet.pImageInfo->sampler == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, writeDescSet.dstBinding, L"Sampler");
				debugCount++;
			}
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // image sampler
			if ((writeDescSet.pImageInfo->sampler == VK_NULL_HANDLE) || (writeDescSet.pImageInfo->imageView == VK_NULL_HANDLE))
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, writeDescSet.dstBinding, L"Sampler and Image");
				debugCount++;
			}
			break;

		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			if (writeDescSet.pBufferInfo->buffer == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, writeDescSet.dstBinding, L"BufferView");
				debugCount++;
			}
			break;

		case V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			if (writeDescSet.pTexelBufferView == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, writeDescSet.dstBinding, L"TexelBufferView");
				debugCount++;
			}
			break;
		}
	}

	if (debugCount > 0)
	{
		return;
	}
#endif //_DEBUG

	vkUpdateDescriptorSets(m_pDevice->GetSource().device, m_Source.descriptorCount, m_Source.pWriteDescriptors, 0, nullptr);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DDescriptorSet::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DDescriptorSet::GetRefCount() const
{
	return m_RefCounter;
}

void V3DDescriptorSet::AddRef()
{
	++m_RefCounter;
}

void V3DDescriptorSet::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DDescriptorSet);
	}
}

/***********************/
/* private - V3DDescriptorSet */
/***********************/

V3DDescriptorSet::V3DDescriptorSet() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pLayout(nullptr),
	m_PoolHandle(nullptr),
	m_Source({})
{
	m_Source.descriptorSet = VK_NULL_HANDLE;
}

V3DDescriptorSet::~V3DDescriptorSet()
{
	if (m_Source.pWriteDescriptors != nullptr)
	{
		V3D_FREE(m_Source.pWriteDescriptors);
	}

	if (m_ImageViews.empty() == false)
	{
		STLVector<V3DImageView*>::iterator it_begin = m_ImageViews.begin();
		STLVector<V3DImageView*>::iterator it_end = m_ImageViews.end();
		STLVector<V3DImageView*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3DImageView* pImageView = (*it);
			V3D_RELEASE(pImageView);
		}
	}

	if (m_Samplers.empty() == false)
	{
		STLVector<V3DSampler*>::iterator it_begin = m_Samplers.begin();
		STLVector<V3DSampler*>::iterator it_end = m_Samplers.end();
		STLVector<V3DSampler*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3DSampler* pSampler = (*it);
			V3D_RELEASE(pSampler);
		}
	}

	if (m_BufferViews.empty() == false)
	{
		STLVector<V3DBufferView*>::iterator it_begin = m_BufferViews.begin();
		STLVector<V3DBufferView*>::iterator it_end = m_BufferViews.end();
		STLVector<V3DBufferView*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3DBufferView* pBufferView = (*it);
			V3D_RELEASE(pBufferView);
		}
	}

	if (m_PoolHandle != nullptr)
	{
		m_pLayout->Vulkan_DestroyDescriptorSet(m_PoolHandle, m_Source.descriptorSet);
	}

	V3D_RELEASE(m_pLayout);
	V3D_RELEASE(m_pDevice);
}