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
		m_pPipelineLayout(nullptr),
		m_pVertexBuffer(nullptr),
		m_pVertexBufferView(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr)
	{
	}

	virtual ~Simple2Window() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// �V�F�[�_�[���W���[�����쐬
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\simple2d.vert", srcFilePath);
			CreateFilePath(L"shader\\simple2d.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\simple2d.frag", srcFilePath);
			CreateFilePath(L"shader\\simple2d.frag.spv", dstFilePath);
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

		V3D_RESULT result = Application::GetDevice()->CreatePipelineLayout(0, nullptr, 0, nullptr, &m_pPipelineLayout);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �o�[�e�b�N�X�o�b�t�@�[���쐬
		// ----------------------------------------------------------------------------------------------------

		const Simple2Window::Vertex vertices[] =
		{
			{ Vector4(+0.0f, -0.5f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), },
			{ Vector4(-0.5f, +0.5f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), },
			{ Vector4(+0.5f, +0.5f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), },
		};

		Array1<V3DBufferSubresourceDesc, 1> bufferSubresources;
		bufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_VERTEX;
		bufferSubresources[0].size = sizeof(vertices);

		result = Application::GetDevice()->CreateBuffer(static_cast<uint32_t>(bufferSubresources.size()), bufferSubresources.data(), &m_pVertexBuffer);
		if (result != V3D_OK)
		{
			return false;
		}

		// �o�b�t�@�[��C���[�W���쐬���������ł̓������͊m�ۂ���Ă��Ȃ����߁A�蓮�Ŋ��蓖�ĂĂ�����K�v������܂��B
		// �{���̓f�o�C�X���������g�p���܂����A����̓z�X�g���������g�p���܂��B
		// �܂����������m�ۂ���O�́A���\�[�X���g�p�\�ȃ������v���p�e�B���`�F�b�N���Ă��������B

		Array1<IV3DResource*, 1> resources = { m_pVertexBuffer };

		V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
		if (Application::GetDevice()->CheckResourceMemoryProperty(memoryPropertyFlags, TO_UI32(resources.size()), resources.data()) != V3D_OK)
		{
			memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
		}

		result = Application::GetDevice()->AllocateResourceMemoryAndBind(memoryPropertyFlags, m_pVertexBuffer);
		if (result != V3D_OK)
		{
			return false;
		}

		// �}�b�v���Ē��_�����������݂܂�
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
		// �o�[�e�b�N�X�o�b�t�@�[�r���[���쐬
		// ----------------------------------------------------------------------------------------------------

		result = Application::GetDevice()->CreateBufferView(m_pVertexBuffer, 0, V3D_FORMAT_UNDEFINED, &m_pVertexBufferView);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X���쐬
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

		SAFE_RELEASE(m_pVertexBufferView);
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pPipelineLayout);
		SAFE_RELEASE(m_pFragmentShaderModule);
		SAFE_RELEASE(m_pVertexShaderModule);
	}

	virtual bool OnIdle() override
	{
		// ----------------------------------------------------------------------------------------------------
		// �O����
		// ----------------------------------------------------------------------------------------------------

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()];

		IV3DImageView* pImageView;
		pFrameBuffer->GetAttachment(0, &pImageView);

		// ----------------------------------------------------------------------------------------------------
		// �R�}���h����������Ŏ��s
		// ----------------------------------------------------------------------------------------------------

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			return false;
		}

		V3DBarrierImageDesc barrier{};
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.srcAccessMask = V3D_ACCESS_MEMORY_READ;
		barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pCommandBufer->BarrierImageView(pImageView, barrier);

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

		pCommandBufer->BindPipeline(m_pPipeline);
		pCommandBufer->BindVertexBufferViews(0, 1, &m_pVertexBufferView);
		pCommandBufer->Draw(3, 1, 0, 0);

		pCommandBufer->EndRenderPass();

		if (EndGraphics() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �㏈��
		// ----------------------------------------------------------------------------------------------------

		SAFE_RELEASE(pImageView);

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
		// �����_�[�p�X���쐬
		// ----------------------------------------------------------------------------------------------------

		// �A�^�b�`�����g
		std::array<V3DAttachmentDesc, 1> attachments;
		attachments[0].format = swapChainDesc.imageFormat;
		attachments[0].samples = V3D_SAMPLE_COUNT_1;
		attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[0].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		attachments[0].clearValue.color.float32[0] = 0.1f;
		attachments[0].clearValue.color.float32[1] = 0.1f;
		attachments[0].clearValue.color.float32[2] = 0.1f;
		attachments[0].clearValue.color.float32[3] = 0.1f;

		// �T�u�p�X 0
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

		V3D_RESULT result = Application::GetDevice()->CreateRenderPass(
			TO_UI32(attachments.size()), attachments.data(),
			TO_UI32(subpasses.size()), subpasses.data(),
			0, nullptr,
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
		// �O���t�B�b�N�X�p�C�v���C�����쐬
		// ----------------------------------------------------------------------------------------------------

		// xyzw ���W
		// rgba �F
		Array1<V3DPipelineVertexElement, 2> vertexElements =
		{ {
			{ 0,  0, V3D_FORMAT_R32G32B32A32_SFLOAT },
			{ 1, 16, V3D_FORMAT_R32G32B32A32_SFLOAT }
		} };

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{ {
			{ 0, sizeof(Simple2Window::Vertex), 0, TO_UI32(vertexElements.size()) },
		} };

		// �u�����h���@�̓R�s�[�ł��B
		Array1<V3DPipelineColorBlendAttachment, 1> coloeBlendAttachments =
		{ {
			{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL }
		} };

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
		pipelineDesc.colorBlend.attachmentCount = TO_UI32(coloeBlendAttachments.size());
		pipelineDesc.colorBlend.pAttachments = coloeBlendAttachments.data();
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
		Vector4 color;
	};

	IV3DShaderModule* m_pVertexShaderModule;
	IV3DShaderModule* m_pFragmentShaderModule;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DBuffer* m_pVertexBuffer;
	IV3DBufferView* m_pVertexBufferView;

	IV3DRenderPass* m_pRenderPass;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;
};

class Simple2dApp : public Application
{
public:
	virtual ~Simple2dApp() {}

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

		if (m_Window.Initialize(L"simple2d", 1024, 768, pWorkQueue, pGraphicsQueue) == false)
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

	Simple2dApp app;
	return app.Execute();
}
