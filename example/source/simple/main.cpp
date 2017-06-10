#include <Windows.h>
#include <new>
#include <array>
#include <v3d.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif //_DEBUG

// ----------------------------------------------------------------------------------------------------
// 定数
// ----------------------------------------------------------------------------------------------------

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define QUEUE_COUNT 2 // グラフィックスと作業用の二つ

// ----------------------------------------------------------------------------------------------------
// 構造体
// ----------------------------------------------------------------------------------------------------

struct Frame
{
	IV3DCommandBuffer* pGraphicsCommandBuffer;
	IV3DImageView* pImageView;
	IV3DFrameBuffer* pFrameBuffer;
};

// ----------------------------------------------------------------------------------------------------
// グローバル関数
// ----------------------------------------------------------------------------------------------------

V3D_RESULT BeginFrame(Frame** ppFrame);
void EndFrame();

V3D_RESULT CreateSwapChainChilds();
void ReleaseSwapChainChilds();
void ReleaseObjects();

void V3D_CALLBACK LogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData);
void V3D_CALLBACK LostSwapChainFunction(void* pUserData);
void V3D_CALLBACK RestoreSwapChainFunction(void* pUserData);

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);

// ----------------------------------------------------------------------------------------------------
// グローバル変数
// ----------------------------------------------------------------------------------------------------

IV3DInstance* g_pInstance = nullptr;
IV3DAdapter* g_pAdapter = nullptr;
IV3DDevice* g_pDevice = nullptr;
IV3DFence* g_pFence = nullptr;
IV3DQueue* g_pGraphicsQueue = nullptr;
IV3DQueue* g_pWorkQueue = nullptr;
HWND g_WindowHandle = nullptr;
IV3DSwapChain* g_pSwapChain = nullptr;
uint32_t g_FrameCount = 0;
Frame* g_pFrames = nullptr;
IV3DCommandPool* g_pGraphicsCommandPool = nullptr;
IV3DCommandBuffer* g_pWorkCommandBuffer = nullptr;
IV3DRenderPass* g_pRenderPass = nullptr;

// ----------------------------------------------------------------------------------------------------
// メイン
// ----------------------------------------------------------------------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// インスタンスを作成
	// ----------------------------------------------------------------------------------------------------

	V3DInstanceDesc instanceDesc{};
#ifdef _DEBUG
	instanceDesc.layer = V3D_LAYER_STANDARD;
#else //_DEBUG
	instanceDesc.layer = V3D_LAYER_OPTIMUS;
