#pragma once

#include "v3d.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include <array>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Camera.h"
#include "Font.h"

#ifdef _DEBUG
#include <cassert>
#include <crtdbg.h>
#endif //_DEBUG

#pragma comment(lib, "winmm.lib")

// ----------------------------------------------------------------------------------------------------
// マクロ
// ----------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define ASSERT(expression) assert(expression)
#define DEBUG_NAME(name) name
#else //_DEBUG
#define ASSERT(expression)
#define DEBUG_NAME(name)
#endif //_DEBUG

#define TO_I32(value) static_cast<int32_t>(value)
#define TO_UI32(value) static_cast<uint32_t>(value)
#define TO_F32(value) static_cast<float>(value)

#define FLOAT_PI 3.14159265359f
#define FLOAT_EPSILON 1.192092896e-07F

#define FLOAT_IS_ZERO( a ) ( ( -FLOAT_EPSILON < ( a ) ) && ( FLOAT_EPSILON > ( a ) ) )
#define FLOAT_EQUAL( a, b ) ( ( ( ( a ) - FLOAT_EPSILON ) <= ( b ) ) && ( ( ( a ) + FLOAT_EPSILON ) >= ( b ) ) )
#define FLOAT_RECIPROCAL( a ) ( ( FLOAT_IS_ZERO( a ) == false )? ( 1.0f / ( a ) ) : 0.0f )
#define FLOAT_DIV( a, b ) ( ( FLOAT_IS_ZERO( b ) == false )? ( ( a ) / ( b ) ) : 0.0f )

#define MAXIMUM(a, b) ((a) < (b))? (b) : (a)
#define MINIMUM(a, b) ((a) > (b))? (b) : (a)
#define CLAMP( a, b, c ) ( ( ( b ) > ( a ) )? ( b ) : ( ( c ) < ( a ) )? ( c ) : ( a ) )

#define DEG_TO_RAD( x ) ( FLOAT_PI * ( x ) / 180.0f )
#define RAD_TO_DEG( x ) ( ( x ) * 180.0f / FLOAT_PI )

#define SAFE_DELETE(x) if ( x != nullptr ) { delete x; x = nullptr; }
#define SAFE_DELETE_ARRAY(x) if ( x != nullptr ) { delete[] x; x = nullptr; }

#define SAFE_ADD_REF(x) if ((x) != nullptr) { (x)->AddRef(); }
#define SAFE_RELEASE(x) if ((x) != nullptr) { (x)->Release(); x = nullptr; }

// 1次元配列
template<typename T, size_t count>
using Array1 = std::array<T, count>;

// 2次元配列
template<typename T, size_t count1, size_t count2>
using Array2 = std::array<std::array<T, count2>, count1>;

// 3次元配列
template<typename T, size_t count1, size_t count2, size_t count3>
using Array3 = std::array<std::array<std::array<T, count3>, count2>, count1>;

// ----------------------------------------------------------------------------------------------------
// 構想体
// ----------------------------------------------------------------------------------------------------

// シンプルバーテックス
struct SimpleVertex
{
	Vector4 pos;
	Vector2 uv;
};

// ----------------------------------------------------------------------------------------------------
// コマンド
// ----------------------------------------------------------------------------------------------------

// コマンドヘルパ
class CommandHelper
{
public:
	CommandHelper(IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer) :
		m_pQueue(pQueue),
		m_pCommandBuffer(pCommandBuffer),
		m_pFence(nullptr)
	{
	}

	CommandHelper(IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence) :
		m_pQueue(pQueue),
		m_pCommandBuffer(pCommandBuffer),
		m_pFence(pFence)
	{
	}

	IV3DCommandBuffer* Begin()
	{
		if (m_pCommandBuffer->Begin() != V3D_OK)
		{
			return nullptr;
		}

		return m_pCommandBuffer;
	}

	bool End()
	{
		V3D_RESULT result = m_pCommandBuffer->End();
		if (result != V3D_OK)
		{
			return false;
		}

		result = m_pFence->Reset();
		if (result == V3D_OK)
		{
			result = m_pQueue->Submit(1, &m_pCommandBuffer, m_pFence);
			if (result == V3D_OK)
			{
				result = m_pFence->Wait();
			}
		}

		if (result != V3D_OK)
		{
			return false;
		}

		return true;
	}

	bool End(IV3DFence* pFence)
	{
		V3D_RESULT result = m_pCommandBuffer->End();
		if (result != V3D_OK)
		{
			return false;
		}

		result = pFence->Reset();
		if (result == V3D_OK)
		{
			result = m_pQueue->Submit(1, &m_pCommandBuffer, pFence);
			if (result == V3D_OK)
			{
				result = pFence->Wait();
			}
		}

		if (result != V3D_OK)
		{
			return false;
		}

		return true;
	}

	bool End(IV3DEvent* pSignalEvent, V3DFlags stageMask, IV3DFence* pFence)
	{
		m_pCommandBuffer->SetEvent(pSignalEvent, stageMask);

		V3D_RESULT result = m_pCommandBuffer->End();
		if (result != V3D_OK)
		{
			return false;
		}

		result = pFence->Reset();
		if (result != V3D_OK)
		{
			return false;
		}

		result = m_pQueue->Submit(1, &m_pCommandBuffer, pFence);
		if (result != V3D_OK)
		{
			return false;
		}

		return true;
	}

