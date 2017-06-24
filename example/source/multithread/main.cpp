#include "Application.h"
#include "ViewCameraWindow.h"
#include "ResourceHeap.h"
#include "ParallelManager.h"

#define MESH_SIDE_COUNT 40
#define ENABLE_MULTITREAD

class MultiThreadWindow : public ViewCameraWindow
{
public:
	MultiThreadWindow() :
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pDescriptorSetLayout(nullptr),
		m_pPipelineLayout(nullptr),
		m_pImageView(nullptr),
		m_pSampler(nullptr),
		m_pDepthStencilImage(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr),
		m_ParallelData({})
	{
	}

	virtual ~MultiThreadWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		uint32_t meshCount = MESH_SIDE_COUNT * MESH_SIDE_COUNT * MESH_SIDE_COUNT;

		// ----------------------------------------------------------------------------------------------------
		// 並列マネージャーを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_ParallelManager.Initialize() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// フォントを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Initialize(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), L"Arial", 16, 400) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// シェーダーモジュールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\multithread.vert", srcFilePath);
			CreateFilePath(L"shader\\multithread.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\multithread.frag", srcFilePath);
			CreateFilePath(L"shader\\multithread.frag.spv", dstFilePath);
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
			Array1<V3DDescriptorDesc, 2> descriptors =
			{
				{
					{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, V3D_SHADER_STAGE_VERTEX },
					{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				}
			};

			V3D_RESULT result = Application::GetDevice()->CreateDescriptorSetLayout(TO_UI32(descriptors.size()), descriptors.data(), meshCount, meshCount, &m_pDescriptorSetLayout);
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
					{ V3D_SHADER_STAGE_FRAGMENT, 0, sizeof(MultiThreadWindow::SceneConstant) },
				}
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
		// イメージを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			CreateFilePath(L"image\\multithread.bmp", srcFilePath);

