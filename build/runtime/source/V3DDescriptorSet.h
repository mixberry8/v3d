#pragma once

#include "V3DDescriptorSetLayout.h"

class V3DDevice;
class V3DImageView;
class V3DSampler;
class V3DBufferView;

class V3DDescriptorSet : public IV3DDescriptorSet
{
public:
	struct Source
	{
		VkDescriptorSet descriptorSet;

		STLVector<VkDescriptorImageInfo> descriptorImageInfos;
		STLVector<VkDescriptorBufferInfo> descriptorBufferInfos;

		uint32_t descriptorCount;
		VkWriteDescriptorSet* pWriteDescriptors;
	};

	static V3DDescriptorSet* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout);
	const V3DDescriptorSet::Source& GetSource() const;

	/*********************/
	/* IV3DDescriptorSet */
	/*********************/
	virtual void GetLayout(IV3DDescriptorSetLayout** ppLayout) override;
	virtual V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) override;
	virtual V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) override;
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) override;
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) override;
	virtual V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) override;
	virtual V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) override;
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) override;
	virtual V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) override;
	virtual V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) override;
	virtual void Update() override;

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
	V3DDescriptorSetLayout* m_pLayout;
	V3DDescriptorSetLayout::Handle m_PoolHandle;
	STLVector<V3DImageView*> m_ImageViews;
	STLVector<V3DSampler*> m_Samplers;
	STLVector<V3DBufferView*> m_BufferViews;
	V3DDescriptorSet::Source m_Source;

	V3DDescriptorSet();
	virtual ~V3DDescriptorSet();

	V3D_DECLARE_ALLOCATOR
};
