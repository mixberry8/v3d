#pragma once

class V3DDevice;

class V3DCommandPool : public IV3DCommandPool
{
public:
	struct Source
	{
		VkCommandPool commandPool;
	};

	static V3DCommandPool* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DCommandPoolDesc& desc, const wchar_t* pDebugName);
	const V3DCommandPool::Source& GetSource() const;

	/******************************/
	/* override - IV3DCommandPool */
	/******************************/
	virtual const V3DCommandPoolDesc& GetDesc() const override;
	virtual V3D_RESULT Reset(V3DFlags resetFlags) override;

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
	V3DCommandPoolDesc m_Desc;
	V3DCommandPool::Source m_Source;

	V3DCommandPool();
	virtual ~V3DCommandPool();

	V3D_DECLARE_ALLOCATOR
};
