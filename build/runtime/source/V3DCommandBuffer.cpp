#include "V3DCommandBuffer.h"
#include "V3DDevice.h"
#include "V3DAdapter.h"
#include "V3DCommandPool.h"
#include "V3DBuffer.h"
#include "V3DBufferView.h"
#include "IV3DImageBase.h"
#include "V3DImageView.h"
#include "V3DRenderPass.h"
#include "V3DFrameBuffer.h"
#include "IV3DPipelineBase.h"
#include "V3DPipelineLayout.h"
#include "V3DStandardDescriptorSet.h"
#include "V3DPushDescriptorSet.h"
#include "V3DQueryPool.h"

/*****************************/
/* public - V3DCommandBuffer */
/*****************************/

V3DCommandBuffer* V3DCommandBuffer::Create()
{
	return V3D_NEW_T(V3DCommandBuffer);
}

V3D_RESULT V3DCommandBuffer::Initialize(IV3DDevice* pDevice, IV3DCommandPool* pCommandPool, V3D_COMMAND_BUFFER_TYPE commandBufferType, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pCommandPool != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pCommandPool = V3D_TO_ADD_REF(static_cast<V3DCommandPool*>(pCommandPool));

	// ----------------------------------------------------------------------------------------------------
	// コマンドバッファを作成
	// ----------------------------------------------------------------------------------------------------

	VkCommandBufferLevel level;
	switch (commandBufferType)
	{
	case V3D_COMMAND_BUFFER_TYPE_PRIMARY:
		level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		break;
	case V3D_COMMAND_BUFFER_TYPE_SECONDARY:
		level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		break;

	default:
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.commandPool = m_pCommandPool->GetSource().commandPool;
	allocInfo.level = level;
	allocInfo.commandBufferCount = 1;

	VkResult vkResult = vkAllocateCommandBuffers(m_pDevice->GetSource().device, &allocInfo, &m_Source.commandBuffer);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.commandBuffer, V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// 拡張機能を取得
	// ----------------------------------------------------------------------------------------------------

	const V3DDeviceCaps& deviceCaps = m_pDevice->GetCaps();

	if (deviceCaps.extensionFlags & V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS)
	{
		m_pPushDescriptorSetFunction = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(m_pDevice->GetSource().device, "vkCmdPushDescriptorSetKHR"));
	}

	if (deviceCaps.extensionFlags & V3D_DEVICE_EXTENSION_DEBUG_MARKER)
	{
		m_pDebugMarkerBeginFunction = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(m_pDevice->GetSource().device, "vkCmdDebugMarkerBeginEXT"));
		m_pDebugMarkerEndFunction = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(m_pDevice->GetSource().device, "vkCmdDebugMarkerEndEXT"));
		m_pDebugMarkerInsertFunction = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(m_pDevice->GetSource().device, "vkCmdDebugMarkerInsertEXT"));
	}

	// ----------------------------------------------------------------------------------------------------
	// 記述の初期化
	// ----------------------------------------------------------------------------------------------------

	m_Type = commandBufferType;

	const V3DDeviceCaps& adapterCaps = m_pDevice->GetCaps();

	VkBool32 occlusionQueryEnable = (adapterCaps.queryFlags & V3D_QUERY_CAP_INHERITED_QUERIES) ? VK_TRUE : VK_FALSE;
	VkBool32 queryPreciseEnable = (adapterCaps.queryFlags & V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE) ? VK_TRUE : VK_FALSE;

	VkQueryPipelineStatisticFlags pipelineStatisticFlags = 0;
	if (adapterCaps.queryFlags & V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY)
	{
		pipelineStatisticFlags = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
	}

	VkCommandBufferInheritanceInfo& commandBufferInheritanceInfo = m_Source.commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = VK_NULL_HANDLE;
	commandBufferInheritanceInfo.occlusionQueryEnable = occlusionQueryEnable;
	commandBufferInheritanceInfo.queryFlags = ((occlusionQueryEnable == VK_TRUE) && (queryPreciseEnable == VK_TRUE))? VK_QUERY_CONTROL_PRECISE_BIT : 0;
	commandBufferInheritanceInfo.pipelineStatistics = pipelineStatisticFlags;

	VkCommandBufferBeginInfo& commandBufferBeginInfo = m_Source.commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DCommandBuffer::Source& V3DCommandBuffer::GetSource() const
{
	return m_Source;
}

/***************************************/
/* public override - IV3DCommandBuffer */
/***************************************/

V3D_COMMAND_BUFFER_TYPE V3DCommandBuffer::GetType() const
{
	return m_Type;
}

void V3DCommandBuffer::GetPool(IV3DCommandPool** ppPool)
{
	(*ppPool) = V3D_TO_ADD_REF(m_pCommandPool);
}

V3D_RESULT V3DCommandBuffer::Reset(V3DFlags resetFlags)
{
	VkFlags vkResetFlags = 0;
	if (resetFlags & V3D_COMMAND_BUFFER_RESET_RELEASE_RESOURCES) { vkResetFlags |= VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT; }

	VkResult vkResult = vkResetCommandBuffer(m_Source.commandBuffer, vkResetFlags);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DCommandBuffer::Begin()
{
#ifdef _DEBUG
	if (m_Debug.isBegin == true)
	{
		V3D_LOG_ERROR(Log_Error_CommandBufferAlreadyBegin, m_DebugName.c_str());
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	VkCommandBufferBeginInfo& commandBufferBeginInfo = m_Source.commandBufferBeginInfo;
	commandBufferBeginInfo.flags = 0;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;

	VkResult vkResult = vkBeginCommandBuffer(m_Source.commandBuffer, &m_Source.commandBufferBeginInfo);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

#ifdef _DEBUG
	m_Debug.isBegin = true;
#endif //_DEBUG

	return V3D_OK;
}

V3D_RESULT V3DCommandBuffer::Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer)
{
#ifdef _DEBUG
	if (m_Debug.isBegin == true)
	{
		V3D_LOG_ERROR(Log_Error_CommandBufferAlreadyBegin, m_DebugName.c_str());
		return V3D_ERROR_FAIL;
	}

	if (usageFlags & V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE)
	{
		if (m_Type == V3D_COMMAND_BUFFER_TYPE_PRIMARY)
		{
			V3D_LOG_WARNING(Log_Warning_CommandBufferRenderPassContinue, m_DebugName.c_str());
		}

		if (pRenderPass == nullptr)
		{
			V3D_LOG_ERROR(Log_Error_CommandBufferRenderPassContinue, m_DebugName.c_str());
			return V3D_ERROR_FAIL;
		}
	}
#endif //_DEBUG

	VkCommandBufferBeginInfo& commandBufferBeginInfo = m_Source.commandBufferBeginInfo;
	commandBufferBeginInfo.flags = ToVkCommandBufferUsageFlags(usageFlags);

	if (m_Type == V3D_COMMAND_BUFFER_TYPE_SECONDARY)
	{
		VkCommandBufferInheritanceInfo& commandBufferInheritanceInfo = m_Source.commandBufferInheritanceInfo;
		commandBufferInheritanceInfo.renderPass = (pRenderPass != nullptr) ? static_cast<V3DRenderPass*>(pRenderPass)->GetSource().renderPass : VK_NULL_HANDLE;
		commandBufferInheritanceInfo.subpass = subpass;
		commandBufferInheritanceInfo.framebuffer = (pFrameBuffer != nullptr) ? static_cast<V3DFrameBuffer*>(pFrameBuffer)->GetSource().framebuffer : VK_NULL_HANDLE;

		commandBufferBeginInfo.pInheritanceInfo = &m_Source.commandBufferInheritanceInfo;
	}
	else
	{
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
	}

	VkResult vkResult = vkBeginCommandBuffer(m_Source.commandBuffer, &m_Source.commandBufferBeginInfo);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

#ifdef _DEBUG
	m_Debug.isBegin = true;
#endif //_DEBUG

	return V3D_OK;
}

V3D_RESULT V3DCommandBuffer::End()
{
#ifdef _DEBUG
	if (m_Debug.isBegin == false)
	{
		V3D_LOG_ERROR(Log_Error_CommandBufferAlreadyEnd, m_DebugName.c_str());
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	VkResult vkResult = vkEndCommandBuffer(m_Source.commandBuffer);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

#ifdef _DEBUG
	m_Debug.isBegin = false;
#endif //_DEBUG

	return V3D_OK;
}

void V3DCommandBuffer::BarrierBuffer(IV3DBuffer* pBuffer, const V3DBarrierBufferDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pBuffer == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pBuffer));
		return;
	}
#endif //_DEBUG

	const V3DBuffer::Source& source = static_cast<V3DBuffer*>(pBuffer)->GetSource();

	VkBufferMemoryBarrier vkBarrier;
	vkBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	vkBarrier.pNext = nullptr;
	vkBarrier.srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
	vkBarrier.dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
	vkBarrier.srcQueueFamilyIndex = barrier.srcQueueFamily;
	vkBarrier.dstQueueFamilyIndex = barrier.dstQueueFamily;
	vkBarrier.buffer = source.buffer;
	vkBarrier.offset = barrier.offset;
	vkBarrier.size = (barrier.size == 0)? (source.memorySize - barrier.offset) : barrier.size;

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		1, &vkBarrier,
		0, nullptr);
}

void V3DCommandBuffer::BarrierBufferView(IV3DBufferView* pBufferView, const V3DBarrierBufferViewDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pBufferView == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierBufferView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pBufferView));
		return;
	}
