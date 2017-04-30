#pragma once

class V3DDevice;

class V3DPipelineLayout : public IV3DPipelineLayout
{
public:
	struct Source
	{
		VkPipelineLayout pipelineLayout;
	};

	static V3DPipelineLayout* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, uint32_t constantCount, V3DConstantDesc* pConstants, uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts);
	const V3DPipelineLayout::Source& GetSource() const;

	/**********************/
	/* IV3DPipelineLayout */
	/**********************/
	virtual uint32_t GetConstantCount() const override;
	virtual const V3DConstantDesc& GetConstantDesc(uint32_t constantIndex) const override;
	virtual uint32_t GetDescriptorSetCount() const override;
	virtual void GetDescriptorSetLayout(uint32_t descriptorSetIndex, IV3DDescriptorSetLayout** ppDescriptorSetLayout) override;

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
	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	STLVector<V3DConstantDesc> m_Constants;
	STLVector<IV3DDescriptorSetLayout*> m_DescriptorSetLayouts;
	V3DPipelineLayout::Source m_Source;

	V3DPipelineLayout();
	virtual ~V3DPipelineLayout();

	V3D_DECLARE_ALLOCATOR
};
