#pragma once

#include "CriticalSection.h"
#include "V3DBaseDescriptorSetLayout.h"

class V3DStandardDescriptorSetLayout : public V3DBaseDescriptorSetLayout
{
public:
	struct HandleT;
	typedef HandleT* Handle;

	static V3DStandardDescriptorSetLayout* Create();
	V3D_RESULT Initialize(V3DDevice* pDevice, uint32_t descriptorCountconst, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep, const wchar_t* pDebugName);

	// V3DDescriptorSet Ç©ÇÁåƒÇ—èoÇ≥ÇÍÇÈÇ‡ÇÃ
	// [out] pHandle
	// [out] pDescriptorSet
	V3D_RESULT Vulkan_CreateDescriptorSet(V3DStandardDescriptorSetLayout::Handle* pHandle, VkDescriptorSet* pDescriptorSet);
	// [in] handle
	// [in] descriptorSet
	void Vulkan_DestroyDescriptorSet(V3DStandardDescriptorSetLayout::Handle handle, VkDescriptorSet descriptorSet);

	/***************************/
	/* IV3DDescriptorSetLayout */
	/***************************/
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const override;
	virtual const V3DDescriptorSetLayoutStatistics& GetStatistics() const override;

private:
	static constexpr VkDescriptorPoolSize DescriptorPoolSizeInitList[VK_DESCRIPTOR_TYPE_RANGE_SIZE] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 0 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 0 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 0 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 0 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 0 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0 },
	};

	V3DDescriptorSetLayoutStatistics m_Statistics;
	STLVector<V3DStandardDescriptorSetLayout::HandleT*> m_Handles;

	CriticalSection m_CriticalSection;
	STLVector<VkDescriptorPoolSize> m_PoolSizes;
	VkDescriptorPoolCreateInfo m_PoolCreateInfo;

	V3DStandardDescriptorSetLayout();
	virtual ~V3DStandardDescriptorSetLayout();

	V3D_RESULT AddHandle(V3DStandardDescriptorSetLayout::HandleT** ppHandle);

	V3D_DECLARE_ALLOCATOR
};
