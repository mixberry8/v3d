#include "common.h"
#include <iostream>
#include "MemoryInputStream.h"
#include "BitmapImage.h"
#include "DDSImage.h"

// ----------------------------------------------------------------------------------------------------
// シェーダー
// ----------------------------------------------------------------------------------------------------

V3D_RESULT CompileShaderFromFile(V3DFlags flags, const wchar_t* pSrcFilePath, const wchar_t* pDstFilePath)
{
	if ((flags == 0) || (pDstFilePath == nullptr) || (pSrcFilePath == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	// ----------------------------------------------------------------------------------------------------
	// コマンドライン
	// ----------------------------------------------------------------------------------------------------

	wchar_t sdkDirPath[2048];
	if (ExpandEnvironmentStrings(L"%VK_SDK_PATH%", sdkDirPath, sizeof(sdkDirPath) >> 1) == 0)
	{
		return V3D_ERROR_FAIL;
	}

	wchar_t compileOptions[2048]{};
	if (flags & COMPILE_SHADER_SPIR_V_BINARY_VULKAN) { ::wcscat_s(compileOptions, L" -V"); }
	if (flags & COMPILE_SHADER_SPIR_V_BINARY_OPENGL) { ::wcscat_s(compileOptions, L" -G"); }
	if (flags & COMPILE_SHADER_INPUT_HLSL) { ::wcscat_s(compileOptions, L" -D"); }

	wchar_t compilerFilePath[2048];
	//	wsprintf(compilerFilePath, L"%s\\Bin\\glslangValidator.exe %s -e \"%s\" -o \"%s\" \"%s\"", sdkDirPath, compileOptions, pEntryPointName, pDstFilePath, pSrcFilePath);
	wsprintf(compilerFilePath, L"%s\\Bin\\glslangValidator.exe %s -o \"%s\" \"%s\"", sdkDirPath, compileOptions, pDstFilePath, pSrcFilePath);

	// ----------------------------------------------------------------------------------------------------
	// 実行結果の出力用のテンポラリファイルパス
	// ----------------------------------------------------------------------------------------------------

	wchar_t tempDirPath[2048];
	if (GetTempPath(sizeof(tempDirPath) >> 1, tempDirPath) == 0)
	{
		return V3D_ERROR_FAIL;
	}

	wchar_t outputFilePath[2048];
	if (GetTempFileName(tempDirPath, L"", 0, outputFilePath) == 0)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// 実行
	// ----------------------------------------------------------------------------------------------------

	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	HANDLE outputFileHandle = CreateFile(outputFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (outputFileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	STARTUPINFO startupInfo{};
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = stdin;
	startupInfo.hStdOutput = outputFileHandle;
	startupInfo.hStdError = outputFileHandle;

	PROCESS_INFORMATION procInfo{};

	if (CreateProcess(nullptr, compilerFilePath, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &procInfo) == FALSE)
	{
		CloseHandle(outputFileHandle);
		return V3D_ERROR_FAIL;
	}

	DWORD objectState = WaitForSingleObject(procInfo.hProcess, INFINITE);
	if (objectState != WAIT_OBJECT_0)
	{
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(outputFileHandle);
		DeleteFile(outputFilePath);
		return V3D_ERROR_FAIL;
	}

	DWORD result;
	if (GetExitCodeProcess(procInfo.hProcess, &result) == FALSE)
	{
		CloseHandle(procInfo.hProcess);
		CloseHandle(procInfo.hThread);
		CloseHandle(outputFileHandle);
		DeleteFile(outputFilePath);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(procInfo.hProcess);
	CloseHandle(procInfo.hThread);

	// ----------------------------------------------------------------------------------------------------
	// テンポラリファイルに出力された実行結果を読み込む
	// ----------------------------------------------------------------------------------------------------

	SetFilePointer(outputFileHandle, 0, 0, FILE_BEGIN);

	uint32_t fileSize = GetFileSize(outputFileHandle, 0);
	if (fileSize == 0xFFFFFFFF)
	{
		CloseHandle(outputFileHandle);
		DeleteFile(outputFilePath);
		return V3D_ERROR_FAIL;
	}

	char* pOutputBuffer = static_cast<char*>(GlobalAlloc(0, fileSize + 2));
	if (pOutputBuffer == nullptr)
	{
		CloseHandle(outputFileHandle);
		DeleteFile(outputFilePath);
		return V3D_ERROR_FAIL;
	}

	DWORD readSize;
	if (ReadFile(outputFileHandle, pOutputBuffer, fileSize, &readSize, 0) == FALSE)
	{
		GlobalFree(pOutputBuffer);
		CloseHandle(outputFileHandle);
		DeleteFile(outputFilePath);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(outputFileHandle);
	DeleteFile(outputFilePath);

	pOutputBuffer[readSize] = 0;
	pOutputBuffer[readSize + 1] = 0;

	// ----------------------------------------------------------------------------------------------------
	// ログを出力
	// ----------------------------------------------------------------------------------------------------

#ifdef _DEBUG
	OutputDebugStringA(pOutputBuffer);
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// 後処理
	// ----------------------------------------------------------------------------------------------------

	GlobalFree(pOutputBuffer);

	return (result == 0) ? V3D_OK : V3D_ERROR_FAIL;
}

V3D_RESULT CreateShaderModuleFromFile(IV3DDevice* pDevice, const wchar_t* pFilePath, IV3DShaderModule** ppShaderModule)
{
	if ((pDevice == nullptr) || (pFilePath == nullptr) || (ppShaderModule == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	IV3DShaderModule* pShaderModule = nullptr;

	V3D_RESULT result = pDevice->CreateShaderModule(fileSize, pBuffer, &pShaderModule);
	if (result != V3D_OK)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return result;
	}

	delete[] pBuffer;
	CloseHandle(fileHandle);

	(*ppShaderModule) = pShaderModule;

	return V3D_OK;
}

// ----------------------------------------------------------------------------------------------------
// イメージ
// ----------------------------------------------------------------------------------------------------

// イメージインポーター
struct ImageImporter
{
	V3D_RESULT (_stdcall *pPrepareFunction)(IInputStream* pInputStream, IV3DDevice* pDevice, V3DImageDesc* pImageDesc, V3DImageViewDesc* pImageViewDesc, uint64_t* pBufferSize);
	V3D_RESULT (_stdcall *pLoadFunction)(IInputStream* pInputStream, V3D_FORMAT format, uint64_t memorySize, void* pMemory, std::vector<V3DCopyBufferToImageRange>& copyRanges);
};

// イメージの作成
V3D_RESULT CreateImageFromMemory(
	IV3DDevice* pDevice,
	IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer,
	IV3DFence* pFence,
	uint64_t srcSize, const void* pSrc,
	bool generateMipmap,
	IV3DImageView** ppImageView)
{
	if ((pDevice == nullptr) || (pQueue == nullptr) || (pCommandBuffer == nullptr) || (srcSize == 0) || (pSrc == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	MemoryInputStream inputStream(srcSize, pSrc);

	// ----------------------------------------------------------------------------------------------------
	// インポーターを探す
	// ----------------------------------------------------------------------------------------------------

	static constexpr ImageImporter s_ImageImporterTable[] =
	{
		{ PrepareBitmapImage, LoadBitmapImage }, // DDSImage.h
		{ PrepareDDSImage, LoadDDSImage }, // BitmapImage.h
	};
	static auto constexpr s_ImageImporterCount = _countof(s_ImageImporterTable);

	V3DImageDesc imageDesc;
	V3DImageViewDesc imageViewDesc;
	uint64_t bufferSize;

	const ImageImporter* pImporter = nullptr;

	for (auto i = 0; i < s_ImageImporterCount; i++)
	{
		V3D_RESULT result = s_ImageImporterTable[i].pPrepareFunction(&inputStream, pDevice, &imageDesc, &imageViewDesc, &bufferSize);
		if (result == V3D_OK)
		{
			pImporter = &s_ImageImporterTable[i];
		}
		else if (result == V3D_ERROR_NOT_SUPPORTED)
		{
			;
		}
		else
		{
			return result;
		}

		inputStream.Seek(SEEK_METHOD_BEGIN, 0);
	}

	if (pImporter == nullptr)
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	// ----------------------------------------------------------------------------------------------------
	// バッファを作成
	// ----------------------------------------------------------------------------------------------------

	BufferSubresourceDesc bufferSubresourceDesc{};
	bufferSubresourceDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC;
	bufferSubresourceDesc.size = bufferSize;
	bufferSubresourceDesc.count = 1;

	BufferMemoryLayout bufferMemoryLayout{};
	uint64_t bufferMemorySize;

	CalcBufferMemoryLayout(pDevice, V3D_MEMORY_PROPERTY_HOST_VISIBLE, 1, &bufferSubresourceDesc, &bufferMemoryLayout, &bufferMemorySize);

	V3DBufferDesc bufferDesc{};
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC;
	bufferDesc.size = bufferMemorySize;

	IV3DBuffer* pHostBuffer = nullptr;

	V3D_RESULT result = pDevice->CreateBuffer(bufferDesc, &pHostBuffer);
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pHostBuffer);
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// インポーターを実行
	// ----------------------------------------------------------------------------------------------------

	void* pMemory;

	result = pHostBuffer->Map(0, bufferSize, &pMemory);
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	std::vector<V3DCopyBufferToImageRange> copyRanges;

	result = pImporter->pLoadFunction(&inputStream, imageDesc.format, bufferSize, pMemory, copyRanges);
	if (result != V3D_OK)
	{
		pHostBuffer->Unmap();
		pHostBuffer->Release();
		return result;
	}

	result = pHostBuffer->Unmap();
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスのイメージを作成
	// ----------------------------------------------------------------------------------------------------

	CommandHelper commandHelper(pQueue, pCommandBuffer, pFence);

	IV3DImage* pDeviceImage = nullptr;

	if ((generateMipmap == true) && 
		(imageDesc.levelCount == 1) &&
		(imageDesc.layerCount == 1) &&
		(pDevice->GetImageFormatDesc(imageDesc.format, imageDesc.type, V3D_IMAGE_TILING_LINEAR, V3D_IMAGE_USAGE_TRANSFER_SRC | V3D_IMAGE_USAGE_TRANSFER_DST, nullptr) == V3D_OK) &&
		(pDevice->CheckImageFormatFeature(imageDesc.format, V3D_IMAGE_TILING_LINEAR, V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC | V3D_IMAGE_FORMAT_FEATURE_SAMPLED_FILTER_LINEAR) == V3D_OK) &&
		(pDevice->CheckImageFormatFeature(imageDesc.format, V3D_IMAGE_TILING_OPTIMAL, V3D_IMAGE_FORMAT_FEATURE_BLIT_DST) == V3D_OK))
	{
		/**********************/
		/* ミップマップを生成 */
		/**********************/

		// 転送用のイメージを作成
		V3DImageDesc srcImageDesc = imageDesc;
		srcImageDesc.tiling = V3D_IMAGE_TILING_LINEAR;
		srcImageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_SRC | V3D_IMAGE_USAGE_TRANSFER_DST;

		IV3DImage* pHostImage;
		result = pDevice->CreateImage(srcImageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pHostImage);
		if (result != V3D_OK)
		{
			pHostBuffer->Release();
			return result;
		}

		result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pHostImage);
		if (result != V3D_OK)
		{
			pHostImage->Release();
			pHostBuffer->Release();
			return result;
		}

		// 転送範囲を作成
		std::vector<V3DBlitImageRange> copyRanges;
		uint32_t width = imageDesc.width;
		uint32_t height = imageDesc.height;
		uint32_t levelCount = 0;

		while ((width > 1) && (height > 1))
		{
			V3DBlitImageRange range{};
			range.dstSubresource.level = levelCount;
			range.dstSubresource.baseLayer = 0;
			range.dstSubresource.layerCount = imageDesc.layerCount;
			range.dstRect.x = 0;
			range.dstRect.y = 0;
			range.dstRect.z = 0;
			range.dstRect.width = width;
			range.dstRect.height = height;
			range.dstRect.depth = imageDesc.depth;

			range.srcSubresource.level = 0;
			range.srcSubresource.baseLayer = 0;
			range.srcSubresource.layerCount = imageDesc.layerCount;
			range.srcRect.x = 0;
			range.srcRect.y = 0;
			range.srcRect.z = 0;
			range.srcRect.width = imageDesc.width;
			range.srcRect.height = imageDesc.height;
			range.srcRect.depth = imageDesc.depth;

			copyRanges.push_back(range);

			width = MAXIMUM(1, width >> 1);
			height = MAXIMUM(1, height >> 1);

			levelCount++;
		};

		// デバイスイメージを作成
		V3DImageDesc deviceImageDesc = imageDesc;
		deviceImageDesc.levelCount = levelCount;
		deviceImageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
		deviceImageDesc.usageFlags |= V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED;

		result = pDevice->CreateImage(deviceImageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pDeviceImage);
		if (result != V3D_OK)
		{
			pHostBuffer->Release();
			return result;
		}

		result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pDeviceImage);
		if (result != V3D_OK)
		{
			pHostBuffer->Release();
			return result;
		}

		// 転送
		if (commandHelper.Begin() == nullptr)
		{
			pHostBuffer->Release();
			return V3D_ERROR_FAIL;
		}

		V3DBarrierImageDesc barrier{};
		barrier.dependencyFlags = 0;
		barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

		// デバイスイメージを転送先に移行
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		pCommandBuffer->BarrierImage(pHostImage, barrier);

		// ホストバッファの内容をデバイスイメージにコピー
		V3DSize3D size;
		size.width = srcImageDesc.width;
		size.height = srcImageDesc.height;
		size.depth = srcImageDesc.depth;
		pCommandBuffer->CopyBufferToImage(pHostImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, 0, 0, imageDesc.layerCount, V3DPoint3D{}, size, pHostBuffer, 0);

		// ホストイメージを転送元に移行
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.dstAccessMask = V3D_ACCESS_TRANSFER_READ;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_SRC;
		pCommandBuffer->BarrierImage(pHostImage, barrier);

		// デバイスイメージを転送先に移行
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		pCommandBuffer->BarrierImage(pDeviceImage, barrier);

		// ホストイメージをデバイスイメージに転送
		pCommandBuffer->BlitImage(pDeviceImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, pHostImage, V3D_IMAGE_LAYOUT_TRANSFER_SRC, levelCount, copyRanges.data(), V3D_FILTER_LINEAR);

		// デバイスイメージをシェーダー読み取り専用に移行
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
		barrier.dstAccessMask = V3D_ACCESS_SHADER_READ;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pCommandBuffer->BarrierImage(pDeviceImage, barrier);

		if (commandHelper.End() == false)
		{
			pHostImage->Release();
			pHostBuffer->Release();
			return result;
		}

		// 不要なホストイメージを解放
		pHostImage->Release();

		// イメージビューの記述を更新
		imageViewDesc.levelCount = levelCount;
	}

	if (pDeviceImage == nullptr)
	{
		V3DImageDesc deviceImageDesc = imageDesc;
		deviceImageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
		deviceImageDesc.usageFlags |= V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED;

		result = pDevice->CreateImage(deviceImageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pDeviceImage);
		if (result != V3D_OK)
		{
			pHostBuffer->Release();
			return result;
		}

		result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pDeviceImage);
		if (result != V3D_OK)
		{
			pHostBuffer->Release();
			return result;
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージをホストからデバイスへ転送
		// ----------------------------------------------------------------------------------------------------

		if (commandHelper.Begin() == nullptr)
		{
			pHostBuffer->Release();
			pDeviceImage->Release();
			return V3D_ERROR_FAIL;
		}

		{
			V3DBarrierImageDesc barrier{};
			barrier.dependencyFlags = 0;
			barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			pCommandBuffer->BarrierImage(pDeviceImage, barrier);

			if (copyRanges.size() > 0)
			{
				pCommandBuffer->CopyBufferToImage(pDeviceImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, pHostBuffer, static_cast<uint32_t>(copyRanges.size()), copyRanges.data());
			}

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.dstAccessMask = V3D_ACCESS_SHADER_READ;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
			pCommandBuffer->BarrierImage(pDeviceImage, barrier);
		}

		if (commandHelper.End() == false)
		{
			pHostBuffer->Release();
			pDeviceImage->Release();
			return V3D_ERROR_FAIL;
		}
	}

	pHostBuffer->Release();

	// ----------------------------------------------------------------------------------------------------
	// イメージビューを作成
	// ----------------------------------------------------------------------------------------------------

	IV3DImageView* pImageView = nullptr;

	result = pDevice->CreateImageView(pDeviceImage, imageViewDesc, &pImageView);
	if (result != V3D_OK)
	{
		pDeviceImage->Release();
		return result;
	}

	pDeviceImage->Release();

	// ----------------------------------------------------------------------------------------------------
	// 最後の処理
	// ----------------------------------------------------------------------------------------------------

	(*ppImageView) = pImageView;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT CreateImageFromFile(
	IV3DDevice* pDevice,
	IV3DQueue* pCommandQueue, IV3DCommandBuffer* pCommandBuffer,
	IV3DFence* pFence,
	const wchar_t* pSrcFilePath,
	bool generateMipmap,
	IV3DImageView** ppImageView)
{
	HANDLE fileHandle = CreateFile(pSrcFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(fileHandle);

	V3D_RESULT result = CreateImageFromMemory(pDevice, pCommandQueue, pCommandBuffer, pFence, fileSize, pBuffer, generateMipmap, ppImageView);
	if (result != V3D_OK)
	{
		delete[] pBuffer;
		return result;
	}

	delete[] pBuffer;

	return V3D_OK;
}

// ----------------------------------------------------------------------------------------------------
// メッシュ
// ----------------------------------------------------------------------------------------------------

void CreatePrefabCube(std::vector<PrefabVertex>& vertices, std::vector<uint16_t>& indices, DrawDesc* pDrawDesc)
{
	static const PrefabVertex cubeVertices[] =
	{
		// top
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, -0.5f, 0.0f), Vector2(0.0f, 0.0f), },
		{ Vector3(-0.5f, -0.5f, +0.5f), Vector3(0.0f, -0.5f, 0.0f), Vector2(0.0f, 1.0f), },
		{ Vector3(+0.5f, -0.5f, +0.5f), Vector3(0.0f, -0.5f, 0.0f), Vector2(1.0f, 1.0f), },
		{ Vector3(+0.5f, -0.5f, -0.5f), Vector3(0.0f, -0.5f, 0.0f), Vector2(1.0f, 0.0f), },

		// bottom
		{ Vector3(-0.5f, +0.5f, +0.5f), Vector3(0.0f, +0.5f, 0.0f), Vector2(0.0f, 0.0f), },
		{ Vector3(-0.5f, +0.5f, -0.5f), Vector3(0.0f, +0.5f, 0.0f), Vector2(0.0f, 1.0f), },
		{ Vector3(+0.5f, +0.5f, -0.5f), Vector3(0.0f, +0.5f, 0.0f), Vector2(1.0f, 1.0f), },
		{ Vector3(+0.5f, +0.5f, +0.5f), Vector3(0.0f, +0.5f, 0.0f), Vector2(1.0f, 0.0f), },

		// front
		{ Vector3(-0.5f, -0.5f, +0.5f), Vector3(0.0f, 0.0f, +0.5f), Vector2(0.0f, 0.0f), },
		{ Vector3(-0.5f, +0.5f, +0.5f), Vector3(0.0f, 0.0f, +0.5f), Vector2(0.0f, 1.0f), },
		{ Vector3(+0.5f, +0.5f, +0.5f), Vector3(0.0f, 0.0f, +0.5f), Vector2(1.0f, 1.0f), },
		{ Vector3(+0.5f, -0.5f, +0.5f), Vector3(0.0f, 0.0f, +0.5f), Vector2(1.0f, 0.0f), },

		// back
		{ Vector3(+0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -0.5f), Vector2(0.0f, 0.0f), },
		{ Vector3(+0.5f, +0.5f, -0.5f), Vector3(0.0f, 0.0f, -0.5f), Vector2(0.0f, 1.0f), },
		{ Vector3(-0.5f, +0.5f, -0.5f), Vector3(0.0f, 0.0f, -0.5f), Vector2(1.0f, 1.0f), },
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.0f, 0.0f, -0.5f), Vector2(1.0f, 0.0f), },

		// left
		{ Vector3(-0.5f, -0.5f, -0.5f), Vector3(-0.5f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), },
		{ Vector3(-0.5f, +0.5f, -0.5f), Vector3(-0.5f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), },
		{ Vector3(-0.5f, +0.5f, +0.5f), Vector3(-0.5f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), },
		{ Vector3(-0.5f, -0.5f, +0.5f), Vector3(-0.5f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), },

		// right
		{ Vector3(+0.5f, -0.5f, +0.5f), Vector3(+0.5f, 0.0f, 0.0f), Vector2(0.0f, 0.0f), },
		{ Vector3(+0.5f, +0.5f, +0.5f), Vector3(+0.5f, 0.0f, 0.0f), Vector2(0.0f, 1.0f), },
		{ Vector3(+0.5f, +0.5f, -0.5f), Vector3(+0.5f, 0.0f, 0.0f), Vector2(1.0f, 1.0f), },
		{ Vector3(+0.5f, -0.5f, -0.5f), Vector3(+0.5f, 0.0f, 0.0f), Vector2(1.0f, 0.0f), },
	};

	static const uint32_t cubeVertexCount = _countof(cubeVertices);

	static const uint16_t cubeIndices[] =
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

	static const uint32_t cubeIndexCount = _countof(cubeIndices);

	vertices.resize(cubeVertexCount);
	memcpy_s(vertices.data(), sizeof(PrefabVertex) * cubeVertexCount, cubeVertices, sizeof(cubeVertices));

	indices.resize(cubeIndexCount);
	memcpy_s(indices.data(), sizeof(uint16_t) * cubeIndexCount, cubeIndices, sizeof(cubeIndices));

	pDrawDesc->indexCount = 36;
	pDrawDesc->instanceCount = 1;
}

void CreatePrefabSphere(std::vector<PrefabVertex>& vertices, std::vector<uint16_t>& indices, DrawDesc* pDrawDesc)
{
	static constexpr uint16_t vertDiv = 18;
	static constexpr uint16_t horizDiv = vertDiv * 2;
	static constexpr float rad = DEG_TO_RAD(180.0f / static_cast<float>(vertDiv));

	// ----------------------------------------------------------------------------------------------------
	// バーテックス
	// ----------------------------------------------------------------------------------------------------

	vertices.resize(2 + (vertDiv - 1) * horizDiv);

	PrefabVertex* pVertex = vertices.data();
	pVertex->pos.Set(0.0f, -0.5f, 0.0f);
	pVertex->normal.Set(0.0f, -1.0f, 0.0f);
	pVertex->uv.Set(0.5f, 0.0f);
	pVertex++;

	Vector3 pos;
	Quaternion rot;
	Matrix4x4 rotMat;

	for (uint32_t v = 1; v < vertDiv; v++)
	{
		pos.Set(0.0f, -0.5f, 0.0f);
		rot.SetRotationAxis(Vector3::XAxis(), rad * static_cast<float>(v));

		for (uint32_t h = 0; h < horizDiv; h++)
		{
			rot.RotationAxis(Vector3::YAxis(), rad);
			rot.Normalize();

			rotMat.SetRotation(rot);

			pVertex->pos = rotMat * pos;
			pVertex->normal = pVertex->pos.ToNormalize();
			pVertex->uv.Set(pVertex->pos.x * 2.0f * 0.5f + 0.5f, pVertex->pos.y * 2.0f * 0.5f + 0.5f);
			pVertex++;
		}
	}

	pVertex->pos.Set(0.0f, 0.5f, 0.0f);
	pVertex->normal.Set(0.0f, 1.0f, 0.0f);
	pVertex->uv.Set(0.5f, 1.0f);

	// ----------------------------------------------------------------------------------------------------
	// インデックス
	// ----------------------------------------------------------------------------------------------------

	uint16_t horizLast = horizDiv - 1;

	indices.resize(horizDiv * 3 + horizDiv * 3 + (vertDiv - 2) * horizDiv * 6);

	uint16_t* pIndex = indices.data();
	for (uint32_t h = 0; h < horizDiv; h++)
	{
		*pIndex++ = 0;
		*pIndex++ = h + 1;
		*pIndex++ = (h < horizLast) ? (h + 2) : 1;
	}

	for (uint32_t v = 1; v < (vertDiv - 1); v++)
	{
		uint32_t startVertex = (v - 1) * horizDiv + 1;

		for (uint32_t h = 0; h < horizDiv; h++)
		{
			*pIndex++ = startVertex + h;
			*pIndex++ = startVertex + horizDiv + h;
			*pIndex++ = startVertex + horizDiv + (h + 1) % horizDiv;

			if (h != horizLast)
			{
				*pIndex++ = startVertex + horizDiv + h + 1;
				*pIndex++ = startVertex + h + 1;
				*pIndex++ = startVertex + h;
			}
			else
			{
				*pIndex++ = startVertex + horizDiv;
				*pIndex++ = startVertex;
				*pIndex++ = startVertex + h;
			}
		}
	}

	uint16_t lastVertex = static_cast<uint16_t>(vertices.size() - 1);
	uint16_t startVertex = lastVertex - horizDiv;

	for (uint32_t h = 0; h < horizDiv; h++)
	{
		*pIndex++ = startVertex + h;
		*pIndex++ = lastVertex;
		*pIndex++ = startVertex + (h + 1) % horizDiv;
	}

	pDrawDesc->indexCount = TO_UI32(indices.size());
	pDrawDesc->instanceCount = 1;
}

V3D_RESULT CreatePrefab(
	IV3DDevice* pDevice,
	IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence,
	PREFAB_TYPE type,
	IV3DBuffer** ppBuffer, DrawDesc* pDrawDesc)
{
	V3D_RESULT result = V3D_ERROR_FAIL;

	std::vector<PrefabVertex> vertices;
	std::vector<uint16_t> indices;

	switch (type)
	{
	case PREFAB_TYPE_CUBE:
		CreatePrefabCube(vertices, indices, pDrawDesc);
		break;
	case PREFAB_TYPE_SPHERE:
		CreatePrefabSphere(vertices, indices, pDrawDesc);
		break;
	}

	// ----------------------------------------------------------------------------------------------------
	// バッファーを作成
	// ----------------------------------------------------------------------------------------------------

	size_t verticesSize = sizeof(PrefabVertex) * vertices.size();
	size_t indicesSize = sizeof(uint16_t) * indices.size();

	Array1<BufferSubresourceDesc, 2> hostBufferSubresources;
	hostBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX;
	hostBufferSubresources[0].size = verticesSize;
	hostBufferSubresources[0].count = 1;
	hostBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_INDEX;
	hostBufferSubresources[1].size = indicesSize;
	hostBufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> hostBufferMemoryLayouts;
	uint64_t hostBufferMemorySize;

	CalcBufferMemoryLayout(
		pDevice,
		V3D_MEMORY_PROPERTY_HOST_VISIBLE,
		TO_UI32(hostBufferSubresources.size()),
		hostBufferSubresources.data(),
		hostBufferMemoryLayouts.data(),
		&hostBufferMemorySize);

	V3DBufferDesc hostBufferDesc{};
	hostBufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	hostBufferDesc.size = hostBufferMemorySize;

	IV3DBuffer* pHostBuffer;

	result = pDevice->CreateBuffer(hostBufferDesc, &pHostBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pHostBuffer);
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	uint8_t* pMemory;
	result = pHostBuffer->Map(0, V3D_WHOLE_SIZE, reinterpret_cast<void**>(&pMemory));
	if (result == V3D_OK)
	{
		// バーテックスを書き込む
		MemCopy(pMemory + hostBufferMemoryLayouts[0].offset, hostBufferMemoryLayouts[0].stride, vertices.data(), verticesSize);

		// インデックスを書き込む
		MemCopy(pMemory + hostBufferMemoryLayouts[1].offset, hostBufferMemoryLayouts[1].stride, indices.data(), indicesSize);

		result = pHostBuffer->Unmap();
	}

	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	// デバイス
	Array1<BufferSubresourceDesc, 2> deviceBufferSubresources;
	deviceBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX;
	deviceBufferSubresources[0].size = verticesSize;
	deviceBufferSubresources[0].count = 1;
	deviceBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX;
	deviceBufferSubresources[1].size = indicesSize;
	deviceBufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> deviceBufferMemoryLayouts;
	uint64_t deviceBufferMemorySize;

	CalcBufferMemoryLayout(
		pDevice,
		V3D_MEMORY_PROPERTY_DEVICE_LOCAL,
		TO_UI32(deviceBufferSubresources.size()),
		deviceBufferSubresources.data(),
		deviceBufferMemoryLayouts.data(), &deviceBufferMemorySize);

	V3DBufferDesc deviceBufferDesc{};
	deviceBufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	deviceBufferDesc.size = deviceBufferMemorySize;

	IV3DBuffer* pBuffer;

	result = pDevice->CreateBuffer(deviceBufferDesc, &pBuffer);
	if (result != V3D_OK)
	{
		pHostBuffer->Release();
		return result;
	}

	result = pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pBuffer);
	if (result != V3D_OK)
	{
		pBuffer->Release();
		pHostBuffer->Release();
		return result;
	}

	// 転送
	CommandHelper commandHelper(pQueue, pCommandBuffer, pFence);

	if (commandHelper.Begin() != nullptr)
	{
		Array1<V3DCopyBufferRange, 2> copyRanges;
		copyRanges[0].dstOffset = deviceBufferMemoryLayouts[0].offset;
		copyRanges[0].srcOffset = hostBufferMemoryLayouts[0].offset;
		copyRanges[0].size = verticesSize;
		copyRanges[1].dstOffset = deviceBufferMemoryLayouts[1].offset;
		copyRanges[1].srcOffset = hostBufferMemoryLayouts[1].offset;
		copyRanges[1].size = indicesSize;

		pCommandBuffer->CopyBuffer(pBuffer, pHostBuffer, TO_UI32(copyRanges.size()), copyRanges.data());

		if (commandHelper.End() == false)
		{
			pBuffer->Release();
			pHostBuffer->Release();
			return V3D_ERROR_FAIL;
		}
	}
	else
	{
		pBuffer->Release();
		pHostBuffer->Release();
		return V3D_ERROR_FAIL;
	}

	pHostBuffer->Release();

	*ppBuffer = pBuffer;
	pDrawDesc->vertexOffset = deviceBufferMemoryLayouts[0].offset;
	pDrawDesc->indexOffset = deviceBufferMemoryLayouts[1].offset;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

// ----------------------------------------------------------------------------------------------------
// その他
// ----------------------------------------------------------------------------------------------------

V3DPipelineColorBlendAttachment InitializeColorBlendAttachment(BLEND_MODE mode)
{
	static constexpr V3DPipelineColorBlendAttachment table[] =
	{
		{ true, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO , V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL },
		{ true, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA , V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL },
		{ true, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_FACTOR_ONE , V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_FACTOR_ONE, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL },
		{ true, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_FACTOR_ONE , V3D_BLEND_OP_REVERSE_SUBTRACT, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_FACTOR_ONE, V3D_BLEND_OP_REVERSE_SUBTRACT, V3D_COLOR_COMPONENT_ALL },
		{ true, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_FACTOR_SRC_COLOR , V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ZERO, V3D_BLEND_FACTOR_SRC_ALPHA, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL },
		{ true, V3D_BLEND_FACTOR_ONE_MINUS_DST_COLOR, V3D_BLEND_FACTOR_ONE , V3D_BLEND_OP_ADD, V3D_BLEND_FACTOR_ONE_MINUS_DST_ALPHA, V3D_BLEND_FACTOR_ONE, V3D_BLEND_OP_ADD, V3D_COLOR_COMPONENT_ALL },
	};

	return table[mode];
}

void CalcBufferMemoryLayout(IV3DDevice* pDevice, V3DFlags memoryPropertyFlags, uint32_t subresourceCount, const BufferSubresourceDesc* pSubresources, BufferMemoryLayout* pMemoryLayouts, uint64_t* pMemorySize)
{
	const V3DDeviceCaps& deviceCaps = pDevice->GetCaps();
	uint64_t minAlignment = (memoryPropertyFlags & V3D_MEMORY_PROPERTY_HOST_VISIBLE)? deviceCaps.minMemoryMapAlignment : 1;
	uint64_t size = 0;

	for (uint32_t i = 0; i < subresourceCount; i++)
	{
		uint64_t alignment = minAlignment;

		if (pSubresources[i].usageFlags & (V3D_BUFFER_USAGE_UNIFORM_TEXEL | V3D_BUFFER_USAGE_STORAGE_TEXEL))
		{
			alignment = MAXIMUM(alignment, deviceCaps.minTexelBufferOffsetAlignment);
		}

		if (pSubresources[i].usageFlags & V3D_BUFFER_USAGE_UNIFORM)
		{
			alignment = MAXIMUM(alignment, deviceCaps.minUniformBufferOffsetAlignment);
		}

		if (pSubresources[i].usageFlags & V3D_BUFFER_USAGE_STORAGE)
		{
			alignment = MAXIMUM(alignment, deviceCaps.minStorageBufferOffsetAlignment);
		}

		pMemoryLayouts[i].offset = size;

		if (pSubresources[i].size % alignment)
		{
			pMemoryLayouts[i].stride = ((pSubresources[i].size / alignment) + 1) * alignment;
		}
		else
		{
			pMemoryLayouts[i].stride = pSubresources[i].size;
		}

		size += pMemoryLayouts[i].stride * pSubresources[i].count;
	}

	*pMemorySize = size;
}

void MemCopy(void* pDst, uint64_t dstSize, const void* pSrc, uint64_t srcSize)
{
#ifdef _WIN64
	memcpy_s(pDst, dstSize, pSrc, srcSize);
#else //_WIN64
	memcpy_s(pDst, TO_UI32(dstSize), pSrc, TO_UI32(srcSize));
#endif //_WIN64
}

void MbToWc(const char* pSrc, std::wstring& dst)
{
	size_t bufferSize = 0;

	mbstowcs_s(&bufferSize, nullptr, 0, pSrc, _TRUNCATE);

	wchar_t* pBuffer = new wchar_t[bufferSize + 1];

	mbstowcs_s(&bufferSize, pBuffer, bufferSize + 1, pSrc, _TRUNCATE);
	pBuffer[bufferSize] = L'\0';

	dst = pBuffer;

	delete[] pBuffer;
}

void CreateFilePath(const wchar_t* pFilePath, std::wstring& dstFilePath)
{
	static constexpr uint32_t MaxLoop = 8;

	wchar_t dirPath[4096];

	::GetCurrentDirectory(4096, dirPath);
	if ((dirPath[wcslen(dirPath) - 1] != L'\\') && (dirPath[wcslen(dirPath) - 1] != L'/'))
	{
		wcscat_s(dirPath, L"\\");
	}

	bool findDir = false;

	for (uint32_t i = 0; (i < MaxLoop) && (findDir == false); i++)
	{
		wchar_t searchPath[4096];
		wcscpy_s(searchPath, dirPath);
		wcscat_s(searchPath, L"*.*");

		WIN32_FIND_DATA wfd{};

		HANDLE findHandle = FindFirstFile(searchPath, &wfd);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		do
		{
			if (wcscmp(wfd.cFileName, L".") == 0)
			{
			}
			else if (wcscmp(wfd.cFileName, L"..") == 0)
			{
			}
			else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wcscmp(wfd.cFileName, L"data") == 0)
				{
					wcscat_s(dirPath, wfd.cFileName);
					wcscat_s(dirPath, L"\\");
					findDir = true;
				}
/*				if (wcscmp(wfd.cFileName, L"example") == 0)
				{
					wcscat_s(dirPath, wfd.cFileName);
					wcscat_s(dirPath, L"\\data\\");
					findDir = true;
				}
*/			}

		} while ((FindNextFile(findHandle, &wfd) == TRUE) && (findDir == false));

		FindClose(findHandle);

		if (findDir == false)
		{
			wcscat_s(dirPath, L"..\\");
		}
	}

	std::wstringstream stringStream;
	stringStream << dirPath << pFilePath;

	dstFilePath = stringStream.str();
}
