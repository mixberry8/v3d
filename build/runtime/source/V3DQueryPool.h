#pragma once

class V3DDevice;

class V3DQueryPool : public IV3DQueryPool
{
public:
	struct Source
	{
		VkQueryPool queryPool;
		VkQueryControlFlags queryControlFlags;
		uint32_t queryCount;
	};

	static V3DQueryPool* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DQueryPoolDesc& desc, const wchar_t* pDebugName);

	/*****************/
	/* IV3DImageBase */
	/*****************/
	virtual const V3DQueryPool::Source& GetSource() const;

	/*****************/
	/* IV3DQueryPool */
	/*****************/
	virtual const V3DQueryPoolDesc& GetDesc() const override;
	virtual V3D_RESULT GetResult(uint32_t firstQuery, uint32_t queryCount, uint64_t dstSize, void* pDst, uint64_t dstStride, V3DFlags queryResultFlags) override;

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
	V3DQueryPoolDesc m_Desc;
	V3DQueryPool::Source m_Source;

	V3DQueryPool();
	virtual ~V3DQueryPool();

	V3D_DECLARE_ALLOCATOR
};
