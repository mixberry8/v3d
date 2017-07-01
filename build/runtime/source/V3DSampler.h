#pragma once

class V3DDevice;

class V3DSampler : public IV3DSampler
{
public:
	struct Source
	{
		VkSampler sampler;
	};

	static V3DSampler* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DSamplerDesc& desc, const wchar_t* pDebugName);
	const V3DSampler::Source& GetSource() const;

	/***************/
	/* IV3DSampler */
	/***************/
	const V3DSamplerDesc& GetDesc() const override;

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
	V3DSamplerDesc m_Desc;
	V3DSampler::Source m_Source;

	V3DSampler();
	virtual ~V3DSampler();

	V3D_DECLARE_ALLOCATOR
};
