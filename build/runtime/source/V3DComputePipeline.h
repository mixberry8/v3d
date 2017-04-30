#pragma once

#include "IV3DPipelineBase.h"

class V3DDevice;
class V3DPipelineLayout;
class V3DShaderModule;

class V3DComputePipeline : public IV3DPipelineBase
{
public:
	static V3DComputePipeline* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc);

	/********************/
	/* IV3DPipelineBase */
	/********************/
	virtual const IV3DPipelineBase::Source& GetSource() const override;
	virtual void AfterBind(VkCommandBuffer commandBuffer) override;

	/****************/
	/* IV3DPipeline */
	/****************/
	virtual V3D_PIPELINE_TYPE GetType() const override;
	virtual void GetLayout(IV3DPipelineLayout** ppPipelineLayout) override;

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
	V3DPipelineLayout* m_pPipelineLayout;
	V3DShaderModule* m_pShaderModule;
	IV3DPipelineBase::Source m_Source;

	V3DComputePipeline();
	virtual ~V3DComputePipeline();

	V3D_DECLARE_ALLOCATOR
};
