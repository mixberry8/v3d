#pragma once

// R8G8B8 A8R8G8B8 BC1 BC3 BC5 をサポート

#include "IInputStream.h"
#include <ddraw.h>

struct DDS_PIXELFORMAT
{
	DWORD size;
	DWORD flags; //DDS_PIXELFORMAT_FLAGS
	DWORD fourCC;
	DWORD rgbBitCount;
	DWORD rBitMask;
	DWORD gBitMask;
	DWORD bBitMask;
	DWORD rgbAlphaBitMask;
};

struct DDS_CAPS
{
	DWORD caps1; //DDS_CAPS1
	DWORD caps2; //DDS_CAPS2
	DWORD reserved[2];
};

struct DDS_DESC
{
	DWORD size;
	DWORD flags; //DDS_FLAGS
	DWORD height;
	DWORD width;
	DWORD pitchOrLinearSize;
	DWORD depth;
	DWORD mipMapCount;
	DWORD reserved1[11];
	DDS_PIXELFORMAT pixelFormat;
	DDS_CAPS caps;
	DWORD reserved2;
};

static constexpr uint32_t DDS_CUBE_PLANE_TABLE[6] =
{
	DDSCAPS2_CUBEMAP_POSITIVEX,
	DDSCAPS2_CUBEMAP_NEGATIVEX,
	DDSCAPS2_CUBEMAP_POSITIVEY,
	DDSCAPS2_CUBEMAP_NEGATIVEY,
	DDSCAPS2_CUBEMAP_POSITIVEZ,
	DDSCAPS2_CUBEMAP_NEGATIVEZ,
};

static constexpr uint32_t DDS_CUBE_PLANE_COUNT = _countof(DDS_CUBE_PLANE_TABLE);

typedef V3D_RESULT(_stdcall *LoadDDSFunction)(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize);

V3D_RESULT _stdcall LoadDDS_BCn(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize);
V3D_RESULT _stdcall LoadDDS_R8G8B8(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize);
V3D_RESULT _stdcall LoadDDS_R8G8B8A8(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize);

uint64_t CalcDDSVolumeSizeBC(V3DImageDesc* pImageDesc);
uint64_t CalcDDSVolumeSizeRGBA(V3DImageDesc* pImageDesc);

// ----------------------------------------------------------------------------------------------------
// DDS の読み込み準備
// ----------------------------------------------------------------------------------------------------

