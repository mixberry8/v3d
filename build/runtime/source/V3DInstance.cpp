#include "V3DInstance.h"
#include "V3DAdapter.h"
#include "V3DDevice.h"
#include "V3DSwapChain.h"
#include <algorithm>

V3DInstance* V3DInstance::s_pThis = nullptr;

/**********************/
/* public V3DInstance */
/**********************/

bool V3DInstance::IsCreated()
{
	return V3DInstance::s_pThis != nullptr;
}

V3DInstance* V3DInstance::Create()
{
	return V3D_NEW_T(V3DInstance);
}

V3D_RESULT V3DInstance::Initialize(const V3DInstanceDesc& instanceDesc)
{
	m_LayerType = instanceDesc.layerType;

	// ----------------------------------------------------------------------------------------------------
	// ログの初期化
	// ----------------------------------------------------------------------------------------------------

	V3D_LOG_INIT(instanceDesc.log.flags, instanceDesc.log.pFunction, instanceDesc.log.pUserData);

	// ----------------------------------------------------------------------------------------------------
	// アロケーター
	// ----------------------------------------------------------------------------------------------------

	m_AllocationCallbacks.pfnAllocation = V3DInstance::Alloc;
	m_AllocationCallbacks.pfnReallocation = V3DInstance::Realloc;
	m_AllocationCallbacks.pfnFree = V3DInstance::Free;

	// ----------------------------------------------------------------------------------------------------
	// 有効にするレイヤーを列挙
	// ----------------------------------------------------------------------------------------------------

	uint32_t layerPropCount;
	STLVector<VkLayerProperties> layerProps;

	VkResult result = vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr);
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	layerProps.resize(layerPropCount);

	result = vkEnumerateInstanceLayerProperties(&layerPropCount, layerProps.data());
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<const char*> enableLayers;

	if (m_LayerType == V3D_LAYER_STANDARD_VALIDATION)
	{
		if (std::find_if(layerProps.begin(), layerProps.end(), V3DFindLayer(V3D_LAYER_LUNARG_standard_validation)) != layerProps.end())
		{
			enableLayers.push_back(V3D_LAYER_LUNARG_standard_validation);
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 有効にするエクステンションを列挙
	// ----------------------------------------------------------------------------------------------------

	uint32_t extensionPropCount;
	STLVector<VkExtensionProperties> extensionProps;

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropCount, nullptr);
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	extensionProps.resize(extensionPropCount);

	result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropCount, extensionProps.data());
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<char*> enabledExtensions;
	enabledExtensions.reserve(extensionPropCount);
	for (uint32_t i = 0; i < extensionPropCount; i++)
	{
		enabledExtensions.push_back(extensionProps[i].extensionName);
	}

	// ----------------------------------------------------------------------------------------------------
	// インスタンスを作成
	// ----------------------------------------------------------------------------------------------------

	VkApplicationInfo appInfo = {};
	VkInstanceCreateInfo instInfo = {};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "v3d";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "v3d";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instInfo.pNext = nullptr;
	instInfo.flags = 0;
	instInfo.pApplicationInfo = &appInfo;
	instInfo.enabledLayerCount = static_cast<uint32_t>(enableLayers.size());
	instInfo.ppEnabledLayerNames = enableLayers.data();
	instInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
	instInfo.ppEnabledExtensionNames = enabledExtensions.data();

	result = vkCreateInstance(&instInfo, &m_AllocationCallbacks, &m_Source.instance);
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバッグレポート用のコールバックを登録
	// ----------------------------------------------------------------------------------------------------

