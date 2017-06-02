#include "V3DInstance.h"
#include "V3DAdapter.h"
#include "V3DDevice.h"
#include "V3DSwapChain.h"
#include <algorithm>

#ifdef _DEBUG
#include "ScopedLock.h"
#endif //_DEBUG

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
	// ���O�̏�����
	// ----------------------------------------------------------------------------------------------------

	V3D_LOG_INIT(instanceDesc.log.flags, instanceDesc.log.pFunction, instanceDesc.log.pUserData);

#ifdef _DEBUG
	// �萔
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

	// �֐� : �f�X�N���v�^�Z�b�g
	m_DebugFunctionNameMap["vkUpdateDescriptorSets"] = "IV3DDescriptorSet::Update";

	// �֐� : �R�}���h�o�b�t�@�[
	m_DebugFunctionNameMap["vkBeginCommandBuffer"] = "IV3DCommandBuffer::Begin";
	m_DebugFunctionNameMap["vkEndCommandBuffer"] = "IV3DCommandBuffer::End";
	m_DebugFunctionNameMap["vkCmdPipelineBarrier"] = "IV3DCommandBuffer::Barrier???";
	m_DebugFunctionNameMap["vkCmdCopyBuffer"] = "IV3DCommandBuffer::CopyBuffer";
	m_DebugFunctionNameMap["vkCmdCopyImage"] = "IV3DCommandBuffer::CopyImage";
	m_DebugFunctionNameMap["vkCmdCopyBufferToImage"] = "IV3DCommandBuffer::CopyBufferToImage";
	m_DebugFunctionNameMap["vkCmdCopyImageToBuffer"] = "IV3DCommandBuffer::CopyImageToBuffer";
	m_DebugFunctionNameMap["vkCmdBlitImage"] = "IV3DCommandBuffer::BlitImage";
	m_DebugFunctionNameMap["vkCmdResolveImage"] = "IV3DCommandBuffer::ResolveImage???";
	m_DebugFunctionNameMap["vkCmdBeginRenderPass"] = "IV3DCommandBuffer::BeginRenderPass";
	m_DebugFunctionNameMap["vkCmdEndRenderPass"] = "IV3DCommandBuffer::EndRenderPass";
	m_DebugFunctionNameMap["vkCmdNextSubpass"] = "IV3DCommandBuffer::EndRenderPass";
	m_DebugFunctionNameMap["vkCmdClearColorImage"] = "IV3DCommandBuffer::ClearImage???(Color)";
	m_DebugFunctionNameMap["vkCmdClearDepthStencilImage"] = "IV3DCommandBuffer::ClearImage???(DepthStencil)";
	m_DebugFunctionNameMap["vkCmdClearAttachments"] = "IV3DCommandBuffer::ClearAttachments";
	m_DebugFunctionNameMap["vkCmdBindPipeline"] = "IV3DCommandBuffer::BindPipeline";
	m_DebugFunctionNameMap["vkCmdBindDescriptorSets"] = "IV3DCommandBuffer::BindDescriptorSets";
	m_DebugFunctionNameMap["vkCmdBindVertexBuffers"] = "IV3DCommandBuffer::BindVertexBuffers";
	m_DebugFunctionNameMap["vkCmdBindIndexBuffer"] = "IV3DCommandBuffer::BindIndexBuffer";
	m_DebugFunctionNameMap["vkCmdPushConstants"] = "IV3DCommandBuffer::PushConstant";
	m_DebugFunctionNameMap["vkCmdSetViewport"] = "IV3DCommandBuffer::SetViewport";
	m_DebugFunctionNameMap["vkCmdSetScissor"] = "IV3DCommandBuffer::SetScissor";
	m_DebugFunctionNameMap["vkCmdSetBlendConstants"] = "IV3DCommandBuffer::SetBlendConstants";
	m_DebugFunctionNameMap["vkCmdSetStencilReference"] = "IV3DCommandBuffer::SetStencilReference";
	m_DebugFunctionNameMap["vkCmdResetQueryPool"] = "IV3DCommandBuffer::ResetQueryPool";
	m_DebugFunctionNameMap["vkCmdBeginQuery"] = "IV3DCommandBuffer::BeginQuery";
	m_DebugFunctionNameMap["vkCmdEndQuery"] = "IV3DCommandBuffer::EndQuery";
	m_DebugFunctionNameMap["vkCmdWriteTimestamp"] = "IV3DCommandBuffer::WriteTimestamp";
	m_DebugFunctionNameMap["vkCmdDraw"] = "IV3DCommandBuffer::Draw";
	m_DebugFunctionNameMap["vkCmdDrawIndexed"] = "IV3DCommandBuffer::DrawIndexed";
	m_DebugFunctionNameMap["vkCmdDispatch"] = "IV3DCommandBuffer::Dispatch";
	m_DebugFunctionNameMap["vkCmdExecuteCommands"] = "IV3DCommandBuffer::ExecuteCommandBuffers";
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// �A���P�[�^�[
	// ----------------------------------------------------------------------------------------------------

	m_AllocationCallbacks.pfnAllocation = V3DInstance::Alloc;
	m_AllocationCallbacks.pfnReallocation = V3DInstance::Realloc;
	m_AllocationCallbacks.pfnFree = V3DInstance::Free;

	// ----------------------------------------------------------------------------------------------------
	// �L���ɂ��郌�C���[���
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
	// �L���ɂ���G�N�X�e���V�������
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
	// �C���X�^���X���쐬
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

	V3D_ADD_DEBUG_OBJECT(this, m_Source.instance, L"instance");

	// ----------------------------------------------------------------------------------------------------
	// �f�o�b�O���|�[�g�p�̃R�[���o�b�N��o�^
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

