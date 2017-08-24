#include "V3DInstance.h"
#include "V3DAdapter.h"
#include "V3DDevice.h"
#include "V3DSwapChain.h"
#include <algorithm>

#ifdef V3D_DEBUG
#include "ScopedLock.h"
#endif //V3D_DEBUG

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
	m_Layer = instanceDesc.layer;

	// ----------------------------------------------------------------------------------------------------
	// ログの初期化
	// ----------------------------------------------------------------------------------------------------

	V3D_LOG_INIT(instanceDesc.log.flags, instanceDesc.log.pFunction, instanceDesc.log.pUserData);

#ifdef V3D_DEBUG
	// 定数
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_UNDEFINED"] = "V3D_IMAGE_LAYOUT_UNDEFINED";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_GENERAL"] = "V3D_IMAGE_LAYOUT_GENERAL";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL"] = "V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL"] = "V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL"] = "V3D_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL"] = "V3D_IMAGE_LAYOUT_SHADER_READ_ONLY";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL"] = "V3D_IMAGE_LAYOUT_TRANSFER_SRC";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"] = "V3D_IMAGE_LAYOUT_TRANSFER_DST";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_PREINITIALIZED"] = "V3D_IMAGE_LAYOUT_PREINITIALIZED";
	m_DebugConstantNameMap["VK_IMAGE_LAYOUT_PRESENT_SRC_KHR"] = "V3D_IMAGE_LAYOUT_PRESENT_SRC";

	m_DebugConstantNameMap["VK_PIPELINE_BIND_POINT_GRAPHICS"] = "V3D_PIPELINE_TYPE_GRAPHICS";
	m_DebugConstantNameMap["VK_PIPELINE_BIND_POINT_COMPUTE"] = "V3D_PIPELINE_TYPE_COMPUTE";

	m_DebugConstantNameMap["VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT"] = "V3D_PIPELINE_STAGE_TOP_OF_PIPE";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT"] = "V3D_PIPELINE_STAGE_DRAW_INDIRECT";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_VERTEX_INPUT_BIT"] = "V3D_PIPELINE_STAGE_VERTEX_INPUT";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_VERTEX_SHADER_BIT"] = "V3D_PIPELINE_STAGE_VERTEX_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT"] = "V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT"] = "V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT"] = "V3D_PIPELINE_STAGE_GEOMETRY_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT"] = "V3D_PIPELINE_STAGE_FRAGMENT_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT"] = "V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT"] = "V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT"] = "V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT"] = "V3D_PIPELINE_STAGE_COMPUTE_SHADER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_TRANSFER_BIT"] = "V3D_PIPELINE_STAGE_TRANSFER";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT"] = "V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_HOST_BIT"] = "V3D_PIPELINE_STAGE_HOST";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT"] = "V3D_PIPELINE_STAGE_ALL_GRAPHICS";
	m_DebugConstantNameMap["VK_PIPELINE_STAGE_ALL_COMMANDS_BIT"] = "V3D_PIPELINE_STAGE_ALL_COMMANDS";

	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_SAMPLER"] = "V3D_DESCRIPTOR_TYPE_SAMPLER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER"] = "V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE"] = "V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_STORAGE_IMAGE"] = "V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER"] = "V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER"] = "V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER"] = "V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_STORAGE_BUFFER"] = "V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC"] = "V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC"] = "V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC";
	m_DebugConstantNameMap["VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT"] = "V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT";

	// オブジェクトハンドル
	m_DebugHandleNameMap["VkPipeline"] = "IV3DPipeline ( VkPipeline )";

	// 関数 : デスクリプタセット
	m_DebugFunctionNameMap["vkUpdateDescriptorSets"] = "IV3DDescriptorSet::Update";

	// 関数 : コマンドバッファー
	m_DebugFunctionNameMap["vkBeginCommandBuffer"] = "IV3DCommandBuffer::Begin ( vkBeginCommandBuffer )";
	m_DebugFunctionNameMap["vkEndCommandBuffer"] = "IV3DCommandBuffer::End ( vkEndCommandBuffer )";
	m_DebugFunctionNameMap["vkCmdPipelineBarrier"] = "IV3DCommandBuffer::Barrier??? ( vkCmdPipelineBarrier )";
	m_DebugFunctionNameMap["vkCmdCopyBuffer"] = "IV3DCommandBuffer::CopyBuffer ( vkCmdCopyBuffer )";
	m_DebugFunctionNameMap["vkCmdCopyImage"] = "IV3DCommandBuffer::CopyImage ( vkCmdCopyImage )";
	m_DebugFunctionNameMap["vkCmdCopyBufferToImage"] = "IV3DCommandBuffer::CopyBufferToImage ( vkCmdCopyBufferToImage )";
	m_DebugFunctionNameMap["vkCmdCopyImageToBuffer"] = "IV3DCommandBuffer::CopyImageToBuffer ( vkCmdCopyImageToBuffer )";
	m_DebugFunctionNameMap["vkCmdBlitImage"] = "IV3DCommandBuffer::BlitImage??? ( vkCmdBlitImage )";
	m_DebugFunctionNameMap["vkCmdResolveImage"] = "IV3DCommandBuffer::ResolveImage??? ( vkCmdResolveImage )";
	m_DebugFunctionNameMap["vkCmdBeginRenderPass"] = "IV3DCommandBuffer::BeginRenderPass ( vkCmdBeginRenderPass )";
	m_DebugFunctionNameMap["vkCmdEndRenderPass"] = "IV3DCommandBuffer::EndRenderPass ( vkCmdEndRenderPass )";
	m_DebugFunctionNameMap["vkCmdNextSubpass"] = "IV3DCommandBuffer::EndRenderPass ( vkCmdNextSubpass )";
	m_DebugFunctionNameMap["vkCmdClearColorImage"] = "IV3DCommandBuffer::ClearImage ( vkCmdClearColorImage )";
	m_DebugFunctionNameMap["vkCmdClearDepthStencilImage"] = "IV3DCommandBuffer::ClearImage ( vkCmdClearDepthStencilImage )";
	m_DebugFunctionNameMap["vkCmdClearAttachments"] = "IV3DCommandBuffer::ClearAttachments ( vkCmdClearAttachments )";
	m_DebugFunctionNameMap["vkCmdBindPipeline"] = "IV3DCommandBuffer::BindPipeline ( vkCmdBindPipeline )";
	m_DebugFunctionNameMap["vkCmdBindDescriptorSets"] = "IV3DCommandBuffer::BindDescriptorSets ( vkCmdBindDescriptorSets )";
	m_DebugFunctionNameMap["vkCmdBindVertexBuffers"] = "IV3DCommandBuffer::BindVertexBuffers ( vkCmdBindVertexBuffers )";
	m_DebugFunctionNameMap["vkCmdBindIndexBuffer"] = "IV3DCommandBuffer::BindIndexBuffer ( vkCmdBindIndexBuffer )";
	m_DebugFunctionNameMap["vkCmdPushConstants"] = "IV3DCommandBuffer::PushConstant ( vkCmdPushConstants )";
	m_DebugFunctionNameMap["vkCmdSetViewport"] = "IV3DCommandBuffer::SetViewport ( vkCmdSetViewport )";
	m_DebugFunctionNameMap["vkCmdSetScissor"] = "IV3DCommandBuffer::SetScissor ( vkCmdSetScissor )";
	m_DebugFunctionNameMap["vkCmdSetBlendConstants"] = "IV3DCommandBuffer::SetBlendConstants ( vkCmdSetBlendConstants )";
	m_DebugFunctionNameMap["vkCmdSetStencilReference"] = "IV3DCommandBuffer::SetStencilReference ( vkCmdSetStencilReference )";
	m_DebugFunctionNameMap["vkCmdResetQueryPool"] = "IV3DCommandBuffer::ResetQueryPool ( vkCmdResetQueryPool )";
	m_DebugFunctionNameMap["vkCmdBeginQuery"] = "IV3DCommandBuffer::BeginQuery ( vkCmdBeginQuery )";
	m_DebugFunctionNameMap["vkCmdEndQuery"] = "IV3DCommandBuffer::EndQuery ( vkCmdEndQuery )";
	m_DebugFunctionNameMap["vkCmdWriteTimestamp"] = "IV3DCommandBuffer::WriteTimestamp ( vkCmdWriteTimestamp )";
	m_DebugFunctionNameMap["vkCmdDraw"] = "IV3DCommandBuffer::Draw ( vkCmdDraw )";
	m_DebugFunctionNameMap["vkCmdDrawIndexed"] = "IV3DCommandBuffer::DrawIndexed ( vkCmdDrawIndexed )";
	m_DebugFunctionNameMap["vkCmdDispatch"] = "IV3DCommandBuffer::Dispatch ( vkCmdDispatch )";
	m_DebugFunctionNameMap["vkCmdExecuteCommands"] = "IV3DCommandBuffer::ExecuteCommandBuffers ( vkCmdExecuteCommands )";