#endif //_DEBUG
	instanceDesc.log.flags = V3D_LOG_ALL;
	instanceDesc.log.pFunction = LogFunction;

	V3D_RESULT result = V3DCreateInstance(instanceDesc, &g_pInstance);
	if (result != V3D_OK)
	{
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// アダプタを取得
	// ----------------------------------------------------------------------------------------------------

	if (g_pInstance->GetAdapterCount() == 0)
	{
		return -1;
	}

	g_pInstance->GetAdapter(0, &g_pAdapter);

	// ----------------------------------------------------------------------------------------------------
	// デバイスを作成
	// ----------------------------------------------------------------------------------------------------

	result = g_pInstance->CreateDevice(g_pAdapter, &g_pDevice, L"Simple(Device)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// フェンスを取得
	// ----------------------------------------------------------------------------------------------------

	result = g_pDevice->CreateFence(&g_pFence, L"Simple(Fence)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// キューを取得
	// ----------------------------------------------------------------------------------------------------

	uint32_t queueFamily = ~0U;

	for (uint32_t i = 0; (i < g_pDevice->GetQueueFamilyCount()) && (queueFamily == ~0U); i++)
	{
		const V3DDeviceQueueFamilyDesc& queueFamilyDesc = g_pDevice->GetQueueFamilyDesc(i);
		if ((queueFamilyDesc.queueFlags & V3D_QUEUE_GRAPHICS) && (queueFamilyDesc.queueCount >= 2))
		{
			queueFamily = i;
		}
	}

	if (queueFamily == ~0U)
	{
		ReleaseObjects();
		return -1;
	}

	// グラフィックス用
	g_pDevice->GetQueue(queueFamily, 0, &g_pGraphicsQueue);

	// 作業用
	g_pDevice->GetQueue(queueFamily, 1, &g_pWorkQueue);

	// ----------------------------------------------------------------------------------------------------
	// グラフィックス用のコマンドプールを作成
	// ----------------------------------------------------------------------------------------------------

	V3DCommandPoolDesc graphicsCommandPoolDesc{};
	graphicsCommandPoolDesc.queueFamily = queueFamily;
	graphicsCommandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER | V3D_COMMAND_POOL_PROPERTY_TRANSIENT;

	result = g_pDevice->CreateCommandPool(graphicsCommandPoolDesc, &g_pGraphicsCommandPool, L"Simple(CommandPool)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// 作業用のコマンドバッファを作成
	// ----------------------------------------------------------------------------------------------------

	V3DCommandPoolDesc workCommandPoolDesc{};
	workCommandPoolDesc.queueFamily = queueFamily;
	workCommandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER | V3D_COMMAND_POOL_PROPERTY_TRANSIENT;

	result = g_pDevice->CreateCommandBuffer(workCommandPoolDesc, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &g_pWorkCommandBuffer, L"Simple(CommandBuffer)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// ウィンドウを作成
	// ----------------------------------------------------------------------------------------------------

	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = (CS_HREDRAW | CS_VREDRAW);
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"simple";

	if (::RegisterClassEx(&wcex) == 0)
	{
		ReleaseObjects();
		return -1;
	}

	uint32_t windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_VISIBLE | WS_THICKFRAME;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = SCREEN_WIDTH;
	rect.bottom = SCREEN_HEIGHT;
	::AdjustWindowRect(&rect, windowStyle, FALSE);

	g_WindowHandle = ::CreateWindowEx(
		0,
		L"simple",
		L"simple",
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		HWND_DESKTOP,
		NULL,
		hInstance,
		NULL);

	if (g_WindowHandle == nullptr)
	{
		ReleaseObjects();
		return -1;
	}

	::ShowWindow(g_WindowHandle, SW_SHOW);

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインを作成
	// ----------------------------------------------------------------------------------------------------

	V3DSwapChainDesc swapChainDesc{};
	swapChainDesc.windowHandle = g_WindowHandle;
	swapChainDesc.imageFormat = V3D_FORMAT_UNDEFINED; // 現状サーフェイスのフォーマットを列挙することができないので V3D_FORMAT_UNDEFINED にしてください。
	swapChainDesc.imageWidth = SCREEN_WIDTH;
	swapChainDesc.imageHeight = SCREEN_HEIGHT;
	swapChainDesc.imageCount = 2;
	swapChainDesc.imageUsageFlags = V3D_IMAGE_USAGE_COLOR_ATTACHMENT; // イメージの出力なので V3D_IMAGE_USAGE_COLOR_ATTACHMENT は必ず指定してください。
	swapChainDesc.queueWaitDstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
	swapChainDesc.fullscreenAssistEnable = true;
	swapChainDesc.vsyncEnable = true;
	swapChainDesc.windowed = true;

	V3DSwapChainCallbacks swapChainCallbacks{};
	swapChainCallbacks.pLostFunction = LostSwapChainFunction;
	swapChainCallbacks.pRestoreFunction = RestoreSwapChainFunction;

	result = g_pDevice->CreateSwapChain(swapChainDesc, swapChainCallbacks, &g_pSwapChain, L"Simple(SwapChain)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// スワップチェインの作成は致命的なエラーが発生しない限りエラーは出しません。
	// スワップチェインの記述は実装によって変更される可能性があるため、取得しなおします
	swapChainDesc = g_pSwapChain->GetDesc();

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインに関係するインターフェースを作成
	// ----------------------------------------------------------------------------------------------------

	result = CreateSwapChainChilds();
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// メインループ
	// ----------------------------------------------------------------------------------------------------

	bool loop = true;

	do
	{
		// メッセージポンプ
		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
			if (GetMessage(&message, NULL, 0, 0) == TRUE)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else
			{
				loop = false;
			}
		}

		// 描画
		if (loop == true)
		{
			Frame* pFrame = nullptr;

			// フレームを開始
			V3D_RESULT result = BeginFrame(&pFrame);
			if (result == V3D_OK)
			{
				// レンダーパスを開始
				pFrame->pGraphicsCommandBuffer->BeginRenderPass(g_pRenderPass, pFrame->pFrameBuffer, true);

				// ここで描画を行う

				// レンダーパスを終了
				pFrame->pGraphicsCommandBuffer->EndRenderPass();

				// フレームを終了
				EndFrame();
			}
		}

	} while (loop == true);

	// ----------------------------------------------------------------------------------------------------

	// 解放は WM_DESTROY で行います

	// ----------------------------------------------------------------------------------------------------

	return 0;
}

V3D_RESULT BeginFrame(Frame** ppFrame)
{
	// 次のイメージを獲得します。
	// ここで g_pSwapChain->GetCurrentImageIndex() で取得できるイメージのインデックスが変わります。
	V3D_RESULT result = g_pSwapChain->AcquireNextImage();
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return result;
	}

	// 現在のフレームを取得します。
	Frame* pFrame = &g_pFrames[g_pSwapChain->GetCurrentImageIndex()];

	// コマンドの書き込みを開始します。
	result = pFrame->pGraphicsCommandBuffer->Begin();
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return result;
	}

	*ppFrame = pFrame;

	return V3D_OK;
}

void EndFrame()
{
	Frame* pFrame = &g_pFrames[g_pSwapChain->GetCurrentImageIndex()];

	// コマンドの書き込みを終了します。
	V3D_RESULT result = pFrame->pGraphicsCommandBuffer->End();
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return;
	}

	// フェンスをリセットして、コマンドを送信し、完了を待機します。
	result = g_pFence->Reset();
	if (result == V3D_OK)
	{
		result = g_pGraphicsQueue->Submit(g_pSwapChain, 1, &pFrame->pGraphicsCommandBuffer, g_pFence);
		if (result == V3D_OK)
		{
			result = g_pFence->Wait();
		}
	}

	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return;
	}

	// イメージを表示します
	result = g_pGraphicsQueue->Present(g_pSwapChain);
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return;
	}
}

