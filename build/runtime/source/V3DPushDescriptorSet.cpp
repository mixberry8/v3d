#include "V3DPushDescriptorSet.h"
#include "V3DDevice.h"

/*************************************/
/* public - V3DPushDescriptorSet */
/*************************************/

V3DPushDescriptorSet* V3DPushDescriptorSet::Create()
{
	return V3D_NEW_T(V3DPushDescriptorSet);
}

V3D_RESULT V3DPushDescriptorSet::Initialize(IV3DDevice* pDevice, IV3DDescriptorSetLayout* pLayout, const wchar_t* pDebugName)
{
	// ----------------------------------------------------------------------------------------------------
	// ベースを初期化
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = V3DBaseDescriptorSet::Initialize(pDevice, pLayout, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットの書き込み情報を書き換える
	// ----------------------------------------------------------------------------------------------------

	VkWriteDescriptorSet* pWrite = m_Source.pWriteDescriptors;
	VkWriteDescriptorSet* pWriteEnd = pWrite + m_Source.descriptorCount;

	while (pWrite != pWriteEnd)
	{
		switch (pWrite->descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			pWrite->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			pWrite->descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		}

		pWrite++;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

/***************************************/
/* public override - IV3DDescriptorSet */
/***************************************/

V3D_DESCRIPTOR_SET_TYPE V3DPushDescriptorSet::GetType() const
{
	return V3D_DESCRIPTOR_SET_TYPE_PUSH;
}

void V3DPushDescriptorSet::Update()
{
	V3D_LOG_WARNING(Log_Error_DescriptorSetNoNeedUpdate);
}

/**********************************/
/* private - V3DPushDescriptorSet */
/**********************************/

V3DPushDescriptorSet::V3DPushDescriptorSet()
{
}

V3DPushDescriptorSet::~V3DPushDescriptorSet()
{
}