#ifdef _DEBUG

	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Source.instance, "vkCreateDebugReportCallbackEXT");
	if (dbgCreateDebugReportCallback == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {};
	dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	dbgCreateInfo.pNext = nullptr;
	dbgCreateInfo.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
	dbgCreateInfo.pfnCallback = V3DInstance::DebugReportCallbackEXT;
	dbgCreateInfo.pUserData = nullptr;

	result = dbgCreateDebugReportCallback(m_Source.instance, &dbgCreateInfo, nullptr, &m_DebugReportCallbackEXT);
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// 物理デバイスの列挙
	// ----------------------------------------------------------------------------------------------------

	uint32_t physicalDeviceCount = 0;
	STLVector<VkPhysicalDevice> physicalDevices;

	result = vkEnumeratePhysicalDevices(m_Source.instance, &physicalDeviceCount, nullptr);
	if ((result != VK_SUCCESS) || (physicalDeviceCount < 1))
	{
		return V3D_ERROR_FAIL;
	}

	physicalDevices.resize(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(m_Source.instance, &physicalDeviceCount, physicalDevices.data());
	if (result != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	m_Adapters.reserve(physicalDeviceCount);

	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		V3DAdapter* pAdapter = V3DAdapter::Create();
		if (pAdapter == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pAdapter->Initialize(physicalDevices[i]);
		if (result != V3D_OK)
		{
			pAdapter->Release();
			return result;
		}

		m_Adapters.push_back(pAdapter);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DInstance::Source& V3DInstance::GetSource() const
{
	return m_Source;
}

void V3DInstance::AddWindow(V3DSwapChain* pSwapChain)
{
	V3D_ASSERT(pSwapChain != nullptr);

	HWND windowHandle = pSwapChain->GetDesc().windowHandle;

#ifdef _DEBUG
	STLVector<V3DInstance::Window>::iterator it_debug = std::find_if(m_Windows.begin(), m_Windows.end(), [windowHandle](const V3DInstance::Window& window) { return window.handle == windowHandle; });
	V3D_ASSERT(it_debug == m_Windows.end());
#endif //_DEBUG

	WNDPROC pWindowProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(windowHandle, GWLP_WNDPROC));
	::SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(V3DInstance::WindowProc));

	V3DInstance::Window window;
	window.handle = windowHandle;
	window.pProc = pWindowProc;
	window.pSwapChain = pSwapChain;
	m_Windows.push_back(window);
}

void V3DInstance::RemoveWindow(V3DSwapChain* pSwapChain)
{
	V3D_ASSERT(pSwapChain != nullptr);

	HWND windowHandle = pSwapChain->GetDesc().windowHandle;
	STLVector<V3DInstance::Window>::iterator it_window = std::find_if(m_Windows.begin(), m_Windows.end(), [windowHandle](const V3DInstance::Window& window) { return window.handle == windowHandle; });

	if (it_window != m_Windows.end())
	{
		V3DInstance::Window* pWindow = &(*it_window);

		::SetWindowLongPtr(windowHandle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pWindow->pProc));

		if (pWindow->pSwapChain->GetFullscreenState() == true)
		{
			pWindow->pSwapChain->SetFullscreenState(false);
		}

		m_Windows.erase(it_window);
	}
}

bool V3DInstance::ExistsFulscreenWindow(V3DSwapChain* pSwapChain)
{
	V3D_ASSERT(pSwapChain != nullptr);

	HWND windowHandle = pSwapChain->GetDesc().windowHandle;
	STLVector<V3DInstance::Window>::iterator it_window = std::find_if(m_Windows.begin(), m_Windows.end(), [windowHandle](const V3DInstance::Window& window) { return (window.handle != windowHandle) && (window.pSwapChain->GetFullscreenState() == true); });

	return (it_window != m_Windows.end());
}

/********************************/
/* public override IV3DInstance */
/********************************/

V3D_LAYER_TYPE V3DInstance::GetLayerType() const
{
	return m_LayerType;
}

uint32_t V3DInstance::GetAdapterCount() const
{
	return static_cast<uint32_t>(m_Adapters.size());
}

void V3DInstance::GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter)
{
	(*ppAdapter) = V3D_TO_ADD_REF(static_cast<IV3DAdapter*>(m_Adapters[adapterIndex]));
}