V3D_RESULT CreateSwapChainChilds()
{
	const V3DSwapChainDesc& swapChainDesc = g_pSwapChain->GetDesc();

	// ----------------------------------------------------------------------------------------------------
	// レンダーパスを作成
	// ----------------------------------------------------------------------------------------------------

	// アタッチメント
	std::array<V3DAttachmentDesc, 1> attachments;
	attachments[0].format = swapChainDesc.imageFormat; // 0 番めのアタッチメントのスワップチェインのイメージです。
	attachments[0].samples = V3D_SAMPLE_COUNT_1;       // スワップチェインのイメージは常に V3D_SAMPLE_COUNT_1 です。
	attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;   // レンダーパス開始時にアタッチメントの内容をクリアします。
	attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE; // レンダーパス終了時にアタッチメントの内容を保存します。
	attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;   // ステンシルはないので定義しません。
	attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED; // ↑に同じ
	attachments[0].initialLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC; // レンダーパス開始時のイメージレイアウトです。( イメージレイアウトの移行はサブパスの依存性で指定します )
	attachments[0].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;   // レンダーパス終了時のイメージレイアウトです。( イメージレイアウトの移行はサブパスの依存性で指定します )
	attachments[0].clearValue.color.float32[0] = 0.1f; // レンダーパス開始時のクリア色
	attachments[0].clearValue.color.float32[1] = 0.1f;
	attachments[0].clearValue.color.float32[2] = 0.1f;
	attachments[0].clearValue.color.float32[3] = 0.1f;

	// サブパス 0
	std::array<V3DAttachmentReference, 1> colorAttachments;
	colorAttachments[0].attachment = 0; // attachments[0] のこと
	colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT; // サブパスの開始時に必要に応じてこのイメージレイアウトに自動的に移行します。

	std::array<V3DSubpassDesc, 1> subpasses;
	subpasses[0].inputAttachmentCount = 0;    // フラグメントシェーダーに入力するアタッチメントはありません。
	subpasses[0].pInputAttachments = nullptr; // ↑に同じ
	subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()); // 出力先となるカラーアタッチメントです。
	subpasses[0].pColorAttachments = colorAttachments.data();                           // ↑に同じ
	subpasses[0].pResolveAttachments = nullptr; // マルチサンプルは行いません。
	subpasses[0].pDepthStencilAttachment = nullptr; // デプスステンシルアタッチメントはありません。
	subpasses[0].preserveAttachmentCount = 0;    // サブパスが一つのため特に内容の保持を保証したいアタッチメントはありません。
	subpasses[0].pPreserveAttachments = nullptr; // ↑に同じ

	// サブパスの依存性
	std::array<V3DSubpassDependencyDesc, 2> subpassDependencies;

	// レンダーパス開始 → サブパス 0 への移行を定義します。
	subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL; // サブパス外 ( レンダーパス開始 ) からの移行なので V3D_SUBPASS_EXTERNAL を指定します。
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;             // 最初のステージから
	subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT; // カラーアタッチメントのステージへ移行します。
	subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;            // ディスプレイに表示できる読み込みアクセスから
	subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE; // カラーアタッチメントの書き込みアクセスへ移行します。
	subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

	// サブパス 0 → レンーダパス終了 への移行を定義します。
	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].dstSubpass = V3D_SUBPASS_EXTERNAL; // サブパス外 ( レンダーパス終了 ) への移行なので V3D_SUBPASS_EXTERNAL を指定します。
	subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT; // カラーアタッチメントのステージから
	subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;          // 最後のステージへ移行します。
	subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE; // カラーアタッチメントの書き込みアクセスから
	subpassDependencies[1].dstAccessMask = V3D_ACCESS_MEMORY_READ;            // ディスプレイに表示できる読み込みアクセスへ移行します。
	subpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

	V3D_RESULT result = g_pDevice->CreateRenderPass(
		static_cast<uint32_t>(attachments.size()), attachments.data(),
		static_cast<uint32_t>(subpasses.size()), subpasses.data(),
		static_cast<uint32_t>(subpassDependencies.size()), subpassDependencies.data(),
		&g_pRenderPass,
		L"Simple(RenderPass)");

	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// フレーム配列を作成
	// ----------------------------------------------------------------------------------------------------

	// スワップチェイン作成時の記述にある imageCount に 2 を指定 ( ダブルバッファリング ) したので、その分のフレームを作成します。
	// またフレームには、コマンドバッファー、イメージビュー、フレームバッファが格納されています。

	g_FrameCount = swapChainDesc.imageCount;

	g_pFrames = new(std::nothrow) Frame[g_FrameCount];
	if (g_pFrames == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	for (uint32_t i = 0; i < g_FrameCount; i++)
	{
		g_pFrames[i] = Frame{};
	}

	// ----------------------------------------------------------------------------------------------------
	// グラフィックス用のコマンドバッファーを作成
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < g_FrameCount; i++)
	{
		result = g_pDevice->CreateCommandBuffer(g_pGraphicsCommandPool, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &g_pFrames[i].pGraphicsCommandBuffer, L"Simple(CommandBuffer)");
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// スワップチェインからイメージを取得してイメージビューを作成
	// ----------------------------------------------------------------------------------------------------

	result = g_pWorkCommandBuffer->Begin();
	if (result == V3D_OK)
	{
		for (uint32_t i = 0; i < g_FrameCount; i++)
		{
			// イメージを取得します。
			IV3DImage* pImage = nullptr;
			g_pSwapChain->GetImage(i, &pImage);

			// イメージビューを作成します。
			V3DImageViewDesc imageViewDesc{};
			imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
			imageViewDesc.baseLevel = 0;
			imageViewDesc.levelCount = 1;
			imageViewDesc.baseLayer = 0;
			imageViewDesc.layerCount = 1;

			result = g_pDevice->CreateImageView(pImage, imageViewDesc, &g_pFrames[i].pImageView, L"Simple(SwapChainImage)");
			if (result != V3D_OK)
			{
				pImage->Release();
				return result;
			}

			pImage->Release();

			// イメージビューにバリアを張って、ステージ、アクセス、レイアウトを初期化します。
			V3DBarrierImageViewDesc barrier{};
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dependencyFlags = 0;
			barrier.srcAccessMask = 0; // イメージ作成直後は何も指定されていないので 0 を指定します。
			barrier.dstAccessMask = V3D_ACCESS_MEMORY_READ; // イメージレイアウトが V3D_IMAGE_LAYOUT_PRESENT_SRC の場合は V3D_ACCESS_MEMORY_READ というおまじないです。
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED; // キューファミリーの移行はないので両方に V3D_QUEUE_FAMILY_IGNORED を指定します。
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED; // スワップチェイン作成直後のイメージレイアウトは V3D_IMAGE_LAYOUT_UNDEFINED です。
			barrier.dstLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC; // イメージをウィンドウに表示できるレイアウトにします。

			g_pWorkCommandBuffer->BarrierImageView(g_pFrames[i].pImageView, barrier);
		}

		result = g_pWorkCommandBuffer->End();
	}

	if (result == V3D_OK)
	{
		result = g_pFence->Reset();
		if (result == V3D_OK)
		{
			result = g_pWorkQueue->Submit(1, &g_pWorkCommandBuffer, g_pFence);
			if (result == V3D_OK)
			{
				result = g_pFence->Wait();
			}
		}
	}

	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// フレームバッファを作成
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < g_FrameCount; i++)
	{
		std::array<IV3DImageView*, 1> pAttachments = { g_pFrames[i].pImageView };

		result = g_pDevice->CreateFrameBuffer(g_pRenderPass, static_cast<uint32_t>(pAttachments.size()), pAttachments.data(), &g_pFrames[i].pFrameBuffer, L"Simple(FrameBuffer)");
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void ReleaseSwapChainChilds()
{
	if (g_pFrames != nullptr)
	{
		for (uint32_t i = 0; i < g_FrameCount; i++)
		{
			if (g_pFrames[i].pFrameBuffer != nullptr)
			{
				g_pFrames[i].pFrameBuffer->Release();
			}

			if (g_pFrames[i].pImageView != nullptr)
			{
				g_pFrames[i].pImageView->Release();
			}

			if (g_pFrames[i].pGraphicsCommandBuffer != nullptr)
			{
				g_pFrames[i].pGraphicsCommandBuffer->Release();
			}
		}

		delete[] g_pFrames;
		g_pFrames = nullptr;
	}

	if (g_pRenderPass != nullptr)
	{
		g_pRenderPass->Release();
		g_pRenderPass = nullptr;
	}
}

void ReleaseObjects()
{
	// ----------------------------------------------------------------------------------------------------
	// デバイスの処理が完了するまで待機します。
	//   デバイスの処理が完了しないと、デバイスから作成されたインターフェースは正常に解放することができません。
	// ----------------------------------------------------------------------------------------------------

	if (g_pDevice != nullptr)
	{
		g_pDevice->WaitIdle();
	}

	// ----------------------------------------------------------------------------------------------------
	// 各種インターフェースを破棄します。
	// ----------------------------------------------------------------------------------------------------

	ReleaseSwapChainChilds();

	if (g_pSwapChain != nullptr)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}

	if (g_WindowHandle != nullptr)
	{
		DestroyWindow(g_WindowHandle);
		g_WindowHandle = nullptr;
	}

	if (g_pWorkCommandBuffer != nullptr)
	{
		g_pWorkCommandBuffer->Release();
		g_pWorkCommandBuffer = nullptr;
	}

	if (g_pGraphicsCommandPool != nullptr)
	{
		g_pGraphicsCommandPool->Release();
		g_pGraphicsCommandPool = nullptr;
	}

	if (g_pWorkQueue != nullptr)
	{
		g_pWorkQueue->Release();
		g_pWorkQueue = nullptr;
	}

	if (g_pWorkQueue != nullptr)
	{
		g_pWorkQueue->Release();
		g_pWorkQueue = nullptr;
	}

	if (g_pGraphicsQueue != nullptr)
	{
		g_pGraphicsQueue->Release();
		g_pGraphicsQueue = nullptr;
	}

	if (g_pFence != nullptr)
	{
		g_pFence->Release();
		g_pFence = nullptr;
	}

	if (g_pDevice != nullptr)
	{
		g_pDevice->Release();
		g_pDevice = nullptr;
	}

	if (g_pAdapter != nullptr)
	{
		g_pAdapter->Release();
		g_pAdapter = nullptr;
	}

	if (g_pInstance != nullptr)
	{
		g_pInstance->Release();
		g_pInstance = nullptr;
	}
}

void V3D_CALLBACK LogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData)
{
	switch (type)
	{
	case V3D_LOG_INFORMATION:
		OutputDebugString(L"( info ) : ");
		break;
	case V3D_LOG_WARNING:
		OutputDebugString(L"( warning ) : ");
		break;
	case V3D_LOG_PERFORMANCE_WARNING:
		OutputDebugString(L"( performance warning ) : ");
		break;
	case V3D_LOG_ERROR:
		OutputDebugString(L"( error ) : ");
		break;
	case V3D_LOG_DEBUG:
		OutputDebugString(L"( debug ) : ");
		break;
	}

	OutputDebugString(pMessage);
}

void V3D_CALLBACK LostSwapChainFunction(void* pUserData)
{
	// スワップチェインに関係するインターフェースを破棄します。
	ReleaseSwapChainChilds();
}

void V3D_CALLBACK RestoreSwapChainFunction(void* pUserData)
{
	// スワップチェインが復帰した際にイメージのプロパティ ( フォーマットやサイズなど )が変化することがあるため、
	// スワップチェインに関係するインターフェースを作成しなおします。
	CreateSwapChainChilds();
}

LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		g_WindowHandle = nullptr;
		ReleaseObjects();
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(windowHandle, message, wparam, lparam);
}
