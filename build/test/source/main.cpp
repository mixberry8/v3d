#include "Application.h"
#include "ViewCameraWindow.h"
#include "ResourceHeap.h"
#include "ParallelManager.h"

class MultiThreadWindow : public ViewCameraWindow
{
public:
	MultiThreadWindow() {}
	virtual ~MultiThreadWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();
	}

	virtual bool OnIdle() override
	{
		IV3DCommandBuffer* pCommandBuffer = BeginGraphics();
		if (pCommandBuffer == nullptr)
		{
			return false;
		}

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		V3DViewport viewport{};
		viewport.rect.width = swapChainDesc.imageWidth;
		viewport.rect.height = swapChainDesc.imageHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		pCommandBuffer->SetViewport(0, 1, &viewport);
		pCommandBuffer->SetScissor(0, 1, &viewport.rect);

		pCommandBuffer->BeginRenderPass(m_pRenderPass, m_FrameBuffers[GetSwapChain()->GetCurrentImageIndex()], true);

		// サブパス 0

		// サブパス 1
		pCommandBuffer->NextSubpass();

		// サブパス 2
		pCommandBuffer->NextSubpass();

		pCommandBuffer->EndRenderPass();

		if (EndGraphics() == false)
		{
			return false;
		}

		return true;
	}

	virtual void OnLostSwapChain() override
	{
		SAFE_RELEASE(m_pRenderPass);

		if (m_FrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_FrameBuffers[i]);
			}

			m_FrameBuffers.clear();
		}
	}

	virtual bool OnRestoreSwapChain() override
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		Array1<IV3DImageView*, 3> imageViews;

		{
			// ----------------------------------------------------------------------------------------------------
			// アタッチメント
			// ----------------------------------------------------------------------------------------------------

			V3DAttachmentDesc attachments[3];

			// サブパス０で出力されるアタッチメント
			attachments[0].format = V3D_FORMAT_B8G8R8A8_UNORM;
			attachments[0].samples = V3D_SAMPLE_COUNT_1;
			attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[0].clearValue = V3DClearValue{};

			// サブパス１で出力されるアタッチメント
			attachments[1].format = V3D_FORMAT_B8G8R8A8_UNORM;
			attachments[1].samples = V3D_SAMPLE_COUNT_1;
			attachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[1].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[1].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[1].clearValue = V3DClearValue{};

			// サブパス０および１で生成したものを出力するアタッチメント
			attachments[2].format = swapChainDesc.imageFormat;
			attachments[2].samples = V3D_SAMPLE_COUNT_1;
			attachments[2].loadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[2].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[2].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[2].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[2].initialLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
			attachments[2].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
			attachments[2].clearValue = V3DClearValue{};

			// ----------------------------------------------------------------------------------------------------
			// サブパス
			// ----------------------------------------------------------------------------------------------------

			V3DSubpassDesc subpasses[3];

			// サブパス０
			V3DAttachmentReference colorAttachmentRefs0[1];
			colorAttachmentRefs0[0].attachment = 0;
			colorAttachmentRefs0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

			uint32_t preserveAttachmentRefs0[2];
			preserveAttachmentRefs0[0] = 1;
			preserveAttachmentRefs0[1] = 2;

			subpasses[0].inputAttachmentCount = 0;
			subpasses[0].pInputAttachments = nullptr;
			subpasses[0].colorAttachmentCount = _countof(colorAttachmentRefs0);
			subpasses[0].pColorAttachments = colorAttachmentRefs0;
			subpasses[0].pResolveAttachments = nullptr;
			subpasses[0].pDepthStencilAttachment = nullptr;
			subpasses[0].preserveAttachmentCount = _countof(preserveAttachmentRefs0);
			subpasses[0].pPreserveAttachments = preserveAttachmentRefs0;

			// サブパス１
			V3DAttachmentReference colorAttachmentRefs1[1];
			colorAttachmentRefs1[0].attachment = 1;
			colorAttachmentRefs1[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

			uint32_t preserveAttachmentRefs1[2];
			preserveAttachmentRefs1[0] = 0;
			preserveAttachmentRefs1[1] = 2;

			subpasses[1].inputAttachmentCount = 0;
			subpasses[1].pInputAttachments = nullptr;
			subpasses[1].colorAttachmentCount = _countof(colorAttachmentRefs1);
			subpasses[1].pColorAttachments = colorAttachmentRefs1;
			subpasses[1].pResolveAttachments = nullptr;
			subpasses[1].pDepthStencilAttachment = nullptr;
			subpasses[1].preserveAttachmentCount = _countof(preserveAttachmentRefs1);
			subpasses[1].pPreserveAttachments = preserveAttachmentRefs1;

			// サブパス２
			V3DAttachmentReference inputAttachmentRefs2[2];
			inputAttachmentRefs2[0].attachment = 0;
			inputAttachmentRefs2[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
			inputAttachmentRefs2[1].attachment = 1;
			inputAttachmentRefs2[1].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

			V3DAttachmentReference colorAttachmentRefs2[1];
			colorAttachmentRefs2[0].attachment = 2;
			colorAttachmentRefs2[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

			subpasses[2].inputAttachmentCount = _countof(inputAttachmentRefs2);
			subpasses[2].pInputAttachments = inputAttachmentRefs2;
			subpasses[2].colorAttachmentCount = _countof(colorAttachmentRefs2);
			subpasses[2].pColorAttachments = colorAttachmentRefs2;
			subpasses[2].pResolveAttachments = nullptr;
			subpasses[2].pDepthStencilAttachment = nullptr;
			subpasses[2].preserveAttachmentCount = 0;
			subpasses[2].pPreserveAttachments = nullptr;

			// ----------------------------------------------------------------------------------------------------
			// サブパスの依存性
			// ----------------------------------------------------------------------------------------------------

			V3DSubpassDependencyDesc subpassDependencies[6];

			// サブパス０からサブパス２の依存
			subpassDependencies[0].srcSubpass = 0;
			subpassDependencies[0].dstSubpass = 2;
			subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
			subpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[0].dstAccessMask = V3D_ACCESS_INPUT_ATTACHMENT_READ;
			subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

			// サブパス１からサブパス２への依存
			subpassDependencies[1].srcSubpass = 1;
			subpassDependencies[1].dstSubpass = 2;
			subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
			subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[1].dstAccessMask = V3D_ACCESS_INPUT_ATTACHMENT_READ;
			subpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

			// レンダーパス外からサブパス２への依存
			subpassDependencies[2].srcSubpass = V3D_SUBPASS_EXTERNAL;
			subpassDependencies[2].dstSubpass = 2;
			subpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			subpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[2].srcAccessMask = V3D_ACCESS_MEMORY_READ;
			subpassDependencies[2].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[2].dependencyFlags = 0;

			// サブパス０からレンダーパス外への依存
			subpassDependencies[3].srcSubpass = 0;
			subpassDependencies[3].dstSubpass = V3D_SUBPASS_EXTERNAL;
			subpassDependencies[3].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
			subpassDependencies[3].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[3].srcAccessMask = V3D_ACCESS_INPUT_ATTACHMENT_READ;
			subpassDependencies[3].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[3].dependencyFlags = 0;

			// サブパス１からレンダーパス外への依存
			subpassDependencies[4].srcSubpass = 1;
			subpassDependencies[4].dstSubpass = V3D_SUBPASS_EXTERNAL;
			subpassDependencies[4].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[4].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
			subpassDependencies[4].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[4].dstAccessMask = V3D_ACCESS_INPUT_ATTACHMENT_READ;
			subpassDependencies[4].dependencyFlags = 0;

			// サブパス２からレンダーパス外への依存
			subpassDependencies[5].srcSubpass = 2;
			subpassDependencies[5].dstSubpass = V3D_SUBPASS_EXTERNAL;
			subpassDependencies[5].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			subpassDependencies[5].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
			subpassDependencies[5].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			subpassDependencies[5].dstAccessMask = V3D_ACCESS_MEMORY_READ;
			subpassDependencies[5].dependencyFlags = 0;

			if (Application::GetDevice()->CreateRenderPass(
				_countof(attachments), attachments,
				_countof(subpasses), subpasses,
				_countof(subpassDependencies), subpassDependencies,
				&m_pRenderPass) != V3D_OK)
			{
				return false;
			}
		}

		CommandHelper commandHelper(GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence());

		IV3DCommandBuffer* pCommandBuffer = commandHelper.Begin();
		if (pCommandBuffer == nullptr)
		{
			return false;
		}

		{
			V3DImageDesc imageDesc;
			imageDesc.type = V3D_IMAGE_TYPE_2D;
			imageDesc.format = V3D_FORMAT_B8G8R8A8_UNORM;
			imageDesc.width = swapChainDesc.imageWidth;
			imageDesc.height = swapChainDesc.imageHeight;
			imageDesc.depth = 1;
			imageDesc.levelCount = 1;
			imageDesc.layerCount = 1;
			imageDesc.samples = V3D_SAMPLE_COUNT_1;
			imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
			imageDesc.usageFlags = V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;

			V3DImageViewDesc imageViewDesc;
			imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
			imageViewDesc.baseLevel = 0;
			imageViewDesc.levelCount = 1;
			imageViewDesc.baseLayer = 0;
			imageViewDesc.layerCount = 1;

			V3DBarrierImageViewDesc barrier;
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			barrier.dependencyFlags = 0;

			for (uint32_t i = 0; i < 2; i++)
			{
				IV3DImage* pImage;

				if (Application::GetDevice()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pImage) != V3D_OK)
				{
					return false;
				}

				if (Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pImage) != V3D_OK)
				{
					pImage->Release();
					return false;
				}

				IV3DImageView* pImageView;

				if (Application::GetDevice()->CreateImageView(pImage, imageViewDesc, &pImageView) != V3D_OK)
				{
					pImage->Release();
					return false;
				}

				pImage->Release();

				pCommandBuffer->BarrierImageView(pImageView, barrier);

				imageViews[i] = pImageView;
			}
		}

		if (commandHelper.End() == false)
		{
			return false;
		}

		{
			for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
			{
				IV3DImageView* pImageView;

				if (CreateImageView(i, &pImageView) == false)
				{
					return false;
				}

				imageViews[2] = pImageView;

				IV3DFrameBuffer* pFrameBuffer;

				if (Application::GetDevice()->CreateFrameBuffer(m_pRenderPass, TO_UI32(imageViews.size()), imageViews.data(), &pFrameBuffer) != V3D_OK)
				{
					return false;
				}

				m_FrameBuffers.push_back(pFrameBuffer);

				pImageView->Release();
			}

			{
				for (uint32_t i = 0; i < 2; i++)
				{
					imageViews[i]->Release();
				}
			}
		}

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) override
	{
		return true;
	}

private:
	IV3DRenderPass* m_pRenderPass = nullptr;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
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
