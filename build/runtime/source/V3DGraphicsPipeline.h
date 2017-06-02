#pragma once

#include "IV3DPipelineBase.h"

class V3DDevice;
class V3DPipelineLayout;
class V3DRenderPass;
class V3DShaderModule;

class V3DGraphicsPipeline : public IV3DPipelineBase
{
public:
	static V3DGraphicsPipeline* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, const wchar_t* pDebugName);

	/********************/
	/* IV3DPipelineBase */
	/********************/
	virtual const IV3DPipelineBase::Source& GetSource() const override;

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
	struct Native
	{
		VkPipelineRasterizationStateCreateInfo rasterizationState;
		VkPipelineDepthStencilStateCreateInfo depthStencilState;
	};

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DPipelineLayout* m_pPipelineLayout;
	V3DRenderPass* m_pRenderPass;
	STLVector<V3DShaderModule*> m_ShaderModules;
	IV3DPipelineBase::Source m_Source;
	V3DGraphicsPipeline::Native m_Native;

	V3DGraphicsPipeline();
	virtual ~V3DGraphicsPipeline();

	static bool PreShader(
		const V3DPipelineShader& shader,
		uint32_t& shaderCount, uint32_t& specializationCount, uint32_t& constantCount);

	static bool AddShader(
		const V3DPipelineShader& shader,
		VkShaderStageFlagBits shaderStage,
		STLVector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
		STLVector<VkSpecializationInfo>& specializations,
		STLVector<VkSpecializationMapEntry>& specializationMapEntries,
		STLVector<V3DShaderModule*>& shaderModules);

	V3D_DECLARE_ALLOCATOR
};