#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// �����f�o�C�X�̗�
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

#ifdef _DEBUG

void V3DInstance::AddDebugObject(void* pVulkanObject, const wchar_t* pName)
{
	AddDebugObject(reinterpret_cast<uint64_t>(pVulkanObject), pName);
}

void V3DInstance::AddDebugObject(uint64_t vulkanObject, const wchar_t* pName)
{
	ScopedLock lock(m_DebugSync);

	std::string debugName;
	ToMultibyteString(V3D_SAFE_NAME(pName), debugName);

	m_DebugObjectNameMap[vulkanObject] = debugName;
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

#endif //_DEBUG

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

		V3D_REMOVE_DEBUG_OBJECT(this, m_Source.instance);
	}

	V3D_ASSERT(m_DebugObjectNameMap.size() == 0);

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
				// �ő剻�{�^���Ńt���X�N���[��������ꍇ�́A�ő剻�A�j���[�V�������X�L�b�v����
				return 0;
			}
		}
		else if (wparam == SC_MINIMIZE)
		{
			if (pSwapChain->GetFullscreenState() == true)
			{
				// �t���X�N���[�����ɍŏ���
				// WM_ACTIVATEAPP �ōŏ������邽�߁A�ŏ����A�j���[�V�������X�L�b�v����
				return 0;
			}
		}
		else if (wparam == SC_RESTORE)
		{
			if (pSwapChain->SetIconic(false) == V3D_OK)
			{
				// �t���X�N���[�����ɍŏ�������Ă����Ԃ��畜�A����ꍇ�́A���A�A�j���[�V�������X�L�b�v����
				return 0;
			}
		}
		break;

	case WM_ACTIVATEAPP:
		if (wparam == FALSE)
		{
			// �t���X�N���[�����ɔ�A�N�e�B�u�ɂȂ����ꍇ�́A�A�C�R��������
			pSwapChain->SetIconic(true);
		}
		break;

	case WM_SIZE:
		// �C���[�W�̃T�C�Y��ύX
		pSwapChain->ResizeImage();
		break;

	case WM_SYSKEYDOWN:
		if ((pSwapChain->GetDesc().fullscreenAssistEnable == true) && (wparam == VK_RETURN) && ((lparam & (1 << 30)) == 0))
		{
			// ALT + ENTER �ɂ��t���X�N���[����Ԃ̕ύX
			pSwapChain->SetFullscreenState(pSwapChain->GetDesc().windowed);
		}
		break;

	case WM_NCLBUTTONDBLCLK:
		if (wparam == HTCAPTION)
		{
			if ((pSwapChain->GetDesc().fullscreenAssistEnable == true) && (pSwapChain->SetFullscreenState(true) == V3D_OK))
			{
				// �L���v�V�����̃_�u���N���b�N�ɂ��ꍇ�̃t���X�N���[�����̏ꍇ�́A�ő剻�A�j���[�V�������X�L�b�v
				return 0;
			}
		}
		break;
	}

	return ::CallWindowProc(pWindowProc, windowHandle, message, wparam, lparam);
}

