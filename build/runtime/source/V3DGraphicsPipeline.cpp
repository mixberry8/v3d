#include "V3DGraphicsPipeline.h"
#include "V3DDevice.h"
#include "V3DPipelineLayout.h"
#include "V3DRenderPass.h"
#include "V3DShaderModule.h"

/********************************/
/* public - V3DGraphicsPipeline */
/********************************/

V3DGraphicsPipeline* V3DGraphicsPipeline::Create()
{
	return V3D_NEW_T(V3DGraphicsPipeline);
}

V3D_RESULT V3DGraphicsPipeline::Initialize(IV3DDevice* pDevice, IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc)
{
	V3D_ASSERT(pPipelineLayout != nullptr);
	V3D_ASSERT(pipelineDesc.pRenderPass != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pPipelineLayout = V3D_TO_ADD_REF(static_cast<V3DPipelineLayout*>(pPipelineLayout));
	m_pRenderPass = V3D_TO_ADD_REF(static_cast<V3DRenderPass*>(pipelineDesc.pRenderPass));

	const V3DRenderPass::Source& renderPassSource = m_pRenderPass->GetSource();

#ifdef _DEBUG
	if (renderPassSource.debug.subpasses.size() <= pipelineDesc.subpass)
	{
		V3D_LOG_ERROR(Log_Error_InvalidSubpass, pipelineDesc.subpass);
		return V3D_ERROR_FAIL;
	}

	if (renderPassSource.debug.subpasses[pipelineDesc.subpass].colorAttachments.size() != pipelineDesc.colorBlend.attachmentCount)
	{
		V3D_LOG_ERROR(Log_Error_MismatchSubpassColorAttachmentCount);
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// シェーダーステージ
	// ----------------------------------------------------------------------------------------------------

	uint32_t shaderCount = 0;
	uint32_t specializationCount = 0;
	uint32_t constantCount = 0;

	// 用意する構造体の数をカウント

	if (V3DGraphicsPipeline::PreShader(pipelineDesc.vertexShader, shaderCount, specializationCount, constantCount) == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (V3DGraphicsPipeline::PreShader(pipelineDesc.tessellationControlShader, shaderCount, specializationCount, constantCount) == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (V3DGraphicsPipeline::PreShader(pipelineDesc.tessellationEvaluationShader, shaderCount, specializationCount, constantCount) == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (V3DGraphicsPipeline::PreShader(pipelineDesc.geometryShader, shaderCount, specializationCount, constantCount) == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (V3DGraphicsPipeline::PreShader(pipelineDesc.fragmentShader, shaderCount, specializationCount, constantCount) == false)
	{
		return V3D_ERROR_FAIL;
	}

	// リストを作成

	STLVector<VkPipelineShaderStageCreateInfo> vkStages;
	vkStages.reserve(shaderCount);

	STLVector<VkSpecializationInfo> vkSpecializations;
	vkSpecializations.reserve(specializationCount);

	STLVector<VkSpecializationMapEntry> vkSpecializationMapEntries;
	vkSpecializationMapEntries.reserve(constantCount);

	bool tessellationEnable = false;

	// バーテックス
	V3DGraphicsPipeline::AddShader(pipelineDesc.vertexShader, VK_SHADER_STAGE_VERTEX_BIT, vkStages, vkSpecializations, vkSpecializationMapEntries, m_ShaderModules);

	// テッセレーション制御
	if (V3DGraphicsPipeline::AddShader(pipelineDesc.tessellationControlShader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, vkStages, vkSpecializations, vkSpecializationMapEntries, m_ShaderModules) == true)
	{
		tessellationEnable = true;
	}

	// テッセレーション評価
	if (V3DGraphicsPipeline::AddShader(pipelineDesc.tessellationEvaluationShader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, vkStages, vkSpecializations, vkSpecializationMapEntries, m_ShaderModules) == true)
	{
		tessellationEnable = true;
	}

	// ジオメトリ
	V3DGraphicsPipeline::AddShader(pipelineDesc.geometryShader, VK_SHADER_STAGE_GEOMETRY_BIT, vkStages, vkSpecializations, vkSpecializationMapEntries, m_ShaderModules);

	// フラグメント
	V3DGraphicsPipeline::AddShader(pipelineDesc.fragmentShader, VK_SHADER_STAGE_FRAGMENT_BIT, vkStages, vkSpecializations, vkSpecializationMapEntries, m_ShaderModules);

	// ----------------------------------------------------------------------------------------------------
	// バーテックスインプット
	// ----------------------------------------------------------------------------------------------------

	const V3DPipelineVertexLayout* pSrcVertexLayoutBegin = &pipelineDesc.vertexInput.pLayouts[0];
	const V3DPipelineVertexLayout* pSrcVertexLayoutEnd = pSrcVertexLayoutBegin + pipelineDesc.vertexInput.layoutCount;
	const V3DPipelineVertexLayout* pSrcVertexLayout = pSrcVertexLayoutBegin;

	STLVector<VkVertexInputBindingDescription> vkVertexBindings;
	vkVertexBindings.resize(pipelineDesc.vertexInput.layoutCount, VkVertexInputBindingDescription{});

	STLVector<VkVertexInputAttributeDescription> vkVertexAttributes;
	vkVertexAttributes.resize(pipelineDesc.vertexInput.elementCount, VkVertexInputAttributeDescription{});

	VkVertexInputBindingDescription* pDstVertexBinding = vkVertexBindings.data();
	VkVertexInputAttributeDescription* pDstVertexAttribute = vkVertexAttributes.data();

	pSrcVertexLayout = pSrcVertexLayoutBegin;
	while (pSrcVertexLayout != pSrcVertexLayoutEnd)
	{
		if ((pipelineDesc.vertexInput.elementCount <= pSrcVertexLayout->firstElement) ||
			(pipelineDesc.vertexInput.elementCount < (pSrcVertexLayout->firstElement + pSrcVertexLayout->elementCount)))
		{
			return V3D_ERROR_FAIL;
		}

		pDstVertexBinding->binding = pSrcVertexLayout->binding;
		pDstVertexBinding->stride = pSrcVertexLayout->stride;
		pDstVertexBinding->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		const V3DPipelineVertexElement* pSrcVertexElement = &pipelineDesc.vertexInput.pElements[pSrcVertexLayout->firstElement];
		const V3DPipelineVertexElement* pSrcVertexElementEnd = pSrcVertexElement + pSrcVertexLayout->elementCount;

		while (pSrcVertexElement != pSrcVertexElementEnd)
		{
			pDstVertexAttribute->location = pSrcVertexElement->location;
			pDstVertexAttribute->binding = pSrcVertexLayout->binding;
			pDstVertexAttribute->format = ToVkFormat(pSrcVertexElement->format);
			pDstVertexAttribute->offset = pSrcVertexElement->offset;

			pDstVertexAttribute++;
			pSrcVertexElement++;
		}

		pDstVertexBinding++;
		pSrcVertexLayout++;
	}

	VkPipelineVertexInputStateCreateInfo vkVertexInputState;
	vkVertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkVertexInputState.pNext = nullptr;
	vkVertexInputState.flags = 0;
	vkVertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vkVertexBindings.size());
	vkVertexInputState.pVertexBindingDescriptions = vkVertexBindings.data();
	vkVertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vkVertexAttributes.size());
	vkVertexInputState.pVertexAttributeDescriptions = vkVertexAttributes.data();

	// ----------------------------------------------------------------------------------------------------
	// インプットアセンブリ
	// ----------------------------------------------------------------------------------------------------

	VkPipelineInputAssemblyStateCreateInfo vkInputAssembluState{};
	vkInputAssembluState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkInputAssembluState.pNext = nullptr;
	vkInputAssembluState.flags = 0;
	vkInputAssembluState.topology = ToVkPrimitiveTopology(pipelineDesc.primitiveTopology);
	vkInputAssembluState.primitiveRestartEnable = VK_FALSE;

	// ----------------------------------------------------------------------------------------------------
	// テッセレーション
	// ----------------------------------------------------------------------------------------------------

	VkPipelineTessellationStateCreateInfo vkTessellationState{};
	vkTessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	vkTessellationState.pNext = nullptr;
	vkTessellationState.flags = 0;
	vkTessellationState.patchControlPoints = pipelineDesc.tessellation.patchControlPoints;

	// ----------------------------------------------------------------------------------------------------
	// ビューポート
	// ----------------------------------------------------------------------------------------------------

	VkViewport viewport{};
	VkRect2D scissor{};

	VkPipelineViewportStateCreateInfo vkPipelineViewportState{};
	vkPipelineViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkPipelineViewportState.pNext = nullptr;
	vkPipelineViewportState.flags = 0;
	vkPipelineViewportState.viewportCount = 1;
	vkPipelineViewportState.pViewports = &viewport;
	vkPipelineViewportState.scissorCount = 1;
	vkPipelineViewportState.pScissors = &scissor;

	// ----------------------------------------------------------------------------------------------------
	// ラスタライザ
	// ----------------------------------------------------------------------------------------------------

	VkPipelineRasterizationStateCreateInfo vkRasterizationState{};
	vkRasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkRasterizationState.pNext = nullptr;
	vkRasterizationState.flags = 0;
	vkRasterizationState.depthClampEnable = ToVkBool32(pipelineDesc.rasterization.depthClampEnable);
	vkRasterizationState.rasterizerDiscardEnable = ToVkBool32(pipelineDesc.rasterization.discardEnable);
	vkRasterizationState.polygonMode = ToVkPolygonMode(pipelineDesc.rasterization.polygonMode);
	vkRasterizationState.cullMode = ToVkCullMode(pipelineDesc.rasterization.cullMode);
	vkRasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	vkRasterizationState.depthBiasEnable = ToVkBool32(pipelineDesc.rasterization.depthBiasEnable);
	vkRasterizationState.depthBiasConstantFactor = pipelineDesc.rasterization.depthBiasConstantFactor;
	vkRasterizationState.depthBiasClamp = pipelineDesc.rasterization.depthBiasClamp;
	vkRasterizationState.depthBiasSlopeFactor = pipelineDesc.rasterization.depthBiasSlopeFactor;
	vkRasterizationState.lineWidth = 1.0f;

	m_Native.rasterizationState = vkRasterizationState;

	// ----------------------------------------------------------------------------------------------------
	// マルチサンプル
	// ----------------------------------------------------------------------------------------------------

	VkPipelineMultisampleStateCreateInfo vkMultisampleState{};
	vkMultisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkMultisampleState.pNext = nullptr;
	vkMultisampleState.flags = 0;
	vkMultisampleState.rasterizationSamples = ToVkSampleCountFlagBits(pipelineDesc.multisample.rasterizationSamples);
	vkMultisampleState.sampleShadingEnable = ToVkBool32(pipelineDesc.multisample.sampleShadingEnable);
	vkMultisampleState.minSampleShading = pipelineDesc.multisample.minSampleShading;
	vkMultisampleState.pSampleMask = nullptr;
	vkMultisampleState.alphaToCoverageEnable = ToVkBool32(pipelineDesc.multisample.alphaToCoverageEnable);
	vkMultisampleState.alphaToOneEnable = ToVkBool32(pipelineDesc.multisample.alphaToOneEnable);

	// ----------------------------------------------------------------------------------------------------
	// デプスステンシル
	// ----------------------------------------------------------------------------------------------------

	VkPipelineDepthStencilStateCreateInfo vkDepthStencilState{};
	vkDepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	vkDepthStencilState.pNext = nullptr;
	vkDepthStencilState.flags = 0;

	vkDepthStencilState.depthTestEnable = ToVkBool32(pipelineDesc.depthStencil.depthTestEnable);
	vkDepthStencilState.depthWriteEnable = ToVkBool32(pipelineDesc.depthStencil.depthWriteEnable);
	vkDepthStencilState.depthCompareOp = ToVkCompareOp(pipelineDesc.depthStencil.depthCompareOp);
	vkDepthStencilState.depthBoundsTestEnable = ToVkBool32(pipelineDesc.depthStencil.depthBoundsTestEnable);
	vkDepthStencilState.stencilTestEnable = ToVkBool32(pipelineDesc.depthStencil.stencilTestEnable);

	vkDepthStencilState.front.failOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilFront.failOp);
	vkDepthStencilState.front.passOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilFront.passOp);
	vkDepthStencilState.front.depthFailOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilFront.depthFailOp);
	vkDepthStencilState.front.compareOp = ToVkCompareOp(pipelineDesc.depthStencil.stencilFront.compareOp);
	vkDepthStencilState.front.compareMask = pipelineDesc.depthStencil.stencilFront.readMask;
	vkDepthStencilState.front.writeMask = pipelineDesc.depthStencil.stencilFront.writeMask;
	vkDepthStencilState.front.reference = pipelineDesc.depthStencil.stencilFront.reference;

	vkDepthStencilState.back.failOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilBack.failOp);
	vkDepthStencilState.back.passOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilBack.passOp);
	vkDepthStencilState.back.depthFailOp = ToVkStencilOp(pipelineDesc.depthStencil.stencilBack.depthFailOp);
	vkDepthStencilState.back.compareOp = ToVkCompareOp(pipelineDesc.depthStencil.stencilBack.compareOp);
	vkDepthStencilState.back.compareMask = pipelineDesc.depthStencil.stencilBack.readMask;
	vkDepthStencilState.back.writeMask = pipelineDesc.depthStencil.stencilBack.writeMask;
	vkDepthStencilState.back.reference = pipelineDesc.depthStencil.stencilBack.reference;

	vkDepthStencilState.minDepthBounds = pipelineDesc.depthStencil.minDepthBounds;
	vkDepthStencilState.maxDepthBounds = pipelineDesc.depthStencil.maxDepthBounds;

	m_Native.depthStencilState = vkDepthStencilState;

	// ----------------------------------------------------------------------------------------------------
	// カラーブレンド
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkPipelineColorBlendAttachmentState> vkAttachments;
	vkAttachments.resize(pipelineDesc.colorBlend.attachmentCount, VkPipelineColorBlendAttachmentState{});

	const V3DPipelineColorBlendAttachment* pSrcAttachment = &pipelineDesc.colorBlend.pAttachments[0];
	const V3DPipelineColorBlendAttachment* pSrcAttachmentEnd = pSrcAttachment + pipelineDesc.colorBlend.attachmentCount;
	VkPipelineColorBlendAttachmentState* pDstAttachment = vkAttachments.data();

	while (pSrcAttachment != pSrcAttachmentEnd)
	{
		pDstAttachment->blendEnable = ToVkBool32(pSrcAttachment->blendEnable);
		pDstAttachment->srcColorBlendFactor = ToVkBlendFactor(pSrcAttachment->srcColorFactor);
		pDstAttachment->dstColorBlendFactor = ToVkBlendFactor(pSrcAttachment->dstColorFactor);
		pDstAttachment->colorBlendOp = ToVkBlendOp(pSrcAttachment->colorOp);
		pDstAttachment->srcAlphaBlendFactor = ToVkBlendFactor(pSrcAttachment->srcAlphaFactor);
		pDstAttachment->dstAlphaBlendFactor = ToVkBlendFactor(pSrcAttachment->dstAlphaFactor);
		pDstAttachment->alphaBlendOp = ToVkBlendOp(pSrcAttachment->alphaOp);
		pDstAttachment->colorWriteMask = ToVkColorComponentFlags(pSrcAttachment->writeMask);
	
		pDstAttachment++;
		pSrcAttachment++;
	}

	VkPipelineColorBlendStateCreateInfo vkColorBlendState{};
	vkColorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkColorBlendState.pNext = nullptr;
	vkColorBlendState.flags = 0;
	vkColorBlendState.logicOpEnable = ToVkBool32(pipelineDesc.colorBlend.logicOpEnable);
	vkColorBlendState.logicOp = ToVkLogicOp(pipelineDesc.colorBlend.logicOp);
	vkColorBlendState.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
	vkColorBlendState.pAttachments = vkAttachments.data();
	vkColorBlendState.blendConstants[0] = 0.0f;
	vkColorBlendState.blendConstants[1] = 0.0f;
	vkColorBlendState.blendConstants[2] = 0.0f;
	vkColorBlendState.blendConstants[3] = 0.0f;

	// ----------------------------------------------------------------------------------------------------
	// ダイナミックステート
	// ----------------------------------------------------------------------------------------------------

	static constexpr VkDynamicState vkDefaultDynamicStates[] =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_DEPTH_BIAS,
		VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		VK_DYNAMIC_STATE_DEPTH_BOUNDS,
		VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
		VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
		VK_DYNAMIC_STATE_STENCIL_REFERENCE
	};

	static constexpr uint32_t vkDefaultDynamicStateCount = _countof(vkDefaultDynamicStates);

	VkPipelineDynamicStateCreateInfo vkDynamicState{};
	vkDynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vkDynamicState.pNext = nullptr;
	vkDynamicState.flags = 0;
	vkDynamicState.dynamicStateCount = vkDefaultDynamicStateCount;
	vkDynamicState.pDynamicStates = vkDefaultDynamicStates;

	// ----------------------------------------------------------------------------------------------------
	// 作成
	// ----------------------------------------------------------------------------------------------------

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.stageCount = static_cast<uint32_t>(vkStages.size());
	createInfo.pStages = vkStages.data();
	createInfo.pVertexInputState = &vkVertexInputState;
	createInfo.pInputAssemblyState = &vkInputAssembluState;
	createInfo.pTessellationState = (tessellationEnable == true)? &vkTessellationState : nullptr;
	createInfo.pViewportState = &vkPipelineViewportState;
	createInfo.pRasterizationState = &vkRasterizationState;
	createInfo.pMultisampleState = &vkMultisampleState;
	createInfo.pDepthStencilState = &vkDepthStencilState;
	createInfo.pColorBlendState = &vkColorBlendState;
	createInfo.pDynamicState = &vkDynamicState;
	createInfo.layout = m_pPipelineLayout->GetSource().pipelineLayout;
	createInfo.renderPass = m_pRenderPass->GetSource().renderPass;
	createInfo.subpass = pipelineDesc.subpass;
	createInfo.basePipelineHandle = VK_NULL_HANDLE;
	createInfo.basePipelineIndex = 0;

	VkResult vkResult = vkCreateGraphicsPipelines(m_pDevice->GetSource().device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_Source.pipeline);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

