#include "Application.h"
#include "ViewCameraWindow.h"
#include "Font.h"

#define RENDER_PASS_MULTISAMPLE

#define WORK_QUEUE_INDEX 0
#define GRAPHICS_QUEUE_INDEX 1

class MultiSampleWindow : public ViewCameraWindow
{
public:
	MultiSampleWindow() :
		m_pVertexShaderModule(nullptr),
		m_pFragmentShaderModule(nullptr),
		m_pDescriptorSetLayout(nullptr),
		m_pPipelineLayout(nullptr),
		m_pMeshBuffer(nullptr),
		m_pVertexBufferView(nullptr),
		m_pIndexBufferView(nullptr),
		m_pImageView(nullptr),
		m_pSampler(nullptr),
		m_pDescriptorSet(nullptr),
		m_pColorImage(nullptr),
		m_pColorImageView(nullptr),
		m_pDepthStencilImage(nullptr),
		m_pDepthStencilImageView(nullptr),
		m_pRenderPass(nullptr),
		m_pPipeline(nullptr),
		m_SampleCountIndex(0),
		m_SampleShading(0.45f),
		m_pTextRenderPass(0)
	{
	}

	virtual ~MultiSampleWindow() {}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// �f�v�X�X�e���V���̃t�H�[�}�b�g������
		// ----------------------------------------------------------------------------------------------------