#endif //_DEBUG

	const V3DBufferView::Source& source = static_cast<V3DBufferView*>(pBufferView)->GetSource();

	VkBufferMemoryBarrier vkBarrier;
	vkBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	vkBarrier.pNext = nullptr;
	vkBarrier.srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
	vkBarrier.dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
	vkBarrier.srcQueueFamilyIndex = barrier.srcQueueFamily;
	vkBarrier.dstQueueFamilyIndex = barrier.dstQueueFamily;
	vkBarrier.buffer = source.buffer;
	vkBarrier.offset = source.offset;
	vkBarrier.size = source.size;

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		1, &vkBarrier,
		0, nullptr);
}

void V3DCommandBuffer::BarrierBufferViews(uint32_t bufferViewCount, IV3DBufferView** ppBufferViews, const V3DBarrierBufferViewDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((bufferViewCount == 0) || (ppBufferViews == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierBufferViews << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(bufferViewCount, 0) << V3D_LOG_S_PTR(ppBufferViews));
		return;
	}
#endif //_DEBUG

	V3DBufferView** ppSrc = reinterpret_cast<V3DBufferView**>(ppBufferViews);
	V3DBufferView** ppSrcEnd = ppSrc + bufferViewCount;

	m_Temp.bufferMemoryBarriers.resize(bufferViewCount);
	VkBufferMemoryBarrier* pDst = m_Temp.bufferMemoryBarriers.data();

	while (ppSrc != ppSrcEnd)
	{
		const V3DBufferView::Source& bufferViewSource = (*ppSrc)->GetSource();

		pDst->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		pDst->pNext = nullptr;
		pDst->srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
		pDst->dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
		pDst->srcQueueFamilyIndex = barrier.srcQueueFamily;
		pDst->dstQueueFamilyIndex = barrier.dstQueueFamily;
		pDst->buffer = bufferViewSource.buffer;
		pDst->offset = bufferViewSource.offset;
		pDst->size = bufferViewSource.size;

		ppSrc++;
		pDst++;
	}

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		bufferViewCount, m_Temp.bufferMemoryBarriers.data(),
		0, nullptr);
}

void V3DCommandBuffer::BarrierImage(IV3DImage* pImage, const V3DBarrierImageDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pImage == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pImage));
		return;
	}
#endif //_DEBUG

	const IV3DImageBase::Source& source = static_cast<IV3DImageBase*>(pImage)->GetSource();
	const V3DImageDesc& desc = pImage->GetDesc();

	VkImageMemoryBarrier vkBarrier;
	vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkBarrier.pNext = nullptr;
	vkBarrier.srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
	vkBarrier.dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
	vkBarrier.oldLayout = ToVkImageLayout(barrier.srcLayout);
	vkBarrier.newLayout = ToVkImageLayout(barrier.dstLayout);
	vkBarrier.srcQueueFamilyIndex = barrier.srcQueueFamily;
	vkBarrier.dstQueueFamilyIndex = barrier.dstQueueFamily;
	vkBarrier.image = source.image;
	vkBarrier.subresourceRange.aspectMask = source.aspectFlags;
	vkBarrier.subresourceRange.baseMipLevel = barrier.baseLevel;
	vkBarrier.subresourceRange.levelCount = (barrier.levelCount == 0)? (desc.levelCount - barrier.baseLevel) : barrier.levelCount;
	vkBarrier.subresourceRange.baseArrayLayer = barrier.baseLayer;
	vkBarrier.subresourceRange.layerCount = (barrier.layerCount == 0)? (desc.layerCount - barrier.baseLayer) : barrier.layerCount;

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		0, nullptr,
		1, &vkBarrier);
}

void V3DCommandBuffer::BarrierImageView(IV3DImageView* pImageView, const V3DBarrierImageViewDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pImageView == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& source = static_cast<V3DImageView*>(pImageView)->GetSource();

	VkImageMemoryBarrier vkBarrier;
	vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkBarrier.pNext = nullptr;
	vkBarrier.srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
	vkBarrier.dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
	vkBarrier.oldLayout = ToVkImageLayout(barrier.srcLayout);
	vkBarrier.newLayout = ToVkImageLayout(barrier.dstLayout);
	vkBarrier.srcQueueFamilyIndex = barrier.srcQueueFamily;
	vkBarrier.dstQueueFamilyIndex = barrier.dstQueueFamily;
	vkBarrier.image = source.image;
	vkBarrier.subresourceRange = source.imageSubresourceRange;

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		0, nullptr,
		1, &vkBarrier);
}

void V3DCommandBuffer::BarrierImageViews(uint32_t imageVewCount, IV3DImageView** ppImageViews, const V3DBarrierImageViewDesc& barrier)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((imageVewCount == 0) || (ppImageViews == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BarrierImageViews << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument <<  V3D_LOG_S_NUM_GREATER(imageVewCount, 0) << V3D_LOG_S_PTR(ppImageViews));
		return;
	}
#endif //_DEBUG

	V3DImageView** ppSrc = reinterpret_cast<V3DImageView**>(ppImageViews);
	V3DImageView** ppSrcEnd = ppSrc + imageVewCount;

	m_Temp.imageMemoryBarriers.resize(imageVewCount);
	VkImageMemoryBarrier* pDst = m_Temp.imageMemoryBarriers.data();

	while (ppSrc != ppSrcEnd)
	{
		const V3DImageView::Source& imageViewSource = (*ppSrc)->GetSource();

		pDst->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		pDst->pNext = nullptr;
		pDst->srcAccessMask = ToVkAccessFlags(barrier.srcAccessMask);
		pDst->dstAccessMask = ToVkAccessFlags(barrier.dstAccessMask);
		pDst->oldLayout = ToVkImageLayout(barrier.srcLayout);
		pDst->newLayout = ToVkImageLayout(barrier.dstLayout);
		pDst->srcQueueFamilyIndex = barrier.srcQueueFamily;
		pDst->dstQueueFamilyIndex = barrier.dstQueueFamily;
		pDst->image = imageViewSource.image;
		pDst->subresourceRange = imageViewSource.imageSubresourceRange;

		ppSrc++;
		pDst++;
	}

	vkCmdPipelineBarrier(
		m_Source.commandBuffer,
		ToVkPipelineStageFlags(barrier.srcStageMask),
		ToVkPipelineStageFlags(barrier.dstStageMask),
		ToVkDependencyFlags(barrier.dependencyFlags),
		0, nullptr,
		0, nullptr,
		imageVewCount, m_Temp.imageMemoryBarriers.data());
}

void V3DCommandBuffer::FillBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pDstBuffer == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_FillBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer));
		return;
	}
#endif //_DEBUG

	vkCmdFillBuffer(m_Source.commandBuffer, static_cast<V3DBuffer*>(pDstBuffer)->GetSource().buffer, dstOffset, (size == 0)? VK_WHOLE_SIZE : size, data);
}

void V3DCommandBuffer::UpdateBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t dataSize, const void* pData)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstBuffer == nullptr) || (dataSize == 0) || (pData == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_UpdateBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer) << V3D_LOG_S_NUM_GREATER(dataSize, 0) << V3D_LOG_S_PTR(pData));
		return;
	}
#endif //_DEBUG

	VkBuffer vBuffer = static_cast<V3DBuffer*>(pDstBuffer)->GetSource().buffer;

	do
	{
		vkCmdUpdateBuffer(m_Source.commandBuffer, vBuffer, dstOffset, dataSize, pData);

		dstOffset += 65536;
		dataSize = (dataSize >= 65536) ? (dataSize - 65536) : 0;

	} while (dataSize > 0);
}

void V3DCommandBuffer::CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstBuffer == nullptr) || (pSrcBuffer == nullptr) || (size == 0))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer) << V3D_LOG_S_PTR(pSrcBuffer) << V3D_LOG_S_NUM_GREATER(size, 0));
		return;
	}
#endif //_DEBUG

	VkBufferCopy region;
	region.srcOffset = srcOffset;
	region.dstOffset = dstOffset;
	region.size = size;

	vkCmdCopyBuffer(m_Source.commandBuffer, static_cast<V3DBuffer*>(pSrcBuffer)->GetSource().buffer, static_cast<V3DBuffer*>(pDstBuffer)->GetSource().buffer, 1, &region);
}

void V3DCommandBuffer::CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstBuffer == nullptr) || (pSrcBuffer == nullptr) || (rangeCount == 0) || (pRanges == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer) << V3D_LOG_S_PTR(pSrcBuffer) << V3D_LOG_S_NUM(rangeCount) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	const V3DCopyBufferRange* pSrcRange = &pRanges[0];
	const V3DCopyBufferRange* pSrcRangeEnd = pSrcRange + rangeCount;

	m_Temp.bufferCopies.resize(rangeCount);
	VkBufferCopy* pDstCopy = m_Temp.bufferCopies.data();

	while (pSrcRange != pSrcRangeEnd)
	{
		pDstCopy->srcOffset = pSrcRange->srcOffset;
		pDstCopy->dstOffset = pSrcRange->dstOffset;
		pDstCopy->size = pSrcRange->size;

		pDstCopy++;
		pSrcRange++;
	}

	vkCmdCopyBuffer(
		m_Source.commandBuffer,
		static_cast<V3DBuffer*>(pSrcBuffer)->GetSource().buffer,
		static_cast<V3DBuffer*>(pDstBuffer)->GetSource().buffer,
		rangeCount, m_Temp.bufferCopies.data());
}

void V3DCommandBuffer::CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pDstImage == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcImage));
		return;
	}
