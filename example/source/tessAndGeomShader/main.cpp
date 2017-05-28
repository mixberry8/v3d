#include "Application.h"
#include "ViewCameraWindow.h"
#include "Font.h"

#define WORK_QUEUE_INDEX 0
#define GRAPHICS_QUEUE_INDEX 1

class TessellationShaderWindow : public ViewCameraWindow
{
public:
	TessellationShaderWindow() :
		m_pVertShaderModule(nullptr),
		m_pTescShaderModule(nullptr),
		m_pTeseShaderModule(nullptr),
		m_pGeomShaderModule(nullptr),
		m_pFragShaderModule(nullptr),
		m_pDescriptorSetLayout(nullptr),
		m_pPipelineLayout(nullptr),
		m_pMeshBuffer(nullptr),
		m_VertexOffset(0),
		m_IndexOffset(0),
		m_pImageView(nullptr),
		m_pSampler(nullptr),
		m_pDescriptorSet(nullptr),
		m_pDepthStencilImage(nullptr),
		m_pDepthStencilImageView(nullptr),
		m_pRenderPass(nullptr),
		m_IndexCount(0),
		m_PipelineType(TessellationShaderWindow::PIPELINE_TYPE_SOLID),
		m_Height(3.0f),
		m_Level(3)
	{
	}

