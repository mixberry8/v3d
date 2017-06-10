#include <Windows.h>
#include <new>
#include <array>
#include <v3d.h>

#ifdef _DEBUG
#include <crtdbg.h>
#endif //_DEBUG

// ----------------------------------------------------------------------------------------------------
// �萔
// ----------------------------------------------------------------------------------------------------

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define QUEUE_COUNT 2 // �O���t�B�b�N�X�ƍ�Ɨp�̓��

// ----------------------------------------------------------------------------------------------------
// �\����
// ----------------------------------------------------------------------------------------------------

struct Frame
{
	IV3DCommandBuffer* pGraphicsCommandBuffer;
	IV3DImageView* pImageView;
	IV3DFrameBuffer* pFrameBuffer;
};

// ----------------------------------------------------------------------------------------------------
// �O���[�o���֐�
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
// �O���[�o���ϐ�
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
// ���C��
// ----------------------------------------------------------------------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); 
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// �C���X�^���X���쐬
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
	// �A�_�v�^���擾
	// ----------------------------------------------------------------------------------------------------

	if (g_pInstance->GetAdapterCount() == 0)
	{
		return -1;
	}

	g_pInstance->GetAdapter(0, &g_pAdapter);

	// ----------------------------------------------------------------------------------------------------
	// �f�o�C�X���쐬
	// ----------------------------------------------------------------------------------------------------

	result = g_pInstance->CreateDevice(g_pAdapter, &g_pDevice, L"Simple(Device)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// �t�F���X���擾
	// ----------------------------------------------------------------------------------------------------

	result = g_pDevice->CreateFence(&g_pFence, L"Simple(Fence)");
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// �L���[���擾
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

	// �O���t�B�b�N�X�p
	g_pDevice->GetQueue(queueFamily, 0, &g_pGraphicsQueue);

	// ��Ɨp
	g_pDevice->GetQueue(queueFamily, 1, &g_pWorkQueue);

	// ----------------------------------------------------------------------------------------------------
	// �O���t�B�b�N�X�p�̃R�}���h�v�[�����쐬
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
	// ��Ɨp�̃R�}���h�o�b�t�@���쐬
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
	// �E�B���h�E���쐬
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
	// �X���b�v�`�F�C�����쐬
	// ----------------------------------------------------------------------------------------------------

	V3DSwapChainDesc swapChainDesc{};
	swapChainDesc.windowHandle = g_WindowHandle;
	swapChainDesc.imageFormat = V3D_FORMAT_UNDEFINED; // ����T�[�t�F�C�X�̃t�H�[�}�b�g��񋓂��邱�Ƃ��ł��Ȃ��̂� V3D_FORMAT_UNDEFINED �ɂ��Ă��������B
	swapChainDesc.imageWidth = SCREEN_WIDTH;
	swapChainDesc.imageHeight = SCREEN_HEIGHT;
	swapChainDesc.imageCount = 2;
	swapChainDesc.imageUsageFlags = V3D_IMAGE_USAGE_COLOR_ATTACHMENT; // �C���[�W�̏o�͂Ȃ̂� V3D_IMAGE_USAGE_COLOR_ATTACHMENT �͕K���w�肵�Ă��������B
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

	// �X���b�v�`�F�C���̍쐬�͒v���I�ȃG���[���������Ȃ�����G���[�͏o���܂���B
	// �X���b�v�`�F�C���̋L�q�͎����ɂ���ĕύX�����\�������邽�߁A�擾���Ȃ����܂�
	swapChainDesc = g_pSwapChain->GetDesc();

	// ----------------------------------------------------------------------------------------------------
	// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X���쐬
	// ----------------------------------------------------------------------------------------------------

	result = CreateSwapChainChilds();
	if (result != V3D_OK)
	{
		ReleaseObjects();
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// ���C�����[�v
	// ----------------------------------------------------------------------------------------------------

	bool loop = true;

	do
	{
		// ���b�Z�[�W�|���v
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

		// �`��
		if (loop == true)
		{
			Frame* pFrame = nullptr;

			// �t���[�����J�n
			V3D_RESULT result = BeginFrame(&pFrame);
			if (result == V3D_OK)
			{
				// �����_�[�p�X���J�n
				pFrame->pGraphicsCommandBuffer->BeginRenderPass(g_pRenderPass, pFrame->pFrameBuffer, true);

				// �����ŕ`����s��

				// �����_�[�p�X���I��
				pFrame->pGraphicsCommandBuffer->EndRenderPass();

				// �t���[�����I��
				EndFrame();
			}
		}

	} while (loop == true);

	// ----------------------------------------------------------------------------------------------------

	// ����� WM_DESTROY �ōs���܂�

	// ----------------------------------------------------------------------------------------------------

	return 0;
}

V3D_RESULT BeginFrame(Frame** ppFrame)
{
	// ���̃C���[�W���l�����܂��B
	// ������ g_pSwapChain->GetCurrentImageIndex() �Ŏ擾�ł���C���[�W�̃C���f�b�N�X���ς��܂��B
	V3D_RESULT result = g_pSwapChain->AcquireNextImage();
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return result;
	}

	// ���݂̃t���[�����擾���܂��B
	Frame* pFrame = &g_pFrames[g_pSwapChain->GetCurrentImageIndex()];

	// �R�}���h�̏������݂��J�n���܂��B
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

	// �R�}���h�̏������݂��I�����܂��B
	V3D_RESULT result = pFrame->pGraphicsCommandBuffer->End();
	if (result != V3D_OK)
	{
		CloseWindow(g_WindowHandle);
		return;
	}

	// �t�F���X�����Z�b�g���āA�R�}���h�𑗐M���A������ҋ@���܂��B
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

	// �C���[�W��\�����܂�
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
	// �����_�[�p�X���쐬
	// ----------------------------------------------------------------------------------------------------

	// �A�^�b�`�����g
	std::array<V3DAttachmentDesc, 1> attachments;
	attachments[0].format = swapChainDesc.imageFormat; // 0 �Ԃ߂̃A�^�b�`�����g�̃X���b�v�`�F�C���̃C���[�W�ł��B
	attachments[0].samples = V3D_SAMPLE_COUNT_1;       // �X���b�v�`�F�C���̃C���[�W�͏�� V3D_SAMPLE_COUNT_1 �ł��B
	attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;   // �����_�[�p�X�J�n���ɃA�^�b�`�����g�̓��e���N���A���܂��B
	attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE; // �����_�[�p�X�I�����ɃA�^�b�`�����g�̓��e��ۑ����܂��B
	attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;   // �X�e���V���͂Ȃ��̂Œ�`���܂���B
	attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED; // ���ɓ���
	attachments[0].initialLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC; // �����_�[�p�X�J�n���̃C���[�W���C�A�E�g�ł��B( �C���[�W���C�A�E�g�̈ڍs�̓T�u�p�X�̈ˑ����Ŏw�肵�܂� )
	attachments[0].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;   // �����_�[�p�X�I�����̃C���[�W���C�A�E�g�ł��B( �C���[�W���C�A�E�g�̈ڍs�̓T�u�p�X�̈ˑ����Ŏw�肵�܂� )
	attachments[0].clearValue.color.float32[0] = 0.1f; // �����_�[�p�X�J�n���̃N���A�F
	attachments[0].clearValue.color.float32[1] = 0.1f;
	attachments[0].clearValue.color.float32[2] = 0.1f;
	attachments[0].clearValue.color.float32[3] = 0.1f;

	// �T�u�p�X 0
	std::array<V3DAttachmentReference, 1> colorAttachments;
	colorAttachments[0].attachment = 0; // attachments[0] �̂���
	colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT; // �T�u�p�X�̊J�n���ɕK�v�ɉ����Ă��̃C���[�W���C�A�E�g�Ɏ����I�Ɉڍs���܂��B

	std::array<V3DSubpassDesc, 1> subpasses;
	subpasses[0].inputAttachmentCount = 0;    // �t���O�����g�V�F�[�_�[�ɓ��͂���A�^�b�`�����g�͂���܂���B
	subpasses[0].pInputAttachments = nullptr; // ���ɓ���
	subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()); // �o�͐�ƂȂ�J���[�A�^�b�`�����g�ł��B
	subpasses[0].pColorAttachments = colorAttachments.data();                           // ���ɓ���
	subpasses[0].pResolveAttachments = nullptr; // �}���`�T���v���͍s���܂���B
	subpasses[0].pDepthStencilAttachment = nullptr; // �f�v�X�X�e���V���A�^�b�`�����g�͂���܂���B
	subpasses[0].preserveAttachmentCount = 0;    // �T�u�p�X����̂��ߓ��ɓ��e�̕ێ���ۏ؂������A�^�b�`�����g�͂���܂���B
	subpasses[0].pPreserveAttachments = nullptr; // ���ɓ���

	// �T�u�p�X�̈ˑ���
	std::array<V3DSubpassDependencyDesc, 2> subpassDependencies;

	// �����_�[�p�X�J�n �� �T�u�p�X 0 �ւ̈ڍs���`���܂��B
	subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL; // �T�u�p�X�O ( �����_�[�p�X�J�n ) ����̈ڍs�Ȃ̂� V3D_SUBPASS_EXTERNAL ���w�肵�܂��B
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;             // �ŏ��̃X�e�[�W����
	subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT; // �J���[�A�^�b�`�����g�̃X�e�[�W�ֈڍs���܂��B
	subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;            // �f�B�X�v���C�ɕ\���ł���ǂݍ��݃A�N�Z�X����
	subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE; // �J���[�A�^�b�`�����g�̏������݃A�N�Z�X�ֈڍs���܂��B
	subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

	// �T�u�p�X 0 �� �����[�_�p�X�I�� �ւ̈ڍs���`���܂��B
	subpassDependencies[1].srcSubpass = 0;
	subpassDependencies[1].dstSubpass = V3D_SUBPASS_EXTERNAL; // �T�u�p�X�O ( �����_�[�p�X�I�� ) �ւ̈ڍs�Ȃ̂� V3D_SUBPASS_EXTERNAL ���w�肵�܂��B
	subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT; // �J���[�A�^�b�`�����g�̃X�e�[�W����
	subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;          // �Ō�̃X�e�[�W�ֈڍs���܂��B
	subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE; // �J���[�A�^�b�`�����g�̏������݃A�N�Z�X����
	subpassDependencies[1].dstAccessMask = V3D_ACCESS_MEMORY_READ;            // �f�B�X�v���C�ɕ\���ł���ǂݍ��݃A�N�Z�X�ֈڍs���܂��B
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
	// �t���[���z����쐬
	// ----------------------------------------------------------------------------------------------------

	// �X���b�v�`�F�C���쐬���̋L�q�ɂ��� imageCount �� 2 ���w�� ( �_�u���o�b�t�@�����O ) �����̂ŁA���̕��̃t���[�����쐬���܂��B
	// �܂��t���[���ɂ́A�R�}���h�o�b�t�@�[�A�C���[�W�r���[�A�t���[���o�b�t�@���i�[����Ă��܂��B

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
	// �O���t�B�b�N�X�p�̃R�}���h�o�b�t�@�[���쐬
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
	// �X���b�v�`�F�C������C���[�W���擾���ăC���[�W�r���[���쐬
	// ----------------------------------------------------------------------------------------------------

	result = g_pWorkCommandBuffer->Begin();
	if (result == V3D_OK)
	{
		for (uint32_t i = 0; i < g_FrameCount; i++)
		{
			// �C���[�W���擾���܂��B
			IV3DImage* pImage = nullptr;
			g_pSwapChain->GetImage(i, &pImage);

			// �C���[�W�r���[���쐬���܂��B
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

			// �C���[�W�r���[�Ƀo���A�𒣂��āA�X�e�[�W�A�A�N�Z�X�A���C�A�E�g�����������܂��B
			V3DBarrierImageViewDesc barrier{};
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dependencyFlags = 0;
			barrier.srcAccessMask = 0; // �C���[�W�쐬����͉����w�肳��Ă��Ȃ��̂� 0 ���w�肵�܂��B
			barrier.dstAccessMask = V3D_ACCESS_MEMORY_READ; // �C���[�W���C�A�E�g�� V3D_IMAGE_LAYOUT_PRESENT_SRC �̏ꍇ�� V3D_ACCESS_MEMORY_READ �Ƃ������܂��Ȃ��ł��B
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED; // �L���[�t�@�~���[�̈ڍs�͂Ȃ��̂ŗ����� V3D_QUEUE_FAMILY_IGNORED ���w�肵�܂��B
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED; // �X���b�v�`�F�C���쐬����̃C���[�W���C�A�E�g�� V3D_IMAGE_LAYOUT_UNDEFINED �ł��B
			barrier.dstLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC; // �C���[�W���E�B���h�E�ɕ\���ł��郌�C�A�E�g�ɂ��܂��B

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
	// �t���[���o�b�t�@���쐬
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
	// �f�o�C�X�̏�������������܂őҋ@���܂��B
	//   �f�o�C�X�̏������������Ȃ��ƁA�f�o�C�X����쐬���ꂽ�C���^�[�t�F�[�X�͐���ɉ�����邱�Ƃ��ł��܂���B
	// ----------------------------------------------------------------------------------------------------

	if (g_pDevice != nullptr)
	{
		g_pDevice->WaitIdle();
	}

	// ----------------------------------------------------------------------------------------------------
	// �e��C���^�[�t�F�[�X��j�����܂��B
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
	// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X��j�����܂��B
	ReleaseSwapChainChilds();
}

void V3D_CALLBACK RestoreSwapChainFunction(void* pUserData)
{
	// �X���b�v�`�F�C�������A�����ۂɃC���[�W�̃v���p�e�B ( �t�H�[�}�b�g��T�C�Y�Ȃ� )���ω����邱�Ƃ����邽�߁A
	// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X���쐬���Ȃ����܂��B
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