V3D_RESULT _stdcall PrepareDDSImage(IInputStream* pInputStream, IV3DDevice* pDevice, V3DImageDesc* pImageDesc, V3DImageViewDesc* pImageViewDesc, uint64_t* pBufferSize)
{
	char magic[4];
	DDS_DESC desc;

	// ----------------------------------------------------------------------------------------------------
	// マジック値
	// ----------------------------------------------------------------------------------------------------

	if (pInputStream->Read(sizeof(char[4]), magic) != sizeof(char[4]))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if (::strncmp(magic, "DDS", 3) != 0)
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	// ----------------------------------------------------------------------------------------------------
	// ヘッダ
	// ----------------------------------------------------------------------------------------------------

	if (pInputStream->Read(sizeof(DDS_DESC), &desc) != sizeof(DDS_DESC))
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if (((desc.flags & DDSD_CAPS) != DDSD_CAPS) ||
		((desc.flags & DDSD_HEIGHT) != DDSD_HEIGHT) ||
		((desc.flags & DDSD_WIDTH) != DDSD_WIDTH) ||
		((desc.flags & DDSD_PIXELFORMAT) != DDSD_PIXELFORMAT))
	{
		// DDSファイルに必要なものがそろっていない
		return V3D_ERROR_NOT_SUPPORTED;
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージの記述を設定
	// ----------------------------------------------------------------------------------------------------

	pImageDesc->type = V3D_IMAGE_TYPE_1D;  // ※
	pImageDesc->format = V3D_FORMAT_UNDEFINED;  // ※
	pImageDesc->width = desc.width;
	pImageDesc->height = desc.height;
	pImageDesc->depth = 0;  // ※
	pImageDesc->levelCount = MAXIMUM(1, desc.mipMapCount);
	pImageDesc->layerCount = 0; // ※
	pImageDesc->samples = V3D_SAMPLE_COUNT_1;
	pImageDesc->tiling = V3D_IMAGE_TILING_LINEAR;
	pImageDesc->usageFlags = 0;

	pImageViewDesc->type = V3D_IMAGE_VIEW_TYPE_1D; // ※
	pImageViewDesc->baseLevel = 0;
	pImageViewDesc->levelCount = MAXIMUM(1, desc.mipMapCount);
	pImageViewDesc->baseLayer = 0;
	pImageViewDesc->layerCount = 0; // ※

	// フォーマット

	if ((desc.pixelFormat.flags & DDPF_FOURCC) == DDPF_FOURCC)
	{
		switch (desc.pixelFormat.fourCC)
		{
		case FOURCC_DXT1: //BC1 ( Width * Height / 2 ) 1/8
			pImageDesc->format = V3D_FORMAT_BC1_RGB_UNORM;
			break;
		case FOURCC_DXT3: //BC2 ( Width * Height ) 1/4
			pImageDesc->format = V3D_FORMAT_BC3_UNORM;
			break;
		case FOURCC_DXT5: //BC3 ( Width * Height ) 1/4
			pImageDesc->format = V3D_FORMAT_BC5_UNORM;
			break;

		default:
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}
	else
	{
		if ((desc.pixelFormat.flags & DDPF_RGB) == DDPF_RGB)
		{
			bool supportedFormat = false;

			if ((desc.pixelFormat.flags & DDPF_ALPHAPIXELS) == DDPF_ALPHAPIXELS)
			{
				if ((desc.pixelFormat.rgbAlphaBitMask == 0xFF000000) &&
					(desc.pixelFormat.rBitMask == 0x00FF0000) &&
					(desc.pixelFormat.gBitMask == 0x0000FF00) &&
					(desc.pixelFormat.bBitMask == 0x000000FF))
				{
					supportedFormat = true;
				}
			}
			else
			{
				if ((desc.pixelFormat.rBitMask == 0x00FF0000) &&
					(desc.pixelFormat.gBitMask == 0x0000FF00) &&
					(desc.pixelFormat.bBitMask == 0x000000FF))
				{
					supportedFormat = true;
				}
			}

			if (supportedFormat == false)
			{
				return V3D_ERROR_NOT_SUPPORTED;
			}

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
		}
		else
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}

	// 残りのイメージの記述を設定

	if ((desc.caps.caps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP)
	{
		pImageDesc->type = V3D_IMAGE_TYPE_2D;
		pImageDesc->depth = 1;
		pImageDesc->layerCount = 6;
		pImageDesc->usageFlags = V3D_IMAGE_USAGE_CUBE_COMPATIBLE;

		pImageViewDesc->type = V3D_IMAGE_VIEW_TYPE_CUBE;
		pImageViewDesc->layerCount = 6;
	}
	else if ((desc.caps.caps2 & DDSCAPS2_VOLUME) == DDSCAPS2_VOLUME)
	{
		pImageDesc->type = V3D_IMAGE_TYPE_3D;
		pImageDesc->depth = desc.depth;
		pImageDesc->layerCount = 1;

		pImageViewDesc->type = V3D_IMAGE_VIEW_TYPE_3D;
		pImageViewDesc->layerCount = 1;
	}
	else
	{
		pImageDesc->type = V3D_IMAGE_TYPE_2D;
		pImageDesc->depth = 1;
		pImageDesc->layerCount = 1;

		pImageViewDesc->type = V3D_IMAGE_VIEW_TYPE_2D;
		pImageViewDesc->layerCount = 1;
	}

	// ----------------------------------------------------------------------------------------------------
	// バッファのサイズを求める
	// ----------------------------------------------------------------------------------------------------

	if (desc.flags & DDSD_PITCH)
	{
//		uint64_t pitch = desc.pitchOrLinearSize;
		uint64_t pitch = pImageDesc->width << 2;
		uint64_t height = pImageDesc->height;
		uint64_t depth = max(1, pImageDesc->depth);

		*pBufferSize = 0;

		for (uint32_t i = 0; i < pImageDesc->levelCount; i++)
		{
			*pBufferSize += pitch * height * depth;

			pitch = max(1, pitch >> 1);
			height = max(1, height >> 1);
		}
	}
	else if (desc.flags & DDSD_LINEARSIZE)
	{
		*pBufferSize = (desc.mipMapCount > 1) ? (desc.pitchOrLinearSize * 2) : desc.pitchOrLinearSize;
	}
	else
	{
		if ((desc.pixelFormat.flags & DDPF_FOURCC) == DDPF_FOURCC)
		{
			*pBufferSize = 0;

			if ((desc.caps.caps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP)
			{
				for (uint32_t i = 0; i < DDS_CUBE_PLANE_COUNT; i++)
				{
					if ((desc.caps.caps2 & DDS_CUBE_PLANE_TABLE[i]) == DDS_CUBE_PLANE_TABLE[i])
					{
						*pBufferSize += CalcDDSVolumeSizeBC(pImageDesc);
					}
				}
			}
			else
			{
				*pBufferSize = CalcDDSVolumeSizeBC(pImageDesc);
			}
		}
		else
		{
			if ((desc.caps.caps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP)
			{
				*pBufferSize = 0;

				for (uint32_t i = 0; i < DDS_CUBE_PLANE_COUNT; i++)
				{
					if ((desc.caps.caps2 & DDS_CUBE_PLANE_TABLE[i]) == DDS_CUBE_PLANE_TABLE[i])
					{
						*pBufferSize += CalcDDSVolumeSizeRGBA(pImageDesc);
					}
				}
			}
			else
			{
				*pBufferSize = CalcDDSVolumeSizeRGBA(pImageDesc);
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

// ----------------------------------------------------------------------------------------------------
// DDS の読み込み
// ----------------------------------------------------------------------------------------------------

V3D_RESULT _stdcall LoadDDSImage(IInputStream* pInputStream, V3D_FORMAT format, uint64_t memorySize, void* pMemory, std::vector<V3DCopyBufferToImageRange>& copyRanges)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// マジック値
	////////////////////////////////////////////////////////////////////////////////////////////////////

	char magic[4];

	if (pInputStream->Read(sizeof(char[4]), magic) != sizeof(char[4]))
	{
		return V3D_ERROR_FAIL;
	}

	if (::strncmp(magic, "DDS", 3) != 0)
	{
		return V3D_ERROR_FAIL;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ヘッダ
	////////////////////////////////////////////////////////////////////////////////////////////////////

	DDS_DESC desc;

	if (pInputStream->Read(sizeof(DDS_DESC), &desc) != sizeof(DDS_DESC))
	{
		return V3D_ERROR_FAIL;
	}

	if (((desc.flags & DDSD_CAPS) != DDSD_CAPS) ||
		((desc.flags & DDSD_HEIGHT) != DDSD_HEIGHT) ||
		((desc.flags & DDSD_WIDTH) != DDSD_WIDTH) ||
		((desc.flags & DDSD_PIXELFORMAT) != DDSD_PIXELFORMAT))
	{
		// DDSファイルに必要なものがそろっていない
		return V3D_ERROR_FAIL;
	}

	if ((desc.flags & DDSD_MIPMAPCOUNT) != DDSD_MIPMAPCOUNT)
	{
		// ミップマップが 0 の場合はサブリースの確保のため最低 1 にしておく。
		desc.mipMapCount = 1;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// フォーマットとロードファンクションの決定
	////////////////////////////////////////////////////////////////////////////////////////////////////

	LoadDDSFunction loadFunc = nullptr;

	if ((desc.pixelFormat.flags & DDPF_FOURCC) == DDPF_FOURCC)
	{
		switch (desc.pixelFormat.fourCC)
		{
		case FOURCC_DXT1:
		case FOURCC_DXT3:
		case FOURCC_DXT5:
			loadFunc = LoadDDS_BCn;
			break;

		default:
			return V3D_ERROR_FAIL;
		}
	}
	else
	{
		if ((desc.pixelFormat.flags & DDPF_RGB) == DDPF_RGB)
		{
			if ((desc.pixelFormat.flags & DDPF_ALPHAPIXELS) == DDPF_ALPHAPIXELS)
			{
				if ((desc.pixelFormat.rgbAlphaBitMask == 0xFF000000) &&
					(desc.pixelFormat.rBitMask == 0x00FF0000) &&
					(desc.pixelFormat.gBitMask == 0x0000FF00) &&
					(desc.pixelFormat.bBitMask == 0x000000FF))
				{
					loadFunc = LoadDDS_R8G8B8A8;
				}
			}
			else
			{
				if ((desc.pixelFormat.rBitMask == 0x00FF0000) &&
					(desc.pixelFormat.gBitMask == 0x0000FF00) &&
					(desc.pixelFormat.bBitMask == 0x000000FF))
				{
					loadFunc = LoadDDS_R8G8B8;
				}
			}
		}
		else
		{
			//(desc.pixelFormat.flags & DDPF_ALPHA
			return V3D_ERROR_FAIL;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ロード
	////////////////////////////////////////////////////////////////////////////////////////////////////

	V3D_RESULT result;
	uint64_t srcReadSize = 0;
	uint64_t srcBufferOffset = 0;

	if ((desc.caps.caps2 & DDSCAPS2_CUBEMAP) == DDSCAPS2_CUBEMAP)
	{
		uint32_t flags = 0;

		result = V3D_OK;

		for (uint32_t i = 0; (i < DDS_CUBE_PLANE_COUNT) && (result == V3D_OK); i++)
		{
			if ((desc.caps.caps2 & DDS_CUBE_PLANE_TABLE[i]) == DDS_CUBE_PLANE_TABLE[i])
			{
				for (uint32_t j = 0; j < desc.mipMapCount; j++)
				{
					V3DCopyBufferToImageRange range;
					range.dstImageSubresource.level = j;
					range.dstImageSubresource.baseLayer = i;
					range.dstImageSubresource.layerCount = 1;
					range.dstImageOffset.x = 0;
					range.dstImageOffset.y = 0;
					range.dstImageOffset.z = 0;
					range.srcBufferOffset = srcBufferOffset;

					uint8_t* pBuffer = static_cast<uint8_t*>(pMemory) + srcBufferOffset;

					result = loadFunc(pInputStream, format, desc, j, 1, pBuffer, &srcReadSize, &range.dstImageSize);
					if (result == V3D_OK)
					{
						flags |= DDS_CUBE_PLANE_TABLE[i];
					}

					srcBufferOffset += srcReadSize;

					copyRanges.push_back(range);
				}
			}
		}

		if (flags == 0)
		{
			result = V3D_ERROR_FAIL;
		}
	}
	else if ((desc.caps.caps2 & DDSCAPS2_VOLUME) == DDSCAPS2_VOLUME)
	{
		DWORD sliceCount = desc.depth;

		for (uint32_t i = 0; i < desc.mipMapCount; i++)
		{
			V3DCopyBufferToImageRange range;
			range.dstImageSubresource.level = i;
			range.dstImageSubresource.baseLayer = 0;
			range.dstImageSubresource.layerCount = 1;
			range.dstImageOffset.x = 0;
			range.dstImageOffset.y = 0;
			range.dstImageOffset.z = 0;
			range.srcBufferOffset = srcBufferOffset;

			uint8_t* pBuffer = static_cast<uint8_t*>(pMemory) + srcBufferOffset;

			result = loadFunc(pInputStream, format, desc, i, sliceCount, pBuffer, &srcReadSize, &range.dstImageSize);
			if (result != V3D_OK)
			{
				break;
			}

			sliceCount = max(1, sliceCount >> 1);

			srcBufferOffset += srcReadSize;

			copyRanges.push_back(range);
		}
	}
	else
	{
		for (uint32_t i = 0; i < desc.mipMapCount; i++)
		{
			V3DCopyBufferToImageRange range{};
			range.dstImageSubresource.level = i;
			range.dstImageSubresource.baseLayer = 0;
			range.dstImageSubresource.layerCount = 1;
			range.dstImageOffset.x = 0;
			range.dstImageOffset.y = 0;
			range.dstImageOffset.z = 0;
			range.srcBufferOffset = srcBufferOffset;

			uint8_t* pBuffer = static_cast<uint8_t*>(pMemory) + srcBufferOffset;

			result = loadFunc(pInputStream, format, desc, i, 1, pBuffer, &srcReadSize, &range.dstImageSize);
			if (result != V3D_OK)
			{
				break;
			}

			srcBufferOffset += srcReadSize;

			copyRanges.push_back(range);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	return result;
}

// ----------------------------------------------------------------------------------------------------
// DDS ( BC 1 2 3 ) の読み込み
// ----------------------------------------------------------------------------------------------------

V3D_RESULT _stdcall LoadDDS_BCn(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize)
{
	uint32_t width;
	uint32_t height;

	if (level > 0)
	{
		width = max(1, desc.width >> level);
		height = max(1, desc.height >> level);
	}
	else
	{
		width = desc.width;
		height = desc.height;
	}

	unsigned int blockSize;

	switch (desc.pixelFormat.fourCC)
	{
	case FOURCC_DXT1: //BC1 ( Width * Height / 2 ) 1/8
		blockSize = 8;
		break;
	case FOURCC_DXT3: //BC2 ( Width * Height ) 1/4
		blockSize = 16;
		break;
	case FOURCC_DXT5: //BC3 ( Width * Height ) 1/4
		blockSize = 16;
		break;

	default:
		return V3D_ERROR_FAIL;
	}

	uint64_t rowPitch = max(1, width >> 2) * blockSize;
	uint64_t depthPitch = rowPitch * max(1, height >> 2);
	uint64_t size = depthPitch * depth;

	uint64_t readSize = pInputStream->Read(size, pSrcBuffer);
	if (readSize != size)
	{
		return V3D_ERROR_FAIL;
	}

	*pSrcReadSize = size;

	pDstSize->width = width;
	pDstSize->height = height;
	pDstSize->depth = depth;

	return V3D_OK;
}

V3D_RESULT _stdcall LoadDDS_R8G8B8(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize)
{
	unsigned int width;
	unsigned int height;

	if (level > 0)
	{
		width = desc.width >> level;
		height = desc.height >> level;
	}
	else
	{
		width = desc.width;
		height = desc.height;
	}

	uint64_t srcRowPitch = width * 3;
	uint64_t srcDepthPitch = srcRowPitch * height;
	uint64_t srcSize = srcDepthPitch * depth;

	uint64_t dstRowPitch = width << 2;
	uint64_t dstDepthPitch = dstRowPitch * height;
	uint64_t dstSize = dstDepthPitch * depth;

	uint8_t* pRow = pSrcBuffer;

	for (uint32_t y = 0; y < height; y++)
	{
		uint64_t readSize = pInputStream->Read(srcRowPitch, pRow);
		if (readSize != srcRowPitch)
		{
			return V3D_ERROR_FAIL;
		}

		uint8_t* pSrcEnd = pRow;
		uint8_t* pSrc = pSrcEnd + srcRowPitch - 1;
		uint8_t* pDst = pRow + dstRowPitch - 1;

		if (format == V3D_FORMAT_B8G8R8A8_UNORM)
		{
			while (pSrcEnd < pSrc)
			{
				uint8_t b = *pSrc--;
				uint8_t g = *pSrc--;
				uint8_t r = *pSrc--;

				*pDst-- = 0xFF;
				*pDst-- = b;
				*pDst-- = g;
				*pDst-- = r;
			}
		}
		else if(format == V3D_FORMAT_A8B8G8R8_UNORM)
		{
			while (pSrcEnd < pSrc)
			{
				uint8_t b = *pSrc--;
				uint8_t g = *pSrc--;
				uint8_t r = *pSrc--;

				*pDst-- = 0xFF;
				*pDst-- = r;
				*pDst-- = g;
				*pDst-- = b;
			}
		}
		else if (format == V3D_FORMAT_R8G8B8A8_UNORM)
		{
			while (pSrcEnd < pSrc)
			{
				uint8_t b = *pSrc--;
				uint8_t g = *pSrc--;
				uint8_t r = *pSrc--;

				*pDst-- = 0xFF;
				*pDst-- = r;
				*pDst-- = g;
				*pDst-- = b;
			}
		}

		pRow += dstRowPitch;
	}

	*pSrcReadSize = dstSize;

	pDstSize->width = width;
	pDstSize->height = height;
	pDstSize->depth = depth;

	return V3D_OK;
}

V3D_RESULT _stdcall LoadDDS_R8G8B8A8(IInputStream* pInputStream, V3D_FORMAT format, const DDS_DESC& desc, uint32_t level, uint32_t depth, uint8_t* pSrcBuffer, uint64_t* pSrcReadSize, V3DSize3D* pDstSize)
{
	unsigned int width;
	unsigned int height;

	if (level > 0)
	{
		width = desc.width >> level;
		height = desc.height >> level;
	}
	else
	{
		width = desc.width;
		height = desc.height;
	}

	uint64_t rowPitch = width << 2;
	uint64_t depthPitch = rowPitch * height;
	uint64_t size = depthPitch * depth;

	uint64_t readSize = pInputStream->Read(size, pSrcBuffer);
	if (readSize != size)
	{
		return V3D_ERROR_FAIL;
	}

	if (format == V3D_FORMAT_B8G8R8A8_UNORM)
	{
		uint8_t* pData = pSrcBuffer;
		uint8_t* pDataEnd = pData + size;
		while (pData != pDataEnd)
		{
			uint32_t b = pData[0];
			uint32_t g = pData[1];
			uint32_t r = pData[2];
			uint32_t a = pData[3];

			*pData++ = b;
			*pData++ = g;
			*pData++ = r;
			*pData++ = a;
		}
	}
	else if (format == V3D_FORMAT_A8B8G8R8_UNORM)
	{
		uint8_t* pData = pSrcBuffer;
		uint8_t* pDataEnd = pData + size;
		while (pData != pDataEnd)
		{
			uint32_t b = pData[0];
			uint32_t g = pData[1];
			uint32_t r = pData[2];
			uint32_t a = pData[3];

			*pData++ = r;
			*pData++ = g;
			*pData++ = b;
			*pData++ = a;
		}
	}
	else if (format == V3D_FORMAT_R8G8B8A8_UNORM)
	{
		uint8_t* pData = pSrcBuffer;
		uint8_t* pDataEnd = pData + size;
		while (pData != pDataEnd)
		{
			uint32_t b = pData[0];
			uint32_t g = pData[1];
			uint32_t r = pData[2];
			uint32_t a = pData[3];

			*pData++ = r;
			*pData++ = g;
			*pData++ = b;
			*pData++ = a;
		}
	}

	*pSrcReadSize = size;

	pDstSize->width = width;
	pDstSize->height = height;
	pDstSize->depth = depth;

	return V3D_OK;
}

uint64_t CalcDDSVolumeSizeBC(V3DImageDesc* pImageDesc)
{
	uint32_t width = pImageDesc->width;
	uint32_t height = pImageDesc->height;
	uint64_t bufferSize = 0;

	for (uint32_t i = 0; i < pImageDesc->levelCount; i++)
	{
		unsigned int blockSize;

		switch (pImageDesc->format)
		{
		case V3D_FORMAT_BC1_RGB_UNORM:
			blockSize = 8;
			break;
		case V3D_FORMAT_BC3_UNORM:
			blockSize = 16;
			break;
		case V3D_FORMAT_BC5_UNORM:
			blockSize = 16;
			break;

		default:
			return V3D_ERROR_FAIL;
		}

		uint64_t rowPitch = max(1, width >> 2) * blockSize;
		uint64_t depthPitch = rowPitch * max(1, height >> 2);
		uint64_t size = depthPitch * pImageDesc->depth;

		bufferSize += size;

		width = MAXIMUM(1, width >> 1);
		height = MAXIMUM(1, height >> 1);
	}

	return bufferSize;
}

uint64_t CalcDDSVolumeSizeRGBA(V3DImageDesc* pImageDesc)
{
	uint32_t width = pImageDesc->width;
	uint32_t height = pImageDesc->height;
	uint64_t bufferSize = 0;

	for (uint32_t i = 0; i < pImageDesc->levelCount; i++)
	{
		uint64_t rowPitch = width * 4;
		uint64_t depthPitch = rowPitch * height;
		uint64_t size = depthPitch * pImageDesc->depth;

		bufferSize += size;

		width = MAXIMUM(1, width >> 1);
		height = MAXIMUM(1, height >> 1);
	}

	return bufferSize;
}
