#pragma once

#include "CriticalSection.h"

class V3DDevice;

class V3DDescriptorSetLayout : public IV3DDescriptorSetLayout
{
public:
	struct HandleT;
	typedef HandleT* Handle;

	enum RESOURCE_FLAG : V3DFlags
	{
		RESOURCE_IMAGE = 0x00000001,
		RESOURCE_SAMPLER = 0x00000002,
		RESOURCE_BUFFER = 0x00000004,
		RESOURCE_BUFFER_VIEW = 0x00000008,

		RESOURCE_IMAGE_SAMPLER = RESOURCE_IMAGE | RESOURCE_SAMPLER,
	};

	struct Descriptor
	{
		uint32_t binding;
		VkDescriptorType type;
		V3DFlags resourceFlags;
		uint32_t resource;
		uint32_t write;
	};

	struct Source
	{
		VkDescriptorSetLayout descriptorSetLayout;

		STLVector<uint32_t> references;
		STLVector<V3DDescriptorSetLayout::Descriptor> descriptors;

		uint32_t bufferCount;
		uint32_t bufferViewCount;
		uint32_t imageViewCount;

		STLVector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolCreateInfo poolCreateInfo;
	};

	static V3DDescriptorSetLayout* Create();
	V3D_RESULT Initialize(V3DDevice* pDevice, uint32_t descriptorCountconst, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep, const wchar_t* pDebugName);
	const V3DDescriptorSetLayout::Source& GetSource() const;
	const V3DDescriptorSetLayout::Descriptor* GetDescriptor(uint32_t binding, V3DFlags resourceFlags) const;

	// V3DDescriptorSet Ç©ÇÁåƒÇ—èoÇ≥ÇÍÇÈÇ‡ÇÃ
	// [out] pHandle
	// [out] pDescriptorSet
	V3D_RESULT Vulkan_CreateDescriptorSet(V3DDescriptorSetLayout::Handle* pHandle, VkDescriptorSet* pDescriptorSet);
	// [in] handle
	// [in] descriptorSet
	void Vulkan_DestroyDescriptorSet(V3DDescriptorSetLayout::Handle handle, VkDescriptorSet descriptorSet);

	/***************************/
	/* IV3DDescriptorSetLayout */
	/***************************/
	virtual const V3DDescriptorSetLayoutStatistics& GetStatistics() const override;

	/*******************/
	/* IV3DDeviceChild */
	/*******************/
	virtual void GetDevice(IV3DDevice** ppDevice) override;

	/**************/
	/* IV3DObject */
	/**************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	static constexpr V3DDescriptorSetLayout::Descriptor DescriptorInit = { ~0U, VK_DESCRIPTOR_TYPE_SAMPLER, 0, ~0U, ~0U };
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

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DDescriptorSetLayoutStatistics m_Statistics;
	V3DDescriptorSetLayout::Source m_Source;
	STLVector<V3DDescriptorSetLayout::HandleT*> m_Handles;
	CriticalSection m_CriticalSection;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DDescriptorSetLayout();
	virtual ~V3DDescriptorSetLayout();

	V3D_RESULT AddHandle(V3DDescriptorSetLayout::HandleT** ppHandle);

	V3D_DECLARE_ALLOCATOR
};