#endif //V3D_DEBUG

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
	switch (m_Layer)
	{
	case V3D_LAYER_VALIDATION:
		if (std::find_if(layerProps.begin(), layerProps.end(), V3DFindLayer(V3D_LAYER_LUNARG_standard_validation)) != layerProps.end())
		{
			enableLayers.push_back(V3D_LAYER_LUNARG_standard_validation);
		}
		break;

	case V3D_LAYER_NSIGHT:
//		if (std::find_if(layerProps.begin(), layerProps.end(), V3DFindLayer(V3D_LAYER_NV_nsight)) != layerProps.end())
//		{
//			enableLayers.push_back(V3D_LAYER_NV_nsight);
//		}
		break;

	case V3D_LAYER_RENDERDOC:
		if (std::find_if(layerProps.begin(), layerProps.end(), V3DFindLayer(V3D_LAYER_RENDERDOC_Capture)) != layerProps.end())
		{
			enableLayers.push_back(V3D_LAYER_RENDERDOC_Capture);
		}
		break;
	}

	// ----------------------------------------------------------------------------------------------------
	// 有効にするエクステンションを列挙
	// ----------------------------------------------------------------------------------------------------

	const char* EXTENSION_surface = VK_KHR_SURFACE_EXTENSION_NAME;
	const char* EXTENSION_win32_surface = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	const char* EXTENSION_get_physical_device_properties2 = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
