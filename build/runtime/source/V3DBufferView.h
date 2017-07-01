#pragma once

class V3DDevice;
class V3DBuffer;

class V3DBufferView : public IV3DBufferView
{
public:
	struct Source
	{
		VkBuffer buffer;
		VkBufferView bufferView;
		uint64_t offset;
		uint64_t size;
	};

	static V3DBufferView* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, const wchar_t* pDebugName);
	const V3DBufferView::Source& GetSource() const;

	/*****************/
	/* IV3DBufferView */
	/*****************/
	const V3DBufferViewDesc& GetDesc() const override;
	void GetBuffer(IV3DBuffer** ppBuffer) override;

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
	V3DBuffer* m_pBuffer;
	V3DBufferViewDesc m_Desc;
	V3DBufferView::Source m_Source;

	V3DBufferView();
	virtual ~V3DBufferView();

	V3D_DECLARE_ALLOCATOR
};
