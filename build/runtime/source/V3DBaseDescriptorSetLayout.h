#pragma once

class V3DDevice;

class V3DBaseDescriptorSetLayout : public IV3DDescriptorSetLayout
{
public:
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
		STLVector<V3DBaseDescriptorSetLayout::Descriptor> descriptors;

		uint32_t bufferCount;
		uint32_t bufferViewCount;
		uint32_t imageViewCount;
	};

	V3D_RESULT Initialize(V3DDevice* pDevice, uint32_t descriptorCountconst, const V3DDescriptorDesc* descriptors, bool pushable, const wchar_t* pDebugName);
	const V3DBaseDescriptorSetLayout::Source& GetSource() const;
	const V3DBaseDescriptorSetLayout::Descriptor* GetDescriptor(uint32_t binding, V3DFlags resourceFlags) const;

	/*******************/
	/* IV3DDeviceChild */
	/*******************/
	void GetDevice(IV3DDevice** ppDevice) override;

	/**************/
	/* IV3DObject */
	/**************/
	int64_t GetRefCount() const override;
	void AddRef() override;
	void Release() override;

protected:
	V3DDevice* m_pDevice;
	V3DBaseDescriptorSetLayout::Source m_Source;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DBaseDescriptorSetLayout();
	virtual ~V3DBaseDescriptorSetLayout();

private:
	static constexpr V3DBaseDescriptorSetLayout::Descriptor DescriptorInit = { ~0U, VK_DESCRIPTOR_TYPE_SAMPLER, 0, ~0U, ~0U };

	ReferenceCounter m_RefCounter;
};
