#pragma once

class V3DDevice;
//class V3DImage;
class IV3DImageBase;

class V3DImageView : public IV3DImageView
{
public:
	struct Source
	{
		VkImage image;
		VkImageView imageView;
		VkImageSubresourceRange imageSubresourceRange;
		VkExtent3D extent;
	};

	static V3DImageView* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DImage* pImage, const V3DImageViewDesc& desc, const wchar_t* pDebugName);
	IV3DImageBase* GetInternalImagePtr() const;
	const V3DImageView::Source& GetSource() const;

	/*****************/
	/* IV3DImageView */
	/*****************/
	virtual const V3DImageViewDesc& GetDesc() const override;
	virtual void GetImage(IV3DImage** ppImage) override;
	virtual const V3DSize3D& GetImageSubresourceSize() const override;

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
#ifdef _DEBUG

#ifdef _WIN64
	uint64_t m_DebugImageAddr;
#else //_WIN64
	uint32_t m_DebugImageAddr;
#endif //_WIN64

#endif //_DEBUG

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	IV3DImageBase* m_pImage;
	V3DImageViewDesc m_Desc;
	V3DSize3D m_SubresourceSize;
	V3DImageView::Source m_Source;

	V3DImageView();
	virtual ~V3DImageView();

	V3D_DECLARE_ALLOCATOR
};
