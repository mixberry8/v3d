#include "Application.h"
#include "Window.h"

#define WORK_QUEUE_INDEX 0
#define GRAPHICS_QUEUE_INDEX 1

class Simple2Window : public Window
{
public:
	Simple2Window() :
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pDescriptorSetLayout(nullptr),
		m_pPipelineLayout(nullptr),
		m_pVertexBuffer(nullptr),
		m_pImageView(nullptr),
		m_pSampler(nullptr),
		m_pDescriptorSet(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr)
	{
	}

	virtual ~Simple2Window() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// シェーダーモジュールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\texture2d.vert", srcFilePath);
			CreateFilePath(L"shader\\texture2d.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\texture2d.frag", srcFilePath);
			CreateFilePath(L"shader\\texture2d.frag.spv", dstFilePath);
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
		// デスクリプタセットレイアウトを作成
		// ----------------------------------------------------------------------------------------------------

		{
			Array1<V3DDescriptorDesc, 1> descriptors;
			descriptors[0].binding = 0; // シェーダーのバインディング ( layout(set = 0, binding = この値) uniform sampler2D imageSampler )
			descriptors[0].type = V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // イメージとサンプラーをセット ( sampler2D ) にした、V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER を使用します。
			descriptors[0].stageFlags = V3D_SHADER_STAGE_FRAGMENT; // フラグメントシェーダーから見えるようにします

			V3D_RESULT result = Application::GetDevice()->CreateDescriptorSetLayout(TO_UI32(descriptors.size()), descriptors.data(), 1, 1, &m_pDescriptorSetLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// パイプラインレイアウトを作成
		// ----------------------------------------------------------------------------------------------------

		{
			Array1<V3DConstantDesc, 1> constants =
			{
				{
					{ V3D_SHADER_STAGE_VERTEX, 0, sizeof(Matrix4x4) }
				}
			};

			Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts = { m_pDescriptorSetLayout };

			V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(TO_UI32(constants.size()), constants.data(), TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(), &m_pPipelineLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// バーテックスバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		const Simple2Window::Vertex vertices[] =
		{
			{ Vector4(-256.0f, -256.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), },
			{ Vector4(-256.0f, +256.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f), },
			{ Vector4(+256.0f, +256.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), },

			{ Vector4(+256.0f, +256.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f), },
			{ Vector4(+256.0f, -256.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f), },
			{ Vector4(-256.0f, -256.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f), },
		};

		V3DBufferDesc vertexBufferDesc{};
		vertexBufferDesc.usageFlags = V3D_BUFFER_USAGE_VERTEX;
		vertexBufferDesc.size = sizeof(vertices);

		V3D_RESULT result = Application::GetDevice()->CreateBuffer(vertexBufferDesc, &m_pVertexBuffer);
		if (result != V3D_OK)
		{
			return false;
		}

		Array1<IV3DResource*, 1> resources = { m_pVertexBuffer };

		V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
		if (Application::GetDevice()->CheckResourceMemoryProperty(memoryPropertyFlags, TO_UI32(resources.size()), resources.data()) != V3D_OK)
		{
			memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
		}

		result = Application::GetDevice()->AllocateResourceMemoryAndBind(memoryPropertyFlags, TO_UI32(resources.size()), resources.data());
		if (result != V3D_OK)
		{
			return false;
		}

		// マップして頂点情報を書き込みます
		void* pMemory;
		result = m_pVertexBuffer->Map(0, sizeof(vertices), &pMemory);
		if (result == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(vertices), vertices, sizeof(vertices));

			result = m_pVertexBuffer->Unmap();
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring filePath;
			CreateFilePath(L"image\\texture2d.bmp", filePath);

			result = CreateImageFromFile(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), filePath.c_str(), false, V3D_PIPELINE_STAGE_FRAGMENT_SHADER, &m_pImageView);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// サンプラーを作成
		// ----------------------------------------------------------------------------------------------------

		{
			V3DSamplerDesc samplerDesc{};
			samplerDesc.magFilter = V3D_FILTER_NEAREST; // 最近点フィルタリング
			samplerDesc.minFilter = V3D_FILTER_NEAREST;
			samplerDesc.mipmapMode = V3D_MIPMAP_MODE_NEAREST;
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_REPEAT; // 範囲外に出たらイメージを繰り返して描画
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_ALWAYS;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 0.0f;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK; // アドレスモードが V3D_ADDRESS_MODE_CLAMP_TO_BORDER のときに有効です。

			result = Application::GetDevice()->CreateSampler(samplerDesc, &m_pSampler);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// デスクリプタセットを作成
		// ----------------------------------------------------------------------------------------------------

		{
			result = Application::GetDevice()->CreateDescriptorSet(m_pDescriptorSetLayout, &m_pDescriptorSet);
			if (result != V3D_OK)
			{
				return false;
			}

			m_pDescriptorSet->SetImageViewAndSampler(0, m_pImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSampler);
			m_pDescriptorSet->Update();
		}

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

		SAFE_RELEASE(m_pDescriptorSet);
		SAFE_RELEASE(m_pSampler);
		SAFE_RELEASE(m_pImageView);
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pPipelineLayout);
		SAFE_RELEASE(m_pDescriptorSetLayout);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);
	}

	virtual bool OnIdle() override
	{
		// ----------------------------------------------------------------------------------------------------
		// 前処理
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		Matrix4x4 matrix;
		matrix.x.x = 2.0f / TO_F32(swapChainDesc.imageWidth);
		matrix.y.y = 2.0f / TO_F32(swapChainDesc.imageHeight);
		matrix.z.x = -1.0f;
		matrix.z.y = -1.0f;

		// ----------------------------------------------------------------------------------------------------
		// コマンドを書き込んで実行
		// ----------------------------------------------------------------------------------------------------

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		pCommandBufer->BeginRenderPass(m_pRenderPass, pFrameBuffer, true);

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

		pCommandBufer->PushConstant(m_pPipelineLayout, 0, &matrix);

		pCommandBufer->BindPipeline(m_pPipeline);
		pCommandBufer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, m_pDescriptorSet);

		pCommandBufer->BindVertexBuffer(0, m_pVertexBuffer);

		pCommandBufer->Draw(6, 1, 0, 0);

		pCommandBufer->EndRenderPass();

		if (EndGraphics() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual void OnLostSwapChain() override
	{
		SAFE_RELEASE(m_pPipeline);

		if (m_FrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_FrameBuffers[i]);
			}
			m_FrameBuffers.clear();
		}

		SAFE_RELEASE(m_pRenderPass);
	}