/**************************************/
/* public override - IV3DPipelineBase */
/**************************************/

const IV3DPipelineBase::Source& V3DGraphicsPipeline::GetSource() const
{
	return m_Source;
}

void V3DGraphicsPipeline::AfterBind(VkCommandBuffer commandBuffer)
{
	const VkPipelineRasterizationStateCreateInfo& vkRasterizationState = m_Native.rasterizationState;

	if (vkRasterizationState.depthBiasEnable == VK_TRUE)
	{
		vkCmdSetDepthBias(commandBuffer, vkRasterizationState.depthBiasConstantFactor, vkRasterizationState.depthBiasClamp, vkRasterizationState.depthBiasSlopeFactor);
	}

	const VkPipelineDepthStencilStateCreateInfo& vkDepthStencilState = m_Native.depthStencilState;

	if (vkDepthStencilState.stencilTestEnable == VK_TRUE)
	{
		vkCmdSetStencilCompareMask(commandBuffer, VK_STENCIL_FACE_FRONT_BIT, vkDepthStencilState.front.compareMask);
		vkCmdSetStencilWriteMask(commandBuffer, VK_STENCIL_FACE_FRONT_BIT, vkDepthStencilState.front.writeMask);
		vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FACE_FRONT_BIT, vkDepthStencilState.front.reference);

		vkCmdSetStencilCompareMask(commandBuffer, VK_STENCIL_FACE_BACK_BIT, vkDepthStencilState.back.compareMask);
		vkCmdSetStencilWriteMask(commandBuffer, VK_STENCIL_FACE_BACK_BIT, vkDepthStencilState.back.writeMask);
		vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FACE_BACK_BIT, vkDepthStencilState.back.reference);
	}

	if (vkDepthStencilState.depthBoundsTestEnable == VK_TRUE)
	{
		vkCmdSetDepthBounds(commandBuffer, vkDepthStencilState.minDepthBounds, vkDepthStencilState.maxDepthBounds);
	}
}

