#pragma once

class V3DDevice;

class V3DFence : public IV3DFence
{
public:
	struct Source
	{
		VkFence fence;
	};

	static V3DFence* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const wchar_t* pDebugName);
	const V3DFence::Source& GetSource() const;

	/************************/
	/* override - IV3DFence */
	/************************/
	virtual bool IsSignaled() const override;
	virtual V3D_RESULT Reset() override;
	virtual V3D_RESULT Wait(uint64_t timeOut) override;

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
	V3DFence::Source m_Source;

	V3DFence();
	virtual ~V3DFence();

	V3D_DECLARE_ALLOCATOR
};
