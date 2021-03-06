#include "Application.h"
#include "ViewCameraWindow.h"
#include "Font.h"
#include <time.h>

#define PARTICLE_MAX_AMOUNT 1024
#define PARTICLE_MAX (PARTICLE_MAX_AMOUNT * 256)

// 0 : フェンスで待機
// 1 : セマフォで待機
// 2 : イベントで待機 ( グラフィックスキューとコンピュートキューのファミリーが一致していないと動作しません )
#define SYNC_MODE 2

class ComputeParticleWindow : public ViewCameraWindow
{
public:
	ComputeParticleWindow() :
		m_GraphicsQueueFamily(0),
		m_ComputeQueueFamily(0),
		m_pComputeQueue(nullptr),
		m_pComputeFence(nullptr),
		m_pComputeShaderModule(nullptr),
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pCompPipelineLayout(nullptr),
		m_pComputePipeline(nullptr),
		m_pComputeDedscriptorSet(nullptr),
		m_pComputeDedscriptorSetLayout(nullptr),
		m_pGraphicsPipelineLayout(nullptr),
		m_pDepthStencilImage(nullptr),
		m_pDepthStencilImageView(nullptr),
		m_pRenderPass(nullptr),
		m_pGraphicsPipeline(nullptr),
		m_PointSize(1)
	{
#if SYNC_MODE == 1
		m_pComputeSemaphore = nullptr;
#elif SYNC_MODE == 2
		m_pComputeEvent = nullptr;
#endif //SYNC_MODE
		for (auto i = 0; i < _countof(m_pParticleBuffer); i++)
		{
			m_pParticleBuffer[i] = nullptr;
		}
	}

	virtual ~ComputeParticleWindow() {}

	bool Initialize()
	{
		// ----------------------------------------------------------------------------------------------------
		// キューファミリを取得
		// ----------------------------------------------------------------------------------------------------

		m_GraphicsQueueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 2);
		if (m_GraphicsQueueFamily == ~0U)
		{
			return false;
		}

		m_ComputeQueueFamily = Application::FindQueueFamily(V3D_QUEUE_COMPUTE, 1);
		if (m_ComputeQueueFamily == ~0U)
		{
			return false;
		}

#if SYNC_MODE == 2
		if (m_GraphicsQueueFamily != m_ComputeQueueFamily)
		{
			// イベントを使用するときはキューファミリーは一致している必要がある
			return false;
		}
#endif //SYNC_MODE

		// ----------------------------------------------------------------------------------------------------
		// キューを取得
		// ----------------------------------------------------------------------------------------------------

		IV3DQueue* pWorkQueue;
		IV3DQueue* pGraphicsQueue;

		if (m_GraphicsQueueFamily == m_ComputeQueueFamily)
		{
			if (Application::GetDevice()->GetQueueFamilyDesc(m_GraphicsQueueFamily).queueCount < 3)
			{
				return false;
			}

			Application::GetDevice()->GetQueue(m_ComputeQueueFamily, 2, &m_pComputeQueue);
		}
		else
		{
			Application::GetDevice()->GetQueue(m_ComputeQueueFamily, 0, &m_pComputeQueue);
		}