/**********************************/
/* public override - IV3DPipeline */
/**********************************/

V3D_PIPELINE_TYPE V3DGraphicsPipeline::GetType() const
{
	return V3D_PIPELINE_TYPE_GRAPHICS;
}

void V3DGraphicsPipeline::GetLayout(IV3DPipelineLayout** ppPipelineLayout)
{
	(*ppPipelineLayout) = V3D_TO_ADD_REF(m_pPipelineLayout);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DGraphicsPipeline::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DGraphicsPipeline::GetRefCount() const
{
	return m_RefCounter;
}

void V3DGraphicsPipeline::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DGraphicsPipeline::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DGraphicsPipeline);
	}
}

/*********************************/
/* private - V3DGraphicsPipeline */
/*********************************/

V3DGraphicsPipeline::V3DGraphicsPipeline() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pPipelineLayout(nullptr),
	m_pRenderPass(nullptr),
	m_Source({}),
	m_Native({})
{
	m_Source.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	m_Source.pipeline = VK_NULL_HANDLE;
}

V3DGraphicsPipeline::~V3DGraphicsPipeline()
{
	if (m_Source.pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(m_pDevice->GetSource().device, m_Source.pipeline, nullptr);
	}

	if (m_ShaderModules.empty() == false)
	{
		STLVector<V3DShaderModule*>::iterator it_begin = m_ShaderModules.begin();
		STLVector<V3DShaderModule*>::iterator it_end = m_ShaderModules.end();
		STLVector<V3DShaderModule*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	V3D_RELEASE(m_pRenderPass);
	V3D_RELEASE(m_pPipelineLayout);
	V3D_RELEASE(m_pDevice);
}

bool V3DGraphicsPipeline::PreShader(
	const V3DPipelineShader& shader,
	uint32_t& shaderCount, uint32_t& specializationCount, uint32_t& constantCount)
{
	if ((shader.pModule != nullptr) && ((shader.pEntryPointName == nullptr) || (::strlen(shader.pEntryPointName) == 0)))
	{
		return false;
	}

	if ((shader.constantCount > 0) && ((shader.pConstantData == nullptr) || (shader.pConstantData == nullptr)))
	{
		return false;
	}

	if (shader.pModule != nullptr)
	{
		shaderCount++;
	}

	if (shader.constantCount > 0)
	{
		specializationCount++;
		constantCount += shader.constantCount;
	}

	return true;
}

bool V3DGraphicsPipeline::AddShader(
	const V3DPipelineShader& shader, VkShaderStageFlagBits shaderStage,
	STLVector<VkPipelineShaderStageCreateInfo>& shaderStageCreateInfos,
	STLVector<VkSpecializationInfo>& specializationInfos,
	STLVector<VkSpecializationMapEntry>& specializationMapEntries,
	STLVector<V3DShaderModule*>& shaderModules)
{
	if ((shader.pModule == nullptr) || (shader.pEntryPointName == nullptr) || (::strlen(shader.pEntryPointName) == 0))
	{
		return false;
	}

	V3DShaderModule* pShaderModule = static_cast<V3DShaderModule*>(shader.pModule);

	VkPipelineShaderStageCreateInfo vkShaderStageCreateInfo{};
	vkShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkShaderStageCreateInfo.pNext = nullptr;
	vkShaderStageCreateInfo.flags = 0;
	vkShaderStageCreateInfo.stage = shaderStage;
	vkShaderStageCreateInfo.module = pShaderModule->GetSource().shaderModule;
	vkShaderStageCreateInfo.pName = shader.pEntryPointName;

	if (shader.constantCount > 0)
	{
		size_t specializationInfoIndex = specializationInfos.size();
		size_t specializationMapEntryIndex = specializationMapEntries.size();

		V3DPipelineShaderConstantDesc* pConstant = shader.pConstants;
		V3DPipelineShaderConstantDesc* pConstantEnd = pConstant + shader.constantCount;

		uint32_t dataSize = 0;

		while (pConstant != pConstantEnd)
		{
			VkSpecializationMapEntry vkMapEntry{};
			vkMapEntry.constantID = pConstant->id;
			vkMapEntry.offset = pConstant->offset;
			vkMapEntry.size = pConstant->size;
			specializationMapEntries.push_back(vkMapEntry);

			dataSize = V3D_MAX(dataSize, pConstant->offset + pConstant->size);

			pConstant++;
		}

		VkSpecializationInfo vkSpecializationInfo{};
		vkSpecializationInfo.mapEntryCount = shader.constantCount;
		vkSpecializationInfo.pMapEntries = &specializationMapEntries[specializationMapEntryIndex];
		vkSpecializationInfo.pData = shader.pConstantData;
		vkSpecializationInfo.dataSize = dataSize;
		specializationInfos.push_back(vkSpecializationInfo);

		vkShaderStageCreateInfo.pSpecializationInfo = &specializationInfos[specializationInfoIndex];
	}
	else
	{
		vkShaderStageCreateInfo.pSpecializationInfo = nullptr;
	}

	shaderStageCreateInfos.push_back(vkShaderStageCreateInfo);

	shaderModules.push_back(V3D_TO_ADD_REF(pShaderModule));

	return true;
}