			V3D_RESULT result = CreateImageFromFile(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), srcFilePath.c_str(), true, V3D_PIPELINE_STAGE_FRAGMENT_SHADER, &m_pImageView);
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
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_ALWAYS;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = static_cast<float>(pImage->GetDesc().levelCount);
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			V3D_RESULT result = Application::GetDevice()->CreateSampler(samplerDesc, &m_pSampler);
			if (result != V3D_OK)
			{
				pImage->Release();
				return false;
			}

			pImage->Release();
		}

		// ----------------------------------------------------------------------------------------------------
		// メッシュを作成
		// ----------------------------------------------------------------------------------------------------

		const float meshInterval = 1.5f;
		const float meshBasePos = -(meshInterval * MESH_SIDE_COUNT) / 2.0f;

		// バッファー
		V3D_RESULT result = CreatePrefab(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), PREFAB_TYPE_CUBE, &m_ParallelData.pMeshBuffer, &m_ParallelData.meshDrawDesc);
		if (result != V3D_OK)
		{
			return false;
		}

		m_Meshes.reserve(meshCount);

		for (int32_t x = 0; x < MESH_SIDE_COUNT; x++)
		{
			float wx = meshBasePos + meshInterval * TO_F32(x);

			for (int32_t y = 0; y < MESH_SIDE_COUNT; y++)
			{
				float wy = meshBasePos + meshInterval * TO_F32(y);

				for (int32_t z = 0; z < MESH_SIDE_COUNT; z++)
				{
					float wz = meshBasePos + meshInterval * TO_F32(z);

					MultiThreadWindow::Mesh mesh{};

					// ワールド行列
					mesh.uniform.worldMatrix.SetTranslation(Vector3(wx, wy, wz));

					m_Meshes.push_back(mesh);
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// 並列処理用のデータを初期化
		// ----------------------------------------------------------------------------------------------------

		m_ParallelData.pPipelineLayout = m_pPipelineLayout;
		m_ParallelData.pMeshes = m_Meshes.data();

		// ----------------------------------------------------------------------------------------------------
		// スワップチェインに関係するインターフェースを作成
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		SetCameraRotation(Vector3(1.0f, 1.0f, 0.0f).ToNormalize(), DEG_TO_RAD(40.0f));
		SetCameraDistance(100.0f);

		return true;
	}

	virtual void OnFinalize() override
	{
		m_ParallelManager.Finalize();

		OnLostSwapChain();

		if (m_Meshes.empty() == false)
		{
			m_Meshes.clear();
		}

		SAFE_RELEASE(m_ParallelData.pMeshBuffer);

		if (m_ParallelData.descriptorSets.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.descriptorSets.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.descriptorSets[i]);
			}
			m_ParallelData.descriptorSets.clear();
		}

		if (m_ParallelData.uniformBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.uniformBuffers.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.uniformBuffers[i]);
			}
			m_ParallelData.uniformBuffers.clear();
		}

		if (m_ParallelData.uniformMemories.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.uniformMemories.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.uniformMemories[i]);
			}
			m_ParallelData.uniformMemories.clear();
		}

		SAFE_RELEASE(m_pSampler);
		SAFE_RELEASE(m_pImageView);
		SAFE_RELEASE(m_pPipelineLayout);
		SAFE_RELEASE(m_pDescriptorSetLayout);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);
	}

	virtual bool OnIdle() override
	{
		std::vector<MultiThreadWindow::Mesh>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<MultiThreadWindow::Mesh>::iterator it_mesh_end = m_Meshes.end();
		std::vector<MultiThreadWindow::Mesh>::iterator it_mesh;

		// ----------------------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------------------

		m_Font.Reset();

		wchar_t text[128];

		swprintf_s(text, L"Fps %f", Application::GetFps());
		m_Font.AddText(16, 16, text);

		if (m_Font.Bake() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// シーンを更新
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();

		ViewCameraWindow::OnIdle();

		m_ParallelData.viewProjMatrix = GetCamera()->GetViewProjectionMatrix();
		m_ParallelData.sceneConstant.lightDirection = GetCamera()->GetEye().ToNormalize();
		m_ParallelData.frame = currentImageIndex;
		m_ParallelData.pFrameBuffer = m_FrameBuffers[currentImageIndex];

		// ----------------------------------------------------------------------------------------------------
		// メッシュの処理
		// ----------------------------------------------------------------------------------------------------

		m_ParallelManager.Reset();

#ifdef ENABLE_MULTITREAD

		// 更新
		m_ParallelData.uniformMemories[currentImageIndex]->BeginMap();
		m_ParallelManager.Execute(MultiThreadWindow::ParallelUpdate, TO_UI32(m_Meshes.size()), &m_ParallelData);
		m_ParallelData.uniformMemories[currentImageIndex]->EndMap();

		// セカンダリコマンドバッファーに描画コマンドを記録
		m_ParallelManager.Execute(MultiThreadWindow::ParallelDraw, TO_UI32(m_Meshes.size()), &m_ParallelData);

#else //ENABLE_MULTITREAD

		const Matrix4x4& viewProjMatrix = GetCamera()->GetViewProjectionMatrix();
		uint8_t* pMemory;

		V3D_RESULT result = m_ParallelData.uniformBuffers[m_ParallelData.frame]->Map(0, sizeof(MultiThreadWindow::MeshUniform), reinterpret_cast<void**>(&pMemory));
		if (result == V3D_OK)
		{
			for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
			{
				MultiThreadWindow::Mesh& mesh = (*it_mesh);

				mesh.uniform.worldViewProjectionMatrix = viewProjMatrix * mesh.uniform.worldMatrix;

				memcpy_s(pMemory, sizeof(MultiThreadWindow::MeshUniform), &mesh.uniform, sizeof(MultiThreadWindow::MeshUniform));

				pMemory += m_ParallelData.uniformStride;
			}

			result = m_ParallelData.uniformBuffers[m_ParallelData.frame]->Unmap();
		}

		if (result != V3D_OK)
		{
			return false;
		}

#endif //ENABLE_MULTITREAD

		// ----------------------------------------------------------------------------------------------------
		// 描画
		// ----------------------------------------------------------------------------------------------------

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		// サブパス 0
#ifdef ENABLE_MULTITREAD

		// レンダーパスを開始
		// 第三引数はサブパスのコマンドがセカンダリコマンドバッファーに記録されるので false を指定します。
		pCommandBufer->BeginRenderPass(m_pRenderPass, pFrameBuffer, false);

		pCommandBufer->ExecuteCommandBuffers(m_ParallelManager.GetThreadCount(), m_ParallelData.commandBuffers[m_ParallelData.frame].data());

#else //ENABLE_MULTITREAD

		// レンダーパスを開始
		// 第三引数はサブパスのコマンドがプライマリコマンドバッファーに記録されるので true を指定します。
		pCommandBufer->BeginRenderPass(m_pRenderPass, pFrameBuffer, true);

		const DrawDesc& drawDesc = m_ParallelData.meshDrawDesc;
		IV3DDescriptorSet* pDescriptorSet = m_ParallelData.descriptorSets[m_ParallelData.frame];
		uint32_t uniformDynamicOffset = 0;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			MultiThreadWindow::Mesh& mesh = (*it_mesh);

			pCommandBufer->PushConstant(m_pPipelineLayout, 0, &m_ParallelData.sceneConstant);

			pCommandBufer->BindPipeline(m_pPipeline);

			pCommandBufer->SetViewport(0, 1, &m_ParallelData.viewport);
			pCommandBufer->SetScissor(0, 1, &m_ParallelData.scissor);

			pCommandBufer->BindVertexBuffers(0, 1, &m_ParallelData.pMeshBuffer, &m_ParallelData.meshDrawDesc.vertexOffset);
			pCommandBufer->BindIndexBuffer(m_ParallelData.pMeshBuffer, m_ParallelData.meshDrawDesc.indexOffset, V3D_INDEX_TYPE_UINT16);

			pCommandBufer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &pDescriptorSet, 1, &uniformDynamicOffset);
			pCommandBufer->DrawIndexed(drawDesc.indexCount, drawDesc.instanceCount, 0, 0, 0);

			uniformDynamicOffset += m_ParallelData.uniformStride;
		}

