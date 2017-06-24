#include "Application.h"
#include "ViewCameraWindow.h"
#include "Font.h"

#define WORK_QUEUE_INDEX 0
#define GRAPHICS_QUEUE_INDEX 1

class TextureCubeWindow : public ViewCameraWindow
{
public:
	TextureCubeWindow() :
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pDescriptorSetLayout(nullptr),
		m_pPipelineLayout(nullptr),
		m_pMeshBuffer(nullptr),
		m_MeshDrawDesc({}),
		m_pImageView(nullptr),
		m_pSampler(nullptr),
		m_pDescriptorSet(nullptr),
		m_pDepthStencilImage(nullptr),
		m_pDepthStencilImageView(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr),
		m_LodBias(0)
	{
	}

	virtual ~TextureCubeWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// フォントを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Initialize(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), L"Arial", 16, FW_NORMAL) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// シェーダーモジュールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\textureCube.vert", srcFilePath);
			CreateFilePath(L"shader\\textureCube.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\textureCube.frag", srcFilePath);
			CreateFilePath(L"shader\\textureCube.frag.spv", dstFilePath);
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
			Array1<V3DDescriptorDesc, 1> descriptors =
			{
				{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT }
			};

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
				{ V3D_SHADER_STAGE_VERTEX, 0, sizeof(TextureCubeWindow::Scene) }
			};

			Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
			{
				m_pDescriptorSetLayout
			};

			V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(TO_UI32(constants.size()), constants.data(), TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(), &m_pPipelineLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// メッシュバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		V3D_RESULT result = CreatePrefab(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), PREFAB_TYPE_SPHERE, &m_pMeshBuffer, &m_MeshDrawDesc);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			CreateFilePath(L"image\\textureCube.dds", srcFilePath);

			result = CreateImageFromFile(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), srcFilePath.c_str(), true, V3D_PIPELINE_STAGE_FRAGMENT_SHADER, &m_pImageView);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// サンプラーを作成
		// ----------------------------------------------------------------------------------------------------

		{
			IV3DImage* pImage;
			m_pImageView->GetImage(&pImage);

			V3DSamplerDesc samplerDesc{};
			samplerDesc.magFilter = V3D_FILTER_LINEAR;
			samplerDesc.minFilter = V3D_FILTER_LINEAR;
			samplerDesc.mipmapMode = V3D_MIPMAP_MODE_LINEAR;
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_ALWAYS;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = static_cast<float>(pImage->GetDesc().levelCount);
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			result = Application::GetDevice()->CreateSampler(samplerDesc, &m_pSampler);
			if (result != V3D_OK)
			{
				pImage->Release();
				return false;
			}

			pImage->Release();
		}

		// ----------------------------------------------------------------------------------------------------
		// デスクリプタセットを作成
		// ----------------------------------------------------------------------------------------------------

		result = Application::GetDevice()->CreateDescriptorSet(m_pDescriptorSetLayout, &m_pDescriptorSet);
		if (result != V3D_OK)
		{
			return false;
		}

		result = m_pDescriptorSet->SetImageViewAndSampler(0, m_pImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSampler);
		if (result != V3D_OK)
		{
			return false;
		}

		m_pDescriptorSet->Update();

		// ----------------------------------------------------------------------------------------------------
		// スワップチェインに関係するインターフェースを作成
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		SetCameraRotation(Vector3(1.0f, 1.0f, 1.0f).ToNormalize(), DEG_TO_RAD(30.0f));
		SetCameraDistance(2.0f);

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		SAFE_RELEASE(m_pDescriptorSet);
		SAFE_RELEASE(m_pSampler);
		SAFE_RELEASE(m_pImageView);
		SAFE_RELEASE(m_pMeshBuffer);
		SAFE_RELEASE(m_pPipelineLayout);
		SAFE_RELEASE(m_pDescriptorSetLayout);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);

		m_Font.Dispose();
	}

	virtual bool OnIdle() override
	{
		// ----------------------------------------------------------------------------------------------------
		// テキストを更新
		// ----------------------------------------------------------------------------------------------------

		std::wstring text;

		m_Font.Reset();

		text = L"Fps " + std::to_wstring(Application::GetFps());
		m_Font.AddText(16, 16, text.c_str());

		if (m_Font.Bake() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// カメラを更新
		// ----------------------------------------------------------------------------------------------------

		ViewCameraWindow::OnIdle();

		// ----------------------------------------------------------------------------------------------------
		// 定数を準備
		// ----------------------------------------------------------------------------------------------------

		TextureCubeWindow::Scene scene{};
		scene.lodBias = TO_F32(m_LodBias);
		scene.eyePos = GetCamera()->GetEye();
		scene.lightDirection = GetCamera()->GetEye().ToNormalize();
		scene.worldMatrix = Matrix4x4::Identity();
		scene.worldViewProjectionMatrix = GetCamera()->GetViewProjectionMatrix() * scene.worldMatrix;

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

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

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

		// サブパス 0 : メッシュの描画
		pCommandBufer->PushConstant(m_pPipelineLayout, 0, &scene);
		pCommandBufer->BindPipeline(m_pPipeline);
		pCommandBufer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 0, nullptr);
		pCommandBufer->BindVertexBuffers(0, 1, &m_pMeshBuffer, &m_MeshDrawDesc.vertexOffset);
		pCommandBufer->BindIndexBuffer(m_pMeshBuffer, m_MeshDrawDesc.indexOffset, V3D_INDEX_TYPE_UINT16);
		pCommandBufer->DrawIndexed(m_MeshDrawDesc.indexCount, m_MeshDrawDesc.instanceCount, 0, 0, 0);

		// サブパス 1 : テキストの描画
		pCommandBufer->NextSubpass();
		m_Font.Flush(pCommandBufer);

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
		m_Font.Lost();

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

			Array1<IV3DResource*, 1> resources = { m_pDepthStencilImage };
			result = Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, TO_UI32(resources.size()), resources.data());
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

			V3DBarrierImageViewDesc barrier{};
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
			pCommandBuffer->BarrierImageView(m_pDepthStencilImageView, barrier);

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
		attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].clearValue.depthStencil.depth = 1.0f;
		attachments[1].clearValue.depthStencil.stencil = 0;

		/**************/
		/* サブパス 0 */
		/**************/

		std::array<V3DSubpassDesc, 2> subpasses;

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

		/**************/
		/* サブパス 1 */
		/**************/

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

		std::array<V3DSubpassDependencyDesc, 3> subpassDependencies;

		subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = 1;
		subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[1].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[1].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		subpassDependencies[2].srcSubpass = 1;
		subpassDependencies[2].dstSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
		subpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[2].dstAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

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

		Array1<V3DPipelineVertexElement, 2> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32_SFLOAT }, // position
				{ 1, 12, V3D_FORMAT_R32G32B32_SFLOAT }, // normal
			}
		};

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{
			{ 0, sizeof(PrefabVertex), 0, TO_UI32(vertexElements.size()) },
		};

		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments =
		{
			{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL }
		};

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
		pipelineDesc.depthStencil.depthTestEnable = true;
		pipelineDesc.depthStencil.depthWriteEnable = true;
		pipelineDesc.depthStencil.depthCompareOp = V3D_COMPARE_OP_LESS;
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
		// フォントを復帰
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Restore(m_pRenderPass, 1, swapChainDesc.imageWidth, swapChainDesc.imageHeight) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) override
	{
		if (keyCode == 'Z')
		{
			if (m_LodBias > 0)
			{
				m_LodBias--;
			}
		}
		else if (keyCode == 'X')
		{
			const V3DImageViewDesc& imageViewDesc = m_pImageView->GetDesc();
			if (m_LodBias <= imageViewDesc.levelCount)
			{
				m_LodBias++;
			}
		}

		return true;
	}

private:
	struct Scene
	{
		float lodBias;
		float reserve0[3];
		Vector3 eyePos;
		float reserve1[1];
		Vector3 lightDirection;
		float reserve2[1];
		Matrix4x4 worldMatrix;
		Matrix4x4 worldViewProjectionMatrix;
	};

	Font m_Font;

	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;
	IV3DDescriptorSetLayout* m_pDescriptorSetLayout;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DBuffer* m_pMeshBuffer;
	DrawDesc m_MeshDrawDesc;
	IV3DImageView* m_pImageView;
	IV3DSampler* m_pSampler;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DRenderPass* m_pRenderPass;
	IV3DImage* m_pDepthStencilImage;
	IV3DImageView* m_pDepthStencilImageView;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;

	uint32_t m_LodBias;
};

class TextureCubeApp : public Application
{
public:
	virtual ~TextureCubeApp() {}

	virtual void OnPreCreate(ApplicationDesc& desc) override
	{
		desc.fps = 0;
	}

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

		if (m_Window.Initialize(L"textureCube", 1024, 768, false, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
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
	TextureCubeWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	TextureCubeApp app;
	return app.Execute();
}
