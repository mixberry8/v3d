#pragma once

class V3DDevice;

class V3DEvent : public IV3DEvent
{
public:
	struct Source
	{
		VkEvent event;
	};

	static V3DEvent* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const wchar_t* pDebugName);
	const V3DEvent::Source& GetSource() const;

	/************************/
	/* override - IV3DEvent */
	/************************/
	bool GetState() const override;
	V3D_RESULT Reset() override;
	V3D_RESULT Set() override;

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
	V3DEvent::Source m_Source;

	V3DEvent();
	virtual ~V3DEvent();

	V3D_DECLARE_ALLOCATOR
};
