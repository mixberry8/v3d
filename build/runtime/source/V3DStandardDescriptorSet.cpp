#include "V3DStandardDescriptorSet.h"
#include "V3DDevice.h"

/*************************************/
/* public - V3DStandardDescriptorSet */
/*************************************/

V3DStandardDescriptorSet* V3DStandardDescriptorSet::Create()
{
	return V3D_NEW_T(V3DStandardDescriptorSet);
}

V3D_RESULT V3DStandardDescriptorSet::Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout, const wchar_t* pDebugName)
{
	// ----------------------------------------------------------------------------------------------------
	// ベースを初期化
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = V3DBaseDescriptorSet::Initialize(pDevice, pLayout, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	result = static_cast<V3DStandardDescriptorSetLayout*>(m_pLayout)->Vulkan_CreateDescriptorSet(&m_PoolHandle, &m_Source.descriptorSet);
	if (result != V3D_OK)
	{
		return result;
	}

	VkWriteDescriptorSet* pWrite = m_Source.pWriteDescriptors;
	VkWriteDescriptorSet* pWriteEnd = pWrite + m_Source.descriptorCount;

	while (pWrite != pWriteEnd)
	{
		(pWrite++)->dstSet = m_Source.descriptorSet;
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.descriptorSet, m_DebugName.c_str());

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

/***************************************/
/* public override - IV3DDescriptorSet */
/***************************************/

V3D_DESCRIPTOR_SET_TYPE V3DStandardDescriptorSet::GetType() const
{
	return V3D_DESCRIPTOR_SET_TYPE_STANDARD;
}

void V3DStandardDescriptorSet::Update()
{
#ifdef _DEBUG
	uint32_t debugErrorCount = 0;

	for (uint32_t i = 0; i < m_Source.descriptorCount; i++)
	{
		const VkWriteDescriptorSet& writeDescSet = m_Source.pWriteDescriptors[i];

		switch (writeDescSet.descriptorType)
		{
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		case V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
		case V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			if (writeDescSet.pBufferInfo->buffer == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, m_DebugName.c_str(), writeDescSet.dstBinding, L"Buffer");
				debugErrorCount++;
			}
			break;

		case V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
		case V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			if (writeDescSet.pTexelBufferView == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, m_DebugName.c_str(), writeDescSet.dstBinding, L"BufferView");
				debugErrorCount++;
			}
			break;

		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:    // image -------
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:    // image -------
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: // image -------
			if (writeDescSet.pImageInfo->imageView == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, m_DebugName.c_str(), writeDescSet.dstBinding, L"ImageView");
				debugErrorCount++;
			}
			break;
		case VK_DESCRIPTOR_TYPE_SAMPLER: // ----  sampler
			if (writeDescSet.pImageInfo->sampler == VK_NULL_HANDLE)
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, m_DebugName.c_str(), writeDescSet.dstBinding, L"Sampler");
				debugErrorCount++;
			}
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: // image sampler
			if ((writeDescSet.pImageInfo->sampler == VK_NULL_HANDLE) || (writeDescSet.pImageInfo->imageView == VK_NULL_HANDLE))
			{
				V3D_LOG_ERROR(Log_Error_DescriptorNotSet, m_DebugName.c_str(), writeDescSet.dstBinding, L"ImageView And Sampler");
				debugErrorCount++;
			}
			break;
		}
	}

	if (debugErrorCount > 0)
	{
		return;
	}
#endif //_DEBUG

	vkUpdateDescriptorSets(m_pDevice->GetSource().device, m_Source.descriptorCount, m_Source.pWriteDescriptors, 0, nullptr);
}

/******************************/
/* private - V3DStandardDescriptorSet */
/******************************/

V3DStandardDescriptorSet::V3DStandardDescriptorSet() :
	m_PoolHandle(nullptr)
{
}

V3DStandardDescriptorSet::~V3DStandardDescriptorSet()
{
	if (m_PoolHandle != nullptr)
	{
		static_cast<V3DStandardDescriptorSetLayout*>(m_pLayout)->Vulkan_DestroyDescriptorSet(m_PoolHandle, m_Source.descriptorSet);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.descriptorSet);
	}
}
