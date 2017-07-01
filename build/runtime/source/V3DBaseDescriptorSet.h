#pragma once

#include "V3DBaseDescriptorSetLayout.h"

class V3DDevice;
class V3DBuffer;
class V3DBufferView;
class V3DImageView;
class V3DSampler;

class V3DBaseDescriptorSet : public IV3DDescriptorSet
{
public:
	struct Source
	{
		VkDescriptorSet descriptorSet;

		STLVector<VkDescriptorImageInfo> descriptorImageInfos;
		STLVector<VkDescriptorBufferInfo> descriptorBufferInfos;

		uint32_t descriptorCount;
		VkWriteDescriptorSet* pWriteDescriptors;

		uint64_t dynamicBufferMask;
	};

	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout, const wchar_t* pDebugName);
	const V3DBaseDescriptorSet::Source& GetSource() const;

	/*********************/
	/* IV3DDescriptorSet */
	/*********************/
	void GetLayout(IV3DDescriptorSetLayout** ppLayout) override;
	V3D_RESULT GetBuffer(uint32_t binding, IV3DBuffer** ppBuffer) override;
	V3D_RESULT SetBuffer(uint32_t binding, IV3DBuffer* pBuffer, uint64_t offset, uint64_t size) override;
	V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) override;
	V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) override;
	V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) override;
	V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) override;
	V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) override;
	V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) override;
	V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) override;
	V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) override;
	V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, IV3DSampler* pSampler) override;
	V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) override;

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

protected:
	V3DDevice* m_pDevice;
	V3DBaseDescriptorSetLayout* m_pLayout;
	STLVector<V3DBuffer*> m_Buffers;
	STLVector<V3DBufferView*> m_BufferViews;
	STLVector<V3DImageView*> m_ImageViews;
	STLVector<V3DSampler*> m_Samplers;
	V3DBaseDescriptorSet::Source m_Source;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DBaseDescriptorSet();
	virtual ~V3DBaseDescriptorSet();

private:
	ReferenceCounter m_RefCounter;
};