#endif //_DEBUG

	const V3DImageDesc& imageDesc = pDstImage->GetDesc();

	m_Temp.imageCopies.resize(imageDesc.levelCount);
	VkImageCopy* pDstImageCopy = m_Temp.imageCopies.data();
	VkImageCopy* pDstImageCopyEnd = pDstImageCopy + imageDesc.levelCount;

	const IV3DImageBase::Source& srcSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();
	const IV3DImageBase::Source& dstSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();

	VkImageAspectFlags aspectMask = dstSource.aspectFlags;
	uint32_t levelCount = 0;

	while (pDstImageCopy != pDstImageCopyEnd)
	{
		pDstImageCopy->dstSubresource.aspectMask = aspectMask;
		pDstImageCopy->dstSubresource.mipLevel = levelCount;
		pDstImageCopy->dstSubresource.baseArrayLayer = 0;
		pDstImageCopy->dstSubresource.layerCount = imageDesc.layerCount;
		pDstImageCopy->dstOffset.x = 0;
		pDstImageCopy->dstOffset.y = 0;
		pDstImageCopy->dstOffset.z = 0;
		pDstImageCopy->srcSubresource.aspectMask = aspectMask;
		pDstImageCopy->srcSubresource.mipLevel = levelCount;
		pDstImageCopy->srcSubresource.baseArrayLayer = 0;
		pDstImageCopy->srcSubresource.layerCount = imageDesc.layerCount;
		pDstImageCopy->srcOffset.x = 0;
		pDstImageCopy->srcOffset.y = 0;
		pDstImageCopy->srcOffset.z = 0;
		pDstImageCopy->extent.width = imageDesc.width;
		pDstImageCopy->extent.height = imageDesc.height;
		pDstImageCopy->extent.depth = imageDesc.depth;

		levelCount++;
		pDstImageCopy++;
	}

	vkCmdCopyImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		levelCount, m_Temp.imageCopies.data());
}

void V3DCommandBuffer::CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DCopyImageRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pDstImage == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcImage) << V3D_LOG_S_NUM_GREATER(rangeCount, 0) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	const V3DCopyImageRange* pSrcRange = &pRanges[0];
	const V3DCopyImageRange* pSrcRangeEnd = pSrcRange + rangeCount;

	m_Temp.imageCopies.resize(rangeCount);
	VkImageCopy* pDstRange = m_Temp.imageCopies.data();

	const IV3DImageBase::Source& srcSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();
	const IV3DImageBase::Source& dstSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();

	while (pSrcRange != pSrcRangeEnd)
	{
		pDstRange->dstSubresource.aspectMask = srcSource.aspectFlags;
		pDstRange->dstSubresource.mipLevel = pSrcRange->dstSubresource.level;
		pDstRange->dstSubresource.baseArrayLayer = pSrcRange->dstSubresource.baseLayer;
		pDstRange->dstSubresource.layerCount = pSrcRange->dstSubresource.layerCount;
		pDstRange->dstOffset.x = pSrcRange->dstOffset.x;
		pDstRange->dstOffset.y = pSrcRange->dstOffset.y;
		pDstRange->dstOffset.z = pSrcRange->dstOffset.z;
		pDstRange->srcSubresource.aspectMask = srcSource.aspectFlags;
		pDstRange->srcSubresource.mipLevel = pSrcRange->srcSubresource.level;
		pDstRange->srcSubresource.baseArrayLayer = pSrcRange->srcSubresource.baseLayer;
		pDstRange->srcSubresource.layerCount = pSrcRange->srcSubresource.layerCount;
		pDstRange->srcOffset.x = pSrcRange->srcOffset.x;
		pDstRange->srcOffset.y = pSrcRange->srcOffset.y;
		pDstRange->srcOffset.z = pSrcRange->srcOffset.z;
		pDstRange->extent.width = pSrcRange->size.width;
		pDstRange->extent.height = pSrcRange->size.height;
		pDstRange->extent.depth = pSrcRange->size.depth;

		pDstRange++;
		pSrcRange++;
	}

	vkCmdCopyImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		rangeCount, m_Temp.imageCopies.data());
}

void V3DCommandBuffer::CopyBufferToImage(
	IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize,
	IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pSrcBuffer == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyBufferToImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcBuffer));
		return;
	}
#endif //_DEBUG

	const V3DBuffer::Source& srcBufferSource = static_cast<V3DBuffer*>(pSrcBuffer)->GetSource();
	const IV3DImageBase::Source& dstImageSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();

	VkBufferImageCopy range;
	range.bufferOffset = srcBufferOffset;
	range.bufferRowLength = 0;
	range.bufferImageHeight = 0;
	range.imageSubresource.aspectMask = dstImageSource.aspectFlags;
	range.imageSubresource.mipLevel = dstLevel;
	range.imageSubresource.baseArrayLayer = dstBaseLayer;
	range.imageSubresource.layerCount = dstLayerCount;
	range.imageOffset.x = dstOffset.x;
	range.imageOffset.y = dstOffset.y;
	range.imageOffset.z = dstOffset.z;
	range.imageExtent.width = dstSize.width;
	range.imageExtent.height = dstSize.height;
	range.imageExtent.depth = dstSize.depth;

	vkCmdCopyBufferToImage(
		m_Source.commandBuffer,
		srcBufferSource.buffer,
		dstImageSource.image,
		ToVkImageLayout(dstImageLayout),
		1, &range);
}

void V3DCommandBuffer::CopyBufferToImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pSrcBuffer == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyBufferToImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcBuffer) << V3D_LOG_S_NUM_GREATER(rangeCount, 0) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	m_Temp.bufferImageCopies.resize(rangeCount);
	VkBufferImageCopy* pDst = m_Temp.bufferImageCopies.data();

	const V3DCopyBufferToImageRange* pSrc = &pRanges[0];
	const V3DCopyBufferToImageRange* pSrcEnd = pSrc + rangeCount;

	const IV3DImageBase::Source& dstImageSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();
	const V3DBuffer::Source& srcBufferSource = static_cast<V3DBuffer*>(pSrcBuffer)->GetSource();

	while (pSrc != pSrcEnd)
	{
		pDst->bufferOffset = pSrc->srcBufferOffset;
		pDst->bufferRowLength = 0;
		pDst->bufferImageHeight = 0;
		pDst->imageSubresource.aspectMask = dstImageSource.aspectFlags;
		pDst->imageSubresource.mipLevel = pSrc->dstImageSubresource.level;
		pDst->imageSubresource.baseArrayLayer = pSrc->dstImageSubresource.baseLayer;
		pDst->imageSubresource.layerCount = pSrc->dstImageSubresource.layerCount;
		pDst->imageOffset.x = pSrc->dstImageOffset.x;
		pDst->imageOffset.y = pSrc->dstImageOffset.y;
		pDst->imageOffset.z = pSrc->dstImageOffset.z;
		pDst->imageExtent.width = pSrc->dstImageSize.width;
		pDst->imageExtent.height = pSrc->dstImageSize.height;
		pDst->imageExtent.depth = pSrc->dstImageSize.depth;

		pDst++;
		pSrc++;
	}

	vkCmdCopyBufferToImage(
		m_Source.commandBuffer,
		srcBufferSource.buffer,
		dstImageSource.image,
		ToVkImageLayout(dstImageLayout),
		rangeCount, m_Temp.bufferImageCopies.data());
}

void V3DCommandBuffer::CopyImageToBuffer(IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstBuffer == nullptr) || (pSrcImage == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyImageToBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer) << V3D_LOG_S_PTR(pSrcImage));
		return;
	}
#endif //_DEBUG

	const IV3DImageBase::Source& srcImageSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();
	const V3DBuffer::Source& dstBufferSource = static_cast<V3DBuffer*>(pDstBuffer)->GetSource();

	VkBufferImageCopy range;
	range.bufferOffset = dstBufferOffset;
	range.bufferRowLength = 0;
	range.bufferImageHeight = 0;
	range.imageSubresource.aspectMask = srcImageSource.aspectFlags;
	range.imageSubresource.mipLevel = srcLevel;
	range.imageSubresource.baseArrayLayer = srcBaseLayer;
	range.imageSubresource.layerCount = srcLayerCount;
	range.imageOffset.x = srcOffset.x;
	range.imageOffset.y = srcOffset.y;
	range.imageOffset.z = srcOffset.z;
	range.imageExtent.width = srcSize.width;
	range.imageExtent.height = srcSize.height;
	range.imageExtent.depth = srcSize.depth;

	vkCmdCopyImageToBuffer(
		m_Source.commandBuffer,
		srcImageSource.image, ToVkImageLayout(srcImageLayout),
		dstBufferSource.buffer,
		1, &range);
}

void V3DCommandBuffer::CopyImageToBuffer(IV3DBuffer* pDstBuffer, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstBuffer == nullptr) || (pSrcImage == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_CopyImageToBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstBuffer) << V3D_LOG_S_PTR(pSrcImage) << V3D_LOG_S_NUM_GREATER(rangeCount, 0) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	const V3DCopyImageToBufferRange* pSrc = &pRanges[0];
	const V3DCopyImageToBufferRange* pSrcEnd = pSrc + rangeCount;

	m_Temp.bufferImageCopies.resize(rangeCount);
	VkBufferImageCopy* pDst = m_Temp.bufferImageCopies.data();

	const IV3DImageBase::Source& srcImageSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();
	const V3DBuffer::Source& dstBufferSource = static_cast<V3DBuffer*>(pDstBuffer)->GetSource();

	while (pSrc != pSrcEnd)
	{
		pDst->bufferOffset = pSrc->dstBufferOffset;
		pDst->bufferRowLength = 0;
		pDst->bufferImageHeight = 0;
		pDst->imageSubresource.aspectMask = srcImageSource.aspectFlags;
		pDst->imageOffset.x = pSrc->srcImageOffset.x;
		pDst->imageOffset.y = pSrc->srcImageOffset.y;
		pDst->imageOffset.z = pSrc->srcImageOffset.z;
		pDst->imageExtent.width = pSrc->srcImageSize.width;
		pDst->imageExtent.height = pSrc->srcImageSize.height;
		pDst->imageExtent.depth = pSrc->srcImageSize.depth;

		pDst++;
		pSrc++;
	}

	vkCmdCopyImageToBuffer(
		m_Source.commandBuffer,
		srcImageSource.image, ToVkImageLayout(srcImageLayout),
		dstBufferSource.buffer,
		rangeCount, m_Temp.bufferImageCopies.data());
}

