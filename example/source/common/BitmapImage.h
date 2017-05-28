#pragma once

#include "IInputStream.h"

V3D_RESULT _stdcall PrepareBitmapImage(IInputStream* pInputStream, IV3DDevice* pDevice, V3DImageDesc* pImageDesc, V3DImageViewDesc* pImageViewDesc, uint64_t* pBufferSize)
{
	// ----------------------------------------------------------------------------------------------------
	// イメージのチェック
	// ----------------------------------------------------------------------------------------------------

	// ファイルヘッダ
	BITMAPFILEHEADER fileHeader;

	if (pInputStream->Read(sizeof(BITMAPFILEHEADER), &fileHeader) != sizeof(BITMAPFILEHEADER))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if (fileHeader.bfType != ('B' | ('M' << 8)))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	// 情報ヘッダ
	BITMAPV5HEADER infoHeader;

	// 最初 BITMAPV5HEADER::bV5Size だけ読み込む
	if (pInputStream->Read(sizeof(DWORD), &infoHeader) != sizeof(DWORD))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	// ヘッダサイズのチェック
	if (infoHeader.bV5Size == sizeof(BITMAPCOREHEADER))
	{
		// BITMAPCOREHEADER

		BITMAPCOREHEADER temp;
		uint8_t* pHeader = reinterpret_cast<uint8_t*>(&temp) + sizeof(DWORD);
		uint64_t headerSize = infoHeader.bV5Size - sizeof(DWORD);

		if (pInputStream->Read(headerSize, pHeader) != headerSize)
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}

		infoHeader.bV5BitCount = temp.bcBitCount;
	}
	else if (infoHeader.bV5Size > sizeof(BITMAPCOREHEADER))
	{
		// BITMAPINFOHEADER or BITMAPV4HEADER or BITMAPV5HEADER

		uint8_t* pHeader = reinterpret_cast<unsigned char*>(&infoHeader) + sizeof(DWORD);
		uint64_t headerSize = infoHeader.bV5Size - sizeof(DWORD);

		if (fileHeader.bfOffBits < (sizeof(BITMAPFILEHEADER) + infoHeader.bV5Size))
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}

		if (pInputStream->Read(headerSize, pHeader) != headerSize)
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}
	else
	{
		//不明な情報ヘッダ
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if (infoHeader.bV5BitCount < 24)
	{
		// 24、32 Bit のみサポート
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if (infoHeader.bV5Size >= sizeof(BITMAPINFOHEADER))
	{
		// BITMAPV4HEADER or BITMAPV5HEADER

		if (infoHeader.bV5Compression == BI_BITFIELDS)
		{
			if ((infoHeader.bV5AlphaMask != 0x00000000) &&
				(infoHeader.bV5AlphaMask != 0x000000FF))
			{
				// アルファチャンネルが 8Bit のみサポート
				return V3D_ERROR_NOT_SUPPORTED;
			}

			if ((infoHeader.bV5BlueMask != 0x0000FF00) ||
				(infoHeader.bV5GreenMask != 0x00FF0000) ||
				(infoHeader.bV5RedMask != 0xFF000000))
			{
				// 各チャンネルが 8Bit の RGB のみサポート
				return V3D_ERROR_NOT_SUPPORTED;
			}
		}
		else if (infoHeader.bV5Compression == BI_RGB)
		{
			// BITMAPV5HEADER
			;
		}
		else
		{
			//不明なコンプレッション
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}
	else
	{
		// BITMAPCOREHEADER or BITMAPINFOHEADER
		;
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージの記述を作成
	// ----------------------------------------------------------------------------------------------------

	pImageDesc->type = V3D_IMAGE_TYPE_2D;
	pImageDesc->format = V3D_FORMAT_UNDEFINED; // ※
	pImageDesc->width = infoHeader.bV5Width;
	pImageDesc->height = infoHeader.bV5Height;
	pImageDesc->depth = 1;
	pImageDesc->levelCount = 1;
	pImageDesc->layerCount = 1;
	pImageDesc->samples = V3D_SAMPLE_COUNT_1;
	pImageDesc->tiling = V3D_IMAGE_TILING_OPTIMAL;
	pImageDesc->usageFlags = 0;

	pImageDesc->format = V3D_FORMAT_B8G8R8A8_UNORM;
	if (pDevice->CheckImageFormatFeature(pImageDesc->format, pImageDesc->tiling, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
	{
		pImageDesc->format = V3D_FORMAT_A8B8G8R8_UNORM;
		if (pDevice->CheckImageFormatFeature(pImageDesc->format, pImageDesc->tiling, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
		{
			pImageDesc->format = V3D_FORMAT_R8G8B8A8_UNORM;
			if (pDevice->CheckImageFormatFeature(pImageDesc->format, pImageDesc->tiling, V3D_IMAGE_FORMAT_FEATURE_SAMPLED) != V3D_OK)
			{
				return V3D_ERROR_NOT_SUPPORTED;
			}
		}
	}

	pImageViewDesc->type = V3D_IMAGE_VIEW_TYPE_2D;
	pImageViewDesc->baseLevel = 0;
	pImageViewDesc->levelCount = 1;
	pImageViewDesc->baseLayer = 0;
	pImageViewDesc->layerCount = 1;

	*pBufferSize = 4 * infoHeader.bV5Width * infoHeader.bV5Height;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT _stdcall LoadBitmapImage(IInputStream* pInputStream, V3D_FORMAT format , uint64_t memorySize, void* pMemory, std::vector<V3DCopyBufferToImageRange>& copyRanges)
{
	// ----------------------------------------------------------------------------------------------------
	// イメージのチェック
	// ----------------------------------------------------------------------------------------------------

	// ファイルヘッダ
	BITMAPFILEHEADER fileHeader;

	if (pInputStream->Read(sizeof(BITMAPFILEHEADER), &fileHeader) != sizeof(BITMAPFILEHEADER))
	{
		return V3D_ERROR_FAIL;
	}

	if (fileHeader.bfType != ('B' | ('M' << 8)))
	{
		return V3D_ERROR_FAIL;
	}

	// 情報ヘッダ
	BITMAPV5HEADER infoHeader{};

	// 最初 BITMAPV5HEADER::bV5Size だけ読み込む
	if (pInputStream->Read(sizeof(DWORD), &infoHeader) != sizeof(DWORD))
	{
		return V3D_ERROR_FAIL;
	}

	if (infoHeader.bV5Size == sizeof(BITMAPCOREHEADER))
	{
		// BITMAPCOREHEADER

		BITMAPCOREHEADER temp;
		unsigned char* pHeader = reinterpret_cast<unsigned char*>(&temp) + sizeof(DWORD);
		unsigned int headerSize = infoHeader.bV5Size - sizeof(DWORD);

		if (pInputStream->Read(headerSize, pHeader) != headerSize)
		{
			return V3D_ERROR_FAIL;
		}

		infoHeader.bV5Width = temp.bcWidth;
		infoHeader.bV5Height = temp.bcHeight;
		infoHeader.bV5Planes = temp.bcPlanes;
		infoHeader.bV5BitCount = temp.bcBitCount;
	}
	else if (infoHeader.bV5Size > sizeof(BITMAPCOREHEADER))
	{
		// BITMAPINFOHEADER or BITMAPV4HEADER or BITMAPV5HEADER

		unsigned char* pHeader = reinterpret_cast<unsigned char*>(&infoHeader) + sizeof(DWORD);
		unsigned int headerSize = infoHeader.bV5Size - sizeof(DWORD);

		if (fileHeader.bfOffBits < (sizeof(BITMAPFILEHEADER) + infoHeader.bV5Size))
		{
			return V3D_ERROR_FAIL;
		}

		if (pInputStream->Read(headerSize, pHeader) != headerSize)
		{
			return V3D_ERROR_FAIL;
		}
	}
	else
	{
		//不明な情報ヘッダ
		return V3D_ERROR_FAIL;
	}

	if (infoHeader.bV5BitCount < 24)
	{
		// 24、32 Bit のみサポート
		return V3D_ERROR_FAIL;
	}

	if (infoHeader.bV5Size >= sizeof(BITMAPINFOHEADER))
	{
		// BITMAPV4HEADER or BITMAPV5HEADER

		if (infoHeader.bV5Compression == BI_BITFIELDS)
		{
			if ((infoHeader.bV5AlphaMask != 0x00000000) &&
				(infoHeader.bV5AlphaMask != 0x000000FF))
			{
				// アルファチャンネルが 8Bit のみサポート
				return V3D_ERROR_FAIL;
			}

			if ((infoHeader.bV5BlueMask != 0x0000FF00) ||
				(infoHeader.bV5GreenMask != 0x00FF0000) ||
				(infoHeader.bV5RedMask != 0xFF000000))
			{
				// 各チャンネルが 8Bit の RGB のみサポート
				return V3D_ERROR_FAIL;
			}
		}
		else if (infoHeader.bV5Compression == BI_RGB)
		{
			// チャンネルの確認用
			infoHeader.bV5AlphaMask = 0x00000000;
			infoHeader.bV5BlueMask = 0x0000FF00;
			infoHeader.bV5GreenMask = 0x00FF0000;
			infoHeader.bV5RedMask = 0xFF000000;
		}
		else
		{
			//不明なコンプレッション
			return V3D_ERROR_FAIL;
		}
	}
	else
	{
		// BITMAPCOREHEADER or BITMAPINFOHEADER

		// チャンネルの確認用
		infoHeader.bV5AlphaMask = 0x00000000;
		infoHeader.bV5BlueMask = 0x0000FF00;
		infoHeader.bV5GreenMask = 0x00FF0000;
		infoHeader.bV5RedMask = 0xFF000000;
	}

	pInputStream->Seek(SEEK_METHOD_CURRENT, fileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - infoHeader.bV5Size);

	// ----------------------------------------------------------------------------------------------------
	// イメージの読み込み
	// ----------------------------------------------------------------------------------------------------

	uint64_t imageRowPitch = 4 * infoHeader.bV5Width;
	uint8_t* pImageMemory = reinterpret_cast<uint8_t*>(pMemory) + (imageRowPitch * (infoHeader.bV5Height - 1));

	if (infoHeader.bV5BitCount == 24)
	{
		uint64_t srcOffset = infoHeader.bV5Width % 4;
		uint64_t srcPitch = infoHeader.bV5Width * 3 + srcOffset;

		for (LONG y = 0; y < infoHeader.bV5Height; y++)
		{
			uint64_t readResult = pInputStream->Read(srcPitch, pImageMemory);
			if (srcPitch != readResult)
			{
				return V3D_ERROR_FAIL;
			}

			const uint8_t* pSrcPixelEnd = reinterpret_cast<uint8_t*>(pImageMemory);
			const uint8_t* pSrcPixel = pSrcPixelEnd + srcPitch - srcOffset - 1;

			uint32_t* pDstPixel = reinterpret_cast<uint32_t*>(pImageMemory) + infoHeader.bV5Width - 1;

			while (pSrcPixelEnd < pSrcPixel)
			{
				uint32_t b = *pSrcPixel--;
				uint32_t g = *pSrcPixel--;
				uint32_t r = *pSrcPixel--;

				*pDstPixel-- = (0xFF000000) | (b << 16) | (g << 8) + r;
			}

			pImageMemory -= imageRowPitch;
		}
	}
	else if (infoHeader.bV5BitCount == 32)
	{
		uint64_t srcPitch = infoHeader.bV5Width * 4;

		for (LONG y = 0; y < infoHeader.bV5Height; y++)
		{
			uint64_t readResult = pInputStream->Read(srcPitch, pImageMemory);
			if (srcPitch != readResult)
			{
				return V3D_ERROR_FAIL;
			}

			const uint8_t* pSrcPixel = reinterpret_cast<uint8_t*>(pImageMemory);
			const uint8_t* pSrcPixelEnd = pSrcPixel + srcPitch;

			uint32_t* pDstPixel = reinterpret_cast<uint32_t*>(pImageMemory);

			if (infoHeader.bV5AlphaMask == 0)
			{
				if (format == V3D_FORMAT_B8G8R8A8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						pSrcPixel++;

						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (0xFF000000) | (b << 16) | (g << 8) + r;
					}
				}
				else if (format == V3D_FORMAT_A8B8G8R8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						pSrcPixel++;

						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (0xFF000000) | (r << 16) | (g << 8) + b;
					}
				}
				else if (format == V3D_FORMAT_R8G8B8A8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						pSrcPixel++;

						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (0xFF000000) | (r << 16) | (g << 8) + b;
					}
				}
			}
			else
			{
				if (format == V3D_FORMAT_B8G8R8A8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						uint32_t a = *pSrcPixel++;
						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (a << 24) | (b << 16) | (g << 8) + r;
					}
				}
				else if (format == V3D_FORMAT_A8B8G8R8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						uint32_t a = *pSrcPixel++;
						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (a << 24) | (r << 16) | (g << 8) + b;
					}
				}
				else if (format == V3D_FORMAT_R8G8B8A8_UNORM)
				{
					while (pSrcPixel != pSrcPixelEnd)
					{
						uint32_t a = *pSrcPixel++;
						uint32_t r = *pSrcPixel++;
						uint32_t g = *pSrcPixel++;
						uint32_t b = *pSrcPixel++;

						*pDstPixel++ = (a << 24) | (r << 16) | (g << 8) + b;
					}
				}
			}

			pImageMemory -= imageRowPitch;
		}
	}

	V3DCopyBufferToImageRange copyRange;
	copyRange.dstImageSubresource.level = 0;
	copyRange.dstImageSubresource.baseLayer = 0;
	copyRange.dstImageSubresource.layerCount = 1;
	copyRange.dstImageOffset.x = 0;
	copyRange.dstImageOffset.y = 0;
	copyRange.dstImageOffset.z = 0;
	copyRange.dstImageSize.width = infoHeader.bV5Width;
	copyRange.dstImageSize.height = infoHeader.bV5Height;
	copyRange.dstImageSize.depth = 1;
	copyRange.srcBufferOffset = 0;

	copyRanges.push_back(copyRange);

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}