	virtual bool OnRestoreSwapChain() override
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		// ----------------------------------------------------------------------------------------------------
		// レンダーパスを作成
		// ----------------------------------------------------------------------------------------------------

		// アタッチメント
		std::array<V3DAttachmentDesc, 1> attachments;
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

		// サブパス 0
		std::array<V3DAttachmentReference, 1> colorAttachments;
		colorAttachments[0].attachment = 0;
		colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		std::array<V3DSubpassDesc, 1> subpasses;
		subpasses[0].inputAttachmentCount = 0;
		subpasses[0].pInputAttachments = nullptr;
		subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpasses[0].pColorAttachments = colorAttachments.data();
		subpasses[0].pResolveAttachments = nullptr;
		subpasses[0].pDepthStencilAttachment = nullptr;
		subpasses[0].preserveAttachmentCount = 0;
		subpasses[0].pPreserveAttachments = nullptr;

		// サブパスの依存性
		std::array<V3DSubpassDependencyDesc, 2> subpassDependencies;

		subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dependencyFlags = 0;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
		subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[1].dstAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[1].dependencyFlags = 0;

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

			Array1<IV3DImageView*, 1> attachments = { pImageView };
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

		Array1<V3DPipelineVertexElement, 2> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32A32_SFLOAT },
				{ 1, 16, V3D_FORMAT_R32G32_SFLOAT }
			}
		};

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{
			{ 0, sizeof(Simple2Window::Vertex), 0, TO_UI32(vertexElements.size()) },
		};

		// アルファブレンディング
		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments;
		colorBlendAttachments[0].blendEnable = true;
		colorBlendAttachments[0].srcColorFactor = V3D_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachments[0].dstColorFactor = V3D_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachments[0].colorOp = V3D_BLEND_OP_ADD;
		colorBlendAttachments[0].srcAlphaFactor = V3D_BLEND_FACTOR_ONE;
		colorBlendAttachments[0].dstAlphaFactor = V3D_BLEND_FACTOR_ZERO;
		colorBlendAttachments[0].alphaOp = V3D_BLEND_OP_ADD;
		colorBlendAttachments[0].writeMask = V3D_COLOR_COMPONENT_ALL;

		V3DGraphicsPipelineDesc pipelineDesc{};
		pipelineDesc.vertexShader.pModule = m_pVertexShaderModule;
		pipelineDesc.vertexShader.pEntryPointName = "main";
		pipelineDesc.fragmentShader.pModule = m_pFragmentShaderModule;
		pipelineDesc.fragmentShader.pEntryPointName = "main";
		pipelineDesc.vertexInput.elementCount = TO_UI32(vertexElements.size());
		pipelineDesc.vertexInput.pElements = vertexElements.data();
		pipelineDesc.vertexInput.layoutCount = TO_UI32(vertexLayouts.size());
		pipelineDesc.vertexInput.pLayouts = vertexLayouts.data();
		pipelineDesc.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineDesc.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipelineDesc.rasterization.cullMode = V3D_CULL_MODE_BACK;
		pipelineDesc.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipelineDesc.colorBlend.attachmentCount = TO_UI32(colorBlendAttachments.size());
		pipelineDesc.colorBlend.pAttachments = colorBlendAttachments.data();
		pipelineDesc.pRenderPass = m_pRenderPass;
		pipelineDesc.subpass = 0;

		result = Application::GetDevice()->CreateGraphicsPipeline(m_pPipelineLayout, pipelineDesc, &m_pPipeline);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

private:
	struct Vertex
	{
		Vector4 pos;
		Vector2 uv;
	};

	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;
	IV3DDescriptorSetLayout* m_pDescriptorSetLayout;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DBuffer* m_pVertexBuffer;
	IV3DImageView* m_pImageView;
	IV3DSampler* m_pSampler;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DRenderPass* m_pRenderPass;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;
};

class Texture2dApp : public Application
{
public:
	virtual ~Texture2dApp() {}

	virtual bool OnInitialize() override
	{
		uint32_t queueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 2);
		if (queueFamily == ~0U)
		{
			return false;
		}

		IV3DQueue* pWorkQueue;
		Application::GetDevice()->GetQueue(queueFamily, WORK_QUEUE_INDEX, &pWorkQueue);

		IV3DQueue* pGraphicsQueue;
		Application::GetDevice()->GetQueue(queueFamily, GRAPHICS_QUEUE_INDEX, &pGraphicsQueue);

		if (m_Window.Initialize(L"texture2d", 1024, 768, true, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);

		return true;
	}

private:
	Simple2Window m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	Texture2dApp app;
	return app.Execute();
}
