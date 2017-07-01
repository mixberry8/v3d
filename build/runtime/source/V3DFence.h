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
	V3D_RESULT Initialize(IV3DDevice* pDevice, bool initialState, const wchar_t* pDebugName);
	const V3DFence::Source& GetSource() const;

	/************************/
	/* override - IV3DFence */
	/************************/
	bool GetState() const override;
	V3D_RESULT Reset() override;
	V3D_RESULT Wait(uint64_t timeOut) override;

	/******************************/
	/* override - IV3DDeviceChild */
	/******************************/
	void GetDevice(IV3DDevice** ppDevice) override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	int64_t GetRefCount() const override;
	void AddRef() override;
	void Release() override;

private:
	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DFence::Source m_Source;

	V3DFence();
	virtual ~V3DFence();

	V3D_DECLARE_ALLOCATOR
};