#ifdef V3D_DEBUG
	const char* EXTENSION_debug_report = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
#endif //V3D_DEBUG

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

	STLVector<const char*> enabledExtensions;
	enabledExtensions.reserve(extensionPropCount);

	if (std::find_if(extensionProps.begin(), extensionProps.end(), V3DFindExtension(EXTENSION_surface)) != extensionProps.end())
	{
		enabledExtensions.push_back(EXTENSION_surface);
	}
	else
	{
		return V3D_ERROR_FAIL;
	}

	if (std::find_if(extensionProps.begin(), extensionProps.end(), V3DFindExtension(EXTENSION_win32_surface)) != extensionProps.end())
	{
		enabledExtensions.push_back(EXTENSION_win32_surface);
	}
	else
	{
		return V3D_ERROR_FAIL;
	}

	if (std::find_if(extensionProps.begin(), extensionProps.end(), V3DFindExtension(EXTENSION_get_physical_device_properties2)) != extensionProps.end())
	{
		enabledExtensions.push_back(EXTENSION_get_physical_device_properties2);
	}
	else
	{
		return V3D_ERROR_FAIL;
	}

#ifdef V3D_DEBUG
	bool debugReportEnable = false;
	if (std::find_if(extensionProps.begin(), extensionProps.end(), V3DFindExtension(EXTENSION_debug_report)) != extensionProps.end())
	{
		enabledExtensions.push_back(EXTENSION_debug_report);
		debugReportEnable = true;
	}

	for (size_t i = 0; i < enableLayers.size(); i++)
	{
		V3D_LOG_PRINT_DEBUG_A(Log_Debug_InstanceLayer, enableLayers[i]);
	}

	for (size_t i = 0; i < enabledExtensions.size(); i++)
	{
		V3D_LOG_PRINT_DEBUG_A(Log_Debug_InstanceExtension, enabledExtensions[i]);
	}
#endif //V3D_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// インスタンスを作成
	// ----------------------------------------------------------------------------------------------------

	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo instInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "v3d";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 6);
	appInfo.pEngineName = "v3d";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 6);
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

	V3D_ADD_DEBUG_OBJECT(this, m_Source.instance, L"Instance");

	// ----------------------------------------------------------------------------------------------------
	// デバッグレポート用のコールバックを登録
	// ----------------------------------------------------------------------------------------------------

