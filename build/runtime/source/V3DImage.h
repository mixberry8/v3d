#pragma once

#include "IV3DImageBase.h"

class V3DDevice;
class V3DResourceMemory;

class V3DImage : public IV3DImageBase
{
public:
	static V3DImage* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout);

	/*****************/
	/* IV3DImageBase */
	/*****************/
	virtual const IV3DImageBase::Source& GetSource() const override;
	virtual bool CheckBindMemory() const override;
	virtual V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset) override;

	/*************/
	/* IV3DImage */
	/*************/
	virtual const V3DImageDesc& GetDesc() const override;
	virtual const V3DImageSubresourceLayout& GetSubresourceLayout(uint32_t level, uint32_t layer) const override;

	/****************/
	/* IV3DResource */
	/****************/
	virtual const V3DResourceDesc& GetResourceDesc() const override;
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) override;
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) override;
	virtual V3D_RESULT Unmap() override;

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
	static constexpr V3DImageSubresourceLayout DummySubresource = V3DImageSubresourceLayout{};

#ifdef _DEBUG
	uint64_t m_DebugImageAddr;
#endif //_DEBUG

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DResourceMemory* m_pMemory;
	V3DResourceDesc m_ResourceDesc;
	V3DImageDesc m_Desc;
	IV3DImageBase::Source m_Source;
	STLVector<V3DImageSubresourceLayout> m_SubresourceLayouts;

	V3DImage();
	virtual ~V3DImage();

	V3D_DECLARE_ALLOCATOR
};
