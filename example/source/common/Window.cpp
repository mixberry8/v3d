#include "Window.h"
#include "Application.h"
#include <windowsx.h>

Window::Window() :
	m_WindowHandle(nullptr),
	m_pSwapChain(nullptr),
	m_pWorkQueue(nullptr),
	m_pWorkCommandPool(nullptr),
	m_pWorkCommandBuffer(nullptr),
	m_pWorkFence(nullptr),
	m_pGraphicsQueue(nullptr),
	m_pGraphicsCommandPool(nullptr),
	m_ScreenshotRequest(false),
	m_ScreenshotFilePath(L"screenshot")
{
}

Window::~Window()
{
	Dispose();
}

bool Window::Initialize(const wchar_t* pCaption, uint32_t width, uint32_t height, IV3DQueue* pWorkQueue, IV3DQueue* pGraphicsQueue)
{
	SAFE_ADD_REF(pGraphicsQueue);
	m_pGraphicsQueue = pGraphicsQueue;

	SAFE_ADD_REF(pWorkQueue);
	m_pWorkQueue = pWorkQueue;

	// ----------------------------------------------------------------------------------------------------
	// ウィンドウを作成
	// ----------------------------------------------------------------------------------------------------

	WNDCLASSEX wcex = { 0 };
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = (CS_HREDRAW | CS_VREDRAW);
	wcex.lpfnWndProc = Window::WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = pCaption;

	if (::RegisterClassEx(&wcex) == 0)
	{
		return false;
	}

	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_THICKFRAME;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	::AdjustWindowRect(&rect, windowStyle, FALSE);

	m_WindowHandle = ::CreateWindowEx(
		0,
		pCaption,
		pCaption,
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL);

	if (m_WindowHandle == nullptr)
	{
		Dispose();
		return false;
	}

	::SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	::ShowWindow(m_WindowHandle, SW_SHOW);

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインを作成
	// ----------------------------------------------------------------------------------------------------

	V3DSwapChainDesc swapChainDesc{};
	swapChainDesc.windowHandle = m_WindowHandle;
	swapChainDesc.imageFormat = V3D_FORMAT_UNDEFINED;
	swapChainDesc.imageWidth = width;
	swapChainDesc.imageHeight = height;
	swapChainDesc.imageCount = Window::BufferCount;
	swapChainDesc.imageUsageFlags = V3D_IMAGE_USAGE_TRANSFER_SRC | V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;
	swapChainDesc.fullscreenAssistEnable = true;
	swapChainDesc.vsyncEnable = true;
	swapChainDesc.windowed = true;
	swapChainDesc.queueFamily = m_pGraphicsQueue->GetFamily();
	swapChainDesc.queueWaitDstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;

	V3DSwapChainCallbacks swapChainCallbacks{};
	swapChainCallbacks.pLostFunction = Window::LostSwapChainFunction;
	swapChainCallbacks.pRestoreFunction = Window::RestoreSwapChainFunction;
	swapChainCallbacks.pUserData = this;

	V3D_RESULT result = Application::GetDevice()->CreateSwapChain(swapChainDesc, swapChainCallbacks, &m_pSwapChain);
	if (result != V3D_OK)
	{
		Dispose();
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// ワークコマンドバッファを作成
	// ----------------------------------------------------------------------------------------------------

	{
		V3DCommandPoolDesc commandPoolDesc{};
		commandPoolDesc.queueFamily = m_pWorkQueue->GetFamily();
		commandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER;

		result = Application::GetDevice()->CreateCommandPool(commandPoolDesc, &m_pWorkCommandPool);
		if (result != V3D_OK)
		{
			Dispose();
			return false;
		}

		result = Application::GetDevice()->CreateCommandBuffer(m_pWorkCommandPool, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &m_pWorkCommandBuffer);
		if (result != V3D_OK)
		{
			Dispose();
			return false;
		}

		result = Application::GetDevice()->CreateFence(&m_pWorkFence);
		if (result != V3D_OK)
		{
			Dispose();
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// グラフィックスコマンドプールを作成
	// ----------------------------------------------------------------------------------------------------

	{
		V3DCommandPoolDesc commandPoolDesc{};
		commandPoolDesc.queueFamily = m_pGraphicsQueue->GetFamily();
		commandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER;

		result = Application::GetDevice()->CreateCommandPool(commandPoolDesc, &m_pGraphicsCommandPool);
		if (result != V3D_OK)
		{
			Dispose();
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインに関係するリソースを作成
	// ----------------------------------------------------------------------------------------------------

	result = CreateSwapChainResources();
	if(result != V3D_OK)
	{
		Dispose();
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// 初期化をコール
	// ----------------------------------------------------------------------------------------------------

	if (OnInitialize() == false)
	{
		OnFinalize();
		Dispose();
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// ウィンドウを追加
	// ----------------------------------------------------------------------------------------------------

	if (Application::AddWindow(this) == false)
	{
		Dispose();
		return false;
	}

	// ----------------------------------------------------------------------------------------------------

	return true;
}

bool Window::Restart()
{
	Application::GetDevice()->WaitIdle();
	OnFinalize();
	return OnInitialize();
}

void Window::Resize(uint32_t width, uint32_t height)
{
	DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_THICKFRAME;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	::AdjustWindowRect(&rect, windowStyle, FALSE);

	::SetWindowPos(m_WindowHandle, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}

HWND Window::GetHandle() const
{
	return m_WindowHandle;
}

IV3DSwapChain* Window::GetSwapChain() const
{
	return m_pSwapChain;
}

uint32_t Window::GetImageCount() const
{
	return m_pSwapChain->GetDesc().imageCount;
}

IV3DImage* Window::GetImage(uint32_t index) const
{
	return m_Images[index];
}

IV3DImage* Window::GetCurrentImage() const
{
	return m_Images[m_pSwapChain->GetCurrentImageIndex()];
}

bool Window::CreateImageView(uint32_t index, IV3DImageView** ppImageView)
{
	if (m_Images.size() <= index)
	{
		return false;
	}

	IV3DCommandBuffer* pCommandBuffer = BeginWork();
	if (pCommandBuffer == nullptr)
	{
		return false;
	}

	V3DImageViewDesc imageViewDesc{};
	imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
	imageViewDesc.baseLevel = 0;
	imageViewDesc.levelCount = 1;
	imageViewDesc.baseLayer = 0;
	imageViewDesc.layerCount = 1;

	IV3DImageView* pImageView;
	V3D_RESULT result = Application::GetDevice()->CreateImageView(m_Images[index], imageViewDesc, &pImageView);
	if (result != V3D_OK)
	{
		return false;
	}

	V3DBarrierImageDesc barrier;
	barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
	barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
	barrier.dependencyFlags = 0;
	barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
	barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
	pCommandBuffer->BarrierImageView(pImageView, barrier);

	pCommandBuffer->ClearImageView(pImageView, V3D_IMAGE_LAYOUT_TRANSFER_DST, V3DClearValue{});

	barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	barrier.dstAccessMask = V3D_ACCESS_MEMORY_READ;
	barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
	barrier.dstLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
	pCommandBuffer->BarrierImageView(pImageView, barrier);

	if (EndWork() == false)
	{
		return false;
	}

	*ppImageView = pImageView;

	return true;
}

IV3DQueue* Window::GetWorkQueue()
{
	return m_pWorkQueue;
}

IV3DCommandBuffer* Window::GetWorkCommandBuffer()
{
	return m_pWorkCommandBuffer;
}

IV3DFence* Window::GetWorkFence()
{
	return m_pWorkFence;
}

IV3DCommandBuffer* Window::GetCurrentGraphicsCommandBuffer()
{
	return m_Frames[m_pSwapChain->GetCurrentImageIndex()].pGraphicsCommandBuffer;
}

IV3DCommandBuffer* Window::BeginWork()
{
	if (m_pWorkCommandBuffer->Begin() != V3D_OK)
	{
		return nullptr;
	}

	return m_pWorkCommandBuffer;
}

bool Window::EndWork()
{
	V3D_RESULT result = m_pWorkCommandBuffer->End();
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pWorkFence->Reset();
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pWorkQueue->Submit(1, &m_pWorkCommandBuffer, m_pWorkFence);
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pWorkFence->Wait();
	if (result != V3D_OK)
	{
		return false;
	}

	return true;
}

void Window::SaveScreenshot(const wchar_t* pFileName)
{
	m_ScreenshotFilePath = (pFileName != nullptr)? pFileName : L"screenshot.bmp";
	m_ScreenshotRequest = true;
}

IV3DCommandBuffer* Window::BeginGraphics()
{
	IV3DCommandBuffer* pGraphicsCommandBuffer = m_Frames[m_pSwapChain->GetCurrentImageIndex()].pGraphicsCommandBuffer;

	V3D_RESULT result = pGraphicsCommandBuffer->Begin();
	if (result != V3D_OK)
	{
		return nullptr;
	}

	return pGraphicsCommandBuffer;
}

bool Window::EndGraphics()
{
	uint32_t imageIndex = m_pSwapChain->GetCurrentImageIndex();
	IV3DCommandBuffer* pGraphicsCommandBuffer = m_Frames[imageIndex].pGraphicsCommandBuffer;
	IV3DFence* pGraphicsFence = m_Frames[imageIndex].pGraphicsFence;

	V3D_RESULT result = pGraphicsCommandBuffer->End();
	if (result != V3D_OK)
	{
		return false;
	}

	result = pGraphicsFence->Reset();
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsQueue->Submit(m_pSwapChain, 1, &pGraphicsCommandBuffer, pGraphicsFence);
	if (result != V3D_OK)
	{
		return false;
	}

	m_Frames[imageIndex].initFence = true;

	result = pGraphicsFence->Wait();
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsQueue->Present(m_pSwapChain);
	if (result != V3D_OK)
	{
		return false;
	}

	return true;
}

IV3DCommandBuffer* Window::BeginGraphicsEx()
{
	IV3DCommandBuffer* pGraphicsCommandBuffer = m_Frames[m_pSwapChain->GetCurrentImageIndex()].pGraphicsCommandBuffer;

	V3D_RESULT result = pGraphicsCommandBuffer->Begin();
	if (result != V3D_OK)
	{
		return nullptr;
	}

	return pGraphicsCommandBuffer;
}

bool Window::EndGraphicsEx()
{
	uint32_t imageIndex = m_pSwapChain->GetCurrentImageIndex();
	IV3DCommandBuffer* pGraphicsCommandBuffer = m_Frames[imageIndex].pGraphicsCommandBuffer;
	IV3DFence* pCurrentGraphicsFence = m_Frames[imageIndex].pGraphicsFence;

	V3D_RESULT result = pGraphicsCommandBuffer->End();
	if (result != V3D_OK)
	{
		return false;
	}

	result = pCurrentGraphicsFence->Reset();
	if (result != V3D_OK)
	{
		return false;
	}

	result = m_pGraphicsQueue->Submit(m_pSwapChain, 1, &pGraphicsCommandBuffer, pCurrentGraphicsFence);
	if (result != V3D_OK)
	{
		return false;
	}

	m_Frames[imageIndex].initFence = true;

	uint32_t nextImageIndex = (imageIndex + 1) % m_pSwapChain->GetDesc().imageCount;
	if (m_Frames[nextImageIndex].initFence == true)
	{
		result = m_Frames[nextImageIndex].pGraphicsFence->Wait();
		if (result != V3D_OK)
		{
			return false;
		}
	}

	result = m_pGraphicsQueue->Present(m_pSwapChain);
	if (result != V3D_OK)
	{
		return false;
	}

	return true;
}

bool Window::OnMessage(UINT message, WPARAM wparam, LPARAM lparam)
{
	POINT point;
	WPARAM keyFlags;
	int16_t delta;
	uint16_t repeatCount;
	uint8_t scanCode;
	bool extendedKey;
	bool prevKeyState;

	bool result = true;

	switch (message)
	{
	case WM_MOUSEMOVE:
		point.x = GET_X_LPARAM(lparam);
		point.y = GET_Y_LPARAM(lparam);
		result = OnMouseMove(wparam, point);
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		point.x = GET_X_LPARAM(lparam);
		point.y = GET_Y_LPARAM(lparam);
		result = OnMouseButtonDown(wparam, point);
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		point.x = GET_X_LPARAM(lparam);
		point.y = GET_Y_LPARAM(lparam);
		result = OnMouseButtonUp(wparam, point);
		break;

	case WM_MOUSEWHEEL:
		keyFlags = GET_KEYSTATE_WPARAM(wparam);
		delta = GET_WHEEL_DELTA_WPARAM(wparam);
		point.x = GET_X_LPARAM(lparam);
		point.y = GET_Y_LPARAM(lparam);
		result = OnMouseWheel(keyFlags, delta, point);
		break;

	case WM_KEYDOWN:
		repeatCount = static_cast<uint16_t>(lparam & 0x0000FFFF);
		scanCode = static_cast<uint8_t>((lparam & 0x00FF0000) >> 16);
		extendedKey = ((lparam & 0x01000000) >> 24) != 0;
		prevKeyState = ((lparam & 0x40000000) >> 30) != 0;
		result = OnKeyDown(wparam, repeatCount, scanCode, extendedKey, prevKeyState);
		break;

	case WM_KEYUP:
		repeatCount = static_cast<uint16_t>(lparam & 0x0000FFFF);
		scanCode = static_cast<uint8_t>((lparam & 0x00FF0000) >> 16);
		extendedKey = ((lparam & 0x01000000) >> 24) != 0;
		prevKeyState = ((lparam & 0x40000000) >> 30) != 0;
		result = OnKeyUp(wparam, repeatCount, scanCode, extendedKey, prevKeyState);
		break;
	}

	return result;
}

void Window::Halt()
{
	if (m_WindowHandle != nullptr)
	{
		::DestroyWindow(m_WindowHandle);
		m_WindowHandle = nullptr;
	}
}

void Window::Dispose()
{
	if (m_Images.empty() == false)
	{
		for (size_t i = 0; i < m_Images.size(); i++)
		{
			SAFE_RELEASE(m_Images[i]);
		}
		m_Images.clear();
	}

	if (m_Frames.empty() == false)
	{
		for (size_t i = 0; i < m_Frames.size(); i++)
		{
			SAFE_RELEASE(m_Frames[i].pGraphicsCommandBuffer);
			SAFE_RELEASE(m_Frames[i].pGraphicsFence);
		}
		m_Frames.clear();
	}

	SAFE_RELEASE(m_pGraphicsQueue);
	SAFE_RELEASE(m_pGraphicsCommandPool);

	SAFE_RELEASE(m_pWorkCommandBuffer);
	SAFE_RELEASE(m_pWorkQueue);
	SAFE_RELEASE(m_pWorkCommandPool);

	SAFE_RELEASE(m_pWorkFence);
	SAFE_RELEASE(m_pSwapChain);

	Halt();
}

V3D_RESULT Window::CreateSwapChainResources()
{
	// ----------------------------------------------------------------------------------------------------
	// グラフィックスコマンドバッファを作成
	// ----------------------------------------------------------------------------------------------------

	V3DSwapChainDesc swapChainDesc = m_pSwapChain->GetDesc();

	m_Frames.reserve(swapChainDesc.imageCount);

	for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
	{
		Window::Frame frame{};

		V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(m_pGraphicsCommandPool, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &frame.pGraphicsCommandBuffer);
		if (result != V3D_OK)
		{
			return result;
		}

		result = Application::GetDevice()->CreateFence(&frame.pGraphicsFence);
		if (result != V3D_OK)
		{
			SAFE_RELEASE(frame.pGraphicsCommandBuffer);
			return result;
		}

		m_Frames.push_back(frame);
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージの取得
	// ----------------------------------------------------------------------------------------------------

	m_Images.reserve(swapChainDesc.imageCount);

	for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
	{
		IV3DImage* pImage;
		m_pSwapChain->GetImage(i, &pImage);
		m_Images.push_back(pImage);
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void Window::ReleaseSwapChainResources()
{
	if (m_Images.empty() == false)
	{
		for (size_t i = 0; i < m_Images.size(); i++)
		{
			SAFE_RELEASE(m_Images[i]);
		}
		m_Images.clear();
	}

	if (m_Frames.empty() == false)
	{
		for (size_t i = 0; i < m_Frames.size(); i++)
		{
			SAFE_RELEASE(m_Frames[i].pGraphicsCommandBuffer);
			SAFE_RELEASE(m_Frames[i].pGraphicsFence);
		}
		m_Frames.clear();
	}
}

void Window::LostSwapChain()
{
	OnLostSwapChain();

	ReleaseSwapChainResources();
}

void Window::RestoreSwapChain()
{
	V3D_RESULT result = CreateSwapChainResources();
	if (result != V3D_OK)
	{
		Halt();
		return;
	}

	if (OnRestoreSwapChain() == false)
	{
		Halt();
	}
}

LRESULT CALLBACK Window::WindowProc(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
	LONG_PTR wlpUserData = ::GetWindowLongPtr(windowHandle, GWLP_USERDATA);
	bool callDeWindowProc = false;

	if (wlpUserData != 0)
	{
		Window* pWindow = reinterpret_cast<Window*>(wlpUserData);
		
		callDeWindowProc = pWindow->OnMessage(message, wparam, lparam);

		if (message == WM_DESTROY)
		{
			::SetWindowLongPtr(windowHandle, GWLP_USERDATA, 0);

			pWindow->m_WindowHandle = nullptr;
			Application::GetDevice()->WaitIdle();

			pWindow->OnFinalize();
			pWindow->Dispose();

			if (Application::RemoveWindow(pWindow) == false)
			{
				PostQuitMessage(0);
			}
		}
	}
	else
	{
		callDeWindowProc = true;
	}

	return (callDeWindowProc == true)? DefWindowProc(windowHandle, message, wparam, lparam) : 0;
}

void V3D_CALLBACK Window::LostSwapChainFunction(void* pUserData)
{
	static_cast<Window*>(pUserData)->LostSwapChain();
}

void V3D_CALLBACK Window::RestoreSwapChainFunction(void* pUserData)
{
	static_cast<Window*>(pUserData)->RestoreSwapChain();
}

void Window::SaveScreenshot()
{
	IV3DImage* pDeviceImage = m_Images[m_pSwapChain->GetCurrentImageIndex()];
	V3DImageDesc imageDesc = pDeviceImage->GetDesc();
	imageDesc.tiling = V3D_IMAGE_TILING_LINEAR;
	imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST;

	IV3DImage* pHostImage = nullptr;
	if (Application::GetDevice()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pHostImage) == V3D_OK)
	{
		if (Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pHostImage) == V3D_OK)
		{
			// ----------------------------------------------------------------------------------------------------
			// イメージをコピー
			// ----------------------------------------------------------------------------------------------------

			IV3DCommandBuffer* pCommandBuffer = BeginWork();
			if (pCommandBuffer != nullptr)
			{
				V3DBarrierImageDesc barrier{};
				barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
				barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
				barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;

				barrier.srcAccessMask = V3D_ACCESS_MEMORY_READ;
				barrier.dstAccessMask = V3D_ACCESS_TRANSFER_READ;
				barrier.srcLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
				barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
				pCommandBuffer->BarrierImage(pDeviceImage, barrier);

				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
				barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
				barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
				pCommandBuffer->BarrierImage(pHostImage, barrier);

				pCommandBuffer->CopyImage(pHostImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, pDeviceImage, V3D_IMAGE_LAYOUT_TRANSFER_SRC);

				barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
				barrier.dstStageMask = V3D_PIPELINE_STAGE_HOST;
				barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
				barrier.dstAccessMask = V3D_ACCESS_HOST_READ;
				barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
				barrier.dstLayout = V3D_IMAGE_LAYOUT_GENERAL;
				pCommandBuffer->BarrierImage(pHostImage, barrier);

				barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
				barrier.dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
				barrier.srcAccessMask = V3D_ACCESS_TRANSFER_READ;
				barrier.dstAccessMask = V3D_ACCESS_MEMORY_READ;
				barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
				barrier.dstLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
				pCommandBuffer->BarrierImage(pDeviceImage, barrier);

				EndWork();
			}

			// ----------------------------------------------------------------------------------------------------
			// イメージをファイルに保存
			// ----------------------------------------------------------------------------------------------------

			uint32_t imageRowPitch = imageDesc.width * 3;
			if (imageRowPitch % 4) { imageRowPitch = ((imageRowPitch / 4) + 1) * 4; }

			uint32_t imageSize = imageRowPitch * imageDesc.height * 3;

			uint8_t* pImage = new uint8_t[imageSize];
			if (pImage != nullptr)
			{
				memset(pImage, 0, imageSize);

				void* pMemory;
				if (pHostImage->Map(0, pHostImage->GetResourceDesc().memorySize, &pMemory) == V3D_OK)
				{
					uint8_t* pDstRow = pImage + imageRowPitch * (imageDesc.height - 1);

					const V3DImageSubresourceLayout& srLayout = pHostImage->GetSubresourceLayout(0, 0);
					uint8_t* pSrcRow = static_cast<uint8_t*>(pMemory);

					for (uint32_t y = 0; y < imageDesc.height; y++)
					{
						uint8_t* pDst = pDstRow;
						uint8_t* pSrc = pSrcRow;

						for (uint32_t x = 0; x < imageDesc.width; x++)
						{
							*pDst++ = *pSrc++;
							*pDst++ = *pSrc++;
							*pDst++ = *pSrc++;

							pSrc++;
						}

						pDstRow -= imageRowPitch;
						pSrcRow += srLayout.rowPitch;
					}

					pHostImage->Unmap();
				}

				HANDLE fileHandle = CreateFile(m_ScreenshotFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (fileHandle != INVALID_HANDLE_VALUE)
				{
					DWORD writeSize;

					BITMAPFILEHEADER bmpFileHeader{};
					bmpFileHeader.bfType = ('B' | ('M' << 8));
					bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
					bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
					WriteFile(fileHandle, &bmpFileHeader, sizeof(bmpFileHeader), &writeSize, nullptr);

					BITMAPINFOHEADER bmpInfoHeader{};
					bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
					bmpInfoHeader.biWidth = imageDesc.width;
					bmpInfoHeader.biHeight = imageDesc.height;
					bmpInfoHeader.biPlanes = 1;
					bmpInfoHeader.biBitCount = 24;
					bmpInfoHeader.biSizeImage = imageSize;
					WriteFile(fileHandle, &bmpInfoHeader, sizeof(bmpInfoHeader), &writeSize, nullptr);

					WriteFile(fileHandle, pImage, imageSize, &writeSize, nullptr);

					CloseHandle(fileHandle);
				}

				delete[] pImage;
			}
		}

		SAFE_RELEASE(pHostImage);
	}
}

void Window::Process()
{
	if (m_ScreenshotRequest == true)
	{
		SaveScreenshot();
		m_ScreenshotRequest = false;
	}

	V3D_RESULT result = m_pSwapChain->AcquireNextImage();
	if (result != V3D_OK)
	{
		Halt();
	}

	if (OnIdle() == false)
	{
		Halt();
	}
}
