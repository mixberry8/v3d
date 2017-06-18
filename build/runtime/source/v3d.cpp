#include "V3DInstance.h"

V3D_RESULT V3DCheckLayer(V3D_LAYER layer)
{
	uint32_t vLayerPropCount;
	std::vector<VkLayerProperties> vLayerProps;

	VkResult vResult = vkEnumerateInstanceLayerProperties(&vLayerPropCount, nullptr);
	if (vResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	vLayerProps.resize(vLayerPropCount);
	vResult = vkEnumerateInstanceLayerProperties(&vLayerPropCount, vLayerProps.data());
	if (vResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	const char* pLayerName = nullptr;

	switch (layer)
	{
	case V3D_LAYER_VALIDATION:
		pLayerName = V3D_LAYER_LUNARG_standard_validation;
		break;

	case V3D_LAYER_NSIGHT:
		pLayerName = V3D_LAYER_NV_nsight;
		break;

	case V3D_LAYER_RENDERDOC:
		pLayerName = V3D_LAYER_RENDERDOC_Capture;
		break;
	}

	if ((pLayerName == nullptr) || (std::find_if(vLayerProps.begin(), vLayerProps.end(), V3DFindLayer(pLayerName)) == vLayerProps.end()))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	return V3D_OK;
}

V3D_RESULT V3DCreateInstance(const V3DInstanceDesc& instanceDesc, IV3DInstance** ppInstance)
{
	if (V3DInstance::IsCreated() == true)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// メモリの初期化
	// ----------------------------------------------------------------------------------------------------

	if (((instanceDesc.memory.pAllocateFunction == nullptr) && (instanceDesc.memory.pReallocateFunction == nullptr) && (instanceDesc.memory.pFreeFunction == nullptr)) ||
		((instanceDesc.memory.pAllocateFunction != nullptr) && (instanceDesc.memory.pReallocateFunction != nullptr) && (instanceDesc.memory.pFreeFunction != nullptr)))
	{
		;
	}
	else
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3D_MEM_INIT(instanceDesc.memory.pAllocateFunction, instanceDesc.memory.pReallocateFunction, instanceDesc.memory.pFreeFunction, instanceDesc.memory.pUserData);

	// ----------------------------------------------------------------------------------------------------
	// インスタンスの作成
	// ----------------------------------------------------------------------------------------------------

	V3DInstance* pInstance = V3DInstance::Create();
	if (pInstance == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pInstance->Initialize(instanceDesc);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pInstance);
		return result;
	}

	(*ppInstance) = pInstance;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}
