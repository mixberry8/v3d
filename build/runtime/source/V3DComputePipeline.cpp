#include "V3DComputePipeline.h"
#include "V3DDevice.h"
#include "V3DPipelineLayout.h"
#include "V3DShaderModule.h"

/*******************************/
/* public - V3DComputePipeline */
/*******************************/

V3DComputePipeline* V3DComputePipeline::Create()
{
	return V3D_NEW_T(V3DComputePipeline);
}

V3D_RESULT V3DComputePipeline::Initialize(IV3DDevice* pDevice, IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pipelineDesc.computeShader.pModule != nullptr);
	V3D_ASSERT(pipelineDesc.computeShader.pEntryPointName != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pPipelineLayout = V3D_TO_ADD_REF(static_cast<V3DPipelineLayout*>(pPipelineLayout));
	m_pShaderModule = V3D_TO_ADD_REF(static_cast<V3DShaderModule*>(pipelineDesc.computeShader.pModule));

	VkComputePipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage.pNext = nullptr;
	createInfo.stage.flags = 0;
	createInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	createInfo.stage.module = m_pShaderModule->GetSource().shaderModule;
	createInfo.stage.pName = pipelineDesc.computeShader.pEntryPointName;
	createInfo.stage.pSpecializationInfo = nullptr;
	createInfo.layout = m_pPipelineLayout->GetSource().pipelineLayout;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = 0;

	VkResult vkResult = vkCreateComputePipelines(m_pDevice->GetSource().device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_Source.pipeline);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipeline, pDebugName);

	return V3D_OK;
}

/**************************************/
/* public override - IV3DPipelineBase */
/**************************************/

const IV3DPipelineBase::Source& V3DComputePipeline::GetSource() const
{
	return m_Source;
}

/**********************************/
/* public override - IV3DPipeline */
/**********************************/

V3D_PIPELINE_TYPE V3DComputePipeline::GetType() const
{
	return V3D_PIPELINE_TYPE_COMPUTE;
}

void V3DComputePipeline::GetLayout(IV3DPipelineLayout** ppPipelineLayout)
{
	(*ppPipelineLayout) = V3D_TO_ADD_REF(m_pPipelineLayout);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DComputePipeline::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DComputePipeline::GetRefCount() const
{
	return m_RefCounter;
}

void V3DComputePipeline::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DComputePipeline::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DComputePipeline);
	}
}

/***************************/
/* private - V3DComputePipeline */
/***************************/

V3DComputePipeline::V3DComputePipeline() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pPipelineLayout(nullptr),
	m_pShaderModule(nullptr),
	m_Source({})
{
	m_Source.pipeline = VK_NULL_HANDLE;
	m_Source.pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
}

V3DComputePipeline::~V3DComputePipeline()
{
	if (m_Source.pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_pDevice->GetSource().device, m_Source.pipeline, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipeline);
	}

	V3D_RELEASE(m_pShaderModule);
	V3D_RELEASE(m_pPipelineLayout);
	V3D_RELEASE(m_pDevice);
}