		// �t�H�[�}�b�g���g�p�ł��邩�`�F�b�N����
		m_DepthStencilFormat = V3D_FORMAT_D24_UNORM_S8_UINT;
		if (Application::GetDevice()->GetImageFormatDesc(m_DepthStencilFormat, V3D_IMAGE_TYPE_2D, V3D_IMAGE_TILING_OPTIMAL, V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT, nullptr) != V3D_OK)
		{
			m_DepthStencilFormat = V3D_FORMAT_D16_UNORM;
			if (Application::GetDevice()->GetImageFormatDesc(m_DepthStencilFormat, V3D_IMAGE_TYPE_2D, V3D_IMAGE_TILING_OPTIMAL, V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT, nullptr) != V3D_OK)
			{
				m_DepthStencilFormat = V3D_FORMAT_D16_UNORM_S8_UINT;
				if (Application::GetDevice()->GetImageFormatDesc(m_DepthStencilFormat, V3D_IMAGE_TYPE_2D, V3D_IMAGE_TILING_OPTIMAL, V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT, nullptr) != V3D_OK)
				{
					return false;
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �T���v�������
		// ----------------------------------------------------------------------------------------------------

		V3D_SAMPLE_COUNT_FLAG sampleCountTable[] =
		{
			V3D_SAMPLE_COUNT_1,
			V3D_SAMPLE_COUNT_2,
			V3D_SAMPLE_COUNT_4,
			V3D_SAMPLE_COUNT_8,
			V3D_SAMPLE_COUNT_16,
			V3D_SAMPLE_COUNT_32,
			V3D_SAMPLE_COUNT_64,
		};

		const V3DDeviceCaps& deviceCaps = Application::GetDevice()->GetCaps();

		for (uint32_t i = 0; i < _countof(sampleCountTable); i++)
		{
			if ((deviceCaps.framebufferColorSampleCounts & sampleCountTable[i]) &&
				(deviceCaps.framebufferDepthSampleCounts & sampleCountTable[i]) &&
				(deviceCaps.framebufferStencilSampleCounts & sampleCountTable[i]))
			{
				m_SampleCounts.push_back(sampleCountTable[i]);
			}
		}

		m_SampleCountIndex = m_SampleCounts.size() - 1;

		// ----------------------------------------------------------------------------------------------------
		// �t�H���g��������
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Initialize(Application::GetDevice(), L"Arial", 16, 400) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �V�F�[�_�[���W���[�����쐬
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			std::wstring dstFilePath;

			CreateFilePath(L"shader\\source\\multisample.vert", srcFilePath);
			CreateFilePath(L"shader\\multisample.vert.spv", dstFilePath);
			if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
			{
				return false;
			}

			V3D_RESULT result = CreateShaderModuleFromFile(Application::GetDevice(), dstFilePath.c_str(), &m_pVertexShaderModule);
			if (result != V3D_OK)
			{
				return false;
			}

			CreateFilePath(L"shader\\source\\multisample.frag", srcFilePath);
			CreateFilePath(L"shader\\multisample.frag.spv", dstFilePath);
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
		// �f�X�N���v�^�Z�b�g���C�A�E�g���쐬
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
		// �p�C�v���C�����C�A�E�g���쐬
		// ----------------------------------------------------------------------------------------------------

		{
			Array1<V3DConstantDesc, 1> constants =
			{
				{ V3D_SHADER_STAGE_VERTEX, 0, sizeof(MultiSampleWindow::Scene) }
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
		// �o�[�e�b�N�X�o�b�t�@�[���쐬
		// ----------------------------------------------------------------------------------------------------

		const MultiSampleWindow::Vertex vertices[] =
		{
			// top
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector2(1.0f, 0.0f) },

			// bottom
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(0.0f, +1.0f, 0.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(0.0f, +1.0f, 0.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(0.0f, +1.0f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(0.0f, +1.0f, 0.0f), Vector2(1.0f, 0.0f) },

			// front
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, +1.0f), Vector2(1.0f, 0.0f) },

			// back
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector2(1.0f, 0.0f) },

			// left
			{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) },

			// right
			{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(+1.0f, 0.0f, 0.0f), Vector2(0.0f, 0.0f) },
			{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(+1.0f, 0.0f, 0.0f), Vector2(0.0f, 1.0f) },
			{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(+1.0f, 0.0f, 0.0f), Vector2(1.0f, 1.0f) },
			{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(+1.0f, 0.0f, 0.0f), Vector2(1.0f, 0.0f) },
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

		Array1<V3DBufferSubresourceDesc, 2> bufferSubresources =
		{
			{
				{ V3D_BUFFER_USAGE_VERTEX, sizeof(vertices) },
				{ V3D_BUFFER_USAGE_INDEX, sizeof(indices) },
			}
		};

		V3D_RESULT result = Application::GetDevice()->CreateBuffer(static_cast<uint32_t>(bufferSubresources.size()), bufferSubresources.data(), &m_pMeshBuffer);
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

		result = Application::GetDevice()->AllocateResourceMemoryAndBind(memoryPropertyFlags, TO_UI32(resources.size()), resources.data());
		if (result != V3D_OK)
		{
			return false;
		}

		uint8_t* pMemory;
		result = m_pMeshBuffer->Map(0, sizeof(vertices), reinterpret_cast<void**>(&pMemory));
		if (result == V3D_OK)
		{
			V3DBufferSubresourceLayout layout;

			// �o�[�e�b�N�X����������
			layout = m_pMeshBuffer->GetSubresourceLayout(0);
			memcpy_s(pMemory + layout.offset, layout.size, vertices, sizeof(vertices));

			// �C���f�b�N�X����������
			layout = m_pMeshBuffer->GetSubresourceLayout(1);
			memcpy_s(pMemory + layout.offset, layout.size, indices, sizeof(indices));

			result = m_pMeshBuffer->Unmap();
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �o�[�e�b�N�X�o�b�t�@�[�r���[���쐬
		// ----------------------------------------------------------------------------------------------------

		result = Application::GetDevice()->CreateBufferView(m_pMeshBuffer, 0, V3D_FORMAT_UNDEFINED, &m_pVertexBufferView);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
		// ----------------------------------------------------------------------------------------------------

		result = Application::GetDevice()->CreateBufferView(m_pMeshBuffer, 1, V3D_FORMAT_UNDEFINED, &m_pIndexBufferView);
		if (result != V3D_OK)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �C���[�W���쐬
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring srcFilePath;
			CreateFilePath(L"image\\multisample.bmp", srcFilePath);

			result = CreateImageFromFile(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), srcFilePath.c_str(), true, &m_pImageView);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �T���v���[���쐬
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

			result = Application::GetDevice()->CreateSampler(samplerDesc, &m_pSampler);
			if (result != V3D_OK)
			{
				pImage->Release();
				return false;
			}

			pImage->Release();
		}

		// ----------------------------------------------------------------------------------------------------
		// �f�X�N���v�^�Z�b�g���쐬
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
		// �X���b�v�`�F�C���Ɋ֌W����C���^�[�t�F�[�X���쐬
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		SetCameraRotation(Vector3(1.0f, -1.0f, 0.0f).ToNormalize(), DEG_TO_RAD(35.0f));
		SetCameraDistance(3.0f);

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		SAFE_RELEASE(m_pDescriptorSet);
		SAFE_RELEASE(m_pSampler);
		SAFE_RELEASE(m_pImageView);
		SAFE_RELEASE(m_pIndexBufferView);
		SAFE_RELEASE(m_pVertexBufferView);
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
		// �J�������X�V
		// ----------------------------------------------------------------------------------------------------

		ViewCameraWindow::OnIdle();

		// ----------------------------------------------------------------------------------------------------
		// �T���v����
		// ----------------------------------------------------------------------------------------------------

		uint32_t sampleNum = 0;

		switch (m_SampleCounts[m_SampleCountIndex])
		{
		case V3D_SAMPLE_COUNT_1:
			sampleNum = 1;
			break;
		case V3D_SAMPLE_COUNT_2:
			sampleNum = 2;
			break;
		case V3D_SAMPLE_COUNT_4:
			sampleNum = 4;
			break;
		case V3D_SAMPLE_COUNT_8:
			sampleNum = 8;
			break;
		case V3D_SAMPLE_COUNT_16:
			sampleNum = 16;
			break;
		case V3D_SAMPLE_COUNT_32:
			sampleNum = 32;
			break;
		case V3D_SAMPLE_COUNT_64:
			sampleNum = 64;
			break;
		}

		// ----------------------------------------------------------------------------------------------------
		// �e�L�X�g
		// ----------------------------------------------------------------------------------------------------

		wchar_t text[256];

		m_Font.Reset();

		swprintf_s(text, L"Fps %f", Application::GetFps());
		m_Font.AddText(16, 16, text);

		swprintf_s(text, L"A - S : SampleShading %.2f", m_SampleShading);
		m_Font.AddText(16, 48, text);

		wsprintf(text, L"Z - X : SampleCount %u", sampleNum);
		m_Font.AddText(16, 64, text);

		if (m_Font.Bake(GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence()) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// �萔������
		// ----------------------------------------------------------------------------------------------------

		MultiSampleWindow::Scene scene{};
		scene.lightDirection = GetCamera()->GetEye().ToNormalize();
		scene.worldMatrix = Matrix4x4::Identity();
		scene.worldViewProjectionMatrix = GetCamera()->GetViewProjectionMatrix() * scene.worldMatrix;

		// ----------------------------------------------------------------------------------------------------
		// �`��
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();

		IV3DFrameBuffer* pFrameBuffer = m_FrameBuffers[currentImageIndex];
		IV3DFrameBuffer* pTextFrameBuffer = m_TextFrameBuffers[currentImageIndex];

		IV3DImageView* pImageView;
		pFrameBuffer->GetAttachment(0, &pImageView);

		IV3DCommandBuffer* pCommandBufer = BeginGraphics();
		if (pCommandBufer == nullptr)
		{
			SAFE_RELEASE(pImageView);
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

		/*********************************/
		/* �����_�[�p�X - ���b�V���̕`�� */
		/*********************************/

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

		pCommandBufer->PushConstant(m_pPipelineLayout, 0, &scene);

		pCommandBufer->BindPipeline(m_pPipeline);
		pCommandBufer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet);
		pCommandBufer->BindVertexBufferViews(0, 1, &m_pVertexBufferView);
		pCommandBufer->BindIndexBufferView(m_pIndexBufferView, V3D_INDEX_TYPE_UINT16);
		pCommandBufer->DrawIndexed(36, 1, 0, 0, 0);

		pCommandBufer->EndRenderPass();

		/*******************************************************/
		/* �����_�[�p�X���g�p���Ȃ����]���u ( �}���`�T���v�� ) */
		/*******************************************************/

#ifndef RENDER_PASS_MULTISAMPLE

		if (sampleNum != 1)
		{
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			pCommandBufer->BarrierImageView(pImageView, barrier);

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.dstAccessMask = V3D_ACCESS_TRANSFER_READ;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
			pCommandBufer->BarrierImageView(m_pColorImageView, barrier);

			pCommandBufer->ResolveImageView(pImageView, V3D_IMAGE_LAYOUT_TRANSFER_DST, m_pColorImageView, V3D_IMAGE_LAYOUT_TRANSFER_SRC);

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			pCommandBufer->BarrierImageView(pImageView, barrier);

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = V3D_ACCESS_TRANSFER_READ;
			barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			pCommandBufer->BarrierImageView(m_pColorImageView, barrier);
		}

#endif //RENDER_PASS_MULTISAMPLE

		/*********************************/
		/* �����_�[�p�X - �e�L�X�g�̕`�� */
		/*********************************/

		pCommandBufer->BeginRenderPass(m_pTextRenderPass, pTextFrameBuffer, true);

		m_Font.Flush(pCommandBufer, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

		pCommandBufer->EndRenderPass();

		/*********************************/

		if (EndGraphics() == false)
		{
			SAFE_RELEASE(pImageView);
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
		m_Font.Lost();

		ReleaseGraphicsPipeline();
		ReleaseFrameBuffer();
		ReleaseRenderPass();
	}

	virtual bool OnRestoreSwapChain() override
	{
		if (CreateRenderPass() == false)
		{
			return false;
		}

		if (CreateFrameBuffer() == false)
		{
			return false;
		}

		if (CreateGraphicsPipeline() == false)
		{
			return false;
		}

		m_Font.Restore(m_pTextRenderPass, 0);

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) override
	{
		if (keyCode == 'A')
		{
			if (m_SampleShading > 0.0f)
			{
				m_SampleShading -= 0.01f;
				m_SampleShading = MAXIMUM(0.0f, m_SampleShading);

				Application::GetDevice()->WaitIdle();

				ReleaseGraphicsPipeline();
				CreateGraphicsPipeline();
			}
		}
		else if (keyCode == 'S')
		{
			if (m_SampleShading < 1.0f)
			{
				m_SampleShading += 0.01f;
				m_SampleShading = MINIMUM(1.0f, m_SampleShading);

				Application::GetDevice()->WaitIdle();

				ReleaseGraphicsPipeline();
				CreateGraphicsPipeline();
			}
		}

		if (keyCode == 'Z')
		{
			if (m_SampleCountIndex > 0)
			{
				m_SampleCountIndex--;

				Application::GetDevice()->WaitIdle();

				OnLostSwapChain();
				OnRestoreSwapChain();
			}
		}
		else if (keyCode == 'X')
		{
			if (m_SampleCountIndex < (m_SampleCounts.size() - 1))
			{
				m_SampleCountIndex++;

				Application::GetDevice()->WaitIdle();

				OnLostSwapChain();
				OnRestoreSwapChain();
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

	struct Scene
	{
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
	IV3DBufferView* m_pVertexBufferView;
	IV3DBufferView* m_pIndexBufferView;
	IV3DImageView* m_pImageView;
	IV3DSampler* m_pSampler;
	IV3DDescriptorSet* m_pDescriptorSet;

	IV3DRenderPass* m_pRenderPass;
	IV3DImage* m_pColorImage;
	IV3DImageView* m_pColorImageView;
	IV3DImage* m_pDepthStencilImage;
	IV3DImageView* m_pDepthStencilImageView;
	std::vector<IV3DFrameBuffer*> m_FrameBuffers;
	IV3DPipeline* m_pPipeline;

	V3D_FORMAT m_DepthStencilFormat;
	std::vector<V3D_SAMPLE_COUNT_FLAG> m_SampleCounts;
	size_t m_SampleCountIndex;
	float m_SampleShading;

	std::vector<IV3DFrameBuffer*> m_TextFrameBuffers;
	IV3DRenderPass* m_pTextRenderPass;

	bool CreateRenderPass()
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		V3D_SAMPLE_COUNT_FLAG sampleCount = m_SampleCounts[m_SampleCountIndex];

		// ----------------------------------------------------------------------------------------------------
		// ���b�V���̕`��
		// ----------------------------------------------------------------------------------------------------

		{
			/******************/
			/* �A�^�b�`�����g */
			/******************/

			std::vector<V3DAttachmentDesc> attachments;

			if (sampleCount == V3D_SAMPLE_COUNT_1)
			{
				attachments.resize(2);
			}
			else
			{
				attachments.resize(3);
			}

			// �J���[
			attachments[0].format = swapChainDesc.imageFormat;
			attachments[0].samples = V3D_SAMPLE_COUNT_1;
			attachments[0].loadOp = (sampleCount == V3D_SAMPLE_COUNT_1) ? V3D_ATTACHMENT_LOAD_OP_CLEAR : V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[0].clearValue.color.float32[0] = 0.1f;
			attachments[0].clearValue.color.float32[1] = 0.1f;
			attachments[0].clearValue.color.float32[2] = 0.1f;
			attachments[0].clearValue.color.float32[3] = 0.1f;

			// �f�v�X�X�e���V�� - �}���`�T���v��
			attachments[1].format = m_DepthStencilFormat;
			attachments[1].samples = sampleCount;
			attachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
			attachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
			attachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
			attachments[1].clearValue.depthStencil.depth = 1.0f;
			attachments[1].clearValue.depthStencil.stencil = 0;

			// �J���[ - �}���`�T���v��
			if (sampleCount != V3D_SAMPLE_COUNT_1)
			{
				attachments[2].format = swapChainDesc.imageFormat;
				attachments[2].samples = sampleCount;
				attachments[2].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
				attachments[2].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
				attachments[2].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
				attachments[2].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
				attachments[2].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
				attachments[2].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
				attachments[2].clearValue.color.float32[0] = 0.1f;
				attachments[2].clearValue.color.float32[1] = 0.1f;
				attachments[2].clearValue.color.float32[2] = 0.1f;
				attachments[2].clearValue.color.float32[3] = 0.1f;
			}

			/*************************/
			/* �T�u�p�X 0 - ���b�V�� */
			/*************************/

			std::array<V3DSubpassDesc, 1> subpasses;

			// �J���[
			Array1<V3DAttachmentReference, 1> colorAttachments0;
			if (sampleCount == V3D_SAMPLE_COUNT_1)
			{
				colorAttachments0[0].attachment = 0;
				colorAttachments0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			}
			else
			{
				colorAttachments0[0].attachment = 2;
				colorAttachments0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			}

			// �f�v�X�X�e���V��
			V3DAttachmentReference depthStencilAttachment0;
			depthStencilAttachment0.attachment = 1;
			depthStencilAttachment0.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

			// ���]���u ( �}���`�T���v�� )
			std::vector<V3DAttachmentReference> resolveAttachments0;
#ifdef RENDER_PASS_MULTISAMPLE
			if (sampleCount != V3D_SAMPLE_COUNT_1)
			{
				resolveAttachments0.resize(1);
				resolveAttachments0[0].attachment = 0;
				resolveAttachments0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			}
#endif //RENDER_PASS_MULTISAMPLE

			subpasses[0].inputAttachmentCount = 0;
			subpasses[0].pInputAttachments = nullptr;
			subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments0.size());
			subpasses[0].pColorAttachments = colorAttachments0.data();
			subpasses[0].pResolveAttachments = resolveAttachments0.data();
			subpasses[0].pDepthStencilAttachment = &depthStencilAttachment0;
			subpasses[0].preserveAttachmentCount = 0;
			subpasses[0].pPreserveAttachments = nullptr;

			V3D_RESULT result = Application::GetDevice()->CreateRenderPass(
				TO_UI32(attachments.size()), attachments.data(),
				TO_UI32(subpasses.size()), subpasses.data(),
				0, nullptr,
				&m_pRenderPass);
		}

		// ----------------------------------------------------------------------------------------------------
		// �e�L�X�g�̕`��
		// ----------------------------------------------------------------------------------------------------

		{
			Array1<V3DAttachmentDesc, 1> attachments;

			// �J���[
			attachments[0].format = swapChainDesc.imageFormat;
			attachments[0].samples = V3D_SAMPLE_COUNT_1;
			attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
			attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
			attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
			attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
			attachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			attachments[0].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;

			Array1<V3DAttachmentReference, 1> colorAttachments0;
			colorAttachments0[0].attachment = 0;
			colorAttachments0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

			/*************************/
			/* �T�u�p�X 0 - �e�L�X�g */
			/*************************/

			std::array<V3DSubpassDesc, 1> subpasses;

			subpasses[0].inputAttachmentCount = 0;
			subpasses[0].pInputAttachments = nullptr;
			subpasses[0].colorAttachmentCount = static_cast<uint32_t>(colorAttachments0.size());
			subpasses[0].pColorAttachments = colorAttachments0.data();
			subpasses[0].pResolveAttachments = nullptr;
			subpasses[0].pDepthStencilAttachment = nullptr;
			subpasses[0].preserveAttachmentCount = 0;
			subpasses[0].pPreserveAttachments = nullptr;

			V3D_RESULT result = Application::GetDevice()->CreateRenderPass(
				TO_UI32(attachments.size()), attachments.data(),
				TO_UI32(subpasses.size()), subpasses.data(),
				0, nullptr,
				&m_pTextRenderPass);
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	void ReleaseRenderPass()
	{
		SAFE_RELEASE(m_pTextRenderPass);
		SAFE_RELEASE(m_pRenderPass);
	}

	bool CreateFrameBuffer()
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		V3D_SAMPLE_COUNT_FLAG sampleCount = m_SampleCounts[m_SampleCountIndex];

		// ----------------------------------------------------------------------------------------------------
		// �J���[�C���[�W���쐬
		// ----------------------------------------------------------------------------------------------------

		// �J���[
		if (sampleCount != V3D_SAMPLE_COUNT_1)
		{
			V3DImageDesc imageDesc{};
			imageDesc.type = V3D_IMAGE_TYPE_2D;
			imageDesc.format = swapChainDesc.imageFormat;
			imageDesc.width = swapChainDesc.imageWidth;
			imageDesc.height = swapChainDesc.imageHeight;
			imageDesc.depth = 1;
			imageDesc.levelCount = 1;
			imageDesc.layerCount = 1;
			imageDesc.samples = sampleCount;
			imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
			imageDesc.usageFlags = V3D_IMAGE_USAGE_COLOR_ATTACHMENT;

#ifndef RENDER_PASS_MULTISAMPLE
			imageDesc.usageFlags |= V3D_IMAGE_USAGE_TRANSFER_SRC;
#endif //RENDER_PASS_MULTISAMPLE

			V3D_RESULT result = Application::GetDevice()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &m_pColorImage);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// �f�v�X�X�e���V��
		{
			V3DImageDesc imageDesc{};
			imageDesc.type = V3D_IMAGE_TYPE_2D;
			imageDesc.format = m_DepthStencilFormat;
			imageDesc.width = swapChainDesc.imageWidth;
			imageDesc.height = swapChainDesc.imageHeight;
			imageDesc.depth = 1;
			imageDesc.levelCount = 1;
			imageDesc.layerCount = 1;
			imageDesc.samples = sampleCount;
			imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
			imageDesc.usageFlags = V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;

			V3D_RESULT result = Application::GetDevice()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &m_pDepthStencilImage);
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �C���[�W�̃��������m��
		// ----------------------------------------------------------------------------------------------------

		{
			std::vector<IV3DResource*> resources;

			if (m_pColorImage != nullptr)
			{
				resources.push_back(m_pColorImage);
			}

			resources.push_back(m_pDepthStencilImage);

			V3D_RESULT result = Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, TO_UI32(resources.size()), resources.data());
			if (result != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// �C���[�W�r���[���쐬
		// ----------------------------------------------------------------------------------------------------

		// �J���[
		if (sampleCount != V3D_SAMPLE_COUNT_1)
		{
			V3DImageViewDesc imageViewDesc{};
			imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
			imageViewDesc.baseLevel = 0;
			imageViewDesc.levelCount = 1;
			imageViewDesc.baseLayer = 0;
			imageViewDesc.layerCount = 1;

			V3D_RESULT result = Application::GetDevice()->CreateImageView(m_pColorImage, imageViewDesc, &m_pColorImageView);
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
			barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
			pCommandBuffer->BarrierImageView(m_pColorImageView, barrier);

			EndWork();
		}

		// �f�v�X�X�e���V��
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
		// �t���[���o�b�t�@���쐬
		// ----------------------------------------------------------------------------------------------------

		m_FrameBuffers.resize(swapChainDesc.imageCount);
		m_TextFrameBuffers.resize(swapChainDesc.imageCount);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			IV3DImageView* pImageView;
			if (CreateImageView(i, &pImageView) == false)
			{
				return false;
			}

			std::vector<IV3DImageView*> attachments;

			// ���b�V���̕`��
			if (sampleCount == V3D_SAMPLE_COUNT_1)
			{
				attachments.push_back(pImageView);
				attachments.push_back(m_pDepthStencilImageView);
			}
			else
			{
				attachments.push_back(pImageView);
				attachments.push_back(m_pDepthStencilImageView);
				attachments.push_back(m_pColorImageView);
			}

			V3D_RESULT result = Application::GetDevice()->CreateFrameBuffer(m_pRenderPass, TO_UI32(attachments.size()), attachments.data(), &m_FrameBuffers[i]);
			if (result != V3D_OK)
			{
				SAFE_RELEASE(pImageView);
				return false;
			}

			// �e�L�X�g�̕`��
			attachments.clear();
			attachments.push_back(pImageView);

			result = Application::GetDevice()->CreateFrameBuffer(m_pTextRenderPass, TO_UI32(attachments.size()), attachments.data(), &m_TextFrameBuffers[i]);
			if (result != V3D_OK)
			{
				SAFE_RELEASE(pImageView);
				return false;
			}

			SAFE_RELEASE(pImageView);
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	void ReleaseFrameBuffer()
	{
		if (m_TextFrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_TextFrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_TextFrameBuffers[i]);
			}
			m_TextFrameBuffers.clear();
		}

		if (m_FrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_FrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_FrameBuffers[i]);
			}
			m_FrameBuffers.clear();
		}

		SAFE_RELEASE(m_pDepthStencilImageView);
		SAFE_RELEASE(m_pDepthStencilImage);

		SAFE_RELEASE(m_pColorImageView);
		SAFE_RELEASE(m_pColorImage);
	}

	bool CreateGraphicsPipeline()
	{
		Array1<V3DPipelineVertexElement, 3> vertexElements =
		{
			{
				{ 0,  0, V3D_FORMAT_R32G32B32_SFLOAT }, // position
				{ 1, 12, V3D_FORMAT_R32G32B32_SFLOAT }, // normal
				{ 2, 24, V3D_FORMAT_R32G32_SFLOAT }, // uv
			}
		};

		Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
		{
			{ 0, sizeof(MultiSampleWindow::Vertex), 0, TO_UI32(vertexElements.size()) },
		};

		Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments =
		{
			{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL }
		};

		V3D_SAMPLE_COUNT_FLAG sampleCount = m_SampleCounts[m_SampleCountIndex];

		const V3DDeviceCaps& deviceCaps = Application::GetDevice()->GetCaps();

		// �T���v���V�F�[�f�B���O���L�����ǂ���
		bool sampleShadingEnable = (deviceCaps.multisampleFlags & V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING) == V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING;

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
		pipelineDesc.multisample.rasterizationSamples = sampleCount;
		pipelineDesc.multisample.sampleShadingEnable = sampleShadingEnable;
		pipelineDesc.multisample.minSampleShading = (sampleShadingEnable == true)? m_SampleShading : 0.0f;
		pipelineDesc.colorBlend.attachmentCount = TO_UI32(colorBlendAttachments.size());
		pipelineDesc.colorBlend.pAttachments = colorBlendAttachments.data();
		pipelineDesc.pRenderPass = m_pRenderPass;
		pipelineDesc.subpass = 0;

		V3D_RESULT result = Application::GetDevice()->CreateGraphicsPipeline(m_pPipelineLayout, pipelineDesc, &m_pPipeline);
		if (result != V3D_OK)
		{
			return false;
		}

		return true;
	}

	void ReleaseGraphicsPipeline()
	{
		SAFE_RELEASE(m_pPipeline);
	}
};

class MultiSampleApp : public Application
{
public:
	virtual ~MultiSampleApp() {}

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

		if (m_Window.Initialize(L"multisample", 1024, 768, pWorkQueue, pGraphicsQueue) == false)
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
	MultiSampleWindow m_Window;
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	MultiSampleApp app;
	return app.Execute();
}