#ifdef V3D_DEBUG
	if ((instanceDesc.log.flags != 0) && (debugReportEnable == true))
	{
		PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Source.instance, "vkCreateDebugReportCallbackEXT");
		if (dbgCreateDebugReportCallback == nullptr)
		{
			return V3D_ERROR_FAIL;
		}

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo{};
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pNext = nullptr;
		dbgCreateInfo.flags = 0;
		dbgCreateInfo.pfnCallback = V3DInstance::DebugReportCallbackEXT;
		dbgCreateInfo.pUserData = this;

		if (instanceDesc.log.flags & V3D_LOG_INFORMATION) { dbgCreateInfo.flags |= VK_DEBUG_REPORT_INFORMATION_BIT_EXT; }
		if (instanceDesc.log.flags & V3D_LOG_WARNING) { dbgCreateInfo.flags |= VK_DEBUG_REPORT_WARNING_BIT_EXT; }
		if (instanceDesc.log.flags & V3D_LOG_PERFORMANCE_WARNING) { dbgCreateInfo.flags |= VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT; }
		if (instanceDesc.log.flags & V3D_LOG_ERROR) { dbgCreateInfo.flags |= VK_DEBUG_REPORT_ERROR_BIT_EXT; }
		if (instanceDesc.log.flags & V3D_LOG_DEBUG) { dbgCreateInfo.flags |= VK_DEBUG_REPORT_DEBUG_BIT_EXT; }

		result = dbgCreateDebugReportCallback(m_Source.instance, &dbgCreateInfo, nullptr, &m_DebugReportCallbackEXT);
		if (result != VK_SUCCESS)
		{
			return V3D_ERROR_FAIL;
		}
	}
#endif //V3D_DEBUG

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

		V3D_RESULT result = pAdapter->Initialize(this, physicalDevices[i]);
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

#ifdef V3D_DEBUG
	STLVector<V3DInstance::Window>::iterator it_debug = std::find_if(m_Windows.begin(), m_Windows.end(), [windowHandle](const V3DInstance::Window& window) { return window.handle == windowHandle; });
	V3D_ASSERT(it_debug == m_Windows.end());
#endif //V3D_DEBUG

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

#ifdef V3D_DEBUG

void V3DInstance::AddDebugObject(void* pVulkanObject, const wchar_t* pName)
{
	AddDebugObject(reinterpret_cast<uint64_t>(pVulkanObject), pName);
}

void V3DInstance::AddDebugObject(uint64_t vulkanObject, const wchar_t* pName)
{
	ScopedLock lock(m_DebugSync);

	V3D_ASSERT(pName != nullptr);

	STLStringA nameA;
	ToMultibyteString(pName, nameA);

	char addrStrA[19];
#ifdef V3D64
	sprintf_s(addrStrA, "0x%.16I64x", vulkanObject);
#else //V3D64
	uint32_t vulkanObject32 = static_cast<uint32_t>(vulkanObject);
	sprintf_s(addrStrA, "0x%.8x", vulkanObject32);
#endif //V3D64

	STLStringStreamA compNameA;
	compNameA << nameA << "[" << addrStrA << "]";

	m_DebugObjectNameMap[vulkanObject] = compNameA.str();
}

void V3DInstance::RemoveDebugObject(void* pVulkanObject)
{
	RemoveDebugObject(reinterpret_cast<uint64_t>(pVulkanObject));
}

void V3DInstance::RemoveDebugObject(uint64_t vulkanObject)
{
	ScopedLock lock(m_DebugSync);

	V3DInstance::DebugObjectNameMap::iterator it = m_DebugObjectNameMap.find(vulkanObject);

	if (it != m_DebugObjectNameMap.end())
	{
		m_DebugObjectNameMap.erase(it);
	}
}

void V3DInstance::AddDebugMemoryObject(void* pInterface, V3D_DEBUG_OBJECT_TYPE type, const wchar_t* pName)
{
	V3D_ASSERT(pInterface != nullptr);
	V3D_ASSERT(pName != nullptr);

	ScopedLock lock(m_DebugSync);

	V3DInstance::DebugObject debugObject;
	debugObject.type = type;
	debugObject.name = pName;

	m_DebugMemoryObjectMap[pInterface] = debugObject;
}

void V3DInstance::RemoveDebugMemoryObject(void* pInterface)
{
	V3D_ASSERT(pInterface != nullptr);

	ScopedLock lock(m_DebugSync);

	auto it = m_DebugMemoryObjectMap.find(pInterface);
	V3D_ASSERT(it != m_DebugMemoryObjectMap.end());

	m_DebugMemoryObjectMap.erase(it);
}

#endif //V3D_DEBUG

/********************************/
/* public override IV3DInstance */
/********************************/

V3D_LAYER V3DInstance::GetLayer() const
{
	return m_Layer;
}

uint32_t V3DInstance::GetAdapterCount() const
{
	return static_cast<uint32_t>(m_Adapters.size());
}

void V3DInstance::GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter)
{
	(*ppAdapter) = V3D_TO_ADD_REF(static_cast<IV3DAdapter*>(m_Adapters[adapterIndex]));
}

