#pragma once

class V3DDevice;

class V3DShaderModule : public IV3DShaderModule
{
public:
	struct Source
	{
		VkShaderModule shaderModule;
	};

	static V3DShaderModule* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, size_t codeSize, const void* pCode);
	const V3DShaderModule::Source& GetSource() const;

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
	V3DShaderModule::Source m_Source;

	V3DShaderModule();
	virtual ~V3DShaderModule();

	V3D_DECLARE_ALLOCATOR
};