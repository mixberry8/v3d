#include "Font.h"

Font::Font() :
	m_Height(0),
	m_HalfHeight(0),
	m_BlockCount(0),
	m_pDevice(nullptr),
	m_pVertShaderModule(nullptr),
	m_pFragShaderModule(nullptr),
	m_pDescriptorSet(nullptr),
	m_pPipelineLayout(nullptr),
	m_pPipeline(nullptr),
	m_ScreenWidth(0),
	m_ScreenHeight(0)
{
}

Font::~Font()
{
	Dispose();
}

bool Font::Initialize(IV3DDevice* pDevice, IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence, const wchar_t* pFontName, int32_t height, int32_t weight)
{
	SAFE_ADD_REF(pDevice);
	m_pDevice = pDevice;

	// ----------------------------------------------------------------------------------------------------
	// フォントイメージのフォーマットを決定する
	// ----------------------------------------------------------------------------------------------------

	V3D_FORMAT imageFormat = V3D_FORMAT_B8G8R8A8_UNORM;
	if (m_pDevice->CheckImageFormatFeature(imageFormat, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
	{
		imageFormat = V3D_FORMAT_A8B8G8R8_UNORM;
		if (m_pDevice->CheckImageFormatFeature(imageFormat, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
		{
			imageFormat = V3D_FORMAT_B8G8R8A8_UNORM;
			if (m_pDevice->CheckImageFormatFeature(imageFormat, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
			{
				return false;
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// シェーダーモジュールを作成
	// ----------------------------------------------------------------------------------------------------

	{
		std::wstring srcFilePath;
		std::wstring dstFilePath;

		CreateFilePath(L"shader\\source\\font.vert", srcFilePath);
		CreateFilePath(L"shader\\font.vert.spv", dstFilePath);

		V3D_RESULT result = CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str());
		if (result != V3D_OK)
		{
			return false;
		}

		result = CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pVertShaderModule);
		if (result != V3D_OK)
		{
			return false;
		}

		CreateFilePath(L"shader\\source\\font.frag", srcFilePath);
		CreateFilePath(L"shader\\font.frag.spv", dstFilePath);

		result = CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str());
		if (result != V3D_OK)
		{
			return false;
		}

		result = CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pFragShaderModule);
		if (result != V3D_OK)
		{
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// フォントイメージを作成
	// ----------------------------------------------------------------------------------------------------

	IV3DImageView* pDeviceImageView;

	{
		/******************/
		/* フォントを作成 */
		/******************/

		HFONT fontHandle = CreateFont(
			height,
			0, 0, 0,
			weight,
			0, 0, 0,
			SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN,
			pFontName);

		if (fontHandle == nullptr)
		{
			return false;
		}

		/************************/
		/* ホストイメージを作成 */
		/************************/

		V3DImageDesc imageDesc;
		imageDesc.type = V3D_IMAGE_TYPE_2D;
		imageDesc.format = imageFormat;
		imageDesc.width = Font::ImageSize;
		imageDesc.height = Font::ImageSize;
		imageDesc.depth = 1;
		imageDesc.levelCount = 1;
		imageDesc.layerCount = 1;
		imageDesc.samples = V3D_SAMPLE_COUNT_1;
		imageDesc.tiling = V3D_IMAGE_TILING_LINEAR;
		imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_SRC;

		IV3DImage* pHostImage;
		V3D_RESULT result = m_pDevice->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_PREINITIALIZED, &pHostImage);
		if (result != V3D_OK)
		{
			DeleteObject(fontHandle);
			return false;
		}

		result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pHostImage);
		if (result != V3D_OK)
		{
			pHostImage->Release();
			DeleteObject(fontHandle);
			return false;
		}

		/************************************************/
		/* デバイスコンテキストを取得してフォントを設定 */
		/************************************************/

		HDC dcHandle = ::GetDC(nullptr);
		HFONT oldFontHandle = static_cast<HFONT>(SelectObject(dcHandle, fontHandle));

		TEXTMETRIC textMetric;
		if (::GetTextMetrics(dcHandle, &textMetric) == FALSE)
		{
			pHostImage->Release();
			DeleteObject(fontHandle);
			::SelectObject(dcHandle, oldFontHandle);
			::ReleaseDC(nullptr, dcHandle);
			return false;
		}

		/****************************************/
		/* ホストイメージにフォントをマッピング */
		/****************************************/

		void* pMemory;
		result = pHostImage->Map(0, 0, &pMemory);
		if (result != V3D_OK)
		{
			pHostImage->Release();
			DeleteObject(fontHandle);
			::SelectObject(dcHandle, oldFontHandle);
			::ReleaseDC(nullptr, dcHandle);
			return false;
		}

		std::vector<uint8_t> glyphBuffer;
		uint32_t imageX = 0;
		uint32_t imageY = 0;
		uint32_t imageLine = 0;

		m_Glyphs.reserve(Font::LastCode - Font::FirstCode + 1);

		for (wchar_t code = Font::FirstCode; code <= Font::LastCode; code++)
		{
			GLYPHMETRICS glyphMetrics;

			uint32_t glyphBufferSize = GetGlyphOutlineW(dcHandle, code, GGO_GRAY8_BITMAP, &glyphMetrics, 0, nullptr, &Font::Matrix);
			ASSERT(glyphBufferSize != GDI_ERROR);

			glyphBuffer.resize(glyphBufferSize);

			if (Font::ImageSize <= (imageX + glyphMetrics.gmBlackBoxX))
			{
				imageX = Font::Margin;
				imageY = imageLine + Font::Margin;
			}

			ASSERT(Font::ImageSize >= (imageY + glyphMetrics.gmBlackBoxY));

			Glyph glyph;
			glyph.code = code;
			glyph.left = static_cast<float>(imageX) * Font::InvImageSizeF;
			glyph.right = static_cast<float>(imageX + glyphMetrics.gmBlackBoxX) * Font::InvImageSizeF;
			glyph.top = static_cast<float>(imageY) * Font::InvImageSizeF;
			glyph.bottom = static_cast<float>(imageY + glyphMetrics.gmBlackBoxY) * Font::InvImageSizeF;
			glyph.x = glyphMetrics.gmptGlyphOrigin.x;
			glyph.y = textMetric.tmAscent - glyphMetrics.gmptGlyphOrigin.y;
			glyph.width = glyphMetrics.gmBlackBoxX;
			glyph.height = glyphMetrics.gmBlackBoxY;
			glyph.cellWidth = glyphMetrics.gmCellIncX;
			m_Glyphs.push_back(glyph);

			if (GetGlyphOutlineW(dcHandle, code, GGO_GRAY8_BITMAP, &glyphMetrics, glyphBufferSize, glyphBuffer.data(), &Font::Matrix) != GDI_ERROR)
			{
				uint32_t srcPitch = ((glyphMetrics.gmBlackBoxX + 3) >> 2) << 2;
				uint8_t* pSrcRow = glyphBuffer.data();
				uint32_t* pDstRow = static_cast<uint32_t*>(pMemory) + Font::ImageSize * imageY + imageX;

				for (uint32_t i = 0; i < glyphMetrics.gmBlackBoxY; i++)
				{
					uint8_t* pSrc = pSrcRow;
					uint8_t* pSrcEnd = pSrc + glyphMetrics.gmBlackBoxX;
					uint32_t* pDst = pDstRow;

					while (pSrc != pSrcEnd)
					{
						uint8_t alpha = *pSrc++;
						alpha = ((alpha * 255) >> 6);

						*pDst++ = (alpha << 24) | 0x00FFFFFF;
					}

					pSrcRow += srcPitch;
					pDstRow += Font::ImageSize;
				}
			}

			imageX += glyphMetrics.gmBlackBoxX + Font::Margin;
			imageLine = MAXIMUM(imageY + TO_I32(glyphMetrics.gmBlackBoxY), imageLine);
		}

		DeleteObject(fontHandle);
		SelectObject(dcHandle, oldFontHandle);
		ReleaseDC(nullptr, dcHandle);

		result = pHostImage->Unmap();
		if (result != V3D_OK)
		{
			pHostImage->Release();
			return false;
		}

		/**************************/
		/* デバイスイメージを作成 */
		/**************************/

		imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
		imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED;

		IV3DImage* pDeviceImage;
		result = m_pDevice->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pDeviceImage, L"Font");
		if (result != V3D_OK)
		{
			pHostImage->Release();
			return false;
		}

		result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pDeviceImage, L"Font");
		if (result != V3D_OK)
		{
			pDeviceImage->Release();
			pHostImage->Release();
			return false;
		}

		/******************************************/
		/* ホストイメージをデバイスイメージに転送 */
		/******************************************/

		CommandHelper commandHelper(pQueue, pCommandBuffer, pFence);

		if (commandHelper.Begin() == nullptr)
		{
			pDeviceImage->Release();
			pHostImage->Release();
			return false;
		}

		V3DBarrierImageDesc barrier{};
		barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dependencyFlags = 0;

		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = V3D_ACCESS_TRANSFER_READ;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_PREINITIALIZED;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
		pCommandBuffer->BarrierImage(pHostImage, barrier);

		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		pCommandBuffer->BarrierImage(pDeviceImage, barrier);

		pCommandBuffer->CopyImage(pDeviceImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, pHostImage, V3D_IMAGE_LAYOUT_TRANSFER_SRC);

		barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.dstAccessMask = V3D_ACCESS_SHADER_READ;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pCommandBuffer->BarrierImage(pDeviceImage, barrier);

		if (commandHelper.End() == false)
		{
			pDeviceImage->Release();
			pHostImage->Release();
			return false;
		}

		pHostImage->Release();

		/**********************************/
		/* デバイスイメージのビューを作成 */
		/**********************************/

		V3DImageViewDesc imageViewDesc;
		imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
		imageViewDesc.baseLevel = 0;
		imageViewDesc.levelCount = 1;
		imageViewDesc.baseLayer = 0;
		imageViewDesc.layerCount = 1;

		result = m_pDevice->CreateImageView(pDeviceImage, imageViewDesc, &pDeviceImageView, L"Font");
		if (result != V3D_OK)
		{
			pDeviceImage->Release();
			return false;
		}

		pDeviceImage->Release();
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	IV3DDescriptorSetLayout* pDescriptorSetLayout;

	{
		Array1<V3DDescriptorDesc, 1> descriptors =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT }
		};

		V3D_RESULT result = m_pDevice->CreateDescriptorSetLayout(TO_UI32(descriptors.size()), descriptors.data(), Font::DescriptorSetPoolSize, Font::DescriptorSetPoolSize, &pDescriptorSetLayout);
		if (result != V3D_OK)
		{
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	{
		Array1<V3DConstantDesc, 1> constants = { V3D_SHADER_STAGE_VERTEX, 0, sizeof(Matrix4x4) };
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts = { pDescriptorSetLayout };

		V3D_RESULT result = m_pDevice->CreatePipelineLayout(TO_UI32(constants.size()), constants.data(), TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(), &m_pPipelineLayout);
		if (result != V3D_OK)
		{
			pDescriptorSetLayout->Release();
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// サンプラーを作成
	// ----------------------------------------------------------------------------------------------------

	IV3DSampler* pSampler;

	{
		V3DSamplerDesc samplerDesc{};
		samplerDesc.magFilter = V3D_FILTER_NEAREST;
		samplerDesc.minFilter = V3D_FILTER_NEAREST;
		samplerDesc.mipmapMode = V3D_MIPMAP_MODE_NEAREST;
		samplerDesc.addressModeU = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.addressModeV = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.addressModeW = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerDesc.mipLodBias = 0;
		samplerDesc.anisotropyEnable = false;
		samplerDesc.maxAnisotropy = 0.0f;
		samplerDesc.compareEnable = false;
		samplerDesc.compareOp = V3D_COMPARE_OP_ALWAYS;
		samplerDesc.minLod = 0.0f;
		samplerDesc.maxLod = 0.0f;
		samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

		V3D_RESULT result = m_pDevice->CreateSampler(samplerDesc, &pSampler);
		if (result != V3D_OK)
		{
			pDescriptorSetLayout->Release();
			return false;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	{
		V3D_RESULT result = m_pDevice->CreateDescriptorSet(pDescriptorSetLayout, &m_pDescriptorSet);
		if (result != V3D_OK)
		{
			pDescriptorSetLayout->Release();
			pSampler->Release();
			return false;
		}

		result = m_pDescriptorSet->SetImageViewAndSampler(0, pDeviceImageView, pSampler);
		if (result != V3D_OK)
		{
			m_pDescriptorSet->Release();
			pDescriptorSetLayout->Release();
			pSampler->Release();
			pDeviceImageView->Release();
			return false;
		}

		m_pDescriptorSet->Update();
	}

	// ----------------------------------------------------------------------------------------------------
	// 後処理
	// ----------------------------------------------------------------------------------------------------

	pDescriptorSetLayout->Release();
	pSampler->Release();
	pDeviceImageView->Release();

	// ----------------------------------------------------------------------------------------------------
	// 初期化
	// ----------------------------------------------------------------------------------------------------

	m_BlockCharacterMax = (Font::ImageSize / height) * (Font::ImageSize / height) * 2;
	m_Height = height;
	m_HalfHeight = m_Height / 2;

	if (AddBlock() == nullptr)
	{
		return false;
	}

	m_BlockCount = 0;

	return true;
}

void Font::Dispose()
{
	for (size_t i = 0; i < m_Blocks.size(); i++)
	{
		Font::Block& block = m_Blocks[i];
		SAFE_RELEASE(block.pVertexBuffer);
	}

	SAFE_RELEASE(m_pPipeline);
	SAFE_RELEASE(m_pPipelineLayout);
	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pFragShaderModule);
	SAFE_RELEASE(m_pVertShaderModule);
	SAFE_RELEASE(m_pDevice);
}

void Font::Lost()
{
	SAFE_RELEASE(m_pPipeline);
}

bool Font::Restore(IV3DRenderPass* pRenderPass, uint32_t subpass, uint32_t screenWidth, uint32_t screenHeight)
{
	// ----------------------------------------------------------------------------------------------------
	// 念のため解放
	// ----------------------------------------------------------------------------------------------------

	Lost();

	// ----------------------------------------------------------------------------------------------------
	// パイプラインを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<V3DPipelineVertexElement, 2> vertexElements =
	{
		{
			{ 0,  0, V3D_FORMAT_R32G32B32A32_SFLOAT },
			{ 1, 16, V3D_FORMAT_R32G32_SFLOAT },
		}
	};

	Array1<V3DPipelineVertexLayout, 1> vertexLayouts =
	{
		{ 0, sizeof(Font::Vertex), 0, TO_UI32(vertexElements.size()) }
	};

	Array1<V3DPipelineColorBlendAttachment, 1> colorBlendAttachments =
	{
		InitializeColorBlendAttachment(BLEND_MODE_NORMAL),
	};

	V3DGraphicsPipelineDesc pipelineDesc{};

	pipelineDesc.vertexShader.pModule = m_pVertShaderModule;
	pipelineDesc.vertexShader.pEntryPointName = "main";
	pipelineDesc.fragmentShader.pModule = m_pFragShaderModule;
	pipelineDesc.fragmentShader.pEntryPointName = "main";

	pipelineDesc.vertexInput.elementCount = TO_UI32(vertexElements.size());
	pipelineDesc.vertexInput.pElements = vertexElements.data();

	pipelineDesc.vertexInput.layoutCount = TO_UI32(vertexLayouts.size());
	pipelineDesc.vertexInput.pLayouts = vertexLayouts.data();

	pipelineDesc.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineDesc.rasterization.discardEnable = false;

	pipelineDesc.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
	pipelineDesc.rasterization.cullMode = V3D_CULL_MODE_NONE;
	pipelineDesc.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
	pipelineDesc.multisample.sampleShadingEnable = false;
	pipelineDesc.multisample.minSampleShading = 0.0f;
	pipelineDesc.multisample.alphaToCoverageEnable = false;
	pipelineDesc.multisample.alphaToOneEnable = false;
	pipelineDesc.depthStencil.depthTestEnable = false;
	pipelineDesc.depthStencil.depthWriteEnable = false;
	pipelineDesc.depthStencil.depthCompareOp = V3D_COMPARE_OP_ALWAYS;

	pipelineDesc.colorBlend.logicOpEnable = false;
	pipelineDesc.colorBlend.logicOp = V3D_LOGIC_OP_CLEAR;
	pipelineDesc.colorBlend.attachmentCount = TO_UI32(colorBlendAttachments.size());
	pipelineDesc.colorBlend.pAttachments = colorBlendAttachments.data();

	pipelineDesc.pRenderPass = pRenderPass;
	pipelineDesc.subpass = subpass;

	V3D_RESULT result = m_pDevice->CreateGraphicsPipeline(m_pPipelineLayout, pipelineDesc, &m_pPipeline);
	if (result != V3D_OK)
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// スクリーンサイズを更新
	// ----------------------------------------------------------------------------------------------------

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

	// ----------------------------------------------------------------------------------------------------

	return true;
}

void Font::Reset()
{
	for (uint32_t i = 0; i < m_BlockCount; i++)
	{
		Font::Block& block = m_Blocks[i];

		block.characters.clear();
		block.vertexCount = 0;
	}

	m_BlockCount = 0;
}

bool Font::Bake()
{
	if (m_BlockCount == 0)
	{
		return true;
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックスを書き込む
	// ----------------------------------------------------------------------------------------------------

	Font::Block* pBlock = m_Blocks.data();
	Font::Block* pBlockEnd = pBlock + m_BlockCount;

	while (pBlock != pBlockEnd)
	{
		void* pMemory;

		if (pBlock->pVertexBuffer->Map(0, pBlock->pVertexBuffer->GetResourceDesc().memorySize, &pMemory) == V3D_OK)
		{
			Font::Character* pChara = pBlock->characters.data();
			Font::Character* pCharaEnd = pChara + pBlock->characters.size();

			Font::Vertex* pDst = reinterpret_cast<Font::Vertex*>(pMemory);

			while (pChara != pCharaEnd)
			{
				const Font::Glyph* pGlyph = pChara->pGlyph;

				float dx = TO_F32(pChara->dx);
				float dy = TO_F32(pChara->dy);
				float dxx = dx + pGlyph->width;
				float dyy = dy + pGlyph->height;

				const float& su = pGlyph->left;
				const float& sv = pGlyph->top;
				const float& suu = pGlyph->right;
				const float& svv = pGlyph->bottom;

				pDst[0].pos.Set(dx, dy, 0.0f, 1.0f);
				pDst[1].pos.Set(dx, dyy, 0.0f, 1.0f);
				pDst[2].pos.Set(dxx, dyy, 0.0f, 1.0f);

				pDst[3].pos.Set(dxx, dyy, 0.0f, 1.0f);
				pDst[4].pos.Set(dxx, dy, 0.0f, 1.0f);
				pDst[5].pos.Set(dx, dy, 0.0f, 1.0f);

				pDst[0].uv.Set(su, sv);
				pDst[1].uv.Set(su, svv);
				pDst[2].uv.Set(suu, svv);

				pDst[3].uv.Set(suu, svv);
				pDst[4].uv.Set(suu, sv);
				pDst[5].uv.Set(su, sv);

				pDst += 6;
				pChara++;

				pBlock->vertexCount += 6;
			}

			pBlock->pVertexBuffer->Unmap();
		}

		// ----------------------------------------------------------------------------------------------------

		pBlock++;
	}

	return true;
}

void Font::Flush(IV3DCommandBuffer* pCommandBuffer)
{
	if (m_BlockCount == 0)
	{
		return;
	}

	Font::Block* pBlock = m_Blocks.data();
	Font::Block* pBlockEnd = pBlock + m_BlockCount;

	Matrix4x4 matrix;
	matrix.x.x = FLOAT_DIV(2.0f, TO_F32(m_ScreenWidth));
	matrix.y.y = FLOAT_DIV(2.0f, TO_F32(m_ScreenHeight));
	matrix.w.x = -1.0f;
	matrix.w.y = -1.0f;

	V3DViewport viewport{};
	viewport.rect.width = m_ScreenWidth;
	viewport.rect.height = m_ScreenHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	pCommandBuffer->SetViewport(0, 1, &viewport);
	pCommandBuffer->SetScissor(0, 1, &viewport.rect);

	pCommandBuffer->PushConstant(m_pPipelineLayout, 0, &matrix);
	pCommandBuffer->BindPipeline(m_pPipeline);
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet);

	while (pBlock != pBlockEnd)
	{			
		// 描画
		pCommandBuffer->BindVertexBuffers(0, 1, &pBlock->pVertexBuffer, &pBlock->vertexBufferOffset);
		pCommandBuffer->Draw(pBlock->vertexCount, 1, 0, 0);

		pBlock++;
	}
}

bool Font::AddText(int32_t x, int32_t y, const wchar_t* pText)
{
	size_t characterCount = wcsnlen_s(pText, Font::MaxCharacter);

	int dx = x;
	int dy = y;

	Font::Block* pBlock = GetCurrentBlock();
	if (pBlock == nullptr)
	{
		pBlock = AddBlock();
		if (pBlock == nullptr)
		{
			return false;
		}
	}

	for (uint32_t i = 0; i < characterCount; i++)
	{
		wchar_t code = pText[i];

		if (code == L' ')
		{
			dx += m_HalfHeight;
		}
		else if (code == L'　')
		{
			dx += m_Height;
		}
		else if (code == L'\t')
		{
			dx += m_HalfHeight * Font::TabSize;
		}
		else if (code == L'\r')
		{
		}
		else if (code == L'\n')
		{
			dx = x;
			dy += m_Height;
		}
		else
		{
			if ((Font::FirstCode <= code) && (code <= Font::LastCode))
			{
				const Font::Glyph* pGlyph = &m_Glyphs[code - Font::FirstCode];

				Font::Character character;
				character.dx = dx + pGlyph->x;
				character.dy = dy + pGlyph->y;
				character.pGlyph = pGlyph;

				if (m_BlockCharacterMax <= pBlock->characters.size())
				{
					pBlock = AddBlock();
				}

				pBlock->characters.push_back(character);

				dx += pGlyph->cellWidth;
			}
		}
	}

	return true;
}

Font::Block* Font::GetCurrentBlock()
{
	if (m_BlockCount == 0)
	{
		return nullptr;
	}

	return &m_Blocks[m_BlockCount - 1];
}

Font::Block* Font::AddBlock()
{
	m_BlockCount++;

	if (m_Blocks.size() >= m_BlockCount)
	{
		return &m_Blocks[m_BlockCount - 1];
	}

	m_Blocks.push_back(Font::Block{});

	Font::Block* pBlock = &m_Blocks.back();

	// ----------------------------------------------------------------------------------------------------
	// バーテックスバッファを作成
	// ----------------------------------------------------------------------------------------------------

	V3DBufferDesc bufferDesc{};
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX;
	bufferDesc.size = 6 * sizeof(Font::Vertex) * m_BlockCharacterMax;

	V3D_RESULT result = m_pDevice->CreateBuffer(bufferDesc, &pBlock->pVertexBuffer);
	if (result == V3D_OK)
	{
		result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pBlock->pVertexBuffer);
		if (result != V3D_OK)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------

	return pBlock;
}
