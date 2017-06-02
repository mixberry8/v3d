#pragma once

class V3DInstance;
class V3DAdapter;
class V3DQueue;

class V3DDevice : public IV3DDevice
{
public:
	struct Source
	{
		VkDevice device;
		VkPhysicalDeviceProperties deviceProps;
		VkPhysicalDeviceFeatures deviceFeatures;
	};

	static V3DDevice* Create();
	V3D_RESULT Initialize(V3DInstance* pInstance, IV3DAdapter* pAdapter, const wchar_t* pDebugName);
	V3DInstance* GetInternalInstancePtr() const;
	V3DAdapter* GetInternalAdapterPtr() const;
	const V3DDevice::Source& GetSource() const;

	void NotifyReleaseSampler();
	void NotifyReleaseResourceMemory();

	/*************************/
	/* override - IV3DDevice */
	/*************************/
	virtual void GetAdapter(IV3DAdapter** ppAdapter) override;
	virtual const V3DDeviceCaps& GetCaps() const override;
	virtual const V3DDeviceStatistics& GetStatistics() const override;
	virtual uint32_t GetQueueFamilyCount() const override;
	virtual const V3DDeviceQueueFamilyDesc& GetQueueFamilyDesc(uint32_t family) const override;
	virtual void GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue) override;
	virtual V3D_RESULT GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc) override;
	virtual V3D_RESULT CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const override;
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const override;
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const override;
	virtual V3D_RESULT CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateCommandBuffer(IV3DCommandPool* pCommandPool, V3D_COMMAND_BUFFER_TYPE commandBufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateFence(IV3DFence** ppFence, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const override;
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const override;
	virtual V3D_RESULT AllocateResourceMemory(V3DFlags propertyFlags, uint64_t size, IV3DResourceMemory** ppResourceMemory, const wchar_t* pDebugName) override;
	virtual V3D_RESULT BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset) override;
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags propertyFlags, IV3DResource* pResource, const wchar_t* pDebugName) override;
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags propertyFlags, uint32_t resourceCount, IV3DResource** ppResources, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateBuffer(const V3DBufferDesc& desc, IV3DBuffer** ppBuffer, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateBufferView(IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, IV3DBufferView** ppBufferView, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateRenderPass(uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments, uint32_t subpassCount, const V3DSubpassDesc* pSubpasses, uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies, IV3DRenderPass** ppRenderPass, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateFrameBuffer(IV3DRenderPass* pRenderPass,	uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateDescriptorSetLayout(uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep, IV3DDescriptorSetLayout** ppDescriptorSetLayout, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreatePipelineLayout(uint32_t constantCount, V3DConstantDesc* pConstants, uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts, IV3DPipelineLayout** ppPipelineLayout, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName) override;
	virtual V3D_RESULT CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName) override;
	virtual V3D_RESULT WaitIdle() override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	struct QueueFamily
	{
		V3DDeviceQueueFamilyDesc desc;
		STLVector<V3DQueue*> queues;
	};

	ReferenceCounter m_RefCounter;
	V3DInstance* m_pInstance;
	V3DAdapter* m_pAdapter;
	V3DDeviceCaps m_Caps;
	V3DDeviceStatistics m_Statistics;
	V3DDevice::Source m_Source;
	STLVector<V3DDevice::QueueFamily> m_QueueFamilies;

	V3DDevice();
	virtual ~V3DDevice();

	V3D_DECLARE_ALLOCATOR
};