void V3DCommandBuffer::BlitImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, V3DBlitImageRange* pRanges, V3D_FILTER filter)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pDstImage == nullptr) || (rangeCount == 0) || (pRanges == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BlitImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcImage) << V3D_LOG_S_NUM_GREATER(rangeCount, 0) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	V3DBlitImageRange* pSrc = pRanges;
	V3DBlitImageRange* pSrcEnd = pSrc + rangeCount;

	m_Temp.imageBlits.resize(rangeCount);
	VkImageBlit* pDst = m_Temp.imageBlits.data();

	const IV3DImageBase::Source& dstImageSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();
	const IV3DImageBase::Source& srcImageSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();

	while (pSrc != pSrcEnd)
	{
		pDst->dstSubresource.aspectMask = srcImageSource.aspectFlags;
		pDst->dstSubresource.mipLevel = pSrc->dstSubresource.level;
		pDst->dstSubresource.baseArrayLayer = pSrc->dstSubresource.baseLayer;
		pDst->dstSubresource.layerCount = pSrc->dstSubresource.layerCount;
		pDst->dstOffsets[0].x = pSrc->dstRect.x;
		pDst->dstOffsets[0].y = pSrc->dstRect.y;
		pDst->dstOffsets[0].z = pSrc->dstRect.z;
		pDst->dstOffsets[1].x = pSrc->dstRect.x + pSrc->dstRect.width;
		pDst->dstOffsets[1].y = pSrc->dstRect.y + pSrc->dstRect.height;
		pDst->dstOffsets[1].z = pSrc->dstRect.z + pSrc->dstRect.depth;

		pDst->srcSubresource.aspectMask = srcImageSource.aspectFlags;
		pDst->srcSubresource.mipLevel = pSrc->srcSubresource.level;
		pDst->srcSubresource.baseArrayLayer = pSrc->srcSubresource.baseLayer;
		pDst->srcSubresource.layerCount = pSrc->srcSubresource.layerCount;
		pDst->srcOffsets[0].x = pSrc->srcRect.x;
		pDst->srcOffsets[0].y = pSrc->srcRect.y;
		pDst->srcOffsets[0].z = pSrc->srcRect.z;
		pDst->srcOffsets[1].x = pSrc->srcRect.x + pSrc->srcRect.width;
		pDst->srcOffsets[1].y = pSrc->srcRect.y + pSrc->srcRect.height;
		pDst->srcOffsets[1].z = pSrc->srcRect.z + pSrc->srcRect.depth;

		pDst++;
		pSrc++;
	}

	vkCmdBlitImage(
		m_Source.commandBuffer,
		srcImageSource.image, ToVkImageLayout(srcImageLayout),
		dstImageSource.image, ToVkImageLayout(dstImageLayout),
		rangeCount, m_Temp.imageBlits.data(),
		ToVkFilter(filter));
 }

void V3DCommandBuffer::BlitImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DRectangle2D& dstRect, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DRectangle2D& srcRect, V3D_FILTER filter)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImageView == nullptr) || (pSrcImageView == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BlitImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImageView) << V3D_LOG_S_PTR(pSrcImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& dstSource = static_cast<V3DImageView*>(pDstImageView)->GetSource();
	const V3DImageView::Source& srcSource = static_cast<V3DImageView*>(pSrcImageView)->GetSource();

	const VkImageSubresourceRange& dstSubresourceRange = dstSource.imageSubresourceRange;
	const VkImageSubresourceRange& srcSubresourceRange = srcSource.imageSubresourceRange;

	uint32_t dstX = dstRect.x;
	uint32_t dstY = dstRect.y;
	uint32_t dstWidth = dstRect.width;
	uint32_t dstHeight = dstRect.height;

	uint32_t srcX = srcRect.x;
	uint32_t srcY = srcRect.y;
	uint32_t srcWidth = srcRect.width;
	uint32_t srcHeight = srcRect.height;

	uint32_t depth = V3D_MIN(dstSource.extent.depth, srcSource.extent.depth);

	uint32_t levelOffset = 0;
	uint32_t layerCount = V3D_MIN(dstSubresourceRange.layerCount, srcSubresourceRange.layerCount);

	uint32_t blitCount = V3D_MIN(dstSubresourceRange.levelCount, srcSubresourceRange.levelCount);
	m_Temp.imageBlits.resize(blitCount);

	VkImageBlit* pDst = m_Temp.imageBlits.data();
	VkImageBlit* pDstEnd = pDst + blitCount;

	while (pDst != pDstEnd)
	{
		pDst->dstSubresource.aspectMask = dstSubresourceRange.aspectMask;
		pDst->dstSubresource.mipLevel = dstSubresourceRange.baseMipLevel + levelOffset;
		pDst->dstSubresource.baseArrayLayer = dstSubresourceRange.baseArrayLayer;
		pDst->dstSubresource.layerCount = layerCount;
		pDst->dstOffsets[0].x = dstX;
		pDst->dstOffsets[0].y = dstY;
		pDst->dstOffsets[0].z = 0;
		pDst->dstOffsets[1].x = dstX + dstWidth;
		pDst->dstOffsets[1].y = dstY + dstHeight;
		pDst->dstOffsets[1].z = depth;

		pDst->srcSubresource.aspectMask = srcSubresourceRange.aspectMask;
		pDst->srcSubresource.mipLevel = srcSubresourceRange.baseMipLevel + levelOffset;
		pDst->srcSubresource.baseArrayLayer = srcSubresourceRange.baseArrayLayer;
		pDst->srcSubresource.layerCount =layerCount;
		pDst->srcOffsets[0].x = srcX;
		pDst->srcOffsets[0].y = srcY;
		pDst->srcOffsets[0].z = 0;
		pDst->srcOffsets[1].x = srcX + srcWidth;
		pDst->srcOffsets[1].y = srcY + srcHeight;
		pDst->srcOffsets[1].z = depth;

		pDst++;

		levelOffset++;

		srcX = srcX >> 1;
		srcY = srcY >> 1;
		srcWidth = V3D_MAX(1, srcWidth >> 1);
		srcHeight = V3D_MAX(1, srcHeight >> 1);

		dstX = dstX >> 1;
		dstY = dstY >> 1;
		dstWidth = V3D_MAX(1, dstWidth >> 1);
		dstHeight = V3D_MAX(1, dstHeight >> 1);
	}

	vkCmdBlitImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		blitCount, m_Temp.imageBlits.data(),
		ToVkFilter(filter));
}

void V3DCommandBuffer::BlitImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, V3D_FILTER filter)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImageView == nullptr) || (pSrcImageView == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BlitImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImageView) << V3D_LOG_S_PTR(pSrcImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& dstSource = static_cast<V3DImageView*>(pDstImageView)->GetSource();
	const V3DImageView::Source& srcSource = static_cast<V3DImageView*>(pSrcImageView)->GetSource();

	const VkImageSubresourceRange& dstSubresourceRange = dstSource.imageSubresourceRange;
	const VkImageSubresourceRange& srcSubresourceRange = srcSource.imageSubresourceRange;

	uint32_t dstWidth = dstSource.extent.width;
	uint32_t dstHeight = dstSource.extent.height;

	uint32_t srcWidth = srcSource.extent.width;
	uint32_t srcHeight = srcSource.extent.height;

	uint32_t depth = V3D_MIN(dstSource.extent.depth, srcSource.extent.depth);

	uint32_t levelOffset = 0;
	uint32_t layerCount = V3D_MIN(dstSubresourceRange.layerCount, srcSubresourceRange.layerCount);

	uint32_t blitCount = V3D_MIN(dstSubresourceRange.levelCount, srcSubresourceRange.levelCount);
	m_Temp.imageBlits.resize(blitCount);

	VkImageBlit* pDst = m_Temp.imageBlits.data();
	VkImageBlit* pDstEnd = pDst + blitCount;

	while (pDst != pDstEnd)
	{
		pDst->dstSubresource.aspectMask = dstSubresourceRange.aspectMask;
		pDst->dstSubresource.mipLevel = dstSubresourceRange.baseMipLevel + levelOffset;
		pDst->dstSubresource.baseArrayLayer = dstSubresourceRange.baseArrayLayer;
		pDst->dstSubresource.layerCount = layerCount;
		pDst->dstOffsets[0].x = 0;
		pDst->dstOffsets[0].y = 0;
		pDst->dstOffsets[0].z = 0;
		pDst->dstOffsets[1].x = dstWidth;
		pDst->dstOffsets[1].y = dstHeight;
		pDst->dstOffsets[1].z = depth;

		pDst->srcSubresource.aspectMask = srcSubresourceRange.aspectMask;
		pDst->srcSubresource.mipLevel = srcSubresourceRange.baseMipLevel + levelOffset;
		pDst->srcSubresource.baseArrayLayer = srcSubresourceRange.baseArrayLayer;
		pDst->srcSubresource.layerCount = layerCount;
		pDst->srcOffsets[0].x = 0;
		pDst->srcOffsets[0].y = 0;
		pDst->srcOffsets[0].z = 0;
		pDst->srcOffsets[1].x = srcWidth;
		pDst->srcOffsets[1].y = srcHeight;
		pDst->srcOffsets[1].z = depth;

		pDst++;

		levelOffset++;

		srcWidth = V3D_MAX(1, srcWidth >> 1);
		srcHeight = V3D_MAX(1, srcHeight >> 1);

		dstWidth = V3D_MAX(1, dstWidth >> 1);
		dstHeight = V3D_MAX(1, dstHeight >> 1);
	}

	vkCmdBlitImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		blitCount, m_Temp.imageBlits.data(),
		ToVkFilter(filter));
}