V3D_RESULT V3DInstance::CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice, const wchar_t* pDebugName)
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


	V3D_RESULT result = pDevice->Initialize(this, pAdapter, pDebugName);
	if (result != V3D_OK)
	{
		pDevice->Release();
		return result;
	}

	(*ppDevice) = pDevice;

	return V3D_OK;
}

void V3DInstance::DumpObjects()
{
#ifdef V3D_DEBUG
	ScopedLock lock(m_DebugSync);
	DumpObjects(V3D_LOG_DEBUG);
#endif //V3D_DEBUG
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
	m_Source({})
{
	V3DInstance::s_pThis = this;

	m_AllocationCallbacks = {};

	m_Source.instance = VK_NULL_HANDLE;

	V3D_DEBUG_CODE(m_DebugReportCallbackEXT = VK_NULL_HANDLE);
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

#ifdef V3D_DEBUG
		PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Source.instance, "vkDestroyDebugReportCallbackEXT");
		if ((dbgDestroyDebugReportCallback != nullptr) && (m_DebugReportCallbackEXT != VK_NULL_HANDLE))
		{
			dbgDestroyDebugReportCallback(m_Source.instance, m_DebugReportCallbackEXT, nullptr);
		}
#endif //V3D_DEBUG

		vkDestroyInstance(m_Source.instance, &m_AllocationCallbacks);

		V3D_REMOVE_DEBUG_OBJECT(this, m_Source.instance);
	}

	V3D_ASSERT(m_DebugObjectNameMap.size() == 0);

#ifdef V3D_DEBUG
	m_DebugSync.Enter();
	if (m_DebugMemoryObjectMap.empty() == false)
	{
		V3D_LOG_PRINT_ERROR(L"!!! MemoryLeaks !!!");
		DumpObjects(V3D_LOG_ERROR);
	}
	m_DebugSync.Leave();
#endif //V3D_DEBUG

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

#ifdef V3D_DEBUG

const char* V3DInstance::GetDebugObjectName(uint64_t objectAddr)
{
	V3DInstance::DebugObjectNameMap::iterator it = m_DebugObjectNameMap.find(objectAddr);
	if (it != m_DebugObjectNameMap.end())
	{
		return it->second.c_str();
	}

#ifdef V3D64
	sprintf_s(m_DebugObjectAddr, "Unknown[0x%.16I64x]", objectAddr);
#else //V3D64
	uint32_t objectAddr32 = static_cast<uint32_t>(objectAddr);
	sprintf_s(m_DebugObjectAddr, "Unknown[0x%.8x]", objectAddr32);
#endif //V3D64

	return m_DebugObjectAddr;
}

const char* V3DInstance::ConvertDebugMessage(const char* pMessage)
{
	const char* pSrc = pMessage;
	char temp[4096] = {};
	uint32_t tempCount = 0;

	m_DebugStringStream.str("");

	do
	{
		char code = *pSrc++;

		if ((code == ' ') || (code == '\t') || (code == '.') || (code == '[') || (code == ']') || (code == '(') || (code == ')'))
		{
			if (tempCount > 0)
			{
				m_DebugStringStream << ConvertDebugString(temp);
				memset(temp, 0, sizeof(temp));
				tempCount = 0;
			}

			m_DebugStringStream << code;
		}
		else
		{
			V3D_ASSERT(tempCount < sizeof(temp));
			temp[tempCount++] = code;
		}

	} while (*pSrc != '\0');

	if (tempCount > 0)
	{
		m_DebugStringStream << ConvertDebugString(temp);
		memset(temp, 0, sizeof(temp));
		tempCount = 0;
	}

	m_DebugString = m_DebugStringStream.str();

	return m_DebugString.c_str();
}

const char* V3DInstance::ConvertDebugString(const char* pString)
{
	size_t count = strlen(pString);

	if ((count > 2) && (pString[0] == '0') && (pString[1] == 'x'))
	{
		// オブジェクトのアドレス
		char* endPtr;
		uint64_t objectAddr = strtoll(pString, &endPtr, 0);
		if (objectAddr != 0)
		{
			return GetDebugObjectName(objectAddr);
		}
	}
	else if ((count > 2) && (pString[0] == 'V') && (pString[1] == 'k'))
	{
		// オブジェクトのハンドル名
		auto it_name = m_DebugHandleNameMap.find(pString);
		if (it_name != m_DebugHandleNameMap.end())
		{
			return it_name->second.c_str();
		}
	}
	else if ((count > 2) && (pString[0] == 'v') && (pString[1] == 'k'))
	{
		// 関数名
		auto it_name = m_DebugFunctionNameMap.find(pString);
		if (it_name != m_DebugFunctionNameMap.end())
		{
			return it_name->second.c_str();
		}
	}
	else if ((count > 3) && (pString[0] == 'V') && (pString[1] == 'K') && (pString[2] == '_'))
	{
		// 定数
		auto it_name = m_DebugConstantNameMap.find(pString);
		if (it_name != m_DebugConstantNameMap.end())
		{
			return it_name->second.c_str();
		}
	}

	return pString;
}