#endif //ENABLE_MULTITREAD

		// サブパス 1
		pCommandBufer->NextSubpass();
		m_Font.Flush(pCommandBufer);

		// レンダーパスを終了
		pCommandBufer->EndRenderPass();

		if (EndGraphics() == false)
		{
			return false;
		}

		return true;
	}

	virtual void OnLostSwapChain() override
	{
		if (m_ParallelData.commandBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.commandBuffers.size(); i++)
			{
				for (size_t j = 0; j < m_ParallelData.commandBuffers[i].size(); j++)
				{
					SAFE_RELEASE(m_ParallelData.commandBuffers[i][j]);
				}

				m_ParallelData.commandBuffers[i].clear();
			}

			m_ParallelData.commandBuffers.clear();
		}

		if (m_ParallelData.descriptorSets.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.descriptorSets.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.descriptorSets[i]);
			}
			m_ParallelData.descriptorSets.clear();
		}

		if (m_ParallelData.uniformBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.uniformBuffers.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.uniformBuffers[i]);
			}
			m_ParallelData.uniformBuffers.clear();
		}

		if (m_ParallelData.uniformMemories.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.uniformMemories.size(); i++)
			{
				SAFE_RELEASE(m_ParallelData.uniformMemories[i]);
			}
			m_ParallelData.uniformMemories.clear();
		}

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

		if (m_DepthStencilImageViews.empty() == false)
		{
			for (size_t i = 0; i < m_DepthStencilImageViews.size(); i++)
			{
				SAFE_RELEASE(m_DepthStencilImageViews[i]);
			}
			m_DepthStencilImageViews.clear();
		}

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
			imageDesc.layerCount = swapChainDesc.imageCount;
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

		m_DepthStencilImageViews.resize(swapChainDesc.imageCount);

		{
			V3DImageViewDesc imageViewDesc{};
			imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
			imageViewDesc.baseLevel = 0;
			imageViewDesc.levelCount = 1;
			imageViewDesc.layerCount = 1;

			for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
			{
				imageViewDesc.baseLayer = i;

				V3D_RESULT result = Application::GetDevice()->CreateImageView(m_pDepthStencilImage, imageViewDesc, &m_DepthStencilImageViews[i]);
				if (result != V3D_OK)
				{
					return false;
				}
			}

			IV3DCommandBuffer* pCommandBuffer = BeginWork();
			if (pCommandBuffer == nullptr)
			{
				return false;
			}

			for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
			{
				V3DBarrierImageViewDesc barrier{};
				barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
				barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
				barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
				barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
				barrier.dstLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

				pCommandBuffer->BarrierImageView(m_DepthStencilImageViews[i], barrier);
			}

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
		std::array<V3DAttachmentReference, 1> colorAttachments;
		colorAttachments[0].attachment = 0;
		colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// デプスステンシル
		V3DAttachmentReference depthStencilAttachment;
		depthStencilAttachment.attachment = 1;
		depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		subpasses[0].inputAttachmentCount = 0;
		subpasses[0].pInputAttachments = nullptr;
		subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpasses[0].pColorAttachments = colorAttachments.data();
		subpasses[0].pResolveAttachments = nullptr;
		subpasses[0].pDepthStencilAttachment = &depthStencilAttachment;
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

		Array1<V3DSubpassDependencyDesc, 3> subpassDependencies;

		subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

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
		subpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		V3D_RESULT result = Application::GetDevice()->CreateRenderPass(
			TO_UI32(attachments.size()), attachments.data(),
			TO_UI32(subpasses.size()), subpasses.data(),
			TO_UI32(subpassDependencies.size()), subpassDependencies.data(),
			&m_pRenderPass);

		if (result != V3D_OK)
		{
			return false;
		}

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
				m_DepthStencilImageViews[i],
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

		Array1<V3DPipelineVertexElement, 3> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32_SFLOAT }, // position
				{ 1, 12, V3D_FORMAT_R32G32B32_SFLOAT }, // normal
				{ 2, 24, V3D_FORMAT_R32G32_SFLOAT },    // uv
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

		m_ParallelData.pRenderPass = m_pRenderPass;
		m_ParallelData.pPipeline = m_pPipeline;

		// ----------------------------------------------------------------------------------------------------
		// フォント
		// ----------------------------------------------------------------------------------------------------

		m_Font.Restore(m_pRenderPass, 1, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

		// ----------------------------------------------------------------------------------------------------
		// ユニフォーム
		// ----------------------------------------------------------------------------------------------------

		uint32_t meshCount = MESH_SIDE_COUNT * MESH_SIDE_COUNT * MESH_SIDE_COUNT;

		BufferSubresourceDesc uniformBufferSubresource;
		uniformBufferSubresource.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
		uniformBufferSubresource.size = sizeof(MultiThreadWindow::MeshUniform);
		uniformBufferSubresource.count = meshCount;

		BufferMemoryLayout uniformMemoryLayout;
		uint64_t uniformMemorySize;

		CalcBufferMemoryLayout(Application::GetDevice(), V3D_MEMORY_PROPERTY_HOST_VISIBLE, 1, &uniformBufferSubresource, &uniformMemoryLayout, &uniformMemorySize);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			// バッファ
			V3DBufferDesc uniformBufferDesc{};
			uniformBufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
			uniformBufferDesc.size = uniformMemorySize;

			IV3DBuffer* pUniformBuffer;
			result = Application::GetDevice()->CreateBuffer(uniformBufferDesc, &pUniformBuffer);
			if (result != V3D_OK)
			{
				return false;
			}

			m_ParallelData.uniformBuffers.push_back(pUniformBuffer);

			// メモリ
			result = Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pUniformBuffer);
			if (result != V3D_OK)
			{
				return false;
			}

			IV3DResourceMemory* pUniformMemory;
			pUniformBuffer->GetResourceMemory(&pUniformMemory);
			m_ParallelData.uniformMemories.push_back(pUniformMemory);

			// デスクリプタセット
			IV3DDescriptorSet* pDescriptorSet;
			result = Application::GetDevice()->CreateDescriptorSet(m_pDescriptorSetLayout, &pDescriptorSet);
			if (result != V3D_OK)
			{
				return false;
			}

			m_ParallelData.descriptorSets.push_back(pDescriptorSet);

			result = pDescriptorSet->SetBuffer(0, pUniformBuffer, 0, uniformBufferSubresource.size);
			if (result != V3D_OK)
			{
				return false;
			}

			result = pDescriptorSet->SetImageViewAndSampler(1, m_pImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSampler);
			if (result != V3D_OK)
			{
				return false;
			}

			pDescriptorSet->Update();

		}

		// ----------------------------------------------------------------------------------------------------
		// 並列データの設定
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_ParallelData.uniformStride = TO_UI32(uniformMemoryLayout.stride);

		m_ParallelData.commandBuffers.resize(swapChainDesc.imageCount);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			V3DCommandPoolDesc commandPoolDesc{};
			commandPoolDesc.usageFlags = V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER;
			commandPoolDesc.queueFamily = GetGraphicsQueueFamily();

			m_ParallelData.commandBuffers[i].reserve(threadCount);

			for (uint32_t j = 0; j < threadCount; j++)
			{
				IV3DCommandBuffer* pCommandBuffer;
				result = Application::GetDevice()->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_ParallelData.commandBuffers[i].push_back(pCommandBuffer);
			}

			m_ParallelData.viewport.rect.x = 0;
			m_ParallelData.viewport.rect.y = 0;
			m_ParallelData.viewport.rect.width = swapChainDesc.imageWidth;
			m_ParallelData.viewport.rect.height = swapChainDesc.imageHeight;
			m_ParallelData.viewport.minDepth = 0.0f;
			m_ParallelData.viewport.maxDepth = 1.0f;

			m_ParallelData.scissor.x = 0;
			m_ParallelData.scissor.y = 0;
			m_ParallelData.scissor.width = swapChainDesc.imageWidth;
			m_ParallelData.scissor.height = swapChainDesc.imageHeight;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) override
	{
		return true;
	}

	static void __stdcall ParallelUpdate(uint32_t thread, uint32_t page, uint32_t first, uint32_t count, void* pData)
	{
		MultiThreadWindow::ParallelData* pParallel = static_cast<MultiThreadWindow::ParallelData*>(pData);

		uint64_t memoryOffset = pParallel->uniformStride * first;
		uint64_t memorySize = pParallel->uniformStride * count;
		uint8_t* pMemory;

		V3D_RESULT result = pParallel->uniformBuffers[pParallel->frame]->Map(memoryOffset, memorySize, reinterpret_cast<void**>(&pMemory));
		if (result == V3D_OK)
		{
			MultiThreadWindow::Mesh* pMesh = &pParallel->pMeshes[first];
			MultiThreadWindow::Mesh* pMeshEnd = pMesh + count;

			uint32_t uniformStride = pParallel->uniformStride;
			uint32_t dynamicOffset = 0;

			while (pMesh != pMeshEnd)
			{
				pMesh->uniform.worldViewProjectionMatrix = pParallel->viewProjMatrix * pMesh->uniform.worldMatrix;

				memcpy_s(pMemory, sizeof(MultiThreadWindow::MeshUniform), &pMesh->uniform, sizeof(MultiThreadWindow::MeshUniform));

				pMemory += uniformStride;
				pMesh++;
			}

			result = pParallel->uniformBuffers[pParallel->frame]->Unmap();
		}
	}

	static void __stdcall ParallelDraw(uint32_t thread, uint32_t page, uint32_t first, uint32_t count, void* pData)
	{
		MultiThreadWindow::ParallelData* pParallel = static_cast<MultiThreadWindow::ParallelData*>(pData);
		const DrawDesc& meshDrawDesc = pParallel->meshDrawDesc;
		IV3DDescriptorSet* pDescriptorSet = pParallel->descriptorSets[pParallel->frame];

		IV3DCommandBuffer* pCommandBuffer = pParallel->commandBuffers[pParallel->frame][thread];

		MultiThreadWindow::Mesh* pMesh = &pParallel->pMeshes[first];
		MultiThreadWindow::Mesh* pMeshEnd = pMesh + count;

		uint32_t uniformStride = pParallel->uniformStride;
		uint32_t dynamicOffset = uniformStride * first;

		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallel->pRenderPass, 0, pParallel->pFrameBuffer);

		while (pMesh != pMeshEnd)
		{
			pCommandBuffer->PushConstant(pParallel->pPipelineLayout, 0, &pParallel->sceneConstant);

			pCommandBuffer->BindPipeline(pParallel->pPipeline);

			pCommandBuffer->SetViewport(0, 1, &pParallel->viewport);
			pCommandBuffer->SetScissor(0, 1, &pParallel->scissor);

			pCommandBuffer->BindVertexBuffers(0, 1, &pParallel->pMeshBuffer, &meshDrawDesc.vertexOffset);
			pCommandBuffer->BindIndexBuffer(pParallel->pMeshBuffer, meshDrawDesc.indexOffset, V3D_INDEX_TYPE_UINT16);

			pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, pParallel->pPipelineLayout, 0, 1, &pDescriptorSet, 1, &dynamicOffset);
			pCommandBuffer->DrawIndexed(meshDrawDesc.indexCount, meshDrawDesc.instanceCount, 0, 0, 0);

			dynamicOffset += uniformStride;
			pMesh++;
		}

		pCommandBuffer->End();
	}