	bool End(IV3DSemaphore* pSignalSemaphore)
	{
		V3D_RESULT result = m_pCommandBuffer->End();
		if (result != V3D_OK)
		{
			return false;
		}

		result = m_pQueue->Submit(0, nullptr, nullptr, 1, &m_pCommandBuffer, 1, &pSignalSemaphore, nullptr);
		if (result != V3D_OK)
		{
			return false;
		}

		return true;
	}

private:
	IV3DQueue* m_pQueue;
	IV3DCommandBuffer* m_pCommandBuffer;
	IV3DFence* m_pFence;
};

// ----------------------------------------------------------------------------------------------------
// シェーダー
// ----------------------------------------------------------------------------------------------------

// シェーダーのコンパイルフラグ
enum COMPILE_SHADER_FLAG : V3DFlags
{
	COMPILE_SHADER_SPIR_V_BINARY_VULKAN = 0x00000001, // Vulkan 下で SPIR-V バイナリを作成
	COMPILE_SHADER_SPIR_V_BINARY_OPENGL = 0x00000002, // OpenGL 下で SPIR-V バイナリを作成
	COMPILE_SHADER_INPUT_HLSL = 0x00000004, // ソースファイルは HLSL
};

// シェーダーのコンパイル
V3D_RESULT CompileShaderFromFile(V3DFlags flags, const wchar_t* pSrcFilePath, const wchar_t* pDstFilePath);
// ファイルからシェーダーモジュールを作成
V3D_RESULT CreateShaderModuleFromFile(IV3DDevice* pDevice, const wchar_t* pFilePath, IV3DShaderModule** ppShaderModule);

// ----------------------------------------------------------------------------------------------------
// イメージ
// ----------------------------------------------------------------------------------------------------

// メモリ内のファイルからイメージを作成
V3D_RESULT CreateImageFromMemory(
	IV3DDevice* pDevice,
	IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence,
	uint64_t srcSize, const void* pSrc,
	bool generateMipmap,
	V3DFlags dstStageMask,
	IV3DImageView** ppImageView);

// ファイルからイメージを作成
V3D_RESULT CreateImageFromFile(
	IV3DDevice* pDevice,
	IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence,
	const wchar_t* pSrcFilePath,
	bool generateMipmap,
	V3DFlags dstStageMask,
	IV3DImageView** ppImageView);

// ----------------------------------------------------------------------------------------------------
// メッシュ
// ----------------------------------------------------------------------------------------------------

enum PREFAB_TYPE
{
	PREFAB_TYPE_CUBE = 0,
	PREFAB_TYPE_SPHERE = 1,
};

struct PrefabVertex
{
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
};

struct DrawDesc
{
	uint64_t vertexOffset;
	uint64_t indexOffset;
	uint32_t indexCount;
	uint32_t instanceCount;
};

// プレハブの作成
V3D_RESULT CreatePrefab(
	IV3DDevice* pDevice,
	IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence,
	PREFAB_TYPE type,
	IV3DBuffer** ppBuffer, DrawDesc* pDrawDesc);

// ----------------------------------------------------------------------------------------------------
// その他
// ----------------------------------------------------------------------------------------------------

// ブレンドモード
enum BLEND_MODE
{
	BLEND_MODE_COPY = 0, // コピー
	BLEND_MODE_NORMAL = 1, // アルファブレンド
	BLEND_MODE_ADDITION = 2, // 加算
	BLEND_MODE_SUBTRACT = 3, // 減算
	BLEND_MODE_MULTIPLICATION = 4, // 乗算
	BLEND_MODE_SCREEN = 5, // スクリーン
};

// ブレンドの初期化
V3DPipelineColorBlendAttachment InitializeColorBlendAttachment(BLEND_MODE mode, V3DFlags writeMask = V3D_COLOR_COMPONENT_ALL);

struct BufferSubresourceDesc
{
	V3DFlags usageFlags; // バッファーの使用法 V3D_BUFFER_USAGE_FLAG
	uint64_t size; // 必要とするメモリのサイズ
	uint32_t count; // 個数
};

struct BufferMemoryLayout
{
	uint64_t offset; // メモリのオフセット
	uint64_t stride; // 個々のメモリのストライド ( メモリのサイズは BufferMemoryLayout::stride * BufferSubresourceDesc::count になります )
};

// 指定したバッファーのサブリソースからメモリのレイアウトとサイズを求めます
void CalcBufferMemoryLayout(IV3DDevice* pDevice, V3DFlags memoryPropertyFlags, uint32_t subresourceCount, const BufferSubresourceDesc* pSubresources, BufferMemoryLayout* pMemoryLayouts, uint64_t* pMemorySize);

// メモリのコピー
void MemCopy(void* pDst, uint64_t dstSize, const void* pSrc, uint64_t srcSize);
// マルチバイト文字列をワイド文字列に変換
void ToWideChar(const char* pSrc, std::wstring& dst);
// ファイルパスの作成
void CreateFilePath(const wchar_t* pSrcFilePath, std::wstring& dstFilePath);
