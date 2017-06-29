#pragma once

class V3DDevice;
class V3DCommandPool;
class V3DRenderPass;
class V3DFrameBuffer;

class V3DCommandBuffer : public IV3DCommandBuffer
{
public:
	struct Source
	{
		VkCommandBuffer commandBuffer;
		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
		VkCommandBufferBeginInfo commandBufferBeginInfo;
	};

#ifdef _DEBUG
	struct Debug
	{
		bool isBegin;
	};
#endif //_DEBUG

	static V3DCommandBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DCommandPool* pCommandPool, V3D_COMMAND_BUFFER_TYPE commandBufferType, const wchar_t* pDebugName);
	const V3DCommandBuffer::Source& GetSource() const;
#ifdef _DEBUG
	const V3DCommandBuffer::Debug& GetDebug() const
	{
		return m_Debug;
	}
#endif //_DEBUG

	/********************************/
	/* override - IV3DCommandBuffer */
	/********************************/
	virtual V3D_COMMAND_BUFFER_TYPE GetType() const override;
	virtual void GetPool(IV3DCommandPool** ppPool) override;

	virtual V3D_RESULT Reset(V3DFlags resetFlags) override;
	virtual V3D_RESULT Begin() override;
	virtual V3D_RESULT Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer) override;
	virtual V3D_RESULT End() override;

	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DBufferMemoryBarrier& bufferMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DBufferViewMemoryBarrier& bufferViewMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DImageMemoryBarrier& imageMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DImageViewMemoryBarrier& imageViewMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DBufferMemoryBarrier* pMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DBufferViewMemoryBarrier* pMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DImageMemoryBarrier* pMemoryBarriers) override;
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DImageViewMemoryBarrier* pMemoryBarriers) override;
	virtual void Barrier(IV3DBarrierSet* pBarrierSet) override;
	virtual void ResetEvent(IV3DEvent* pEvent, V3DFlags stageMask) override;
	virtual void SetEvent(IV3DEvent* pEvent, V3DFlags stageMask) override;
	virtual void WaitEvent(uint32_t eventCount, IV3DEvent** ppEvents, V3DFlags srcStageMask, V3DFlags dstStageMask) override;
	virtual void FillBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data) override;
	virtual void UpdateBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t dataSize, const void* pData) override;
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size) override;
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges) override;
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) override;
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DCopyImageRange* pRanges) override;
	virtual void CopyBufferToImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize, IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset) override;
	virtual void CopyBufferToImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges) override;
	virtual void CopyImageToBuffer(IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize) override;
	virtual void CopyImageToBuffer(IV3DBuffer* pDstBuffer, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges) override;
	virtual void BlitImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, V3DBlitImageRange* pRanges, V3D_FILTER filter) override;
	virtual void BlitImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DRectangle2D& dstRect, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DRectangle2D& srcRect, V3D_FILTER filter) override;
	virtual void BlitImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, V3D_FILTER filter) override;
	virtual void ResolveImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t rangeCount, const V3DResolveImageRange* pRanges) override;
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DPoint2D& dstOffset, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DPoint2D& srcOffset, const V3DSize2D& size) override;
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout) override;
	virtual void BeginRenderPass(IV3DRenderPass* pRenderPass, IV3DFrameBuffer* pFrameBuffer, bool subpassContentInline, const V3DRectangle2D* pRenderArea) override;
	virtual void EndRenderPass() override;
	virtual void NextSubpass() override;
	virtual void ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) override;
	virtual void ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) override;
	virtual void ClearAttachment(uint32_t colorAttachmentCount, const V3DClearColorAttachmentDesc* pColorAttachments, const V3DClearDepthStencilAttachmentDesc* pDepthStencilAttachment, uint32_t rangeCount, const V3DClearRange* pRanges) override;
	virtual void BindPipeline(IV3DPipeline* pPipeline) override;
	virtual void BindDescriptorSet(V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout, uint32_t set, IV3DDescriptorSet* pDescriptorSet, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) override;
	virtual void BindDescriptorSet(V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout, uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) override;
	virtual void BindVertexBuffer(uint32_t binding, IV3DBuffer* pBuffer, uint64_t offset) override;
	virtual void BindVertexBuffer(uint32_t firstBinding, uint32_t bindingCount, IV3DBuffer** ppBuffers, const uint64_t* pOffsets) override;
	virtual void BindIndexBuffer(IV3DBuffer* pBuffer, uint64_t offset, V3D_INDEX_TYPE indexType) override;
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData) override;
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData) override;
	virtual void PushDescriptorSet(V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout, uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) override;
	virtual void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports) override;
	virtual void SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors) override;
	virtual void SetBlendConstant(const float blendConstants[4]) override;
	virtual void SetStencilReference(V3DFlags faceMask, uint32_t reference) override;
	virtual void ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount) override;
	virtual void BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query) override;
	virtual void EndQuery(IV3DQueryPool* pQueryPool, uint32_t query) override;
	virtual void WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query) override;
	virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset) override;
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
	virtual void ExecuteCommandBuffer(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers) override;
	virtual void BeginDebugMarker(const char* pName, const float color[4]) override;
	virtual void EndDebugMarker() override;
	virtual void InsertDebugMarker(const char* pName, const float color[4]) override;

	/******************************/
	/* override - IV3DDeviceChild */
	/******************************/
	virtual void GetDevice(IV3DDevice** ppDevice) override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	struct Temp
	{
		STLVector<VkClearAttachment> clearAttachments;
		STLVector<VkClearRect> clearRects;
		STLVector<VkEvent> events;
		STLVector<VkBufferMemoryBarrier> bufferMemoryBarriers;
		STLVector<VkImageMemoryBarrier> imageMemoryBarriers;
		STLVector<VkBufferCopy> bufferCopies;
		STLVector<VkBufferImageCopy> bufferImageCopies;
		STLVector<VkImageCopy> imageCopies;
		STLVector<VkImageBlit> imageBlits;
		STLVector<VkImageResolve> imageResolves;
		STLVector<VkCommandBuffer> commandBuffers;

		VkViewport* viewports;
		VkRect2D* scissors;
		VkBuffer* vertexBuffers;
		uint64_t* vertexBufferOffsets;
		VkDescriptorSet* descriptorSets;

		VkDescriptorBufferInfo descriptorBufferInfos[V3D_PUSH_DESCRIPTOR_MAX];
		VkWriteDescriptorSet writeDescriptors[V3D_PUSH_DESCRIPTOR_MAX];
	};

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DCommandPool* m_pCommandPool;
	V3D_COMMAND_BUFFER_TYPE m_Type;
	V3DCommandBuffer::Source m_Source;
	V3DCommandBuffer::Temp m_Temp;

	PFN_vkCmdPushDescriptorSetKHR m_pPushDescriptorSetFunction;
	PFN_vkCmdDebugMarkerBeginEXT m_pDebugMarkerBeginFunction;
	PFN_vkCmdDebugMarkerEndEXT m_pDebugMarkerEndFunction;
	PFN_vkCmdDebugMarkerInsertEXT m_pDebugMarkerInsertFunction;

#ifdef _DEBUG
	STLStringW m_DebugName;
	V3DCommandBuffer::Debug m_Debug;
#endif //_DEBUG

	V3DCommandBuffer();
	virtual ~V3DCommandBuffer();

#ifdef _DEBUG
	bool Debug_Command_FirstCheck()
	{
		if (m_Debug.isBegin == false)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_CommandBufferNotBegin, m_DebugName.c_str());
			return false;
		}

		return true;
	}
#endif //_DEBUG

	V3D_DECLARE_ALLOCATOR
};
