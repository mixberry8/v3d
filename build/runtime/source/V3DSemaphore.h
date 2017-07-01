#pragma once

class V3DDevice;
class V3DBuffer;

class V3DSemaphore : public IV3DSemaphore
{
public:
	struct Source
	{
		VkSemaphore semaphore;
	};

	static V3DSemaphore* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const wchar_t* pDebugName);
	const V3DSemaphore::Source& GetSource() const;

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
	V3DSemaphore::Source m_Source;

	V3DSemaphore();
	virtual ~V3DSemaphore();

	V3D_DECLARE_ALLOCATOR
};
