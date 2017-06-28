#pragma once

//! @file v3d.h
//! @brief v3d

//! @defgroup v3d_interface_group インターフェース
//! @defgroup v3d_callback_group コールバック
//! @defgroup v3d_func_group 関数
//! @defgroup v3d_struct_group 構造体
//! @defgroup v3d_enum_group 列挙型
//! @defgroup v3d_constant_group 定数

#include <cstdint>
#include <Windows.h>

#ifdef _WINDOWS

// CreateEvent の再定義
#ifdef CreateEvent
#undef CreateEvent
#ifdef UNICODE
static inline HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName)
{
	return CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
}
#else //UNICODE
static inline HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
	return CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
}
#endif //UNICODE

#endif //CreateEvent

// CreateSemaphore の再定義
#ifdef CreateSemaphore
#undef CreateSemaphore
static inline HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCTSTR lpName)
{
#ifdef UNICODE
	return CreateSemaphoreW(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
#else //UNICODE
	return CreateSemaphoreA(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
#endif //UNICODE
}
#endif //CreateSemaphore

#endif //_WINDOWS

// ----------------------------------------------------------------------------------------------------
// 定数
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_constant_group
//! @{

#ifdef V3D_DLL
#ifdef V3D_DLL_EXPORTS
//! @def V3D_DLL_API
//! @brief v3d の API 定義
#define V3D_DLL_API __declspec( dllexport )
#else // V3D_DLL_EXPORTS
//! @def V3D_DLL_API
//! @brief v3d の API 定義
#define V3D_DLL_API __declspec( dllimport )
#endif // V3D_DLL_EXPORTS
#else //V3D_DLL
//! @def V3D_DLL_API
//! @brief v3d の DLL 定義
#define V3D_DLL_API
#endif //V3D_DLL

//! @cond EXCLUDE
#ifdef _WIN64
#define V3D64
#else //_WIN64
#define V3D32
#endif //_WIN64
//! @endcond

//! @def V3D_CALLBACK
//! @brief v3d のコールバック定義
#define V3D_CALLBACK __stdcall

//! @def V3D_INFINITE
//! @brief オブジェクトがシグナル状態になるのを待機することができる最大時間です。
#define V3D_INFINITE UINT64_MAX

//! @def V3D_QUEUE_FAMILY_IGNORED
//! @brief キューファミリーのリソースの所有権の変更がないことを表します。
#define V3D_QUEUE_FAMILY_IGNORED (~0U)
//! @def V3D_ATTACHMENT_UNUSED
//! @brief 除外するアタッチメントを表します。
#define V3D_ATTACHMENT_UNUSED (~0U)
//! @def V3D_SUBPASS_EXTERNAL
//! @brief サブパス外を表します。
#define V3D_SUBPASS_EXTERNAL (~0U)

//! @}

// ----------------------------------------------------------------------------------------------------
// 型
// ----------------------------------------------------------------------------------------------------

//! @typedef V3DFlags
//! @brief v3d のフラグの型
typedef uint32_t V3DFlags;

// ----------------------------------------------------------------------------------------------------
// 列挙型
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_RESULT
//! @brief v3d のメソッドが返すことができる値
enum V3D_RESULT : int8_t
{
	V3D_OK = 0, //!< 成功しました。
	V3D_NOT_READY = 1, //!< 処理は完了していません。
	V3D_TIMEOUT = 2, //!< タイムアウトしました。
	V3D_ERROR_FAIL = -1, //!< 失敗しました。
	V3D_ERROR_INVALID_ARGUMENT = -2, //!< 不正な引数が渡されました。
	V3D_ERROR_NOT_FOUND = -3, //!< 見つかりません。
	V3D_ERROR_NOT_SUPPORTED = -4, //!< サポートされていません。
	V3D_ERROR_OUT_OF_HOST_MEMORY = -5, //!< ホストメモリが不足しています。
	V3D_ERROR_OUT_OF_DEVICE_MEMORY = -6, //!< デバイスメモリが不足しています。
	V3D_ERROR_DEVICE_LOST = -7, //!< デバイスがロストしました。
};

//! @enum V3D_FORMAT
//! @brief v3d で使用できるフォーマット
enum V3D_FORMAT : uint8_t
{
	V3D_FORMAT_UNDEFINED = 0, //!< フォーマットは不明です。
	V3D_FORMAT_R4G4_UNORM = 1, //!< 2 成分、8 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R4G4B4A4_UNORM = 2, //!<  4 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_B4G4R4A4_UNORM = 3, //!< 4 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R5G6B5_UNORM = 4, //!< 3 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_B5G6R5_UNORM = 5, //!< 3 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R5G5B5A1_UNORM = 6, //!< 4 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_B5G5R5A1_UNORM = 7, //!< 4 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_A1R5G5B5_UNORM = 8, //!<  4 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R8_UNORM = 9, //!< 1 成分、8 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R8_SNORM = 10, //!< 1 成分、8 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R8_USCALED = 11, //!< 1 成分、8 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R8_SSCALED = 12, //!< 1 成分、8 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R8_UINT = 13, //!< 1 成分、8 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R8_SINT = 14, //!< 1 成分、8 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R8_SRGB = 15, //!< 1 成分、8 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_R8G8_UNORM = 16, //!< 2 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R8G8_SNORM = 17, //!< 2 成分、16 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R8G8_USCALED = 18, //!< 2 成分、16 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R8G8_SSCALED = 19, //!< 2 成分、16 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R8G8_UINT = 20, //!< 2 成分、16 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R8G8_SINT = 21, //!< 2 成分、16 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R8G8_SRGB = 22, //!< 2 成分、16 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_R8G8B8_UNORM = 23, //!< 3 成分、24 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R8G8B8_SNORM = 24, //!< 3 成分、24 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R8G8B8_USCALED = 25, //!< 3 成分、24 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R8G8B8_SSCALED = 26, //!< 3 成分、24 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R8G8B8_UINT = 27, //!< 3 成分、24 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R8G8B8_SINT = 28, //!< 3 成分、24 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R8G8B8_SRGB = 29, //!< 3 成分、24 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_B8G8R8_UNORM = 30, //!< 3 成分、24 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_B8G8R8_SNORM = 31, //!< 3 成分、24 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_B8G8R8_USCALED = 32, //!< 3 成分、24 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_B8G8R8_SSCALED = 33, //!< 3 成分、24 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_B8G8R8_UINT = 34, //!< 3 成分、24 ビット符号なし整数フォーマットです。
	V3D_FORMAT_B8G8R8_SINT = 35, //!< 3 成分、24 ビット符号付き整数フォーマットです。
	V3D_FORMAT_B8G8R8_SRGB = 36, //!< 3 成分、24 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_R8G8B8A8_UNORM = 37, //!< 4 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R8G8B8A8_SNORM = 38, //!< 4 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R8G8B8A8_USCALED = 39, //!< 4 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R8G8B8A8_SSCALED = 40, //!< 4 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R8G8B8A8_UINT = 41, //!< 4 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R8G8B8A8_SINT = 42, //!< 4 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R8G8B8A8_SRGB = 43, //!< 4 成分、32 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_B8G8R8A8_UNORM = 44, //!< 4 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_B8G8R8A8_SNORM = 45, //!< 4 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_B8G8R8A8_USCALED = 46, //!< 4 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_B8G8R8A8_SSCALED = 47, //!< 4 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_B8G8R8A8_UINT = 48, //!< 4 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_B8G8R8A8_SINT = 49, //!< 4 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_B8G8R8A8_SRGB = 50, //!< 4 成分、32 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_A8B8G8R8_UNORM = 51, //!< 4 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_A8B8G8R8_SNORM = 52, //!< 4 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_A8B8G8R8_USCALED = 53, //!< 4 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_A8B8G8R8_SSCALED = 54, //!< 4 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_A8B8G8R8_UINT = 55, //!< 4 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_A8B8G8R8_SINT = 56, //!< 4 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_A8B8G8R8_SRGB = 57, //!< 4 成分、32 ビット符号なし sRGB 正規化フォーマットです。
	V3D_FORMAT_A2R10G10B10_UNORM = 58, //!< 4 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_A2R10G10B10_SNORM = 59, //!< 4 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_A2R10G10B10_USCALED = 60, //!< 4 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_A2R10G10B10_SSCALED = 61, //!< 4 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_A2R10G10B10_UINT = 62, //!< 4 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_A2R10G10B10_SINT = 63, //!< 4 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_A2B10G10R10_UNORM = 64, //!< 4 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_A2B10G10R10_SNORM = 65, //!< 4 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_A2B10G10R10_USCALED = 66, //!< 4 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_A2B10G10R10_SSCALED = 67, //!< 4 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_A2B10G10R10_UINT = 68, //!< 4 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_A2B10G10R10_SINT = 69, //!< 4 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R16_UNORM = 70, //!< 1 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R16_SNORM = 71, //!< 1 成分、16 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R16_USCALED = 72, //!< 1 成分、16 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R16_SSCALED = 73, //!< 1 成分、16 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R16_UINT = 74, //!< 1 成分、16 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R16_SINT = 75, //!< 1 成分、16 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R16_SFLOAT = 76, //!< 1 成分、16 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R16G16_UNORM = 77, //!< 2 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R16G16_SNORM = 78, //!< 2 成分、32 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R16G16_USCALED = 79, //!< 2 成分、32 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R16G16_SSCALED = 80, //!< 2 成分、32 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R16G16_UINT = 81, //!< 2 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R16G16_SINT = 82, //!< 2 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R16G16_SFLOAT = 83, //!< 2 成分、32 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R16G16B16_UNORM = 84, //!< 3 成分、48 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R16G16B16_SNORM = 85, //!< 3 成分、48 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R16G16B16_USCALED = 86, //!< 3 成分、48 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R16G16B16_SSCALED = 87, //!< 3 成分、48 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R16G16B16_UINT = 88, //!< 3 成分、48 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R16G16B16_SINT = 89, //!< 3 成分、48 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R16G16B16_SFLOAT = 90, //!< 3 成分、48 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R16G16B16A16_UNORM = 91, //!< 4 成分、64 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_R16G16B16A16_SNORM = 92, //!< 4 成分、64 ビット符号付き正規化フォーマットです。
	V3D_FORMAT_R16G16B16A16_USCALED = 93, //!< 4 成分、64 ビット符号なしスケール整数フォーマットです。
	V3D_FORMAT_R16G16B16A16_SSCALED = 94, //!< 4 成分、64 ビット符号付きスケール整数フォーマットです。
	V3D_FORMAT_R16G16B16A16_UINT = 95, //!< 4 成分、64 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R16G16B16A16_SINT = 96, //!< 4 成分、64 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R16G16B16A16_SFLOAT = 97, //!< 4 成分、64 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R32_UINT = 98, //!< 1 成分、32 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R32_SINT = 99, //!< 1 成分、32 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R32_SFLOAT = 100, //!< 1 成分、32 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R32G32_UINT = 101, //!< 2 成分、64 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R32G32_SINT = 102, //!< 2 成分、64 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R32G32_SFLOAT = 103, //!< 2 成分、64 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R32G32B32_UINT = 104, //!< 3 成分、96 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R32G32B32_SINT = 105, //!< 3 成分、96 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R32G32B32_SFLOAT = 106, //!< 3 成分、96 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R32G32B32A32_UINT = 107, //!< 4 成分、128 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R32G32B32A32_SINT = 108, //!< 4 成分、128 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R32G32B32A32_SFLOAT = 109, //!< 4 成分、128 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R64_UINT = 110, //!< 1 成分、64 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R64_SINT = 111, //!< 1 成分、64 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R64_SFLOAT = 112, //!< 1 成分、64 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R64G64_UINT = 113, //!< 2 成分、128 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R64G64_SINT = 114, //!< 2 成分、128 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R64G64_SFLOAT = 115, //!< 2 成分、128 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R64G64B64_UINT = 116, //!< 3 成分、192 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R64G64B64_SINT = 117, //!< 3 成分、192 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R64G64B64_SFLOAT = 118, //!< 3 成分、192 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_R64G64B64A64_UINT = 119, //!< 4 成分、256 ビット符号なし整数フォーマットです。
	V3D_FORMAT_R64G64B64A64_SINT = 120, //!< 4 成分、256 ビット符号付き整数フォーマットです。
	V3D_FORMAT_R64G64B64A64_SFLOAT = 121, //!< 4 成分、256 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_B10G11R11_UFLOAT = 122, //!< 3 成分、32 ビット符号なし浮動小数点フォーマットです。
	V3D_FORMAT_E5B9G9R9_UFLOAT = 123, //!< 4 成分、共有指数を含む 32 ビット符号なし浮動小数点フォーマットです。
	V3D_FORMAT_D16_UNORM = 124, //!< 1 成分、深度 16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_X8_D24_UNORM = 125, //!< 2 成分、未使用 8 ビット、深度 24 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_D32_SFLOAT = 126, //!< 1 成分、深度 32 ビット符号付き浮動小数点フォーマットです。
	V3D_FORMAT_S8_UINT = 127, //!< 1 成分、8 ビット符号なし整数フォーマットです。
	V3D_FORMAT_D16_UNORM_S8_UINT = 128, //!< デプスに 16 ビット符号なし正規化整数、ステンシルに 8 ビット符号無し整数を使用するフォーマットです。
	V3D_FORMAT_D24_UNORM_S8_UINT = 129, //!< デプスに 24 ビット符号なし正規化整数、ステンシルに 8 ビット符号無し整数を使用するフォーマットです。
	V3D_FORMAT_D32_SFLOAT_S8_UINT = 130, //!< デプスに 32 ビット符号付き浮動小数点、ステンシルに 8 ビット符号無し整数を使用するフォーマットです。
	V3D_FORMAT_BC1_RGB_UNORM = 131, //!< 3 成分、64 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC1_RGB_SRGB = 132, //!< 3 成分、64 ビット符号なし sRGB 正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC1_RGBA_UNORM = 133, //!< 4 成分、64 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC1_RGBA_SRGB = 134, //!< 4 成分、64 ビット符号なし sRGB 正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC2_UNORM = 135, //!< 4 成分、128 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC2_SRGB = 136, //!< 4 成分、128 ビット符号なし sRGB 正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC3_UNORM = 137, //!< 4 成分、128 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC3_SRGB = 138, //!< 4 成分、128 ビット符号なし sRGB 正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC4_UNORM = 139, //!< 1 成分、64 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC4_SNORM = 140, //!< 1 成分、64 ビット符号付き正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC5_UNORM = 141, //!< 4 成分、128 ビット符号なし正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC5_SNORM = 142, //!< 4 成分、128 ビット符号付き正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC6H_UFLOAT = 143, //!< 3 成分、128 ビット符号なし浮動小数点 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC6H_SFLOAT = 144, //!< 3 成分、128 ビット符号付き浮動小数点 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC7_UNORM = 145, //!< 4 成分、128 ビット符号なし浮動小数点 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_BC7_SRGB = 146, //!< 4 成分、128 ビット符号なし sRGB 正規化 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8_UNORM = 147, //!< 3 成分、64 ビット符号なし正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8_SRGB = 148, //!< 3 成分、64 ビット符号なし sRGB 正規化 4x4 ブロックETC2 圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8A1_UNORM = 149, //!< 4 成分、64 ビット符号なし正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8A1_SRGB = 150, //!< 4 成分、64 ビット符号なし sRGB 正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8A8_UNORM = 151, //!< 4 成分、64 ビット符号なし正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ETC2_R8G8B8A8_SRGB = 152, //!< 4 成分、64 ビット符号なし sRGB 正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_EAC_R11_UNORM = 153, //!< 1 成分、64 ビット符号なし正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_EAC_R11_SNORM = 154, //!< 1 成分、64 ビット符号付き正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_EAC_R11G11_UNORM = 155, //!< 2 成分、128 ビット符号なし正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_EAC_R11G11_SNORM = 156, //!< 2 成分、128 ビット符号付き正規化 ETC2 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_4X4_UNORM = 157, //!< 4 成分、128 ビット符号なし正規化 ASTC 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_4X4_SRGB = 158, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 4x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_5X4_UNORM = 159, //!< 4 成分、128 ビット符号なし正規化 ASTC 5x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_5X4_SRGB = 160, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 5x4 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_5X5_UNORM = 161, //!< 4 成分、128 ビット符号なし正規化 ASTC 5x5 ブロック圧縮フォーマットです。
	V3D_FORMAT_ASTC_5X5_SRGB = 162, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 5x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_6X5_UNORM = 163, //!< 4 成分、128 ビット符号なし正規化 ASTC 6x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_6X5_SRGB = 164, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 6x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_6X6_UNORM = 165, //!< 4 成分、128 ビット符号なし正規化 ASTC 6x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_6X6_SRGB = 166, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 6x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X5_UNORM = 167, //!< 4 成分、128 ビット符号なし正規化 ASTC 8x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X5_SRGB = 168, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 8x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X6_UNORM = 169, //!< 4 成分、128 ビット符号なし正規化 ASTC 8x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X6_SRGB = 170, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 8x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X8_UNORM = 171, //!< 4 成分、128 ビット符号なし正規化 ASTC 8x8 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_8X8_SRGB = 172, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 8x8 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X5_UNORM = 173, //!< 4 成分、128 ビット符号なし正規化 ASTC 10x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X5_SRGB = 174, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 10x5 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X6_UNORM = 175, //!< 4 成分、128 ビット符号なし正規化 ASTC 10x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X6_SRGB = 176, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 10x6 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X8_UNORM = 177, //!< 4 成分、128 ビット符号なし正規化 ASTC 10x8 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X8_SRGB = 178, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 10x8 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X10_UNORM = 179, //!< 4 成分、128 ビット符号なし正規化 ASTC 10x10 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_10X10_SRGB = 180, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 10x10 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_12X10_UNORM = 181, //!< 4 成分、128 ビット符号なし正規化 ASTC 12x10 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_12X10_SRGB = 182, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 12x10 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_12X12_UNORM = 183, //!< 4 成分、128 ビット符号なし正規化 ASTC 12x12 ブロック圧縮圧縮フォーマットです。
	V3D_FORMAT_ASTC_12X12_SRGB = 184, //!< 4 成分、128 ビット符号なし sRGB 正規化 ASTC 12x12 ブロック圧縮圧縮フォーマットです。
};

//! @enum V3D_MEMORY_PROPERTY_FLAG
//! @brief メモリプロパティフラグ
enum V3D_MEMORY_PROPERTY_FLAG : V3DFlags
{
	//! @brief デバイスからアクセスできるメモリです。
	V3D_MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001,
	//! @brief ホストからアクセスできるメモリです。
	V3D_MEMORY_PROPERTY_HOST_VISIBLE = 0x00000002,
	//! @brief ホストから書き込んだ内容をフラッシュする必要はありません。<br>
	//! このフラグを指定した場合は IV3DResource::Map および IV3DResource::Unmap 時にデバイスまたはホストからのメモリの可視、不可視の変更処理を行わないため、速度の向上が見込まれます。<br>
	V3D_MEMORY_PROPERTY_HOST_COHERENT = 0x00000004,
	//! @brief メモリの内容をホストでキャッシュします。<br>
	//! ホストで内容をキャッシュすることにメモリへのアクセス速度の向上が見込まれます。
	V3D_MEMORY_PROPERTY_HOST_CACHED = 0x00000008,
	//! @brief メモリが遅延して割り当てられる場合があります。<br>
	//! このフラグは カラーアタッチメント、デプスステンシルアタッチメント、インプットアタッチメントに対して有効です。<br>
	//! また、イメージの使用法には ::V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT を指定してください。
	V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED = 0x00000010,
};

//! @enum V3D_PIPELINE_STAGE_FLAG
//! @brief パイプラインステージフラグ
enum V3D_PIPELINE_STAGE_FLAG : V3DFlags
{
	//! @brief 最初のステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
	//! @brief バーテックスバッファとインデックスバッファが消費されるステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
	//! @brief バーテックスシェーダステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
	//! @brief テッセレーション制御シェーダステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
	//! @brief テッセレーション評価シェーダステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
	//! @brief ジオメトリシェーダステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
	//! @brief フラグメントシェーダステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
	//! @brief フラグメントシェーディング前のデプステストとステンシルテストが実行されるステージです。<br>
	//! サブパスのデプスステンシルアタッチメントのロード処理が含まれます。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
	//! @brief フラグメントシェーディング後のデプステストとステンシルテストが実行されるステージです。<br>
	//! サブパスのデプスステンシルアタッチメントのストア処理が含まれます。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
	//! @brief ブレンドされたカラーが出力されるパイプラインステージです。<br>
	//! サブパスのカラーアタッチメントのロード、ストア、リゾルブ ( マルチサンプル ) が含まれます。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	//! @brief コンピュートシェーダーステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_COMPUTE </td></tr></table>
	V3D_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
	//! @brief コピー操作をするステージです。<br>
	//! 以下のコマンドはこのステージで実行する必要があります。<br>
	//! <br>
	//! IV3DCommandBuffer::CopyBuffer<br>
	//! IV3DCommandBuffer::CopyImage<br>
	//! IV3DCommandBuffer::CopyBufferToImage<br>
	//! IV3DCommandBuffer::CopyImageToBuffer<br>
	//! IV3DCommandBuffer::ClearImage<br>
	//! IV3DCommandBuffer::ResolveImage<br>
	//! IV3DCommandBuffer::ResolveImageView<br>
	//! IV3DCommandBuffer::BlitImage<br>
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br>または<br> ::V3D_QUEUE_TRANSFER </td></tr></table>
	V3D_PIPELINE_STAGE_TRANSFER = 0x00001000,
	//! @brief 最後のステージです。<br>
	//! コマンドによって生成されたオペレーションが実行を完了するステージであり、
	//! 次のアクセスが別のキューまたはプレゼンテーション ( IV3DQueue::Present ) で行われる場合の移行に使用します。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,

	//! @brief ホストからの読み込み、または書き込みを実行するステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_HOST = 0x00004000,
	//! @brief すべてのグラフィックパイプラインステージです。<br>
	//! <br>
	//! ::V3D_PIPELINE_STAGE_TOP_OF_PIPE<br>
	//! ::V3D_PIPELINE_STAGE_VERTEX_INPUT<br>
	//! ::V3D_PIPELINE_STAGE_VERTEX_SHADER<br>
	//! ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER<br>
	//! ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER<br>
	//! ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER<br>
	//! ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER<br>
	//! ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS<br>
	//! ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS<br>
	//! ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT<br>
	//! ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE<br>
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000,
	//! @brief すべてのコマンドのステージです。
	//! <br>
	//! <table class="pielineStage"><tr><th>必須のキュー機能</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
};

//! @enum V3D_ACCESS_FLAG
//! @brief アクセスフラグ
enum V3D_ACCESS_FLAG : V3DFlags
{
	//! @brief インデックスバッファーへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_VERTEX_INPUT</td><td>::V3D_BUFFER_USAGE_INDEX</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_INDEX_READ = 0x00000002,
	//! @brief バーテックスバッファへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td class="td_stage">::V3D_PIPELINE_STAGE_VERTEX_INPUT</td><td>::V3D_BUFFER_USAGE_VERTEX</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_VERTEX_READ = 0x00000004,
	//! @brief ユニフォームバッファへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER
	//!     </td>
	//!     <td>
	//!       ::V3D_BUFFER_USAGE_UNIFORM_TEXEL<br>
	//!       ::V3D_BUFFER_USAGE_UNIFORM
	//!     </td>
	//!     <td>::V3D_QUEUE_GRAPHICS<br> ::V3D_QUEUE_COMPUTE</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_UNIFORM_READ = 0x00000008,
	//! @brief インプットアタッチメントへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_FRAGMENT_SHADER</td><td>::V3D_IMAGE_USAGE_INPUT_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
	//! @brief ストレージバッファ、ユニフォームテクセルバッファ、ストレージテクセルバッファ、サンプリングイメージ、ストレージイメージへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER
	//!     </td>
	//!     <td>
	//!       ::V3D_BUFFER_USAGE_UNIFORM_TEXEL<br>
	//!       ::V3D_BUFFER_USAGE_STORAGE_TEXEL<br>
	//!       ::V3D_BUFFER_USAGE_STORAGE<br>
	//!       <br>
	//!       ::V3D_IMAGE_USAGE_SAMPLED<br>
	//!       ::V3D_IMAGE_USAGE_STORAGE
	//!     </td>
	//!     <td>::V3D_QUEUE_GRAPHICS<br> ::V3D_QUEUE_COMPUTE</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_SHADER_READ = 0x00000020,
	//! @brief ストレージバッファ、ストレージテクセルバッファ、ストレージイメージへの書き込みアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER<br>
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER
	//!     </td>
	//!     <td>
	//!       ::V3D_BUFFER_USAGE_STORAGE_TEXEL<br>
	//!       ::V3D_BUFFER_USAGE_STORAGE<br>
	//!       <br>
	//!       ::V3D_IMAGE_USAGE_STORAGE
	//!     </td>
	//!     <td>::V3D_QUEUE_GRAPHICS<br> ::V3D_QUEUE_COMPUTE</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_SHADER_WRITE = 0x00000040,
	//! @brief ブレンディング、ロジック、サブパスのロードなどのカラーアタッチメントへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td class="td_stage">::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT</td><td>::V3D_IMAGE_USAGE_COLOR_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
	//! @brief レンダリングパス中のサブパスのロード後からストアまでの処理などのカラーアタッチメントへの書き込みアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT</td><td>::V3D_IMAGE_USAGE_COLOR_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
	//! @brief デプス、ステンシルの操作、サブパスのロードなどのデプスステンシルアタッチメントへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS<br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS<br>
	//!     </td>
	//!     <td>::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT</td>
	//!     <td>::V3D_QUEUE_GRAPHICS</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
	//! @brief レンダリングパス中のサブパスのロード後からストアまでの処理などのデプスステンシルアタッチメントへの書き込みアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS<br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS<br>
	//!     </td>
	//!     <td>::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT</td>
	//!     <td>::V3D_QUEUE_GRAPHICS</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
	//! @brief コピーコマンドでのバッファまたはイメージへの読み取りアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_TRANSFER</td><td> ::V3D_BUFFER_USAGE_TRANSFER_SRC<br> <br> ::V3D_IMAGE_USAGE_TRANSFER_SRC</td><td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE<br> ::V3D_QUEUE_TRANSFER</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_TRANSFER_READ = 0x00000800,
	//! @brief コピーコマンドでのバッファまたはイメージへの書き込みアクセスです。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_TRANSFER</td><td>::V3D_BUFFER_USAGE_TRANSFER_DST<br> <br> ::V3D_IMAGE_USAGE_TRANSFER_DST</td><td>::V3D_QUEUE_GRAPHICS<br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td></tr>
	//! </table>
	V3D_ACCESS_TRANSFER_WRITE = 0x00001000,
	//! @brief ホストからの読み取りアクセスです。<br>
	//! バインドしたリソースメモリの特性に ::V3D_MEMORY_PROPERTY_HOST_VISIBLE が含まれているリソースを対象とします。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_HOST</td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_HOST_READ = 0x00002000,
	//! @brief ホストからの書き込みアクセスです。<br>
	//! バインドしたリソースメモリの特性に ::V3D_MEMORY_PROPERTY_HOST_VISIBLE が含まれているリソースを対象とします。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_HOST</td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_HOST_WRITE = 0x00004000,
	//! @brief 特定のリソースへの読み取りアクセスです。<br>
	//! スワップチェインのイメージなどがこの特定のリソースに含まれます。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td> </td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_MEMORY_READ = 0x00008000,
	//! @brief 特定のリソースへの書き込みアクセスです。<br>
	//! スワップチェインのイメージなどがこの特定のリソースに含まれます。<br>
	//! <table class="access">
	//!   <tr><th class="stage">サポートするステージ</th><th class="usage">サポートするリソースの使用法</th><th class="queue">必須のキュー機能</th></tr>
	//!   <tr><td> </td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_MEMORY_WRITE = 0x00010000,
};

//! @enum V3D_DEPENDENCY_FLAG
//! @brief 依存性フラグ
enum V3D_DEPENDENCY_FLAG : V3DFlags
{
	//! @brief サブパスの依存関係がフレームバッファー内であることを表します。
	V3D_DEPENDENCY_BY_REGION = 0x00000001,
};

//! @enum V3D_SHADER_STAGE_FLAG
//! @brief シェーダーステージフラグ
enum V3D_SHADER_STAGE_FLAG : V3DFlags
{
	V3D_SHADER_STAGE_VERTEX = 0x00000001, //!< バーテックスシェーダーステージです。
	V3D_SHADER_STAGE_TESSELLATION_CONTROL = 0x00000002, //!< テッセレーション制御シェーダーステージです。
	V3D_SHADER_STAGE_TESSELLATION_EVALUATION = 0x00000004, //!< テッセレーション評価シェーダーステージです。
	V3D_SHADER_STAGE_GEOMETRY = 0x00000008, //!< ジオメトリシェーダーステージです。
	V3D_SHADER_STAGE_FRAGMENT = 0x00000010, //!< フラグメントシェーダーステージです。
	V3D_SHADER_STAGE_COMPUTE = 0x00000020, //!< コンピュートシェーダーステージです。
};

//! @enum V3D_IMAGE_LAYOUT
//! @brief イメージのレイアウト
enum V3D_IMAGE_LAYOUT
{
	//! @brief デバイスのアクセスをサポートしません。<br>
	//! イメージ作成時の初期レイアウトとして使用することができます。<br>
	//! また、このレイアウトから移行するとき、メモリの内容の保持は保証されません。
	//! @sa IV3DDevice::CreateImage
	V3D_IMAGE_LAYOUT_UNDEFINED = 0,
	//! @brief すべてのアクセスをサポートします。<br>
	V3D_IMAGE_LAYOUT_GENERAL = 1,
	//! @brief カラーアタッチメント、リゾルブアタッチメントに対して有効なレイアウトです。<br>
	//! このレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT が含まれている必要があります。
	V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT = 2,
	//! @brief デプスステンシルアタッチメントに対して有効なレイアウトです。<br>
	//! このレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT が含まれている必要があります。
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT = 3,
	//! @brief シェーダーから読み取ることのできるデプスステンシルイメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT 、 ::V3D_IMAGE_USAGE_SAMPLED もしくわ ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT のいずれかが含まれている必要があります。
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY = 4,
	//! @brief シェーダーからの読み取り専用イメージに対して有効なレイアウトです。<br>
	//! これはサンプリングすることのできるイメージ、インプットアタッチメントとして使用することができます。<br>
	//! またこのレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_SAMPLED または ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT のいずれかが含まれている必要があります。
	V3D_IMAGE_LAYOUT_SHADER_READ_ONLY = 5,
	//! @brief コピーコマンドのコピー元イメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_TRANSFER_SRC 、パイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。<br>
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_LAYOUT_TRANSFER_SRC = 6,
	//! @brief コピーコマンドのコピー先イメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用するイメージの使用法には ::V3D_IMAGE_USAGE_TRANSFER_DST 、パイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_LAYOUT_TRANSFER_DST = 7,
	//! @brief デバイスのアクセスをサポートしません<br>
	//! イメージ作成時の初期レイアウトとして使用することができ、イメージ作成後すぐにメモリへの書き込みができます。<br>
	//! また異なるレイアウトへ移行するとき、メモリの内容は保持されます。
	//! @sa IV3DDevice::CreateImage
	V3D_IMAGE_LAYOUT_PREINITIALIZED = 8,
	//! @brief スワップチェインのイメージを表示する際のレイアウトです。<br>
	//! スワップチェインのイメージはプレゼント ( IV3DQueue::Present ) する前にこのイメージレイアウトに移行しておく必要があります。
	V3D_IMAGE_LAYOUT_PRESENT_SRC = 9,
};

//! @enum V3D_SAMPLE_COUNT_FLAG
//! @brief サンプル数フラグ
enum V3D_SAMPLE_COUNT_FLAG : V3DFlags
{
	V3D_SAMPLE_COUNT_1 = 0x00000001, //!< 1 サンプルです。
	V3D_SAMPLE_COUNT_2 = 0x00000002, //!< 2 サンプルです。
	V3D_SAMPLE_COUNT_4 = 0x00000004, //!< 4 サンプルです。
	V3D_SAMPLE_COUNT_8 = 0x00000008, //!< 8 サンプルです。
	V3D_SAMPLE_COUNT_16 = 0x00000010, //!< 16 サンプルです。
	V3D_SAMPLE_COUNT_32 = 0x00000020, //!< 32 サンプルです。
	V3D_SAMPLE_COUNT_64 = 0x00000040, //!< 64 サンプルです。
};

//! @enum V3D_COMPARE_OP
//! @brief 比較演算子
//! @sa V3DSamplerDesc::compareOp
//! @sa V3DPipelineStencilOpDesc::compareOp
//! @sa V3DPipelineDepthStencilDesc::depthCompareOp
enum V3D_COMPARE_OP : uint8_t
{
	V3D_COMPARE_OP_NEVER = 0, //!< テストは絶対に通過しません。
	V3D_COMPARE_OP_LESS = 1, //!< R<S のときにテストに合格します。
	V3D_COMPARE_OP_EQUAL = 2, //!< R=S のときにテストに合格します。
	V3D_COMPARE_OP_LESS_OR_EQUAL = 3, //!< R≦S のときにテストに合格します。
	V3D_COMPARE_OP_GREATER = 4, //!< R>S のときにテストに合格します。
	V3D_COMPARE_OP_NOT_EQUAL = 5, //!< R≠S のときにテストが合格します。
	V3D_COMPARE_OP_GREATER_OR_EQUAL = 6, //!< R≧S のときにテストに合格します。
	V3D_COMPARE_OP_ALWAYS = 7, //!< テストは常に通過します。
};

//! @enum V3D_INDEX_TYPE
//! @brief インデックスのタイプ
enum V3D_INDEX_TYPE : uint8_t
{
	V3D_INDEX_TYPE_UINT16 = 0, //!< 符号無し 16Bit 整数のインデックスです。
	V3D_INDEX_TYPE_UINT32 = 1, //!< 符号無し 32Bit 整数のインデックスです。
};

//! @}

// ----------------------------------------------------------------------------------------------------
// 構造体
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @union V3DPoint2D
//! @brief 符号無し整数の x y 座標
struct V3DPoint2D
{
	uint32_t x; //!< X 座標です。
	uint32_t y; //!< Y 座標です。
};

//! @union V3DPoint3D
//! @brief 符号無し整数の x y z 座標
struct V3DPoint3D
{
	uint32_t x; //!< X 座標です。
	uint32_t y; //!< Y 座標です。
	uint32_t z; //!< Z 座標です。
};

//! @union V3DSize2D
//! @brief 符号無し整数の width height サイズ
struct V3DSize2D
{
	uint32_t width; //!< 幅です。
	uint32_t height; //!< 高さです。
};

//! @union V3DSize3D
//! @brief 符号無し整数の width height depth サイズ
struct V3DSize3D
{
	uint32_t width; //!< 幅です。
	uint32_t height; //!< 高さです。
	uint32_t depth; //!< 深さです。
};

//! @union V3DRectangle2D
//! @brief 領域
struct V3DRectangle2D
{
	uint32_t x; //!< X 座標です。
	uint32_t y; //!< Y 座標です。
	uint32_t width; //!< 幅です。
	uint32_t height; //!< 高さです。
};

//! @union V3DRectangle3D
//! @brief 領域
struct V3DRectangle3D
{
	uint32_t x; //!< X 座標です。
	uint32_t y; //!< Y 座標です。
	uint32_t z; //!< Z 座標です。
	uint32_t width; //!< 幅です。
	uint32_t height; //!< 高さです。
	uint32_t depth; //!< 深さです。
};

//! @union V3DViewport
//! @brief ビューポート
//! @remarks 深度は minDepth < maxDepth である必要があります。
struct V3DViewport
{
	//! @brief ビューポートの領域です。
	V3DRectangle2D rect;
	//! @brief 最小深度です。<br>
	//! 値は { 0.0f <= minDepth <= 1.0f } の範囲に制限されます。
	float minDepth;
	//! @brief 最大深度です。<br>
	//! 値は { minDepth <= maxDepth <= 1.0f } の範囲に制限されます。
	float maxDepth;
};

//! @union V3DClearColorValue
//! @brief カラーのクリア値
union V3DClearColorValue
{
	float float32[4]; //!< 符号付 32bit 浮動小数点です。
	int32_t int32[4]; //!< 符号付 32bit 整数です。
	uint32_t uint32[4]; //!< 符号無し 32bit 整数です。
};

//! @struct V3DClearDepthStencilValue
//! @brief デプスステンシルのクリア値
struct V3DClearDepthStencilValue
{
	//! @brief 深度をクリアする値です。<br>
	//! この値は { 0.0f <= depth <= 1.0f } の範囲に制限されます。
	float depth;
	//! @brief ステンシルをクリアする値です。
	uint32_t stencil;
};

//! @union V3DClearValue
//! @brief クリア値
union V3DClearValue
{
	V3DClearColorValue color; //!< カラーのクリア値です。
	V3DClearDepthStencilValue depthStencil; //!< デプスステンシルのクリア値です。
};

//! @}

// ----------------------------------------------------------------------------------------------------
// オブジェクト
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DObject
//! @brief オブジェクトインターフェース
class V3D_DLL_API IV3DObject
{
public:
	//! @brief 参照数を取得します。
	//! @return 参照数を返します。
	virtual int64_t GetRefCount() const = 0;
	//! @brief 参照数をインクリメントします。
	virtual void AddRef() = 0;
	//! @brief 参照数をデクリメントし、参照数が 0 になった時点でオブジェクを破棄します。
	virtual void Release() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DObject() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// デバイスチャイルド
// ----------------------------------------------------------------------------------------------------

class IV3DDevice;

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDeviceChild
//! @brief デバイスチャイルドインターフェース
class V3D_DLL_API IV3DDeviceChild : public IV3DObject
{
public:
	//! @brief このインターフェースが作成されたデバイスを取得します
	//! @param[out] ppDevice デバイスを表す IV3DDevice インターフェースのポインタのアドレス
	virtual void GetDevice(IV3DDevice** ppDevice) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDeviceChild() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// バリアセット
// ----------------------------------------------------------------------------------------------------

class IV3DBuffer;
class IV3DBufferView;
class IV3DImage;
class IV3DImageView;

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DPipelineBarrier
//! @brief パイプラインのバリア
struct V3DPipelineBarrier
{
	V3DFlags srcStageMask; //!< 移行元のパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存性を表す ::V3D_DEPENDENCY_FLAG 列挙定数の組み合わせです。
};

//! @struct V3DBufferMemoryBarrier
//! @brief バッファーメモリのバリア
struct V3DBufferMemoryBarrier
{
	V3DFlags srcAccessMask; //!< 移行元のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	uint32_t srcQueueFamily; //!< 移行元のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。

	IV3DBuffer* pBuffer; //!< バッファーです。
	uint64_t offset; //!< バッファーのメモリオフセットをバイト単位で指定します。
	uint64_t size; //!< バッファーのメモリオフセットからのサイズをバイト単位で指定します。
};

//! @struct V3DBufferViewMemoryBarrier
//! @brief バッファービューメモリのバリア
struct V3DBufferViewMemoryBarrier
{
	V3DFlags srcAccessMask; //!< 移行元のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	uint32_t srcQueueFamily; //!< 移行元のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。

	IV3DBufferView* pBufferView; //!< バッファービューです。
};

//! @struct V3DImageMemoryBarrier
//! @brief イメージメモリのバリア
struct V3DImageMemoryBarrier
{
	V3DFlags srcAccessMask; //!< 移行元のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	uint32_t srcQueueFamily; //!< 移行元のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。
	V3D_IMAGE_LAYOUT srcLayout; //!< 移行元のイメージレイアウトです。
	V3D_IMAGE_LAYOUT dstLayout; //!< 移行先のイメージレイアウトです。

	IV3DImage* pImage; //!< イメージです。
	uint32_t baseLevel; //!< イメージの最初のレベルです。
	uint32_t levelCount; //!< イメージのレベル数です。<br>0 を指定すると baseLevel から最後のレベルまでを指定したことになります。
	uint32_t baseLayer; //!< イメージの最初のレイヤーです。
	uint32_t layerCount; //!< イメージのレイヤー数です。<br>0 を指定すると baseLayer から最後のレイヤーまでを指定したことになります。
};

//! @struct V3DImageViewMemoryBarrier
//! @brief イメージビューメモリのバリア
struct V3DImageViewMemoryBarrier
{
	V3DFlags srcAccessMask; //!< 移行元のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	uint32_t srcQueueFamily; //!< 移行元のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。
	V3D_IMAGE_LAYOUT srcLayout; //!< 移行元のイメージレイアウトです。
	V3D_IMAGE_LAYOUT dstLayout; //!< 移行先のイメージレイアウトです。

	IV3DImageView* pImageView; //!< イメージビューです。
};

//! @struct V3DBarrierSetDesc
//! @brief バリアセットの記述
struct V3DBarrierSetDesc
{
	V3DPipelineBarrier pipeline; //!< パイプラインの依存です。

	uint32_t bufferMemoryCount; //!< バッファーのメモリ依存の数です。
	V3DBufferMemoryBarrier* pBufferMemories; //!< バッファーのメモリ依存の配列です。<br>bufferMemoryCount が 1 以上の場合、配列の要素数は bufferMemoryCount である必要があり、0 の場合は nullptr を指定します。

	uint32_t bufferViewMemoryCount; //!< バッファービューのメモリ依存の数です。
	V3DBufferViewMemoryBarrier* pBufferViewMemories; //!< バッファービューのメモリ依存の配列です。<br>bufferViewMemoryCount が 1 以上の場合、配列の要素数は bufferViewMemoryCount である必要があり、0 の場合は nullptr を指定します。

	uint32_t imageMemoryCount; //!< イメージのメモリ依存の数です。
	V3DImageMemoryBarrier* pImageMemories; //!< イメージのメモリ依存の配列です。<br>imageMemoryCount が 1 以上の場合、配列の要素数は imageMemoryCount である必要があり、0 の場合は nullptr を指定します。

	uint32_t imageViewMemoryCount; //!< イメージビューのメモリ依存の数です。
	V3DImageViewMemoryBarrier* pImageViewMemories; //!< イメージビューのメモリ依存の配列です。<br>imageViewMemoryCount が 1 以上の場合、配列の要素数は imageViewMemoryCount である必要があり、0 の場合は nullptr を指定します。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DBarrierSet
//! @brief バリアセットインターフェース
//! @sa IV3DDevice::CreateBarrier
class V3D_DLL_API IV3DBarrierSet : public IV3DDeviceChild
{
protected:
	//! @cond MISC
	virtual ~IV3DBarrierSet() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// イベント
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DEvent
//! @brief イベントインターフェース
//! @sa IV3DDevice::CreateEvent
class V3D_DLL_API IV3DEvent : public IV3DDeviceChild
{
public:
	//! @brief シグナル状態かどうかを確認します。
	//! @return シグナル状態の場合は true を返します。
	virtual bool IsSignaled() const = 0;

	//! @brief フェンスをリセットして、非シグナル状態にします。
	//! @retval V3D_OK フェンスのリセットに成功しました。
	//! @retval V3D_ERROR_FAIL フェンスのリセットに失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset() = 0;

	//! @brief セットして、シグナル状態にします。
	//! @retval V3D_OK フェンスのセットに成功しました。
	//! @retval V3D_ERROR_FAIL フェンスのセットに失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Set() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DEvent() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// フェンス
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DFence
//! @brief フェンスインターフェース
//! @sa IV3DDevice::CreateFence
class V3D_DLL_API IV3DFence : public IV3DDeviceChild
{
public:
	//! @brief シグナル状態かどうかを確認します。
	//! @return シグナル状態の場合は true を返します。
	virtual bool IsSignaled() const = 0;

	//! @brief フェンスをリセットして、非シグナル状態にします。
	//! @retval V3D_OK フェンスのリセットに成功しました。
	//! @retval V3D_ERROR_FAIL フェンスのリセットに失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset() = 0;
	//! @brief フェンスがシグナル状態になるまで待機します。
	//! @param[in] timeOut タイムアウト時間をナノ秒単位で指定します。
	//! @retval V3D_OK フェンスがシグナル状態になりました。
	//! @retval V3D_TIMEOUT フェンスは非シグナル状態です。
	//! @retval V3D_ERROR_FAIL フェンスは待機に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Wait(uint64_t timeOut = V3D_INFINITE) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DFence() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// セマフォ
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DSemaphore
//! @brief セマフォインターフェース
//! @sa IV3DDevice::CreateSemaphore
class V3D_DLL_API IV3DSemaphore : public IV3DDeviceChild
{
protected:
	//! @cond MISC
	virtual ~IV3DSemaphore() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// クエリプール
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUERY_TYPE
//! @brief クエリのタイプ
enum V3D_QUERY_TYPE : uint8_t
{
	V3D_QUERY_TYPE_OCCLUSION = 0, //!< オクルージョンクエリです。
	V3D_QUERY_TYPE_PIPELINE_STATISTICS = 1, //!< パイプラインの統計クエリです。
	V3D_QUERY_TYPE_TIMESTAMP = 2, //!< タイムスタンプクエリです。
};

//! @enum V3D_QUERY_RESULT_FLAG
//! @brief クエリの結果フラグ
enum V3D_QUERY_RESULT_FLAG : uint8_t
{
	V3D_QUERY_RESULT_WAIT = 0x00000001, //!< 指定したすべてのクエリの結果が取得できるまで待機します。
	V3D_QUERY_RESULT_WITH_AVAILABILITY = 0x00000002, //!< 結果が出ているクエリがあるかどうかを確認できるようにします。
	V3D_QUERY_RESULT_PARTIAL = 0x00000004, //!< 指定したすべてのクエリでなくても結果を取得できるようにします。
};

//! @enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
//! @brief クエリのパイプラインの統計タイプ
enum V3D_QUERY_PIPELINE_STATISTIC_TYPE : uint8_t
{
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_VERTICES = 0, //!< 入力された頂点の数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_PRIMITIVES = 1, //!< 入力されたプリミティブの数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_VERTEX_SHADER_INVOCATIONS = 2, //!< バーテックスシェーダーの呼び出し回数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_GEOMETRY_SHADER_INVOCATIONS = 3, //!< ジオメトリシェーダーの呼び出し回数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_GEOMETRY_SHADER_PRIMITIVES = 4, //!< ジオメトリシェーダーによって生成されたプリミティブの数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_CLIPPING_INVOCATIONS = 5, //!< クリッピングステージで処理されたプリミティブの数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_CLIPPING_PRIMITIVES = 6, //!< クリッピングステージによって出力されたされたプリミティブの数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_FRAGMENT_SHADER_INVOCATIONS = 7, //!< フラグメントシェーダーの呼び出し回数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_TESSELLATION_CONTROL_SHADER_PATCHES = 8, //!< テッセレーション制御シェーダーのよって処理されるパッチ数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_TESSELLATION_EVALUATION_SHADER_INVOCATIONS = 9, //!< テッセレーション評価シェーダーの呼び出し回数です。
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_COMPUTE_SHADER_INVOCATIONS = 10, //!< コンピュートシェーダーの呼び出し回数です。

	V3D_QUERY_PIPELINE_STATISTIC_TYPE_COUNT = 11, //!< クエリのパイプラインの統計タイプの数です。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DQueryPoolDesc
//! @brief クエリの記述
struct V3DQueryPoolDesc
{
	V3D_QUERY_TYPE queryType; //!< クエリのタイプです。
	uint32_t queryCount; //!< プールによって管理されるクエリの数です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DQueryPool
//! @brief クエリプールインターフェース
//! @sa IV3DDevice::CreateQueryPool
class V3D_DLL_API IV3DQueryPool : public IV3DDeviceChild
{
public:
	//! @brief クエリプールの記述を取得します
	//! @return クエリプールの記述を返します
	virtual const V3DQueryPoolDesc& GetDesc() const = 0;

	//! @brief クエリの結果を取得します。
	//! @param[in] firstQuery 結果を取得する最初のクエリのインデックスです。
	//! @param[in] queryCount 結果を取得するクエリの数です。
	//! @param[in] queryResultCount 取得するクエリの結果の数です。
	//! @param[in] pQueryResults queryResultCount の値の要素の数を持つクエリの結果を格納する uint64_t 型の配列です。
	//! @param[in] queryResultFlags クエリの結果の取得方法を表す ::V3D_QUERY_RESULT_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK クエリの結果を取得しました。
	//! @retval V3D_NOT_READY クエリの結果はまだ取得できていません。
	//! @retval V3D_ERROR_FAIL クエリの結果の取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! - 返り値が ::V3D_OK でなくてもクエリの結果を取得しようとした後は IV3DCommandBuffer::ResetQueryPool でクエリプールをリセットしてください。
	//! .
	//! - クエリのタイプが ::V3D_QUERY_TYPE_OCCLUSION または ::V3D_QUERY_TYPE_TIMESTAMP であった場合は queryResultCount の値は queryCount になります。
	//! .
	//! - クエリのタイプが ::V3D_QUERY_TYPE_PIPELINE_STATISTICS であった場合は queryResultCount の値は queryCount * 11 になり、それぞれの 0 ～ 10 要素の値の意味は ::V3D_QUERY_PIPELINE_STATISTIC_TYPE 列挙型を参照してください。
	//! .
	//! - クエリのタイプが ::V3D_QUERY_TYPE_TIMESTAMP であった場合は pQueryResults に書き込まれる数値の単位はナノ秒になります。
	//! .
	//! - flags に ::V3D_QUERY_RESULT_WAIT が含まれている場合は、クエリの結果が取得できるまで待機します。<br>
	//! ただしクエリがキューに送信されていない状態で結果を取得しようとすると、デッドロックをひきおこしますので注意が必要です。
	//! .
	//! - flags に ::V3D_QUERY_RESULT_PARTIAL が含まれている場合は、指定したすべてのクエリでなく、部分的なクエリの途中の結果を取得できるようになります。<br>
	//! ただし、クエリのタイプが ::V3D_QUERY_TYPE_TIMESTAMP であった場合、このフラグを指定することはできません。<br>
	//! .
	//! - flags に ::V3D_QUERY_RESULT_WITH_AVAILABILITY が含まれている場合は、queryResultCount は queryCount + 1 を必要とします。<br>
	//! そして最後の要素 ( pQueryResults[queryResultCount - 1] ) にクエリの結果が取得できた場合は 1 以上の値、取得できなかった場合は 0 が書き込まれます。
	virtual V3D_RESULT GetResults(uint32_t firstQuery, uint32_t queryCount, uint32_t queryResultCount, uint64_t* pQueryResults, V3DFlags queryResultFlags) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DQueryPool() {}
	//! @endcond
};

//! @}


// ----------------------------------------------------------------------------------------------------
// シェーダーモジュール
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DShaderModule
//! @brief シェーダーモジュールインターフェース
//! @sa IV3DDevice::CreateShaderModule
class V3D_DLL_API IV3DShaderModule : public IV3DDeviceChild
{
protected:
	//! @cond MISC
	virtual ~IV3DShaderModule() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// リソースメモリ
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DResourceMemoryDesc
//! @brief リソースメモリの記述
struct V3DResourceMemoryDesc
{
	//! @brief メモリのプロパティフラグを表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	V3DFlags propertyFlags;
	//! @brief メモリのサイズです。
	uint64_t size;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DResourceMemory
//! @brief リソースメモリインターフェース
class V3D_DLL_API IV3DResourceMemory : public IV3DDeviceChild
{
public:
	//! @brief リソースメモリの記述を取得します。
	//! @return リソースメモリの記述を返します。
	virtual const V3DResourceMemoryDesc& GetDesc() const = 0;

	//! @brief リソースメモリのマップ開始を通知します。
	//! @retval V3D_OK リソースメモリのマップ開始の通知に成功しました。
	//! @retval V3D_ERROR_FAIL リソースメモリのマップ開始の通知に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! このメソッドはメモリ全体のアクセスを有効な状態にします。<br>
	//! 一度に多くのリソースがこのメモリをアクセスする際 ( IV3DResource::Map および IV3DResource::Unmap ) のパフォーマンスを向上させることができます。
	virtual V3D_RESULT BeginMap() = 0;
	//! @brief リソースメモリのマップ終了を通知します。
	//! @retval V3D_OK リソースメモリのマップ終了の通知に成功しました。
	//! @retval V3D_ERROR_FAIL リソースメモリのマップ終了の通知に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT EndMap() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DResourceMemory() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// リソース
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_RESOURCE_TYPE
//! @brief リソースタイプ
enum V3D_RESOURCE_TYPE : uint8_t
{
	V3D_RESOURCE_TYPE_BUFFER = 0, //!< バッファーとして使用します。
	V3D_RESOURCE_TYPE_IMAGE = 1, //!< イメージとして使用します。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DResourceDesc
//! @brief リソースの記述
struct V3DResourceDesc
{
	//! @brief リソースのタイプです。
	V3D_RESOURCE_TYPE type;

	//! @brief メモリの特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	V3DFlags memoryPropertyFlags;
	//! @brief メモリタイプのビットマスクです。
	//! @remarks 各ビットは IV3DAdapter::GetMemoryTypeDesc の第一引数である memoryType を表します。
	uint32_t memoryTypeBits;
	//! @brief アライメントが適用されたメモリのサイズをバイト単位で指定します。
	uint64_t memorySize;
	//! @brief メモリのサイズのアライメントをバイト単位で指定します。
	uint64_t memoryAlignment;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DResource
//! @brief リソースインターフェース
class V3D_DLL_API IV3DResource : public IV3DDeviceChild
{
public:
	//! @brief リソースの記述を取得します。
	//! @return リソースの記述を返します。
	virtual const V3DResourceDesc& GetResourceDesc() const = 0;

	//! @brief リソースメモリを取得します。
	//! @param[out] ppMemory 取得したリソースメモリを渡す IV3DResourceMemory インターフェースのポインタのアドレスです。
	//! @retval V3D_OK リソースメモリの取得に成功しました。
	//! @retval V3D_ERROR_FAIL リソースメモリの取得に失敗しました。<br>
	//! リソースメモリを取得しようとしたリソースは、スワップチェインのイメージである可能性があります。
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) = 0;

	//! @brief リソースの指定されたメモリのポインタ有効にし、取得します。
	//! @param[in] offset 有効にするメモリのオフセットをバイト単位で指定します。
	//! @param[in] size 有効にするメモリのサイズをバイト単位で指定します。<br>
	//! また、0 を指定することで offset からメモリの最後までをマップします。
	//! @param[out] ppMemory 取得したメモリのポインタを渡す void ポインタのアドレスです。
	//! @retval V3D_OK リソースのメモリのポインタの取得に成功しました。
	//! @retval V3D_ERROR_FAIL リソースのメモリのポインタの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) = 0;
	//! @brief リソースの指定されたメモリのポインタを無効に、返却します。
	//! @retval V3D_OK リソースのメモリのポインタの返却に成功しました。
	//! @retval V3D_ERROR_FAIL リソースのメモリのポインタの返却に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Unmap() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DResource() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// バッファー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_BUFFER_USAGE_FLAG
//! @brief バッファーの使用法フラグ
enum V3D_BUFFER_USAGE_FLAG : V3DFlags
{
	//! @brief コピー元バッファとして使用します。
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	V3D_BUFFER_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief コピー先バッファとして使用します。
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	V3D_BUFFER_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief ユニフォームテクセルバッファをして使用します。
	V3D_BUFFER_USAGE_UNIFORM_TEXEL = 0x00000004,
	//! @brief ストレージテクセルバッファとして使用します。
	V3D_BUFFER_USAGE_STORAGE_TEXEL = 0x00000008,
	//! @brief ユニフォームバッファとして使用します。
	V3D_BUFFER_USAGE_UNIFORM = 0x00000010,
	//! @brief ストレージバッファとして使用します。
	V3D_BUFFER_USAGE_STORAGE = 0x00000020,
	//! @brief インデックスバッファとして使用します。
	V3D_BUFFER_USAGE_INDEX = 0x00000040,
	//! @brief バーテックスバッファとして使用します。
	V3D_BUFFER_USAGE_VERTEX = 0x00000080,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBufferDesc
//! @brief バッファーの記述
//! @sa IV3DDevice::CreateBuffer
//! @sa IV3DBuffer::GetDesc
struct V3DBufferDesc
{
	V3DFlags usageFlags; //!< バッファーの使用法を表す ::V3D_BUFFER_USAGE_FLAG 列挙定数の組み合わせです。
	uint64_t size; //!< バッファーのサイズです。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DBuffer
//! @brief バッファーインターフェース
//! @sa IV3DDevice::CreateBuffer
class V3D_DLL_API IV3DBuffer : public IV3DResource
{
public:
	//! @brief バッファーの記述を取得します。
	//! @return バッファーの記述を返します。
	virtual const V3DBufferDesc& GetDesc() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DBuffer() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// イメージ
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_IMAGE_TYPE
//! @brief イメージのタイプ
enum V3D_IMAGE_TYPE : uint8_t
{
	V3D_IMAGE_TYPE_1D = 0, //!< 1D ( width ) イメージです。
	V3D_IMAGE_TYPE_2D = 1, //!< 2D ( width height ) イメージです。
	V3D_IMAGE_TYPE_3D = 2, //!< 3D ( width height depth )イメージです。
};

//! @enum V3D_IMAGE_TILING
//! @brief イメージのタイリング
enum V3D_IMAGE_TILING : uint8_t
{
	//! @brief 最適なタイリングを使用します。<br>
	//! テクセルの配置は実装に依存することになるため、サブリソースにアクセスすることはできません。
	V3D_IMAGE_TILING_OPTIMAL = 0,
	//! @brief 線形タイリングを使用します。<br>
	//! テクセルは定められた順序で配置されるため、サブリースへのアクセスを可能にします。
	//! @sa IV3DImage::GetSubresourceLayout
	V3D_IMAGE_TILING_LINEAR = 1,
};

//! @enum V3D_IMAGE_USAGE_FLAG
//! @brief イメージの使用法フラグ
enum V3D_IMAGE_USAGE_FLAG : V3DFlags
{
	//! @brief コピー元イメージとして使用します。
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief コピー先イメージとして使用します。
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	//! @sa IV3DCommandBuffer::ClearImage
	V3D_IMAGE_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief シェーダーからサンプリングすることができるイメージとして使用します。
	//! @sa IV3DDescriptorSet::SetImageView
	//! @sa IV3DDescriptorSet::SetImageViewAndSampler
	V3D_IMAGE_USAGE_SAMPLED = 0x00000004,
	//! @brief キューブイメージとして使用します。
	V3D_IMAGE_USAGE_CUBE_COMPATIBLE = 0x00000008,
	//! @brief コンピュートシェーダーから書き込むことのできるストレージイメージとして使用します。
	V3D_IMAGE_USAGE_STORAGE = 0x00000010,
	//! @brief サブパスのカラーアタッチメントとして使用します。
	V3D_IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000020,
	//! @brief サブパスのデプスステンシルとして使用します。
	V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000040,
	//! @brief イメージのメモリプロパティに ::V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED を指定されていることを表します。<br>
	//! また以下の使用法を用いて作成されたイメージに最適です。<br>
	//! ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT<br>
	//! ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT<br>
	//! ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT<br>
	V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT = 0x0000080,
	//! @brief サブパスのインプットアタッチメントとして使用します。<br>
	//! このイメージはフラグメントシェーダーのみが読み取ることができます。
	V3D_IMAGE_USAGE_INPUT_ATTACHMENT = 0x00000100,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageDesc
//! @brief イメージの記述
//! @sa IV3DDevice::CreateImage
//! @sa IV3DImage::GetDesc
struct V3DImageDesc
{
	V3D_IMAGE_TYPE type; //!< イメージのタイプです。
	V3D_FORMAT format; //!< イメージのフォーマットです。
	uint32_t width; //!< イメージの幅です。
	uint32_t height; //!< イメージの高さです。
	uint32_t depth; //!< イメージの深さです。
	uint32_t levelCount; //!< イメージのレベル数です。
	uint32_t layerCount; //!< イメージのレイヤー数です。
	V3D_SAMPLE_COUNT_FLAG samples; //!< イメージのサンプル数です。
	V3D_IMAGE_TILING tiling; //!< イメージのタイリングです。
	V3DFlags usageFlags; //!< イメージの使用法を表す ::V3D_IMAGE_USAGE_FLAG 列挙定数の組み合わせです。
};

//! @struct V3DImageSubresourceLayout
//! @brief イメージのサブリソースレイアウト
//! @sa IV3DImage::GetSubresourceLayout
struct V3DImageSubresourceLayout
{
	uint64_t offset; //!< メモリのオフセットをバイト単位で指定します。
	uint64_t size; //!< メモリのサイズをバイト単位で指定します。
	uint64_t rowPitch; //!< 行のピッチをバイト単位で指定します。
	uint64_t layerPitch; //!< レイヤーのピッチをバイト単位で指定します。
	uint64_t depthPitch; //!< 深さのピッチですをバイト単位で指定します。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DImage
//! @brief イメージインターフェース
//! @sa IV3DDevice::CreateImage
class V3D_DLL_API IV3DImage : public IV3DResource
{
public:
	//! @brief イメージの記述を取得します。
	//! @return イメージの記述を返します。
	virtual const V3DImageDesc& GetDesc() const = 0;
	//! @brief イメージのサブリソースレイアウトを取得します。
	//! @param[in] level サブリソースのレベルです。
	//! @param[in] layer サブリソースのレイヤーです。
	//! @return イメージのサブリソースレイアウトを返します。
	//! @remarks
	//! サブリソースのレイアウトを取得できるようにするためには、イメージのタイリングに ::V3D_IMAGE_TILING_LINEAR が指定されている必要があります。
	virtual const V3DImageSubresourceLayout& GetSubresourceLayout(uint32_t level, uint32_t layer) const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DImage() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// バッファービュー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBufferViewDesc
//! @brief バッファービューの記述
//! @sa IV3DDevice::CreateBufferView
//! @sa IV3DBufferView::GetDesc
struct V3DBufferViewDesc
{
	V3D_FORMAT format; //!< フォーマットです。
	uint64_t offset; //!< アクセスするバッファのメモリのオフセットをバイト単位で指定します。
	uint64_t size; //!< アクセスするバッファのメモリのオフセットからのサイズをバイト単位で指定します。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DBufferView
//! @brief バッファービューインターフェース
//! @sa IV3DDevice::CreateBufferView
class V3D_DLL_API IV3DBufferView : public IV3DDeviceChild
{
public:
	//! @brief バッファービューの記述を取得します。
	//! @return バッファビューの記述を表す V3DBufferViewDesc 構造体のアドレスを返します。
	virtual const V3DBufferViewDesc& GetDesc() const = 0;
	//! @brief バッファービューがアクセスするバッファを取得します。
	//! @param[out] ppBuffer 取得したバッファービューを表す IV3DBuffer インターフェースのポインタのアドレスです。
	virtual void GetBuffer(IV3DBuffer** ppBuffer) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DBufferView() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// イメージビュー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_IMAGE_VIEW_TYPE
//! @brief イメージビューのタイプ
enum V3D_IMAGE_VIEW_TYPE : uint8_t
{
	V3D_IMAGE_VIEW_TYPE_1D = 0, //!< 1D ( width ) イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_2D = 1, //!< 2D ( width height ) イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_3D = 2, //!< 3D ( width height depth ) イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_CUBE = 3, //!< キューブイメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_1D_ARRAY = 4, //!< 1D 配列イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_2D_ARRAY = 5, //!< 2D 配列イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY = 6, //!< キューブ配列イメージビューとして使用します。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageViewDesc
//! @brief イメージビューの記述
//! @sa IV3DDevice::CreateImageView
//! @sa IV3DImageView::GetDesc
struct V3DImageViewDesc
{
	V3D_IMAGE_VIEW_TYPE type; //!< イメージビューのタイプです。
	uint32_t baseLevel; //!< 最初のレベルです。
	uint32_t levelCount; //!< レベル数です。
	uint32_t baseLayer; //!< 最初のレイヤーです。
	uint32_t layerCount; //!< レーヤー数です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DImageView
//! @brief イメージビューインターフェース
//! @sa IV3DDevice::CreateImageView
class V3D_DLL_API IV3DImageView : public IV3DDeviceChild
{
public:
	//! @brief イメージビューの記述を取得します。
	//! @return イメージビューの記述を表す V3DImageViewDesc 構造体のアドレスを返します。
	virtual const V3DImageViewDesc& GetDesc() const = 0;
	//! @brief アクセスするイメージを取得します。
	//! @param[out] ppImage 取得したイメージを渡す IV3DImage インターフェースのポインタのアドレスです。
	virtual void GetImage(IV3DImage** ppImage) = 0;
	//! @brief アクセスするイメージのサブリソースのサイズを取得します。
	//! @return アクセスするイメージのサブリソースのサイズを返します。
	virtual const V3DSize3D& GetImageSubresourceSize() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DImageView() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// サンプラー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_FILTER
//! @brief フィルター
enum V3D_FILTER : uint8_t
{
	//! @brief 最も近い座標のテクセルを使用します。
	V3D_FILTER_NEAREST = 0,
	//! @brief イメージの次元数に応じたテクセルの加重平均を使用します。<br>
	//! <ul>
	//! <li>1Dイメージ</li><br>
	//! 2x1<br>
	//! <li>2Dイメージ、キューブイメージ</li><br>
	//! 2x2<br>
	//! <li>3Dイメージ</li><br>
	//! 2x2x2<br>
	//! </ul>
	V3D_FILTER_LINEAR = 1,
};

//! @enum V3D_MIPMAP_MODE
//! @brief ミップマップモード
enum V3D_MIPMAP_MODE : uint8_t
{
	//! @brief 最も近い座標のテクセルを使用します。
	V3D_MIPMAP_MODE_NEAREST = 0,
	//! @brief イメージの次元数に応じたテクセルの加重平均を使用します。
	V3D_MIPMAP_MODE_LINEAR = 1,
};

//! @enum V3D_ADDRESS_MODE
//! @brief アドレスモード
enum V3D_ADDRESS_MODE : uint8_t
{
	//! @brief リピートします。
	V3D_ADDRESS_MODE_REPEAT = 0,
	//! @brief 反転してリピートします。
	V3D_ADDRESS_MODE_MIRRORED_REPEAT = 1,
	//! @brief エッジを繰り返します。
	V3D_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
	//! @brief 指定された境界線の色を繰り返します。
	//! @sa V3D_BORDER_COLOR
	V3D_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
};

//! @enum V3D_BORDER_COLOR
//! @brief ボーダーカラー
enum V3D_BORDER_COLOR : uint8_t
{
	V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK = 0, //!< RGBA(0.0f, 0.0f, 0.0f, 0.0f)
	V3D_BORDER_COLOR_INT_TRANSPARENT_BLACK = 1, //!< RGBA(0, 0, 0, 0)
	V3D_BORDER_COLOR_FLOAT_OPAQUE_BLACK = 2, //!< RGBA(0.0f, 0.0f, 0.0f, 1.0f)
	V3D_BORDER_COLOR_INT_OPAQUE_BLACK = 3, //!< RGBA(0, 0, 0, 1)
	V3D_BORDER_COLOR_FLOAT_OPAQUE_WHITE = 4, //!< RGBA(1.0f, 1.0f, 1.0f, 1.0f)
	V3D_BORDER_COLOR_INT_OPAQUE_WHITE = 5, //!< RGBA(1, 1, 1, 1)
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DSamplerDesc
//! @brief サンプラの記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>magFilter</td><td>V3D_FILTER_NEAREST</td></tr>
//! <tr><td>minFilter</td><td>V3D_FILTER_NEAREST</td></tr>
//! <tr><td>mipmapMode</td><td>V3D_MIPMAP_MODE_NEAREST</td></tr>
//! <tr><td>addressModeU</td><td>V3D_ADDRESS_MODE_REPEAT</td></tr>
//! <tr><td>addressModeV</td><td>V3D_ADDRESS_MODE_REPEAT</td></tr>
//! <tr><td>addressModeW</td><td>V3D_ADDRESS_MODE_REPEAT</td></tr>
//! <tr><td>mipLodBias</td><td>0.0f</td></tr>
//! <tr><td>anisotropyEnable</td><td>false</td></tr>
//! <tr><td>maxAnisotropy</td><td>0.0f</td></tr>
//! <tr><td>compareEnable</td><td>false</td></tr>
//! <tr><td>compareOp</td><td>VK_COMPARE_OP_ALWAYS</td></tr>
//! <tr><td>minLod</td><td>0.0f</td></tr>
//! <tr><td>maxLod</td><td>0.0f</td></tr>
//! <tr><td>borderColor</td><td>V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK</td></tr>
//! </table>
//! @sa IV3DDevice::CreateSampler
//! @sa IV3DSampler::GetDesc
struct V3DSamplerDesc
{
	V3D_FILTER magFilter; //!< 拡大フィルタです。
	V3D_FILTER minFilter; //!< 縮小フィルタです。
	V3D_MIPMAP_MODE mipmapMode; //!< ミップマップモード
	V3D_ADDRESS_MODE addressModeU; //!< U 座標のアドレッシングモードです。
	V3D_ADDRESS_MODE addressModeV; //!< V 座標のアドレッシングモードです。
	V3D_ADDRESS_MODE addressModeW; //!< W 座標のアドレッシングモードです。
	float mipLodBias; //!< ミップマップのレベルのオフセットです。<br>この値は { - V3DDeviceCaps::maxSamplerLodBias <= mipLodBias <= + V3DDeviceCaps::maxSamplerLodBias } の範囲に制限されます。
	bool anisotropyEnable; //!< 異方性フィルタリングを有効にするかどうかを指定します。<br>この値は V3DDeviceCaps::flags に ::V3D_CAP_SAMPLER_ANISOTROPY が含まれている場合にのみ true を指定することができます。
	float maxAnisotropy; //!< 異方性値をクランプする値です。<br>この値は anisotropyEnable が true のときに有効になり V3DDeviceCaps::maxSamplerAnisotropy 以下に制限されます。
	bool compareEnable; //!< 比較オペーレーターである compareOp を有効にするかどうかを指定します。
	V3D_COMPARE_OP compareOp; //!< 比較演算子です。<br> ::V3D_COMPARE_OP の説明にある R はサンプラーによって入力される値であり、S はアタッチメントのテクセルの値を表します。
	float minLod; //!< ミップマップのレベルをクランプする最小値です。<br>通常この値は最初のミップマップのレベルを指定します。
	float maxLod; //!< ミップマップのレベルをクランプする最大値です。<br>通常この値は最後ミップマップのレベルを指定します。
	V3D_BORDER_COLOR borderColor; //!< 境界線の色です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DSampler
//! @brief サンプラーインターフェース
//! @sa IV3DDevice::CreateSampler
class V3D_DLL_API IV3DSampler : public IV3DDeviceChild
{
public:
	//! @brief サンプラーの記述を取得します。
	//! @return サンプラーの記述を返します。
	virtual const V3DSamplerDesc& GetDesc() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DSampler() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// レンダーパス
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_ATTACHMENT_LOAD_OP
//! @brief アタッチメントのロード操作
enum V3D_ATTACHMENT_LOAD_OP : uint8_t
{
	//! @brief 最初のサブパスが開始された際にアタッチメントの以前の内容が保持されている必要がないことを表します。<br>
	//! またアタッチメントのアクセスには、 ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE または ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_UNDEFINED = 0,
	//! @brief 最初のサブパスが開始された際にアタッチメントの以前の内容を読み込みます。<br>
	//! またアタッチメントのアクセスには、 ::V3D_ACCESS_COLOR_ATTACHMENT_READ または ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_LOAD = 1,
	//! @brief 最初のサブパスが開始された際にアタッチメントをクリアします。
	//! またアタッチメントのアクセスには、 ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE または ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_CLEAR = 2,
};

//! @enum V3D_ATTACHMENT_STORE_OP
//! @brief アタッチメントのストア操作
enum V3D_ATTACHMENT_STORE_OP : uint8_t
{
	//! @brief 最後のサブパスが終了した際にアタッチメントの内容が保存されず、破棄される可能性があることを表します。<br>
	//! またアタッチメントのアクセスには、 ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE または ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE が含まれている必要があります。
	V3D_ATTACHMENT_STORE_OP_UNDEFINED = 0,
	//! @brief 最後のサブパスが終了した際にアタッチメントの内容を保存します。<br>
	//! またアタッチメントのアクセスには、 ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE または ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE が含まれている必要があります。
	V3D_ATTACHMENT_STORE_OP_STORE = 1,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAttachmentDesc
//! @brief アタッチメントの記述
//! @sa IV3DDevice::CreateRenderPass
struct V3DAttachmentDesc
{
	//! @brief アタッチメントのフォーマットです。
	V3D_FORMAT format;
	//! @brief アタッチメントのサンプル数です。
	V3D_SAMPLE_COUNT_FLAG samples;
	//! @brief アタッチメントのロードオペレーションです。
	V3D_ATTACHMENT_LOAD_OP loadOp;
	//! @brief アタッチメントのストアオペレーションです。
	V3D_ATTACHMENT_STORE_OP storeOp;
	//! @brief アタッチメントのステンシルのロードオペレーションです。
	V3D_ATTACHMENT_LOAD_OP stencilLoadOp;
	//! @brief アタッチメントのステンシルのストアオペレーションです。
	V3D_ATTACHMENT_STORE_OP stencilStoreOp;
	//! @brief レンダーパス開始時のアタッチメントのイメージレイアウトです。
	V3D_IMAGE_LAYOUT initialLayout;
	//! @brief レンダーパス終了時のアタッチメントのイメージレイアウトです。
	V3D_IMAGE_LAYOUT finalLayout;
	//! @brief レンダーパス開始時のアタッチメントのクリア値です。<br>
	//! この値は loadOp または stencilLoadOp が ::V3D_ATTACHMENT_LOAD_OP_CLEAR のときに有効です。
	V3DClearValue clearValue;
};

//! @struct V3DAttachmentReference
//! @brief アタッチメントの参照
//! @sa V3DSubpassDesc
struct V3DAttachmentReference
{
	uint32_t attachment; //!< 参照するアタッチメントです。<br>IV3DDevice::CreateRenderPass で指定した V3DAttachmentDesc 構造体の配列のインデックスです。
	V3D_IMAGE_LAYOUT layout; //!< サブパス開始時のアタッチメントのイメージレイアウトです。
};

//! @struct V3DSubpassDesc
//! @brief サブパスの記述
//! @sa IV3DDevice::CreateRenderPass
struct V3DSubpassDesc
{
	//! @brief フラグメントシェーダーに入力するインプットアタッチメントの数です。
	uint32_t inputAttachmentCount;
	//! @brief フラグメントシェーダーに入力するインプットアタッチメントの参照配列です。<br>
	//! - inputAttachmentCount が 1 以上の場合、配列の要素数は inputAttachmentCount である必要があり、0 の場合は nullptr を指定します。<br>
	//! - アタッチメントのイメージの使用法 ( V3DImageDesc::usageFlags ) には ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT が含まれている必要があります。
	const V3DAttachmentReference* pInputAttachments;

	//! @brief 出力先のカラーアタッチメントの数です。
	uint32_t colorAttachmentCount;
	//! @brief 出力先のカラーアタッチメントの参照配列です。<br>
	//! - colorAttachmentCount が 1 以上の場合、配列の要素数は colorAttachmentCount である必要があり、0 の場合は nullptr を指定します。<br>
	//! - アタッチメントのイメージの使用法 ( V3DImageDesc::usageFlags ) には ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT が含まれている必要があります。
	const V3DAttachmentReference* pColorAttachments;
	//! @brief 出力先のマルチサンプルカラーアタッチメント ( pColorAttachments ) の変換先のアタッチメントの参照配列です。<br>
	//! - カラーアタッチメント ( pColorAttachments ) のサンプル数が ::V3D_SAMPLE_COUNT_1 でなく colorAttachmentCount が 1 以上の場合、配列の要素数は colorAttachmentCount である必要があり、それ以外の場合は nullptr を指定します。<br>
	//! - アタッチメントのイメージの使用法 ( V3DImageDesc::usageFlags ) には ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT が含まれている必要があり、サンプル数 ( V3DImageDesc::samples ) は ::V3D_SAMPLE_COUNT_1 である必要があります。<br>
	//! また、変換する必要がないアタッチメントの場合は V3DAttachmentReference::attachment に ::V3D_ATTACHMENT_UNUSED を指定してください。<br>
	const V3DAttachmentReference* pResolveAttachments;

	//! @brief 出力先のデプスステンシルアタッチメントの参照です。<br>
	//! - デプスステンシルアタッチメントが存在しない場合は nullptr を指定してください。<br>
	//! - アタッチメントのイメージの使用法 ( V3DImageDesc::usageFlags ) には ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT が含まれている必要があり、<br>
	//! サンプル数 ( V3DImageDesc::samples ) はカラーアタッチメントと同じである必要があります。<br>
	const V3DAttachmentReference* pDepthStencilAttachment;

	//! @brief このサブパス参照されてなく、内容を保持したいアタッチメントの数です。
	uint32_t preserveAttachmentCount;
	//! @brief このサブパスで参照されてなく、内容を保持したいアタッチメントのインデックス配列です。<br>
	//! - preserveAttachmentCount が 1 以上の場合、配列の要素数は preserveAttachmentCount である必要があり、0 の場合は nullptr を指定します。
	const uint32_t* pPreserveAttachments;
};

//! @struct V3DSubpassDependencyDesc
//! @brief サブパスの依存性の記述
//! @sa IV3DDevice::CreateRenderPass
struct V3DSubpassDependencyDesc
{
	uint32_t srcSubpass; //!< 移行元のサブパスのインデックスです。
	uint32_t dstSubpass; //!< 移行先のサブパスのインデックスです。

	V3DFlags srcStageMask; //!< 移行元のパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。

	V3DFlags srcAccessMask; //!< 移行元のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す ::V3D_ACCESS_FLAG 列挙定数の組み合わせです。

	//! 依存を表す ::V3D_DEPENDENCY_FLAG 列挙定数の組み合わせです。<br>
	//! srcSubpass または dstSubpass に ::V3D_SUBPASS_EXTERNAL が指定されている場合は ::V3D_DEPENDENCY_BY_REGION を含めてはいけません。
	V3DFlags dependencyFlags;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DRenderPass
//! @brief レンダーパスインターフェース
//! @sa IV3DDevice::CreateRenderPass
class V3D_DLL_API IV3DRenderPass : public IV3DDeviceChild
{
protected:
	//! @cond MISC
	virtual ~IV3DRenderPass() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// フレームバッファー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DFrameBufferDesc
//! @brief フレームバッファーの記述
//! @sa IV3DFrameBuffer::GetDesc
struct V3DFrameBufferDesc
{
	uint32_t attachmentWidth; //!< アタッチメントの幅です。
	uint32_t attachmentHeight; //!< アタッチメントの高さです。
	uint32_t attachmentDepth; //!< アタッチメントの深さです。
	uint32_t attachmentLayerCount; //!< アタッチメントのレイヤー数です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DFrameBuffer
//! @brief フレームバッファーインターフェース
//! @sa IV3DDevice::CreateFrameBuffer
class V3D_DLL_API IV3DFrameBuffer : public IV3DDeviceChild
{
public:
	//! @brief フレームバッファーの記述を取得します。
	//! @return フレームバッファーの記述を返します。
	virtual const V3DFrameBufferDesc& GetDesc() const = 0;

	//! @brief レンダーパスを取得します。
	//! @param[out] ppRenderPass 取得したレンダーパスを渡す IV3DRenderPass インターフェースのポインタのアドレスです。
	virtual void GetRenderPass(IV3DRenderPass** ppRenderPass) = 0;

	//! @brief アタッチメントの数を取得します。
	//! @return アタッチメントの数を返します。
	virtual uint32_t GetAttachmentCount() const = 0;
	//! @brief アタッチメントを取得します
	//! @param[in] attachmentIndex アタッチメントのインデックスです。
	//! @param[out] ppImageView 取得したアタッチメントを渡す IV3DImageView インターフェースのポインタのアドレスです。
	virtual void GetAttachment(uint32_t attachmentIndex, IV3DImageView** ppImageView) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DFrameBuffer() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// デスクリプタセットレイアウト
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_DESCRIPTOR_TYPE
//! @brief デスクリプタのタイプ
enum V3D_DESCRIPTOR_TYPE : uint8_t
{
	//! @brief サンプラーとして使用します。
	V3D_DESCRIPTOR_TYPE_SAMPLER = 0,
	//! @brief サンプリング可能なイメージとサンプラーのセットで使用します。
	V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
	//! @brief サンプリング可能なイメージとして使用します。
	V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
	//! @brief ストレージイメージとして使用します。
	V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
	//! @brief ユニフォームテクセルバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
	//! @brief ストレージテクセルバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
	//! @brief ユニフォームバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
	//! @brief ストレージバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
	//! @brief ダイナミックユニフォームバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
	//! @brief ダイナミックストレージバッファーとして使用します。
	V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
	//! @brief インプットアタッチメントとして使用します。
	V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
};

//! @enum V3D_DESCRIPTOR_SET_TYPE
//! @brief デスクリプタセットのタイプ
enum V3D_DESCRIPTOR_SET_TYPE : uint8_t
{
	V3D_DESCRIPTOR_SET_TYPE_STANDARD = 0, //!< IV3DCommandBuffer::BindDescriptorSets でバインドするスタンダードデスクリプタセットです。
	V3D_DESCRIPTOR_SET_TYPE_PUSH = 1, //!< IV3DCommandBuffer::PushDescriptorSets で更新するプッシュデスクリプタセットです。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDescriptorDesc
//! @brief デスクリプタの記述
//! @sa IV3DDevice::CreateDescriptorSetLayout
struct V3DDescriptorDesc
{
	uint32_t binding; //!< バインディングです。
	V3D_DESCRIPTOR_TYPE type; //!< デスクリプタのタイプです。
	V3DFlags stageFlags; //!< バインドするシェーダーステージを表す ::V3D_SHADER_STAGE_FLAG 列挙定数の組み合わせです。
};

//! @struct V3DDescriptorSetLayoutStatistics
//! @brief デスクリプタセットレイアウトの統計
//! @sa IV3DDescriptorSetLayout::GetStatistics
struct V3DDescriptorSetLayoutStatistics
{
	uint32_t poolCount; //!< 作成されたプールの数です。
	uint32_t setCount; //!< 作成されたデスクリプタセットの数です。
	uint32_t setCapacity; //!< 作成できるデスクリプタセットの最大数です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDescriptorSetLayout
//! @brief デスクリプタセットレイアウトインターフェース
//! @sa IV3DDevice::CreateDescriptorSetLayout
class V3D_DLL_API IV3DDescriptorSetLayout : public IV3DDeviceChild
{
public:
	//! @brief デスクリプタセットのタイプを取得します。
	//! @return デスクリプタセットのタイプを返します。
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const = 0;
	//! @brief デスクリタセットレイアウトの統計を取得します。
	//! @return デスクリタセットレイアウトの統計を返します。
	virtual const V3DDescriptorSetLayoutStatistics& GetStatistics() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDescriptorSetLayout() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// デスクリプタセット
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDescriptorSet
//! @brief デスクリプタセットインターフェース
//! @sa IV3DDevice::CreateDescriptorSet
class V3D_DLL_API IV3DDescriptorSet : public IV3DDeviceChild
{
public:
	//! @brief デスクリプタセットのタイプを取得します。
	//! @return デスクリプタセットのタイプを返します。
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const = 0;
	//! @brief デスクリプタセットのレイアウトを取得します。
	//! @param[out] ppLayout 取得したデスクリプタセットのレイアウトを渡す IV3DDescriptorSetLayout インタフェースのポインタのアドレスです。
	virtual void GetLayout(IV3DDescriptorSetLayout** ppLayout) = 0;

	//! @brief バッファーを取得します。
	//! @param[in] binding 取得するバッファーのバインディングです。
	//! @param[out] ppBuffer 取得したバッファーを渡す IV3DBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK バッファーの取得に成功しました。
	//! @retval V3D_ERROR_FAIL バッファーの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBuffer(uint32_t binding, IV3DBuffer** ppBuffer) = 0;
	//! @brief バッファーを設定します。
	//! @param[in] binding バッファーを設定するバインディングです。
	//! @param[in] pBuffer 設定するバッファーです。
	//! @param[in] offset 設定するバッファーのオフセットをバイト単位で指定します。
	//! @param[in] size 設定するバッファーのオフセットからのサイズをバイト単位で指定します。
	//! @retval V3D_OK バッファーの設定に成功しました。
	//! @retval V3D_ERROR_FAIL バッファの設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! デスクリプタの種類によってアライメントが異なるため、offset には適切な値を指定してください。<br>
	//! 各デスクリプタのアライメントは V3DDeviceCaps に記述されており、以下のようになります。<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! また、指定したバインディングのデスクリプタが ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC または ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC の場合、<br>
	//! size に指定する値は、バッファー自体のサイズではなく、シェーダーが読み書きする範囲のサイズを指定します。<br>
	//! <br>
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	virtual V3D_RESULT SetBuffer(uint32_t binding, IV3DBuffer* pBuffer, uint64_t offset, uint64_t size) = 0;

	//! @brief バッファービューを取得します。
	//! @param[in] binding 取得するバッファービューのバインディングです。
	//! @param[out] ppBufferView 取得したバッファービューを渡す IV3DBufferView インターフェースのポインタのアドレスです。
	//! @retval V3D_OK バッファービューの取得に成功しました。
	//! @retval V3D_ERROR_FAIL バッファービューの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) = 0;
	//! @brief バッファービューを設定します。
	//! @param[in] binding バッファービューを設定するバインディングです。
	//! @param[in] pBufferView 設定するバッファービューのポインタです。
	//! @retval V3D_OK バッファービューの設定に成功しました。
	//! @retval V3D_ERROR_FAIL バッファービューの設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
	virtual V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) = 0;

	//! @brief イメージビューを取得します。
	//! @param[in] binding 取得するイメージのバインディングです。
	//! @param[out] ppImageView 取得したイメージビューを渡す IV3DImageView インターフェースのポインタのアドレスです。
	//! @retval V3D_OK イメージビューの取得に成功しました。
	//! @retval V3D_ERROR_FAIL イメージビューの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) = 0;
	//! @brief デスクリプタセットがバインドされた際のイメージレイアウトを取得します。
	//! @param[in] binding 取得するイメージレイアウトのバインディングです。
	//! @param[out] pImageLayout 取得したイメージレイアウトを格納する ::V3D_IMAGE_LAYOUT 列挙定数のポインタです。
	//! @retval V3D_OK イメージレイアウトの取得に成功しました。
	//! @retval V3D_ERROR_FAIL イメージレイアウトの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) = 0;
	//! @brief イメージビューを設定します。
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @retval V3D_OK イメージビューの設定に成功しました。
	//! @retval V3D_ERROR_FAIL イメージビューの設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br> 
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) = 0;
	//! @brief イメージビューを設定します。<br>
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @param[in] imageLayout デスクリプタセットをバインドする際のイメージレイアウトです。<br>
	//! デスクリプタセットをバインドする前にこのイメージレイアウトに移行しておく必要があります。
	//! @retval V3D_OK イメージビューの設定に成功しました。
	//! @retval V3D_ERROR_FAIL イメージビューの設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) = 0;

	//! @brief サンプラーを取得します。
	//! @param[in] binding 取得するサンプラーのバインディングです。
	//! @param[out] ppSampler 取得したサンプラーを渡す IV3DSampler インターフェースのポインタのアドレスです。
	//! @retval V3D_OK サンプラーの取得に成功しました。
	//! @retval V3D_ERROR_FAIL サンプラーの取得に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) = 0;
	//! @brief サンプラーを設定します。<br>
	//! @param[in] binding サンプラーを設定するバインディングです。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK サンプラーの設定に成功しました。
	//! @retval V3D_ERROR_FAIL サンプラーの設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) = 0;

	//! @brief イメージビューとサンプラーを設定します。<br>
	//! このバインディングのデスクリプタは 
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER である必要があります。
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK メージビューとサンプラーを設定に成功しました。
	//! @retval V3D_ERROR_FAIL メージビューとサンプラーを設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! 設定したイメージビューはデスクリプタセットをバインドする前にイメージレイアウトを ::V3D_IMAGE_LAYOUT_SHADER_READ_ONLY に移行しておく必要があります。<br>
	//! <br>
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, IV3DSampler* pSampler) = 0;

	//! @brief イメージビューとサンラプーを設定します。<br>
	//! このバインディングのデスクリプタは 
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER である必要があります。
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @param[in] imageLayout デスクリプタセットをバインドする際のイメージレイアウトです。<br>
	//! デスクリプタセットをバインドする前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK メージビューとサンプラーを設定に成功しました。
	//! @retval V3D_ERROR_FAIL メージビューとサンプラーを設定に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! サポートしているデスクリプタタイプ<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) = 0;

	//! @brief デスクリプタセットを更新します
	//! @remarks
	//! - デスクリプタセットに各種リソースを設定した際に実行する必要があります。<br>
	//! ただし、デスクリプタセットのタイプが ::V3D_DESCRIPTOR_SET_TYPE_PUSH の場合、このメソッドを実行する必要はありません。
	virtual void Update() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDescriptorSet() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// パイプラインレイアウト
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DConstantDesc
//! @brief プッシュ定数の記述
//! @sa IV3DDevice::CreatePipelineLayout
struct V3DConstantDesc
{
	V3DFlags shaderStageFlags; //!< 定数をプッシュするシェーダーステージを表す ::V3D_SHADER_STAGE_FLAG 列挙定数の組み合わせです。
	uint32_t offset; //!< 定数のメモリオフセットをバイト単位で指定します。
	uint32_t size; //!< 定数のメモリサイズをバイト単位で指定します。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DPipelineLayout
//! @brief パイプラインレイアウトインターフェース
//! @sa IV3DDevice::CreatePipelineLayout
class V3D_DLL_API IV3DPipelineLayout : public IV3DDeviceChild
{
public:
	//! @brief 定数の数を取得します。
	//! @return 定数の数を返します。
	virtual uint32_t GetConstantCount() const = 0;
	//! @brief 定数の記述を取得します。
	//! @param[in] constant 定数のインデックスです。
	//! @return 定数の記述を表す V3DConstantDesc 構造体のアドレスを返します。
	virtual const V3DConstantDesc& GetConstantDesc(uint32_t constant) const = 0;

	//! @brief デスクリプタセットの数を取得します。
	//! @return デスクリプタセットの数を返します。
	virtual uint32_t GetDescriptorSetCount() const = 0;
	//! @brief デスクリプタセットのレイアウトを取得します。
	//! @param[in] descriptorSetIndex デスクリプタセットのインデックスです。
	//! @param[out] ppDescriptorSetLayout 取得したデスクリプタセットのレイアウトを渡す IV3DDescriptorSetLayout インターフェースのポインタのアドレスです。
	virtual void GetDescriptorSetLayout(uint32_t descriptorSetIndex, IV3DDescriptorSetLayout** ppDescriptorSetLayout) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DPipelineLayout() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// パイプライン
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_PIPELINE_TYPE
//! @brief パイプラインのタイプ
enum V3D_PIPELINE_TYPE : uint8_t
{
	V3D_PIPELINE_TYPE_GRAPHICS = 0, //!< グラフィックスパイプラインとして使用します。
	V3D_PIPELINE_TYPE_COMPUTE = 1, //!< コンピュートパイプラインとして使用します。
};

//! @enum V3D_PRIMITIVE_TOPOLOGY
//! @brief プリミティブトポロジ
enum V3D_PRIMITIVE_TOPOLOGY : uint8_t
{
	V3D_PRIMITIVE_TOPOLOGY_POINT_LIST = 0, //!< ポイントのリストです。
	V3D_PRIMITIVE_TOPOLOGY_LINE_LIST = 1, //!< ラインのリストです。
	V3D_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2, //!< ラインのストリップです。
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3, //!< 三角形のリストです。
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4, //!< 三角形のストリップです。
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5, //!< 三角形のファンです。
	V3D_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
	V3D_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
	V3D_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10, //!< テッセレーションのパッチリストです。
};

//! @enum V3D_POLYGON_MODE
//! @brief ポリゴンモード
enum V3D_POLYGON_MODE : uint8_t
{
	V3D_POLYGON_MODE_FILL = 0, //!< ポリゴンを塗りつぶして描画します。
	V3D_POLYGON_MODE_LINE = 1, //!< ポリゴンをエッジを描画します。
	V3D_POLYGON_MODE_POINT = 2, //!< ポリゴンの頂点を描画します。
};

//! @enum V3D_CULL_MODE
//! @brief カリングモード
enum V3D_CULL_MODE : uint8_t
{
	V3D_CULL_MODE_NONE = 0, //!< カリングをしません。
	V3D_CULL_MODE_FRONT = 1, //!< 前面をカリングします。
	V3D_CULL_MODE_BACK = 2, //!< 後面をカリングします。
};

//! @enum V3D_STENCIL_OP
//! @brief ステンシルオペレーション
enum V3D_STENCIL_OP : uint8_t
{
	V3D_STENCIL_OP_KEEP = 0, //!< 現在の値を保持します。
	V3D_STENCIL_OP_ZERO = 1, //!< 値を 0 に設定します。
	V3D_STENCIL_OP_REPLACE = 2, //!< IV3DCommandBuffer::SetStencilReference で設定された値に置き換えます。
	V3D_STENCIL_OP_INCREMENT_AND_CLAMP = 3, //!< 現在の値をインクリメントし、最大値にクランプします。
	V3D_STENCIL_OP_DECREMENT_AND_CLAMP = 4, //!< 現在の値をデクリメントして 0 にクランプします。
	V3D_STENCIL_OP_INVERT = 5, //!< 現在の値をビット反転します。
	V3D_STENCIL_OP_INCREMENT_AND_WRAP = 6, //!< 現在の値をインクリメントし、最大値を超えた場合は 0 にラップします。
	V3D_STENCIL_OP_DECREMENT_AND_WRAP = 7, //!< 現在の値をデクリメントし、値が 0 より小さくなると最大値にラップします。
};

//! @enum V3D_BLEND_FACTOR
//! @brief ブレンド係数<br>
//! <table class="basic">
//! <tr><td>Rs0 Gs0 Bs0 As0</td><td>ブレンドされるカラーアタッチメントの第１のソースカラー</td></tr>
//! <tr><td>Rs1 Gs1 Bs1 As1</td><td>ブレンドされるカラーアタッチメントの第２のソースカラー</td></tr>
//! <tr><td>Rd Gd Bd Ad</td><td></td>出力先のカラー</tr>
//! <tr><td>Rc Gc Bc Ac</td><td></td>コマンド IV3DCommandBuffer::SetBlendConstants で設定するブレンド定数</tr>
//! </table>
//! @remarks
//! 以下のものは V3DDeviceCaps::colorBlendFlags に ::V3D_COLOR_BLEND_CAP_DUAL_SRC が含まれている場合にのみ使用することができます。<br>
//! <br>
//! ::V3D_BLEND_FACTOR_SRC1_COLOR<br>
//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR<br>
//! ::V3D_BLEND_FACTOR_SRC1_ALPHA<br>
//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA<br>
enum V3D_BLEND_FACTOR : uint8_t
{
	//! @brief
	//! RGB (0, 0, 0)<br>
	//! A (0)
	V3D_BLEND_FACTOR_ZERO = 0,
	//! @brief
	//! RGB (1, 1, 1)<br>
	//! A (1)
	V3D_BLEND_FACTOR_ONE = 1,
	//! @brief
	//! RGB (Rs0, Gs0, Bs0)<br>
	//! A (As0)
	V3D_BLEND_FACTOR_SRC_COLOR = 2,
	//! @brief
	//! RGB (1.0 - Rs0, 1.0 - Gs0, 1.0 - Bs0)<br>
	//! A (1.0 - As0)
	V3D_BLEND_FACTOR_ONE_MINUS_SRC_COLOR = 3,
	//! @brief
	//! RGB (Rd, Gd, Bd)<br>
	//! A (Ad)
	V3D_BLEND_FACTOR_DST_COLOR = 4,
	//! @brief
	//! RGB (1.0 - Rd, 1.0 - Gd, 1.0 - Bd)<br>
	//! A (1.0 - Ad)
	V3D_BLEND_FACTOR_ONE_MINUS_DST_COLOR = 5,
	//! @brief
	//! RGB (As0, As0, As0)<br>
	//! A (As0)
	V3D_BLEND_FACTOR_SRC_ALPHA = 6,
	//! @brief
	//! RGB (1.0 - As0, 1.0 - As0, 1.0 - As0)<br>
	//! A (1.0 - As0)
	V3D_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA = 7,
	//! @brief
	//! RGB (Ad, Ad, Ad)<br>
	//! A (Ad)
	V3D_BLEND_FACTOR_DST_ALPHA = 8,
	//! @brief
	//! RGB (1.0 - Ad, 1.0 - Ad, 1.0 - Ad)<br>
	//! A (1.0 - Ad)
	V3D_BLEND_FACTOR_ONE_MINUS_DST_ALPHA = 9,
	//! @brief
	//! RGB (Rc, Gc, Bc)<br>
	//! A (Ac)
	V3D_BLEND_FACTOR_CONSTANT_COLOR = 10,
	//! @brief
	//! RGB (1.0 - Rc, 1.0 - Gc, 1.0 - Bc)<br>
	//! A (1.0 - Ac)
	V3D_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
	//! @brief
	//! RGB (Ac, Ac, Ac)<br>
	//! A (Ac)
	V3D_BLEND_FACTOR_CONSTANT_ALPHA = 12,
	//! @brief
	//! RGB (1.0 - Ac, 1.0 - Ac, 1.0 - Ac)<br>
	//! A (1.0 - Ac)
	V3D_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
	//! @brief
	//! RGB (f, f, f) f = min(As0, 1.0 - Ad)<br>
	//! A(1.0)
	V3D_BLEND_FACTOR_SRC_ALPHA_SATURATE = 14,
	//! @brief
	//! RGB (Rs1, Gs1, Bs1)<br>
	//! A (As1)
	V3D_BLEND_FACTOR_SRC1_COLOR = 15,
	//! @brief
	//! RGB (1.0 - Rs1, 1.0 - Gs1, 1.0 - Bs1)<br>
	//! A (1.0 - As1)
	V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR = 16,
	//! @brief
	//! RGB (As1, As1, As1)<br>
	//! A (As1)
	V3D_BLEND_FACTOR_SRC1_ALPHA = 17,
	//! @brief
	//! RGB (1.0 - As1, 1.0 - As1, 1.0 - As1)<br>
	//! A (1.0 - As1)
	V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA = 18,
};

//! @enum V3D_BLEND_OP
//! @brief ブレンド演算子<br>
//! <table class="basic">
//! <tr><td>Cs</td><td>入力元のインプットアタッチメントなどのカラー</td></tr>
//! <tr><td>Cd</td><td>出力先になるカラーアタッチメントのカラー</td></tr>
//! <tr><td>Fs</td><td>入力元のブレンド係数</td></tr>
//! <tr><td>Fd</td><td>出力先のブレンド係数</td></tr>
//! </table>
//! @sa V3D_BLEND_FACTOR
enum V3D_BLEND_OP : uint8_t
{
	V3D_BLEND_OP_ADD = 0, //!< ( Cs * Fs ) + ( Cd * Fd )
	V3D_BLEND_OP_SUBTRACT = 1, //!< ( Cs * Fs ) - ( Cd * Fd )
	V3D_BLEND_OP_REVERSE_SUBTRACT = 2, //!< ( Cd * Fd ) - ( Cs * Fs )
	V3D_BLEND_OP_MIN = 3, //!< min( Cs, Cd )
	V3D_BLEND_OP_MAX = 4, //!< max( Cs, Cd )
};

//! @enum V3D_COLOR_COMPONENT_FLAG
//! @brief カラーのコンポーネントフラグ
enum V3D_COLOR_COMPONENT_FLAG : V3DFlags
{
	V3D_COLOR_COMPONENT_R = 0x00000001, //!< 赤要素です。
	V3D_COLOR_COMPONENT_G = 0x00000002, //!< 緑要素です。
	V3D_COLOR_COMPONENT_B = 0x00000004, //!< 青要素です。
	V3D_COLOR_COMPONENT_A = 0x00000008, //!< アルファ要素です。
	V3D_COLOR_COMPONENT_ALL = V3D_COLOR_COMPONENT_R | V3D_COLOR_COMPONENT_G | V3D_COLOR_COMPONENT_B | V3D_COLOR_COMPONENT_A, //!< 全ての要素です。
};

//! @enum V3D_LOGIC_OP
//! @brief 論理演算子<br>
//! <table class="basic">
//! <tr><td>s</td><td>フラグメントによって出力される RGBA 成分です。</td></tr>
//! <tr><td>d</td><td>カラーアタッチメントによって入力される RGBA 成分です。</td></tr>
//! </table>
//! <br>
enum V3D_LOGIC_OP : uint8_t
{
	V3D_LOGIC_OP_CLEAR = 0, //!< 0
	V3D_LOGIC_OP_AND = 1, //!< s & d
	V3D_LOGIC_OP_AND_REVERSE = 2, //!< s & ~d
	V3D_LOGIC_OP_COPY = 3, //!< s
	V3D_LOGIC_OP_AND_INVERTED = 4, //!< ~s & d
	V3D_LOGIC_OP_NO_OP = 5, //!< d
	V3D_LOGIC_OP_XOR = 6, //!< s ^ d
	V3D_LOGIC_OP_OR = 7, //!< s | d
	V3D_LOGIC_OP_NOR = 8, //!< ~( s | d )
	V3D_LOGIC_OP_EQUIVALENT = 9, //!< ~( s ^ d )
	V3D_LOGIC_OP_INVERT = 10, //!< ~d
	V3D_LOGIC_OP_OR_REVERSE = 11, //!< s | ~d
	V3D_LOGIC_OP_COPY_INVERTED = 12, //!< ~s
	V3D_LOGIC_OP_OR_INVERTED = 13, //!< ~s | d
	V3D_LOGIC_OP_NAND = 14, //!< ~( s & d )
	V3D_LOGIC_OP_SET = 15, //!< 1
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DPipelineShaderConstantDesc
//! @brief パイプラインのシェーダーの特殊化定数の記述
//! @sa V3DPipelineShader
struct V3DPipelineShaderConstantDesc
{
	//! @brief 定数の識別子を整数で指定します。<br>
	//! layout(constant_id = id) const
	uint32_t id;
	//! @brief データ上の定数のオフセットをバイト単位で指定します。
	uint32_t offset;
	//! @brief 定数のサイズをバイト単位で指定します。
	uint32_t size;
};

//! @struct V3DPipelineShader
//! @brief パイプラインのシェーダー
struct V3DPipelineShader
{
	//! @name モジュール
	//! @{

	IV3DShaderModule* pModule; //!< シェーダーモジュールのポインタです。
	const char* pEntryPointName; //!< シェーダーのエントリーポイントの名前です。

	//! @}

	//! @name 特殊化定数
	//! @{

	uint32_t constantCount; //!< 特殊化定数の数です。
	V3DPipelineShaderConstantDesc* pConstants; //!< constantCount の値の数の要素を持つ V3DPipelineShaderConstantDesc 構造体の配列です。
	void* pConstantData; //!< 特殊化定数データのポインタです。

	//! @}
};

//! @struct V3DPipelineVertexElement
//! @brief パイプラインのバーテックスエレメント
struct V3DPipelineVertexElement
{
	//! @brief ロケーションです。<br>
	//! layout(location = [location])
	uint32_t location;
	//! @brief オフセットをバイト単位で指定します。<br>
	//! この値は V3DDeviceCaps::maxVertexInputElementOffset 以下に制限されます。
	uint32_t offset;
	//! @brief フォーマットです。
	V3D_FORMAT format;
};

//! @struct V3DPipelineVertexLayout
//! @brief パイプラインのバーテックスレイアウト
struct V3DPipelineVertexLayout
{
	//! @brief バーテックスバッファーのバインド位置です。
	uint32_t binding;
	//! @brief バーテックスのストライドをバイト単位で指定します。<br>
	//! この値は V3DDeviceCaps::maxVertexInputBindingStride 以下に制限されます。
	uint32_t stride;
	//! @brief バーテックスレイアウトの最初のエレメントのインデックスです。
	//! @sa V3DPipelineVertexInputDesc::pElements
	uint32_t firstElement;
	//! @brief エレメントの数です。
	uint32_t elementCount;
};

//! @struct V3DPipelineVertexInputDesc
//! @brief バーテックスインプットの記述
struct V3DPipelineVertexInputDesc
{
	//! @name エレメント
	//! @{

	uint32_t elementCount; //!< バーテックスエレメントの数です。
	V3DPipelineVertexElement* pElements; //!< elementCount の値の数の要素を持つ V3DPipelineVertexElement 構造体の配列です。

	//! @}

	//! @name レイアウト
	//! @{

	uint32_t layoutCount; //!< バーテックスレイアウトの数です。
	V3DPipelineVertexLayout* pLayouts; //!< layoutCount の値の数の要素を持つ V3DPipelineVertexLayout 構造体の配列です。

	//! @}
};

//! @struct V3DPipelineTessellationDesc
//! @brief パイプラインのテッセレーションの記述
struct V3DPipelineTessellationDesc
{
	uint32_t patchControlPoints; //!< パッチごとのコントロールポイントの数です。<br>この値は V3DDeviceCaps::maxTessellationPatchSize 以下に制限されます。
};

//! @struct V3DPipelineRasterizationDesc
//! @brief パイプラインのラスター化の記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>discardEnable</td><td>false</td></tr>
//! <tr><td>depthClampEnable</td><td>false</td></tr>
//! <tr><td>polygonMode</td><td>V3D_POLYGON_MODE_FILL</td></tr>
//! <tr><td>cullMode</td><td>V3D_CULL_MODE_BACK</td></tr>
//! <tr><td>depthBiasEnable</td><td>false</td></tr>
//! <tr><td>depthBiasConstantFactor</td><td>0.0f</td></tr>
//! <tr><td>depthBiasClamp</td><td>0.0f</td></tr>
//! <tr><td>depthBiasSlopeFactor</td><td>0.0f</td></tr>
//! </table>
//! <br>
struct V3DPipelineRasterizationDesc
{
	//! @brief ラスタライズステージの直前にプリミティブを破棄するかどうかを指定します。
	bool discardEnable;
	//! @brief 深度をクランプするかどうかを指定します。<br>この値は V3DDeviceCaps::rasterizerFlags に ::V3D_RASTERIZER_CAP_DEPTH_CLAMP が含まれている場合にのみ true を指定することができます。
	bool depthClampEnable;

	//! @brief ポリゴンモードです。<br>
	//! V3DDeviceCaps::rasterizerFlags に ::V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID が含まれていなかった場合、
	//! ::V3D_POLYGON_MODE_LINE および ::V3D_POLYGON_MODE_POINT を指定することはできません。
	V3D_POLYGON_MODE polygonMode;
	//! @brief カリングモードです。
	V3D_CULL_MODE cullMode;

	//! @brief 深度バイアスを有効にするかどうかを指定します。
	bool depthBiasEnable;
	//! @brief 一定の深度値を制御する係数です。
	float depthBiasConstantFactor;
	//! @brief 最大 または 最小の深度バイアスです。<br>
	//! V3DDeviceCaps::rasterizerFlags に ::V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP が含まれていなかった場合は 0.0f を指定する必要があります。
	float depthBiasClamp;
	//! @brief 深度バイアスの計算におけるスロープに適用される係数です。
	float depthBiasSlopeFactor;
};

//! @struct V3DPipelineMultisampleDesc
//! @brief パイプラインのマルチサンプルの記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>rasterizationSamples</td><td>V3D_SAMPLE_COUNT_1</td></tr>
//! <tr><td>sampleShadingEnable</td><td>false</td></tr>
//! <tr><td>minSampleShading</td><td>0.0f</td></tr>
//! <tr><td>alphaToCoverageEnable</td><td>false</td></tr>
//! <tr><td>alphaToOneEnable</td><td>false</td></tr>
//! </table>
//! <br>
struct V3DPipelineMultisampleDesc
{
	//! @brief ラスター化のサンプル数です。<br>
	//! V3DDeviceCaps::multisampleFlags に ::V3D_MULTISAMPLE_CAP_VARIABLE_RATE が含まれていなかった場合、
	//! サブパスのカラーアタッチメント、デプスステンシルアタッチメントのサンプル数はこの値である必要があります。
	V3D_SAMPLE_COUNT_FLAG rasterizationSamples;
	//! @brief フラグメントがサンプル毎にシェーディングを実行するかどうかを指定します。<br>
	//! この値は V3DDeviceCaps::multisampleFlags に ::V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING が含まれている場合にのみ true を指定することができます。
	bool sampleShadingEnable;
	//! @brief サンプル毎のシェーディングの最小分率です。<br>
	//! この値は sampleShadingEnable が true のときに有効になり、0.0f ～ 1.0f の間に制限されます。
	float minSampleShading;
	//! @brief アルファトゥカバレッジを使用するかどうかを指定します。
	bool alphaToCoverageEnable;
	//! @brief フラグメントシェーダから出力されたアルファ値を、固定小数点が表現可能な最大値または 1.0 で置き換えるかどうかを指定します。<br>
	//! この値は V3DDeviceCaps::multisampleFlags に ::V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE が含まれている場合にのみ true を指定することができます。
	bool alphaToOneEnable;
};

//! @struct V3DPipelineStencilOpDesc
//! @brief パイプラインのステンシルオペレーションの記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>failOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>passOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>depthFailOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>compareOP</td><td>V3D_COMPARE_OP_ALWAYS</td></tr>
//! <tr><td>readMask</td><td>0x00000000</td></tr>
//! <tr><td>writeMask</td><td>0x00000000</td></tr>
//! </table>
//! <br>
struct V3DPipelineStencilOpDesc
{
	V3D_STENCIL_OP failOp; //!< ステンシルテストに失敗したサンプルに対して実行されるオペレーションです。。
	V3D_STENCIL_OP passOp; //!< 深さテストとステンシルテストの両方に合格したサンプルに対して実行されるオペレーションです。
	V3D_STENCIL_OP depthFailOp; //!< ステンシルテストに合格し、深度テストに合格しなかったサンプルに対して実行されるオペレーションです。

	//! @brief ステンシルテストで使用される比較演算子です。<br>
	//! ::V3D_COMPARE_OP の説明にある R はマスクされた IV3DCommandBuffer::SetStencilReference で設定された値であり、S はマスクされたステンシルの値を表します。
	V3D_COMPARE_OP compareOp;

	//! @brief ステンシルテストによって読み込まれる値のビットマスクです。
	uint32_t readMask;
	//! @brief ステンシルテストによって書き込まれる値のビットマスクです。
	uint32_t writeMask;
};

//! @struct V3DPipelineDepthStencilDesc
//! @brief パイプラインのデプスステンシルの記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>depthTestEnable</td><td>true</td></tr>
//! <tr><td>depthWriteEnable</td><td>true</td></tr>
//! <tr><td>depthCompareOp</td><td>V3D_COMPARE_OP_LESS</td></tr>
//! <tr><td>stencilTestEnable</td><td>false</td></tr>
//! <tr><td>stencilFront</td><td>V3DPipelineStencilOpDesc を参照してください。</td></tr>
//! <tr><td>stencilBack</td><td>V3DPipelineStencilOpDesc を参照してください。</td></tr>
//! <tr><td>depthBoundsTestEnable</td><td>false</td></tr>
//! <tr><td>minDepthBounds</td><td>0.0f</td></tr>
//! <tr><td>maxDepthBounds</td><td>1.0f</td></tr>
//! </table>
//! <br>
struct V3DPipelineDepthStencilDesc
{
	//! @brief 深度テストをするかどうかを指定します。
	bool depthTestEnable;
	//! @brief 深度を書き込むかどうかを指定します。
	bool depthWriteEnable;
	//! @brief 深度テストの比較演算子です。<br>
	//! ::V3D_COMPARE_OP の説明にある R は入力される深度であり、S はアタッチメントの深度を表します。
	V3D_COMPARE_OP depthCompareOp;

	//! @brief ステンシルテストをするかどうかを指定します。
	bool stencilTestEnable;
	//! @brief ステンシル前面のオペレーションです。
	V3DPipelineStencilOpDesc stencilFront;
	//! @brief ステンシル後面のオペレーションです。
	V3DPipelineStencilOpDesc stencilBack;

	//! @brief 深度の境界テストをするかどうかを指定します。<br>
	//! この値は V3DDeviceCaps::depthStencilFlags に ::V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS が含まれている場合にのみ true を指定することができます。
	bool depthBoundsTestEnable;
	//! @brief 深度の境界テストの最小値です。<br>
	//! この値は depthBoundsTestEnable が true のときに有効になり、0.0f ～ 1.0f の間に制限されます。
	float minDepthBounds;
	//! @brief 深度の境界テストの最大値です。<br>
	//! この値は depthBoundsTestEnable が true のときに有効になり、0.0f ～ 1.0f の間に制限されます。
	float maxDepthBounds;
};

//! @struct V3DPipelineColorBlendAttachment
//! @brief パイプラインのカラーアタッチメントのブレンド
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>blendEnable</td><td>false</td></tr>
//! <tr><td>srcColorFactor</td><td>V3D_BLEND_FACTOR_ONE</td></tr>
//! <tr><td>dstColorFactor</td><td>V3D_BLEND_FACTOR_ZERO</td></tr>
//! <tr><td>colorOp</td><td>V3D_BLEND_OP_ADD</td></tr>
//! <tr><td>srcAlphaFactor</td><td>V3D_BLEND_FACTOR_ONE</td></tr>
//! <tr><td>dstAlphaFactor</td><td>V3D_BLEND_FACTOR_ZERO</td></tr>
//! <tr><td>alphaOp</td><td>V3D_BLEND_OP_ADD</td></tr>
//! <tr><td>writeMask</td><td>V3D_COLOR_COMPONENT_ALL</td></tr>
//! </table>
//! <br>
struct V3DPipelineColorBlendAttachment
{
	bool blendEnable; //!< ブレンドを有効にするかどうかを指定します。
	V3D_BLEND_FACTOR srcColorFactor; //!< ブレンド元のRGB成分のブレンド係数です。
	V3D_BLEND_FACTOR dstColorFactor; //!< ブレンド先のRGB成分のブレンド係数です。
	V3D_BLEND_OP colorOp; //!< RGB成分のブレンド演算子です。
	V3D_BLEND_FACTOR srcAlphaFactor; //!< ブレンド元のA成分のブレンド係数です。
	V3D_BLEND_FACTOR dstAlphaFactor; //!< ブレンド先のA成分のブレンド係数です。
	V3D_BLEND_OP alphaOp; //!< A成分のブレンド演算子です。
	V3DFlags writeMask; //!< カラーの書き込みマスクを表す ::V3D_COLOR_COMPONENT_FLAG 列挙定数の組み合わせです。
};

//! @struct V3DPipelineColorBlendDesc
//! @brief パイプラインのカラーブレンドの記述
//! @remarks
//! デフォルト値<br>
//! <table class="basic">
//! <tr><th>ステート</th><th>デフォルト値</th></tr>
//! <tr><td>logicOpEnable</td><td>false</td></tr>
//! <tr><td>logicOp</td><td>V3D_LOGIC_OP_COPY</td></tr>
//! <tr><td>attachmentCount</td><td>1 以上</td></tr>
//! <tr><td>pAttachments</td><td>attachmentCount の値の数の要素を持つ V3DPipelineColorBlendAttachment 構造体の配列</td></tr>
//! </table>
//! <br>
struct V3DPipelineColorBlendDesc
{
	//! @brief 論理演算を有効にするかどうかを指定します。<br>
	//! この値は V3DDeviceCaps::colorBlendFlags に ::V3D_COLOR_BLEND_CAP_LOGIC_OP が含まれている場合にのみ true を指定することができます。
	bool logicOpEnable;
	//! @brief 論理演算の種類です。<br>
	//! この値は logicOpEnable が true のときに有効になります。
	V3D_LOGIC_OP logicOp;

	//! @brief サブパスのカラーアタッチメントの数です。
	uint32_t attachmentCount;
	//! @brief サブパスのカラーアタッチメントのブレンド配列です。<br>
	//! 配列の要素の数は attachmentCount 値と同じである必要がります。<br>
	//! また V3DDeviceCaps::colorBlendFlags に ::V3D_COLOR_BLEND_CAP_INDEPENDENT が含まれていなかった場合、アタッチメント毎に異なるブレンドをすることができないため、
	//! 配列の要素はすべて同じ値でないといけません。<br>
	V3DPipelineColorBlendAttachment* pAttachments;
};

//! @struct V3DGraphicsPipelineDesc
//! @brief グラフィックスパイプラインの記述
struct V3DGraphicsPipelineDesc
{
	V3DPipelineShader vertexShader; //!< バーテックスシェーダーです。
	V3DPipelineShader tessellationControlShader; //!< テッセーレーション制御シェーダーです。
	V3DPipelineShader tessellationEvaluationShader; //!< テッセーレーション評価シェーダーです。
	V3DPipelineShader geometryShader; //!< ジオメトリシェーダーです。
	V3DPipelineShader fragmentShader; //!< フラグメントシェーダーです。

	V3DPipelineVertexInputDesc vertexInput; //!< バーテックスインプットです。

	V3D_PRIMITIVE_TOPOLOGY primitiveTopology; //!< プリミティブトポロジです。

	V3DPipelineTessellationDesc tessellation; //!< テッセーレーションです。
	V3DPipelineRasterizationDesc rasterization; //!< ラスター化です。
	V3DPipelineMultisampleDesc multisample; //!< マルチサンプルです。

	V3DPipelineDepthStencilDesc depthStencil; //!< デプスステンシルです。
	V3DPipelineColorBlendDesc colorBlend; //!< カラーブレンドです。

	IV3DRenderPass* pRenderPass; //!< レンダーパスです。
	uint32_t subpass; //!< サブパスのインデックスです。
};

//! @struct V3DComputePipelineDesc
//! @brief コンピュートパイプラインの記述
struct V3DComputePipelineDesc
{
	V3DPipelineShader computeShader; //!< コンピュートシェーダーです。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DPipeline
//! @brief パイプラインインターフェース
//! @sa IV3DDevice::CreateGraphicsPipeline
//! @sa IV3DDevice::CreateComputePipeline
class V3D_DLL_API IV3DPipeline : public IV3DDeviceChild
{
public:
	//! @brief パイプラインのタイプを取得します。
	//! @return パイプラインのタイプを表す V3D_PIPELINE_TYPE 列挙定数のいずれかを返します。
	virtual V3D_PIPELINE_TYPE GetType() const = 0;
	//! @brief パイプラインのレイアウトを取得します。
	//! @param[out] ppPipelineLayout パイプラインのレイアウトを表す IV3DPipelineLayout インターフェースのポインタのアドレスです。
	virtual void GetLayout(IV3DPipelineLayout** ppPipelineLayout) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DPipeline() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// スワップチェイン
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_callback_group
//! @{

//! @fn void PV3DLostSwapChainFunction(void* pUserData)
//! @brief スワップチェインがロストした際に呼び出されるコールバック関数の定義です。
//! @param pUserData ユーザーデータです。
typedef void (V3D_CALLBACK *PV3DLostSwapChainFunction)(void* pUserData);
//! @fn void PV3DRestoreSwapChainFunction(void* pUserData)
//! @brief スワップチェインが復帰した際に呼び出されるコールバック関数の定義です。
//! @param pUserData ユーザーデータです。
typedef void (V3D_CALLBACK *PV3DRestoreSwapChainFunction)(void* pUserData);

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DSwapChainCallbacks
//! @brief スワップチェインのコールバック
struct V3DSwapChainCallbacks
{
	PV3DLostSwapChainFunction pLostFunction; //!< スワップチェインがロストした際に呼び出されるコールバック関数のポインタです。
	PV3DRestoreSwapChainFunction pRestoreFunction; //!< スワップチェインが復帰した際に呼び出されるコールバック関数のポインタです。
	void* pUserData; //!< コールバック関数に渡されるユーザーデータです。
};

//! @struct V3DSwapChainDesc
//! @brief スワップチェインの記述
struct V3DSwapChainDesc
{
	HWND windowHandle; //!< ウィンドウのハンドルです。

	V3D_FORMAT imageFormat; //!< イメージのフォーマットです。
	uint32_t imageWidth; //!< イメージの幅です。
	uint32_t imageHeight; //!< イメージの高さです。
	uint32_t imageCount; //!< イメージの数です。
	V3DFlags imageUsageFlags; //!< イメージの使用法です。

	//! @brief キューファミリーのインデックスです。
	uint32_t queueFamily;
	//! @brief キューに送信した際に待機が発生するパイプラインステージのマスクです。
	//! 最後のコマンドにはこのパイプラインステージが含まれている必要があります。
	V3DFlags queueWaitDstStageMask;

	bool fullscreenAssistEnable; //!< フルスクリーンの切り替えを タイトルバーのダブルクリック、最大化ボタン、ALT + ENTER キーによってできるようにします。
	bool vsyncEnable; //!< 垂直同期を待機するかどうかを指定します。
	bool windowed; //!< 開始時の表示モードをウィンドウモードにするかどうかを指定します、
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DSwapChain
//! @brief スワップチェインインターフェース
//! @sa IV3DDevice::CreateSwapChain
class V3D_DLL_API IV3DSwapChain : public IV3DDeviceChild
{
public:
	//! @brief スワップチェインの記述を取得します。
	//! @return スワップチェインの記述を表す V3DSwapChainDesc 構造体のアドレスを返します。
	virtual const V3DSwapChainDesc& GetDesc() const = 0;

	//! @brief レンダリングイメージを獲得します。
	//! @retval V3D_OK レンダリングイメージの獲得に成功しました。
	//! @retval V3D_TIMEOUT タイムアウトしたためレンダリングイメージを獲得できませんでした。
	//! @retval V3D_ERROR_FAIL レンダリングイメージの獲得に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT AcquireNextImage() = 0;

	//! @brief 現在のイメージのインデックスを取得します。
	//! @return 現在のイメージのインデックスを返します。
	//! @remarks 返される値は AcquireNextImage で更新されます。
	virtual uint32_t GetCurrentImageIndex() const = 0;
	//! @brief 指定したインデックスのイメージを取得します。
	//! @param[in] imageIndex 取得するイメージのインデックスです。
	//! @param[out] ppImage 取得したイメージを渡す IV3DImage インターフェースのポインタのアドレスです。
	virtual void GetImage(uint32_t imageIndex, IV3DImage** ppImage) = 0;

	//! @brief 表示モードの設定を取得します。
	//! @return 表示モードが全画面モードの場合は true を返します。
	virtual bool GetFullscreenState() const = 0;
	//! @brief 表示モードをウィンドウモードまたは全画面モードに設定します。
	//! @param[in] bFullscreen 全画面モードにするかどうかを指定します。
	//! @retval V3D_OK 表示モードの設定に成功しました。
	//! @retval V3D_ERROR_FAIL 表示モードの設定に失敗しました。<br>
	//! すでに他のスワップチェインが全画面モードの場合に、このスワップチェインを全画面モードにすることはできません。<br>
	//! それ以外の場合は内部エラーが発生した可能性があります。
	virtual V3D_RESULT SetFullscreenState(bool bFullscreen) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DSwapChain() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// コマンドプール
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DCommandPoolDesc
//! @brief コマンドプールの記述
struct V3DCommandPoolDesc
{
	uint32_t queueFamily; //!< キューファミリーです。
	V3DFlags usageFlags; //!< 使用法を表す ::V3D_COMMAND_POOL_USAGE_FLAG 列挙定数の組み合わせです。
};

//! @}

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_POOL_USAGE_FLAG
//! @brief コマンドプールの使用法フラグ
enum V3D_COMMAND_POOL_USAGE_FLAG : V3DFlags
{
	//! @brief コマンドプールーから割り当てられたコマンドバッファーが比較的短時間にリセットまたは解放されることを表します。
	V3D_COMMAND_POOL_USAGE_TRANSIENT = 0x00000001,
	//! @brief コマンドプールーから割り当てられたコマンドバッファーを個別にリセットできるようにします。<br>
	//! このフラグが指定された場合は IV3DCommandBuffer::Reset によるリセットができ IV3DCommandBuffer::Begin による暗黙的なリセットが行われます。<br>
	//! また、このフラグが指定されなかった場合 IV3DCommandBuffer::Reset は使用できず IV3DCommandPool::Reset によってコマンドバッファーを一括でリセットするようになります。
	V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER = 0x00000002,
};

//! @enum V3D_COMMAND_POOL_RESET_FLAG
//! @brief コマンドプールのリセットフラグ
enum V3D_COMMAND_POOL_RESET_FLAG : V3DFlags
{
	//! @brief リセットした際にコマンドバッファーが所有するメモリリソースをコマンドプールーに返却します。<br>
	//! またこのフラグが指定されなかった場合はメモリリソースを返却せず、再利用します。
	V3D_COMMAND_POOL_RESET_RELEASE_RESOURCES = 0x00000001,
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DCommandPool
//! @brief コマンドプールインターフェース
//! @sa IV3DDevice::CreateCommandPool
class V3D_DLL_API IV3DCommandPool : public IV3DDeviceChild
{
public:
	//! @brief コマンドプールの記述を取得します。
	//! @return コマンドプールの記述を返します。
	virtual const V3DCommandPoolDesc& GetDesc() const = 0;

	//! @brief コマンドプールをリセットします
	//! @param[in] resetFlags リセットフラグを表す ::V3D_COMMAND_POOL_RESET_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK コマンドプールのリセットに成功しました。
	//! @retval V3D_ERROR_FAIL コマンドプールのリセットに失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset(V3DFlags resetFlags) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DCommandPool() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// コマンドバッファー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_BUFFER_TYPE
//! @brief コマンドバッファーのレベル
enum V3D_COMMAND_BUFFER_TYPE : uint8_t
{
	//! @brief キューに送信することができる、プライマリコマンドバッファーです。
	//! @sa IV3DQueue::Submit
	V3D_COMMAND_BUFFER_TYPE_PRIMARY = 0,
	//! @brief プライマリコマンドバッファーで実行することができる、セカンダリコマンドバッファーです。
	//! @sa IV3DCommandBuffer::ExecuteCommandBuffers
	V3D_COMMAND_BUFFER_TYPE_SECONDARY = 1,
};

//! @enum V3D_COMMAND_BUFFER_RESET_FLAG
//! @brief コマンドバッファーのリセットフラグ
enum V3D_COMMAND_BUFFER_RESET_FLAG : V3DFlags
{
	//! @brief リセットした際にコマンドバッファが所有するメモリリソースをコマンドプールーに返却します。<br>
	//! またこのフラグが指定されなかった場合はメモリリソースを返却せず、再利用します。
	V3D_COMMAND_BUFFER_RESET_RELEASE_RESOURCES = 0x00000001,
};

//! @enum V3D_COMMAND_BUFFER_USAGE_FLAG
//! @brief コマンドバッファの使用法フラグ
enum V3D_COMMAND_BUFFER_USAGE_FLAG : V3DFlags
{
	//! @brief コマンドバッファーは一度だけキューに送信されることを表します。
	V3D_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001,
	//! @brief プライマリコマンドバッファーで開始されたレンダーパス内で実行されるセカンダリコマンドバッファーがレンダーパス、サブパス、フレームバッファを引き継ぐことを表します。
	V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE = 0x00000002,
	//! @brief コマンドバッファーが複数のキューに送信 (IV3DQueue::Submit) またはプライマリコマンドバッファーに記録 ( IV3DCommandBuffer::ExecuteCommandBuffers ) できることを表します。<br>
	//! また、プライマリコマンドバッファーにこの使用法が含まれている場合、そこで実行されるセカンダリコマンドバッファーも同様にこの使用法が含まれている必要があります。
	V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004,
};

//! @enum V3D_STENCIL_FACE_FLAG
//! @brief ステンシルのフェイスフラグ
enum V3D_STENCIL_FACE_FLAG : V3DFlags
{
	V3D_STENCIL_FACE_FRONT = 0x00000001, //!< 前を指定します。
	V3D_STENCIL_FACE_BACK = 0x00000002, //!< 後ろを指定します。
	V3D_STENCIL_FACE_BOTH = V3D_STENCIL_FACE_FRONT | V3D_STENCIL_FACE_BACK, //!< 前後両方を指定します。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageSubresourceRange
//! @brief イメージのサブリソースをコピーする範囲
struct V3DImageSubresourceRange
{
	uint32_t level; //!< レベルです。
	uint32_t baseLayer; //!< 最初のレイヤーです。
	uint32_t layerCount; //!< レイヤー数です。
};

//! @struct V3DCopyImageRange
//! @brief イメージをコピーする範囲
struct V3DCopyImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< コピー先のサブリソースです。
	V3DPoint3D dstOffset; //!< コピー先のオフセットです。

	V3DImageSubresourceRange srcSubresource; //!< コピー元のサブリソースです。
	V3DPoint3D srcOffset; //!< コピー元のオフセットです。

	V3DSize3D size; //!< コピーするサイズです。
};

//! @struct V3DCopyBufferToImageRange
//! @brief バッファをイメージにコピーする範囲
struct V3DCopyBufferToImageRange
{
	V3DImageSubresourceRange dstImageSubresource; //!< コピー先イメージのサブリソースです。
	V3DPoint3D dstImageOffset; //!< コピー先イメージのオフセットです。
	V3DSize3D dstImageSize; //!< コピー先イメージのサイズ

	uint64_t srcBufferOffset; //!< コピー元バッファのオフセットをバイト単位で指定します。
};

//! @struct V3DCopyImageToBufferRange
//! @brief イメージをバッファにコピーする範囲
struct V3DCopyImageToBufferRange
{
	uint64_t dstBufferOffset; //!< コピー先バッファのオフセットをバイト単位で指定します。

	V3DImageSubresourceRange srcImageSubresource; //!< コピー元イメージのサブリソースです。
	V3DPoint3D srcImageOffset; //!< コピー元イメージのオフセットです。
	V3DSize3D srcImageSize; //!< コピー元イメージのサイズです。
};

//! @struct V3DBlitImageRange
//! @brief イメージを転送する範囲
struct V3DBlitImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< 転送先のサブリソースです。
	V3DRectangle3D dstRect; //!< 転送先の領域です。

	V3DImageSubresourceRange srcSubresource; //!< 転送元のサブリソースです。
	V3DRectangle3D srcRect; //!< 転送元の領域です。
};

//! @struct V3DResolveImageRange
//! @brief イメージを変換する範囲
struct V3DResolveImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< 変換先のサブリソースです。
	V3DPoint3D dstOffset; //!< 変換先のオフセットです。

	V3DImageSubresourceRange srcSubresource; //!< 変換元のサブリソースです。
	V3DPoint3D srcOffset; //!< 変換元のオフセットです。

	V3DSize3D size; //!< 変換するサイズです。
};

//! @struct V3DClearColorAttachmentDesc
//! @brief カラーアタッチメントをクリアするための記述
struct V3DClearColorAttachmentDesc
{
	uint32_t attachment; //!< サブパスのカラーアタッチメントのインデックスです。
	V3DClearColorValue value; //!< クリア値です。
};

//! @struct V3DClearDepthStencilAttachmentDesc
//! @brief デスプステンシルアタッチメントをクリアするための記述
struct V3DClearDepthStencilAttachmentDesc
{
	//! @brief 深度をクリアするかどうかを指定します。<br>
	//! クリアしようとしているデプスステンシルアタッチメントのフォーマットに深度が含まれている必要があります。
	bool depthEnable;
	//! @brief ステンシルをクリアするかどうかを指定します。<br>
	//! クリアしようとしているデプスステンシルアタッチメントのフォーマットにステンシルが含まれている必要があります。
	bool stencilEnable;
	//! @brief クリア値です。
	V3DClearDepthStencilValue value;
};

//! @struct V3DClearRange
//! @brief クリア範囲
struct V3DClearRange
{
	V3DRectangle2D rect; //!< クリアする領域です。
	uint32_t baseLayer; //!< クリアする最初のレイヤーです。
	uint32_t layerCount; //!< クリアするレイヤーの数です。
};

//! @struct V3DCopyBufferRange
//! @brief バッファーのコピー範囲
struct V3DCopyBufferRange
{
	uint64_t srcOffset; //!< コピー元のメモリオフセットをバイト単位で指定します。
	uint64_t dstOffset; //!< コピー先のメモリオフセットをバイト単位で指定します。
	uint64_t size; //!< コピーするメモリサイズをバイト単位で指定します。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DCommandBuffer
//! @brief コマンドバッファーインターフェース
//! @sa IV3DDevice::CreateCommandBuffer
class V3D_DLL_API IV3DCommandBuffer : public IV3DDeviceChild
{
public:
	//! @name 全般
	//! @{

	//! @brief コマンドバッファーのタイプを取得します。
	//! @return コマンドバッファーのタイプを返します。
	virtual V3D_COMMAND_BUFFER_TYPE GetType() const = 0;
	//! @brief コマンドプールを取得します。
	//! @param[out] ppPool 取得したコマンドプールを渡す IV3DCommandPool インターフェースのポインタのアドレスです。
	virtual void GetPool(IV3DCommandPool** ppPool) = 0;

	//! @brief コマンドバッファーをリセットします。
	//! @param[in] resetFlags コマンドバッファーのリセットフラグを表す ::V3D_COMMAND_BUFFER_RESET_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK コマンドバッファーのリセットに成功しました。
	//! @retval V3D_ERROR_FAIL コマンバッファーのリセットに失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! このコマンドバッファーがアクセスするコマンドプール作成時に指定した使用法である V3DCommandPoolDesc::usageFlags に ::V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER が含まれていない場合、
	//! このメソッドは機能しません。
	virtual V3D_RESULT Reset(V3DFlags resetFlags) = 0;

	//! @brief コマンドの書き込みを開始します。
	//! @retval V3D_OK コマンドの書き込みの開始に成功しました。
	//! @retval V3D_ERROR_FAIL コマンドの書き込みの開始に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! このメソッドはセカンダリコマンドバッファーで使用することはできません。
	virtual V3D_RESULT Begin() = 0;
	//! @brief コマンドの書き込みを開始します。
	//! @param[in] usageFlags コマンドバッファーの使用法を表す ::V3D_COMMAND_BUFFER_USAGE_FLAG 列挙定数の組み合わせです。
	//! @param[in] pRenderPass プライマリコマンドバッファーから引き継ぐレンダーパスです。
	//! @param[in] subpass プライマリコマンドバッファーから引き継ぐサブパスです。
	//! @param[in] pFrameBuffer プライマリコマンドバッファーから引き継ぐフレームバッファーです。
	//! @retval V3D_OK コマンドの書き込みの開始に成功しました。
	//! @retval V3D_ERROR_FAIL コマンドの書き込みの開始に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! usageFlags に ::V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定した場合は pRenderPass と subpass は必ず指定してください。<br>
	//! また pFrameBuffer に nullptr を指定することも可能ですが、パフォーマンスを考慮したい場合は有効なフレームバッファを指定することを推奨します。
	virtual V3D_RESULT Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer) = 0;
	//! @brief コマンドの書き込みを終了します。
	//! @retval V3D_OK コマンドの書き込みの終了に成功しました。
	//! @retval V3D_ERROR_FAIL コマンドの書き込みの終了に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT End() = 0;

	//! @}

	//! @name コマンド
	//! @{

	//! @brief バッファーのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] bufferMemoryBarriers バッファーのメモリバリアです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DBufferMemoryBarrier& bufferMemoryBarriers) = 0;
	//! @brief バッファービューにメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] bufferViewMemoryBarriers バッファービューのメモリバリアです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DBufferViewMemoryBarrier& bufferViewMemoryBarriers) = 0;
	//! @brief イメージのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] imageMemoryBarriers イメージのメモリバリアです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドイメージ</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DImageMemoryBarrier& imageMemoryBarriers) = 0;
	//! @brief イメージビューにメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] imageViewMemoryBarriers イメージビューのメモリバリアです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドイメージ</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, const V3DImageViewMemoryBarrier& imageViewMemoryBarriers) = 0;

	//! @brief バッファーのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] memoryBarrierCount バッファーのメモリバリアの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] pMemoryBarriers バッファーのメモリバリア配列です。
	//! - 配列の要素数は memoryBarrierCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DBufferMemoryBarrier* pMemoryBarriers) = 0;
	//! @brief バッファービューのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] memoryBarrierCount バッファービューのメモリバリアの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] pMemoryBarriers バッファービューのメモリバリア配列です。
	//! - 配列の要素数は memoryBarrierCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファービュー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DBufferViewMemoryBarrier* pMemoryBarriers) = 0;
	//! @brief イメージのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] memoryBarrierCount イメージのメモリバリアの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] pMemoryBarriers イメージのメモリバリア配列です。
	//! - 配列の要素数は memoryBarrierCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドイメージ</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DImageMemoryBarrier* pMemoryBarriers) = 0;
	//! @brief イメージビューのメモリにバリアを張ります。
	//! @param[in] pipelineBarrier パイプラインバリアです。
	//! @param[in] memoryBarrierCount イメージビューのメモリバリアの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] pMemoryBarriers イメージビューのメモリバリア配列です。
	//! - 配列の要素数は memoryBarrierCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドイメージビュー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void Barrier(const V3DPipelineBarrier& pipelineBarrier, uint32_t memoryBarrierCount, const V3DImageViewMemoryBarrier* pMemoryBarriers) = 0;

	//! @brief バリアセットを使用してバリアを張ります。
	//! @param[in] pBarrierSet バリアセットです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドイメージ</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreateBarrierSet
	virtual void Barrier(IV3DBarrierSet* pBarrierSet) = 0;

	//! @brief イベントをリセットして、非シグナル状態にします。
	//! @param[in] pEvent リセットするイベントです。
	//! @param[in] stageMask イベントを非シグナル状態と判断するためのステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void ResetEvent(IV3DEvent* pEvent, V3DFlags stageMask) = 0;
	//! @brief イベントをリセットして、シグナル状態にします。
	//! @param[in] pEvent セットするイベントです。
	//! @param[in] stageMask イベントをシグナル状態と判断するためのステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void SetEvent(IV3DEvent* pEvent, V3DFlags stageMask) = 0;

	//! @brief イベントを待機します。
	//! @param[in] eventCount 待機に使用するイベントの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppEvents 待機に使用するイベントの配列です。
	//! - 配列の要素数は eventCount である必要があります。
	//! @param[in] srcStageMask 非シグナル状態のステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	//! @param[in] dstStageMask シグナル状態のステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせです。
	//! @remarks
	//! - 待機に使用するイベントは必ずシグナル状態になるようにしてください。<br>
	//! これを怠った場合、TDR が発生します。
	//! .
	//! - イベントの待機が完了しても、イベントはリセットされず、非シグナル状態には移行しません。<br>
	//! IV3DCommandBuffer::ResetEvent または IV3DEvent::Reset を使用して、非シグナル状態にする必要があります。
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void WaitEvents(uint32_t eventCount, IV3DEvent** ppEvents, V3DFlags srcStageMask, V3DFlags dstStageMask) = 0;
	//! @brief イベントを待機します。
	//! @param[in] eventCount 待機に使用するイベントの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppEvents 待機に使用するイベントの配列です。
	//! - 配列の要素数は eventCount である必要があります。
	//! @param[in] pBarrierSet イベントがシグナル状態になった際に実行するバリアのセットです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::WaitEvents(uint32_t, IV3DEvent**, V3DFlags, V3DFlags)
	virtual void WaitEvents(uint32_t eventCount, IV3DEvent** ppEvents, IV3DBarrierSet* pBarrierSet) = 0;

	//! @brief バッファーを指定した値で塗りつぶします
	//! @param[in] pDstBuffer 塗りつぶすバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] dstOffset 塗りつぶすバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= dstOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] size 塗りつぶすサイズをバイト単位で指定します。<br>
	//! - この値は 4 の整数倍である必要があり、{ 4 <= size <= (V3DBufferDesc::size - dstOffset) } の範囲に制限されます。
	//! @param[in] data 塗りつぶす任意の値を指定します。
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void FillBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data) = 0;

	//! @brief バッファーを更新します。
	//! @param[in] pDstBuffer 更新するバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] dstOffset 更新するバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は 4 の整数倍である必要があり { 0 <= dstOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] srcSize 更新するソースのサイズをバイト単位で指定します。
	//! - この値は 4 の整数倍である必要があります。
	//! - この値が 65536 バイト以上の場合は内部で分割して更新するため、パフォーマンスが低下することに注意してください。
	//! @param[in] pSrc 更新するソースです。
	//! - nullptr を指定することはできません。
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void UpdateBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t srcSize, const void* pSrc) = 0;

	//! @brief バッファーをコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] dstOffset コピー先のバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= dstOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] pSrcBuffer コピー元のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! @param[in] srcOffset コピー元のバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= dstOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] size コピーするサイズをバイト単位で指定します。
	//! - この値は { 1 <= size <= min(dstBufferSize - dstOffset, srcBufferSize - srcOffset) } の範囲に制限されます。
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size) = 0;

	//! @brief バッファーをコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] pSrcBuffer コピー元のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! - この値は、1以上を指定する必要があります。
	//! @param[in] pRanges コピーする範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges) = 0;

	//! @brief イメージをコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout コピー先のイメージレイアウトです。
	//! - イメージレイアウトは ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。<br>
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout コピー元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれか指定します。
	//! また pSrcImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @remarks
	//! コピー操作をするイメージは、タイプ、サイズ、レベル数、レイヤー数、サンプル数が一致している必要があります。<br>
	//! またフォーマットはコピー元が V3D_FORMAT_R8G8B8A8_UNORM、コピー先が V3D_FORMAT_R32_UINT のように異なっていても、エレメントのサイズが一致していればコピーすることが可能です。<br>
	//! ただしデプスステンシルの場合、フォーマットは一致していなければなりません。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief イメージをコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout コピー先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout コピー元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges コピーする範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @remarks
	//! コピー操作をするイメージは、タイプ、サンプル数が一致している必要があります。<br>
	//! またフォーマットはコピー元が V3D_FORMAT_R8G8B8A8_UNORM、コピー先が V3D_FORMAT_R32_UINT のように異なっていても、エレメントのサイズが一致していればコピーすることが可能です。<br>
	//! ただしデプスステンシルの場合、フォーマットは一致していなければなりません。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageRange* pRanges) = 0;

	//! @brief バッファーをイメージにコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout コピー先のイメージのレイアウトです。
	//! - メージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] dstLevel コピー先のイメージのレベルです。<br>
	//! - この値は { 0 <= dstLevel < V3DImageDesc::levelCount } の範囲に制限されます。
	//! @param[in] dstBaseLayer コピー先のイメージの最初のレイヤーです。
	//! - この値は { 0 <= dstBaseLayer < V3DImageDesc::layerCount } の範囲に制限されます。
	//! @param[in] dstLayerCount コピー先のイメージのレイヤー数です。
	//! - この値は { 1 <= dstLayerCount <= (V3DImageDesc::layerCount - dstBaseLayer) } の範囲に制限されます。
	//! @param[in] dstOffset コピー先のイメージのオフセットです。
	//! - この値は { (0, 0, 0) <= dstOffset < (V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) } の範囲に制限されます。
	//! @param[in] dstSize コピー先のイメージのサイズです。
	//  - この値は、1以上である必要があります。
	//! @param[in] pSrcBuffer コピー元のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! @param[in] srcBufferOffset コピー元のバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= srcBufferOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! - ( V3DBufferDesc::size - srcBufferOffset ) の値は、実際にコピーされるサイズと同じであるか、上回っていなければなりません。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize,
		IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset) = 0;

	//! @brief バッファーをイメージにコピーします
	//! @param[in] pDstImage コピー先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout コピー先のイメージのレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL 、 ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcBuffer コピー元のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges コピーする範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DBuffer* pSrcBuffer,
		uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges) = 0;

	//! @brief イメージをバッファーにコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] dstBufferOffset コピー先のバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= dstBufferOffset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout コピー元のイメージのレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL 、::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] srcLevel コピー元のイメージのレベルです。
	//! - この値は { 0 <= srcLevel < V3DImageDesc::levelCount } の範囲に制限されます。
	//! @param[in] srcBaseLayer コピー元のイメージの最初のレイヤーです。
	//! - この値は { 0 <= srcBaseLayer < V3DImageDesc::layerCount } の範囲に制限されます。
	//! @param[in] srcLayerCount コピー元のイメージのレイヤー数です。
	//! - この値は { 1 <= srcLayerCount <= (V3DImageDesc::layerCount - srcBaseLayer) } の範囲に制限されます。
	//! @param[in] srcOffset コピー元のイメージのオフセットです。
	//! - この値は { (0, 0, 0) <= srcOffset < (V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) } の範囲に制限されます。
	//! @param[in] srcSize コピー元のイメージのサイズです。
	//! - この値は { {1, 1, 1} <= srcSize <= ((V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) - srcOffset) } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize) = 0;

	//! @brief イメージをバッファーにコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout コピー元のイメージのレイアウトです。
	//! - メージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImage はコピーを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges コピーする範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges) = 0;

	//! @brief イメージビューを転送します。
	//! @param[in] pDstImageView 転送先のイメージビューです。
	//! - アクセスするイメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 転送先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImageView は転送を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] dstRect 転送先の領域です。
	//! - この値は、転送先イメージの範囲 ( width height ) を超えて指定してはいけません。
	//! @param[in] pSrcImageView 転送元のイメージビューです。
	//! - アクセスするイメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 転送元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL 、::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImageView は転送を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] srcRect 転送元の領域です。
	//! - この値は、転送元イメージの範囲 ( width height ) を超えて指定してはいけません。
	//! @param[in] filter フィルターです。
	//! @remarks
	//! 転送操作をするイメージビューはタイプ、フォーマット、深さ、レベル数、レイヤー数、サンプル数が一致している必要があります。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BlitImage
	virtual void BlitImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DRectangle2D& dstRect,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DRectangle2D& srcRect,
		V3D_FILTER filter) = 0;

	//! @brief イメージを転送します。
	//! @param[in] pDstImage 転送先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 転送先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImage は転送を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImage 転送元のイメージです。
	//! - イメージ作成時の使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 転送元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImage は転送を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] rangeCount 転送する範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges rangeCount 値の数の要素を持つ V3DBlitImageRange 構造体の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @param[in] filter フィルターです。
	//! @remarks
	//! 転送操作をするイメージは、フォーマットによっては使用できない場合がありますので IV3DDevice::CheckImageFormatFeature を使用して事前に確認しておく必要があります。<br>
	//! また IV3DDevice::CheckImageFormatFeature の第２引数 または 第３引数である featureFlags に指定する値は以下のようになります。<br>
	//! - 転送先イメージ ( pDstImage )<br>
	//! ::V3D_IMAGE_FORMAT_FEATURE_BLIT_DST<br>
	//! - 転送元イメージ ( pSrcImage )<br>
	//! ::V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC<br>
	//! また filter に ::V3D_FILTER_LINEAR を指定する場合は ::V3D_IMAGE_FORMAT_FEATURE_SAMPLED_FILTER_LINEAR も含めてください。<br>
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BlitImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, V3DBlitImageRange* pRanges,
		V3D_FILTER filter) = 0;

	//! @brief イメージビューを転送します。
	//! @param[in] pDstImageView 転送先のイメージビューです。
	//! - アクセスするイメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 転送先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImageView は転送を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImageView 転送元のイメージビューです。
	//! - アクセスするイメージ作成時の使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 転送元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImageView は転送を開始する前にこのイメージレイアウトに移行く必要があります。
	//! @param[in] filter フィルターです。
	//! @remarks
	//! 転送操作をするイメージビューはタイプ、フォーマット、深さ、レベル数、レイヤー数、サンプル数が一致している必要があります。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BlitImage
	virtual void BlitImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout,
		V3D_FILTER filter) = 0;

	//! @brief マルチサンプルイメージを非マルチサンプルイメージに変換します。
	//! @param[in] pDstImage 変換先のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかである必要があります。<br>
	//! また pDstImage は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImage 変換元のイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。
	//! - イメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! - イメージレイアウトは ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImage は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] rangeCount 変換する範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges rangeCount 変換する範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @remarks
	//! 変換操作をするイメージビューはタイプ、フォーマットが一致している必要があります。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DResolveImageRange* pRanges) = 0;

	//! @brief マルチサンプルイメージビューを非マルチサンプルイメージビューに変換します。
	//! @param[in] pDstImageView 変換先のイメージビューです。
	//! - アクセスするイメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。<br>
	//! またサンプル数である V3DImageDesc::samples には ::V3D_SAMPLE_COUNT_1 が指定されている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! - イメージレイアウトは ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImageView は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSrcImageView 変換元のイメージビューです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。<br>
	//! またサンプル数である V3DImageDesc::samples は ::V3D_SAMPLE_COUNT_1 ではあってはいけません。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImageView は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @remarks
	//! 変換操作をするイメージビューはタイプ、フォーマット、深さ、レベル数、レイヤー数が一致している必要があります。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief マルチサンプルイメージビューを非マルチサンプルイメージビューに変換します。
	//! @param[in] pDstImageView 変換先のイメージビューです。
	//! - アクセスするイメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。<br>
	//! またサンプル数である V3DImageDesc::samples には ::V3D_SAMPLE_COUNT_1 が指定されている必要があります。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pDstImageView は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] dstOffset 変換先イメージのオフセットです。<br>
	//! - この値は、変換先イメージの範囲 ( width height ) を超えて指定してはいけません。
	//! @param[in] pSrcImageView 変換元のイメージビューです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_SRC が含まれている必要があります。<br>
	//! またサンプル数である V3DImageDesc::samples は ::V3D_SAMPLE_COUNT_1 ではあってはいけません。
	//! - アクセスするイメージのパイプラインステージには ::V3D_PIPELINE_STAGE_TRANSFER が含まれている必要があります。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_SRC のいずれかを指定します。<br>
	//! また pSrcImageView は変換を開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] srcOffset 変換元イメージのオフセットです。
	//! - この値は、変換元イメージの範囲 ( width height ) を超えて指定してはいけません。
	//! @param[in] size 変換するイメージのサイズです。
	//! - この値は変換操作をするイメージの範囲 ( imageSize - offset ) を超えて指定してはいけません。
	//! @remarks
	//! 変換操作をするイメージビューはタイプ、フォーマット、深さ、レベル数、レイヤー数が一致している必要があります。<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DPoint2D& dstOffset,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DPoint2D& srcOffset,
		const V3DSize2D& size ) = 0;

	//! @brief レンダーパスを開始します。
	//! @param[in] pRenderPass レンダーパスです。
	//! @param[in] pFrameBuffer 指定したレンダーパスに対応するフレームバッファーです。
	//! @param[in] subpassContentInline サブパスのコマンドの記録方法です。
	//! - サブパスのコマンドをこのコマンドバッファーに記録する場合は true を指定します。<br>
	//! またサブパスのコマンドをセカンダリコマンドバッファーで記録して、このコマンドバッファーで実行する場合は false を指定してください。
	//! @param[in] pRenderArea レンダリング領域です。
	//! - レンダリング領域は、アタッチメントの範囲 ( width height ) を超えて指定してはいけません。<br>
	//! また nullptr を指定した場合は、アタッチメント全体をレンダリング領域とします。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> 無効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BeginRenderPass(IV3DRenderPass* pRenderPass, IV3DFrameBuffer* pFrameBuffer, bool subpassContentInline, const V3DRectangle2D* pRenderArea = nullptr) = 0;

	//! @brief レンダーパスを終了します。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> レンダーパス内でのみ有効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void EndRenderPass() = 0;

	//! @brief 次のサブパスへ移行します。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> レンダーパス内でのみ有効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void NextSubpass() = 0;

	//! @brief イメージをクリアします。
	//! @param[in] pImage クリアするイメージです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] imageLayout クリアするイメージのレイアウトです。<br>
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL 、 ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pImage はクリアを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] clearValue クリアする値です。
	//! - クリアするイメージがカラーの場合は V3DClearValue::color 、デプスステンシルの場合は V3DClearValue::depthStencil に任意の値を指定します。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER <b>※</b> <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!       <br>
	//!       <b>※</b> クリアするイメージがデプスステンシルの場合は無効です
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief イメージビューをクリアします。
	//! @param[in] pImageView クリアするイメージのビューです。
	//! - イメージ作成時に指定した使用法である V3DImageDesc::usageFlags には ::V3D_IMAGE_USAGE_TRANSFER_DST が含まれている必要があります。
	//! @param[in] imageLayout クリアするイメージレイアウトです。
	//! - イメージレイアウトには ::V3D_IMAGE_LAYOUT_GENERAL または ::V3D_IMAGE_LAYOUT_TRANSFER_DST のいずれかを指定します。<br>
	//! また pImageView はクリアを開始する前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] clearValue クリアする値です。
	//! - クリアするイメージがカラーの場合は V3DClearValue::color 、デプスステンシルの場合は V3DClearValue::depthStencil に任意の値を指定します。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER <b>※</b> <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!       <br>
	//!       <b>※</b> クリアするイメージがデプスステンシルの場合は無効です
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief サブパスのアタッチメントをクリアします。
	//! @param[in] colorAttachmentCount クリアするアタッチメントの数です。
	//! - クリアするカラーアタッチメントが存在しない場合は 0 を指定することができます。
	//! @param[in] pColorAttachments クリアするカラーアタッチメントの記述の配列です。
	//! - colorAttachmentCount が 1以上の場合は、colorAttachmentCount の値の要素を持つ配列を指定する必要があります。<br>
	//! また colorAttachmentCount の値が 0 の場合は nullptr を指定してください。
	//! @param[in] pDepthStencilAttachment クリアするデプスステンシルアタッチメントの記述です。
	//! - デプスステンシルアタッチメントをクリアしない、または存在しない場合は nullptr を指定することができます。
	//! @param[in] rangeCount クリアする範囲の数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] pRanges クリアする範囲の配列です。
	//! - 配列の要素数は rangeCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> レンダーパス内でのみ有効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ClearAttachments(
		uint32_t colorAttachmentCount, const V3DClearColorAttachmentDesc* pColorAttachments,
		const V3DClearDepthStencilAttachmentDesc* pDepthStencilAttachment,
		uint32_t rangeCount, const V3DClearRange* pRanges) = 0;

	//! @brief パイプラインをバインドします。
	//! @param[in] pPipeline バインドするパイプラインです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BindPipeline(IV3DPipeline* pPipeline) = 0;

	//! @brief デスクリプタセットをバインドします。
	//! @param[in] pipelineType パイプラインのタイプです。
	//! @param[in] pPipelineLayout パイプラインのレイアウトです。
	//! @param[in] firstSet デスクリプタセットを設定する最初のセット番号です。
	//! - この値は { 0 <= firstSet < V3DDeviceCaps::maxBoundDescriptorSets } の範囲に制限されます。
	//! @param[in] descriptorSetCount バインドするデスクリプタセットの数です。
	//! - この値は { 1 <= descriptorSetCount <= (V3DDeviceCaps::maxBoundDescriptorSets - firstSet) } の範囲に制限されます。
	//! @param[in] ppDescriptorSets バインドするデスクリプタセットの配列です。
	//! - 配列の要素数は descriptorSetCount ある必要があります。
	//! @param[in] dynamicOffsetCount ダイナミックオフセットの数です。
	//! - この値はデスクリプタセットに格納されているダイナミックバッファーの数と一致している必要があります。
	//! @param[in] pDynamicOffsets ダイナミックオフセットの配列です。
	//! - dynamicOffsetCount が 1以上の場合は、dynamicOffsetCount 値の要素を持つ配列を指定してください。<br>
	//! また dynamicOffsetCount が 0 の場合は nullptr を指定してください。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BindDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType,	IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets,
		uint32_t dynamicOffsetCount = 0, const uint32_t* pDynamicOffsets = nullptr) = 0;

	//! @brief バーテックスバッファーをバインドします。
	//! @param[in] firstBinding 最初のバインディングのインデックスです。
	//! - この値は { 0 <= firstBinding < V3DDeviceCaps::maxVertexInputBindings } の範囲に制限されます。
	//! @param[in] bindingCount バインディングの数です。
	//! - この値は { 1 <= bindingCount < (V3DDeviceCaps::maxVertexInputBindings - firstBinding) } の範囲に制限されます。
	//! @param[in] ppBuffers バーテックスバッファーの配列です。
	//! - 配列の要素数は bindingCount である必要があります。<br>
	//! またバッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_VERTEX が含まれている必要があります。
	//! @param[in] pOffsets バーテックスバッファーのメモリオフセットの配列です。
	//! - 配列の要素数は bindingCount ある必要があります。<br>
	//! また配列の要素であるメモリオフセットは、バイト単位で指定します。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, IV3DBuffer** ppBuffers, const uint64_t* pOffsets) = 0;

	//! @brief インデックスバッファーをバインドします。
	//! @param[in] pBuffer バインドするインデックスバッファーです。
	//! - バッファー作成時に指定した使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_INDEX が含まれている必要があります。<br>
	//! またバッファーに書き込まれているインデックスの値は V3DDeviceCaps::maxDrawIndexedIndexValue 以下に制限されます。
	//! @param[in] offset バインドするインデックスバッファーのメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= offset < V3DBufferDesc::size } の範囲に制限されます。
	//! @param[in] indexType インデックスタイプを表す ::V3D_INDEX_TYPE 列挙定数のいずれかを指定します。
	//! - ::V3D_INDEX_TYPE_UINT16 は実装に依存することなく、どの環境でも指定することができます。<br>
	//! - ::V3D_INDEX_TYPE_UINT32 は V3DDeviceCaps::flags に ::V3D_CAP_FULL_DRAW_INDEX_UINT32 が含まれている場合にのみ指定することができます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BindIndexBuffer(IV3DBuffer* pBuffer, uint64_t offset, V3D_INDEX_TYPE indexType) = 0;

	//! @brief プッシュ定数を更新します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] slot プッシュ定数のスロットです。<br>
	//! - pPipelineLayout の作成時に指定した V3DConstantDesc 構造体の配列のインデックスを指定します。
	//! @param[in] pData プッシュ定数を更新するデータです。
	//! - 更新するデータのバイトサイズは V3DConstantDesc::size である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData) = 0;

	//! @brief プッシュ定数を更新します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] slot プッシュ定数のスロットです。
	//! - pPipelineLayout 作成時に指定した V3DConstantDesc 構造体の配列のインデックスを指定します。
	//! @param[in] offset プッシュ定数のメモリオフセットをバイト単位で指定します。
	//! - この値は { 0 <= offset < V3DConstantDesc::size } の範囲に制限されます。
	//! @param[in] size プッシュ定数を更新するデータのサイズをバイト単位で指定します。
	//! - この値は { 1 <= size <= (V3DConstantDesc::size - offset) } の範囲に制限されます。
	//! @param[in] pData プッシュ定数を更新するデータです。
	//! - 更新するデータのバイトサイズは V3DConstantDesc::size の値である必要がある必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData) = 0;

	//! @brief プッシュデスクリプタセットを更新します。
	//! @param[in] pipelineType パイプラインのタイプです。
	//! @param[in] pPipelineLayout パイプラインのレイアウトです。
	//! @param[in] firstSet デスクリプタセットを設定する最初のセット番号です。
	//! - この値は { 0 <= firstSet < V3DDeviceCaps::maxBoundDescriptorSets } の範囲に制限されます。
	//! @param[in] descriptorSetCount バインドするデスクリプタセットの数です。
	//! - この値は { 1 <= descriptorSetCount <= (V3DDeviceCaps::maxBoundDescriptorSets - firstSet) } の範囲に制限されます。
	//! @param[in] ppDescriptorSets バインドするデスクリプタセットの配列です。
	//! - 配列の要素数は descriptorSetCount ある必要があります。
	//! @param[in] dynamicOffsetCount ダイナミックオフセットの数です。
	//! - この値はデスクリプタセットに格納されているダイナミックバッファーの数と一致している必要があります。
	//! @param[in] pDynamicOffsets ダイナミックオフセットの配列です。
	//! - dynamicOffsetCount が 1以上の場合は、dynamicOffsetCount 値の要素を持つ配列を指定してください。<br>
	//! また dynamicOffsetCount が 0 の場合は nullptr を指定してください。
	//! @remarks
	//! - このコマンドは V3DDeviceCaps::extensionFlags に ::V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS が含まれている場合に使用することができます。
	//! .
	//! <br>
	//! <table class="basic">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void PushDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets,
		uint32_t dynamicOffsetCount = 0, const uint32_t* pDynamicOffsets = nullptr) = 0;

	//! @brief ビューポートを設定します。
	//! @param[in] firstViewport 最初のビューポートのインデックスです。
	//! - この値は { 0 <= firstViewport < V3DDeviceCaps::maxViewports } の範囲に制限されます。
	//! @param[in] viewportCount ビューポートの数です。
	//! - この値は { 1 <= viewportCount < (V3DDeviceCaps::maxViewports - firstViewport) } の範囲に制限されます。
	//! @param[in] pViewports ビューポートの配列です。
	//! - 配列の要素数は viewportCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports) = 0;

	//! @brief シザリングを設定します。
	//! @param[in] firstScissor 最初のシザリングのインデックスです。
	//! - この値は { 0 <= firstScissor < V3DDeviceCaps::maxViewports } の範囲に制限されます。
	//! @param[in] scissorCount ザリング領域の数です。
	//! - この値は { 1 <= scissorCount <= (V3DDeviceCaps::maxViewports - firstScissor) } の範囲に制限されます。
	//! @param[in] pScissors シザリング領域の配列です。
	//! - 配列の要素数は scissorCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors) = 0;

	//! @brief ステンシルの比較演算子で使用される値を設定します。
	//! @param[in] faceMask 設定するフェイスを表す ::V3D_STENCIL_FACE_FLAG 列挙定数の組み合わせです。
	//! @param[in] reference 比較演算子で使用される値です。
	//! - ステンシルのビット数を超える値を指定することはできません。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_COMPARE_OP
	virtual void SetStencilReference(V3DFlags faceMask, uint32_t reference) = 0;

	//! @brief ブレンド定数を設定します。
	//! @param[in] blendConstants ブレンド定数です。
	//! - 配列の要素 [0, 3] は RGBA を表しています。<br>
	//!   また、要素の値は { 0.0 <= blendConstants <= 1.0 } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br>::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_BLEND_FACTOR
	virtual void SetBlendConstants(const float blendConstants[4]) = 0;

	//! @brief クエリプールをリセットします。
	//! @param[in] pQueryPool リセットするクエリプールです。
	//! @param[in] firstQuery リセットする最初のクエリのインデックスです。
	//! - この値は { 0 <= firstQuery < V3DQueryPoolDesc::queryCount } の範囲に制限されます。
	//! @param[in] queryCount リセットするクエリの数です。
	//! - この値は { 1 <= queryCount <= (V3DQueryPoolDesc::queryCount - firstQuery) } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount) = 0;

	//! @brief クエリを開始します。
	//! @param[in] pQueryPool クエリを開始するクエリプールです。
	//! @param[in] query 開始するクエリのインデックスです。
	//! - この値は { 0 <= query < V3DQueryPoolDesc::queryCount } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;

	//! @brief クエリを終了します。
	//! @param[in] pQueryPool クエリを終了するクエリプールです。
	//! @param[in] query 終了するクエリのインデックスです。
	//! - この値は { 0 <= query < V3DQueryPoolDesc::queryCount } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void EndQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;

	//! @brief タイプスタンプを書き込みます。
	//! @param[in] pQueryPool タイムスタンプを管理するクエリプールです。
	//! @param[in] pipelineStage パイプラインステージです。
	//! @param[in] query タイムスタンプのクエリのインデックスです。
	//! - この値は { 0 <= query < V3DQueryPoolDesc::queryCount } の範囲に制限されます。
	//! @remarks
	//! - このコマンドは V3DDeviceCaps::queryFlags に ::V3D_QUERY_CAP_TIMESTAMP_QUERY_GRAPHICS_AND_COMPUTE が含まれている場合にのみ使用することができます。
	//! .
	//! - タイムスタンプの値は、指定したパイプラインステージの実行が完了した後に書き込まれます。<br>
	//! また書き込まれる値は、ナノ秒単位です。
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query) = 0;

	//! @brief 描画をします。
	//! @param[in] vertexCount 頂点の数です。
	//! @param[in] instanceCount インスタンスの数です。
	//! @param[in] firstVertex 最初の頂点のインデックスです。
	//! @param[in] firstInstance 最初のインスタンスのインデックスです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> レンダーパス内でのみ有効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

	//! @brief インデックスバッファを使用した描画をします。
	//! @param[in] indexCount インデックスの数です。
	//! @param[in] instanceCount インスタンスの数です。
	//! @param[in] firstIndex 最初のインデックスです。
	//! @param[in] firstInstance 最初のインスタンスのインデックスです。
	//! @param[in] vertexOffset 頂点のバイトオフセットです。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> レンダーパス内でのみ有効 </td>
	//!     <td>
	//!       ::V3D_PIPELINE_STAGE_TOP_OF_PIPE <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_INPUT <br>
	//!       ::V3D_PIPELINE_STAGE_VERTEX_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_GEOMETRY_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_FRAGMENT_SHADER <br>
	//!       ::V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS <br>
	//!       ::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset) = 0;

	//! @brief コンピュートシェーダーの作業をディスパッチします。
	//! @param[in] groupCountX X 方向にディスパッチされるグループの数です。
	//! - この値は { 1 <= groupCountX < V3DDeviceCaps::maxComputeDispatchGroupCount.x } の範囲に制限されます。
	//! @param[in] groupCountY Y 方向にディスパッチされるグループの数です。
	//! - この値は { 1 <= groupCountY < V3DDeviceCaps::maxComputeDispatchGroupCount.y } の範囲に制限されます。
	//! @param[in] groupCountZ Z 方向にディスパッチされるグループの数です。
	//! - この値は { 1 <= groupCountZ < V3DDeviceCaps::maxComputeDispatchGroupCount.z } の範囲に制限されます。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 無効 </td>
	//!     <td> ::V3D_PIPELINE_STAGE_COMPUTE_SHADER </td>
	//!     <td> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

	//! @brief セカンダリコマンドバッファーを実行します。
	//! @param[in] commandBufferCount 実行するセカンダリコマンドバッファーの数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] ppCommandBuffers セカンダリコマンドバッファーの配列です。
	//! - 配列の要素数は、commandBufferCount である必要があります。
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void ExecuteCommandBuffers(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers) = 0;

	//! @brief デバッグマーカーを開始します。
	//! @param[in] pName デバッグマーカーの名前です。
	//! @param[in] color デバッグマーカーの色です。
	//! - 配列の要素 [0, 3] は RGBA を表しています。
	//! @remarks
	//! - このコマンドは V3DDeviceCaps::extensionFlags に ::V3D_DEVICE_EXTENSION_DEBUG_MARKER が含まれている場合に使用することができます。
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BeginDebugMarker(const char* pName, const float color[4]) = 0;
	//! @brief デバッグマーカーを終了します。
	//! @remarks
	//! - このコマンドは V3DDeviceCaps::extensionFlags に ::V3D_DEVICE_EXTENSION_DEBUG_MARKER が含まれている場合に使用することができます。
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void EndDebugMarker() = 0;

	//! @brief デバッグマーカーを挿入します。
	//! @param[in] pName デバッグマーカーの名前です。
	//! @param[in] color デバッグマーカーの色です。
	//! - 配列の要素 [0, 3] は RGBA を表しています。
	//! @remarks
	//! - このコマンドは V3DDeviceCaps::extensionFlags に ::V3D_DEVICE_EXTENSION_DEBUG_MARKER が含まれている場合に使用することができます。
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>サポートするコマンドバッファー</th><th>レンダーパス内での使用</th><th>パイプラインステージの制限</th><th>サポートするキュー</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> 有効 </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void InsertDebugMarker(const char* pName, const float color[4]) = 0;

	//! @}

protected:
	//! @cond MISC
	virtual ~IV3DCommandBuffer() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// キュー
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUEUE_FLAG
//! @brief キューの機能フラグ
enum V3D_QUEUE_FLAG : V3DFlags
{
	V3D_QUEUE_GRAPHICS = 0x00000001, //!< グラフィックス操作をサポートしています。
	V3D_QUEUE_COMPUTE = 0x00000002, //!< コンピュート操作をサポートしています。
	V3D_QUEUE_TRANSFER = 0x00000004, //!< コピー操作をサポートしています。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DQueue
//! @brief キューインターフェース
// !@sa IV3DDevice::GetQueue
class V3D_DLL_API IV3DQueue : public IV3DDeviceChild
{
public:
	//! @brief キューファミリーのインデックスを取得します。
	//! @return キューファミリーのインデックスを返します。
	virtual uint32_t GetFamily() const = 0;

	//! @brief プライマリコマンドバッファーをキューに送信します。
	//! @param[in] commandBufferCount 送信するコプライマリマンドバッファーの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーの配列です。
	//! - 配列の要素数は commandBufferCount である必要があります。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。
	//! - nullptr を指定した場合は、送信完了の通知を行いません。
	//! @retval V3D_OK プライマリコマンドバッファーをキューに送信しました。
	//! @retval V3D_ERROR_FAIL プライマリコマンドバッファーをキューに送信できませんでした。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;

	//! @brief プライマリコマンドバッファーをキューに送信します。
	//! @param[in] waitSemaphoreCount コマンドバッファーを実行する前に待機するセマフォの数です。
	//! @param[in] ppWaitSemaphores コマンドバッファーを実行する前に待機するセマフォの配列です。
	//! - waitSemaphoreCount が 1 以上の場合は、waitSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - waitSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] pWaitDstStageMasks セマフォ獲得待ちが発生するパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせの配列です。
	//! - waitSemaphoreCount が 1 以上の場合は、waitSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - waitSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] commandBufferCount 送信するコプライマリマンドバッファーの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーの配列です。
	//! - 配列の要素数は commandBufferCount である必要があります。
	//! @param[in] signalSemaphoreCount コマンドバッファーの実行が完了した時に通知するセマフォの数です。
	//! @param[in] ppSignalSemaphores コマンドバッファーの実行が完了した時に通知するセマフォの配列です。
	//! - signalSemaphoreCount が 1 以上の場合は、signalSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - signalSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。
	//! - nullptr を指定した場合は、送信完了の通知を行いません。
	//! @retval V3D_OK プライマリコマンドバッファーをキューに送信しました。
	//! @retval V3D_ERROR_FAIL プライマリコマンドバッファーをキューに送信できませんでした。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(
		uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks,
		uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers,
		uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores,
		IV3DFence* pFence) = 0;

	//! @brief プライマリコマンドバッファーをキューに送信します。
	//! @param[in] pSwapChain 同期をとるスワップチェインです。
	//! @param[in] commandBufferCount 送信するコプライマリマンドバッファーの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーの配列です。
	//! - 配列の要素数は commandBufferCount である必要があります。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。
	//! - nullptr を指定した場合は、送信完了の通知を行いません。
	//! @retval V3D_OK プライマリコマンドバッファーをキューに送信しました。
	//! @retval V3D_ERROR_FAIL プライマリコマンドバッファーをキューに送信できませんでした。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! このメソッドはスワップチェイン専用になります。
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;

	//! @brief プライマリコマンドバッファーをキューに送信します。
	//! @param[in] pSwapChain 同期をとるスワップチェインです。
	//! @param[in] waitSemaphoreCount コマンドバッファーを実行する前に待機するセマフォの数です。
	//! @param[in] ppWaitSemaphores コマンドバッファーを実行する前に待機するセマフォの配列です。
	//! - waitSemaphoreCount が 1 以上の場合は、waitSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - waitSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] pWaitDstStageMasks セマフォ獲得待ちが発生するパイプラインステージを表す ::V3D_PIPELINE_STAGE_FLAG 列挙定数の組み合わせの配列です。
	//! - waitSemaphoreCount が 1 以上の場合は、waitSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - waitSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] commandBufferCount 送信するコプライマリマンドバッファーの数です。
	//! - この値は 1 以上である必要があります。
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーの配列です。
	//! - 配列の要素数は commandBufferCount である必要があります。
	//! @param[in] signalSemaphoreCount コマンドバッファーの実行が完了した時に通知するセマフォの数です。
	//! @param[in] ppSignalSemaphores コマンドバッファーの実行が完了した時に通知するセマフォの配列です。
	//! - signalSemaphoreCount が 1 以上の場合は、signalSemaphoreCount の値の要素をもつ配列を指定する必要があります。
	//! - signalSemaphoreCount が 0 の場合は nullptr を指定します。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。
	//! - nullptr を指定した場合は、送信完了の通知を行いません。
	//! @retval V3D_OK プライマリコマンドバッファーをキューに送信しました。
	//! @retval V3D_ERROR_FAIL プライマリコマンドバッファーをキューに送信できませんでした。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(
		IV3DSwapChain* pSwapChain,
		uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks,
		uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers,
		uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores,
		IV3DFence* pFence) = 0;

	//! @brief レンダリングイメージを表示します。
	//! @param[in] pSwapChain 同期をとるスワップチェインです。
	//! @retval V3D_OK レンダリングイメージの表示に成功しました。
	//! @retval V3D_ERROR_FAIL レンダリングイメージの表示に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) = 0;

	//! @brief キューがアイドル状態になるまで待機します。
	//! @retval V3D_OK キューがアイドル状態になりました。
	//! @retval V3D_ERROR_FAIL キューがアイドル状態になりませんでした。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT WaitIdle() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DQueue() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// アダプタ
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_MEMORY_HEAP_TYPE
//! @brief メモリヒープのタイプ
enum V3D_MEMORY_HEAP_TYPE : uint8_t
{
	V3D_MEMORY_HEAP_TYPE_HOST = 0, //!< ホストのメモリヒープです。
	V3D_MEMORY_HEAP_TYPE_DEVICE = 1, //!< デバイスのメモリヒープです。
};

//! @enum V3D_BUFFER_FORMAT_FEATURE_FLAG
//! @brief バッファーフォーマットのサポートフラグ
enum V3D_BUFFER_FORMAT_FEATURE_FLAG : V3DFlags
{
	//! @brief ユニフォームテクセルバッファーで使用することができます。
	V3D_BUFFER_FORMAT_FEATURE_UNIFORM_TEXEL = 0x00000008,
	//! @brief ストレージテクセルバッファーで使用することができます。
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL = 0x00000010,
	//! @brief アトミック操作をサポートするストレージテクセルバッファーで使用することができます。
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL_ATOMIC = 0x00000020,
	//! @brief バーテックスバッファーで使用することができます。
	V3D_BUFFER_FORMAT_FEATURE_VERTEX = 0x00000040,
};

//! @enum V3D_IMAGE_FORMAT_FEATURE_FLAG
//! @brief イメージフォーマットのサポートフラグ
enum V3D_IMAGE_FORMAT_FEATURE_FLAG : V3DFlags
{
	//! @brief サンプリングできるイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED = 0x00000001,
	//! @brief ストレージイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_STORAGE = 0x00000002,
	//! @brief アトミック操作をサポートするストレージイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_STORAGE_ATOMIC = 0x00000004,
	//! @brief カラーアタッチメント及びインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT = 0x00000080,
	//! @brief ブレンディングをサポートするカラーアタッチメント及びインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND = 0x00000100,
	//! @brief デプスステンシルアタッチメント及びインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT = 0x00000200,
	//! @brief 転送先のイメージとして使用することができます。
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC = 0x00000400,
	//! @brief 転送元のイメージとして使用することができます。
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	V3D_IMAGE_FORMAT_FEATURE_BLIT_DST = 0x00000800,
	//! @brief 線形フィルターを使用することができます。
	//! - ::V3D_IMAGE_FORMAT_FEATURE_SAMPLED と一緒に指定されていた場合は、サンプラーの拡大フィルタ ( V3DSamplerDesc::magFilter ) 、縮小フィルタ ( V3DSamplerDesc::minFilter ) に ::V3D_FILTER_LINEAR を指定することができます。<br>
	//! また、サンプラーのミップマップモード ( V3DSamplerDesc::mipmapMode ) には ::V3D_MIPMAP_MODE_LINEAR を指定することができます。
	//! .
	//! - ::V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC と一緒に指定されていた場合は、 IV3DCommandBuffer::BlitImage および IV3DCommandBuffer::BlitImageView の引数である filter に ::V3D_FILTER_LINEAR を指定することができます。
	//! <br>
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED_FILTER_LINEAR = 0x00001000,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAdapterDesc
//! @brief アダプタの記述
struct V3DAdapterDesc
{
	uint32_t apiVersion; //!< API バージョンです。
	uint32_t driverVersion; //!< ドライババージョンです。
	uint32_t vendorID; //!< ベンダー ID です。
	uint32_t deviceID; //!< デバイス ID です。
	char deviceName[256]; //!< ドライバー名です。
};

//! @struct V3DMemoryHeapDesc
//! @brief メモリヒープの記述
struct V3DMemoryHeapDesc
{
	V3D_MEMORY_HEAP_TYPE type; //!< メモリヒープのタイプです。
	uint64_t size; //!< メモリヒープのサイズをバイト単位で指定します。
};

//! @struct V3DMemoryTypeDesc
//! @brief メモリタイプの記述
struct V3DMemoryTypeDesc
{
	V3DFlags propertyFlags; //!< メモリ特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	uint32_t heap; //!< メモリヒープのインデックスです。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DAdapter
//! @brief アダプタインターフェース
// !@sa IV3DInstance::GetAdapter
class V3D_DLL_API IV3DAdapter : public IV3DObject
{
public:
	//! @brief アダプタの記述を取得します。
	//! @return アダプタの記述を返します。
	virtual const V3DAdapterDesc& GetDesc() const = 0;

	//! @brief メモリヒープの数を取得します。
	//! @return メモリヒープの数を返します。
	virtual uint32_t GetMemoryHeapCount() const = 0;
	//! @brief メモリヒープの記述を取得します。
	//! @param[in] memoryHeap メモリヒープのインデックスです。
	//! @return メモリヒープの記述を返します。
	virtual const V3DMemoryHeapDesc& GetMemoryHeapDesc(uint32_t memoryHeap) const = 0;

	//! @brief メモリタイプの数を取得します。
	//! @return メモリタイプの数を返します
	virtual uint32_t GetMemoryTypeCount() const = 0;
	//! @brief メモリタイプの記述を取得します。
	//! @param[in] memoryType メモリタイプのインデックスです。
	//! @return メモリタイプの記述を返します。
	virtual const V3DMemoryTypeDesc& GetMemoryTypeDesc(uint32_t memoryType) const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DAdapter() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// デバイス
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_CAP_FLAG
//! @brief 能力フラグ
enum V3D_CAP_FLAG : V3DFlags
{
	//! @brief インデックスバッファーをバインドする IV3DCommandBuffer::BindIndexBuffer の第３引数である indexType に ::V3D_INDEX_TYPE_UINT32 を指定することができます。
	//! @sa IV3DCommandBuffer::BindIndexBuffer
	V3D_CAP_FULL_DRAW_INDEX_UINT32 = 0x00000001,
	//! @brief 異方性フィルタリングが使用できることを表します。
	//! @sa V3DSamplerDesc::anisotropyEnable
	//! @sa V3DSamplerDesc::maxAnisotropy
	V3D_CAP_SAMPLER_ANISOTROPY = 0x00000002,
};

//! @enum V3D_QUERY_CAP_FLAG
//! @brief クエリの能力フラグ
enum V3D_QUERY_CAP_FLAG : V3DFlags
{
	//! @brief オクルージョンクエリ ::V3D_QUERY_TYPE_OCCLUSION で厳密な結果を取得することができます。<br>
	//! このフラグが指定されていなかった場合、オクルージョンクエリの結果はピクセルが書き込まれなかった場合は 0 ピクセルが書き込まれた場合は 1 のように返すことがあります。
	V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE = 0x00000001,
	//! @brief パイプラインの統計クエリである ::V3D_QUERY_TYPE_PIPELINE_STATISTICS をサポートします。
	V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY = 0x00000002,
	//! @brief すべてのグラフィックスキューとコンピュートキューのタイムスタンプをサポートします。
	V3D_QUERY_CAP_TIMESTAMP_QUERY_GRAPHICS_AND_COMPUTE = 0x00000004,
	//! @brief プライマリコマンドバッファーでオクルージョンクエリがアクティブになっている間 ( IV3DCommandBuffer::BeginQuery ～ IV3DCommandBuffer::EndQuery ) で実行される
	//! セカンダリコマンドバッファー ( IV3DCommandBuffer::ExecuteCommandBuffers ) がクエリを引き継ぎ、その結果を取得することができます。
	V3D_QUERY_CAP_INHERITED_QUERIES = 0x00000008,
};

//! @enum V3D_IMAGE_CAP_FLAG
//! @brief イメージの能力フラグ
enum V3D_IMAGE_CAP_FLAG : V3DFlags
{
	//! @brief イメージビューのタイプである V3DImageViewDesc::type に ::V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY を指定することができます。
	V3D_IMAGE_CAP_CUBE_ARRAY = 0x00000001,
	//! @brief ETC2 圧縮イメージをサポートします。<br>
	//! <br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8_UNORM<br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8_SRGB<br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8A1_UNORM<br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8A1_SRGB<br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8A8_UNORM<br>
	//!< ::V3D_FORMAT_ETC2_R8G8B8A8_SRGB<br>
	//!< ::V3D_FORMAT_EAC_R11_UNORM<br>
	//!< ::V3D_FORMAT_EAC_R11_SNORM<br>
	//!< ::V3D_FORMAT_EAC_R11G11_UNORM<br>
	//!< ::V3D_FORMAT_EAC_R11G11_SNORM<br>
	V3D_IMAGE_CAP_COMPRESSION_ETC2 = 0x00000002,
	//! @brief ASTC LDR 圧縮イメージをサポートします。<br>
	//! <br>
	//!< ::V3D_FORMAT_ASTC_4X4_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_4X4_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_5X4_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_5X4_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_5X5_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_5X5_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_6X5_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_6X5_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_6X6_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_6X6_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_8X5_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_8X5_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_8X6_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_8X6_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_8X8_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_8X8_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_10X5_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_10X5_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_10X6_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_10X6_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_10X8_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_10X8_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_10X10_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_10X10_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_12X10_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_12X10_SRGB<br>
	//!< ::V3D_FORMAT_ASTC_12X12_UNORM<br>
	//!< ::V3D_FORMAT_ASTC_12X12_SRGB<br>
	V3D_IMAGE_CAP_COMPRESSION_ASTC_LDR = 0x00000004,
	//! @brief BC 圧縮イメージをサポートします。<br>
	//! <br>
	//!< ::V3D_FORMAT_BC1_RGB_UNORM<br>
	//!< ::V3D_FORMAT_BC1_RGB_SRGB<br>
	//!< ::V3D_FORMAT_BC1_RGBA_UNORM<br>
	//!< ::V3D_FORMAT_BC1_RGBA_SRGB<br>
	//!< ::V3D_FORMAT_BC2_UNORM<br>
	//!< ::V3D_FORMAT_BC2_SRGB<br>
	//!< ::V3D_FORMAT_BC3_UNORM<br>
	//!< ::V3D_FORMAT_BC3_SRGB<br>
	//!< ::V3D_FORMAT_BC4_UNORM<br>
	//!< ::V3D_FORMAT_BC4_SNORM<br>
	//!< ::V3D_FORMAT_BC5_UNORM<br>
	//!< ::V3D_FORMAT_BC5_SNORM<br>
	//!< ::V3D_FORMAT_BC6H_UFLOAT<br>
	//!< ::V3D_FORMAT_BC6H_SFLOAT<br>
	//!< ::V3D_FORMAT_BC7_UNORM<br>
	//!< ::V3D_FORMAT_BC7_SRGB<br>
	V3D_IMAGE_CAP_COMPRESSION_BC = 0x00000008,
};

//! @enum V3D_SHADER_CAP_FLAG
//! @brief シェーダーの能力フラグ
enum V3D_SHADER_CAP_FLAG : V3DFlags
{
	//! @brief ジオメトリシェーダーを使用することができます。
	V3D_SHADER_CAP_GEOMETRY = 0x00000001,
	//! @brief テッセレーションシェーダーを使用することができます。
	V3D_SHADER_CAP_TESSELLATION = 0x00000002,
	//! @brief バッファへのアクセスに対して境界がチェックされます。<br>
	//! 範囲外のアクセスではアプリケーションの終了が発生してはならず、シェーダのロード、ストア、およびアトミックの影響は、後述する実装依存の動作に準拠している必要があります。<br>
	//! <ul>
	//!   <li>次のいずれかが当てはまる場合、バッファアクセスは境界外であるとみなされます。</li>
	//!   <ul>
	//!     <li>ポインターは OpImageTexelPointer によって形成され、座標は 0 より小さいか、または連結範囲内の要素全体の数以上です。</li>
	//!     <li>OpImageTexelPointer によってポインタが形成されず、指されたオブジェクトがバインドされた範囲内に完全に含まれていない。</li><br>
	//!     SPIR-V OpLoad 命令が構造体をロードし、構造体の末尾が境界外である場合、構造体のすべてのメンバは、末尾のメンバが静的に使用されていない場合でも境界外とみなされます。<br>
	//!     <li>特定の SPIR-V ブロック内のバッファアクセスが範囲外であると判断された場合、<br>16 バイト未満のアドレスにアクセスする同じ SPIR-V ブロック内の同じタイプ ( ロード、ストア、またはアトミック ) 範囲外のアドレスから離れても、範囲外と見なすことができます。</li>
	//!   </ul>
	//!   <li>範囲外バッファロードは、次の値のいずれかを返します。</li>
	//!   <ul>
	//!     <li>バッファにバインドされたメモリ範囲内の任意の値。( バッファの最後を過ぎたメモリのバイト数、バインドされた範囲の終わりまでのバイト数を含む可能性があります )</li>
	//!     <li>ゼロ値またはベクトル読み取りの (0, 0, 0, x) ベクトル。ここで、xはベクトル成分の型で表される有効な値であり、次のいずれかになります。</li>
	//!     <ul>
	//!       <li>符号付きまたは符号なし整数コンポーネントの場合、0,1、または最大表現可能な正の整数値</li>
	//!       <li>浮動小数点の場合は 0.0 または 1.0</li>
	//!     </ul>
	//!   </ul>
	//!   <li>範囲外書き込みは、バッファにバインドされたメモリ範囲内の値を変更できますが、他のメモリは変更しないでください。</li>
	//!   <li>範囲外のアトミックは、バッファにバインドされたメモリ範囲内の値を変更することができますが、他のメモリを変更して未定義の値を返すことはできません。</li>
	//!   <li>頂点入力属性は、属性のアドレスに属性のサイズを加えたものがバインドされたバッファのサイズより大きい場合、範囲外と見なされます。<br>さらに、特定の頂点入力バインディングを使用する任意の頂点入力属性が範囲外である場合、<br>その頂点シェーダ呼び出しに対するその頂点入力バインディングを使用するすべての頂点入力属性は範囲外と見なされます。</li>
	//!   <ul>
	//!     <li>頂点入力属性が範囲外である場合、次のいずれかの値が割り当てられます。</li>
	//!     <ul>
	//!       <li>バッファにバインドされたメモリ範囲内の任意の値。属性の形式に従って変換されます。</li>
	//!       <li>0 の値、属性の形式に従ってフォーマットが変換されます。</li>
	//!       <li>0 の値、または (0, 0, 0, x）ベクトル。</li>
	//!     </ul>
	//!   </ul>
	//!   <li>V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS が有効になっていないと、境界外アクセスによってプロセス内のメモリが破壊され、アプリケーション終了までの未定義の動作が発生する可能性があります。</li>
	//! </ul>
	V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS = 0x00000004,
	//! @brief ストレージバッファまたはイメージが、バーテックスシェーダーステージ、テッセレーションシェーダーステージ、ジオメトリシェーダステージにおいて、
	//! ストアとアトミックオペレーションをサポートします。<br>
	//! この機能が有効になっていない場合、シェーダモジュールでこれらのステージで使用されるすべてのストレージイメージ、
	//! ストレージテクセルバッファ、およびストレージバッファ変数に NonWriteable readonly（ または GLSLのメモリ修飾子 ）を装飾する必要があります。
	V3D_SHADER_CAP_VERTEX_PIPELINE_STORES_AND_ATOMICS = 0x00000008,
	//! @brief ストレージバッファとイメージがフラグメントシェーダステージのストアとアトミックオペレーションをサポートします。<br>
	//! この機能が有効になっていない場合、シェーダモジュールのフラグメントステージで使用されるすべてのストレージイメージ、ストレージテクセルバッファ、およびストレージバッファ変数は、NonWriteable readonly（ またはGLSLのメモリ修飾子 ）で装飾されている必要があります。
	V3D_SHADER_CAP_FRAGMENT_STORES_AND_ATOMICS = 0x00000010,
	//! @brief 1.0 より大きいサイズのポイントが使用できます。<br>
	//! この機能が有効になっていない場合、シェーダーによって書き込まれたポイントサイズ 1.0 のみがサポートされます。<br>
	//! サポートされているポイントサイズの範囲と粒度は、それぞれ V3DDeviceCaps 構造体の pointSizeRange および pointSizeGranularity になります。
	V3D_SHADER_CAP_LARGE_POINTS = 0x00000020,
	//! @brief テッセレーション制御シェーダー、テッセレーション評価シェーダー、およびジオメトリシェーダーで gl_PointSize 変数が使用できます。<br>
	//! この機能が有効になっていない場合は、gl_PointSize 変数を読み書きしないでください。<br>
	//! また、テッセレーションシェーダーまたはジオメトリシェーダーから書き込まれるポイントのサイズは 1.0 になります。<br>
	//! これは、シェーダモジュールがテッセレーション制御シェーダーとテッセレーション評価シェーダーの TessellationPointSize 機能、
	//! またはジオメトリシェーダーの GeometryPointSize 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_TESSELLATION_AND_GEOMETRY_POINTSIZE = 0x00000040,
	//! @brief image gather 命令の拡張セットがシェーダコードで利用することができます。<br>
	//! この機能が有効でない場合、OpImage * Gather 命令は Offset オペランドと ConstOffsets オペランドをサポートしません。<br>
	//! これは、シェーダモジュールが ImageGatherExtended 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_IMAGE_GATHER_EXTENDED = 0x00000080,
	//! @brief 拡張ストレイージイメージフォーマットがシェーダコードで利用することができます。<br>
	//! この機能が有効になっていない場合、StorageImageExtendedFormats 機能が必要な形式は、ストレージイメージではサポートされていません。<br>
	//! これは、シェーダーモジュールが StorageImageExtendedFormats 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_IMAGE_EXTENDED_FORMATS = 0x00000100,
	//! @brief マルチサンプルのストレージイメージをサポートします。<br>
	//! この機能が有効になっていない場合 ::V3D_IMAGE_USAGE_STORAGE を含む使用法でイメージを作成するときのサンプル数は ::V3D_SAMPLE_COUNT_1 にする必要があります。<br>
	//! これは、シェーダモジュールが StorageImageMultisample 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_IMAGE_MULTISAMPLE = 0x00000200,
	//! @brief ストレージイメージから読み取るときに、ストレージイメージでフォーマット修飾子を指定する必要があるかどうかを表します。<br>
	//! この機能が有効になっていない場合、OpImageRead 命令の OpTypeImage が Unknown であってはなりません。<br>
	//! これは、シェーダーモジュールが StorageImageReadWithoutFormat 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_IMAGE_READ_WITH_OUT_FORMAT = 0x00000400,
	//! @brief ストレージイメージに書き込むときに format 装飾子を指定する必要があるかどうかを表します。<br>
	//! この機能が有効になっていない場合、OpImageWrite 命令の OpTypeImage が Unknown であってはなりません。<br>
	//! これは、シェーダモジュールが StorageImageWriteWithoutFormat 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_IMAGE_WRITE_WITH_OUT_FORMAT = 0x00000800,
	//! @brief ユニフォームバッファの配列がシェーダコード内の動的に一様な整数式によって索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER または ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC のリソースは、
	//! シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが UniformBufferArrayDynamicIndexing 機能を宣言できるかどうかを表します。<br>
	V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00001000,
	//! @brief シェーダコード内の動的に一様な整数式によってサンプラまたはサンプルイメージの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが ::V3D_DESCRIPTOR_TYPE_SAMPLER 、::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 、
	//! または ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE のリソースは、シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが SampledImageArrayDynamicIndexing 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00002000,
	//! @brief シェーダコード内の動的に一様な整数式によってストレージバッファの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER または ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC のリソースは、
	//! シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが StorageBufferArrayDynamicIndexing 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00004000,
	//! @brief シェーダコード内の動的に一様な整数式によって格納イメージの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE のリソースは、
	//! シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが StorageImageArrayDynamicIndexing 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00008000,
	//! @brief シェーダーコードでクリップ距離をサポートします。<br>
	//! この機能が有効になっていない場合、ClipDistance で装飾されたメンバーは、シェーダーモジュールとの間で読み書きすることはできません。<br>
	//! これは、シェーダモジュールが ClipDistance 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_CLIP_DISTANCE = 0x00010000,
	//! @brief シェーダコードでカリング距離をサポートします。<br>
	//! この機能が有効になっていない場合、CullDistance で装飾されたメンバーは、シェーダーモジュールとの間で読み書きすることはできません。<br>
	//! これは、シェーダモジュールが CullDistance 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_CULL_DISTANCE = 0x00020000,
	//! @brief シェーダコードで 64 ビット浮動小数点数（ double 型 ）を使用することができます。<br>
	//! この機能が有効になっていない場合は、64 ビットの浮動小数点型をシェーダコードで使用しないでください。<br>
	//! これは、シェーダモジュールが Float64 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_FLOAT64 = 0x00040000,
	//! @brief シェーダコードで 64 ビットの整数を使用することができます。<br>
	//! この機能が有効になっていない場合、シェーダコードでは 64 ビット整数型を使用しないでください。<br>
	//! これは、シェーダモジュールが Int64 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_INT64 = 0x00080000,
	//! @brief シェーダコードで 16 ビット整数を使用することができます。<br>
	//! この機能が有効になっていない場合、シェーダコードでは 16 ビット整数型を使用しないでください。<br>
	//! これは、シェーダモジュールが Int16 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_INT16 = 0x00100000,
	//! @brief リソース常駐情報を返すイメージ操作がシェーダコードでサポートされているかどうかを表します。<br>
	//! この機能が有効になっていない場合は、OpImageSparse * 命令をシェーダコードで使用しないでください。<br>
	//! これは、シェーダモジュールが SparseResidency 機能を宣言できるかどうかを表します。<br>
	//! この機能では、少なくとも1つの sparseResidency * 機能がサポートされている必要があります。
	V3D_SHADER_CAP_RESOURCE_RESIDENCY = 0x00200000,
	//! @brief 最小リソースレベル詳細 (LOD) を指定するイメージ操作がシェーダコードをサポートします。<br>
	//! この機能が有効になっていない場合、シェーダコードで MinLod イメージオペランドを使用しないでください。<br>
	//! これは、シェーダモジュールが MinLod 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_RESOURCE_MIN_LOD = 0x00400000,
};

//! @enum V3D_RASTERIZER_CAP_FLAG
//! @brief ラスタライザーの能力フラグ
enum V3D_RASTERIZER_CAP_FLAG : V3DFlags
{
	//! @brief 複数のビューポート、シザリングをサポートします。<br>
	//! また、ビューポート、シザリングの最大数は V3DDeviceCaps::maxViewports になります。
	//! @sa V3DPipelineRasterizationDesc
	//! @sa IV3DCommandBuffer::SetViewport
	//! @sa IV3DCommandBuffer::SetScissor
	V3D_RASTERIZER_CAP_MULTI_VIEWPORT = 0x00000001,
	//! @brief ポリゴンモードに ::V3D_POLYGON_MODE_LINE 、 ::V3D_POLYGON_MODE_POINT を指定することができます。
	//! @sa V3DPipelineRasterizationDesc::polygonMode
	V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID = 0x00000002,
	//! @brief 深度のクランプをサポートします。
	//! @sa V3DPipelineRasterizationDesc::depthClampEnable
	V3D_RASTERIZER_CAP_DEPTH_CLAMP = 0x00000010,
	//! @brief 深度バイアスのクランプをサポートします。
	//! @sa V3DPipelineRasterizationDesc::depthBiasClamp
	V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP = 0x00000020,
};

//! @enum V3D_MULTISAMPLE_CAP_FLAG
//! @brief マルチサンプルの能力フラグ
enum V3D_MULTISAMPLE_CAP_FLAG : V3DFlags
{
	//! @brief サブパスのカラーアタッチメント、デプスステンシルアタッチメントのサンプル数が V3DPipelineMultisampleDesc::rasterizationSamples と同じ値である必要がないことを表します。<br>
	//! このフラグが指定されていなかった場合、サブパスの全てのカラーアタッチメント、デプスステンシルアタッチメントのサンプル数は V3DPipelineMultisampleDesc::rasterizationSamples と同じ値でないといけません。
	V3D_MULTISAMPLE_CAP_VARIABLE_RATE = 0x00000001,
	//! @brief サンプル毎のシェーディングとマルチサンプルの補間をサポートします。<br>
	//! このフラグが指定されている場合は V3DPipelineMultisampleDesc::sampleShadingEnable に true に指定することができます。
	V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING = 0x00000002,
	//! @brief フラグメントシェーダから出力されたアルファ値を、固定小数点が表現できる最大値または 1.0 で置き換えることができます。<br>
	//! このフラグが指定されていた場合、 V3DPipelineMultisampleDesc::alphaToOneEnable にtrue を指定することができます。
	V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE = 0x00000004,
};

//! @enum V3D_DEPTH_STENCIL_CAP_FLAG
//! @brief デプスステンシルの能力フラグ
enum V3D_DEPTH_STENCIL_CAP_FLAG : V3DFlags
{
	//! @brief 深度の境界テストをサポートします。<br>
	//! このフラグが指定されていた場合、V3DPipelineDepthStencilDesc::depthBoundsTestEnable に true を指定することができます。
	V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS = 0x00000001,
};

//! @enum V3D_COLOR_BLEND_CAP_FLAG
//! @brief ブレンドの能力フラグ
enum V3D_COLOR_BLEND_CAP_FLAG : V3DFlags
{
	//! @brief 複数のカラーアタッチメントに異なるブレンドを指定することができます。
	//! @sa V3DPipelineColorBlendDesc
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_INDEPENDENT = 0x00000001,
	//! @brief 二つのソースを使用するブレンドをサポートします。<br>
	//! <br>
	//! ::V3D_BLEND_FACTOR_SRC1_COLOR<br>
	//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR<br>
	//! ::V3D_BLEND_FACTOR_SRC1_ALPHA<br>
	//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA<br>
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_DUAL_SRC = 0x00000002,
	//! @brief カラーのブレンドで論理演算子をサポートします。
	//! @sa V3DPipelineColorBlendDesc::logicOpEnable
	//! @sa V3DPipelineColorBlendDesc::logicOp
	V3D_COLOR_BLEND_CAP_LOGIC_OP = 0x00000004,
};

//! @enum V3D_DEVICE_EXTENSION_FLAG
//! @brief デバイス拡張機能フラグ
enum V3D_DEVICE_EXTENSION_FLAG : V3DFlags
{
	//! @brief プッシュデスクリプタセットを使用することができます。
	//! @sa IV3DCommandBuffer::PushDescriptorSets
	V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS = 0x00000001,
	//! @brief デバッグマーカーを使用することができます。
	//! @remarks
	//! この拡張機能が使用できるレイヤーは ::V3D_LAYER_NSIGHT または ::V3D_LAYER_RENDERDOC のいずれかになります。
	//! @sa IV3DCommandBuffer::BeginDebugMarker
	//! @sa IV3DCommandBuffer::EndDebugMarker
	//! @sa IV3DCommandBuffer::InsertDebugMarker
	V3D_DEVICE_EXTENSION_DEBUG_MARKER = 0x00000002,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDeviceCaps
//! @brief デバイスの能力
//! @sa IV3DDevice::GetCaps
struct V3DDeviceCaps
{
	V3DFlags flags; //!< 全般的な能力を表す ::V3D_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags queryFlags; //!< クエリに関するの能力を表す ::V3D_QUERY_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags imageFlags; //!< イメージに関するの能力を表す ::V3D_IMAGE_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags shaderFlags; //!< シェーダーに関するの能力を表す ::V3D_SHADER_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags rasterizerFlags; //!< ラスタライザーに関する能力を表す ::V3D_RASTERIZER_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags multisampleFlags; //!< マルチサンプルに関する能力を表す ::V3D_MULTISAMPLE_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags depthStencilFlags; //!< デプスステンシルに関する能力を表す ::V3D_DEPTH_STENCIL_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags colorBlendFlags; //!< カラーブレンドに関する能力を表す ::V3D_COLOR_BLEND_CAP_FLAG 列挙定数の組み合わせです。
	V3DFlags extensionFlags; //!< デバイスの拡張機能を表す ::V3D_DEVICE_EXTENSION_FLAG 列挙定数の組み合わせです。

	//! @brief 1Dイメージの幅の最大値です。
	uint32_t maxImageDimension1D;
	//! @brief 2Dイメージの幅、高さの最大値です。
	uint32_t maxImageDimension2D;
	//! @brief 3Dイメージの幅、高さ、深さの最大値です。
	uint32_t maxImageDimension3D;
	//! @brief キューブイメージの幅、高さ最大値です。
	uint32_t maxImageDimensionCube;
	//! @brief イメージの最大レイヤー数です。
	uint32_t maxImageLayers;
	//! @brief テクセルバッファーの要素の最大数です。
	uint32_t maxTexelBufferElements;
	//! @brief ユニフォームバッファーの最大サイズです。
	uint32_t maxUniformBufferSize;
	//! @brief ストレージバッファーの最大サイズです。
	uint32_t maxStorageBufferSize;
	//! @brief IV3DCommandBuffer::PushConstant で更新できる定数の最大サイズをバイト単位で指定します。
	uint32_t maxPushConstantsSize;
	//! @brief IV3DCommandBuffer::PushDescriptorSets で更新できるデスクリプタの数です。<br>
	//! デスクリプタセットの数ではなく、デスクリプタセットに格納されているデスクリプタの数であるということに注意してください。
	uint32_t maxPushDescriptors;
	//! @brief デバイスが確保できるリソースメモリの最大数です。
	//! @sa IV3DDevice::AllocateResourceMemory
	uint32_t maxResourceMemoryCount;
	//! @brief デバイスが作成できるサンプラーの最大数です。
	//! @sa IV3DDevice::CreateSampler
	uint32_t maxSamplerCreateCount;
	//! @brief エイリアシングのない同じメモリ内の隣接するバッファーまたはイメージを結合することができるオフセットの粒度をバイト単位で指定します。
	uint64_t bufferImageGranularity;
	//! @brief パイプラインで設定できるデスクリプタセットの最大数です。
	uint32_t maxBoundDescriptorSets;

	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるサンプラーの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLER 、::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorSamplers;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるユニフォームバッファーの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER 、::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! </ul>
	uint32_t maxPerStageDescriptorUniformBuffers;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるストレージバッファーの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER 、::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	//! </ul>
	uint32_t maxPerStageDescriptorStorageBuffers;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるサンプリングイメージの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 、::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE または ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorSampledImages;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるストレージイメージの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br> または ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorStorageImages;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるインプットアタッチメントの最大数です。
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	//! </ul>
	uint32_t maxPerStageDescriptorInputAttachments;
	//! @brief パイプラインレイアウトで一つのシェーダーステージにアクセスできるリソースの最大数です。<br>
	//! <ul>
	//! <li>制限が適用されるデスクリプター</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	//! または<br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	//! <br>
	//! またフラグメントシェーダステージの場合、フレームバッファーのカラーアタッチメントにもこの制限が適用されます。
	//! </ul>
	uint32_t maxPerStageResources;

	//! @brief デスクリプタセットに設定できるサンプラーの最大数です。
	uint32_t maxDescriptorSetSamplers;
	//! @brief デスクリプタセットに設定できるユニフォームバッファーの最大数です。
	uint32_t maxDescriptorSetUniformBuffers;
	//! @brief デスクリプタセットに設定できるダイナミックユニフォームバッファーの最大数です。
	uint32_t maxDescriptorSetUniformBuffersDynamic;
	//! @brief デスクリプタセットに設定できるストレージバッファーの最大数です。
	uint32_t maxDescriptorSetStorageBuffers;
	//! @brief デスクリプタセットに設定できるダイナミックストレージバッファーの最大数です。
	uint32_t maxDescriptorSetStorageBuffersDynamic;
	//! @brief デスクリプタセットに設定できるサンプリングイメージの最大数です。
	uint32_t maxDescriptorSetSampledImages;
	//! @brief デスクリプタセットに設定できるストレージイメージの最大数です。
	uint32_t maxDescriptorSetStorageImages;
	//! @brief デスクリプタセットに設定できるインプットアタッチメントの最大数です。
	uint32_t maxDescriptorSetInputAttachments;

	//! @brief 頂点要素の数である V3DPipelineVertexInputDesc::elementCount の最大値です。
	uint32_t maxVertexInputElements;
	//! @brief 一度にバインドできる頂点バッファの最大数です。
	//! @sa IV3DCommandBuffer::BindVertexBuffers
	uint32_t maxVertexInputBindings;
	//! @brief 頂点要素の最大オフセットをバイト単位で指定します。
	uint32_t maxVertexInputElementOffset;
	//! @brief 頂点レイアウトのストライドである V3DPipelineVertexLayout::stride の最大サイズをバイト単位で指定します。
	uint32_t maxVertexInputBindingStride;
	//! @brief バーテックスシェーダーで宣言することのできる出力変数の最大数です。
	uint32_t maxVertexOutputComponents;

	//! @brief テッセレーションプリミティブジェネレーターの最大テッセーレーション生成レベルです。
	uint32_t maxTessellationGenerationLevel;
	//! @brief テッセーレーション制御シェーダーおよびテッセーレーションプリミティブジェネレーターで処理可能な最大パッチサイズです。<br>
	//! グラフィックスパイプライン作成時に指定した V3DPipelineTessellationDesc::patchControlPoints の値と、シェーダーの OutputVertices 実行モードで指定された値は、この制限値以下でなければなりません。
	uint32_t maxTessellationPatchSize;
	//! @brief テッセレーション制御シェーダーで宣言することのできる頂点毎の入力変数の最大数です。
	uint32_t maxTessellationControlPerVertexInputComponents;
	//! @brief テッセレーション制御シェーダーで宣言することのできる頂点毎の出力変数の最大数です。
	uint32_t maxTessellationControlPerVertexOutputComponents;
	//! @brief テッセレーション制御シェーダーで宣言することのできるパッチ毎の出力変数の最大数です。
	uint32_t maxTessellationControlPerPatchOutputComponents;
	//! @brief テッセレーション制御シェーダーで宣言することのできる頂点毎、及びパッチ毎の出力変数の最大合計数です。
	uint32_t maxTessellationControlTotalOutputComponents;
	//! @brief テッセレーション評価シェーダーで宣言することのできる頂点毎の入力変数の最大数です。
	uint32_t maxTessellationEvaluationInputComponents;
	//! @brief テッセレーション評価シェーダーで宣言することのできる頂点毎の出力変数の最大数です。
	uint32_t maxTessellationEvaluationOutputComponents;

	//! @brief ジオメトリシェーダーの最大呼び出し回数です。<br>
	//! シェーダーモジュールの Invocations 実行モードで指定された値は、この制限値以下でなければなりません。
	uint32_t maxGeometryShaderInvocations;
	//! @brief ジオメトリシェーダーで宣言することのできる入力変数の最大数です。
	uint32_t maxGeometryInputComponents;
	//! @brief ジオメトリシェーダーで宣言することのできる出力変数の最大数です。
	uint32_t maxGeometryOutputComponents;
	//! @brief ジオメトリシェーダーが出力することができる頂点の最大数です。
	uint32_t maxGeometryOutputVertices;
	//! @brief ジオメトリシェーダーで宣言することのできる出力変数の最大合計数です。
	uint32_t maxGeometryTotalOutputComponents;

	//! @brief フラグメントシェーダーで宣言することのできる入力変数の最大数です。
	uint32_t maxFragmentInputComponents;
	//! @brief フラグメントシェーダーが出力できるアタッチメントの最大数です。
	uint32_t maxFragmentOutputAttachments;
	//! @brief ブレンドが有効で、デュアルソースブレンドモードの1つが使用されているときに、フラグメントシェーダのステージによって書き込まれる出力アタッチメントの最大数です。
	uint32_t maxFragmentDualSrcAttachments;
	//! @brief フラグメントシェーダーで出力できるストレージバッファー、ストレージイメージの総数です。
	uint32_t maxFragmentCombinedOutputResources;

	//! @brief コンピュートシェーダーで WorkgroupLocal ストレージクラスで宣言されたすべての変数の最大合計ストレージサイズをバイト単位で指定します。
	uint32_t maxComputeSharedMemorySize;
	//! @brief コンピュートシェーダーで次元 ( x y z ) ごとにディスパッチできるグループ数です。
	V3DPoint3D maxComputeDispatchGroupCount;
	//! @brief ディスパッチグループで呼び出せるコンピュートシェーダーの最大回数です。
	uint32_t maxComputeDispatchGroupInvocations;
	//! @brief コンピュートシェーダーで次元 ( x y z ) ごとのディスパッチグループの最大サイズです。
	V3DPoint3D maxComputeDispatchGroupSize;

	//! @brief フレームバッファ座標 Xfと Yf におけるサブピクセル精度を表すビット数です。
	uint32_t subPixelPrecisionBits;
	//! @brief 縮小および拡大フィルタに使用される画像の軸に沿った除算における精度を表すビット数です。<br>
	//! この値はイメージの各軸に沿った分割の実際の数です。<br>
	//! またフィルタリングされた結果を計算するときに、フィルタリングハードウェアがこれらの場所にスナップします。
	uint32_t subTexelPrecisionBits;
	//! @brief 各ミップレベルからフィルタリング結果への寄与を決定するときに、フェッチするための LOD 計算がスナップされるビット数です。
	uint32_t mipmapPrecisionBits;

	//! @brief 描画の際にバインドするインデックスバッファに格納できるインデックスの最大値です。
	uint32_t maxDrawIndexedIndexValue;

	//! @brief サンプラーの LOD バイアスの絶対値です。<br>
	//! V3DSamplerDesc::mipLodBias の値は -maxSamplerLodBias ～ +maxSamplerLodBias の間にクランプします。
	float maxSamplerLodBias;
	//! @brief サンプラーの異方性値である V3DSamplerDesc::maxAnisotropy の最大値です。
	float maxSamplerAnisotropy;

	//! @brief ビューポート、シザリングの最大数です。
	uint32_t maxViewports;
	//! @brief ビューポートの最大サイズです。
	V3DSize2D maxViewportDimension;
	//! @brief ビューポートの角が含まれていなければならない範囲の最小値です。
	float minViewportBounds;
	//! @brief ビューポートの角が含まれていなければならない範囲の最大です。
	float maxViewportBounds;
	//! @brief ビューポート境界のサブピクセルの精度を表すのビット数です。<br>
	//! ビューポート境界が判断されるサブピクセルの精度は、この制限によって与えられます。
	uint32_t viewportSubPixelBits;

	//! @brief IV3DResource::Map でリソースをマップする際のオフセットのアライメントをバイト単位で指定します。
	size_t minMemoryMapAlignment;
	//! @brief テクセルバッファのオフセットのアライメントをバイト単位で指定します。
	uint64_t minTexelBufferOffsetAlignment;
	//! @brief ユニフォームバッファのオフセットのアライメントをバイト単位で指定します。
	uint64_t minUniformBufferOffsetAlignment;
	//! @brief ストレージバッファのオフセットのアライメントをバイト単位で指定します。
	uint64_t minStorageBufferOffsetAlignment;

	//! @brief OpImageSample * または OpImageFetch * イメージ命令の ConstOffset イメージオペランドの最小オフセット値です。
	int32_t minTexelOffset;
	//! @brief OpImageSample * または OpImageFetch * イメージ命令の ConstOffset イメージオペランドの最大オフセット値です。
	uint32_t maxTexelOffset;
	//! @brief OpImage * Gather イメージ命令の Offset または ConstOffsets イメージオペランドの最小オフセット値です。
	int32_t minTexelGatherOffset;
	//! @brief OpImage * Gather イメージ命令の Offset または ConstOffsets イメージオペランドの最大オフセット値です。
	uint32_t maxTexelGatherOffset;

	//! @brief InterpolateAtOffset 拡張命令の Offset オペランドの負の最小オフセット値です。
	float minInterpolationOffset;
	//! @brief InterpolateAtOffset 拡張命令の Offset オペランドの正の最大オフセット値です。
	float maxInterpolationOffset;
	//! @brief InterpolateAtOffset 拡張命令に対する x および y オフセットが固定小数点値として丸められるサブピクセルのビット数です。
	uint32_t subPixelInterpolationOffsetBits;

	//! @brief フレームバッファーのアタッチメントの幅の最大です。
	uint32_t maxFrameBufferWidth;
	//! @brief フレームバッファーのアタッチメントの高さの最大です。
	uint32_t maxFrameBufferHeight;
	//! @brief フレームバッファーのアタッチメントのレイヤーの最大数です。
	uint32_t maxFrameBufferLayers;

	//! @brief フレームバッファーのカラーアタッチメントでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags framebufferColorSampleCounts;
	//! @brief フレームバッファーのデプスアタッチメントでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags framebufferDepthSampleCounts;
	//! @brief フレームバッファーのステンシルアタッチメントでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags framebufferStencilSampleCounts;
	//! @brief アタッチメントのないフレームバッファーでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags framebufferNoAttachmentsSampleCounts;

	//! @brief 非整数カラーフォーマットで作成された 2D イメージでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags sampledImageColorSampleCounts;
	//! @brief 整数カラーフォーマットで作成された 2D イメージでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags sampledImageIntegerSampleCounts;
	//! @brief デプスフォーマットで作成された 2D イメージでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags sampledImageDepthSampleCounts;
	//! @brief ステンシルフォーマットで作成された 2D イメージでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags sampledImageStencilSampleCounts;
	//! @brief ストレージとして作成された 2D イメージでサポートされるサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
	V3DFlags storageImageSampleCounts;

	//! @brief サブパスのカラーアタッチメントの最大数です。
	uint32_t maxColorAttachments;

	//! @brief gl_SampleMask 配列の要素の最大数です。
	uint32_t maxSampleMaskWords;

	//! @brief タイムスタンプクエリをイクンリメントするために必要な時間をナノ秒単位で指定します。
	float timestampPeriod;

	//! @brief gl_ClipDistance 配列の要素の最大数です。
	uint32_t maxClipDistances;
	//! @brief gl_CullDistance 配列の要素の最大数です。
	uint32_t maxCullDistances;
	//! @brief gl_ClipDistance および gl_CullDistance 配列のペアの合計の最大数です。
	uint32_t maxCombinedClipAndCullDistances;

	//! @brief gl_PointSize 最小値です。<br>
	//! gl_PointSize 変数の値は、minPointSize 以上である必要があります。
	float minPointSize;
	//! @brief gl_PointSize の最大値です。<br>
	//! gl_PointSize 変数の値は、maxPointSize 以下である必要があります。
	float maxPointSize;
	//! @brief gl_PointSize の増分です。<br>
	//! minPointSize ～  maxPointSize によって定義された範囲内のすべてのポイントサイズがサポートされていないことに注意してください。
	float pointSizeGranularity;

	//! @brief デバイス作成時に指定するキューにスケージューリングとして割り当てることができる個別の優先順位の数です。
	uint32_t discreteQueuePriorities;
	//! @brief マルチサンプリングで標準のサンプル位置を使用します。<br>
	//! 標準のサンプル位置は以下のようになります。<br>
	//! <br>
	//! <table class="basic">
	//! <tr><th>V3D_SAMPLE_COUNT_1</th><th>V3D_SAMPLE_COUNT_2</th><th>V3D_SAMPLE_COUNT_4</th><th>V3D_SAMPLE_COUNT_8</th><th>V3D_SAMPLE_COUNT_16</th></tr>
	//! <tr valign="top">
	//!   <td>
	//!   (0.5, 0.5)<br>
	//!   </td>
	//!   <td>
	//!   (0.25,0.25)<br>
	//!   (0.75,0.75)<br>
	//!   </td>
	//!   <td>
	//!   (0.375, 0.125)<br>
	//!   (0.875, 0.375)<br>
	//!   (0.125, 0.625)<br>
	//!   (0.625, 0.875)<br>
	//!   </td>
	//!   <td>
	//!   (0.5625, 0.3125)<br>
	//!   (0.4375, 0.6875)<br>
	//!   (0.8125, 0.5625)<br>
	//!   (0.3125, 0.1875)<br>
	//!   (0.1875, 0.8125)<br>
	//!   (0.0625, 0.4375)<br>
	//!   (0.6875, 0.9375)<br>
	//!   (0.9375, 0.0625)<br>
	//!   </td>
	//!   <td>
	//!   (0.5625, 0.5625)<br>
	//!   (0.4375, 0.3125)<br>
	//!   (0.3125, 0.625)<br>
	//!   (0.75, 0.4375)<br>
	//!   (0.1875, 0.375)<br>
	//!   (0.625, 0.8125)<br>
	//!   (0.8125, 0.6875)<br>
	//!   (0.6875, 0.1875)<br>
	//!   (0.375, 0.875)<br>
	//!   (0.5, 0.0625)<br>
	//!   (0.25, 0.125)<br>
	//!   (0.125, 0.75)<br>
	//!   (0.0, 0.5)<br>
	//!   (0.9375, 0.25)<br>
	//!   (0.875, 0.9375)<br>
	//!   (0.0625, 0.0)<br>
	//!   </td>
	//! </tr>
	//! </table>
	bool standardSampleLocations;
	//! @brief IV3DCommandBuffer::CopyBufferToImage および IV3DCommandBuffer::CopyImageToBuffer で指定するバッファーのの最適なオフセットアラインメントです。<br>
	//! アプリケーションは最適なパフォーマンスを得るために最適なアライメントを使用する必要があります。
	uint64_t optimalBufferCopyOffsetAlignment;
	//! @brief ホストでマップされたメモリへの同時アクセスを制限するサイズとアライメントをバイト単位で指定します。
	uint64_t nonCoherentAtomSize;
};

//! @struct V3DDeviceStatistics
//! @brief デバイスの統計
struct V3DDeviceStatistics
{
	uint32_t samplerCount; //!< 作成されたサンプラーの数です。
	uint32_t resourceMemoryCount; //!< 作成されたリソースメモリの数です。
};

//! @struct V3DDeviceQueueFamilyDesc
//! @brief キューファミリーの記述
struct V3DDeviceQueueFamilyDesc
{
	V3DFlags queueFlags; //!< キューの操作を表す ::V3D_QUEUE_FLAG 列挙定数の組み合わせです。
	uint32_t queueCount; //!< キューの数です。
};

//! @struct V3DImageFormatDesc
//! @brief イメージフォーマットの記述
struct V3DImageFormatDesc
{
	uint32_t maxWidth; //!< イメージの最大の幅です。
	uint32_t maxHeight; //!< イメージの最大の高さです。
	uint32_t maxDepth; //!< イメージの最大の深さです。
	uint32_t maxLevels; //!< イメージの最大レベル数です。
	uint32_t maxLayers; //!< イメージの最大レイヤー数です。
	V3DFlags sampleCountFlags; //!< イメージのサンプル数を表す ::V3D_SAMPLE_COUNT_FLAG 列挙定数の組み合わせです。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDevice
//! @brief デバイスインターフェース
// !@sa IV3DInstance::CreateDevice
class V3D_DLL_API IV3DDevice : public IV3DObject
{
public:
	//! @brief デバイス作成時に指定したアダプタを取得します。
	//! @param[out] ppAdapter 取得したアダプタを渡す IV3DAdapter インターフェースのポインタのアドレスです。
	virtual void GetAdapter(IV3DAdapter** ppAdapter) = 0;

	//! @brief デバイスの能力を取得します。
	//! @return デバイスの能力を返します。
	virtual const V3DDeviceCaps& GetCaps() const = 0;

	//! @brief デバイスの統計を取得します。
	//! @return デバイスの統計を返します。
	virtual const V3DDeviceStatistics& GetStatistics() const = 0;

	//! @brief キューファミリーの数を取得します。
	//! @return キューファミリーの数を返します。
	virtual uint32_t GetQueueFamilyCount() const = 0;

	//! @brief キューファミリーの記述を取得します。
	//! @param[in] family キューファミリーのインデックスです。<br>
	//! この値は { 0 <= family < IV3DDevice::GetQueueFamilyCount } の範囲に制限されます。
	//! @return キューファミリの記述を返します。
	virtual const V3DDeviceQueueFamilyDesc& GetQueueFamilyDesc(uint32_t family) const = 0;

	//! @brief キューを取得します。
	//! @param[in] family キューファミリーのインデックスです。
	//! @param[in] queue キューのインデックスです。
	//! @param[out] ppQueue 取得したキューを渡す IV3DQueue インターフェースのポインタのアドレスです。
	virtual void GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue) = 0;

	//! @brief 指定したイメージのフォーマットの記述を取得します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] type イメージのタイプです。
	//! @param[in] tiling イメージのタイリングです。
	//! @param[in] usageFlags 使用法を表す ::V3D_IMAGE_USAGE_FLAG 列挙定数の組み合わせです。
	//! @param[out] pDesc 取得したイメージフォーマットの記述を格納する V3DImageFormatDesc 構造体のポインタです。
	//! @retval V3D_OK イメージフォーマットの記述を取得しました。
	//! @retval V3D_ERROR_NOT_SUPPORTED 指定した条件のイメージはサポートされていません。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! pDesc に nullptr を指定することでフォーマットのサポート状況だけを確認することができます。
	virtual V3D_RESULT GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc) = 0;

	//! @brief バッファーとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format バッファーのフォーマットです。
	//! @param[in] featureFlags 確認する機能を表す ::V3D_BUFFER_FORMAT_FEATURE_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK フォーマットは指定した機能をサポートしています。
	//! @retval V3D_ERROR_NOT_SUPPORTED フォーマットは指定した機能をサポートしていません。
	virtual V3D_RESULT CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;

	//! @brief イメージとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] featureFlags 確認する機能を表す ::V3D_IMAGE_FORMAT_FEATURE_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK フォーマットは指定した機能をサポートしています。
	//! @retval V3D_ERROR_NOT_SUPPORTED フォーマットは指定した機能をサポートしていません。
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief イメージとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] tiling イメージのタイリングです。
	//! @param[in] featureFlags 確認する機能を表す ::V3D_IMAGE_FORMAT_FEATURE_FLAG 列挙定数の組み合わせです。
	//! @retval V3D_OK フォーマットは指定したタイリング、機能をサポートしています。
	//! @retval V3D_ERROR_NOT_SUPPORTED フォーマットは指定したタイリング、機能をサポートしていません。
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const = 0;

	//! @brief コマンドプールーを作成します。
	//! @param[in] desc コマンドプールの記述です。
	//! @param[out] ppCommandPool 作成したコマンドプールを渡す IV3DCommandPool インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK コマンドプールを作成しました。
	//! @retval V3D_ERROR_FAIL コマンドプールの作成に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief コマンドバッファーを作成します。
	//! @param[in] pPool コマンドプールです。
	//! @param[in] bufferType コマンドバッファーのタイプです。
	//! @param[out] ppCommandBuffer 作成したコマンドバッファーを渡す IV3DCommandBuffer インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK コマンドバッファーを作成しました。
	//! @retval V3D_ERROR_FAIL コマンドバッファーの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandBuffer(IV3DCommandPool* pPool, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief コマンドバッファーを作成します。
	//! @param[in] poolDesc コマンドプールの記述です。
	//! @param[in] bufferType コマンドバッファーのタイプです。
	//! @param[out] ppCommandBuffer 作成したコマンドバッファーを渡す IV3DCommandBuffer インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK コマンドバッファーを作成しました。
	//! @retval V3D_ERROR_FAIL コマンドバッファーの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! コマンドプールも同時に作成されます。
	virtual V3D_RESULT CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief スワップチェインを作成します。
	//! @param[in] swapChainDesc スワップチェインの記述です。
	//! @param[in] swapChainCallbacks スワップチェインのコールバックです。
	//! @param[out] ppSwapChain 作成したスワップチェインを渡す IV3DSwapChain インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK スワップチェインを作成しました。
	//! @retval V3D_ERROR_FAIL スワップチェインの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! V3DSwapChainDesc 構造体の imageFormat で指定したフォーマットが見つからなかった場合は、自動でフォーマットを決定します。( ::V3D_FORMAT_UNDEFINED が指定できるということを表します )<br>
	//! IV3DSwapChain::GetDesc で取得できる記述は、作成時に指定したものとは異なる場合があるため、必要に応じて取得しなおしてください。<br>
	//! また IV3DSwapChain::GetImage で取得できるスワップチェイン作成直後のイメージのレイアウトは ::V3D_IMAGE_LAYOUT_UNDEFINED になっています。<br>
	virtual V3D_RESULT CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief イベントを作成します。
	//! @param[out] ppEvent 作成したイベントを渡す IV3DEvent インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK イベントを作成しました。
	//! @retval V3D_ERROR_FAIL イベントの作成に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! 作成直後のイベントは非シグナル状態です。
	virtual V3D_RESULT CreateEvent(IV3DEvent** ppEvent, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief フェンスを作成します。
	//! @param[in] initialState 初期状態を指定します。
	//! - true を指定した場合、フェンスはシグナル状態で作成されます。
	//! - false を指定した場合、フェンスは非シグナル状態で作成されます。
	//! @param[out] ppFence 作成したフェンスを渡す IV3DFence インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK フェンスを作成しました。
	//! @retval V3D_ERROR_FAIL フェンスの作成に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFence(bool initialState, IV3DFence** ppFence, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief セマフォを作成します。
	//! @param[out] ppSemaphore 作成したセマフォを渡す IV3DSemaphore インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK セマフォを作成しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateSemaphore(IV3DSemaphore** ppSemaphore, const wchar_t* pDebugName) = 0;

	//! @brief クエリプールを作成します。
	//! @param[in] desc クエリプールの記述です。
	//! @param[out] ppQueryPool 作成したクエリプールを渡す IV3DQueryPool インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK クエリプールを作成しました。
	//! @retval V3D_ERROR_FAIL クエリプールの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief サンプラーを作成します。
	//! @param[in] desc サンプラーの記述です。
	//! @param[out] ppSampler 作成したサンプラーを渡す IV3DSampler インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK サンプラーを作成しました。
	//! @retval V3D_ERROR_FAIL サンプラーの作成に失敗しました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - 作成することのできるサンプラーは V3DDeviceCaps 構造体の maxSamplerCreateCount の値の数に制限されます。 
	virtual V3D_RESULT CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief シェーダーモジュールを作成します。
	//! @param[in] codeSize シェーダコードのサイズをバイト単位で指定します。
	//! - この値は 4 の整数倍である必要があります。
	//! @param[in] pCode シェーダコードです。
	//! @param[out] ppShaderModule 作成したシェーダーモジュールを渡す IV3DShaderModule インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK シェーダーモジュールを作成しました。
	//! @retval V3D_ERROR_FAIL シェーダーモジュールの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief 指定したメモリ特性をリソースがサポートしているかどうかを確認します。
	//! @param[in] memoryPropertyFlags リソースのメモリ特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	//! @param[in] pResource メモリ特性を確認するリソースを表す IV3DResource インターフェースのポインタです。
	//! @retval V3D_OK リソースは指定したメモリ特性をサポートしています。
	//! @retval V3D_ERROR_NOT_SUPPORTED リソースは指定したメモリ特性をサポートしていません。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const = 0;
	//! @brief 指定したメモリ特性をリソースがサポートしているかどうかを確認します。
	//! @param[in] memoryPropertyFlags リソースのメモリ特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	//! @param[in] resourceCount メモリ特性を確認するリソースの数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] ppResources メモリ特性を確認するリソースの配列です。
	//! - 配列の要素数は resourceCount である必要があります。
	//! @retval V3D_OK 全てのリソースは指定したメモリ特性をサポートしています。
	//! @retval V3D_ERROR_NOT_SUPPORTED 一部または全てのリソースは指定したメモリ特性をサポートしていません。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const = 0;

	//! @brief リソースメモリを確保します。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	//! @param[in] memorySize 確保するメモリのサイズをバイト単位で指定します。
	//! @param[out] ppResourceMemory 確保したリソースメモリを渡す IV3DResourceMemory インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK リソースメモリの確保に成功しました。
	//! @retval V3D_ERROR_FAIL リソースメモリの確保に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! IV3DDevice::CreateBuffer または IV3DDevice::CreateImage によって作成された直後のリソースは、メモリが割り当てられていないため、使用できない状態になっています。<br>
	//! そのため IV3DDevice::AllocateResourceMemory でメモリを確保した後 IV3DDevice::BindResourceMemory でリソースをメモリにバインドする必要があります。<br>
	//! <br>
	//! - 確保することのできるリソースメモリは V3DDeviceCaps::maxResourceMemoryCount 以下に制限されます。
	//! .
	//! - メモリ特性 ( memoryPropertyFlags ) に指定できるフラグの組み合わせは以下のものに限定されます。<br>
	//! <br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT ※<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_CACHED ※<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT | ::V3D_MEMORY_PROPERTY_HOST_CACHED ※<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_CACHED<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT | ::V3D_MEMORY_PROPERTY_HOST_CACHED<br>
	//! <br>
	//! また、これらの組み合わせが必ずしも使えるわけではないため、リソースのメモリを確保する前に IV3DDevice::CheckResourceMemoryProperty で使用可能かどうかを調べてください。
	//! .
	//! - リソースのメモリサイズは IV3DResource::GetResourceDesc で取得できる V3DResourceDesc 構造体のメンバである memorySize で確認することができます。<br>
	//! またこの値はアライメントが適用されているものになります。
	virtual V3D_RESULT AllocateResourceMemory(V3DFlags memoryPropertyFlags, uint64_t memorySize, IV3DResourceMemory** ppResourceMemory, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief リソースをメモリにバインドします。
	//! @param[in] pResourceMemory バインド先のメモリです。
	//! @param[in] pResource バインドするリソースです。
	//! @param[in] memoryOffset バインドするメモリのオフセットをバイト単位で指定します。
	//! @retval V3D_OK リソースをメモリにバインドしました。
	//! @retval V3D_ERROR_FAIL リソースをメモリにバインドできませんでした。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - リソースはメモリにバインドすることによってメモリへのアクセスができるようになります。<br>
	//! また一度メモリにバインドするとリソースが解放されるまでバインドを解除することはできません。
	//! .
	//! - メモリオフセット ( memoryOffset ) はリソースの種類によってアライメントの制限があり、以下のメンバの整数倍の値を指定する必要があります。<br>
	//! <br>
	//! <table class="basic">
	//!   <tr><td>テクセルバッファー</td><td>::V3DDeviceCaps::minTexelBufferOffsetAlignment</td></tr>
	//!   <tr><td>ユニフォームバッファー</td><td>::V3DDeviceCaps::minUniformBufferOffsetAlignment</td></tr>
	//!   <tr><td>ストレージバッファー</td><td>::V3DDeviceCaps::minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! さらにホストからアクセスできるメモリ ::V3D_MEMORY_PROPERTY_HOST_VISIBLE にもアライメントの制限があるため ::V3DDeviceCaps::minMemoryMapAlignment も考慮する必要があります。
	virtual V3D_RESULT BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset) = 0;

	//! @brief リソースのメモリを確保し、そのメモリにバインドします。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	//! @param[in] pResource バインドするリソースです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK リソースのメモリ確保、バインドに成功しました。
	//! @retval V3D_ERROR_FAIL リソースのメモリ確保、またはバインドに失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, IV3DResource* pResource, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief 複数のリソースを一つのメモリとして確保し、そのメモリにバインドします。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す ::V3D_MEMORY_PROPERTY_FLAG 列挙定数の組み合わせです。
	//! @param[in] resourceCount バインドするリソースの数です。
	//! - この値は、1 以上である必要があります。
	//! @param[in] ppResources バインドするリソースを表す IV3DResource インターフェースのポインタの配列です。
	//! - 配列の要素数は resourceCount である必要があります。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK リソースのメモリ確保、バインドに成功しました。
	//! @retval V3D_ERROR_FAIL リソースのメモリ確保、またはバインドに失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief バッファーを作成します。
	//! @param[in] desc バッファーの記述です。
	//! @param[in] ppBuffer 作成したバッファーを渡す IV3DBuffer インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK バッファーを作成しました。
	//! @retval V3D_ERROR_FAIL バッファーの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - バッファーに複数のサブリソースを含める場合は、サブリソースを配置するメモリオフセットのアライメントに注意してバッファーのサイズを決定する必要があります。<br>
	//! サブリソースのアライメントは V3DDeviceCaps に記述されており、以下のようになります。<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>::V3D_BUFFER_USAGE_UNIFORM_TEXEL <br> ::V3D_BUFFER_USAGE_STORAGE_TEXEL</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_BUFFER_USAGE_UNIFORM</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_BUFFER_USAGE_STORAGE</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! また、ホストからアクセスできるメモリ ::V3D_MEMORY_PROPERTY_HOST_VISIBLE にもアライメントの制限があるため V3DDeviceCaps::minMemoryMapAlignment も考慮する必要があります。
	//! @sa IV3DDevice::CheckBufferFormatFeature
	virtual V3D_RESULT CreateBuffer(const V3DBufferDesc& desc, IV3DBuffer** ppBuffer, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief イメージを作成します。
	//! @param[in] imageDesc イメージの記述です。
	//! @param[in] initialLayout イメージの初期レイアウトです。
	//! - イメージの初期レイアウトは ::V3D_IMAGE_LAYOUT_UNDEFINED または ::V3D_IMAGE_LAYOUT_PREINITIALIZED のいずれかである必要があります。
	//! @param[in] pDebugName デバッグ名です。
	//! @param[out] ppImage 作成したイメージを渡す IV3DImage インターフェースのポインタです。
	//! @retval V3D_OK イメージを作成しました。
	//! @retval V3D_ERROR_FAIL イメージの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - イメージの記述である V3DImageDesc 構造体のメンバの値には制限があり、その制限は IV3DDevice::GetImageFormatDesc および IV3DDevice::CheckImageFormatFeature を使用して確認することができます。<br>
	//! また V3DImageDesc 構造体のメンバである tiling に ::V3D_IMAGE_TILING_LINEAR を指定した場合、以下の値に限り実装の制限はありません。<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>imageType</td><td>::V3D_IMAGE_TYPE_2D</td></tr>
	//! <tr><td>format</td><td>デプスステンシル以外のフォーマット</td></tr>
	//! <tr><td>levelCount</td><td>1</td></tr>
	//! <tr><td>layerCount</td><td>1</td></tr>
	//! <tr><td>samples</td><td>::V3D_SAMPLE_COUNT_1</td></tr>
	//! <tr><td>usageFlags</td><td>::V3D_IMAGE_USAGE_TRANSFER_SRC と ::V3D_IMAGE_USAGE_TRANSFER_DST の両方、またはいずれか</td></tr>
	//! </table>
	//! <br>
	virtual V3D_RESULT CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief バッファービューを作成します。
	//! @param[in] pBuffer アクセス先のバッファーです。
	//! @param[in] desc バッファービューの記述です。
	//! @param[out] ppBufferView 作成したバッファービューを表す IV3DBufferView インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK バッファービューを作成しました。
	//! @retval V3D_ERROR_FAIL バッファービューの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - アクセス先のバッファーはリソースメモリにバインドされている必要があります。
	//! .
	//! - シェーダーがイメージとしてアクセスすることのできるデスクリプタ ( ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER または ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER ) として使用したい場合は、バッファービューを作成する必要があります。<br>
	//! また、アクセス先のバッファーの使用法である V3DBufferDesc::usageFlags には ::V3D_BUFFER_USAGE_UNIFORM_TEXEL または ::V3D_BUFFER_USAGE_STORAGE_TEXEL のいずれかが含まれている必要があります。
	virtual V3D_RESULT CreateBufferView(IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, IV3DBufferView** ppBufferView, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief イメージビューを作成します。
	//! @param[in] pImage アクセス先のイメージです。
	//! @param[in] desc イメージビューの記述です。
	//! @param[out] ppImageView 作成したイメージビューを渡す IV3DImageView インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK イメージビューを作成しました。
	//! @retval V3D_ERROR_FAIL イメージビューの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - アクセス先のイメージはリソースメモリにバインドされている必要があります。
	//! .
	//! - シェーダーがアクセスすることのできるデスクリプタとして使用したい場合は、イメージビューを作成する必要があります。
	virtual V3D_RESULT CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief バリアセットを作成します。
	//! @param[in] desc バリアセットの記述です。
	//! @param[out] ppBarrierSet 作成したバリアセットを表す IV3DBarrierSet インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK バリアセットを作成しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	virtual V3D_RESULT CreateBarrierSet(const V3DBarrierSetDesc& desc, IV3DBarrierSet** ppBarrierSet, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief レンダーパスを作成します。
	//! @param[in] attachmentCount アタッチメントの数です。
	//! - この値は 1以上である必要があります。
	//! @param[in] pAttachments アタッチメントの参照配列です。
	//! - 配列の要素数は attachmentCount である必要があります。
	//! @param[in] subpassCount サブパスの数です。
	//! - この値は 1以上である必要があります。
	//! @param[in] pSubpasses サブパスの配列です。
	//! - 配列の要素数は subpassCount である必要があります。
	//! @param[in] subpassDependencyCount サブパスの依存性の数です。
	//! - 以下のいずれかに該当する場合は、1 以上の値を指定する必要があります。
	//!  -# サブパスの数 ( subpassCount ) が 2 以上の場合。
	//!  -# レンダーパス開始前のイメージレイアウトと、レンダーパス開始時のイメージレイアウト ( V3DAttachmentDesc::initialLayout ) が異なる場合。
	//!  -# レンダーパス開始時のイメージレイアウト ( V3DAttachmentDesc::initialLayout ) と、レンダーパス終了時のイメージレイアウト ( V3DAttachmentDesc::finalLayout ) が異なる場合。
	//! @param[in] pSubpassDependencies サブパスの依存性の配列です。
	//! - subpassDependencyCount が 1 以上の場合、要素数が subpassDependencyCount である配列を指定します。
	//! - subpassDependencyCount が 0 の場合は nullptr を指定します。
	//! @param[out] ppRenderPass 作成したレンダーパスを表す IV3DRenderPass インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK レンダーパスを作成しました。
	//! @retval V3D_ERROR_FAIL レンダーパスの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateRenderPass(
		uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments,
		uint32_t subpassCount, const V3DSubpassDesc* pSubpasses,
		uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies,
		IV3DRenderPass** ppRenderPass,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief フレームバッファーを作成します。
	//! @param[in] pRenderPass レンダーパスです。
	//! @param[in] attachmentCount アタッチメントの数です。
	//! - この値は、1以上である必要があります。
	//! @param[in] ppAttachments アタッチメントとして使用するイメージビューの配列です。
	//! - 配列の要素数は attachmentCount である必要があります。
	//! - pRenderPass が参照するアタッチメントのフォーマット、サンプル数は一致している必要があります。
	//! @param[out] ppFrameBuffer 作成したフレームバッファを渡す IV3DFrameBuffer インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK フレームバッファーを作成しました。
	//! @retval V3D_ERROR_FAIL フレームバッファーの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - アタッチメントとして使用する全てのイメージビューは 幅、高さ、深さ、レイヤー数が一致している必要があります。
	//! @sa IV3DDevice::CreateRenderPass
	virtual V3D_RESULT CreateFrameBuffer(IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief デスクリプタセットレイアウトを作成します。
	//! @param[in] descriptorCount デスクリプタの数です。
	//! - この値は、1 以上である必要があります。
	//! @param[in] pDescriptors デスクリプタの配列です。
	//! - 配列の要素数は descriptorCount である必要があります。
	//! @param[in] poolSize デスクリプタセットのプールの初期サイズです。
	//! @param[in] poolResizeStep デスクリプタセットのプールのリサイズステップです。
	//! @param[out] ppDescriptorSetLayout 作成したデスクリプタセットレイアウトを渡す IV3DDescriptorSetLayout インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK デスクリプタセットレイアウトを作成しました。
	//! @retval V3D_ERROR_FAIL デスクリプタセットレイアウトの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED プッシュデスクリプタセットをサポートしていません。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - デスクリプタセットには IV3DCommandBuffer::BindDescriptorSets でバインドするスタンダードデスクリプタセットと、
	//! IV3DCommandBuffer::PushDescriptorSets で更新するプッシュデスクリプタセットの２種類があります。<br>
	//! 前者のスタンダードデスクリプタセットに対応したレイアウトは、poolSize が 1以上、poolResizeStep が 0 または 1以上を指定した場合に作成することができ、<br>
	//! 後者のプッシュデスクリプタセットに対応したレイアウトは、poolSize poolResizeStep の両方に 0 を指定した場合に作成することができます。
	virtual V3D_RESULT CreateDescriptorSetLayout(
		uint32_t descriptorCount, const V3DDescriptorDesc* pDescriptors,
		uint32_t poolSize, uint32_t poolResizeStep,
		IV3DDescriptorSetLayout** ppDescriptorSetLayout,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief デスクリプタセットを作成します。
	//! @param[in] pLayout デスクリプタセットレイアウトです。
	//! @param[out] ppDescriptorSet 作成したデスクリプタセットを渡す IV3DDescriptorSet インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK デスクリプタセットを作成しました。
	//! @retval V3D_ERROR_FAIL デスクリプタセットの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - 作成されるデスクリプタセットは、指定されたデスクリプタセットレイアウトのタイプ ( IV3DDescriptorSetLayout::GetType ) によってコマンドバッファーへの設定方法が異なります。<br>
	//! <br>
	//! <table class="basic">
	//!   <tr><th>タイプ</th><th>設定方法</th></tr>
	//!   <tr><td>スタンダードデスクリプタセット<br> ( ::V3D_DESCRIPTOR_SET_TYPE_STANDARD )</td><td>IV3DCommandBuffer::BindDescriptorSets</td></tr>
	//!   <tr><td>プッシュデスクリプタセット<br> ( ::V3D_DESCRIPTOR_SET_TYPE_PUSH )</td><td>IV3DCommandBuffer::PushDescriptorSets</td></tr>
	//! </table>
	//! <br>
	virtual V3D_RESULT CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief パイプラインレイアウトを作成します。
	//! @param[in] constantCount プッシュ定数の数です。
	//! @param[in] pConstants constantCount プッシュ定数の配列です。<br>
	//! - constantCount が 1 以上の場合、要素数が constantCount の配列を指定する必要があります。
	//! - constantCount が 0 の場合は、nullptr を指定します。
	//! - プッシュ定数のサイズ ( V3DConstantDesc::offset + V3DConstantDesc::size の最大値 ) は V3DDeviceCaps::maxPushConstantsSize 以下に制限されます。
	//! @param[in] descriptorSetLayoutCount デスクリプタセットレイアウトの数です。
	//! @param[in] ppDescriptorSetLayouts デスクリプタレイアウトの配列です。
	//! - descriptorSetLayoutCount が 1 以上の場合、要素数が descriptorSetLayoutCount の配列を指定する必要があります。
	//! - descriptorSetLayoutCount が 0 の場合は、nullptr を指定します。
	//! @param[out] ppPipelineLayout 作成したパイプラインレイアウトを表す IV3DPipelineLayout インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK パイプラインレイアウトを作成しました。
	//! @retval V3D_ERROR_FAIL パイプラインレイアウトの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreatePipelineLayout(
		uint32_t constantCount, V3DConstantDesc* pConstants,
		uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts,
		IV3DPipelineLayout** ppPipelineLayout,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief グラフィックスパイプラインを作成します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] pipelineDesc グラフィックスパイプラインの記述です。
	//! @param[out] ppPipeline 作成したグラフィックスパイプラインを渡す IV3DPipeline インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK グラフィックスパイプラインを作成しました。
	//! @retval V3D_ERROR_FAIL グラフィックスパイプラインの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief コンピュートパイプラインを作成します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] pipelineDesc コンピュートパイプラインの記述です。
	//! @param[out] ppPipeline 作成したコンピュートパイプラインを渡す IV3DPipeline インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK コンピュートパイプラインを作成しました。
	//! @retval V3D_ERROR_FAIL コンピュートパイプラインの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief デバイスがアイドル状態になるまで待機します
	//! @retval V3D_OK デバイスはアイドル状態になりました。
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT WaitIdle() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDevice() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// インスタンス
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_LAYER
//! @brief レイヤー
enum V3D_LAYER : uint8_t
{
	//! @brief 検証レイヤーです。<br>
	//! デバッグ時はこのレイヤーを使用します。<br>
	//! <br>
	//! 使用される Vulkan のレイヤー<br>
	//! VK_LAYER_LUNARG_standard_validation
	V3D_LAYER_VALIDATION = 0,
	//! @brief 最適なレイヤーです。<br>
	//! リリース時はこのレイヤーを使用します。<br>
	//! <br>
	//! 使用される Vulkan のレイヤー<br>
	//! N/A
	V3D_LAYER_OPTIMAL = 1,
	//! @brief Nsight のレイヤーです。<br>
	//! Nsight を使用してモニタリングする場合はこのレイヤーを使用します。<br>
	//! <br>
	//! 使用される Vulkan のレイヤー<br>
	//! N/A
	V3D_LAYER_NSIGHT = 2,
	//! @brief RenderDoc のレイヤーです。<br>
	//! RenderDoc を使用してキャプチャリングする場合はこのレイヤーを使用します。<br>
	//! <br>
	//! 使用される Vulkan のレイヤー<br>
	//! VK_LAYER_RENDERDOC_Capture
	//! @warning
	//! RenderDoc の現在の実装では、使用できるキューが一つだけになりますので注意してください。
	V3D_LAYER_RENDERDOC = 3,
};

//! @enum V3D_LOG_FLAG
//! @brief ログのフラグ
enum V3D_LOG_FLAG : V3DFlags
{
	V3D_LOG_INFORMATION = 0x00000001, //!< 情報です。
	V3D_LOG_WARNING = 0x00000002, //!< 警告です。
	V3D_LOG_PERFORMANCE_WARNING = 0x00000004, //!< パフォーマンスに関する警告です。
	V3D_LOG_ERROR = 0x00000008, //!< エラーです。
	V3D_LOG_DEBUG = 0x00000010, //!< デバッグです。

	V3D_LOG_STANDARD = V3D_LOG_WARNING | V3D_LOG_ERROR, //!< 標準のログを表示します。
	V3D_LOG_ALL = V3D_LOG_INFORMATION | V3D_LOG_WARNING | V3D_LOG_PERFORMANCE_WARNING | V3D_LOG_ERROR | V3D_LOG_DEBUG, //!< すべてのログを表示します。
};

//! @}

//! @addtogroup v3d_callback_group
//! @{

//! @fn void PV3DAllocateMemoryFunction(size_t size, size_t alignment, void* pUserData)
//! @brief メモリの確保をするコールバック関数の定義です。
//! @param[in] size 確保するメモリのサイズです。
//! @param[in] alignment 確保するメモリのアラインメントです。
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData です。
//! @return 確保したメモリのポインタを返します。
typedef void* (V3D_CALLBACK *PV3DAllocateMemoryFunction)(size_t size, size_t alignment, void* pUserData);
//! @fn void PV3DReallocateMemoryFunction(void* pMemory, size_t size, size_t alignment, void* pUserData)
//! @brief メモリの確保をするコールバック関数の定義です。
//! @param[in] pMemory 再確保する元のメモリのポインタです。
//! @param[in] size 確保するメモリのサイズです。
//! @param[in] alignment 確保するメモリのアラインメントです。
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData です。
//! @return 再確保したメモリのポインタを返します。
typedef void* (V3D_CALLBACK *PV3DReallocateMemoryFunction)(void* pMemory, size_t size, size_t alignment, void* pUserData);
//! @fn void PV3DFreeMemoryFunction(void* pMemory, void* pUserData)
//! @brief メモリの確保をするコールバック関数の定義です。
//! @param[in] pMemory 解放するメモリのポインタです。
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData です。
typedef void (V3D_CALLBACK *PV3DFreeMemoryFunction)(void* pMemory, void* pUserData);

//! @fn void PV3DLogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData)
//! @brief メモリの確保をする関数の定義です、
//! @param[in] type ログのタイプです。
//! @param[in] pMessage ログのメッセージです。
//! @param[in] pUserData V3DInstanceLogDesc::pUserData です。
typedef void (V3D_CALLBACK *PV3DLogFunction)(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData);

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DInstanceMemoryDesc
//! @brief インスタンスのメモリの記述
struct V3DInstanceMemoryDesc
{
	PV3DAllocateMemoryFunction pAllocateFunction; //!< メモリを確保する関数のポインタです。
	PV3DReallocateMemoryFunction pReallocateFunction; //!< メモリを再確保する関数のポインタです。
	PV3DFreeMemoryFunction pFreeFunction; //!< メモリを解放する関数のポインタです。
	void* pUserData; //!< コールバック関数に渡されるユーザーデータです。
};

//! @struct V3DInstanceLogDesc
//! @brief インスタンスのログの記述
struct V3DInstanceLogDesc
{
	V3DFlags flags; //!< ログのフラグを表す ::V3D_LOG_FLAG 列挙定数の組み合わせです。
	PV3DLogFunction pFunction; //!< ログを出力する関数のポインタです。
	void* pUserData; //!< コールバック関数に渡されるユーザーデータです。
};

//! @struct V3DInstanceDesc
//! @brief インスタンスの記述
struct V3DInstanceDesc
{
	V3D_LAYER layer; //!< レイヤーを表す ::V3D_LAYER 列挙定数のいずれかを指定します。
	V3DInstanceMemoryDesc memory; //!< メモリの記述です。
	V3DInstanceLogDesc log; //!< ログの記述です。
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DInstance
//! @brief インスタンスインターフェース
//! @sa V3DCreateInstance
class V3D_DLL_API IV3DInstance : public IV3DObject
{
public:
	//! @brief レイヤーを取得します。
	//! @return レイヤーを返します。
	virtual V3D_LAYER GetLayer() const = 0;

	//! @brief アダプタの数を取得します。
	//! @return アダプタの数を返します。
	virtual uint32_t GetAdapterCount() const = 0;
	//! @brief アダプタを取得します。
	//! @param[in] adapterIndex アダプタのインデックスです。<br>
	//! この値は { 0 <= adapterIndex < IV3DInstance::GetAdapterCount } の範囲に制限されます。
	//! @param[out] ppAdapter 取得したアダプタを渡す IV3DAdapter インターフェースのポインタのアドレスです。
	virtual void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) = 0;

	//! @brief デバイスを作成します。
	//! @param[in] pAdapter アダプタです。
	//! @param[out] ppDevice 作成したデバイスを渡す IV3DDevice インターフェースのポインタのアドレスです。
	//! @param[in] pDebugName デバッグ名です。
	//! @retval V3D_OK デバイスを作成しました。
	//! @retval V3D_ERROR_FAIL デバイスの作成に失敗しました。
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice, const wchar_t* pDebugName = nullptr) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DInstance() {}
	//! @endcond
};

//! @}

//! @addtogroup v3d_func_group
//! @{

//! @brief 指定したレイヤーがサポートされているかどうかを確認します。
//! @param[in] layer 確認するレイヤーです。
//! @retval V3D_OK 指定したレイヤーはサポートしています。
//! @retval V3D_ERROR_FAIL 指定したレイヤーはサポートされていません。
//! @retval V3D_ERROR_NOT_SUPPORTED
V3D_DLL_API V3D_RESULT V3DCheckLayer(V3D_LAYER layer);

//! @brief インスタンスを作成します。
//! @param[in] instanceDesc インスタンスの記述を表す V3DInstanceDesc 構造体です。
//! @param[out] ppInstance 作成したインスタンスを渡す IV3DInstance インターフェースのポインタのアドレスです。
//! @retval V3D_OK インスタンスを作成しました。
//! @retval V3D_ERROR_FAIL インスタンスの作成に失敗しました。
//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
V3D_DLL_API V3D_RESULT V3DCreateInstance(const V3DInstanceDesc& instanceDesc, IV3DInstance** ppInstance);

//! @}