private:
	struct SceneConstant
	{
		Vector3 lightDirection;
		float reserve[1];
	};

	struct MeshUniform
	{
		Matrix4x4 worldMatrix;
		Matrix4x4 worldViewProjectionMatrix;
	};

	struct Mesh
	{
		MeshUniform uniform;
	};

	struct ParallelData
	{
		uint32_t frame;

		std::vector<std::vector<IV3DCommandBuffer*>> commandBuffers;

		Matrix4x4 viewProjMatrix;
		SceneConstant sceneConstant;

		IV3DRenderPass* pRenderPass;
		IV3DFrameBuffer* pFrameBuffer;

		IV3DPipelineLayout* pPipelineLayout;
		IV3DPipeline* pPipeline;

		V3DViewport viewport;
		V3DRectangle2D scissor;

		uint32_t uniformStride;
		std::vector<IV3DResourceMemory*> uniformMemories;
		std::vector<IV3DBuffer*> uniformBuffers;
		std::vector<IV3DDescriptorSet*> descriptorSets;

		IV3DBuffer* pMeshBuffer;
		DrawDesc meshDrawDesc;

		MultiThreadWindow::Mesh* pMeshes;
	};

	ParallelManager m_ParallelManager;
	MultiThreadWindow::ParallelData m_ParallelData;

	Font m_Font;

	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;
	IV3DDescriptorSetLayout* m_pDescriptorSetLayout;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DImageView* m_pImageView;
	IV3DSampler* m_pSampler;

	std::vector<MultiThreadWindow::Mesh> m_Meshes;

	IV3DRenderPass* m_pRenderPass;
	IV3DImage* m_pDepthStencilImage;
	std::vector<IV3DImageView*> m_DepthStencilImageViews;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;
};

class MultiThreadApp : public Application
{
public:
	virtual ~MultiThreadApp() {}

	virtual void OnPreCreate(ApplicationDesc& desc)
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
		Application::GetDevice()->GetQueue(queueFamily, 0, &pWorkQueue);

		IV3DQueue* pGraphicsQueue;
		Application::GetDevice()->GetQueue(queueFamily, 1, &pGraphicsQueue);

		if (m_Window.Initialize(L"multithread", 1024, 768, false, WINDOW_BUFFERING_TYPE_REAL, pWorkQueue, pGraphicsQueue) == false)
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
	MultiThreadWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	MultiThreadApp app;
	return app.Execute();
}
