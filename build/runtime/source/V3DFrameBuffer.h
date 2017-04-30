#pragma once

class V3DDevice;
class V3DRenderPass;

class V3DFrameBuffer : public IV3DFrameBuffer
{
public:
	struct Source
	{
		VkFramebuffer framebuffer;
		VkExtent2D extent;
		uint32_t layerCount;
	};

	static V3DFrameBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments);

	const V3DFrameBuffer::Source& GetSource() const;

	/*******************/
	/* IV3DFrameBuffer */
	/*******************/
	virtual void GetRenderPass(IV3DRenderPass** ppRenderPass) override;
	virtual uint32_t GetAttachmentCount() const override;
	virtual void GetAttachment(uint32_t attachmentIndex, IV3DImageView** ppImage) override;

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
	V3DRenderPass* m_pRenderPass;
	STLVector<IV3DImageView*> m_ImageViews;
	V3DFrameBuffer::Source m_Source;

	V3DFrameBuffer();
	virtual ~V3DFrameBuffer();

	V3D_DECLARE_ALLOCATOR
};
