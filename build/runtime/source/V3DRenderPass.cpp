#include "V3DRenderPass.h"
#include "V3DDevice.h"

/**************************/
/* public - V3DRenderPass */
/**************************/

V3DRenderPass* V3DRenderPass::Create()
{
	return V3D_NEW_T(V3DRenderPass);
}

V3D_RESULT V3DRenderPass::Initialize(
	IV3DDevice* pDevice,
	uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments,
	uint32_t subpassCount, const V3DSubpassDesc* pSubpasses,
	uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies,
	const wchar_t* pDebugName)
{
	V3D_ASSERT(attachmentCount > 0);
	V3D_ASSERT(pAttachments != nullptr);
	V3D_ASSERT(subpassCount > 0);
	V3D_ASSERT(pSubpasses != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	V3D_DEBUG_CODE(m_DebugName = V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// アタッチメントの記述
	// ----------------------------------------------------------------------------------------------------

	const V3DAttachmentDesc* pSrcAttachment = &pAttachments[0];
	const V3DAttachmentDesc* pSrcAttachmentEnd = pSrcAttachment + attachmentCount;

	STLVector<VkAttachmentDescription> vkAttachments;
	vkAttachments.resize(attachmentCount);
	VkAttachmentDescription* pDstAttachment = vkAttachments.data();

	uint32_t lastClearAttachment = 0;
	uint32_t clearAttachmentCount = 0;
	uint32_t attachmentIndex = 0;

	m_ClearValues.resize(attachmentCount, VkClearValue{});
	VkClearValue* pDstClearValue = m_ClearValues.data();

#ifdef _DEBUG
	m_Source.debug.attachments.resize(attachmentCount);
	V3DRenderPass::Attachment* pDebugAttachments = m_Source.debug.attachments.data();
#endif //_DEBUG

	while (pSrcAttachment != pSrcAttachmentEnd)
	{
		pDstAttachment->flags = 0;
		pDstAttachment->format = ToVkFormat(pSrcAttachment->format);
		pDstAttachment->samples = ToVkSampleCountFlagBits(pSrcAttachment->samples);
		pDstAttachment->loadOp = ToVkAttachmentLoadOp(pSrcAttachment->loadOp);
		pDstAttachment->storeOp = ToVkAttachmentStoreOp(pSrcAttachment->storeOp);
		pDstAttachment->stencilLoadOp = ToVkAttachmentLoadOp(pSrcAttachment->stencilLoadOp);
		pDstAttachment->stencilStoreOp = ToVkAttachmentStoreOp(pSrcAttachment->stencilStoreOp);
		pDstAttachment->initialLayout = ToVkImageLayout(pSrcAttachment->initialLayout);
		pDstAttachment->finalLayout = ToVkImageLayout(pSrcAttachment->finalLayout);

		if ((pDstAttachment->loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) || ((pDstAttachment->stencilLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR)))
		{
			pDstClearValue->color.uint32[0] = pSrcAttachment->clearValue.color.uint32[0];
			pDstClearValue->color.uint32[1] = pSrcAttachment->clearValue.color.uint32[1];
			pDstClearValue->color.uint32[2] = pSrcAttachment->clearValue.color.uint32[2];
			pDstClearValue->color.uint32[3] = pSrcAttachment->clearValue.color.uint32[3];

			lastClearAttachment = attachmentIndex;
			clearAttachmentCount++;
		}

#ifdef _DEBUG
		pDebugAttachments->attachment = attachmentIndex;
		pDebugAttachments->format = pSrcAttachment->format;
		pDebugAttachments->samples = pSrcAttachment->samples;
		pDebugAttachments++;
#endif //_DEBUG

		attachmentIndex++;

		pDstClearValue++;
		pDstAttachment++;
		pSrcAttachment++;
	}

	if (clearAttachmentCount > 0)
	{
		m_Source.clearValueCount = lastClearAttachment + 1;
		m_ClearValues.resize(m_Source.clearValueCount);
	}
	else
	{
		m_Source.clearValueCount = 0;
		m_ClearValues.resize(0);
	}

	m_ClearValues.shrink_to_fit();
	m_Source.pClearValues = m_ClearValues.data();

	// ----------------------------------------------------------------------------------------------------
	// サブパスの記述
	// ----------------------------------------------------------------------------------------------------

	const V3DSubpassDesc* pSrcSubpassBegin = &pSubpasses[0];
	const V3DSubpassDesc* pSrcSubpassEnd = pSrcSubpassBegin + subpassCount;
	const V3DSubpassDesc* pSrcSubpass = pSrcSubpassBegin;

	// すべてのサブパスのアタッチメントの参照をカウント
	uint32_t attachmentReferenceCount = 0;
	while (pSrcSubpass != pSrcSubpassEnd)
	{
		attachmentReferenceCount += pSrcSubpass->inputAttachmentCount;
		attachmentReferenceCount += pSrcSubpass->colorAttachmentCount + ((pSrcSubpass->pResolveAttachments != nullptr) ? pSrcSubpass->colorAttachmentCount : 0) + 1;

		if (pSrcSubpass->pResolveAttachments != nullptr)
		{
			attachmentReferenceCount += pSrcSubpass->colorAttachmentCount;
		}

		if (pSrcSubpass->pDepthStencilAttachment != nullptr)
		{
			attachmentReferenceCount += 1;
		}

		pSrcSubpass++;
	}

	// ここから Vulkan のサブパスを作成
	STLVector<VkAttachmentReference> vkAttachmentReferences;
	vkAttachmentReferences.resize(attachmentReferenceCount);
	VkAttachmentReference* pAttachmentReferencePtr = vkAttachmentReferences.data();

	STLVector<VkSubpassDescription> vkSubpasses;
	vkSubpasses.resize(subpassCount);
	VkSubpassDescription* pDstSubpass = vkSubpasses.data();

#ifdef _DEBUG
	m_Source.debug.subpasses.resize(subpassCount);
	V3DRenderPass::Subpass* pDebugSubpass = m_Source.debug.subpasses.data();
	uint32_t debugSubpassCount = 0;
	uint32_t debugErrorCount = 0;
#endif //_DEBUG

	pSrcSubpass = pSrcSubpassBegin;
	while (pSrcSubpass != pSrcSubpassEnd)
	{
		pDstSubpass->flags = 0;
		pDstSubpass->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		pDstSubpass->inputAttachmentCount = pSrcSubpass->inputAttachmentCount;
		pDstSubpass->pInputAttachments = V3DRenderPass::WriteAttachmentReferences(vkAttachmentReferences, pSrcSubpass->inputAttachmentCount, pSrcSubpass->pInputAttachments, &pAttachmentReferencePtr);
		pDstSubpass->colorAttachmentCount = pSrcSubpass->colorAttachmentCount;
		pDstSubpass->pColorAttachments = V3DRenderPass::WriteAttachmentReferences(vkAttachmentReferences, pSrcSubpass->colorAttachmentCount, pSrcSubpass->pColorAttachments, &pAttachmentReferencePtr);
		pDstSubpass->pResolveAttachments = V3DRenderPass::WriteAttachmentReferences(vkAttachmentReferences, pSrcSubpass->colorAttachmentCount, pSrcSubpass->pResolveAttachments, &pAttachmentReferencePtr);
		pDstSubpass->pDepthStencilAttachment = V3DRenderPass::WriteAttachmentReferences(vkAttachmentReferences, (pSrcSubpass->pDepthStencilAttachment != nullptr)? 1 : 0, pSrcSubpass->pDepthStencilAttachment, &pAttachmentReferencePtr);
		pDstSubpass->preserveAttachmentCount = pSrcSubpass->preserveAttachmentCount;
		pDstSubpass->pPreserveAttachments = pSrcSubpass->pPreserveAttachments;

#ifdef _DEBUG
		pDebugSubpass->inputAttachments.reserve(pSrcSubpass->inputAttachmentCount);
		for (uint32_t i = 0; i < pSrcSubpass->inputAttachmentCount; i++)
		{
			if (attachmentCount <= pSrcSubpass->pInputAttachments[i].attachment)
			{
				V3D_LOG_PRINT_ERROR(Log_Error_OutOfInputAttachment, m_DebugName.c_str(), debugSubpassCount, i, pSrcSubpass->pInputAttachments[i].attachment, attachmentCount);
				debugErrorCount++;
			}

			V3DRenderPass::Attachment debugAttachment;
			debugAttachment.attachment = pSrcSubpass->pInputAttachments[i].attachment;
			debugAttachment.format = pAttachments[debugAttachment.attachment].format;
			debugAttachment.samples = pAttachments[debugAttachment.attachment].samples;
			pDebugSubpass->inputAttachments.push_back(debugAttachment);
		}

		pDebugSubpass->colorAttachments.reserve(pSrcSubpass->colorAttachmentCount);
		for (uint32_t i = 0; i < pSrcSubpass->colorAttachmentCount; i++)
		{
			if (attachmentCount <= pSrcSubpass->pColorAttachments[i].attachment)
			{
				V3D_LOG_PRINT_ERROR(Log_Error_OutOfColorAttachment, m_DebugName.c_str(), debugSubpassCount, i, pSrcSubpass->pColorAttachments[i].attachment, attachmentCount);
				debugErrorCount++;
			}

			V3DRenderPass::Attachment debugAttachment;
			debugAttachment.attachment = pSrcSubpass->pColorAttachments[i].attachment;
			debugAttachment.format = pAttachments[debugAttachment.attachment].format;
			debugAttachment.samples = pAttachments[debugAttachment.attachment].samples;
			pDebugSubpass->colorAttachments.push_back(debugAttachment);
		}

		if (pSrcSubpass->pResolveAttachments != nullptr)
		{
			pDebugSubpass->resolveAttachments.reserve(pSrcSubpass->colorAttachmentCount);
			for (uint32_t i = 0; i < pSrcSubpass->colorAttachmentCount; i++)
			{
				if (attachmentCount <= pSrcSubpass->pResolveAttachments[i].attachment)
				{
					V3D_LOG_PRINT_ERROR(Log_Error_OutOfResolveAttachment, m_DebugName.c_str(), debugSubpassCount, i, pSrcSubpass->pResolveAttachments[i].attachment, attachmentCount);
					debugErrorCount++;
				}

				V3DRenderPass::Attachment debugAttachment;
				debugAttachment.attachment = pSrcSubpass->pResolveAttachments[i].attachment;
				debugAttachment.format = pAttachments[debugAttachment.attachment].format;
				debugAttachment.samples = pAttachments[debugAttachment.attachment].samples;
				if (debugAttachment.samples != V3D_SAMPLE_COUNT_1)
				{
					V3D_LOG_PRINT_ERROR(Log_Error_MismatchResolveAttachmentSamples, m_DebugName.c_str(), debugSubpassCount, i, pSrcSubpass->pResolveAttachments[i].attachment);
					debugErrorCount++;
				}

				pDebugSubpass->resolveAttachments.push_back(debugAttachment);
			}
		}

		debugSubpassCount++;
		pDebugSubpass++;
#endif //_DEBUG

		pDstSubpass++;
		pSrcSubpass++;
	}

#ifdef _DEBUG
	if (debugErrorCount > 0)
	{
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// サブパスの依存性
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkSubpassDependency> vkSubpassDependencies;

	if (subpassDependencyCount > 0)
	{
		const V3DSubpassDependencyDesc* pSrcSubpassDependency = &pSubpassDependencies[0];
		const V3DSubpassDependencyDesc* pSrcSubpassDependencyEnd = pSrcSubpassDependency + subpassDependencyCount;

		vkSubpassDependencies.resize(subpassDependencyCount);

		VkSubpassDependency* pDstSubpassDependency = vkSubpassDependencies.data();

		while (pSrcSubpassDependency != pSrcSubpassDependencyEnd)
		{
			pDstSubpassDependency->srcSubpass = pSrcSubpassDependency->srcSubpass;
			pDstSubpassDependency->dstSubpass = pSrcSubpassDependency->dstSubpass;
			pDstSubpassDependency->srcStageMask = ToVkPipelineStageFlags(pSrcSubpassDependency->srcStageMask);
			pDstSubpassDependency->dstStageMask = ToVkPipelineStageFlags(pSrcSubpassDependency->dstStageMask);
			pDstSubpassDependency->srcAccessMask = ToVkAccessFlags(pSrcSubpassDependency->srcAccessMask);
			pDstSubpassDependency->dstAccessMask = ToVkAccessFlags(pSrcSubpassDependency->dstAccessMask);
			pDstSubpassDependency->dependencyFlags = ToVkDependencyFlags(pSrcSubpassDependency->dependencyFlags);

			pDstSubpassDependency++;
			pSrcSubpassDependency++;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 作成
	// ----------------------------------------------------------------------------------------------------

	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
	createInfo.pAttachments = vkAttachments.data();
	createInfo.subpassCount = static_cast<uint32_t>(vkSubpasses.size());
	createInfo.pSubpasses = vkSubpasses.data();
	createInfo.dependencyCount = static_cast<uint32_t>(vkSubpassDependencies.size());
	createInfo.pDependencies = vkSubpassDependencies.data();

	VkResult vkResult = vkCreateRenderPass(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.renderPass);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.renderPass, m_DebugName.c_str());

	return V3D_OK;
}

const V3DRenderPass::Source& V3DRenderPass::GetSource() const
{
	return m_Source;
}

#ifdef _DEBUG

const wchar_t* V3DRenderPass::GetDebugName() const
{
	return m_DebugName.c_str();
}

#endif //_DEBUG

/************************************/
/* public override - IV3DRenderPass */
/************************************/

V3DClearValue V3DRenderPass::GetAttachmentClearValue(uint32_t attachment)
{
	static constexpr V3DClearValue dummyClearValue{};

	if (m_ClearValues.size() <= attachment)
	{
		return dummyClearValue;
	}

	const VkClearValue& src = m_ClearValues[attachment];

	V3DClearValue ret;
	ret.color.uint32[0] = src.color.uint32[0];
	ret.color.uint32[1] = src.color.uint32[1];
	ret.color.uint32[2] = src.color.uint32[2];
	ret.color.uint32[3] = src.color.uint32[3];
	ret.depthStencil.depth = src.depthStencil.depth;
	ret.depthStencil.stencil = src.depthStencil.stencil;

	return ret;
}

void V3DRenderPass::SetAttachmentClearValue(uint32_t attachment, const V3DClearValue& clearValue)
{
	if (m_ClearValues.size() <= attachment)
	{
		return;
	}

	VkClearValue& dst = m_ClearValues[attachment];
	dst.color.uint32[0] = clearValue.color.uint32[0];
	dst.color.uint32[1] = clearValue.color.uint32[1];
	dst.color.uint32[2] = clearValue.color.uint32[2];
	dst.color.uint32[3] = clearValue.color.uint32[3];
	dst.depthStencil.depth = clearValue.depthStencil.depth;
	dst.depthStencil.stencil = clearValue.depthStencil.stencil;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DRenderPass::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DRenderPass::GetRefCount() const
{
	return m_RefCounter;
}

void V3DRenderPass::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DRenderPass::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DRenderPass);
	}
}

/***************************/
/* private - V3DRenderPass */
/***************************/

V3DRenderPass::V3DRenderPass() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Source({})
{
	m_Source.renderPass = VK_NULL_HANDLE;
}

V3DRenderPass::~V3DRenderPass()
{
	if (m_Source.renderPass != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(m_pDevice->GetSource().device, m_Source.renderPass, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.renderPass);
	}

	V3D_RELEASE(m_pDevice);
}

VkAttachmentReference* V3DRenderPass::AllocateVkAttachmentReferences(STLVector<VkAttachmentReference>& vkAttchmentReferences, uint32_t totalAttachmentReferenceCount)
{
	vkAttchmentReferences.resize(totalAttachmentReferenceCount);
	return vkAttchmentReferences.data();
}

VkAttachmentReference* V3DRenderPass::WriteAttachmentReferences(STLVector<VkAttachmentReference>& vkAttchmentReferences, uint32_t attachmentReferenceCount, const V3DAttachmentReference* pAttachmentReferences, VkAttachmentReference** ppVkAttchmentReferencePtr)
{
	if ((attachmentReferenceCount == 0) || (pAttachmentReferences == nullptr))
	{
		return nullptr;
	}

	const V3DAttachmentReference* pSrcAttachmentReference = &pAttachmentReferences[0];
	const V3DAttachmentReference* pSrcAttachmentReferenceEnd = pSrcAttachmentReference + attachmentReferenceCount;

	VkAttachmentReference* pDstAttachmentReference = *ppVkAttchmentReferencePtr;

	while (pSrcAttachmentReference != pSrcAttachmentReferenceEnd)
	{
		pDstAttachmentReference->attachment = pSrcAttachmentReference->attachment;
		pDstAttachmentReference->layout = ToVkImageLayout(pSrcAttachmentReference->layout);

		pDstAttachmentReference++;
		pSrcAttachmentReference++;
	}

	VkAttachmentReference* pVkOldAttchmentReferencePtr = *ppVkAttchmentReferencePtr;

	*ppVkAttchmentReferencePtr = pDstAttachmentReference;

	return pVkOldAttchmentReferencePtr;
}