void V3DCommandBuffer::ResolveImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DResolveImageRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImage == nullptr) || (pSrcImage == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ResolveImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImage) << V3D_LOG_S_PTR(pSrcImage) << V3D_LOG_S_NUM_GREATER(rangeCount, 0) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	m_Temp.imageResolves.resize(rangeCount);
	VkImageResolve* pDstRange = m_Temp.imageResolves.data();

	const V3DResolveImageRange* pSrcRange = &pRanges[0];
	const V3DResolveImageRange* pSrcRangeEnd = pSrcRange + rangeCount;

	const IV3DImageBase::Source& dstImageSource = static_cast<IV3DImageBase*>(pDstImage)->GetSource();
	const IV3DImageBase::Source& srcImageSource = static_cast<IV3DImageBase*>(pSrcImage)->GetSource();

	while (pSrcRange != pSrcRangeEnd)
	{
		pDstRange->dstSubresource.aspectMask = dstImageSource.aspectFlags;
		pDstRange->dstSubresource.mipLevel = pSrcRange->dstSubresource.level;
		pDstRange->dstSubresource.baseArrayLayer = pSrcRange->dstSubresource.baseLayer;
		pDstRange->dstSubresource.layerCount = pSrcRange->dstSubresource.layerCount;
		pDstRange->dstOffset.x = pSrcRange->dstOffset.x;
		pDstRange->dstOffset.y = pSrcRange->dstOffset.y;
		pDstRange->dstOffset.z = pSrcRange->dstOffset.z;
		pDstRange->srcSubresource.aspectMask = srcImageSource.aspectFlags;
		pDstRange->srcSubresource.mipLevel = pSrcRange->srcSubresource.level;
		pDstRange->srcSubresource.baseArrayLayer = pSrcRange->srcSubresource.baseLayer;
		pDstRange->srcSubresource.layerCount = pSrcRange->srcSubresource.layerCount;
		pDstRange->srcOffset.x = pSrcRange->srcOffset.x;
		pDstRange->srcOffset.y = pSrcRange->srcOffset.y;
		pDstRange->srcOffset.z = pSrcRange->srcOffset.z;
		pDstRange->extent.width = pSrcRange->size.width;
		pDstRange->extent.height = pSrcRange->size.height;
		pDstRange->extent.depth = pSrcRange->size.depth;

		pDstRange++;
		pSrcRange++;
	}

	vkCmdResolveImage(
		m_Source.commandBuffer,
		srcImageSource.image, ToVkImageLayout(srcImageLayout),
		dstImageSource.image, ToVkImageLayout(dstImageLayout),
		rangeCount, m_Temp.imageResolves.data());
}

void V3DCommandBuffer::ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DPoint2D& dstOffset, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DPoint2D& srcOffset, const V3DSize2D& size)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImageView == pSrcImageView) || (pDstImageView == nullptr) || (pSrcImageView == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ResolveImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImageView) << V3D_LOG_S_PTR(pSrcImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& dstSource = static_cast<V3DImageView*>(pDstImageView)->GetSource();
	const V3DImageView::Source& srcSource = static_cast<V3DImageView*>(pSrcImageView)->GetSource();

	const VkImageSubresourceRange& dstSubresourceRange = dstSource.imageSubresourceRange;
	const VkImageSubresourceRange& srcSubresourceRange = srcSource.imageSubresourceRange;

	uint32_t dstX = dstOffset.x;
	uint32_t dstY = dstOffset.y;

	uint32_t srcX = srcOffset.x;
	uint32_t srcY = srcOffset.y;

	uint32_t width = srcSource.extent.width;
	uint32_t height = srcSource.extent.height;
	uint32_t depth = V3D_MIN(dstSource.extent.depth, srcSource.extent.depth);

	uint32_t levelOffset = 0;
	uint32_t layerCount = V3D_MIN(dstSubresourceRange.layerCount, srcSubresourceRange.layerCount);

	uint32_t resolveCount = V3D_MIN(dstSource.imageSubresourceRange.levelCount, srcSource.imageSubresourceRange.levelCount);
	m_Temp.imageResolves.resize(resolveCount);

	VkImageResolve* pDst = m_Temp.imageResolves.data();
	VkImageResolve* pDstEnd = pDst + resolveCount;

	while (pDst != pDstEnd)
	{
		pDst->dstSubresource.aspectMask = dstSubresourceRange.aspectMask;
		pDst->dstSubresource.mipLevel = dstSubresourceRange.baseMipLevel + levelOffset;
		pDst->dstSubresource.baseArrayLayer = dstSubresourceRange.baseArrayLayer;
		pDst->dstSubresource.layerCount = layerCount;
		pDst->dstOffset.x = dstX;
		pDst->dstOffset.y = dstY;
		pDst->dstOffset.z = 0;

		pDst->srcSubresource.aspectMask = srcSubresourceRange.aspectMask;
		pDst->srcSubresource.mipLevel = srcSubresourceRange.baseMipLevel + levelOffset;
		pDst->srcSubresource.baseArrayLayer = srcSubresourceRange.baseArrayLayer;
		pDst->srcSubresource.layerCount = layerCount;
		pDst->srcOffset.x = srcX;
		pDst->srcOffset.y = srcY;
		pDst->srcOffset.z = 0;

		pDst->extent.width = width;
		pDst->extent.height = height;
		pDst->extent.depth = depth;

		pDst++;

		levelOffset++;

		width = V3D_MAX(1, width >> 1);
		height = V3D_MAX(1, height >> 1);

		srcX = srcX >> 1;
		srcY = srcY >> 1;

		dstX = dstX >> 1;
		dstY = dstY >> 1;
	}

	vkCmdResolveImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		resolveCount, m_Temp.imageResolves.data());
}

void V3DCommandBuffer::ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pDstImageView == pSrcImageView) || (pDstImageView == nullptr) || (pSrcImageView == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ResolveImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pDstImageView) << V3D_LOG_S_PTR(pSrcImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& dstSource = static_cast<V3DImageView*>(pDstImageView)->GetSource();
	const V3DImageView::Source& srcSource = static_cast<V3DImageView*>(pSrcImageView)->GetSource();

	const VkImageSubresourceRange& dstSubresourceRange = dstSource.imageSubresourceRange;
	const VkImageSubresourceRange& srcSubresourceRange = srcSource.imageSubresourceRange;

	uint32_t width = srcSource.extent.width;
	uint32_t height = srcSource.extent.height;
	uint32_t depth = srcSource.extent.depth;

	uint32_t levelOffset = 0;
	uint32_t layerCount = V3D_MIN(dstSubresourceRange.layerCount, srcSubresourceRange.layerCount);

	uint32_t resolveCount = V3D_MIN(dstSubresourceRange.levelCount, srcSubresourceRange.levelCount);
	m_Temp.imageResolves.resize(resolveCount);

	VkImageResolve* pDst = m_Temp.imageResolves.data();
	VkImageResolve* pDstEnd = pDst + resolveCount;

	while (pDst != pDstEnd)
	{
		pDst->dstSubresource.aspectMask = dstSubresourceRange.aspectMask;
		pDst->dstSubresource.mipLevel = dstSubresourceRange.baseMipLevel + levelOffset;
		pDst->dstSubresource.baseArrayLayer = dstSubresourceRange.baseArrayLayer;
		pDst->dstSubresource.layerCount = layerCount;
		pDst->dstOffset.x = 0;
		pDst->dstOffset.y = 0;
		pDst->dstOffset.z = 0;

		pDst->srcSubresource.aspectMask = srcSubresourceRange.aspectMask;
		pDst->srcSubresource.mipLevel = srcSubresourceRange.baseMipLevel + levelOffset;
		pDst->srcSubresource.baseArrayLayer = srcSubresourceRange.baseArrayLayer;
		pDst->srcSubresource.layerCount = layerCount;
		pDst->srcOffset.x = 0;
		pDst->srcOffset.y = 0;
		pDst->srcOffset.z = 0;

		pDst->extent.width = width;
		pDst->extent.height = height;
		pDst->extent.depth = depth;

		pDst++;

		levelOffset++;

		width = V3D_MAX(1, width >> 1);
		height = V3D_MAX(1, height >> 1);
	}

	vkCmdResolveImage(
		m_Source.commandBuffer,
		srcSource.image, ToVkImageLayout(srcImageLayout),
		dstSource.image, ToVkImageLayout(dstImageLayout),
		resolveCount, m_Temp.imageResolves.data());
}

void V3DCommandBuffer::BeginRenderPass(IV3DRenderPass* pRenderPass, IV3DFrameBuffer* pFrameBuffer, bool subpassContentInline, const V3DRectangle2D* pRenderArea)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pRenderPass == nullptr) || (pFrameBuffer == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BeginRenderPass << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pRenderPass) << V3D_LOG_S_PTR(pFrameBuffer));
		return;
	}