		Application::GetDevice()->GetQueue(m_GraphicsQueueFamily, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(m_GraphicsQueueFamily, 1, &pGraphicsQueue);

		// ----------------------------------------------------------------------------------------------------
		// ウィンドウを初期化
		// ----------------------------------------------------------------------------------------------------

		if (Window::Initialize(L"computeParticle", 1024, 768, false, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// 乱数の初期化
		// ----------------------------------------------------------------------------------------------------

		srand(static_cast<uint32_t>(time(nullptr)));

		// ----------------------------------------------------------------------------------------------------
		// フォントを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Initialize(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), L"Arial", 16, 400) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// ポイントのサイズ ( gl_PointSize )
		// ----------------------------------------------------------------------------------------------------

		const V3DDeviceCaps& deviceCaps = Application::GetDevice()->GetCaps();
		m_PointSize = CLAMP(2, deviceCaps.minPointSize, deviceCaps.maxPointSize);

		// ----------------------------------------------------------------------------------------------------
		// カメラを初期化
		// ----------------------------------------------------------------------------------------------------

		SetCameraRotation(Vector3(1.0f, 0.0f, 0.0f).ToNormalize(), DEG_TO_RAD(30.0f));
		SetCameraDistance(6.0f);
		SetCameraAt(Vector3(0.0f, -2.0f, 0.0f));

		// ----------------------------------------------------------------------------------------------------
		// シェーダーモジュールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			// コンピュートシェーダー
			CreateFilePath(L"shader\\source\\computeParticle.comp", srcFilePath);
			CreateFilePath(L"shader\\computeParticle.comp.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pComputeShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// バーテックスシェーダー
			CreateFilePath(L"shader\\source\\computeParticle.vert", srcFilePath);
			CreateFilePath(L"shader\\computeParticle.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// フラグメントシェーダー
			CreateFilePath(L"shader\\source\\computeParticle.frag", srcFilePath);
			CreateFilePath(L"shader\\computeParticle.frag.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pFragmentShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// コマンドバッファを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		{
			V3DCommandPoolDesc poolDesc{};
			poolDesc.usageFlags = V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER;
			poolDesc.queueFamily = m_ComputeQueueFamily;

			V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(poolDesc, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &m_pComputeCommandBuffer);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// 同期オブジェクトを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		{
			V3D_RESULT result = Application::GetDevice()->CreateFence(true, &m_pComputeFence, L"ComputeFence");
			if (result != V3D_OK)
			{
				return false;
			}

#if SYNC_MODE == 1
			result = Application::GetDevice()->CreateSemaphore(&m_pComputeSemaphore, L"ComputeSemaphore");
			if (result != V3D_OK)
			{
				return false;
			}
#elif SYNC_MODE == 2
			result = Application::GetDevice()->CreateEvent(&m_pComputeEvent, L"ComputeEvent");
			if (result != V3D_OK)
			{
				return false;
			}
#endif //SYNC_MODE
		}

		// ----------------------------------------------------------------------------------------------------
		// デスクリプタセットレイアウトを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		{
			Array1<V3DDescriptorDesc, 2> descriptors =
			{
				{
					{ 0, V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER, V3D_SHADER_STAGE_COMPUTE },
					{ 1, V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER, V3D_SHADER_STAGE_COMPUTE }
				}
			};

			V3D_RESULT result = Application::GetDevice()->CreateDescriptorSetLayout(TO_UI32(descriptors.size()), descriptors.data(), 1, 1, &m_pComputeDedscriptorSetLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// パイプラインレイアウトを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		{
			Array1<V3DConstantDesc, 1> constants =
			{
				{
					{ V3D_SHADER_STAGE_COMPUTE, 0, sizeof(ComputeParticleWindow::CompConstant) },
				}
			};

			Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts = { m_pComputeDedscriptorSetLayout };

			V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(
				TO_UI32(constants.size()), constants.data(),
				TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
				&m_pCompPipelineLayout);

			if (result != V3D_OK)
			{
				return false;
			}
		}

		// グラフィックス
		{
			Array1<V3DConstantDesc, 1> constants =
			{
				{
					{ V3D_SHADER_STAGE_VERTEX, 0, sizeof(ComputeParticleWindow::VertConstant) }
				}
			};

			V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(TO_UI32(constants.size()), constants.data(), 0, nullptr, &m_pGraphicsPipelineLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// パイプラインを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		{
			V3DComputePipelineDesc desc{};
			desc.computeShader.pModule = m_pComputeShaderModule;
			desc.computeShader.pEntryPointName = "main";

			V3D_RESULT result = Application::GetDevice()->CreateComputePipeline(m_pCompPipelineLayout, desc, &m_pComputePipeline);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// パーティクルバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		uint64_t vertexSize = sizeof(Vertex) * PARTICLE_MAX;

		for (auto i = 0; i < _countof(m_pParticleBuffer); i++)
		{
			V3DBufferDesc bufferDesc{};
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_STORAGE | V3D_BUFFER_USAGE_VERTEX;
			bufferDesc.size = vertexSize;

			V3D_RESULT result = Application::GetDevice()->CreateBuffer(bufferDesc, &m_pParticleBuffer[i]);
			if (result != V3D_OK)
			{
				return false;
			}

			V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
			if (Application::GetDevice()->CheckResourceMemoryProperty(memoryPropertyFlags, 1, reinterpret_cast<IV3DResource**>(&m_pParticleBuffer[i])) != V3D_OK)
			{
				memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
			}

			result = Application::GetDevice()->AllocateResourceMemoryAndBind(memoryPropertyFlags, m_pParticleBuffer[i]);
			if (result != V3D_OK)
			{
				return false;
			}

			uint8_t* pMemory;
			result = m_pParticleBuffer[i]->Map(0, vertexSize, reinterpret_cast<void**>(&pMemory));
			if (result == V3D_OK)
			{
				ComputeParticleWindow::Vertex* pVertex = reinterpret_cast<ComputeParticleWindow::Vertex*>(pMemory);

				for (uint32_t i = 0; i < PARTICLE_MAX; i++)
				{
					float px = (rand() % 100) / 1000.0f - 0.05f;
					float py = (rand() % 100) / 1000.0f - 0.05f;
					float pz = (rand() % 100) / 1000.0f - 0.05f;

					float vx = (rand() % 1000) / 1000.0f - 0.5f;
					float vy = (rand() % 2000) / 1000.0f + 1.0f;
					float vz = (rand() % 1000) / 1000.0f - 0.5f;

					pVertex->pos.Set(px, py, pz, 1.0f);
					pVertex->velocity.Set(vx, -vy, vz, 1.0f);

					pVertex++;
				}

				result = m_pParticleBuffer[i]->Unmap();
				if (result != V3D_OK)
				{
					return false;
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// パーティクルバッファービューを作成
		// ----------------------------------------------------------------------------------------------------

		IV3DCommandBuffer* pCommandBuffer = BeginWork();
		if (pCommandBuffer == nullptr)
		{
			return false;
		}

		// 二つ目のバッファはパーティクルの初期化用
		V3DPipelineBarrier pipelineBarrier{};
		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_COMPUTE_SHADER;

		V3DBufferMemoryBarrier memoryBarrier{};
		memoryBarrier.srcAccessMask = V3D_ACCESS_VERTEX_READ;
		memoryBarrier.dstAccessMask = V3D_ACCESS_SHADER_READ;
		memoryBarrier.srcQueueFamily = m_GraphicsQueueFamily;
		memoryBarrier.dstQueueFamily = m_ComputeQueueFamily;
		memoryBarrier.pBuffer = m_pParticleBuffer[1];
		memoryBarrier.offset = 0;
		memoryBarrier.size = vertexSize;
		pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

		if (EndWork() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// デスクリプタセットを作成
		// ----------------------------------------------------------------------------------------------------

		// コンピュート
		V3D_RESULT result = Application::GetDevice()->CreateDescriptorSet(m_pComputeDedscriptorSetLayout, &m_pComputeDedscriptorSet);
		if (result != V3D_OK)
		{
			return false;
		}

		m_pComputeDedscriptorSet->SetBuffer(0, m_pParticleBuffer[0], 0, vertexSize);
		m_pComputeDedscriptorSet->SetBuffer(1, m_pParticleBuffer[1], 0, vertexSize);
		m_pComputeDedscriptorSet->Update();

		// ----------------------------------------------------------------------------------------------------
		// スワップチェインに関係するインターフェースを作成
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		for (auto i = 0; i < _countof(m_pParticleBuffer); i++)
		{
			SAFE_RELEASE(m_pParticleBuffer[i]);
		}

		SAFE_RELEASE(m_pGraphicsPipelineLayout);
		SAFE_RELEASE(m_pComputePipeline);
		SAFE_RELEASE(m_pCompPipelineLayout);
		SAFE_RELEASE(m_pComputeDedscriptorSet);
		SAFE_RELEASE(m_pComputeDedscriptorSetLayout);
		SAFE_RELEASE(m_pComputeCommandBuffer);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);
		SAFE_RELEASE(m_pComputeShaderModule);
		SAFE_RELEASE(m_pComputeFence);
		SAFE_RELEASE(m_pComputeQueue);

#if SYNC_MODE == 1
		SAFE_RELEASE(m_pComputeSemaphore);
#elif SYNC_MODE == 2
		SAFE_RELEASE(m_pComputeEvent);
#endif //SYNC_MODE

		m_Font.Dispose();
	}

	virtual bool OnIdle() override
	{
		// ----------------------------------------------------------------------------------------------------
		// カメラを更新
		// ----------------------------------------------------------------------------------------------------

		ViewCameraWindow::OnIdle();

		// ----------------------------------------------------------------------------------------------------
		// テキストを更新
		// ----------------------------------------------------------------------------------------------------

		m_Font.Reset();

		std::wstringstream stringStream;
		stringStream << L"Fps " << Application::GetAverageFpsPreSec();

		m_Font.AddText(16, 16, stringStream.str().c_str());

		if (m_Font.Bake() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// 定数を準備
		// ----------------------------------------------------------------------------------------------------

		ComputeParticleWindow::CompConstant compConstant{};
		compConstant.deltaTime = static_cast<float>(Application::GetDeltaTime());
		compConstant.particleCount = PARTICLE_MAX;

		ComputeParticleWindow::VertConstant vertConstant{};
		vertConstant.worldViewProjectionMatrix = GetCamera()->GetViewProjectionMatrix();
		vertConstant.size = m_PointSize;

		// ----------------------------------------------------------------------------------------------------
		// コンピュート
		// ----------------------------------------------------------------------------------------------------

#if SYNC_MODE == 2
		m_pComputeFence->Wait(V3D_INFINITE);
#endif //SYNC_MODE

		CommandHelper compCommandHelper(m_pComputeQueue, m_pComputeCommandBuffer);

		if (compCommandHelper.Begin() == nullptr)
		{
			return false;
		}

		V3DPipelineBarrier pipelineBarrier{};

		V3DBufferMemoryBarrier memoryBarrier{};
		memoryBarrier.pBuffer = m_pParticleBuffer[0];
		memoryBarrier.offset = 0;
		memoryBarrier.size = 0;

		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_COMPUTE_SHADER;

		memoryBarrier.srcAccessMask = V3D_ACCESS_VERTEX_READ;
		memoryBarrier.dstAccessMask = V3D_ACCESS_SHADER_READ | V3D_ACCESS_SHADER_WRITE;
		memoryBarrier.srcQueueFamily = m_GraphicsQueueFamily;
		memoryBarrier.dstQueueFamily = m_ComputeQueueFamily;
		m_pComputeCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

		m_pComputeCommandBuffer->PushConstant(m_pCompPipelineLayout, 0, &compConstant);
		m_pComputeCommandBuffer->BindPipeline(m_pComputePipeline);
		m_pComputeCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_COMPUTE, m_pCompPipelineLayout, 0, m_pComputeDedscriptorSet);
		m_pComputeCommandBuffer->Dispatch(PARTICLE_MAX / 256, 1, 1);

		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_COMPUTE_SHADER;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_VERTEX_INPUT;

		memoryBarrier.srcAccessMask = V3D_ACCESS_SHADER_READ | V3D_ACCESS_SHADER_WRITE;
		memoryBarrier.dstAccessMask = V3D_ACCESS_VERTEX_READ;
		memoryBarrier.srcQueueFamily = m_ComputeQueueFamily;
		memoryBarrier.dstQueueFamily = m_GraphicsQueueFamily;
		m_pComputeCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

#if SYNC_MODE == 0
		if (compCommandHelper.End(m_pComputeFence) == false)
		{
			return false;
		}
#elif SYNC_MODE == 1
		if (compCommandHelper.End(m_pComputeSemaphore) == false)
		{
			return false;
		}
#elif SYNC_MODE == 2
		if (compCommandHelper.End(m_pComputeEvent, V3D_PIPELINE_STAGE_VERTEX_INPUT, m_pComputeFence) == false)
		{
			return false;
		}
#endif //SYNC_MODE

		// ----------------------------------------------------------------------------------------------------
		// グラフィックス
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		// レンダーパスを開始
		pCommandBufer->BeginRenderPass(pFrameBuffer, true);

		// サブパス 0 - パーティクルの描画
		V3DViewport viewport{};
		viewport.rect.width = swapChainDesc.imageWidth;
		viewport.rect.height = swapChainDesc.imageHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		pCommandBufer->SetViewport(0, 1, &viewport);

		V3DRectangle2D scissor{};
		scissor.width = swapChainDesc.imageWidth;
		scissor.height = swapChainDesc.imageHeight;
		pCommandBufer->SetScissor(0, 1, &scissor);

		pCommandBufer->PushConstant(m_pGraphicsPipelineLayout, 0, &vertConstant);
		pCommandBufer->BindPipeline(m_pGraphicsPipeline);

#if SYNC_MODE == 2
		pCommandBufer->WaitEvent(1, &m_pComputeEvent, V3D_PIPELINE_STAGE_HOST, V3D_PIPELINE_STAGE_VERTEX_INPUT);
#endif //SYNC_MODE

		pCommandBufer->BindVertexBuffer(0, m_pParticleBuffer[0]);

		pCommandBufer->Draw(PARTICLE_MAX, 1, 0, 0);

		// サブパス 1 - テキストの描画
		pCommandBufer->NextSubpass();
		m_Font.Flush(pCommandBufer);

		// レンダーパスを終了
		pCommandBufer->EndRenderPass();

#if SYNC_MODE == 2
		pCommandBufer->ResetEvent(m_pComputeEvent, V3D_PIPELINE_STAGE_HOST);
#endif //SYNC_MODE

#if (SYNC_MODE == 0) || (SYNC_MODE == 2)
		if (EndGraphics() == false)
		{
			return false;
		}
#elif SYNC_MODE == 1
		if (EndGraphics(m_pComputeSemaphore, V3D_PIPELINE_STAGE_VERTEX_INPUT) == false)
		{
			return false;
		}
#endif //SYNC_MODE

		// ----------------------------------------------------------------------------------------------------
		// 後処理
		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual void OnLostSwapChain() override
	{
		m_Font.Lost();

		SAFE_RELEASE(m_pGraphicsPipeline);

		if (m_FrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_FrameBuffers[i]);
			}
			m_FrameBuffers.clear();
		}

		SAFE_RELEASE(m_pRenderPass);

		SAFE_RELEASE(m_pDepthStencilImageView);
		SAFE_RELEASE(m_pDepthStencilImage);
	}

	virtual bool OnRestoreSwapChain() override
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		// ----------------------------------------------------------------------------------------------------
		// デプスステンシルイメージを作成
		// ----------------------------------------------------------------------------------------------------

		{
			V3DImageDesc imageDesc{};
			imageDesc.type = V3D_IMAGE_TYPE_2D;
			imageDesc.width = swapChainDesc.imageWidth;
			imageDesc.height = swapChainDesc.imageHeight;
			imageDesc.depth = 1;
			imageDesc.levelCount = 1;
			imageDesc.layerCount = 1;
			imageDesc.samples = V3D_SAMPLE_COUNT_1;
			imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
			imageDesc.usageFlags = V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;

			// フォーマットが使用できるかチェックする
			imageDesc.format = V3D_FORMAT_D24_UNORM_S8_UINT;
			if (Application::GetDevice()->GetImageFormatDesc(imageDesc.format, imageDesc.type, imageDesc.tiling, imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				imageDesc.format = V3D_FORMAT_D16_UNORM;
				if (Application::GetDevice()->GetImageFormatDesc(imageDesc.format, imageDesc.type, imageDesc.tiling, imageDesc.usageFlags, nullptr) != V3D_OK)
				{
					imageDesc.format = V3D_FORMAT_D16_UNORM_S8_UINT;
					if (Application::GetDevice()->GetImageFormatDesc(imageDesc.format, imageDesc.type, imageDesc.tiling, imageDesc.usageFlags, nullptr) != V3D_OK)
					{
						return false;
					}
				}
			}

			V3D_RESULT result = Application::GetDevice()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &m_pDepthStencilImage);
			if (result != V3D_OK)
			{
				return false;
			}

			result = Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, m_pDepthStencilImage);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// デプスステンシルイメージビューを作成
		// ----------------------------------------------------------------------------------------------------

		{
			V3DImageViewDesc imageViewDesc{};
			imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
			imageViewDesc.baseLevel = 0;
			imageViewDesc.levelCount = 1;
			imageViewDesc.baseLayer = 0;
			imageViewDesc.layerCount = 1;

			V3D_RESULT result = Application::GetDevice()->CreateImageView(m_pDepthStencilImage, imageViewDesc, &m_pDepthStencilImageView);
			if (result != V3D_OK)
			{
				return false;
			}

			IV3DCommandBuffer* pCommandBuffer = BeginWork();
			if (pCommandBuffer == nullptr)
			{
				return false;
			}

			V3DPipelineBarrier pipelineBarrier{};
			pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS | V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS;

			V3DImageViewMemoryBarrier memoryBarrier{};
			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
			memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			memoryBarrier.dstLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
			memoryBarrier.pImageView = m_pDepthStencilImageView;
			pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

			EndWork();
		}

		// ----------------------------------------------------------------------------------------------------
		// レンダーパスを作成
		// ----------------------------------------------------------------------------------------------------

		/******************/
		/* アタッチメント */
		/******************/

		std::array<V3DAttachmentDesc, 2> attachments;

		// カラー
		attachments[0].format = swapChainDesc.imageFormat;
		attachments[0].samples = V3D_SAMPLE_COUNT_1;
		attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[0].initialLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		attachments[0].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		attachments[0].clearValue.color.float32[0] = 0.1f;
		attachments[0].clearValue.color.float32[1] = 0.1f;
		attachments[0].clearValue.color.float32[2] = 0.1f;
		attachments[0].clearValue.color.float32[3] = 0.1f;

		// デプスステンシル
		attachments[1].format = m_pDepthStencilImage->GetDesc().format;
		attachments[1].samples = V3D_SAMPLE_COUNT_1;
		attachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;   // ステンシルは使用しない
		attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED; // ↑に同じ
		attachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].clearValue.depthStencil.depth = 1.0f;
		attachments[1].clearValue.depthStencil.stencil = 0;

		std::array<V3DSubpassDesc, 2> subpasses;

		/***********************************/
		/* サブパス 0 - パーティクルの描画 */
		/***********************************/

		// カラー
		std::array<V3DAttachmentReference, 1> colorAttachments0;
		colorAttachments0[0].attachment = 0;
		colorAttachments0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// デプスステンシル
		V3DAttachmentReference depthStencilAttachment0;
		depthStencilAttachment0.attachment = 1;
		depthStencilAttachment0.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		subpasses[0].inputAttachmentCount = 0;
		subpasses[0].pInputAttachments = nullptr;
		subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments0.size());
		subpasses[0].pColorAttachments = colorAttachments0.data();
		subpasses[0].pResolveAttachments = nullptr;
		subpasses[0].pDepthStencilAttachment = &depthStencilAttachment0;
		subpasses[0].preserveAttachmentCount = 0;
		subpasses[0].pPreserveAttachments = nullptr;

		/*******************************/
		/* サブパス 1 - テキストの描画 */
		/*******************************/

		// カラー
		std::array<V3DAttachmentReference, 1> colorAttachments1;
		colorAttachments1[0].attachment = 0;
		colorAttachments1[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		subpasses[1].inputAttachmentCount = 0;
		subpasses[1].pInputAttachments = nullptr;
		subpasses[1].colorAttachmentCount = static_cast<uint32_t>(colorAttachments1.size());
		subpasses[1].pColorAttachments = colorAttachments1.data();
		subpasses[1].pResolveAttachments = nullptr;
		subpasses[1].pDepthStencilAttachment = nullptr;
		subpasses[1].preserveAttachmentCount = 0;
		subpasses[1].pPreserveAttachments = nullptr;

		/********************/
		/* サブパスの依存性 */
		/********************/

		Array1<V3DSubpassDependencyDesc, 3> subpassDependencies;

		subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dependencyFlags = 0;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = 1;
		subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[1].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		subpassDependencies[2].srcSubpass = 1;
		subpassDependencies[2].dstSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
		subpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[2].dstAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[2].dependencyFlags = 0;

		V3D_RESULT result = Application::GetDevice()->CreateRenderPass(
			TO_UI32(attachments.size()), attachments.data(),
			TO_UI32(subpasses.size()), subpasses.data(),
			TO_UI32(subpassDependencies.size()), subpassDependencies.data(),
			&m_pRenderPass);

		// ----------------------------------------------------------------------------------------------------
		// フレームバッファを作成
		// ----------------------------------------------------------------------------------------------------

		m_FrameBuffers.resize(swapChainDesc.imageCount);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			IV3DImageView* pImageView;
			if (CreateImageView(i, &pImageView) == false)
			{
				return false;
			}

			Array1<IV3DImageView*, 2> attachments =
			{
				pImageView,
				m_pDepthStencilImageView,
			};

			IV3DFrameBuffer* pFrameBuffer = nullptr;
			result = Application::GetDevice()->CreateFrameBuffer(m_pRenderPass, TO_UI32(attachments.size()), attachments.data(), &m_FrameBuffers[i]);
			if (result != V3D_OK)
			{
				SAFE_RELEASE(pImageView);
				return false;
			}

			SAFE_RELEASE(pImageView);
		}

		// ----------------------------------------------------------------------------------------------------
		// グラフィックスパイプラインを作成
		// ----------------------------------------------------------------------------------------------------

		Array1<V3DPipelineVertexElement, 1> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32_SFLOAT }, // position
				//{ 1, 12, V3D_FORMAT_R32G32B32_SFLOAT }  // velocity
			}
		};

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{
			{
				{ 0, sizeof(ComputeParticleWindow::Vertex), 0, TO_UI32(vertexElements.size()) },
			}
		};

		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments;
		colorBlendAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_ADDITION);

		V3DGraphicsPipelineDesc pipelineDesc{};
		pipelineDesc.vertexShader.pModule = m_pVertexShaderModule;
		pipelineDesc.vertexShader.pEntryPointName = "main";
		pipelineDesc.fragmentShader.pModule = m_pFragmentShaderModule;
		pipelineDesc.fragmentShader.pEntryPointName = "main";
		pipelineDesc.vertexInput.elementCount = TO_UI32(vertexElements.size());
		pipelineDesc.vertexInput.pElements = vertexElements.data();
		pipelineDesc.vertexInput.layoutCount = TO_UI32(vertexLayouts.size());
		pipelineDesc.vertexInput.pLayouts = vertexLayouts.data();
		pipelineDesc.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_POINT_LIST;
		pipelineDesc.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipelineDesc.rasterization.cullMode = V3D_CULL_MODE_NONE;
		pipelineDesc.depthStencil.depthTestEnable = true;
		pipelineDesc.depthStencil.depthWriteEnable = true;
		pipelineDesc.depthStencil.depthCompareOp = V3D_COMPARE_OP_LESS;
		pipelineDesc.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipelineDesc.colorBlend.attachmentCount = TO_UI32(colorBlendAttachments.size());
		pipelineDesc.colorBlend.pAttachments = colorBlendAttachments.data();
		pipelineDesc.pRenderPass = m_pRenderPass;
		pipelineDesc.subpass = 0;

		result = Application::GetDevice()->CreateGraphicsPipeline(m_pGraphicsPipelineLayout, pipelineDesc, &m_pGraphicsPipeline);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// フォントを復帰
		// ----------------------------------------------------------------------------------------------------

		m_Font.Restore(m_pRenderPass, 1, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

private:
	struct Vertex
	{
		Vector4 pos;
		Vector4 velocity;
	};

	struct CompConstant
	{
		float deltaTime;
		int32_t particleCount;
	};

	struct VertConstant
	{
		Matrix4x4 worldViewProjectionMatrix;
		float size;
		float reserve[3];
	};

	uint32_t m_GraphicsQueueFamily;
	uint32_t m_ComputeQueueFamily;

	Font m_Font;

	IV3DShaderModule* m_pComputeShaderModule;
	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;

	IV3DQueue* m_pComputeQueue;
	IV3DCommandBuffer* m_pComputeCommandBuffer;
	IV3DFence* m_pComputeFence;
#if SYNC_MODE == 1
	IV3DSemaphore* m_pComputeSemaphore;
#elif SYNC_MODE == 2
	IV3DEvent* m_pComputeEvent;
#endif //SYNC_MODE

	IV3DDescriptorSetLayout* m_pComputeDedscriptorSetLayout;

	IV3DPipelineLayout* m_pCompPipelineLayout;
	IV3DPipelineLayout* m_pGraphicsPipelineLayout;

	IV3DBuffer* m_pParticleBuffer[2];

	IV3DPipeline* m_pComputePipeline;
	IV3DDescriptorSet* m_pComputeDedscriptorSet;

	IV3DRenderPass* m_pRenderPass;

	IV3DImage* m_pDepthStencilImage;
	IV3DImageView* m_pDepthStencilImageView;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pGraphicsPipeline;

	float m_PointSize;
};

class ComputeParticleApp : public Application
{
public:
	virtual ~ComputeParticleApp() {}

	virtual void OnPreCreate(ApplicationDesc& desc) override
	{
		desc.fps = 0;
	}

	virtual bool OnInitialize() override
	{
		if (m_Window.Initialize() == false)
		{
			return false;
		}

		return true;
	}

private:
	ComputeParticleWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	ComputeParticleApp app;
	return app.Execute();
}
