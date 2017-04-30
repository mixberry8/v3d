#include "V3DSwapChain.h"
#include "V3DInstance.h"
#include "V3DAdapter.h"
#include "V3DDevice.h"
#include "V3DQueue.h"
#include "V3DBackBuffer.h"

/***********************/
/* public V3DSwapChain */
/***********************/

V3DSwapChain* V3DSwapChain::Create()
{
	return V3D_NEW_T(V3DSwapChain);
}

V3D_RESULT V3DSwapChain::Initialize(IV3DDevice* pDevice, const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(swapChainDesc.windowHandle != nullptr);
	V3D_ASSERT(swapChainDesc.imageWidth > 0);
	V3D_ASSERT(swapChainDesc.imageHeight > 0);
	V3D_ASSERT(swapChainDesc.imageCount >= 2);
	V3D_ASSERT(swapChainDesc.queueWaitDstStageMask != 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	m_Callbacks = swapChainCallbacks;
	m_Source.waitDstStageMask = ToVkPipelineStageFlags(swapChainDesc.queueWaitDstStageMask);
	m_InitialDesc = m_Desc = swapChainDesc;

	// ----------------------------------------------------------------------------------------------------
	// ウィンドウ情報を初期化
	// ----------------------------------------------------------------------------------------------------

	m_WindowedInfo.menuHandle = ::GetMenu(swapChainDesc.windowHandle);
	m_WindowedInfo.style = static_cast<uint32_t>(::GetWindowLongPtr(swapChainDesc.windowHandle, GWL_STYLE));
	::GetWindowRect(m_Desc.windowHandle, &m_WindowedInfo.rect);
	m_WindowedInfo.imageWidth = m_Desc.imageWidth;
	m_WindowedInfo.imageHeight = m_Desc.imageHeight;

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインを作成
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = CreateSurfaceAndSwapChain();
	if (result != V3D_OK)
	{
		return result;
	}

	m_pDevice->GetInternalInstancePtr()->AddWindow(this);

	// ----------------------------------------------------------------------------------------------------
	// 必要な場合にフルスクリーン化
	// ----------------------------------------------------------------------------------------------------

	if (m_Desc.windowed == false)
	{
		if (m_pDevice->GetInternalInstancePtr()->ExistsFulscreenWindow(this) == false)
		{
			result = ChangeFullscreen(true);
			if (result != V3D_OK)
			{
				m_Desc.windowed = true;
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

const V3DSwapChain::Source& V3DSwapChain::GetSource() const
{
	return m_Source;
}

V3D_RESULT V3DSwapChain::SetIconic(bool bIconic)
{
	if (GetFullscreenState() == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (bIconic == true)
	{
		// フルスクリーン状態でアイコン化する
		if (IsIconic(m_Desc.windowHandle) == FALSE)
		{
			if (ChangeDisplaySettings(nullptr, 0) == DISP_CHANGE_SUCCESSFUL)
			{
				ShowWindow(m_Desc.windowHandle, SW_MINIMIZE);
			}
		}
	}
	else
	{
		// アイコン化されている状態からフルスクリーンにする
		if (IsIconic(m_Desc.windowHandle) == TRUE)
		{
			V3D_RESULT result = ChangeFullscreen(false);
			if (result != V3D_OK)
			{
				return result;
			}

			ShowWindow(m_Desc.windowHandle, SW_RESTORE);
		}
	}

	return V3D_OK;
}

V3D_RESULT V3DSwapChain::ResizeImage()
{
	RECT rect;
	if (::GetClientRect(m_Desc.windowHandle, &rect) == FALSE)
	{
		return V3D_ERROR_FAIL;
	}

	uint32_t imageWidth = V3D_MAX(1, static_cast<uint32_t>(rect.right - rect.left));
	uint32_t imageHeight = V3D_MAX(1, static_cast<uint32_t>(rect.bottom - rect.top));
	if ((m_Desc.imageWidth == imageWidth) && (m_Desc.imageHeight == imageHeight))
	{
		return V3D_OK;
	}

	m_Desc.imageWidth = imageWidth;
	m_Desc.imageHeight = imageHeight;

	if (GetFullscreenState() == false)
	{
		m_WindowedInfo.imageWidth = imageWidth;
		m_WindowedInfo.imageHeight = imageHeight;
	}

	return RecreateSwapChain();
}

/***********************************/
/* public override - IV3DSwapChain */
/***********************************/

const V3DSwapChainDesc& V3DSwapChain::GetDesc() const
{
	return m_Desc;
}

V3D_RESULT V3DSwapChain::AcquireNextImage()
{
	V3D_RESULT result = V3D_OK;

	VkResult vkResult = vkAcquireNextImageKHR(m_pDevice->GetSource().device, m_Source.swapChain, UINT64_MAX, m_Source.presentCompleteSemaphore, VK_NULL_HANDLE, &m_Source.currentImageIndex);
	if (vkResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// スワップチェーンのサーフェイスが互換性がなくなり、レンダリングに使用できなくなった
		// サーフェイス、スワップチェーン、セマフォのすべてを作り直す
		result = RecreateSurfaceAndSwapChain();
	}
	else if (vkResult == VK_SUBOPTIMAL_KHR)
	{
		// スワップチェーンのサーフェイスはレンダリングに使用可能だが、ウィンドウサイズなどのプロパティが完全に一致しない
		// スワップチェーンのみ作り直す
		// Present で処理する
	}
	else if(vkResult != VK_SUCCESS)
	{
		result = ToV3DResult(vkResult);
	}

	return result;
}

uint32_t V3DSwapChain::GetCurrentImageIndex() const
{
	return m_Source.currentImageIndex;
}

void V3DSwapChain::GetImage(uint32_t imageIndex, IV3DImage** ppImage)
{
	(*ppImage) = V3D_TO_ADD_REF(m_Images[imageIndex]);
}

bool V3DSwapChain::GetFullscreenState() const
{
	return (m_Desc.windowed == false)? true : false;
}

V3D_RESULT V3DSwapChain::SetFullscreenState(bool bFullscreen)
{
	if (GetFullscreenState() == bFullscreen)
	{
		return V3D_OK;
	}

	if (bFullscreen == true)
	{
		if (m_pDevice->GetInternalInstancePtr()->ExistsFulscreenWindow(this) == true)
		{
			// 他のスワップチェーンがフルスクリーン化されている場合は失敗する
			return V3D_ERROR_FAIL;
		}

		V3D_RESULT result = ChangeFullscreen(true);
		if (result != V3D_OK)
		{
			return result;
		}

		m_Desc.windowed = false;
	}
	else
	{
		if (ChangeDisplaySettings(nullptr, 0) != DISP_CHANGE_SUCCESSFUL)
		{
			return V3D_ERROR_FAIL;
		}

		::SetMenu(m_Desc.windowHandle, m_WindowedInfo.menuHandle);
		::SetWindowLongPtr(m_Desc.windowHandle, GWL_STYLE, m_WindowedInfo.style);
		::SetWindowPos(
			m_Desc.windowHandle,
			HWND_NOTOPMOST,
			m_WindowedInfo.rect.left,
			m_WindowedInfo.rect.top,
			m_WindowedInfo.rect.right - m_WindowedInfo.rect.left,
			m_WindowedInfo.rect.bottom - m_WindowedInfo.rect.top,
			0);

		m_Desc.windowed = true;
	}

	return V3D_OK;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DSwapChain::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DSwapChain::GetRefCount() const
{
	return m_RefCounter;
}

void V3DSwapChain::AddRef()
{
	++m_RefCounter;
}

void V3DSwapChain::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DSwapChain);
	}
}

/************************/
/* private V3DSwapChain */
/************************/

V3DSwapChain::V3DSwapChain() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_InitialDesc({}),
	m_Desc({}),
	m_Callbacks({}),
	m_Source({}),
	m_WindowedInfo({})
{
	m_Desc.windowed = true;

	m_Source.surface = VK_NULL_HANDLE;
	m_Source.swapChain = VK_NULL_HANDLE;
	m_Source.presentCompleteSemaphore = VK_NULL_HANDLE;
	m_Source.renderingCompleteSemaphore = VK_NULL_HANDLE;
}

V3DSwapChain::~V3DSwapChain()
{
	m_pDevice->GetInternalInstancePtr()->RemoveWindow(this);

	for (size_t i = 0; i < m_Images.size(); i++)
	{
		V3D_RELEASE(m_Images[i]);
	}

	if (m_pDevice != nullptr)
	{
		if (m_Source.swapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(m_pDevice->GetSource().device, m_Source.swapChain, nullptr);
		}

		if (m_Source.surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(m_pDevice->GetInternalInstancePtr()->GetSource().instance, m_Source.surface, nullptr);
		}

		if (m_Source.renderingCompleteSemaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_pDevice->GetSource().device, m_Source.renderingCompleteSemaphore, nullptr);
		}

		if (m_Source.presentCompleteSemaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(m_pDevice->GetSource().device, m_Source.presentCompleteSemaphore, nullptr);
		}
	}

	V3D_RELEASE(m_pDevice);
}

V3D_RESULT V3DSwapChain::CreateSurfaceAndSwapChain()
{
	// ----------------------------------------------------------------------------------------------------
	// 物理デバイスを取得
	// ----------------------------------------------------------------------------------------------------

	IV3DAdapter* pAdapter;
	m_pDevice->GetAdapter(&pAdapter);

	V3DAdapter* pInternalAdapter = static_cast<V3DAdapter*>(pAdapter);
	VkPhysicalDevice physicalDevice = pInternalAdapter->GetSource().physicalDevice;

	V3D_RELEASE(pAdapter);

	// ----------------------------------------------------------------------------------------------------
	// サーフェイスを作成
	// ----------------------------------------------------------------------------------------------------

	HINSTANCE instanceHandle = reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(m_Desc.windowHandle, GWLP_HINSTANCE));

	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.pNext = nullptr;
	surfaceInfo.flags = 0;
	surfaceInfo.hinstance = instanceHandle;
	surfaceInfo.hwnd = m_Desc.windowHandle;

	VkResult vkResult = vkCreateWin32SurfaceKHR(m_pDevice->GetInternalInstancePtr()->GetSource().instance, &surfaceInfo, nullptr, &m_Source.surface);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	VkBool32 surfaceSupported;
	vkResult = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, m_InitialDesc.queueFamily, m_Source.surface, &surfaceSupported);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	if (surfaceSupported == VK_FALSE)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージのフォーマット、カラースペースを決める
	// ----------------------------------------------------------------------------------------------------

	uint32_t surfaceFormatCount;
	vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Source.surface, &surfaceFormatCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	STLVector<VkSurfaceFormatKHR> surfaceFormats;
	surfaceFormats.resize(surfaceFormatCount);

	vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Source.surface, &surfaceFormatCount, surfaceFormats.data());
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	VkFormat imageFormat;
	VkColorSpaceKHR imageColorSpace;

	if ((surfaceFormatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		VkFormat compareImageFormat = ToVkFormat(m_Desc.imageFormat);
		bool findSurfaceFormat = false;

		for (uint32_t i = 0; (i < surfaceFormatCount) && (findSurfaceFormat == false); i++)
		{
			if (surfaceFormats[i].format == compareImageFormat)
			{
				imageFormat = surfaceFormats[0].format;
				imageColorSpace = surfaceFormats[0].colorSpace;
				findSurfaceFormat = true;
			}
		}

		if (findSurfaceFormat == false)
		{
			imageFormat = surfaceFormats[0].format;
			imageColorSpace = surfaceFormats[0].colorSpace;
		}
	}

	m_Desc.imageFormat = ToV3DFormat(imageFormat);

	// ----------------------------------------------------------------------------------------------------
	// サーフェイスの能力を取得
	// ----------------------------------------------------------------------------------------------------

	VkSurfaceCapabilitiesKHR surfaceCaps;
	vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Source.surface, &surfaceCaps);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージの数を決める
	// ----------------------------------------------------------------------------------------------------

	if (surfaceCaps.minImageCount > m_Desc.imageCount)
	{
		m_Desc.imageCount = surfaceCaps.minImageCount;
	}
	else if (surfaceCaps.maxImageCount < m_Desc.imageCount)
	{
		m_Desc.imageCount = surfaceCaps.maxImageCount;
	}

	// ----------------------------------------------------------------------------------------------------
	// トランスフォームを決める
	// ----------------------------------------------------------------------------------------------------

	VkSurfaceTransformFlagBitsKHR preTransform;
	if (surfaceCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfaceCaps.currentTransform;
	}

	// ----------------------------------------------------------------------------------------------------
	// プレゼントモードを決める
	// ----------------------------------------------------------------------------------------------------

	uint32_t presentModeCount;
	vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Source.surface, &presentModeCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	STLVector<VkPresentModeKHR> presentModes;
	presentModes.resize(presentModeCount);
	vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Source.surface, &presentModeCount, presentModes.data());
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	static constexpr VkPresentModeKHR vkPresentModeTable[] =
	{
		VK_PRESENT_MODE_MAILBOX_KHR,
		VK_PRESENT_MODE_FIFO_KHR,
		VK_PRESENT_MODE_FIFO_RELAXED_KHR,
		VK_PRESENT_MODE_IMMEDIATE_KHR,
	};

	static constexpr int32_t vkPresentModeCount = static_cast<int32_t>(_countof(vkPresentModeTable));
	static constexpr int32_t vkPresentModeLast = vkPresentModeCount - 1;

	STLVector<VkPresentModeKHR>::iterator it_begin = presentModes.begin();
	STLVector<VkPresentModeKHR>::iterator it_end = presentModes.end();
	int32_t presentModeIndex = -1;

	if (m_Desc.vsyncEnable == true)
	{
		for (int32_t i = 0; (i < vkPresentModeCount) && (presentModeIndex == -1); i++)
		{
			if (std::find(it_begin, it_end, vkPresentModeTable[i]) != presentModes.end())
			{
				presentModeIndex = i;
			}
		}
	}
	else 
	{
		for (int32_t i = vkPresentModeLast; (i >= 0) && (presentModeIndex == -1); i--)
		{
			if (std::find(it_begin, it_end, vkPresentModeTable[i]) != presentModes.end())
			{
				presentModeIndex = i;
			}
		}
	}

	if (presentModeIndex == -1)
	{
		return V3D_ERROR_FAIL;
	}

	m_Desc.vsyncEnable == (presentModeIndex == vkPresentModeLast) ? false : true;

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインを作成
	// ----------------------------------------------------------------------------------------------------

	m_Source.swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	m_Source.swapChainCreateInfo.pNext = nullptr;
	m_Source.swapChainCreateInfo.flags = 0;
	m_Source.swapChainCreateInfo.surface = m_Source.surface;
	m_Source.swapChainCreateInfo.minImageCount = m_Desc.imageCount;
	m_Source.swapChainCreateInfo.imageFormat = imageFormat;
	m_Source.swapChainCreateInfo.imageColorSpace = imageColorSpace;
	m_Source.swapChainCreateInfo.imageExtent.width = m_Desc.imageWidth;
	m_Source.swapChainCreateInfo.imageExtent.height = m_Desc.imageHeight;
	m_Source.swapChainCreateInfo.imageArrayLayers = 1;
	m_Source.swapChainCreateInfo.imageUsage = ToVkImageUsageFlags(m_Desc.imageUsageFlags);
	m_Source.swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	m_Source.swapChainCreateInfo.queueFamilyIndexCount = 0;
	m_Source.swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	m_Source.swapChainCreateInfo.preTransform = preTransform;
	m_Source.swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	m_Source.swapChainCreateInfo.presentMode = presentModes[presentModeIndex];
	m_Source.swapChainCreateInfo.clipped = VK_TRUE;
	m_Source.swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	vkResult = vkCreateSwapchainKHR(m_pDevice->GetSource().device, &m_Source.swapChainCreateInfo, nullptr, &m_Source.swapChain);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージを作成
	// ----------------------------------------------------------------------------------------------------

	vkResult = vkGetSwapchainImagesKHR(m_pDevice->GetSource().device, m_Source.swapChain, &m_Desc.imageCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	STLVector<VkImage> images;
	images.resize(m_Desc.imageCount);
	vkResult = vkGetSwapchainImagesKHR(m_pDevice->GetSource().device, m_Source.swapChain, &m_Desc.imageCount, images.data());
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Images.reserve(m_Desc.imageCount);

	for (uint32_t i = 0; i < m_Desc.imageCount; i++)
	{
		V3DBackBuffer* pBackBuffer = V3DBackBuffer::Create();
		if (pBackBuffer == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pBackBuffer->Initialize(m_pDevice, images[i], imageFormat, m_Desc.imageWidth, m_Desc.imageHeight);
		if (result != V3D_OK)
		{
			V3D_RELEASE(pBackBuffer);
			return result;
		}

		m_Images.push_back(pBackBuffer);
	}

	// ----------------------------------------------------------------------------------------------------
	// コマンドキュート同期をとるためのオブジェクトを作成
	// ----------------------------------------------------------------------------------------------------

	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	vkResult = vkCreateSemaphore(m_pDevice->GetSource().device, &semaphoreCreateInfo, nullptr, &m_Source.presentCompleteSemaphore);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	vkResult = vkCreateSemaphore(m_pDevice->GetSource().device, &semaphoreCreateInfo, nullptr, &m_Source.renderingCompleteSemaphore);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT V3DSwapChain::RecreateSwapChain()
{
	// ----------------------------------------------------------------------------------------------------
	// 事前処理
	// ----------------------------------------------------------------------------------------------------

	m_pDevice->WaitIdle();

	if (m_Callbacks.pLostFunction != nullptr)
	{
		m_Callbacks.pLostFunction(m_Callbacks.pUserData);
	}

	if (m_Images.empty() == false)
	{
		for (size_t i = 0; i < m_Images.size(); i++)
		{
			V3D_RELEASE(m_Images[i]);
		}

		m_Images.clear();
	}

	// ----------------------------------------------------------------------------------------------------
	// 物理デバイスを取得
	// ----------------------------------------------------------------------------------------------------

	IV3DAdapter* pAdapter;
	m_pDevice->GetAdapter(&pAdapter);

	V3DAdapter* pInternalAdapter = static_cast<V3DAdapter*>(pAdapter);
	VkPhysicalDevice vkPhysicalDevice = pInternalAdapter->GetSource().physicalDevice;

	V3D_RELEASE(pAdapter);

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインを作成
	// ----------------------------------------------------------------------------------------------------

	VkSurfaceCapabilitiesKHR vkSurfaceCaps{};
	VkResult vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, m_Source.surface, &vkSurfaceCaps);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Source.swapChainCreateInfo.imageExtent.width = V3D_MAX(vkSurfaceCaps.minImageExtent.width, vkSurfaceCaps.currentExtent.width);
	m_Source.swapChainCreateInfo.imageExtent.height = V3D_MAX(vkSurfaceCaps.minImageExtent.height, vkSurfaceCaps.currentExtent.height);
	m_Source.swapChainCreateInfo.oldSwapchain = m_Source.swapChain;

	vkResult = vkCreateSwapchainKHR(m_pDevice->GetSource().device, &m_Source.swapChainCreateInfo, nullptr, &m_Source.swapChain);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	if (m_Source.swapChainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_pDevice->GetSource().device, m_Source.swapChainCreateInfo.oldSwapchain, nullptr);
		m_Source.swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	}

	// ----------------------------------------------------------------------------------------------------
	// 記述の更新
	// ----------------------------------------------------------------------------------------------------

	m_Desc.imageFormat = ToV3DFormat(m_Source.swapChainCreateInfo.imageFormat);
	m_Desc.imageWidth = m_Source.swapChainCreateInfo.imageExtent.width;
	m_Desc.imageHeight = m_Source.swapChainCreateInfo.imageExtent.height;

	vkResult = vkGetSwapchainImagesKHR(m_pDevice->GetSource().device, m_Source.swapChain, &m_Desc.imageCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージを作成
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkImage> vkImages;
	vkImages.resize(m_Desc.imageCount);
	vkResult = vkGetSwapchainImagesKHR(m_pDevice->GetSource().device, m_Source.swapChain, &m_Desc.imageCount, vkImages.data());
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Images.reserve(m_Desc.imageCount);

	for (uint32_t i = 0; i < m_Desc.imageCount; i++)
	{
		V3DBackBuffer* pBackBuffer = V3DBackBuffer::Create();
		if (pBackBuffer == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pBackBuffer->Initialize(m_pDevice, vkImages[i], m_Source.swapChainCreateInfo.imageFormat, m_Desc.imageWidth, m_Desc.imageHeight);
		if (result != V3D_OK)
		{
			V3D_RELEASE(pBackBuffer);
			return result;
		}

		m_Images.push_back(pBackBuffer);
	}

	// ----------------------------------------------------------------------------------------------------
	// 後処理
	// ----------------------------------------------------------------------------------------------------

	if (m_Callbacks.pRestoreFunction != nullptr)
	{
		m_Callbacks.pRestoreFunction(m_Callbacks.pUserData);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT V3DSwapChain::RecreateSurfaceAndSwapChain()
{
	// ----------------------------------------------------------------------------------------------------
	// 事前処理
	// ----------------------------------------------------------------------------------------------------

	m_pDevice->WaitIdle();

	if (m_Callbacks.pLostFunction != nullptr)
	{
		m_Callbacks.pLostFunction(m_Callbacks.pUserData);
	}

	if (m_Images.empty() == false)
	{
		for (size_t i = 0; i < m_Images.size(); i++)
		{
			V3D_RELEASE(m_Images[i]);
		}

		m_Images.clear();
	}

	if (m_Source.renderingCompleteSemaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(m_pDevice->GetSource().device, m_Source.renderingCompleteSemaphore, nullptr);
		m_Source.renderingCompleteSemaphore = VK_NULL_HANDLE;
	}

	if (m_Source.presentCompleteSemaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(m_pDevice->GetSource().device, m_Source.presentCompleteSemaphore, nullptr);
		m_Source.presentCompleteSemaphore = VK_NULL_HANDLE;
	}

	if (m_Source.swapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_pDevice->GetSource().device, m_Source.swapChain, nullptr);
		m_Source.swapChain = VK_NULL_HANDLE;
	}

	if (m_Source.surface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_pDevice->GetInternalInstancePtr()->GetSource().instance, m_Source.surface, nullptr);
		m_Source.surface = VK_NULL_HANDLE;
	}

	// ----------------------------------------------------------------------------------------------------
	// サーフェイス、スワップチェーン、セマフォを作り直す
	// ----------------------------------------------------------------------------------------------------

	RECT clientRect;
	::GetClientRect(m_Desc.windowHandle, &clientRect);

	bool windowed = m_Desc.windowed;
	m_Desc = m_InitialDesc;

	m_Desc.imageWidth = static_cast<uint32_t>(clientRect.right - clientRect.left);
	m_Desc.imageHeight = static_cast<uint32_t>(clientRect.bottom - clientRect.top);
	m_Desc.windowed = windowed;

	V3D_RESULT result = CreateSurfaceAndSwapChain();
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// 事後処理
	// ----------------------------------------------------------------------------------------------------

	if (m_Callbacks.pRestoreFunction != nullptr)
	{
		m_Callbacks.pRestoreFunction(m_Callbacks.pUserData);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT V3DSwapChain::ChangeFullscreen(bool saveWindowInfo)
{
	DEVMODE currentDevMode;

	if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &currentDevMode) == FALSE)
	{
		return V3D_ERROR_FAIL;
	}

	uint32_t modeIndex = 0;
	bool findBestDevMode = false;
	DEVMODE devMode;
	DEVMODE bestDevMode;

	while ((EnumDisplaySettings(nullptr, modeIndex++, &devMode) == TRUE) && (findBestDevMode == false))
	{
		if ((devMode.dmDisplayFrequency != currentDevMode.dmDisplayFrequency) ||
			(devMode.dmBitsPerPel != currentDevMode.dmBitsPerPel) ||
			(devMode.dmDisplayFlags != currentDevMode.dmDisplayFlags))
		{
			continue;
		}

		if ((devMode.dmPelsWidth == m_WindowedInfo.imageWidth) && (devMode.dmPelsHeight == m_WindowedInfo.imageHeight))
		{
			bestDevMode = devMode;
			findBestDevMode = true;
		}
		else if ((devMode.dmPelsWidth >= m_WindowedInfo.imageWidth) && (devMode.dmPelsHeight >= m_WindowedInfo.imageHeight))
		{
			bestDevMode = devMode;
			findBestDevMode = true;
		}
	}

	if (findBestDevMode == false)
	{
		return V3D_ERROR_FAIL;
	}

	if (ChangeDisplaySettings(&bestDevMode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
	{
		return V3D_ERROR_FAIL;
	}

	if (saveWindowInfo == true)
	{
		m_WindowedInfo.menuHandle = ::GetMenu(m_Desc.windowHandle);
		m_WindowedInfo.style = static_cast<uint32_t>(::GetWindowLongPtr(m_Desc.windowHandle, GWL_STYLE));
		::GetWindowRect(m_Desc.windowHandle, &m_WindowedInfo.rect);
		m_WindowedInfo.imageWidth = m_Desc.imageWidth;
		m_WindowedInfo.imageHeight = m_Desc.imageHeight;
	}

	if (ChangeDisplaySettings(&bestDevMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return V3D_ERROR_FAIL;
	}

	::SetMenu(m_Desc.windowHandle, nullptr);
	::SetWindowLongPtr(m_Desc.windowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	::SetWindowPos(m_Desc.windowHandle, HWND_TOP, 0, 0, bestDevMode.dmPelsWidth, bestDevMode.dmPelsHeight, 0);

	return V3D_OK;
}
