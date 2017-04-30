#include "V3DInstance.h"

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
