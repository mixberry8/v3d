#pragma once

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

	/*************************/
	/* override IV3DInstance */
	/*************************/
	virtual V3D_LAYER_TYPE GetLayerType() const override;
	virtual uint32_t GetAdapterCount() const override;
	virtual void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) override;
	virtual V3D_RESULT CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice) override;

	/***********************/
	/* override IV3DObject */
	/***********************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	struct Window
	{
		HWND handle;
		WNDPROC pProc;
		V3DSwapChain* pSwapChain;
	};

	static V3DInstance* s_pThis;

	ReferenceCounter m_RefCounter;
	V3D_LAYER_TYPE m_LayerType;
	VkAllocationCallbacks m_AllocationCallbacks;
	V3DInstance::Source m_Source;
	STLVector<V3DAdapter*> m_Adapters;
	STLVector<V3DInstance::Window> m_Windows;
#ifdef _DEBUG
	VkDebugReportCallbackEXT m_DebugReportCallbackEXT;
#endif //_DEBUG

	V3DInstance();
	virtual ~V3DInstance();

	V3DInstance::Window* GetWindow(HWND windowHandle);

	static VKAPI_ATTR void* VKAPI_CALL Alloc(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope);
	static VKAPI_ATTR void* VKAPI_CALL Realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope);
	static VKAPI_ATTR void VKAPI_CALL Free(void* pUserData, void* pMemory);

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM eparam, LPARAM lparam);

#ifdef _DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallbackEXT(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objectType,
		uint64_t object,
		size_t location,
		int32_t messageCode,
		const char* pLayerPrefix,
		const char* pMessage,
		void* pUserData);
#endif //_DEBUG

	V3D_DECLARE_ALLOCATOR
};