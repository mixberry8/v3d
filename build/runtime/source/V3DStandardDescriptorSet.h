#pragma once

#include "V3DBaseDescriptorSet.h"
#include "V3DStandardDescriptorSetLayout.h"

class V3DDevice;
class V3DBuffer;
class V3DBufferView;
class V3DImageView;
class V3DSampler;

class V3DStandardDescriptorSet : public V3DBaseDescriptorSet
{
public:
	static V3DStandardDescriptorSet* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout, const wchar_t* pDebugName);

	/*********************/
	/* IV3DDescriptorSet */
	/*********************/
	V3D_DESCRIPTOR_SET_TYPE GetType() const override;
	void Update() override;

private:
	V3DStandardDescriptorSetLayout::Handle m_PoolHandle;

	V3DStandardDescriptorSet();
	virtual ~V3DStandardDescriptorSet();

	V3D_DECLARE_ALLOCATOR
};