#endif //_DEBUG

	V3DRenderPass* pInternalRenderPass = static_cast<V3DRenderPass*>(pRenderPass);
	const V3DRenderPass::Source& renserPassSource = pInternalRenderPass->GetSource();

	V3DFrameBuffer* pInternalFrameBuffer = static_cast<V3DFrameBuffer*>(pFrameBuffer);
	const V3DFrameBuffer::Source& frameBufferSource = pInternalFrameBuffer->GetSource();

	VkRenderPassBeginInfo info{};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.pNext = nullptr;
	info.renderPass = renserPassSource.renderPass;
	info.framebuffer = frameBufferSource.framebuffer;

	if (pRenderArea != nullptr)
	{
		info.renderArea.offset.x = pRenderArea->x;
		info.renderArea.offset.y = pRenderArea->y;
		info.renderArea.extent.width = pRenderArea->width;
		info.renderArea.extent.height = pRenderArea->height;
	}
	else
	{
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		info.renderArea.extent = frameBufferSource.extent;
	}

	info.clearValueCount = renserPassSource.clearValueCount;
	info.pClearValues = renserPassSource.pClearValues;

	vkCmdBeginRenderPass(m_Source.commandBuffer, &info, (subpassContentInline == true)? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
}

void V3DCommandBuffer::EndRenderPass()
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	vkCmdEndRenderPass(m_Source.commandBuffer);
}

void V3DCommandBuffer::NextSubpass()
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	vkCmdNextSubpass(m_Source.commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
}

void V3DCommandBuffer::ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pImage == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ClearImage << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pImage));
		return;
	}
#endif //_DEBUG

	const IV3DImageBase::Source& source = static_cast<IV3DImageBase*>(pImage)->GetSource();
	const V3DImageDesc& desc = pImage->GetDesc();

	VkImageSubresourceRange vkSubresourceRange{};
	vkSubresourceRange.aspectMask = source.aspectFlags;
	vkSubresourceRange.baseMipLevel = 0;
	vkSubresourceRange.levelCount = desc.levelCount;
	vkSubresourceRange.baseArrayLayer = 0;
	vkSubresourceRange.layerCount = desc.layerCount;

	if (vkSubresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		VkClearColorValue vkClearValue;
		vkClearValue.uint32[0] = clearValue.color.uint32[0];
		vkClearValue.uint32[1] = clearValue.color.uint32[1];
		vkClearValue.uint32[2] = clearValue.color.uint32[2];
		vkClearValue.uint32[3] = clearValue.color.uint32[3];

		vkCmdClearColorImage(m_Source.commandBuffer, source.image, ToVkImageLayout(imageLayout), &vkClearValue, 1, &vkSubresourceRange);
	}
	else
	{
		VkClearDepthStencilValue vkClearValue;
		vkClearValue.depth = clearValue.depthStencil.depth;
		vkClearValue.stencil = clearValue.depthStencil.stencil;

		vkCmdClearDepthStencilImage(m_Source.commandBuffer, source.image, ToVkImageLayout(imageLayout), &vkClearValue, 1, &vkSubresourceRange);
	}
}

void V3DCommandBuffer::ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pImageView == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ClearImageView << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pImageView));
		return;
	}
#endif //_DEBUG

	const V3DImageView::Source& source = static_cast<V3DImageView*>(pImageView)->GetSource();

	if (source.imageSubresourceRange.aspectMask == VK_IMAGE_ASPECT_COLOR_BIT)
	{
		VkClearColorValue vkClearValue;
		vkClearValue.uint32[0] = clearValue.color.uint32[0];
		vkClearValue.uint32[1] = clearValue.color.uint32[1];
		vkClearValue.uint32[2] = clearValue.color.uint32[2];
		vkClearValue.uint32[3] = clearValue.color.uint32[3];

		vkCmdClearColorImage(m_Source.commandBuffer, source.image, ToVkImageLayout(imageLayout), &vkClearValue, 1, &source.imageSubresourceRange);
	}
	else
	{
		VkClearDepthStencilValue vkClearValue;
		vkClearValue.depth = clearValue.depthStencil.depth;
		vkClearValue.stencil = clearValue.depthStencil.stencil;

		vkCmdClearDepthStencilImage(m_Source.commandBuffer, source.image, ToVkImageLayout(imageLayout), &vkClearValue, 1, &source.imageSubresourceRange);
	}
}

void V3DCommandBuffer::ClearAttachments(uint32_t colorAttachmentCount, const V3DClearColorAttachmentDesc* pColorAttachments, const V3DClearDepthStencilAttachmentDesc* pDepthStencilAttachment, uint32_t rangeCount, const V3DClearRange* pRanges)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	uint32_t attachmentCount = colorAttachmentCount + ((pDepthStencilAttachment != nullptr) ? 1 : 0);

#ifdef _DEBUG
	if ((attachmentCount == 0) || (rangeCount == 0 ) || (pRanges == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ClearAttachments << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM(rangeCount) << V3D_LOG_S_PTR(pRanges));
		return;
	}
#endif //_DEBUG

	m_Temp.clearAttachments.resize(attachmentCount);
	VkClearAttachment* pDstClearAttachment = m_Temp.clearAttachments.data();

	if (colorAttachmentCount > 0)
	{
		const V3DClearColorAttachmentDesc* pSrcColorAttachment = &pColorAttachments[0];
		const V3DClearColorAttachmentDesc* pSrcColorAttachmentEnd = pSrcColorAttachment + colorAttachmentCount;

		while (pSrcColorAttachment != pSrcColorAttachmentEnd)
		{
			pDstClearAttachment->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			pDstClearAttachment->colorAttachment = pSrcColorAttachment->attachment;
			memcpy_s(&pDstClearAttachment->clearValue.color, sizeof(VkClearColorValue), &pSrcColorAttachment->value, sizeof(V3DClearColorValue));

			pDstClearAttachment++;
			pSrcColorAttachment++;
		}
	}

	if (pDepthStencilAttachment != nullptr)
	{
		pDstClearAttachment->aspectMask = 0;
		if (pDepthStencilAttachment->depthEnable) { pDstClearAttachment->aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT; }
		if (pDepthStencilAttachment->stencilEnable) { pDstClearAttachment->aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT; }

		pDstClearAttachment->colorAttachment = 0;

		pDstClearAttachment->clearValue.depthStencil.depth = pDepthStencilAttachment->value.depth;
		pDstClearAttachment->clearValue.depthStencil.stencil = pDepthStencilAttachment->value.stencil;
	}

	// ----------------------------------------------------------------------------------------------------

	m_Temp.clearRects.resize(rangeCount);
	VkClearRect* pDstRect = m_Temp.clearRects.data();

	const V3DClearRange* pSrcRange = &pRanges[0];
	const V3DClearRange* pSrcRangeEnd = pSrcRange + rangeCount;

	while (pSrcRange != pSrcRangeEnd)
	{
		pDstRect->rect.offset.x = pSrcRange->rect.x;
		pDstRect->rect.offset.y = pSrcRange->rect.y;
		pDstRect->rect.extent.width = pSrcRange->rect.width;
		pDstRect->rect.extent.height = pSrcRange->rect.height;
		pDstRect->baseArrayLayer = pSrcRange->baseLayer;
		pDstRect->layerCount = pSrcRange->layerCount;

		pDstRect++;
		pSrcRange++;
	}

	// ----------------------------------------------------------------------------------------------------

	vkCmdClearAttachments(m_Source.commandBuffer, attachmentCount, m_Temp.clearAttachments.data(), rangeCount, m_Temp.clearRects.data());
}

void V3DCommandBuffer::BindPipeline(IV3DPipeline* pPipeline)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pPipeline == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BindPipeline << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pPipeline));
		return;
	}
#endif //_DEBUG

	IV3DPipelineBase* pPipelineBase = static_cast<IV3DPipelineBase*>(pPipeline);
	const IV3DPipelineBase::Source& source = pPipelineBase->GetSource();

	vkCmdBindPipeline(m_Source.commandBuffer, source.pipelineBindPoint, source.pipeline);
}

void V3DCommandBuffer::BindDescriptorSets(V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout, uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pPipelineLayout == nullptr) || (descriptorSetCount == 0) || (ppDescriptorSets == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BindDescriptorSets << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pPipelineLayout) << V3D_LOG_S_NUM_GREATER(descriptorSetCount, 0) << V3D_LOG_S_PTR(ppDescriptorSets));
		return;
	}

	uint32_t debugErrorCount = 0;

	for (uint32_t i = 0; i < descriptorSetCount; i++)
	{
		if (ppDescriptorSets[i]->GetType() != V3D_DESCRIPTOR_SET_TYPE_STANDARD)
		{
			V3D_LOG_ERROR(Log_Error_NotStandardDescriptorSet, V3D_LOG_TYPE(ppDescriptorSets), i);
			debugErrorCount++;
		}
	}

	if (debugErrorCount > 0)
	{
		return;
	}
#endif //_DEBUG

	m_Temp.descriptorSets.resize(descriptorSetCount);
	VkDescriptorSet* pDstDescriptorSet = m_Temp.descriptorSets.data();

	V3DStandardDescriptorSet** ppSrcDescriptorSet = reinterpret_cast<V3DStandardDescriptorSet**>(ppDescriptorSets);
	V3DStandardDescriptorSet** ppSrcDescriptorSetEnd = ppSrcDescriptorSet + descriptorSetCount;

	while (ppSrcDescriptorSet != ppSrcDescriptorSetEnd)
	{
		*pDstDescriptorSet++ = (*ppSrcDescriptorSet++)->GetSource().descriptorSet;
	}

	vkCmdBindDescriptorSets(
		m_Source.commandBuffer,
		ToVkPipelineBindPoint(pipelineType),
		static_cast<V3DPipelineLayout*>(pPipelineLayout)->GetSource().pipelineLayout,
		firstSet, descriptorSetCount, m_Temp.descriptorSets.data(),
		dynamicOffsetCount, pDynamicOffsets);
}

