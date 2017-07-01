#pragma once

#include "IV3DImageBase.h"

class V3DDevice;

class V3DBackBuffer : public IV3DImageBase
{
public:
	static V3DBackBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, VkImage image, VkFormat format, uint32_t width, uint32_t height, VkImageUsageFlags usage, const wchar_t* pDebugName);

	/*****************/
	/* IV3DImageBase */
	/*****************/
	const IV3DImageBase::Source& GetSource() const override;
	bool CheckBindMemory() const override;
	V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset) override;

#ifdef _DEBUG
	const wchar_t* GetDebugName() const override;
#endif //_DEBUG

	/*************/
	/* IV3DImage */
	/*************/
	const V3DImageDesc& GetDesc() const override;
	const V3DImageSubresourceLayout& GetSubresourceLayout(uint32_t level, uint32_t layer) const override;

	/****************/
	/* IV3DResource */
	/****************/
	const V3DResourceDesc& GetResourceDesc() const override;
	V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) override;
	V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) override;
	V3D_RESULT Unmap() override;

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
	static constexpr V3DImageSubresourceLayout DummySubresource = V3DImageSubresourceLayout{};

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DResourceDesc m_ResourceDesc;
	V3DImageDesc m_Desc;
	IV3DImageBase::Source m_Source;

	V3DBackBuffer();
	virtual ~V3DBackBuffer();

	V3D_DECLARE_ALLOCATOR
};
