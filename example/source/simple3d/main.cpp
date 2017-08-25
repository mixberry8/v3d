#include "Application.h"
#include "ViewCameraWindow.h"

#define WORK_QUEUE_INDEX 0
#define GRAPHICS_QUEUE_INDEX 1

class Simple3dWindow : public ViewCameraWindow
{
public:
	Simple3dWindow() :
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pPipelineLayout(nullptr),
		m_pMeshBuffer(nullptr),
		m_VertexOffset(0),
		m_IndexOffset(0),
		m_pDepthStencilImage(nullptr),
		m_pDepthStencilImageView(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr)
	{
	}

	virtual ~Simple3dWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// �V�F�[�_�[���W���[�����쐬
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\simple3d.vert", srcFilePath);
			CreateFilePath(L"shader\\simple3d.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\simple3d.frag", srcFilePath);
			CreateFilePath(L"shader\\simple3d.frag.spv", dstFilePath);
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
		// �p�C�v���C�����C�A�E�g���쐬
		// ----------------------------------------------------------------------------------------------------

		{
			Array1<V3DConstantDesc, 1> constants =
			{
				{
					{ V3D_SHADER_STAGE_VERTEX, 0, sizeof(Simple3dWindow::Scene) }
				}
			};

			V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(TO_UI32(constants.size()), constants.data(), 0, nullptr, &m_pPipelineLayout);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �o�[�e�b�N�X�o�b�t�@�[���쐬
		// ----------------------------------------------------------------------------------------------------

		const Simple3dWindow::Vertex vertices[] =
		{
			// top
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f), },
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(0.0f, -1.0f, 0.0f), },
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(0.0f, -1.0f, 0.0f), },
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f), },

			// bottom
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(0.0f, +1.0f, 0.0f), },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(0.0f, +1.0f, 0.0f), },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(0.0f, +1.0f, 0.0f), },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(0.0f, +1.0f, 0.0f), },

			// front
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), },
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), },
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), },

			// back
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), },
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), },

			// left
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), },
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(-1.0f, 0.0f, 0.0f), },
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(-1.0f, 0.0f, 0.0f), },

			// right
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(+1.0f, 0.0f, 0.0f), },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(+1.0f, 0.0f, 0.0f), },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(+1.0f, 0.0f, 0.0f), },
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(+1.0f, 0.0f, 0.0f), },
		};

		uint16_t indices[] =
		{
			// top
			0, 1, 2,
			2, 3, 0,

			// bottom
			4, 5, 6,
			6, 7, 4,

			// front
			8, 9, 10,
			10, 11, 8,

			// back
			12, 13, 14,
			14, 15, 12,

			// left
			16, 17, 18,
			18, 19, 16,

			// right
			20, 21, 22,
			22, 23, 20,
		};

		Array1<BufferSubresourceDesc, 2> meshSubresources;
		meshSubresources[0].usageFlags = V3D_BUFFER_USAGE_VERTEX;
		meshSubresources[0].size = sizeof(vertices);
		meshSubresources[0].count = 1;
		meshSubresources[1].usageFlags = V3D_BUFFER_USAGE_INDEX;
		meshSubresources[1].size = sizeof(indices);
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
		result = m_pMeshBuffer->Map(0, sizeof(vertices), reinterpret_cast<void**>(&pMemory));
		if (result == V3D_OK)
		{
			// �o�[�e�b�N�X����������
			MemCopy(pMemory + meshMemoryLayouts[0].offset, meshMemoryLayouts[0].stride, vertices, sizeof(vertices));

			// �C���f�b�N�X����������
			MemCopy(pMemory + meshMemoryLayouts[1].offset, meshMemoryLayouts[1].stride, indices, sizeof(indices));

			result = m_pMeshBuffer->Unmap();
			if (result != V3D_OK)
			{
				return false;
			}
		}

		m_VertexOffset = meshMemoryLayouts[0].offset;
		m_IndexOffset = meshMemoryLayouts[1].offset;

		// ----------------------------------------------------------------------------------------------------
		// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X���쐬
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		SetCameraRotation(Vector3(1.0f, 1.0f, 1.0f).ToNormalize(), DEG_TO_RAD(30.0f));

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		SAFE_RELEASE(m_pMeshBuffer);
		SAFE_RELEASE(m_pPipelineLayout);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);
	}

	virtual bool OnIdle() override
	{
		ViewCameraWindow::OnIdle(); //�J�������X�V

		// ----------------------------------------------------------------------------------------------------
		// �萔������
		// ----------------------------------------------------------------------------------------------------

		Simple3dWindow::Scene scene{};
		scene.lightDirection = GetCamera()->GetEye().ToNormalize();
		scene.worldMatrix = Matrix4x4::Identity();
		scene.worldViewProjectionMatrix = GetCamera()->GetViewProjectionMatrix() * scene.worldMatrix;

		// ----------------------------------------------------------------------------------------------------
		// �R�}���h����������Ŏ��s
		// ----------------------------------------------------------------------------------------------------

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		pCommandBufer->BeginRenderPass(pFrameBuffer, true);

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

		pCommandBufer->PushConstant(m_pPipelineLayout, 0, &scene);

		pCommandBufer->BindPipeline(m_pPipeline);
		pCommandBufer->BindVertexBuffer(0, m_pMeshBuffer, m_VertexOffset);
		pCommandBufer->BindIndexBuffer(m_pMeshBuffer, m_IndexOffset, V3D_INDEX_TYPE_UINT16);
		pCommandBufer->DrawIndexed(36, 1, 0, 0, 0);

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

		SAFE_RELEASE(m_pDepthStencilImageView);
		SAFE_RELEASE(m_pDepthStencilImage);
	}

	virtual bool OnRestoreSwapChain() override
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		// ----------------------------------------------------------------------------------------------------
		// �f�v�X�X�e���V���C���[�W���쐬
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

			// �t�H�[�}�b�g���g�p�ł��邩�`�F�b�N����
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
		// �f�v�X�X�e���V���C���[�W�r���[���쐬
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
			pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;

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
		// �����_�[�p�X���쐬
		// ----------------------------------------------------------------------------------------------------

		/******************/
		/* �A�^�b�`�����g */
		/******************/

		std::array<V3DAttachmentDesc, 2> attachments;

		// �J���[
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

		// �f�v�X�X�e���V��
		attachments[1].format = m_pDepthStencilImage->GetDesc().format;
		attachments[1].samples = V3D_SAMPLE_COUNT_1;
		attachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;   // �X�e���V���͎g�p���Ȃ�
		attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED; // ���ɓ���
		attachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[1].clearValue.depthStencil.depth = 1.0f;
		attachments[1].clearValue.depthStencil.stencil = 0;

		/**************/
		/* �T�u�p�X 0 */
		/**************/

		// �J���[
		std::array<V3DAttachmentReference, 1> colorAttachments;
		colorAttachments[0].attachment = 0;
		colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �f�v�X�X�e���V��
		V3DAttachmentReference depthStencilAttachment;
		depthStencilAttachment.attachment = 1;
		depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		std::array<V3DSubpassDesc, 1> subpasses;
		subpasses[0].inputAttachmentCount = 0;
		subpasses[0].pInputAttachments = nullptr;
		subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpasses[0].pColorAttachments = colorAttachments.data();
		subpasses[0].pResolveAttachments = nullptr;
		subpasses[0].pDepthStencilAttachment = &depthStencilAttachment;
		subpasses[0].preserveAttachmentCount = 0;
		subpasses[0].pPreserveAttachments = nullptr;

		/********************/
		/* �T�u�p�X�̈ˑ��� */
		/********************/

		Array1<V3DSubpassDependencyDesc, 2> subpassDependencies;

		subpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dependencyFlags = 0;

		subpassDependencies[1].srcSubpass = 1;
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
		// �t���[���o�b�t�@���쐬
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
		// �O���t�B�b�N�X�p�C�v���C�����쐬
		// ----------------------------------------------------------------------------------------------------

		Array1<V3DPipelineVertexElement, 2> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32_SFLOAT }, // position
				{ 1, 12, V3D_FORMAT_R32G32B32_SFLOAT }  // normal
			}
		};

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{
			{
				{ 0, sizeof(Simple3dWindow::Vertex), 0, TO_UI32(vertexElements.size()) },
			}
		};

		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments =
		{
			{
				{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL }
			}
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

		return true;
	}

private:
	struct Vertex
	{
		Vector3 pos;
		Vector3 normal;
	};

	struct Scene
	{
		Vector3 lightDirection;
		float reserve[1];
		Matrix4x4 worldMatrix;
		Matrix4x4 worldViewProjectionMatrix;
	};

	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DBuffer* m_pMeshBuffer;
	uint64_t m_VertexOffset;
	uint64_t m_IndexOffset;

	IV3DRenderPass* m_pRenderPass;
	IV3DImage* m_pDepthStencilImage;
	IV3DImageView* m_pDepthStencilImageView;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;
};

class Simple3dApp : public Application
{
public:
	virtual ~Simple3dApp() {}

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

		if (m_Window.Initialize(L"simple3d", 1024, 768, true, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
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
	Simple3dWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	Simple3dApp app;
	return app.Execute();
}
