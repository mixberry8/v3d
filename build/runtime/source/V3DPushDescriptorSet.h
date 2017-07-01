#pragma once

#include "V3DBaseDescriptorSet.h"
#include "V3DPushDescriptorSetLayout.h"

class V3DDevice;
class V3DBuffer;
class V3DBufferView;
class V3DImageView;
class V3DSampler;

class V3DPushDescriptorSet : public V3DBaseDescriptorSet
{
public:
	static V3DPushDescriptorSet* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout, const wchar_t* pDebugName);

	/*********************/
	/* IV3DDescriptorSet */
	/*********************/
	V3D_DESCRIPTOR_SET_TYPE GetType() const override;
	void Update() override;

private:
	V3DPushDescriptorSet();
	virtual ~V3DPushDescriptorSet();

	V3D_DECLARE_ALLOCATOR
};
