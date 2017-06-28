#pragma once

class V3DDevice;

class V3DBarrierSet : public IV3DBarrierSet
{
public:
	struct Source
	{
		VkPipelineStageFlags srcStageMask;
		VkPipelineStageFlags dstStageMask;
		uint32_t dependencyFlags;

		uint32_t bufferMemoryBarrierCount;
		VkBufferMemoryBarrier* pBufferMemoryBarriers;

		uint32_t imageMemoryBarrierCount;
		VkImageMemoryBarrier* pImageMemoryBarriers;
	};

	static V3DBarrierSet* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DBarrierSetDesc& desc, const wchar_t* pDebugName);
	const V3DBarrierSet::Source& GetSource() const;

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
	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DBarrierSet::Source m_Source;

	STLVector<IV3DBuffer*> m_Buffers;
	STLVector<IV3DBufferView*> m_BufferViews;
	STLVector<IV3DImage*> m_Images;
	STLVector<IV3DImageView*> m_ImageViews;

	V3DBarrierSet();
	virtual ~V3DBarrierSet();

	V3D_DECLARE_ALLOCATOR
};