#ifdef _DEBUG

const char* V3DInstance::GetDebugObjectName(uint64_t objectAddr)
{
	static constexpr char* unknownDebugName = "unknown";

	V3DInstance::DebugObjectNameMap::iterator it = m_DebugObjectNameMap.find(objectAddr);
	if (it != m_DebugObjectNameMap.end())
	{
		return it->second.c_str();
	}

	return unknownDebugName;
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
		// �I�u�W�F�N�g
		char* endPtr;
		uint64_t objectAddr = strtoll(pString, &endPtr, 0);
		if (objectAddr != 0)
		{
			return GetDebugObjectName(objectAddr);
		}
	}
	else if ((count > 2) && (pString[0] == 'v') && (pString[1] == 'k'))
	{
		// �֐���
		auto it_name = m_DebugFunctionNameMap.find(pString);
		if (it_name != m_DebugFunctionNameMap.end())
		{
			return it_name->second.c_str();
		}
	}
	else if ((count > 3) && (pString[0] == 'V') && (pString[1] == 'K') && (pString[2] == '_'))
	{
		// �萔
		auto it_name = m_DebugConstantNameMap.find(pString);
		if (it_name != m_DebugConstantNameMap.end())
		{
			return it_name->second.c_str();
		}
	}

	return pString;
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
		"IV3DInstance",
		"IV3DAdapter",
		"IV3DDevice",
		"IV3DQueue",
		"IV3DSwapChain(Semaphore)",
		"IV3DCommandBuffer",
		"IV3DFence",
		"IV3DResourceMemory",
		"IV3DBuffer",
		"IV3DImage",
		"EVENT",
		"IV3DQueryPool",
		"IV3DBufferView",
		"IV3DImageView",
		"IV3DShaderModule",
		"PIPELINE_CACHE",
		"IV3DPipelineLayout",
		"IV3DRenderPass",
		"IV3DPipeline",
		"IV3DDescriptorSetLayout",
		"IV3DSampler",
		"IV3DDescriptorSetLayout(Pool)",
		"IV3DDescriptorSet",
		"IV3DFrameBuffer",
		"IV3DCommandPool",
		"IV3DSwapChain(Sureface)",
		"IV3DSwapChain",
		"IV3DInstance(DebugReport)",
		"DISPLAY_KHR",
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

	// ������̃f�o�b�O���|�[�g ( Destroy ���񂿂�� ) �̃I�u�W�F�N�g�^�C�v (objectType) �������Ă���悤�ȋC������A�A�A�o�O?
	
	char* pObjectType;
	if (objectNameCount > objectType)
	{
		pObjectType = objectNameTable[objectType];
	}
	else
	{
		pObjectType = objectNameTable[0];
	}

	ScopedLock lock(pInstance->m_DebugSync); // GetDebugObjectName �p

	const char* pObjectName = pInstance->GetDebugObjectName(object);
	const char* pConvertedMessage = pInstance->ConvertDebugMessage(pMessage);

#ifdef _WIN64
	V3D_LOG_A(logType, "%s : %s : location[%I64u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectType, pObjectName, location, messageCode, pLayerPrefix, pConvertedMessage);
#else //_WIN64
	V3D_LOG_A(logType, "%s : %s : location[%u] messageCode[%d] layerPrefix[%s] : %s\n", pObjectType, pObjectName, location, messageCode, pLayerPrefix, pConvertedMessage);
#endif //_WIN64

	return VK_FALSE;
}

#endif //_DEBUG