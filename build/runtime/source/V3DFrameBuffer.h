#pragma once

class V3DDevice;
class V3DRenderPass;

class V3DFrameBuffer : public IV3DFrameBuffer
{
public:
	struct Source
	{
		VkRenderPass renderPass;
		uint32_t clearValueCount;
		VkClearValue* pClearValues;
		VkFramebuffer framebuffer;
		VkExtent2D extent;
		uint32_t layerCount;
	};

	static V3DFrameBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, const wchar_t* pDebugName);

	const V3DFrameBuffer::Source& GetSource() const;

	/*******************/
	/* IV3DFrameBuffer */
	/*******************/
	const V3DFrameBufferDesc& GetDesc() const override;
	void GetRenderPass(IV3DRenderPass** ppRenderPass) override;
	uint32_t GetAttachmentCount() const override;
	void GetAttachment(uint32_t attachmentIndex, IV3DImageView** ppImage) override;

	/*******************/
	/* IV3DDeviceChild */
	/*******************/
	void GetDevice(IV3DDevice** ppDevice) override;

	/**************/
	/* IV3DObject */
	/**************/
	int64_t GetRefCount() const override;
	void AddRef() override;
	void Release() override;

private:
	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DFrameBufferDesc m_Desc;
	V3DRenderPass* m_pRenderPass;
	STLVector<IV3DImageView*> m_ImageViews;
	V3DFrameBuffer::Source m_Source;

	V3DFrameBuffer();
	virtual ~V3DFrameBuffer();

	V3D_DECLARE_ALLOCATOR
};
