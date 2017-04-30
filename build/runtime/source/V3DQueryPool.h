#pragma once

class V3DDevice;

class V3DQueryPool : public IV3DQueryPool
{
public:
	struct Source
	{
		VkQueryPool queryPool;
		VkQueryControlFlags queryControlFlags;
	};

	static V3DQueryPool* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DQueryPoolDesc& desc);

	/*****************/
	/* IV3DImageBase */
	/*****************/
	virtual const V3DQueryPool::Source& GetSource() const;

	/*****************/
	/* IV3DQueryPool */
	/*****************/
	virtual const V3DQueryPoolDesc& GetDesc() const override;
	virtual V3D_RESULT GetResults(uint32_t firstQuery, uint32_t queryCount, uint32_t queryResultCount, uint64_t* pQueryResults, V3DFlags queryResultFlags) override;

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
