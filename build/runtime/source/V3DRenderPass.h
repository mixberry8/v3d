#pragma once

class V3DDevice;

class V3DRenderPass : public IV3DRenderPass
{
public:
	struct Attachment
	{
		uint32_t attachment;
		V3D_FORMAT format;
		V3D_SAMPLE_COUNT_FLAG samples;
	};

	struct Subpass
	{
		STLVector<V3DRenderPass::Attachment> inputAttachments;
		STLVector<V3DRenderPass::Attachment> colorAttachments;
		STLVector<V3DRenderPass::Attachment> resolveAttachments;
	};

	struct Source
	{
		VkRenderPass renderPass;

		uint32_t clearValueCount;
		VkClearValue* pClearValues;

#ifdef _DEBUG
		struct Debug
		{
			STLVector<V3DRenderPass::Attachment> attachments;
			STLVector<V3DRenderPass::Subpass> subpasses;
		}debug;
#endif //_DEBUG
	};

	static V3DRenderPass* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments, uint32_t subpassCount, const V3DSubpassDesc* pSubpasses, uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies, const wchar_t* pDebugName);
	const V3DRenderPass::Source& GetSource() const;

#ifdef _DEBUG
	const wchar_t* GetDebugName() const;
#endif //_DEBUG

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
	STLVector<VkClearValue> m_ClearValues;
	V3DRenderPass::Source m_Source;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DRenderPass();
	virtual ~V3DRenderPass();

	static VkAttachmentReference* AllocateVkAttachmentReferences(STLVector<VkAttachmentReference>& vkAttchmentReferences, uint32_t totalAttachmentReferenceCount);
	static VkAttachmentReference* WriteAttachmentReferences(STLVector<VkAttachmentReference>& vkAttchmentReferences, uint32_t attachmentReferenceCount, const V3DAttachmentReference* attachmentReferences, VkAttachmentReference** ppVkAttchmentReferencePtr);

	V3D_DECLARE_ALLOCATOR
};