	virtual ~TessellationShaderWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// フォントを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Initialize(Application::GetDevice(), L"Arial", 16, 400) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// シェーダーモジュールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			// vert
			CreateFilePath(L"shader\\source\\tessAndGeomShader.vert", srcFilePath);
			CreateFilePath(L"shader\\tessAndGeomShader.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// tesc
			CreateFilePath(L"shader\\source\\tessAndGeomShader.tesc", srcFilePath);
			CreateFilePath(L"shader\\tessAndGeomShader.tesc.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pTescShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// tese
			CreateFilePath(L"shader\\source\\tessAndGeomShader.tese", srcFilePath);
			CreateFilePath(L"shader\\tessAndGeomShader.tese.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pTeseShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// geom
			CreateFilePath(L"shader\\source\\tessAndGeomShader.geom", srcFilePath);
			CreateFilePath(L"shader\\tessAndGeomShader.geom.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pGeomShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			// frag
			CreateFilePath(L"shader\\source\\tessAndGeomShader.frag", srcFilePath);
			CreateFilePath(L"shader\\tessAndGeomShader.frag.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pFragShaderModule);
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
				{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_GEOMETRY }
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
			Array1<V3DConstantDesc, 3> constants =
			{
				{
					{ V3D_SHADER_STAGE_TESSELLATION_CONTROL,   0, sizeof(TessellationShaderWindow::TescConstant) },
					{ V3D_SHADER_STAGE_GEOMETRY,              16, sizeof(TessellationShaderWindow::GeomConstant) },
					{ V3D_SHADER_STAGE_FRAGMENT,             160, sizeof(TessellationShaderWindow::FragConstant) },
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
		// バーテックスバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		{
			uint16_t planeDivCount = 8;
			uint16_t rowIndexCount = planeDivCount + 1;

			float planeWidth = 20.0f;
			float halfPlaneWidth = planeWidth * 0.5f;

			float planePosStep = planeWidth / TO_F32(planeDivCount);
			float planeUVStep = 1.0f / TO_F32(planeDivCount);

			std::vector<TessellationShaderWindow::Vertex> vertices;
			vertices.reserve(rowIndexCount * rowIndexCount);

			for (uint16_t z = 0; z < rowIndexCount; z++)
			{
				TessellationShaderWindow::Vertex vertex;

				vertex.pos.z = -halfPlaneWidth + planePosStep * TO_F32(z);
				vertex.normal.Set(0.0f, -1.0f, 0.0f);
				vertex.uv.y = planeUVStep * TO_F32(z);

				for (uint16_t x = 0; x < rowIndexCount; x++)
				{
					vertex.pos.x = -halfPlaneWidth + planePosStep * TO_F32(x);
					vertex.uv.x = planeUVStep * TO_F32(x);

					vertices.push_back(vertex);
				}
			}

			std::vector<uint16_t> indices;
			indices.reserve(planeDivCount * planeDivCount * 3);

			for (uint16_t y = 0; y < planeDivCount; y++)
			{
				uint16_t firstIndex = rowIndexCount * y;
				for (uint16_t x = 0; x < planeDivCount; x++)
				{
					uint16_t i0 = firstIndex + x;
					uint16_t i1 = i0 + rowIndexCount;
					uint16_t i2 = i1 + 1;

					indices.push_back(i0);
					indices.push_back(i1);
					indices.push_back(i2);

					uint16_t i3 = i2;
					uint16_t i4 = i2 - rowIndexCount;
					uint16_t i5 = i0;

					indices.push_back(i3);
					indices.push_back(i4);
					indices.push_back(i5);
				}
			}

			m_IndexCount = TO_UI32(indices.size());

			uint64_t vertexBufferSize = sizeof(TessellationShaderWindow::Vertex) * vertices.size();
			uint64_t indexBufferSize = sizeof(uint16_t) * indices.size();

			Array1<BufferSubresourceDesc, 2> meshSubresources;
			meshSubresources[0].usageFlags = V3D_BUFFER_USAGE_VERTEX;
			meshSubresources[0].size = vertexBufferSize;
			meshSubresources[0].count = 1;
			meshSubresources[1].usageFlags = V3D_BUFFER_USAGE_INDEX;
			meshSubresources[1].size = indexBufferSize;
			meshSubresources[1].count = 1;

			Array1<BufferMemoryLayout, 2> meshMemoryLayouts;
			uint64_t meshMemorySize;

			CalcBufferMemoryLayout(Application::GetDevice(), V3D_MEMORY_PROPERTY_HOST_VISIBLE, TO_UI32(meshSubresources.size()), meshSubresources.data(), meshMemoryLayouts.data(), &meshMemorySize);

			V3DBufferDesc meshBufferDesc{};
			meshBufferDesc.usageFlags = V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
			meshBufferDesc.size = meshMemorySize;

			V3D_RESULT result = Application::GetDevice()->CreateBuffer(meshBufferDesc, &m_pMeshBuffer);
			if (result != V3D_OK)
			{
				return false;
			}

			Array1<IV3DResource*, 1> resources = { m_pMeshBuffer };

			V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
			if (Application::GetDevice()->CheckResourceMemoryProperty(memoryPropertyFlags, TO_UI32(resources.size()), resources.data()) != V3D_OK)
			{
				memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
			}

			result = Application::GetDevice()->AllocateResourceMemoryAndBind(memoryPropertyFlags, m_pMeshBuffer);
			if (result != V3D_OK)
			{
				return false;
			}

			uint8_t* pMemory;
			result = m_pMeshBuffer->Map(0, V3D_WHOLE_SIZE, reinterpret_cast<void**>(&pMemory));
			if (result == V3D_OK)
			{
				// バーテックスを書き込む
				MemCopy(pMemory + meshMemoryLayouts[0].offset, meshMemoryLayouts[0].stride, vertices.data(), vertexBufferSize);

				// インデックスを書き込む
				MemCopy(pMemory + meshMemoryLayouts[1].offset, meshMemoryLayouts[1].stride, indices.data(), indexBufferSize);

				result = m_pMeshBuffer->Unmap();
				if (result != V3D_OK)
				{
					return false;
				}
			}

			m_VertexOffset = meshMemoryLayouts[0].offset;
			m_IndexOffset = meshMemoryLayouts[1].offset;
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを作成
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			CreateFilePath(L"image\\tessAndGeomShader.dds", srcFilePath);

			V3D_RESULT result = CreateImageFromFile(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), srcFilePath.c_str(), false, &m_pImageView);
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
			samplerDesc.maxLod = 0.0f;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			V3D_RESULT result = Application::GetDevice()->CreateSampler(samplerDesc, &m_pSampler);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// デスクリプタセットを作成
		// ----------------------------------------------------------------------------------------------------

		V3D_RESULT result = Application::GetDevice()->CreateDescriptorSet(m_pDescriptorSetLayout, &m_pDescriptorSet);
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

		SetCameraRotation(Vector3(1.0f, 1.0f, 0.0f).ToNormalize(), DEG_TO_RAD(45.0f));
		SetCameraDistance(20.0f);

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
		SAFE_RELEASE(m_pFragShaderModule);
		SAFE_RELEASE(m_pGeomShaderModule);
		SAFE_RELEASE(m_pTeseShaderModule);
		SAFE_RELEASE(m_pTescShaderModule);
		SAFE_RELEASE(m_pVertShaderModule);

		m_Font.Dispose();
	}

	virtual bool OnIdle() override
	{
		// ----------------------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------------------

		m_Font.Reset();

		wchar_t text[128];

		swprintf_s(text, L"Fps %f", Application::GetFps());
		m_Font.AddText(16, 16, text);

		wchar_t polygonMode[5];
		switch (m_PipelineType)
		{
		case 0:
			wcscpy_s(polygonMode, L"Fill");
			break;
		case 1:
			wcscpy_s(polygonMode, L"Line");
			break;
		}

		swprintf_s(text, L"A - S : Height %.1f\nZ - X : Level %u\nSpace : PolygonMode %s", m_Height, m_Level, polygonMode);
		m_Font.AddText(16, 48, text);

		if (m_Font.Bake(GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence()) == false)
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

		TessellationShaderWindow::TescConstant tescConstant{};
		tescConstant.level = TO_F32(m_Level);

		TessellationShaderWindow::GeomConstant geomConstant{};
		geomConstant.height = m_Height;
		geomConstant.worldMatrix = Matrix4x4::Identity();
		geomConstant.worldViewProjectionMatrix = GetCamera()->GetViewProjectionMatrix() * geomConstant.worldMatrix;

		TessellationShaderWindow::FragConstant fragConstant{};
		fragConstant.lightDirection = GetCamera()->GetEye().ToNormalize();

		// ----------------------------------------------------------------------------------------------------
		// 描画
		// ----------------------------------------------------------------------------------------------------

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		// レンダーパスを開始
		pCommandBufer->BeginRenderPass(m_pRenderPass, pFrameBuffer, true);

		// サブパス 0 - メッシュの描画
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

		pCommandBufer->PushConstant(m_pPipelineLayout, 0, &tescConstant);
		pCommandBufer->PushConstant(m_pPipelineLayout, 1, &geomConstant);
		pCommandBufer->PushConstant(m_pPipelineLayout, 2, &fragConstant);

		pCommandBufer->BindPipeline(m_pPipelines[m_PipelineType]);
		pCommandBufer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 0, nullptr);
		pCommandBufer->BindVertexBuffers(0, 1, &m_pMeshBuffer, &m_VertexOffset);
		pCommandBufer->BindIndexBuffer(m_pMeshBuffer, m_IndexOffset, V3D_INDEX_TYPE_UINT16);
		pCommandBufer->DrawIndexed(m_IndexCount, 1, 0, 0, 0);

		// サブパス 1 - テキストの描画
		pCommandBufer->NextSubpass();
		m_Font.Flush(pCommandBufer, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

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
		m_Font.Lost();

		for (size_t i = 0; i < m_pPipelines.size(); i++)
		{
			SAFE_RELEASE(m_pPipelines[i]);
		}

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

			V3DBarrierImageDesc barrier{};
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
			{ 0, sizeof(TessellationShaderWindow::Vertex), 0, TO_UI32(vertexElements.size()) },
		};

		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments =
		{
			{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL }
		};

		V3DGraphicsPipelineDesc pipelineDesc{};
		pipelineDesc.vertexShader.pModule = m_pVertShaderModule;
		pipelineDesc.vertexShader.pEntryPointName = "main";
		pipelineDesc.tessellationControlShader.pModule = m_pTescShaderModule;
		pipelineDesc.tessellationControlShader.pEntryPointName = "main";
		pipelineDesc.tessellationEvaluationShader.pModule = m_pTeseShaderModule;
		pipelineDesc.tessellationEvaluationShader.pEntryPointName = "main";
		pipelineDesc.geometryShader.pModule = m_pGeomShaderModule;
		pipelineDesc.geometryShader.pEntryPointName = "main";
		pipelineDesc.fragmentShader.pModule = m_pFragShaderModule;
		pipelineDesc.fragmentShader.pEntryPointName = "main";
		pipelineDesc.vertexInput.elementCount = TO_UI32(vertexElements.size());
		pipelineDesc.vertexInput.pElements = vertexElements.data();
		pipelineDesc.vertexInput.layoutCount = TO_UI32(vertexLayouts.size());
		pipelineDesc.vertexInput.pLayouts = vertexLayouts.data();
		pipelineDesc.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		pipelineDesc.tessellation.patchControlPoints = 3;
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

		// ソリッド
		result = Application::GetDevice()->CreateGraphicsPipeline(m_pPipelineLayout, pipelineDesc, &m_pPipelines[TessellationShaderWindow::PIPELINE_TYPE_SOLID]);
		if (result != V3D_OK)
		{
			return false;
		}

		// ワイヤー
		pipelineDesc.rasterization.polygonMode = V3D_POLYGON_MODE_LINE;
		result = Application::GetDevice()->CreateGraphicsPipeline(m_pPipelineLayout, pipelineDesc, &m_pPipelines[TessellationShaderWindow::PIPELINE_TYPE_WIRE]);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// フォント
		// ----------------------------------------------------------------------------------------------------

		m_Font.Restore(m_pRenderPass, 1);

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) override
	{
		if ((keyCode == VK_SPACE) && (repeatCount == 1))
		{
			m_PipelineType = (m_PipelineType + 1) % TessellationShaderWindow::PIPELINE_TYPE_COUNT;
		}

		if (keyCode == 'A')
		{
			if (m_Height > 0.0f)
			{
				m_Height -= 0.1f;
			}
		}
		else if (keyCode == 'S')
		{
			if (m_Height <= 10.0f)
			{
				m_Height += 0.1f;
			}
		}

		if (keyCode == 'Z')
		{
			if (m_Level > 1)
			{
				m_Level -= 1;
			}
		}
		else if (keyCode == 'X')
		{
			if (m_Level < Application::GetDevice()->GetCaps().maxTessellationGenerationLevel)
			{
				m_Level += 1;
			}
		}

		return true;
	}

private:
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct TescConstant
	{
		float level;
		float reserve[3];
	};

	struct GeomConstant
	{
		float height;
		float reserve[3];

		Matrix4x4 worldMatrix;
		Matrix4x4 worldViewProjectionMatrix;
	};

	struct FragConstant
	{
		Vector3 lightDirection;
		float reseve[1];
	};

	enum PIPELINE_TYPE
	{
		PIPELINE_TYPE_SOLID = 0,
		PIPELINE_TYPE_WIRE = 1,
	};

	static auto constexpr PIPELINE_TYPE_COUNT = 2;

	Font m_Font;

	IV3DShaderModule* m_pVertShaderModule;
	IV3DShaderModule* m_pTescShaderModule;
	IV3DShaderModule* m_pTeseShaderModule;
	IV3DShaderModule* m_pGeomShaderModule;
	IV3DShaderModule* m_pFragShaderModule;

	IV3DDescriptorSetLayout* m_pDescriptorSetLayout;
	IV3DPipelineLayout* m_pPipelineLayout;

	IV3DBuffer* m_pMeshBuffer;
	uint64_t m_VertexOffset;
	uint64_t m_IndexOffset;

	IV3DImageView* m_pImageView;
	IV3DSampler* m_pSampler;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DRenderPass* m_pRenderPass;
	IV3DImage* m_pDepthStencilImage;
	IV3DImageView* m_pDepthStencilImageView;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	Array1<IV3DPipeline*, TessellationShaderWindow::PIPELINE_TYPE_COUNT> m_pPipelines;

	uint32_t m_IndexCount;
	uint32_t m_PipelineType;
	float m_Height;
	uint32_t m_Level;
};

class TessellationShaderApp : public Application
{
public:
	virtual ~TessellationShaderApp() {}

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

		if (m_Window.Initialize(L"tessellationShader", 1024, 768, true, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
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
	TessellationShaderWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	TessellationShaderApp app;
	return app.Execute();
}