void V3DCommandBuffer::BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, IV3DBuffer** ppBuffers, const uint64_t* pOffsets)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((bindingCount == 0) || (ppBuffers == nullptr) || (pOffsets == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BindVertexBuffers << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument <<  V3D_LOG_S_NUM_GREATER(bindingCount, 0) << V3D_LOG_S_PTR(ppBuffers) << V3D_LOG_S_PTR(pOffsets));
		return;
	}
#endif //_DEBUG

	m_Temp.buffers.resize(bindingCount);
	VkBuffer* pDstBuffer = m_Temp.buffers.data();

	V3DBuffer** ppSrcBuffer = reinterpret_cast<V3DBuffer**>(ppBuffers);
	V3DBuffer** ppSrcBufferEnd = ppSrcBuffer + bindingCount;

	while (ppSrcBuffer != ppSrcBufferEnd)
	{
		*pDstBuffer++ = (*ppSrcBuffer++)->GetSource().buffer;
	}

	vkCmdBindVertexBuffers(m_Source.commandBuffer, firstBinding, bindingCount, m_Temp.buffers.data(), pOffsets);
}

void V3DCommandBuffer::BindIndexBuffer(IV3DBuffer* pBuffer, uint64_t offset, V3D_INDEX_TYPE indexType)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pBuffer == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BindIndexBuffer << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pBuffer));
		return;
	}
#endif //_DEBUG

	vkCmdBindIndexBuffer(m_Source.commandBuffer, static_cast<V3DBuffer*>(pBuffer)->GetSource().buffer, offset, ToVkIndexType(indexType));
}

void V3DCommandBuffer::PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pPipelineLayout == nullptr) || (pData == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_PushConstant << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pPipelineLayout) << V3D_LOG_S_PTR(pData));
		return;
	}

	if (pPipelineLayout->GetConstantCount() <= slot)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_PushConstant << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(slot, pPipelineLayout->GetConstantCount()));
	}
#endif //_DEBUG

	const V3DConstantDesc& constantDesc = pPipelineLayout->GetConstantDesc(slot);

	vkCmdPushConstants(m_Source.commandBuffer, static_cast<V3DPipelineLayout*>(pPipelineLayout)->GetSource().pipelineLayout, ToVkShaderStageFlags(constantDesc.shaderStageFlags), constantDesc.offset, constantDesc.size, pData);
}

void V3DCommandBuffer::PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pPipelineLayout == nullptr) || (size == 0) || (pData == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_PushConstant << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pPipelineLayout) << V3D_LOG_S_NUM(size) << V3D_LOG_S_PTR(pData));
		return;
	}
#endif //_DEBUG

	const V3DConstantDesc& constantDesc = pPipelineLayout->GetConstantDesc(slot);

#ifdef _DEBUG
	if ((pPipelineLayout->GetConstantCount() <= slot) || (constantDesc.size < (offset + size)))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_PushConstant << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(slot, pPipelineLayout->GetConstantCount()) << V3D_LOG_S_NUM_LESS_EQUAL((offset + size), constantDesc.size));
		return;
	}
#endif //_DEBUG

	vkCmdPushConstants(m_Source.commandBuffer, static_cast<V3DPipelineLayout*>(pPipelineLayout)->GetSource().pipelineLayout, ToVkShaderStageFlags(constantDesc.shaderStageFlags), constantDesc.offset + offset, size, pData);
}

void V3DCommandBuffer::PushDescriptorSets(V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout, uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
	if (m_pPushDescriptorSetFunction == nullptr)
	{
		V3D_LOG_ERROR(Log_Error_UnavailablePushDescriptorSets, m_DebugName.c_str());
		return;
	}

#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((pPipelineLayout == nullptr) || (descriptorSetCount == 0) || (ppDescriptorSets == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_PushDescriptorSets << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pPipelineLayout) << V3D_LOG_S_PTR(ppDescriptorSets));
		return;
	}

	uint32_t debugErrorCount = 0;

	for (uint32_t i = 0; i < descriptorSetCount; i++)
	{
		if (ppDescriptorSets[i]->GetType() != V3D_DESCRIPTOR_SET_TYPE_PUSH)
		{
			V3D_LOG_ERROR(Log_Error_NotPushDescriptorSet, V3D_LOG_TYPE(ppDescriptorSets), i);
			debugErrorCount++;
		}
	}

	if (debugErrorCount > 0)
	{
		return;
	}
#endif //_DEBUG

	VkPipelineBindPoint vkPipelineBindPoint = ToVkPipelineBindPoint(pipelineType);
	VkPipelineLayout vkPipelineLayout = static_cast<V3DPipelineLayout*>(pPipelineLayout)->GetSource().pipelineLayout;

	V3DPushDescriptorSet** ppSrcDescriptorSet = reinterpret_cast<V3DPushDescriptorSet**>(ppDescriptorSets);
	V3DPushDescriptorSet** ppSrcDescriptorSetEnd = ppSrcDescriptorSet + descriptorSetCount;

	uint32_t setIndex = firstSet;

	const VkWriteDescriptorSet* pSrcWrite;
	const VkWriteDescriptorSet* pSrcWriteEnd;

	VkDescriptorBufferInfo* pDstBufferInfoBegin = m_Temp.descriptorBufferInfos;
	VkWriteDescriptorSet* pDstWriteBegin = m_Temp.writeDescriptors;

	VkDescriptorBufferInfo* pDstBufferInfo;
	VkWriteDescriptorSet* pDstWrite;

	uint64_t dynamicBufferBit;

	while (ppSrcDescriptorSet != ppSrcDescriptorSetEnd)
	{
		const V3DBaseDescriptorSet::Source& srcSetSource = (*ppSrcDescriptorSet)->GetSource();
		const uint64_t& dynamicBufferMask = srcSetSource.dynamicBufferMask;

		pSrcWrite = srcSetSource.pWriteDescriptors;
		pSrcWriteEnd = pSrcWrite + srcSetSource.descriptorCount;

		pDstBufferInfo = pDstBufferInfoBegin;
		pDstWrite = pDstWriteBegin;

		dynamicBufferBit = 0x1;

		while (pSrcWrite != pSrcWriteEnd)
		{
			*pDstWrite = *pSrcWrite;

			if (dynamicBufferMask & dynamicBufferBit)
			{
				*pDstBufferInfo = *pSrcWrite->pBufferInfo;
				pDstBufferInfo->offset = *pDynamicOffsets++;

				pDstWrite->pBufferInfo = pDstBufferInfo++;
			}

			dynamicBufferBit <<= 1;

			pDstWrite++;
			pSrcWrite++;
		}

		m_pPushDescriptorSetFunction(
			m_Source.commandBuffer,
			vkPipelineBindPoint, vkPipelineLayout,
			setIndex, srcSetSource.descriptorCount, pDstWriteBegin);

		setIndex++;
		ppSrcDescriptorSet++;
	}
}

void V3DCommandBuffer::SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((viewportCount == 0) || (pViewports == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_SetViewport << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(viewportCount, 0) << V3D_LOG_S_PTR(pViewports));
		return;
	}
#endif //_DEBUG

	m_Temp.viewports.resize(viewportCount);
	VkViewport* pDstViewport = m_Temp.viewports.data();

	const V3DViewport* pSrcViewport = &pViewports[0];
	const V3DViewport* pSrcViewportEnd = pSrcViewport + viewportCount;

	while (pSrcViewport != pSrcViewportEnd)
	{
		pDstViewport->x = static_cast<float>(pSrcViewport->rect.x);
		pDstViewport->y = static_cast<float>(pSrcViewport->rect.y);
		pDstViewport->width = static_cast<float>(pSrcViewport->rect.width);
		pDstViewport->height = static_cast<float>(pSrcViewport->rect.height);
		pDstViewport->minDepth = pSrcViewport->minDepth;
		pDstViewport->maxDepth = pSrcViewport->maxDepth;

		pDstViewport++;
		pSrcViewport++;
	}

	vkCmdSetViewport(m_Source.commandBuffer, firstViewport, viewportCount, m_Temp.viewports.data());
}

void V3DCommandBuffer::SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((scissorCount == 0) || (pScissors == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_SetScissor << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(scissorCount, 0) << V3D_LOG_S_PTR(pScissors));
		return;
	}
#endif //__DEBUG

	m_Temp.scissors.resize(scissorCount);
	VkRect2D* pDstScissor = m_Temp.scissors.data();

	const V3DRectangle2D* pSrcScissor = &pScissors[0];
	const V3DRectangle2D* pSrcScissorEnd = pSrcScissor + scissorCount;

	while (pSrcScissor != pSrcScissorEnd)
	{
		pDstScissor->offset.x = static_cast<int32_t>(pSrcScissor->x);
		pDstScissor->offset.y = static_cast<int32_t>(pSrcScissor->y);
		pDstScissor->extent.width = pSrcScissor->width;
		pDstScissor->extent.height = pSrcScissor->height;

		pDstScissor++;
		pSrcScissor++;
	}

	vkCmdSetScissor(m_Source.commandBuffer, firstScissor, scissorCount, m_Temp.scissors.data());
}