V3D_RESULT V3DInstance::CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice)
{
	if (pAdapter == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DDevice* pDevice = V3DDevice::Create();
	if (pDevice == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pDevice->Initialize(this, pAdapter);
	if (result != V3D_OK)
	{
		pDevice->Release();
		return result;
	}

	(*ppDevice) = pDevice;

	return V3D_OK;
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DInstance::GetRefCount() const
{
	return m_RefCounter;
}

void V3DInstance::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DInstance::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DInstance);
	}
}

/*************************/
/* private - V3DInstance */
/*************************/

V3DInstance::V3DInstance() :
	m_RefCounter(1),
	m_AllocationCallbacks({ nullptr }),
	m_Source({})
{
	V3DInstance::s_pThis = this;

	m_Source.instance = VK_NULL_HANDLE;
}

V3DInstance::~V3DInstance()
{
	V3DInstance::s_pThis = nullptr;

	if (m_Source.instance != VK_NULL_HANDLE)
	{
		for (size_t i = 0; i < m_Adapters.size(); i++)
		{
			m_Adapters[i]->Release();
		}

#ifdef _DEBUG
		PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Source.instance, "vkDestroyDebugReportCallbackEXT");
		if ((dbgDestroyDebugReportCallback != nullptr) && (m_DebugReportCallbackEXT != VK_NULL_HANDLE))
		{
			dbgDestroyDebugReportCallback(m_Source.instance, m_DebugReportCallbackEXT, nullptr);
		}
#endif //_DEBUG

		vkDestroyInstance(m_Source.instance, &m_AllocationCallbacks);
	}

	V3D_LOG_FIN();
}

V3DInstance::Window* V3DInstance::GetWindow(HWND windowHandle)
{
	STLVector<V3DInstance::Window>::iterator it = std::find_if(m_Windows.begin(), m_Windows.end(), [windowHandle](const V3DInstance::Window& window) { return window.handle == windowHandle; });
	V3D_ASSERT(it != m_Windows.end());
	return &(*it);
}

VKAPI_ATTR void* VKAPI_CALL V3DInstance::Alloc(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
	return V3D_ALIGNED_MALLOC(size, alignment);
}

VKAPI_ATTR void* VKAPI_CALL V3DInstance::Realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope scope)
{
	return V3D_ALIGNED_REALLOC(pOriginal, size, alignment);
}

VKAPI_ATTR void VKAPI_CALL V3DInstance::Free(void* pUserData, void* pMemory)
{
	V3D_ALIGNED_FREE(pMemory);
}

