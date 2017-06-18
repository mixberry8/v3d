#include "V3DPushDescriptorSetLayout.h"

/***************************************/
/* public - V3DPushDescriptorSetLayout */
/***************************************/

V3DPushDescriptorSetLayout* V3DPushDescriptorSetLayout::Create()
{
	return V3D_NEW_T(V3DPushDescriptorSetLayout);
}

V3D_RESULT V3DPushDescriptorSetLayout::Initialize(V3DDevice* pDevice, uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, const wchar_t* pDebugName)
{
	// ----------------------------------------------------------------------------------------------------
	// �x�[�X��������
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = V3DBaseDescriptorSetLayout::Initialize(pDevice, descriptorCount, descriptors, true, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

/*********************************************/
/* public override - IV3DDescriptorSetLayout */
/*********************************************/

V3D_DESCRIPTOR_SET_TYPE V3DPushDescriptorSetLayout::GetType() const
{
	return V3D_DESCRIPTOR_SET_TYPE_PUSH;
}

const V3DDescriptorSetLayoutStatistics& V3DPushDescriptorSetLayout::GetStatistics() const
{
	static constexpr V3DDescriptorSetLayoutStatistics dummy{};

	return dummy;
}

/****************************************/
/* private - V3DPushDescriptorSetLayout */
/****************************************/

V3DPushDescriptorSetLayout::V3DPushDescriptorSetLayout()
{
}

V3DPushDescriptorSetLayout::~V3DPushDescriptorSetLayout()
{
}