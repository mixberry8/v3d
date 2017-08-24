#pragma once

#ifdef V3D_DEBUG
#include "CriticalSection.h"
#endif //V3D_DEBUG

class V3DAdapter;
class V3DSwapChain;

class V3DInstance : public IV3DInstance
{
public:
	struct Source
	{
		VkInstance instance;
	};

	static bool IsCreated();

	static V3DInstance* Create();
	V3D_RESULT Initialize(const V3DInstanceDesc& instanceDesc);
	const V3DInstance::Source& GetSource() const;

	void AddWindow(V3DSwapChain* pSwapChain);
	void RemoveWindow(V3DSwapChain* pSwapChain);
	bool ExistsFulscreenWindow(V3DSwapChain* pSwapChain);

#ifdef V3D_DEBUG
	void AddDebugObject(void* pVulkanObject, const wchar_t* pName);
	void AddDebugObject(uint64_t vulkanObject, const wchar_t* pName);
	void RemoveDebugObject(void* pVulkanObject);
	void RemoveDebugObject(uint64_t vulkanObject);

	void AddDebugMemoryObject(void* pInterface, V3D_DEBUG_OBJECT_TYPE type, const wchar_t* pName);
	void RemoveDebugMemoryObject(void* pInterface);
#endif //V3D_DEBUG

	/*************************/
	/* override IV3DInstance */
	/*************************/
	V3D_LAYER GetLayer() const override;
	uint32_t GetAdapterCount() const override;
	void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) override;
	V3D_RESULT CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice, const wchar_t* pDebugName) override;
	void DumpObjects() override;

	/***********************/
	/* override IV3DObject */
	/***********************/
	int64_t GetRefCount() const override;
	void AddRef() override;
	void Release() override;

private:
	struct Window
	{
		HWND handle;
		WNDPROC pProc;
		V3DSwapChain* pSwapChain;
	};

#ifdef V3D_DEBUG
	struct DebugObject
	{
		V3D_DEBUG_OBJECT_TYPE type;
		STLStringW name;
	};
#endif //V3D_DEBUG

	static V3DInstance* s_pThis;

	ReferenceCounter m_RefCounter;
	V3D_LAYER m_Layer;
	VkAllocationCallbacks m_AllocationCallbacks;
	V3DInstance::Source m_Source;
	STLVector<V3DAdapter*> m_Adapters;
	STLVector<V3DInstance::Window> m_Windows;

	V3DInstance();
	virtual ~V3DInstance();

	V3DInstance::Window* GetWindow(HWND windowHandle);

	static VKAPI_ATTR void* VKAPI_CALL Alloc(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope);
	static VKAPI_ATTR void* VKAPI_CALL Realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope);
	static VKAPI_ATTR void VKAPI_CALL Free(void* pUserData, void* pMemory);

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM eparam, LPARAM lparam);

#ifdef V3D_DEBUG
	VkDebugReportCallbackEXT m_DebugReportCallbackEXT;

	STLMap<STLStringA, STLStringA> m_DebugHandleNameMap;
	STLMap<STLStringA, STLStringA> m_DebugFunctionNameMap;
	STLMap<STLStringA, STLStringA> m_DebugConstantNameMap;

	typedef STLMap<uint64_t, STLStringA> DebugObjectNameMap;
	V3DInstance::DebugObjectNameMap m_DebugObjectNameMap;

	STLMap<void*, V3DInstance::DebugObject> m_DebugMemoryObjectMap;

	CriticalSection m_DebugSync;
	STLStringStreamA m_DebugStringStream;
	STLStringA m_DebugString;

	char m_DebugObjectAddr[32];

	const char* GetDebugObjectName(uint64_t objectAddr);
	const char* ConvertDebugMessage(const char* pMessage);
	const char* ConvertDebugString(const char* pString);

	void DumpObjects(V3D_LOG_FLAG type);
#endif //V3D_DEBUG

#ifdef V3D_DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallbackEXT(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objectType,
		uint64_t object,
		size_t location,
		int32_t messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData);
#endif //V3D_DEBUG

	V3D_DECLARE_ALLOCATOR
};