LRESULT CALLBACK V3DInstance::WindowProc(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
	V3DInstance::Window* pWindow = V3DInstance::s_pThis->GetWindow(windowHandle);
	WNDPROC pWindowProc = pWindow->pProc;
	V3DSwapChain* pSwapChain = pWindow->pSwapChain;

	switch (message)
	{
	case WM_DESTROY:
		V3DInstance::s_pThis->RemoveWindow(pSwapChain);
		break;

	case WM_SYSCOMMAND:
		if (wparam == SC_MAXIMIZE)
		{
			if ((pSwapChain->GetDesc().fullscreenAssistEnable == true) && (pSwapChain->SetFullscreenState(pSwapChain->GetDesc().windowed) == V3D_OK))
			{
				// 最大化ボタンでフルスクリーン化する場合は、最大化アニメーションをスキップする
				return 0;
			}
		}
		else if (wparam == SC_MINIMIZE)
		{
			if (pSwapChain->GetFullscreenState() == true)
			{
				// フルスクリーン中に最小化
				// WM_ACTIVATEAPP で最小化するため、最小化アニメーションをスキップする
				return 0;
			}
		}
		else if (wparam == SC_RESTORE)
		{
			if (pSwapChain->SetIconic(false) == V3D_OK)
			{
				// フルスクリーン中に最小化されている状態から復帰する場合は、復帰アニメーションをスキップする
				return 0;
			}
		}
		break;

	case WM_ACTIVATEAPP:
		if (wparam == FALSE)
		{
			// フルスクリーン中に非アクティブになった場合は、アイコン化する
			pSwapChain->SetIconic(true);
		}
		break;

	case WM_SIZE:
		// イメージのサイズを変更
		pSwapChain->ResizeImage();
		break;

	case WM_SYSKEYDOWN:
		if ((pSwapChain->GetDesc().fullscreenAssistEnable == true) && (wparam == VK_RETURN) && ((lparam & (1 << 30)) == 0))
		{
			// ALT + ENTER によるフルスクリーン状態の変更
			pSwapChain->SetFullscreenState(pSwapChain->GetDesc().windowed);
		}
		break;

	case WM_NCLBUTTONDBLCLK:
		if (wparam == HTCAPTION)
		{
			if ((pSwapChain->GetDesc().fullscreenAssistEnable == true) && (pSwapChain->SetFullscreenState(true) == V3D_OK))
			{
				// キャプションのダブルクリックによる場合のフルスクリーン化の場合は、最大化アニメーションをスキップ
				return 0;
			}
		}
		break;
	}

	return ::CallWindowProc(pWindowProc, windowHandle, message, wparam, lparam);
}

#ifdef _DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL V3DInstance::DebugReportCallbackEXT(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData)
{
	static constexpr char* objectNameTable[] = 
	{
		"UNKNOWN",
		"INSTANCE",
		"PHYSICAL_DEVICE",
		"DEVICE",
		"QUEUE",
		"SEMAPHORE",
		"COMMAND_BUFFER",
		"FENCE",
		"DEVICE_MEMORY",
		"BUFFER",
		"IMAGE",
		"EVENT",
		"QUERY_POOL",
		"BUFFER_VIEW",
		"IMAGE_VIEW",
		"SHADER_MODULE",
		"PIPELINE_CACHE",
		"PIPELINE_LAYOUT",
		"RENDER_PASS",
		"PIPELINE",
		"DESCRIPTOR_SET_LAYOUT",
		"SAMPLER",
		"DESCRIPTOR_POOL",
		"DESCRIPTOR_SET",
		"FRAMEBUFFER",
		"COMMAND_POOL",
		"SURFACE_KHR",
		"SWAPCHAIN_KHR",
		"DEBUG_REPORT",
		"DISPLAY_KHR",
		"DISPLAY_MODE_KHR",
		"OBJECT_TABLE_NVX",
		"INDIRECT_COMMANDS_LAYOUT_NVX",
	};

	static auto constexpr objectNameCount = sizeof(objectNameTable) / sizeof(char*);

	V3D_LOG_FLAG logType;
	switch (flags)
	{
	case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		logType = V3D_LOG_INFORMATION;
		break;
	case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		logType = V3D_LOG_WARNING;
		break;
	case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
		logType = V3D_LOG_PERFORMANCE_WARNING;
		break;
	case VK_DEBUG_REPORT_ERROR_BIT_EXT:
		logType = V3D_LOG_ERROR;
		break;
	case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
		logType = V3D_LOG_DEBUG;
		break;

	default:
		logType = V3D_LOG_ERROR;
	}
	
	char* pObjectName;
	if (objectNameCount > objectType)
	{
		pObjectName = objectNameTable[objectType];
	}
	else
	{
		pObjectName = objectNameTable[0];
	}

#ifdef _WIN64
	V3D_LOG_A(logType, "object[%s] location[%I64u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectName, location, messageCode, pLayerPrefix, pMessage);
#else //_WIN64
	V3D_LOG_A(logType, "object[%s] location[%u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectName, location, messageCode, pLayerPrefix, pMessage);
#endif //_WIN64

	return VK_FALSE;
}

#endif //_DEBUG