void V3DCommandBuffer::SetBlendConstants(const float blendConstants[4])
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	vkCmdSetBlendConstants(m_Source.commandBuffer, blendConstants);
}

void V3DCommandBuffer::SetStencilReference(V3DFlags faceMask, uint32_t reference)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	vkCmdSetStencilReference(m_Source.commandBuffer, ToVkStencilFaceFlags(faceMask), reference);
}

void V3DCommandBuffer::ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pQueryPool == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ResetQueryPool << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pQueryPool));
		return;
	}

	const V3DQueryPoolDesc& desc = pQueryPool->GetDesc();
	if ((desc.queryCount <= firstQuery) || (desc.queryCount < (firstQuery + queryCount)))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ResetQueryPool << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(firstQuery, desc.queryCount) << V3D_LOG_S_NUM_LESS_EQUAL((firstQuery + queryCount), desc.queryCount));
		return;
	}
#endif //_DEBUG

	vkCmdResetQueryPool(m_Source.commandBuffer, static_cast<V3DQueryPool*>(pQueryPool)->GetSource().queryPool, firstQuery, queryCount);
}

void V3DCommandBuffer::BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pQueryPool == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BeginQuery << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pQueryPool));
		return;
	}

	if (pQueryPool->GetDesc().queryCount <= query)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BeginQuery << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(query, pQueryPool->GetDesc().queryCount));
		return;
	}
#endif //_DEBUG

	const V3DQueryPool::Source& queryPoolSource = static_cast<V3DQueryPool*>(pQueryPool)->GetSource();

	vkCmdBeginQuery(m_Source.commandBuffer, queryPoolSource.queryPool, query, queryPoolSource.queryControlFlags);
}

void V3DCommandBuffer::EndQuery(IV3DQueryPool* pQueryPool, uint32_t query)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pQueryPool == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_EndQuery << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pQueryPool));
		return;
	}

	if (pQueryPool->GetDesc().queryCount <= query)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_EndQuery << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(query, pQueryPool->GetDesc().queryCount));
		return;
	}
#endif //_DEBUG

	vkCmdEndQuery(m_Source.commandBuffer, static_cast<V3DQueryPool*>(pQueryPool)->GetSource().queryPool, query);
}

void V3DCommandBuffer::WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pQueryPool == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_WriteTimestamp << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pQueryPool));
		return;
	}

	const V3DQueryPoolDesc& desc = pQueryPool->GetDesc();

	if (desc.queryCount <= query)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_WriteTimestamp << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_LESS(query, desc.queryCount));
		return;
	}
#endif //_DEBUG

	vkCmdWriteTimestamp(m_Source.commandBuffer, ToVkPipelineStageFlagBits(pipelineStage), static_cast<V3DQueryPool*>(pQueryPool)->GetSource().queryPool, query);
}

void V3DCommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((vertexCount == 0) || (instanceCount == 0))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_Draw << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(vertexCount, 0) << V3D_LOG_S_NUM_GREATER(instanceCount, 0));
		return;
	}
#endif //_DEBUG

	vkCmdDraw(m_Source.commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void V3DCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((indexCount == 0) || (instanceCount == 0))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_DrawIndexed << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(indexCount, 0) << V3D_LOG_S_NUM_GREATER(instanceCount, 0));
		return;
	}
#endif //_DEBUG

	vkCmdDrawIndexed(m_Source.commandBuffer, indexCount, instanceCount, firstIndex, firstInstance, vertexOffset);
}

void V3DCommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((groupCountX == 0) || (groupCountY == 0) || (groupCountZ == 0))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_Dispatch << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(groupCountX, 0) << V3D_LOG_S_NUM_GREATER(groupCountY, 0) << V3D_LOG_S_NUM_GREATER(groupCountZ, 0));
		return;
	}
#endif //_DEBUG

	vkCmdDispatch(m_Source.commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void V3DCommandBuffer::ExecuteCommandBuffers(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers)
{
#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if ((commandBufferCount == 0) || (ppCommandBuffers == nullptr))
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_ExecuteCommandBuffers << V3D_LOG_S_DEBUG_NAME(m_DebugName.c_str()) << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(commandBufferCount, 0) << V3D_LOG_S_PTR(ppCommandBuffers));
		return;
	}

	uint32_t degbuErrorCount = 0;

	for (uint32_t i = 0; i < commandBufferCount; i++)
	{
		if (ppCommandBuffers[i]->GetType() != V3D_COMMAND_BUFFER_TYPE_SECONDARY)
		{
			V3D_LOG_ERROR(Log_Error_NotSecondaryCommandBuffer, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			degbuErrorCount++;
		}

		V3DCommandBuffer* pInternalCommandBuffer = static_cast<V3DCommandBuffer*>(ppCommandBuffers[i]);

		if (pInternalCommandBuffer->m_Debug.isBegin == true)
		{
			V3D_LOG_ERROR(Log_Error_SecondaryCommandBufferNotEnd, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			degbuErrorCount++;
		}

		if ((m_Source.commandBufferBeginInfo.flags & VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT) &&
			((pInternalCommandBuffer->GetSource().commandBufferBeginInfo.flags & VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT) == 0))
		{
			V3D_LOG_ERROR(Log_Error_CommandBuffersSimultaneousUse, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			degbuErrorCount++;
		}
	}

	if (degbuErrorCount > 0)
	{
		return;
	}
#endif //_DEBUG

	V3DCommandBuffer** ppSrcCommandBuffer = reinterpret_cast<V3DCommandBuffer**>(ppCommandBuffers);
	V3DCommandBuffer** ppSrcCommandBufferEnd = ppSrcCommandBuffer + commandBufferCount;

	m_Temp.commandBuffers.resize(commandBufferCount);
	VkCommandBuffer* pDstCommandBuffer = m_Temp.commandBuffers.data();

	while (ppSrcCommandBuffer != ppSrcCommandBufferEnd)
	{
		*pDstCommandBuffer++ = (*ppSrcCommandBuffer++)->GetSource().commandBuffer;
	}

	vkCmdExecuteCommands(m_Source.commandBuffer, commandBufferCount, m_Temp.commandBuffers.data());
}

void V3DCommandBuffer::BeginDebugMarker(const char* pName, const float color[4])
{
	if (m_pDebugMarkerBeginFunction == nullptr)
	{
		V3D_LOG_ERROR(Log_Error_UnavailableBeginDebugMarker, m_DebugName.c_str());
		return;
	}

#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pName == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_BeginDebugMarker << V3D_LOG_S_PTR(pName));
		return;
	}
#endif //_DEBUG

	VkDebugMarkerMarkerInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	info.pNext = nullptr;
	info.pMarkerName = pName;
	info.color[0] = color[0];
	info.color[1] = color[1];
	info.color[2] = color[2];
	info.color[3] = color[3];

	m_pDebugMarkerBeginFunction(m_Source.commandBuffer, &info);
}

void V3DCommandBuffer::EndDebugMarker()
{
	if (m_pDebugMarkerEndFunction == nullptr)
	{
		V3D_LOG_ERROR(Log_Error_UnavailableEndDebugMarker, m_DebugName.c_str());
		return;
	}

#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}
#endif //_DEBUG

	m_pDebugMarkerEndFunction(m_Source.commandBuffer);
}

void V3DCommandBuffer::InsertDebugMarker(const char* pName, const float color[4])
{
	if (m_pDebugMarkerInsertFunction == nullptr)
	{
		V3D_LOG_ERROR(Log_Error_UnavailableInsertDebugMarker, m_DebugName.c_str());
		return;
	}

#ifdef _DEBUG
	if (Debug_Command_FirstCheck() == false)
	{
		return;
	}

	if (pName == nullptr)
	{
		V3D_LOG_S_ERROR(Log_IV3DCommandBuffer_InsertDebugMarker << V3D_LOG_S_PTR(pName));
		return;
	}
#endif //_DEBUG

	VkDebugMarkerMarkerInfoEXT info;
	info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	info.pNext = nullptr;
	info.pMarkerName = pName;
	info.color[0] = color[0];
	info.color[1] = color[1];
	info.color[2] = color[2];
	info.color[3] = color[3];

	m_pDebugMarkerInsertFunction(m_Source.commandBuffer, &info);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DCommandBuffer::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DCommandBuffer::GetRefCount() const
{
	return m_RefCounter;
}

void V3DCommandBuffer::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DCommandBuffer::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DCommandBuffer);
	}
}

/******************************/
/* private - V3DCommandBuffer */
/******************************/

V3DCommandBuffer::V3DCommandBuffer() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Type(V3D_COMMAND_BUFFER_TYPE_PRIMARY),
	m_pCommandPool(nullptr),
	m_Source({}),
	m_Temp({}),
	m_pPushDescriptorSetFunction(nullptr),
	m_pDebugMarkerBeginFunction(nullptr),
	m_pDebugMarkerEndFunction(nullptr),
	m_pDebugMarkerInsertFunction(nullptr)
{
	m_Source.commandBuffer = VK_NULL_HANDLE;
	m_Temp.clearRects.resize(1);

#ifdef _DEBUG
	m_Debug.isBegin = false;
#endif //_DEBUG
}

V3DCommandBuffer::~V3DCommandBuffer()
{
	if ((m_pDevice != nullptr) && (m_pCommandPool != nullptr))
	{
		if (m_Source.commandBuffer != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(m_pDevice->GetSource().device, m_pCommandPool->GetSource().commandPool, 1, &m_Source.commandBuffer);
			V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.commandBuffer);
		}
	}

	V3D_RELEASE(m_pCommandPool);
	V3D_RELEASE(m_pDevice);
}