void V3DInstance::DumpObjects(V3D_LOG_FLAG type)
{
	const wchar_t* objectTypeTable[V3D_DEBUG_OBJECT_TYPE_COUNT] =
	{
		L"ADAPTER",
		L"BACK_BUFFER",
		L"BARRIER_SET",
		L"BUFFER",
		L"BUFFER_VIEW",
		L"COMMAND_BUFFER",
		L"COMMAND_POOL",
		L"COMPUTE_PIPELINE",
		L"DEVICE",
		L"EVENT",
		L"FENCE",
		L"FRAME_BUFFER",
		L"GRAPHICS_PIPELINE",
		L"IMAGE",
		L"IMAGE_VIEW",
		L"PIPELINE_LAYOUT",
		L"PUSH_DESCRIPTOR_SET",
		L"PUSH_DESCRIPTOR_SET_LAYOUT",
		L"QUERY_POOL",
		L"QUEUE",
		L"RENDER_PASS",
		L"RESOURCE_MEMORY",
		L"SAMPLER",
		L"SEMAPHORE",
		L"SHADER_MODULE",
		L"STANDARD_DESCRIPTOR_SET",
		L"STANDARD_DESCRIPTOR_SET_LAYOUT",
		L"SWAPCHAIN",
	};

	auto it_begin = m_DebugMemoryObjectMap.begin();
	auto it_end = m_DebugMemoryObjectMap.end();

	for (auto it = it_begin; it != it_end; ++it)
	{
		V3D_LOG_PRINT_W(type, L"%s : %s", objectTypeTable[it->second.type], it->second.name.c_str());
	}
}

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
	V3DInstance* pInstance = static_cast<V3DInstance*>(pUserData);

	static constexpr char* objectNameTable[] =
	{
		"Unknown",
		"Instance",
		"Adapter",
		"Device",
		"Queue",
		"Semaphore",
		"CommandBuffer",
		"Fence",
		"ResourceMemory",
		"Buffer",
		"Image",
		"Event",
		"QueryPool",
		"BufferView",
		"ImageView",
		"ShaderModule",
		"PipelineLayout(Cache)",
		"PipelineLayout",
		"RenderPass",
		"Pipeline",
		"DescriptorSetLayout",
		"Sampler",
		"DescriptorSetLayout(Pool)",
		"DescriptorSet",
		"FrameBuffer",
		"CommandPool",
		"SwapChain(Sureface)",
		"SwapChain",
		"Instance(DebugReport)",
		"SwapChain(DisplayKHR)",
		"DISPLAY_MODE_KHR",
		"OBJECT_TABLE_NVX",
		"INDIRECT_COMMANDS_LAYOUT_NVX",
	};

	static auto constexpr objectNameCount = _countof(objectNameTable);

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

	// 作成、解放時のデバッグレポート ( CREATE Destroy うんちゃら ) のオブジェクトタイプ (objectType) が一個ずれているような気がする、、、
	
	char* pObjectType;
	if (objectNameCount > objectType)
	{
		pObjectType = objectNameTable[objectType];
	}
	else
	{
		pObjectType = objectNameTable[0];
	}

	ScopedLock lock(pInstance->m_DebugSync); // GetDebugObjectName 用

	const char* pObjectName = pInstance->GetDebugObjectName(object);
	const char* pConvertedMessage = pInstance->ConvertDebugMessage(pMessage);

#ifdef V3D64
	V3D_LOG_PRINT_A(logType, "%s : %s : location[%I64u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectType, pObjectName, location, messageCode, pLayerPrefix, pConvertedMessage);
#else //V3D64
	V3D_LOG_PRINT_A(logType, "%s : %s : location[%u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectType, pObjectName, location, messageCode, pLayerPrefix, pConvertedMessage);
#endif //V3D64

	return VK_FALSE;
}

#endif //V3D_DEBUG