#pragma once

#include "V3DBaseDescriptorSetLayout.h"

class V3DPushDescriptorSetLayout : public V3DBaseDescriptorSetLayout
{
public:
	struct HandleT;
	typedef HandleT* Handle;

	static V3DPushDescriptorSetLayout* Create();
	V3D_RESULT Initialize(V3DDevice* pDevice, uint32_t descriptorCountconst, const V3DDescriptorDesc* descriptors, const wchar_t* pDebugName);

	/***************************/
	/* IV3DDescriptorSetLayout */
	/***************************/
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const override;
	virtual const V3DDescriptorSetLayoutStatistics& GetStatistics() const override;

private:
	V3DPushDescriptorSetLayout();
	virtual ~V3DPushDescriptorSetLayout();

	V3D_DECLARE_ALLOCATOR
};
