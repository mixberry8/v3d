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

//! @def V3D_CALLBACK
//! @brief v3d のコールバック定義
#define V3D_CALLBACK __stdcall

//! @def V3D_INFINITE
//! @brief オブジェクトがシグナル状態になるのを待機する時間です。
#define V3D_INFINITE UINT64_MAX

//! @def V3D_WHOLE_SIZE
//! @brief リソースのメモリの終端までをマップすることを表します。
#define V3D_WHOLE_SIZE (~0ULL)

//! @def V3D_QUEUE_FAMILY_IGNORED
//! @brief キューファミリーのリソースの所有権の変更がないことを表します。
#define V3D_QUEUE_FAMILY_IGNORED (~0U)

//! @def V3D_ATTACHMENT_UNUSED
//! @brief 使用されていないアタッチメントを表します。
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
	V3D_ERROR_INVALID_ARGUMENT = -2, //!< 無効な引数が渡されました。
	V3D_ERROR_NOT_FOUND = -3, //!< 見つかりません。
	V3D_ERROR_NOT_SUPPORTED = -4, //!< サポートされていません。
	V3D_ERROR_OUT_OF_HOST_MEMORY = -5, //!< ホストメモリが不足しています。
	V3D_ERROR_OUT_OF_DEVICE_MEMORY = -6, //!< デバイスメモリが不足しています。
	V3D_ERROR_DEVICE_LOST = -7, //!< デバイスがロストしています。
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
	V3D_FORMAT_D16_UNORM = 124, //!< 1 成分、16 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_X8_D24_UNORM = 125, //!< 2 成分、32 ビット符号なし正規化フォーマットです。
	V3D_FORMAT_D32_SFLOAT = 126, //!< 1 成分、32 ビット符号付き浮動小数点フォーマットです。
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
	//! @brief デバイスからアクセスできるメモリを使用します。
	V3D_MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001,
	//! @brief ホストからアクセスできるメモリを使用します。
	V3D_MEMORY_PROPERTY_HOST_VISIBLE = 0x00000002,
	//! @brief ホストから書き込んだ内容をフラッシュする必要はありません。<br>
	//! このフラグを指定した場合は IV3DResource::Map および IV3DResource::Unmap 時にデバイスまたはホストからのメモリの可視、不可視の変更処理を行わないため、速度の向上が見込まれます。<br>
	V3D_MEMORY_PROPERTY_HOST_COHERENT = 0x00000004,
	//! @brief メモリの内容をホストでキャッシュします。<br>
	//! ホストで内容をキャッシュすることにメモリへのアクセス速度の向上が見込まれます。
	V3D_MEMORY_PROPERTY_HOST_CACHED = 0x00000008,
	//! @brief メモリが遅延して割り当てられる場合があります。<br>
	//! このフラグは カラーアタッチメント、デプスステンシルアタッチメント、インプットアタッチメントに対して有効です。<br>
	//! また、イメージの使用方法には \link V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT \endlink を指定してください。
	//! @sa IV3DDevice::CreateImage
	V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED = 0x00000010,
};

//! @enum V3D_PIPELINE_STAGE_FLAG
//! @brief パイプラインステージフラグ
enum V3D_PIPELINE_STAGE_FLAG : V3DFlags
{
	//! @brief 最初のコマンドがキューによって受信されるパイプラインのステージです。
	V3D_PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
	//! @brief バーテックスバッファとインデックスバッファが消費されるパイプラインのステージです。
	V3D_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
	//! @brief バーテックスシェーダステージです。
	V3D_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
	//! @brief テッセレーション制御シェーダステージです。
	V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
	//! @brief テッセレーション評価シェーダステージです。
	V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
	//! @brief ジオメトリシェーダステージです。
	V3D_PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
	//! @brief フラグメントシェーダステージです。
	V3D_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
	//! @brief フラグメントシェーディング前のデプステストとステンシルテストが実行されるパイプラインのステージです。<br>
	//! サブパスのデプスステンシルアタッチメントのロード処理が含まれます。
	V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
	//! @brief フラグメントシェーディング後のデプステストとステンシルテストが実行されるパイプラインのステージです。<br>
	//! サブパスのデプスステンシルアタッチメントのストア処理が含まれます。
	V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
	//! @brief ブレンドされたカラーが出力されるパイプラインのステージです。<br>
	//! サブパスのカラーアタッチメントのロード、ストア、リゾルブ ( マルチサンプル ) が含まれます。
	V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	//! @brief コンピュートシェーダーステージです。
	V3D_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
	//! @brief コピー操作をするステージです。<br>
	//! バッファー、イメージのコピーなどが含まれます。
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_PIPELINE_STAGE_TRANSFER = 0x00001000,
	//! @brief すべてのコマンドによって生成されたオペレーションが実行を完了するパイプラインの最終ステージです。
	//! @note 次のアクセスが別のキューまたはプレゼンテーション ( IV3DQueue::Present ) で行われる場合の移行に使用します。
	V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,
	//! @brief デバイスメモリの読み書きのホスト上での実行を示す擬似ステージです。
	V3D_PIPELINE_STAGE_HOST = 0x00004000,
	//! @brief すべてのグラフィックパイプラインステージです。<br>
	//! <br>
	//! \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink<br>
	//! \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink<br>
	V3D_PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000,
	//! @brief すべてのコマンドのステージです。
	V3D_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
};

//! @enum V3D_ACCESS_FLAG
//! @brief アクセスフラグ
enum V3D_ACCESS_FLAG : V3DFlags
{
	//! @brief インデックスバッファへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_INDEX \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink
	//! @sa IV3DCommandBuffer::BindIndexBufferView
	V3D_ACCESS_INDEX_READ = 0x00000002,
	//! @brief バーテックスバッファへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_VERTEX \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink
	//! @sa IV3DCommandBuffer::BindVertexBufferViews
	V3D_ACCESS_VERTEX_READ = 0x00000004,
	//! @brief ユニフォームバッファへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM  \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	//! @sa IV3DDescriptorSet::SetBufferView
	V3D_ACCESS_UNIFORM_READ = 0x00000008,
	//! @brief インプットアタッチメントへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink
	//! @sa IV3DDevice::CreateRenderPass
	V3D_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
	//! @brief ストレージバッファ、ユニフォームテクセルバッファ、ストレージテクセルバッファ、サンプリングイメージ、ストレージイメージへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_SAMPLED \endlink<br>
	//! \link V3D_IMAGE_USAGE_STORAGE \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	V3D_ACCESS_SHADER_READ = 0x00000020,
	//! @brief ストレージバッファ、ストレージテクセルバッファ、ストレージイメージへの書き込みアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_STORAGE \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	V3D_ACCESS_SHADER_WRITE = 0x00000040,
	//! @brief ブレンディング、ロジック、サブパスのロードなどのカラーアタッチメントへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink
	V3D_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
	//! @brief レンダリングパス中のサブパスのロード後からストアまでの処理などのカラーアタッチメントへの書き込みアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink
	V3D_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
	//! @brief デプス、ステンシルの操作、サブパスのロードなどのデプスステンシルアタッチメントへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
	//! @brief レンダリングパス中のサブパスのロード後からストアまでの処理などのデプスステンシルアタッチメントへの書き込みアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
	//! @brief コピーコマンドでのバッファまたはイメージへの読み取りアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_TRANSFER_SRC \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_TRANSFER \endlink
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_ACCESS_TRANSFER_READ = 0x00000800,
	//! @brief コピーコマンドでのバッファまたはイメージへの書き込みアクセスです。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! \link V3D_BUFFER_USAGE_TRANSFER_DST \endlink<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! \link V3D_IMAGE_USAGE_TRANSFER_DST \endlink<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_TRANSFER \endlink
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_ACCESS_TRANSFER_WRITE = 0x00001000,
	//! @brief ホストからの読み取りアクセスです。<br>
	//! メモリの使用方法に \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink が指定されている必要があります。<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_HOST \endlink
	V3D_ACCESS_HOST_READ = 0x00002000,
	//! @brief ホストからの書き込みアクセスです。<br>
	//! メモリの使用方法に \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink が指定されている必要があります。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! \link V3D_PIPELINE_STAGE_HOST \endlink
	V3D_ACCESS_HOST_WRITE = 0x00004000,
	//! @brief 特定のリソースへの読み取りアクセスです。<br>
	//! スワップチェインのイメージなどがこの特定のリソースに含まれます。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! N/A
	V3D_ACCESS_MEMORY_READ = 0x00008000,
	//! @brief 特定のリソースへの書き込みアクセスです。<br>
	//! スワップチェインのイメージなどがこの特定のリソースに含まれます。<br>
	//! <br>
	//! サポートされているバッファーの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているイメージの使用方法<br>
	//! N/A<br>
	//! <br>
	//! サポートされているステージ<br>
	//! N/A
	V3D_ACCESS_MEMORY_WRITE = 0x00010000,
};

//! @enum V3D_DEPENDENCY_FLAG
//! @brief 依存性フラグ
enum V3D_DEPENDENCY_FLAG : V3DFlags
{
	V3D_DEPENDENCY_BY_REGION = 0x00000001, //!< 依存関係がフレームバッファのローカルであることを表します。
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
	V3D_IMAGE_LAYOUT_UNDEFINED = 0,
	//! @brief すべてのアクセスをサポートします。<br>
	V3D_IMAGE_LAYOUT_GENERAL = 1,
	//! @brief カラーアタッチメント、リゾルブアタッチメントに対して有効なレイアウトです。<br>
	//! このレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink が指定されている必要があります。
	V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT = 2,
	//! @brief デプスステンシルアタッチメントに対して有効なレイアウトです。<br>
	//! このレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink が指定されている必要があります。
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT = 3,
	//! @brief シェーダーから読み取ることのできるデプスステンシルイメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink と
	//! \link V3D_IMAGE_USAGE_SAMPLED \endlink もしくわ \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink のいずれかが指定されている必要があります。
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY = 4,
	//! @brief シェーダーからの読み取り専用イメージに対して有効なレイアウトです。<br>
	//! これはサンプリングすることのできるイメージ、インプットアタッチメントとして使用することができます。<br>
	//! またこのレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_SAMPLED \endlink または \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink のいずれかが指定されている必要があります。
	V3D_IMAGE_LAYOUT_SHADER_READ_ONLY = 5,
	//! @brief コピーコマンドのコピー元イメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink が指定されている必要があります。
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_LAYOUT_TRANSFER_SRC = 6,
	//! @brief コピーコマンドのコピー先イメージに対して有効なレイアウトです。<br>
	//! このレイアウトを適用できるイメージの使用法に \link V3D_IMAGE_USAGE_TRANSFER_DST \endlink が指定されている必要があります。
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_LAYOUT_TRANSFER_DST = 7,
	//! @brief デバイスのアクセスをサポートしません<br>
	//! イメージ作成時の初期レイアウトとして使用することができます。<br>
	//! このレイアウトはイメージ作成後、すぐにメモリへの書き込みができます。<br>
	//! また異なるレイアウトへ移行するとき、メモリの内容は保持されます。
	V3D_IMAGE_LAYOUT_PREINITIALIZED = 8,
	//! @brief スワップチェインのイメージに対して有効なレイアウトです。<br>
	V3D_IMAGE_LAYOUT_PRESENT_SRC = 9,
};

//! @enum V3D_SAMPLE_COUNT_FLAG
//! @brief サンプルカウントのフラグ
enum V3D_SAMPLE_COUNT_FLAG
{
	V3D_SAMPLE_COUNT_1 = 0x00000001, //!< 1 サンプル
	V3D_SAMPLE_COUNT_2 = 0x00000002, //!< 2 サンプル
	V3D_SAMPLE_COUNT_4 = 0x00000004, //!< 4 サンプル
	V3D_SAMPLE_COUNT_8 = 0x00000008, //!< 8 サンプル
	V3D_SAMPLE_COUNT_16 = 0x00000010, //!< 16 サンプル
	V3D_SAMPLE_COUNT_32 = 0x00000020, //!< 32 サンプル
	V3D_SAMPLE_COUNT_64 = 0x00000040, //!< 64 サンプル
};

//! @enum V3D_COMPARE_OP
//! @brief 比較オペレーター
//! @sa V3DSamplerDesc::compareOp
//! @sa V3DPipelineStencilOpDesc::compareOp
//! @sa V3DPipelineDepthStencilDesc::depthCompareOp
enum V3D_COMPARE_OP
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
enum V3D_INDEX_TYPE
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

//! @union V3DPoint3D
//! @brief 符号無し整数の x y z 座標
struct V3DPoint3D
{
	uint32_t x; //!< X 座標です。
	uint32_t y; //!< Y 座標です。
	uint32_t z; //!< Z 座標です。
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
//! @note 深度は minDepth < maxDepth である必要があります。
struct V3DViewport
{
	V3DRectangle2D rect; //!< ビューポートの領域です。
	float minDepth; //!< 最小深度です。 ( 0.0f～1.0f )
	float maxDepth; //!< 最大深度です。 ( 0.0f～1.0f )
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
	float depth; //!< 深度です。 ( 0.0f～1.0f )
	uint32_t stencil; //!< ステンシルです。
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

	//! @brief フェンスを非シグナル状態にします。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset() = 0;
	//! @brief フェンスがシグナル状態になるまで待機します。
	//! @param[in] timeOut タイムアウト時間をナノ秒単位で指定します。
	//! @retval V3D_OK
	//! @retval V3D_TIMEOUT
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Wait(uint64_t timeOut = V3D_INFINITE) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DFence() {}
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
enum V3D_QUERY_TYPE
{
	V3D_QUERY_TYPE_OCCLUSION = 0, //!< オクルージョンクエリです。
	V3D_QUERY_TYPE_PIPELINE_STATISTICS = 1, //!< パイプラインの統計クエリです。
	V3D_QUERY_TYPE_TIMESTAMP = 2, //!< タイムスタンプクエリです。
};

//! @enum V3D_QUERY_RESULT_FLAG
//! @brief クエリの結果フラグ
enum V3D_QUERY_RESULT_FLAG
{
	V3D_QUERY_RESULT_WAIT = 0x00000001, //!< 指定したすべてのクエリの結果が取得できるまで待機します。
	V3D_QUERY_RESULT_WITH_AVAILABILITY = 0x00000002, //!< 結果が出ているクエリがあるかどうかを確認できるようにします。
	V3D_QUERY_RESULT_PARTIAL = 0x00000004, //!< 指定したすべてクエリでなくても結果を取得できるようにします。
};

//! @enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
//! @brief クエリのパイプラインの統計タイプ
enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
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
	//! @param[in] queryResultFlags クエリの結果の取得方法を表す \link V3D_QUERY_RESULT_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK クエリの結果を取得しました。
	//! @retval V3D_NOT_READY クエリの結果はまだ取得できていません。
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	//! @note
	//! キューを実行 ( IV3DCommandBuffer::ExecuteCommandBuffers ) した後に使用してください。<br>
	//! 返り値が V3D_OK でなくてもクエリの結果を取得しようとした後は IV3DCommandBuffer::ResetQueryPool でクエリプールをリセットしてください。<br>
	//! <br>
	//! クエリのタイプが \link V3D_QUERY_TYPE_OCCLUSION \endlink もしくわ \link V3D_QUERY_TYPE_TIMESTAMP \endlink であった場合は queryResultCount の値は queryCount になります。<br>
	//! クエリのタイプが \link V3D_QUERY_TYPE_PIPELINE_STATISTICS \endlink であった場合は queryResultCount の値は queryCount * 11 になり、それぞれの 0 ～ 10 要素の値の意味は \link V3D_QUERY_PIPELINE_STATISTIC_TYPE \endlink 列挙型を参照してください。<br>
	//! クエリのタイプが \link V3D_QUERY_TYPE_TIMESTAMP \endlink であった場合は pQueryResults に書き込まれる数値の単位はナノ秒になります。<br>
	//! <br>
	//! flags に \link V3D_QUERY_RESULT_WAIT \endlink を指定した場合は、クエリの結果が取得できるまで待機します。<br>
	//! flags に \link V3D_QUERY_RESULT_PARTIAL \endlink を指定した場合は、指定したすべてのクエリでなく、部分的なクエリの途中の結果を取得できるようになります。<br>
	//! ただし、クエリのタイプが \link V3D_QUERY_TYPE_TIMESTAMP \endlink であった場合、このフラグを指定することはできません。<br>
	//! flags に \link V3D_QUERY_RESULT_WITH_AVAILABILITY \endlink を指定した場合は、queryResultCount の値は queryCount + 1 をを必要とし、
	//! 最後の要素 ( pQueryResults[queryResultCount - 1] ) にクエリの結果が取得できた場合は 1 以上の値、取得できなかった場合は 0 が書き込まれます。
	//! @sa V3D_QUERY_TYPE
	//! @sa V3D_QUERY_RESULT_FLAG
	//! @sa V3D_QUERY_PIPELINE_STATISTIC_TYPE
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
	//! @brief メモリのプロパティフラグを表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
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

	//! @brief リソースメモリのマップを開始することを通知します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! このメソッドはメモリ全体のアクセスを有効な状態にします。<br>
	//! 一度に多くのリソースがこのメモリをアクセスする際 ( IV3DResource::Map および IV3DResource::Unmap ) のパフォーマンスを向上させることができます。
	virtual V3D_RESULT BeginMap() = 0;
	//! @brief リソースメモリのマップを終了することを通知します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
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
enum V3D_RESOURCE_TYPE
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

	//! @brief メモリのプロパティフラグを表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags memoryPropertyFlags;
	//! @brief メモリタイプのビットマスクです。
	//! @note 各ビットは IV3DAdapter::GetMemoryTypeDesc の第一引数である memoryType を表します。
	uint32_t memoryTypeBits;
	//! @brief メモリのサイズをバイト単位で指定します。
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
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) = 0;

	//! @brief リソースの指定されたメモリのポインタ有効にし、取得します。
	//! @param[in] offset 有効にするメモリのオフセットをバイト単位で指定します。
	//! @param[in] size 有効にするメモリのサイズをバイト単位で指定します。
	//! @param[out] ppMemory 取得したメモリのポインタを渡す void ポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note size に V3D_WHOLE_SIZE を指定することで offset からメモリの終端までをマップします。
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) = 0;
	//! @brief リソースの指定されたメモリのポインタを無効にします。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
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

//! @enum V3D_BUFFER_USAGE
//! @brief バッファーの使用方法のフラグ
enum V3D_BUFFER_USAGE : V3DFlags
{
	//! @brief コピー元バッファとして使用します。
	//! @sa IV3DCommandBuffer::CopyBuffer
	V3D_BUFFER_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief コピー先バッファとして使用します。
	//! @sa IV3DCommandBuffer::CopyBuffer
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
struct V3DBufferDesc
{
	V3DFlags usageFlags; //!< バッファーの使用法を表す \link V3D_BUFFER_USAGE \endlink 列挙定数の組み合わせです。
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
enum V3D_IMAGE_TYPE
{
	V3D_IMAGE_TYPE_1D = 0, //!< 1D ( width ) イメージです。
	V3D_IMAGE_TYPE_2D = 1, //!< 2D ( width height ) イメージです。
	V3D_IMAGE_TYPE_3D = 2, //!< 3D ( width height depth )イメージです。
};

//! @enum V3D_IMAGE_TILING
//! @brief イメージのタイリング
enum V3D_IMAGE_TILING
{
	//! @brief 最適なタイリングを指定します。<br>
	//! テクセルの配置は実装に依存することになるため、サブリソースにアクセスすることはできません。
	V3D_IMAGE_TILING_OPTIMAL = 0,
	//! @brief 線形タイリングを使用します。<br>
	//! テクセルは定められた順序で配置されるため、サブリースへのアクセスを可能にします。
	//! @sa IV3DImage::GetSubresourceLayout
	V3D_IMAGE_TILING_LINEAR = 1,
};

//! @enum V3D_IMAGE_USAGE
//! @brief イメージの使用方法を表すフラグ
enum V3D_IMAGE_USAGE : V3DFlags
{
	//! @brief コピー元イメージとして使用します。
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief コピー先イメージとして使用します。
	//! @sa IV3DCommandBuffer::ClearImage
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief シェーダーからサンプリングすることができるイメージとして使用します。
	//! @sa IV3DDescriptorSet::SetImageView
	//! @sa IV3DDescriptorSet::SetImageViewAndSampler
	V3D_IMAGE_USAGE_SAMPLED = 0x00000004,
	//! @brief キューブイメージとして使用します。
	V3D_IMAGE_USAGE_CUBE_COMPATIBLE = 0x00000008,
	//! @brief シェーダーから書き込むことのできるストレージイメージとして使用します。
	V3D_IMAGE_USAGE_STORAGE = 0x00000010,
	//! @brief フレームバッファのカラーアタッチメントとして使用します。
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000020,
	//! @brief フレームバッファのデプスステンシルとして使用します。
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000040,
	//! @brief イメージのメモリプロパティに \link V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED \endlink を指定されていることを表します。<br>
	//! また以下の使用法を用いて作成されたイメージに最適です。<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink<br>
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT = 0x0000080,
	//! @brief フレームバッファのインプットアタッチメントとして使用します。<br>
	//! このイメージはフラグメントシェーダーのみが読み取ることができます。
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_INPUT_ATTACHMENT = 0x00000100,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageDesc
//! @brief イメージの記述
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
	V3DFlags usageFlags; //!< イメージの使用方法を表す \link V3D_IMAGE_USAGE \endlink 列挙定数の組み合わせです。
};

//! @struct V3DImageSubresourceLayout
//! @brief イメージのサブリソースレイアウト
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
	//! @note
	//! サブリソースのレイアウトを取得できるようにするためには、イメージのタイリングに \link V3D_IMAGE_TILING_LINEAR \endlink が指定されている必要があります。
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
enum V3D_IMAGE_VIEW_TYPE
{
	V3D_IMAGE_VIEW_TYPE_1D = 0, //!< 1D イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_2D = 1, //!< 2D イメージビューとして使用します。
	V3D_IMAGE_VIEW_TYPE_3D = 2, //!< 3D イメージビューとして使用します。
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
enum V3D_FILTER
{
	//! @brief 最も近い座標のテクセルを使用します。
	V3D_FILTER_NEAREST = 0,
	//! @brief イメージの次元数に応じたテクセルの加重平均を計算します。<br>
	//! <table>
	//! <tr><td>1Dイメージ</td><td>2x1</td></tr>
	//! <tr><td>2Dイメージ<br>キューブイメージ</td><td>2x2</td></tr>
	//! <tr><td>3Dイメージ</td><td>2x2x2</td></tr>
	//! </table>
	V3D_FILTER_LINEAR = 1,
};

//! @enum V3D_MIPMAP_MODE
//! @brief ミップマップモード
enum V3D_MIPMAP_MODE
{
	//! @brief 最も近い座標のテクセルを使用します。
	V3D_MIPMAP_MODE_NEAREST = 0,
	//! @brief イメージの次元数に応じたテクセルの加重平均を計算します。
	V3D_MIPMAP_MODE_LINEAR = 1,
};

//! @enum V3D_ADDRESS_MODE
//! @brief アドレスモード
enum V3D_ADDRESS_MODE
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
enum V3D_BORDER_COLOR
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
//! @note
//! デフォルト値<br>
//! <table>
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
//! <br>
struct V3DSamplerDesc
{
	V3D_FILTER magFilter; //!< 拡大フィルタです。
	V3D_FILTER minFilter; //!< 縮小フィルタです。
	V3D_MIPMAP_MODE mipmapMode; //!< ミップマップモード
	V3D_ADDRESS_MODE addressModeU; //!< U 座標のアドレッシングモードです。
	V3D_ADDRESS_MODE addressModeV; //!< V 座標のアドレッシングモードです。
	V3D_ADDRESS_MODE addressModeW; //!< W 座標のアドレッシングモードです。
	float mipLodBias; //!< 計算されたミップマップレベルからのオフセットです。
	bool anisotropyEnable; //!< 異方性フィルタリングを有効にするかどうかを指定します。
	float maxAnisotropy; //!< 異方性値クランプ値です。
	bool compareEnable; //!< 比較オペーレーターである compareOp を有効にするかどうかを指定します。

	//! @brief 比較オペレーターです。<br>
	//! \link V3D_COMPARE_OP \endlink の説明にある R はサンプラーによって入力される値であり、S はアタッチメントのテクセルの値を表します。
	V3D_COMPARE_OP compareOp;

	float minLod; //!< 計算されたミップレベルをクランプする最小値です。<br>通常この値は最初のミップマップを指定します。
	float maxLod; //!< 計算されたミップレベルをクランプする最大値です。<br>通常この値はミップマップの数を指定します。
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
//! @brief レンダリングパスのロードオペレーター
enum V3D_ATTACHMENT_LOAD_OP
{
	//! @brief レンダリングパスの開始時にアタッチメントの以前の内容が保持されている必要がないことを表します。<br>
	//! またアタッチメントのアクセスには、\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink もしくわ \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_UNDEFINED = 0,
	//! @brief レンダリングパスの開始時にアタッチメントの以前の内容を読み込みます。<br>
	//! またアタッチメントのアクセスには、\link V3D_ACCESS_COLOR_ATTACHMENT_READ \endlink もしくわ \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ \endlink が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_LOAD = 1,
	//! @brief レンダリングパスの開始時にアタッチメントをクリアします。
	//! またアタッチメントのアクセスには、\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink もしくわ \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink が含まれている必要があります。
	V3D_ATTACHMENT_LOAD_OP_CLEAR = 2,
};

//! @enum V3D_ATTACHMENT_STORE_OP
//! @brief レンダリングパスのストアオペレーター
enum V3D_ATTACHMENT_STORE_OP
{
	//! @brief レンダリングパスの終了時にアタッチメントの内容が保存されず、破棄される可能性があることを表します。<br>
	//! またアタッチメントのアクセスには、\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink もしくわ \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink が含まれている必要があります。
	V3D_ATTACHMENT_STORE_OP_UNDEFINED = 0,
	//! @brief レンダリングパスの終了時にアタッチメントの内容を保存します。<br>
	//! またアタッチメントのアクセスには、\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink もしくわ \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink が含まれている必要があります。
	V3D_ATTACHMENT_STORE_OP_STORE = 1,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAttachmentDesc
//! @brief アタッチメントの記述
struct V3DAttachmentDesc
{
	//! @brief イメージのフォーマットです。<br>
	//! @sa V3DImageDesc::format
	V3D_FORMAT format;
	//! @brief イメージのサンプル数です。<br>
	//! @sa V3DImageDesc::samples
	V3D_SAMPLE_COUNT_FLAG samples;
	//! @brief レンダリングパスのロードオペレーターです。
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_ATTACHMENT_LOAD_OP loadOp;
	//! @brief レンダリングパスのストアオペレーターです。
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_ATTACHMENT_STORE_OP storeOp;
	//! @brief レンダリングパスのステンシルのロードオペレーターです。
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_ATTACHMENT_LOAD_OP stencilLoadOp;
	//! @brief レンダリングパスのステンシルのストアオペレーターです。
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_ATTACHMENT_STORE_OP stencilStoreOp;
	//! @brief レンダリングパス開始時のレイアウトです。<br>
	//! レンダリングパス開始前にこのレイアウトへ移行しておく必要があります。
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_IMAGE_LAYOUT initialLayout;
	//! @brief レンダリングパス終了時のレイアウトです。
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_IMAGE_LAYOUT finalLayout;
	//! @brief レンダリングパス開始時のアタッチメントのクリア値です。<br>
	//! この値は loadOp または stencilLoadOp が \link V3D_ATTACHMENT_LOAD_OP_CLEAR \endlink のときに有効です。
	V3DClearValue clearValue;
};

//! @struct V3DAttachmentReference
//! @brief アタッチメントの参照
struct V3DAttachmentReference
{
	uint32_t attachment; //!< IV3DDevice::CreateRenderPass で指定した V3DAttachmentDesc 構造体の配列のインデックスです。
	V3D_IMAGE_LAYOUT layout; //!< サブパス開始時のアタッチメントのレイアウトです。
};

//! @struct V3DSubpassDesc
//! @brief サブパスの記述
struct V3DSubpassDesc
{
	//! @brief 入力として使用するインプットアタッチメントの数です。
	uint32_t inputAttachmentCount;
	//! @brief 入力として使用するインプットアタッチメントの参照配列です。<br>
	//! 配列の要素の数は inputAttachmentCount の値と同じである必要があります。
	//! @note
	//! ここで指定するアタッチメント V3DAttachmentReference::attachment はイメージの使用方法に \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink が指定されている必要があります。
	const V3DAttachmentReference* pInputAttachments;

	//! @brief 出力先のカラーアタッチメントの数です。
	uint32_t colorAttachmentCount;
	//! @brief 出力先のカラーアタッチメントの参照配列です。<br>
	//! 配列の要素の数は colorAttachmentCount の値と同じである必要があります。<br>
	//! @note
	//! ここで指定するアタッチメント V3DAttachmentReference::attachment はイメージの使用方法 V3DImageDesc::usageFlags に \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink が指定されている必要があります。
	const V3DAttachmentReference* pColorAttachments;
	//! @brief 出力先のカラーアタッチメント ( pColorAttachments ) をマルチサンプリングした結果をレンダリングするアタッチメントの参照配列です。<br>
	//! 配列の要素の数は colorAttachmentCount の値と同じである必要があります。
	//! @note
	//! ここで指定するアタッチメント V3DAttachmentReference::attachment はイメージの使用方法 V3DImageDesc::usageFlags に \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink が指定されている必要があり、
	//! サンプル数 ( V3DImageDesc::samples ) は \link V3D_SAMPLE_COUNT_1 \endlink である必要があります。<br>
	//! マルチサンプリングイメージの変換先のアタッチメントとして使用しない場合は V3DAttachmentReference::attachment に \link V3D_ATTACHMENT_UNUSED \endlink を指定してください。<br>
	const V3DAttachmentReference* pResolveAttachments;

	//! @brief 出力先のデプスステンシルアタッチメントの参照です。
	//! @note
	//! ここで指定するアタッチメント V3DAttachmentReference::attachment はイメージの使用方法に \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink が指定されている必要があり、<br>
	//! サンプル数 V3DImageDesc::samples はカラーアタッチメントと同じである必要があります。<br>
	//! また、デプスステンシルアタッチメントが存在しない場合は nullptr を指定してください。<br>
	const V3DAttachmentReference* pDepthStencilAttachment;

	//! @brief このサブパスで使用されてなく、内容を保持したいアタッチメントの数です。
	uint32_t preserveAttachmentCount;
	//! @brief このサブパスで使用されてなく、内容を保持したいアタッチメントのインデックス配列です。<br>
	//! 配列の要素の数は preserveAttachmentCount の値と同じである必要があります。
	const uint32_t* pPreserveAttachments;
};

//! @struct V3DSubpassDependencyDesc
//! @brief サブパスの依存性の記述
struct V3DSubpassDependencyDesc
{
	uint32_t srcSubpass; //!< 現在のサブパスのインデックスです。
	uint32_t dstSubpass; //!< 次のサブパスのインデックスです。
	V3DFlags srcStageMask; //!< 現在のパイプラインステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 次のパイプラインステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags srcAccessMask; //!< 現在のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 次のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存を表す \link V3D_DEPENDENCY_FLAG \endlink 列挙定数の組み合わせです。
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
// フレームバッファ
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DFrameBufferDesc
//! @brief フレームバッファーの記述
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
//! @brief フレームバッファインターフェース
//! @sa IV3DDevice::CreateFrameBuffer
class V3D_DLL_API IV3DFrameBuffer : public IV3DDeviceChild
{
public:
	//! @brief フレームバッファの記述を取得します。
	//! @return フレームバッファの記述を返します。
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

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDescriptorDesc
//! @brief デスクリプタの記述
struct V3DDescriptorDesc
{
	uint32_t binding; //!< バインディングです。
	V3D_DESCRIPTOR_TYPE type; //!< デスクリプタのタイプです。
	V3DFlags stageFlags; //!< バインドするシェーダーステージを表す \link V3D_SHADER_STAGE_FLAG \endlink 列挙定数の組み合わせです。
};

//! @struct V3DDescriptorSetLayoutStatistics
//! @brief デスクリプタセットレイアウトの統計
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
	//! @brief デスクリプタセットのレイアウトを取得します。
	//! @param[out] ppLayout 取得したデスクリプタセットのレイアウトを渡す IV3DDescriptorSetLayout インタフェースのポインタのアドレスです。
	virtual void GetLayout(IV3DDescriptorSetLayout** ppLayout) = 0;

	//! @brief バッファーを取得します。
	//! @param[in] binding 取得するバッファーのバインディングです。
	//! @param[out] ppBuffer 取得したバッファーを渡す IV3DBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBuffer(uint32_t binding, IV3DBuffer** ppBuffer) = 0;
	//! @brief バッファーを設定します。
	//! @param[in] binding バッファーを設定するバインディングです。
	//! @param[in] pBuffer 設定するバッファーです。
	//! @param[in] offset 設定するバッファーのオフセットをバイト単位で指定します。
	//! @param[in] size 設定するバッファーのオフセットからのサイズをバイト単位で指定します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @note
	//! デスクリプタの種類によってアライメントが異なるため、offset には適切な値を指定してください。<br>
	//! 各デスクリプタのアライメントは V3DDeviceCaps に記述されており、以下のようになります。<br>
	//! <br>
	//! <table>
	//! <tr><td>\link V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER \endlink <br> \link V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER \endlink</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>\link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER \endlink <br> \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC \endlink</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>\link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER \endlink <br> \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC \endlink</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! また、指定したバインディングのデスクリプタが \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC \endlink または \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC \endlink の場合、<br>
	//! size に指定する値は、バッファー自体のサイズではなく、シェーダーが読み書きする範囲のサイズを指定します。
	virtual V3D_RESULT SetBuffer(uint32_t binding, IV3DBuffer* pBuffer, uint64_t offset, uint64_t size) = 0;

	//! @brief バッファービューを取得します。
	//! @param[in] binding 取得するバッファービューのバインディングです。
	//! @param[out] ppBufferView 取得したバッファービューを渡す IV3DBufferView インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) = 0;
	//! @brief バッファービューを設定します。
	//! @param[in] binding バッファービューを設定するバインディングです。
	//! @param[in] pBufferView 設定するバッファービューのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) = 0;

	//! @brief イメージビューを取得します。
	//! @param[in] binding 取得するイメージのバインディングです。
	//! @param[out] ppImageView 取得したイメージビューを渡す IV3DImageView インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) = 0;
	//! @brief デスクリプタセットがバインドされた際のイメージレイアウトを取得します。
	//! @param[in] binding 取得するイメージレイアウトのバインディングです。
	//! @param[out] pImageLayout 取得したイメージレイアウトを格納する \link V3D_IMAGE_LAYOUT \endlink 列挙定数のポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) = 0;
	//! @brief イメージビューを設定します。<br>
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @note
	//! サポートしているデスクリプタタイプ<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink<br> 
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) = 0;
	//! @brief イメージビューを設定します。<br>
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @param[in] imageLayout デスクリプタセットをバインドする際のイメージレイアウトです。<br>
	//! デスクリプタセットをバインドしたする前にこのイメージレイアウトに移行しておく必要があります。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! サポートしているデスクリプタタイプ<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) = 0;

	//! @brief サンプラーを取得します。
	//! @param[in] binding 取得するサンプラーのバインディングです。
	//! @param[out] ppSampler 取得したサンプラーを渡す IV3DSampler インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) = 0;
	//! @brief サンプラーを設定します。<br>
	//! @param[in] binding サンプラーを設定するバインディングです。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! <br>
	//! サポートしているデスクリプタタイプ<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLER \endlink 、 
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink のいずれかである必要があります。
	virtual V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) = 0;

	//! @brief イメージビューとサンラプーを設定します。<br>
	//! このバインディングのデスクリプタは 
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink である必要があります。
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! デスクリプタセットをバインドする前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @sa V3DDescriptorDesc
	//! @note イメージレイアウトは \link V3D_IMAGE_LAYOUT_SHADER_READ_ONLY \endlink を指定したことになります。
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, IV3DSampler* pSampler) = 0;

	//! @brief イメージビューとサンラプーを設定します。<br>
	//! このバインディングのデスクリプタは 
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink である必要があります。
	//! @param[in] binding イメージビューを設定するバインディングです。
	//! @param[in] pImageView 設定するイメージビューのポインタです。
	//! @param[in] imageLayout デスクリプタセットをバインドする際のイメージレイアウトです。<br>
	//! デスクリプタセットをバインドする前にこのイメージレイアウトに移行しておく必要があります。
	//! @param[in] pSampler 設定するサンプラーのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @sa V3DDescriptorDesc
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) = 0;

	//! @brief デスクリプタセットを更新します
	//! @note デスクリプタセットに各種リソースを設定した際に実行してください。
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
//! @brief 定数の記述
//! @sa IV3DCommandBuffer::PushConstant
struct V3DConstantDesc
{
	V3DFlags shaderStageFlags; //!< 定数をプッシュするシェーダーステージを表す \link V3D_SHADER_STAGE_FLAG \endlink 列挙定数の組み合わせです。
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
enum V3D_PIPELINE_TYPE
{
	V3D_PIPELINE_TYPE_GRAPHICS = 0, //!< グラフィックスパイプラインとして使用します。
	V3D_PIPELINE_TYPE_COMPUTE = 1, //!< コンピュートパイプラインとして使用します。
};

//! @enum V3D_PRIMITIVE_TOPOLOGY
//! @brief プリミティブトポロジ
enum V3D_PRIMITIVE_TOPOLOGY
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
enum V3D_POLYGON_MODE
{
	V3D_POLYGON_MODE_FILL = 0, //!< ポリゴンを塗りつぶして描画します。
	V3D_POLYGON_MODE_LINE = 1, //!< ポリゴンをエッジを描画します。
	V3D_POLYGON_MODE_POINT = 2, //!< ポリゴンの頂点を描画します。
};

//! @enum V3D_CULL_MODE
//! @brief カリングモード
enum V3D_CULL_MODE
{
	V3D_CULL_MODE_NONE = 0, //!< カリングをしません。
	V3D_CULL_MODE_FRONT = 1, //!< 前面をカリングします。
	V3D_CULL_MODE_BACK = 2, //!< 後面をカリングします。
};

//! @enum V3D_STENCIL_OP
//! @brief ステンシルオペレーター
enum V3D_STENCIL_OP
{
	V3D_STENCIL_OP_KEEP = 0, //!< 現在の値を保持します。
	V3D_STENCIL_OP_ZERO = 1, //!< 値を0に設定します。
	V3D_STENCIL_OP_REPLACE = 2, //!< 値を V3DPipelineStencilOpDesc::reference に設定します。
	V3D_STENCIL_OP_INCREMENT_AND_CLAMP = 3, //!< 現在の値をインクリメントし、最大値にクランプします。
	V3D_STENCIL_OP_DECREMENT_AND_CLAMP = 4, //!< 現在の値をデクリメントして 0 にクランプします。
	V3D_STENCIL_OP_INVERT = 5, //!< 現在の値のビット反転します。
	V3D_STENCIL_OP_INCREMENT_AND_WRAP = 6, //!< 現在の値をインクリメントし、最大値を超えた場合は 0 にラップします。
	V3D_STENCIL_OP_DECREMENT_AND_WRAP = 7, //!< 現在の値をデクリメントし、値が 0 より小さくなると最大値にラップします。
};

//! @enum V3D_BLEND_FACTOR
//! @brief ブレンド係数<br>
//! <table>
//! <tr><td>Rs0 Gs0 Bs0 As0</td><td>ブレンドされるカラーアタッチメントの第１のソースカラー</td></tr>
//! <tr><td>Rs1 Gs1 Bs1 As1</td><td>ブレンドされるカラーアタッチメントの第２のソースカラー</td></tr>
//! <tr><td>Rd Gd Bd Ad</td><td></td>出力先のカラー</tr>
//! <tr><td>Rc Gc Bc Ac</td><td></td>コマンド IV3DCommandBuffer::SetBlendConstants で設定するブレンド定数</tr>
//! </table>
//! <br>
enum V3D_BLEND_FACTOR
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
//! @brief ブレンドオペレーター<br>
//! <table>
//! <tr><td>Cs</td><td>入力元のインプットアタッチメントなどのカラー</td></tr>
//! <tr><td>Cd</td><td>出力先になるカラーアタッチメントのカラー</td></tr>
//! <tr><td>Fs</td><td>入力元のブレンド係数</td></tr>
//! <tr><td>Fd</td><td>出力先のブレンド係数</td></tr>
//! </table>
//! @sa V3D_BLEND_FACTOR
enum V3D_BLEND_OP
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
//! @brief 論理オペレーター<br>
//! <table>
//! <tr><td>s</td><td>フラグメントによって出力される RGBA 成分です。</td></tr>
//! <tr><td>d</td><td>カラーアタッチメントによって入力される RGBA 成分です。</td></tr>
//! </table>
//! <br>
enum V3D_LOGIC_OP
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
//! @brief パイプラインのシェーダー定数の記述
struct V3DPipelineShaderConstantDesc
{
	//! @brief 定数の識別子を整数で指定します。<br>
	//! layout(constant_id = [id]) const
	uint32_t id;
	//! @brief 定数データ上の定数のオフセットをバイト単位で指定します。
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

	uint32_t constantCount; //!< 定数の数です。
	V3DPipelineShaderConstantDesc* pConstants; //!< constantCount の値の数の要素を持つ V3DPipelineShaderConstantDesc 構造体の配列です。
	void* pConstantData; //!< 定数データのポインタです。

	//! @}
};

//! @struct V3DPipelineVertexElement
//! @brief パイプラインのバーテックスエレメント
struct V3DPipelineVertexElement
{
	//! @brief ロケーションです。<br>
	//! layout(location = [location])
	uint32_t location;
	//! @brief オフセットをバイト単位で指定します。
	uint32_t offset;
	//! @brief フォーマットです。
	V3D_FORMAT format;
};

//! @struct V3DPipelineVertexLayout
//! @brief パイプラインのバーテックスレイアウト
struct V3DPipelineVertexLayout
{
	//! @brief バーテックスバッファのバインド位置です。
	uint32_t binding;
	//! @brief バーテックスのストライをバイト単位で指定します。
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
	uint32_t patchControlPoints; //!< パッチごとのコントロールポイントの数です。
};

//! @struct V3DPipelineRasterizationDesc
//! @brief パイプラインのラスタライゼーションの記述
//! @note
//! デフォルト値<br>
//! <table>
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
	//! @brief ラスタライズステージの直前にプリミティブを破棄するかどうかを指定します。<br>
	//! この値が false の場合は、ビューポート、シザリングは必ず指定する必要があります。
	bool discardEnable;
	//! @brief 深度をクランプするかどうかを指定します。
	bool depthClampEnable;

	//! @brief ポリゴンモードです。
	V3D_POLYGON_MODE polygonMode;
	//! @brief カリングモードです。
	V3D_CULL_MODE cullMode;

	//! @brief 深度バイアスを有効にするかどうかを指定します。
	bool depthBiasEnable;
	//! @brief 一定の深度値を制御するスカラー因子です。<br>
	//! この値は IV3DCommandBuffer::SetDepthBias で変更することができます。
	float depthBiasConstantFactor;
	//! @brief 最大 または 最小の深度バイアスです。<br>
	//! この値は IV3DCommandBuffer::SetDepthBias で変更することができます。
	float depthBiasClamp;
	//! @brief 深度バイアスの計算におけるスロープに適用されるスカラー係数です。<br>
	//! この値は IV3DCommandBuffer::SetDepthBias で変更することができます。
	float depthBiasSlopeFactor;
};

//! @struct V3DPipelineMultisampleDesc
//! @brief パイプラインのマルチサンプルの記述
//! @note
//! デフォルト値<br>
//! <table>
//! <tr><td>rasterizationSamples</td><td>V3D_SAMPLE_COUNT_1</td></tr>
//! <tr><td>sampleShadingEnable</td><td>false</td></tr>
//! <tr><td>minSampleShading</td><td>0.0f</td></tr>
//! <tr><td>alphaToCoverageEnable</td><td>false</td></tr>
//! <tr><td>alphaToOneEnable</td><td>false</td></tr>
//! </table>
//! <br>
struct V3DPipelineMultisampleDesc
{
	//! @brief カラーアタッチメント、デプスステンシルアタッチメントのサンプル数です。<br>
	//! V3DDeviceCaps::multisampleFlags に \link V3D_MULTISAMPLE_CAP_VARIABLE_RATE \endlink が指定されていなかった場合、
	//! サブパスのカラーアタッチメント、デプスステンシルアタッチメントのサンプル数はこの値である必要があります。
	V3D_SAMPLE_COUNT_FLAG rasterizationSamples;
	//! @brief フラグメントがサンプル毎にシェーディングを実行するかどうかを指定します。<br>
	//! V3DDeviceCaps::multisampleFlags に \link V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING \endlink が指定されている場合に有効になります。
	bool sampleShadingEnable;
	//! @brief サンプル毎のシェーディングの最小分率です。<br>
	//! sampleShadingEnable を true にした場合は必ず指定してください。<br>
	//! また値の範囲は 0.0f ～ 1.0f の間である必要があります。
	float minSampleShading;
	//! @brief アルファトゥカバレッジを使用するかどうかを指定します。
	bool alphaToCoverageEnable;
	//! @brief フラグメントシェーダから出力されたアルファ値を、固定小数点が表現可能な最大値または 1.0 で置き換えるかどうかを指定します。<br>
	//! この操作は V3DDeviceCaps::multisampleFlags に \link V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE \endlink が指定されている場合にのみ true を指定することができます。
	bool alphaToOneEnable;
};

//! @struct V3DPipelineStencilOpDesc
//! @brief パイプラインのステンシルオペレーターの記述
//! @note
//! デフォルト値<br>
//! <table>
//! <tr><td>failOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>passOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>depthFailOp</td><td>V3D_STENCIL_OP_KEEP</td></tr>
//! <tr><td>compareOP</td><td>V3D_COMPARE_OP_ALWAYS</td></tr>
//! <tr><td>compareMask</td><td>0x000000FF</td></tr>
//! <tr><td>writeMask</td><td>0x000000FF</td></tr>
//! <tr><td>reference</td><td>0</td></tr>
//! </table>
//! <br>
struct V3DPipelineStencilOpDesc
{
	V3D_STENCIL_OP failOp; //!< ステンシルテストに失敗したサンプルに対して実行されるアクションです。
	V3D_STENCIL_OP passOp; //!< 深さテストとステンシルテストの両方に合格したサンプルに対して実行されるアクションです。
	V3D_STENCIL_OP depthFailOp; //!< ステンシルテストに合格し、深度テストに合格しなかったサンプルに対して実行されるアクションです。

	//! @brief ステンシルテストで使用される比較オペレーターです。<br>
	//! \link V3D_COMPARE_OP \endlink の説明にある R はマスクされた reference の値であり、S はマスクされたステンシルの値を表します。
	V3D_COMPARE_OP compareOp;

	//! @brief ステンシルテストによって読み込まれる値のビットマスクを指定します。<br>
	//! この値は IV3DCommandBuffer::SetStencilReadMask で変更することができます。
	uint32_t readMask;
	//! @brief ステンシルテストによって書き込まれる値のビットマスクを指定します。<br>
	//! この値は IV3DCommandBuffer::SetStencilWriteMask で変更することができます。
	uint32_t writeMask;
	//! @brief ステンシルの比較オペレーターで使用される値です。<br>
	//! この値は IV3DCommandBuffer::SetStencilReference で変更することができます。
	//! @sa V3D_COMPARE_OP
	uint32_t reference;
};

//! @struct V3DPipelineDepthStencilDesc
//! @brief パイプラインのデプスステンシルの記述
//! @note
//! デフォルト値<br>
//! <table>
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
	//! @brief 深度テストの比較オペレーターを指定します。<br>
	//! \link V3D_COMPARE_OP \endlink の説明にある R は入力される深度であり、S はアタッチメントの深度を表します。
	V3D_COMPARE_OP depthCompareOp;

	//! @brief ステンシルテストをするかどうかを指定します。
	bool stencilTestEnable;
	//! @brief ステンシルの前面のオペレーターです。
	V3DPipelineStencilOpDesc stencilFront;
	//! @brief ステンシルテストの後面のオペレーターです。
	V3DPipelineStencilOpDesc stencilBack;

	//! @brief 深度の境界テストをするかどうかを指定します。
	bool depthBoundsTestEnable;
	//! @brief 深度の境界テストの最小値です。<br>
	//! 値は 0.0f ～ 1.0f の間で指定する必要があります。<br>
	//! またこの値は IV3DCommandBuffer::SetDepthBounds で変更することができます。
	float minDepthBounds;
	//! @brief 深度の境界テストの最大値です。<br>
	//! 値は 0.0f ～ 1.0f の間で指定する必要があります。<br>
	//! またこの値は IV3DCommandBuffer::SetDepthBounds で変更することができます。
	float maxDepthBounds;
};

//! @struct V3DPipelineColorBlendAttachment
//! @brief パイプラインのカラーアタッチメントのブレンド
//! @note
//! デフォルト値<br>
//! <table>
//! <tr><td>blendEnable</td><td>true</td></tr>
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
	V3D_BLEND_OP colorOp; //!< RGB成分のブレンドオペレーターです。
	V3D_BLEND_FACTOR srcAlphaFactor; //!< ブレンド元のA成分のブレンド係数です。
	V3D_BLEND_FACTOR dstAlphaFactor; //!< ブレンド先のA成分のブレンド係数です。
	V3D_BLEND_OP alphaOp; //!< A成分のブレンドオペレーターです。
	V3DFlags writeMask; //!< カラーの書き込みマスクを表す \link V3D_COLOR_COMPONENT_FLAG \endlink 列挙定数の組み合わせです。
};

//! @struct V3DPipelineColorBlendDesc
//! @brief パイプラインのカラーブレンドの記述
//! @note
//! デフォルト値<br>
//! <table>
//! <tr><td>logicOpEnable</td><td>false</td></tr>
//! <tr><td>logicOp</td><td>V3D_LOGIC_OP_COPY</td></tr>
//! <tr><td>attachmentCount</td><td>1 以上</td></tr>
//! <tr><td>pAttachments</td><td>attachmentCount の値の数の要素を持つ V3DPipelineColorBlendAttachment 構造体の配列</td></tr>
//! </table>
//! <br>
struct V3DPipelineColorBlendDesc
{
	bool logicOpEnable; //!< 論理演算 ( logicOp ) を有効にするかどうかを指定します。
	V3D_LOGIC_OP logicOp; //!< 論理演算の種類です。

	//! @brief サブパスのカラーアタッチメントの数です。
	uint32_t attachmentCount;
	//! @brief サブパスのカラーアタッチメントのブレンド配列です。<br>
	//! 配列の要素の数は attachmentCount 値と同じである必要がります。
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
	V3DPipelineRasterizationDesc rasterization; //!< ラスタライゼーションです。
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
	V3DFlags imageUsageFlags; //!< イメージの使用方法です。

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

	//! @brief レンダリングするイメージを獲得します。
	//! @retval V3D_OK
	//! @retval V3D_TIMEOUT
	//! @retval V3D_NOT_READY
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT AcquireNextImage() = 0;

	//! @brief 現在のイメージのインデックスを取得します。
	//! @return 現在のイメージのインデックスを返します。
	//! @note 返される値は AcquireNextImage で更新されます。
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
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
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
	V3DFlags propertyFlags; //!< 特性を表す V3D_COMMAND_POOL_PROPERTY_FLAG 列挙定数の組み合わせです。
};

//! @}

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_POOL_PROPERTY_FLAG
//! @brief コマンドプールプロパティフラグ
enum V3D_COMMAND_POOL_PROPERTY_FLAG : V3DFlags
{
	//! @brief コマンドプールーから割り当てられたコマンドバッファーが比較的短時間にリセットまたは解放されることを表します。
	V3D_COMMAND_POOL_PROPERTY_TRANSIENT = 0x00000001,
	//! @brief コマンドプールーから割り当てられたコマンドバッファーを個別にリセットできるようにします。<br>
	//! このフラグが指定された場合は IV3DCommandBuffer::Reset によるリセットができ IV3DCommandBuffer::Begin による暗黙的なリセットが行われます。<br>
	//! また、このフラグが指定されなかった場合 IV3DCommandBuffer::Reset は使用できず IV3DCommandPool::Reset によってコマンドバッファーを一括でリセットするようになります。
	V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER = 0x00000002,
};

//! @enum V3D_COMMAND_ALLOCATOR_RESET_FLAG
//! @brief コマンドプールのリセットフラグ
enum V3D_COMMAND_ALLOCATOR_RESET_FLAG : V3DFlags
{
	//! @brief リセットした際にコマンドバッファが所有するメモリリソースをコマンドプールーに返却します。<br>
	//! またこのフラグが指定されなかった場合はメモリリソースを返却せず、再利用します。
	V3D_COMMAND_ALLOCATOR_RESET_RELEASE_RESOURCES = 0x00000001,
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
	//! @param[in] resetFlags リセットフラグを表す \link V3D_COMMAND_ALLOCATOR_RESET_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
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
enum V3D_COMMAND_BUFFER_TYPE
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

//! @enum V3D_COMMAND_BUFFER_USAGE
//! @brief コマンドバッファの使用方法
enum V3D_COMMAND_BUFFER_USAGE
{
	//! @brief コマンドバッファーは一度だけキューに送信されることを表します。
	V3D_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001,
	//! @brief プライマリコマンドバッファーで開始されたレンダーパス内で実行されるセカンダリコマンドバッファーがレンダーパス、サブパス、フレームバッファを引き継ぐことを表します。
	V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE = 0x00000002,
	//! @brief コマンドバッファーが複数のキューに送信 (IV3DQueue::Submit) またはプライマリコマンドバッファーに記録 ( IV3DCommandBuffer::ExecuteCommandBuffers ) できることを表します。<br>
	//! また、プライマリコマンドバッファーにこの使用法が指定されている場合、そこで実行されるセカンダリコマンドバッファーも同様にこの使用法を指定する必要があります。
	V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004,
};

//! @enum V3D_STENCIL_FACE_FLAG
//! @brief ステンシルのフェイスフラグ
enum V3D_STENCIL_FACE_FLAG
{
	V3D_STENCIL_FACE_FRONT = 0x00000001, //!< 前を指定します。
	V3D_STENCIL_FACE_BACK = 0x00000002, //!< 後ろを指定します。
	V3D_STENCIL_FRONT_AND_BACK = V3D_STENCIL_FACE_FRONT | V3D_STENCIL_FACE_BACK, //!< 前後両方を指定します。
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBarrierMemoryDesc
//! @brief メモリバリアの記述
struct V3DBarrierMemoryDesc
{
	V3DFlags srcStageMask; //!< 現在のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存性を表す \link V3D_DEPENDENCY_FLAG \endlink 列挙定数の組み合わせです。

	V3DFlags srcAccessMask; //!< 現在のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
};

//! @struct V3DBarrierBufferDesc
//! @brief バッファーバリアの記述
struct V3DBarrierBufferDesc
{
	V3DFlags srcStageMask; //!< 現在のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存性を表す \link V3D_DEPENDENCY_FLAG \endlink 列挙定数の組み合わせです。

	V3DFlags srcAccessMask; //!< 現在のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。

	uint32_t srcQueueFamily; //!< 現在のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。

	uint64_t offset; //!< バッファーのメモリのオフセットをバイト単位で指定します。
	uint64_t size; //!< バッファーのメモリのオフセットからのサイズをバイト単位で指定します。
};

//! @struct V3DBarrierBufferViewDesc
//! @brief バッファービューバリアの記述
struct V3DBarrierBufferViewDesc
{
	V3DFlags srcStageMask; //!< 現在のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存性を表す \link V3D_DEPENDENCY_FLAG \endlink 列挙定数の組み合わせです。

	V3DFlags srcAccessMask; //!< 現在のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。

	uint32_t srcQueueFamily; //!< 現在のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。
};

//! @struct V3DBarrierImageDesc
//! @brief イメージバリアの記述
struct V3DBarrierImageDesc
{
	V3DFlags srcStageMask; //!< 現在のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstStageMask; //!< 移行先のステージを表す \link V3D_PIPELINE_STAGE_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dependencyFlags; //!< 依存性を表す \link V3D_DEPENDENCY_FLAG \endlink 列挙定数の組み合わせです。

	V3DFlags srcAccessMask; //!< 現在のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags dstAccessMask; //!< 移行先のアクセス方法を表す \link V3D_ACCESS_FLAG \endlink 列挙定数の組み合わせです。

	uint32_t srcQueueFamily; //!< 現在のキューファミリーです。
	uint32_t dstQueueFamily; //!< 移行先のキューファミリーです。

	V3D_IMAGE_LAYOUT srcLayout; //!< 現在のイメージレイアウトです。
	V3D_IMAGE_LAYOUT dstLayout; //!< 移行先のイメージレイアウトです。
};

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
	//! @brief デプスをクリアするかどうかを指定します。<br>
	//! クリアしようとしているデプスステンシルアタッチメントのフォーマットにデプスが含まれている必要があります。
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
	//! @param[in] resetFlags コマンドバッファーのリセットフラグを表す \link V3D_COMMAND_BUFFER_RESET_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset(V3DFlags resetFlags) = 0;

	//! @brief コマンドの書き込みを開始します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! このメソッドはセカンダリコマンドバッファーで使用することはできません。
	virtual V3D_RESULT Begin() = 0;
	//! @brief コマンドの書き込みを開始します。
	//! @param[in] usageFlags コマンドバッファーの使用法を表す \link V3D_COMMAND_BUFFER_USAGE \endlink 列挙定数の組み合わせです。
	//! @param[in] pRenderPass プライマリコマンドバッファーから引き継ぐレンダーパスです。
	//! @param[in] subpass プライマリコマンドバッファーから引き継ぐサブパスです。
	//! @param[in] pFrameBuffer プライマリコマンドバッファーから引き継ぐフレームバッファーです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! usageFlags に \link V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE \endlink を指定した場合は pRenderPass と subpass は必ず指定してください。<br>
	//! また pFrameBuffer に nullptr を指定することも可能ですが、パフォーマンスを考慮したい場合は有効なフレームバッファを指定することを推奨します。
	virtual V3D_RESULT Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer) = 0;
	//! @brief コマンドの書き込みを終了します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT End() = 0;

	//! @}

	//! @name コマンド
	//! @{

	//! @brief メモリにバリアを張ります。
	//! @param[in] barrier バリアを張るメモリの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierMemory(const V3DBarrierMemoryDesc& barrier) = 0;

	//! @brief バッファーにバリアを張ります。
	//! @param[in] pBuffer バリアを張るバッファーです。
	//! @param[in] barrier バリアを張るバッファーの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBuffer(IV3DBuffer* pBuffer, const V3DBarrierBufferDesc& barrier) = 0;

	//! @brief バッファービューにバリアを張ります。
	//! @param[in] pBufferView バリアを張るバッファビューです。
	//! @param[in] barrier バリアを張るバッファーの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBufferView(IV3DBufferView* pBufferView, const V3DBarrierBufferViewDesc& barrier) = 0;

	//! @brief バッファービューにバリアを張ります。
	//! @param[in] bufferViewCount バリアを張るバッファビューの数です。
	//! @param[in] ppBufferView バリアを張るバッファビューの配列です。
	//! @param[in] barrier バリアを張るバッファーの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBufferViews(uint32_t bufferViewCount, IV3DBufferView** ppBufferViews, const V3DBarrierBufferViewDesc& barrier) = 0;

	//! @brief イメージにバリアを張ります。
	//! @param[in] pImage バリアを張るイメージです。
	//! @param[in] barrier バリアを張るイメージの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImage(IV3DImage* pImage, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief イメージビューにバリアを張ります。
	//! @param[in] pImageView バリアを張るイメージビューです。
	//! @param[in] barrier バリアを張るイメージの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImageView(IV3DImageView* pImageView, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief イメージビューにバリアを張ります。
	//! @param[in] imageVewCount バリアを張るイメージビューの数です。
	//! @param[in] ppImageViews バリアを張るイメージビューの配列です。
	//! @param[in] barrier バリアを張るイメージの記述です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImageViews(uint32_t imageVewCount, IV3DImageView** ppImageViews, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief バッファーをコピーします。
	//! @param[in] pDstBuffer コピー先のバッファを表す IV3DBuffer インターフェースのポインタです。
	//! @param[in] dstOffset コピー先のバッファのメモリオフセットをバイト単位で指定します。
	//! @param[in] pSrcBuffer コピー元のバッファを表す IV3DBuffer インターフェースのポインタです。
	//! @param[in] srcOffset コピー元のバッファのメモリオフセットをバイト単位で指定します。
	//! @param[in] size コピーするサイズ
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size) = 0;

	//! @brief バッファーをコピーします。
	//! @param[in] pDstBuffer コピー先のバッファを表す IV3DBuffer インターフェースのポインタです。
	//! @param[in] pSrcBuffer コピー元のバッファを表す IV3DBuffer インターフェースのポインタです。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! @param[in] pRanges rangeCount の値の数の要素を持つ V3DCopyBufferRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges) = 0;

	//! @brief イメージをコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! @param[in] dstImageLayout コピー先のイメージレイアウトです。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! @param[in] srcImageLayout コピー元のイメージレイアウトです。
	//! @note コピー先、コピー元のイメージのタイプ、フォーマット、サイズ、レベル数、レイヤー数、アスペクトのすべてが一致している必要があります。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief イメージをコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! @param[in] dstImageLayout コピー先のイメージレイアウトです。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! @param[in] srcImageLayout コピー元のイメージレイアウトです。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! @param[in] pRanges rangeCount 値の数の要素を持つ V3DCopyImageRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageRange* pRanges) = 0;

	//! @brief バッファをイメージにコピーします。
	//! @param[in] pDstImage コピー先のイメージです。
	//! @param[in] dstImageLayout コピー先のイメージのレイアウトです。
	//! @param[in] dstLevel コピー先のイメージのレベルです。
	//! @param[in] dstBaseLayer コピー先のイメージの最初のレベルです。
	//! @param[in] dstLayerCount コピー先のイメージのレベル数です。
	//! @param[in] dstOffset コピー先のイメージのオフセットです。
	//! @param[in] dstSize コピー先のイメージのサイズです。
	//! @param[in] pSrcBuffer コピー元のバッファです。
	//! @param[in] srcBufferOffset コピー元のバッファのメモリオフセットをバイト単位で指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize,
		IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset) = 0;

	//! @brief バッファをイメージにコピーします
	//! @param[in] pDstImage コピー先のイメージです。
	//! @param[in] dstImageLayout コピー先のイメージのレイアウトです。
	//! @param[in] pSrcBuffer コピー元のバッファです。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! @param[in] pRanges rangeCount の値の要素の数を持つ V3DCopyBufferToImageRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DBuffer* pSrcBuffer,
		uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges) = 0;

	//! @brief イメージをバッファーにコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! @param[in] dstBufferOffset コピー先のバッファーのメモリオフセットをバイト単位で指定します。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! @param[in] srcImageLayout コピー元のイメージのレイアウトです。
	//! @param[in] srcLevel コピー元のイメージのレベルです。
	//! @param[in] srcBaseLayer コピー元のイメージの最初のレイヤーです。
	//! @param[in] srcLayerCount コピー元のイメージのレイヤー数です。
	//! @param[in] srcOffset コピー元のイメージのオフセットです。
	//! @param[in] srcSize コピー元のイメージのサイズです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize) = 0;

	//! @brief イメージをバッファーにコピーします。
	//! @param[in] pDstBuffer コピー先のバッファーです。
	//! @param[in] pSrcImage コピー元のイメージです。
	//! @param[in] srcImageLayout コピー元のイメージのレイアウトです。
	//! @param[in] rangeCount コピーする範囲の数です。
	//! @param[in] pRanges rangeCount の値の要素の数を持つ V3DCopyImageToBufferRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges) = 0;

	//! @brief イメージを転送します。
	//! @param[in] pDstImage 転送先のイメージです。
	//! @param[in] dstImageLayout 転送先のイメージレイアウトです。
	//! @param[in] pSrcImage 転送元のイメージです。
	//! @param[in] srcImageLayout 転送元のイメージレイアウトです。
	//! @param[in] rangeCount 転送する範囲の数です。
	//! @param[in] pRanges rangeCount 値の数の要素を持つ V3DBlitImageRange 構造体の配列です。
	//! @param[in] filter フィルターです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BlitImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, V3DBlitImageRange* pRanges,
		V3D_FILTER filter) = 0;

	//! @brief マルチサンプルイメージを非マルチサンプルイメージの変換します。
	//! @param[in] pDstImage 変換先のイメージです。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! @param[in] pSrcImage 変換元のイメージです。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! @note
	//! 変換先、変換元のイメージのタイプ、フォーマット、サイズ、レベル数、レイヤー数のすべてが一致している必要があります。<br>
	//! <br>
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief マルチサンプルイメージを非マルチサンプルイメージの変換します。
	//! @param[in] pDstImage 変換先のイメージです。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! @param[in] pSrcImage 変換元のイメージです。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! @param[in] rangeCount 変換する範囲の数です。
	//! @param[in] pRanges rangeCount 値の数の要素を持つ V3DResolveImageRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DResolveImageRange* pRanges) = 0;

	//! @brief マルチサンプルイメージを非マルチサンプルイメージの変換します。
	//! @param[in] pDstImageView 変換先のイメージビューです。
	//! @param[in] dstImageLayout 変換先のイメージレイアウトです。
	//! @param[in] pSrcImageView 変換元のイメージビューです。
	//! @param[in] srcImageLayout 変換元のイメージレイアウトです。
	//! @note
	//! 変換先、変換元のイメージビューのタイプ、フォーマット、サイズ、レベル数、レイヤー数のすべてが一致している必要があります。<br>
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief レンダーパスを開始します。
	//! @param[in] pRenderPass 開始するレンダーパスを表す IV3DRenderPass インターフェースのポインタです。
	//! @param[in] pFrameBuffer サブパスが参照するアタッチメントが入っているフレームバッファです。
	//! @param[in] subpassContentInline サブパスのコマンドをこのコマンドバッファーに記録する場合は true を指定します。<br>
	//! またサブパスのコマンドをセカンダリコマンドバッファーで記録してこのコマンドバッファーで実行したい場合は false を指定してください。
	//! @param[in] pRenderArea レンダリング領域です。<br>
	//! nullptr の場合は アタッチメント全体を表します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>

	virtual void BeginRenderPass(IV3DRenderPass* pRenderPass, IV3DFrameBuffer* pFrameBuffer, bool subpassContentInline, const V3DRectangle2D* pRenderArea = nullptr) = 0;
	//! @brief レンダーパスを終了します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     レンダーパス内でのみ有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void EndRenderPass() = 0;

	//! @brief 次のサブパスへ移行します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     レンダーパス内でのみ有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void NextSubpass() = 0;

	//! @brief イメージをクリアします。
	//! @param[in] pImage クリアするイメージです。
	//! @param[in] imageLayout クリアするイメージのレイアウトです。<br>
	//! この値は \link V3D_IMAGE_LAYOUT_GENERAL \endlink 、 \link V3D_IMAGE_LAYOUT_TRANSFER_DST \endlink のいずれかである必要があります。
	//! @param[in] clearValue クリアする値です。
	//! @note 
	//! クリアする前にイメージを指定したレイアウトに移行している必要があります。<br>
	//! <br>
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink ※ <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!       <br>
	//!       ※ クリアするイメージがデプスステンシルの場合は無効です
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BarrierImage
	virtual void ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief イメージビューをクリアします。
	//! @param[in] pImageView クリアするイメージのビューを表す IV3DImageView インターフェースのポインタです。
	//! @param[in] imageLayout クリアするイメージのレイアウトです。<br>
	//! この値は \link V3D_IMAGE_LAYOUT_GENERAL \endlink 、 \link V3D_IMAGE_LAYOUT_TRANSFER_DST \endlink のいずれかである必要があります。
	//! @param[in] clearValue クリアする値です。
	//! @note 
	//! クリアする前にイメージを指定したレイアウトに移行している必要があります。<br>
	//! <br>
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink ※ <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!       <br>
	//!       ※ クリアするイメージがデプスステンシルの場合は無効です
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BarrierImageView
	virtual void ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief サブパスのアタッチメントをクリアします。
	//! @param[in] colorAttachmentCount クリアするアタッチメントの数です。
	//! @param[in] pColorAttachments colorAttachmentCount の値の数の要素を持つ V3DClearAttachmentDesc 構造体の配列です。
	//! @param[in] pDepthStencilAttachment デプスステンシルアタッチメントのクリアの記述のポインタです。 ( デプスステンシルが無い場合は nullptr )
	//! @param[in] rangeCount クリアする範囲の数です。
	//! @param[in] pRanges rectCount の値の数の要素を持つ V3DClearRange 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     レンダーパス内でのみ有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ClearAttachments(
		uint32_t colorAttachmentCount, const V3DClearColorAttachmentDesc* pColorAttachments,
		const V3DClearDepthStencilAttachmentDesc* pDepthStencilAttachment,
		uint32_t rangeCount, const V3DClearRange* pRanges) = 0;

	//! @brief パイプラインをバインドします。
	//! @param[in] pPipeline バインドするパイプラインを表す IV3DPipeline インターフェースのポインタです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindPipeline(IV3DPipeline* pPipeline) = 0;

	//! @brief デスクリプタセットをバインドします。
	//! @param[in] pipelineType パイプラインのタイプです。
	//! @param[in] pPipelineLayout パイプラインのレイアウトです。
	//! @param[in] firstSet デスクリプタセットを設定する最初のセット番号です。
	//! @param[in] descriptorSetCount バインドするデスクリプタセットの数です。
	//! @param[in] ppDescriptorSets descriptorSetCount の値の数の要素を持つ IV3DDescriptorSet インターフェースのポインタの配列です。
	//! @param[in] dynamicOffsetCount ダイナミックオフセットの数です。<br>
	//! この値はデスクリプタセットに格納されているダイナミックユニフォームバッファーの数と同じである必要があります。
	//! @param[in] pDynamicOffsets ダイナミックオフセットの配列です。<br>
	//! この配列は dynamicOffsetCount の値の数の要素を持っている必要があり、各要素の値はバイト単位で指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType,	IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets,
		uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) = 0;

	//! @brief バーテックスバッファーをバインドします。
	//! @param[in] firstBinding 最初のバインディングです。
	//! @param[in] bindingCount バインディング数です。
	//! @param[in] ppBuffers バッファーのポインタの配列です。<br>
	//! この配列は bindingCount の値の数の要素が必要です。
	//! @param[in] pOffsets バッファーのメモリオフセットの配列です。<br>
	//! この配列は bindingCount の値の数の要素が必要であり、各要素の値はバイト単位で指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, IV3DBuffer** ppBuffers, const uint64_t* pOffsets) = 0;

	//! @brief インデックスバッファーをバインドします。
	//! @param[in] pBuffer バインドするインデックスバッファーです。
	//! @param[in] offset バインドするインデックスバッファーのメモリオフセットをバイト単位で指定します。
	//! @param[in] indexType インデックスタイプを表す \link V3D_INDEX_TYPE \endlink 列挙定数のいずれかを指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindIndexBuffer(IV3DBuffer* pBuffer, uint64_t offset, V3D_INDEX_TYPE indexType) = 0;

	//! @brief 定数をプッシュします。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] slot 定数をプッシュするスロットです。<br>
	//! パイプラインレイアウト作成時に指定した V3DConstantDesc 構造体の配列のインデックスになります。
	//! @param[in] pData プッシュするデータです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData) = 0;

	//! @brief 定数をプッシュします。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] slot 定数をプッシュするスロットです。<br>
	//! パイプラインレイアウト作成時に指定した V3DConstantDesc 構造体の配列のインデックスです。
	//! @param[in] offset プッシュ先の定数のメモリオフセットをバイト単位で指定します。
	//! @param[in] size プッシュするデータのサイズをバイト単位で指定します。
	//! @param[in] pData プッシュするデータです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData) = 0;

	//! @brief ビューポートを設定します。
	//! @param[in] firstViewport 設定する最初のビューポートのインデックスです。
	//! @param[in] viewportCount 設定するビューポートの数です。<br>
	//! ビューポートの最大数は V3DDeviceCaps::maxViewports の値になります。
	//! @param[in] pViewports viewportCount の値の数の要素を持つ V3DViewport 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports) = 0;

	//! @brief シザリングを設定します。
	//! @param[in] firstScissor 設定する最初のシザリングのインデックスです。
	//! @param[in] scissorCount 設定するシザリングの数です。<br>
	//! シザリングの最大数は V3DDeviceCaps::maxViewports の値になります。
	//! @param[in] pScissors scissorCount の値の数の要素を持つ V3DRectangle2D 構造体の配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors) = 0;

	//! @brief 深度バイアスを設定します。
	//! @param[in] depthBiasConstantFactor 一定の深度値を制御するスカラー因子です。
	//! @param[in] depthBiasClamp 最大または最小の深度バイアスです。
	//! @param[in] depthBiasSlopeFactor 深度バイアスの計算におけるスロープに適用されるスカラー係数です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;

	//! @brief 深度境界テストを設定します。
	//! @param[in] minDepthBounds 最小深度を 0.0f ～ 1.0f の間で指定します。
	//! @param[in] maxDepthBounds 最大深度を 0.0f ～ 1.0f の間で指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetDepthBounds(float minDepthBounds, float maxDepthBounds) = 0;

	//! @brief ステンシルテストによって読み込まれる値のビットマスクを設定します。
	//! @param[in] faceMask 設定するフェイスを表す \link V3D_STENCIL_FACE_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] readMask 読み込まれる値のビットマスクです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetStencilReadMask(V3DFlags faceMask, uint32_t readMask) = 0;

	//! @brief ステンシルテストによって書き込まれる値のビットマスクを設定します。
	//! @param[in] faceMask 設定するフェイスを表す \link V3D_STENCIL_FACE_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] writeMask 書き込まれる値のビットマスクです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetStencilWriteMask(V3DFlags faceMask, uint32_t writeMask) = 0;

	//! @brief ステンシルの比較オペレーターで使用される値を設定します。
	//! @param[in] faceMask 設定するフェイスを表す \link V3D_STENCIL_FACE_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] reference 比較オペレーターで使用される値です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_COMPARE_OP
	virtual void SetStencilReference(V3DFlags faceMask, uint32_t reference) = 0;

	//! @brief ブレンド定数を設定します。
	//! @param[in] blendConstants RGBA のブレンド定数を 0.0f ～ 1.0f の間で指定します。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_BLEND_FACTOR
	//! @sa V3DPipelineColorBlendAttachment
	virtual void SetBlendConstants(const float blendConstants[4]) = 0;

	//! @brief クエリプールをリセットします。
	//! @param[in] pQueryPool リセットするクエリプールです。
	//! @param[in] firstQuery リセットする最初のクエリのインデックスです。
	//! @param[in] queryCount リセットするクエリの数です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount) = 0;
	//! @brief クエリを開始します。
	//! @param[in] pQueryPool クエリの結果を管理するクエリプールです。
	//! @param[in] query 結果の取得を開始するクエリのインデックスです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;
	//! @brief クエリを終了します。
	//! @param[in] pQueryPool クエリの結果を管理するクエリプールです。
	//! @param[in] query 結果が格納されているクエリのインデックスです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void EndQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;
	//! @brief タイプスタンプを書き込みます。
	//! @param[in] pQueryPool タイムスタンプを管理するクエリプールです。
	//! @param[in] pipelineStage パイプラインステージです。
	//! @param[in] query タイムスタンプのクエリのインデックスです。
	//! @note
	//! 指定したパイプラインステージの実行が完了した際にタイムスタンプの値を書き込みます。<br>
	//! <br>
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_PIPELINE_STAGE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query) = 0;

	//! @brief 描画をします。
	//! @param[in] vertexCount 頂点の数です。
	//! @param[in] instanceCount インスタンスの数です。
	//! @param[in] firstVertex 最初の頂点のインデックスです。
	//! @param[in] firstInstance 最初のインスタンスのインデックスです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     レンダーパス内でのみ有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

	//! @brief インデックスバッファを使用した描画をします。
	//! @param[in] indexCount インデックスの数です。
	//! @param[in] instanceCount インスタンスの数です。
	//! @param[in] firstIndex 最初のインデックスです。
	//! @param[in] firstInstance 最初のインスタンスのインデックスです。
	//! @param[in] vertexOffset 頂点のバイトオフセットです。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_TOP_OF_PIPE \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!     <td>
	//!     レンダーパス内でのみ有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset) = 0;

	//! @brief コンピュートシェーダーの作業をディスパッチします。
	//! @param[in] groupCountX X 方向にディスパッチされるグループの数です。
	//! @param[in] groupCountY Y 方向にディスパッチされるグループの数です。
	//! @param[in] groupCountZ Z 方向にディスパッチされるグループの数です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!       \link V3D_COMMAND_BUFFER_TYPE_SECONDARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!       \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink <br>
	//!     <td>
	//!     無効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

	//! @brief セカンダリコマンドバッファーを実行します。
	//! @param[in] commandBufferCount 実行するセカンダリコマンドバッファーの数です。
	//! @param[in] ppCommandBuffers commandBufferCount の値の数の要素を持つ IV3DCommandBuffer インターフェースのポインタの配列です。
	//! @note
	//! <table>
	//!   <tr><th>サポートされるコマンドバッファー</th><th>サポートされるキュー</th><th>サポートされるパイプラインステージ</th><th>レンダーパス内での使用</th></tr>
	//!   <tr>
	//!     <td>
	//!       \link V3D_COMMAND_BUFFER_TYPE_PRIMARY \endlink <br>
	//!     </td>
	//!     <td>
	//!       \link V3D_QUEUE_GRAPHICS \endlink <br>
	//!       \link V3D_QUEUE_COMPUTE \endlink <br>
	//!       \link V3D_QUEUE_TRANSFER \endlink <br>
	//!     </td>
	//!     <td>
	//!     </td>
	//!     <td>
	//!     有効
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ExecuteCommandBuffers(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers) = 0;

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
//! @brief キューフラグ
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
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーを表す IV3DCommandBuffer インターフェースのポインタの配列です。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。<br>
	//! nullptr を指定した場合は、送信の完了を待機しません。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;
	//! @brief プライマリコマンドバッファーをキューに送信します。
	//! @param[in] pSwapChain 同期をとるスワップチェインです。
	//! @param[in] commandBufferCount 送信するコプライマリマンドバッファーの数です。
	//! @param[in] ppCommandBuffers 送信するプライマリコマンドバッファーを表す IV3DCommandBuffer インターフェースのポインタの配列です。
	//! @param[in] pFence コマンドバッファーの送信の完了の待機に使用するフェンスです。<br>
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORYDrawModels
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;

	//! @brief レンダリングされたイメージを表示します。
	//! @param[in] pSwapChain 同期をとるスワップチェインです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) = 0;

	//! @brief キューがアイドル状態になるまで待機します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
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
enum V3D_MEMORY_HEAP_TYPE
{
	V3D_MEMORY_HEAP_TYPE_HOST = 0, //!< ホストのメモリヒープです。
	V3D_MEMORY_HEAP_TYPE_DEVICE = 1, //!< デバイスのメモリヒープです。
};

//! @enum V3D_BUFFER_FORMAT_FEATURE_FLAG
//! @brief バッファーフォーマットのサポートフラグ
enum V3D_BUFFER_FORMAT_FEATURE_FLAG
{
	//! @brief ユニフォームテクセルバッファとして使用できます。
	V3D_BUFFER_FORMAT_FEATURE_UNIFORM_TEXEL = 0x00000008,
	//! @brief ストレージテクセルバッファとして使用できます。
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL = 0x00000010,
	//! @brief アトミック操作をサポートするストレージテクセルバッファとして使用できます。
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL_ATOMIC = 0x00000020,
	//! @brief バーテックスバッファとして使用できます。
	V3D_BUFFER_FORMAT_FEATURE_VERTEX = 0x00000040,
};

//! @enum V3D_IMAGE_FORMAT_FEATURE_FLAG
//! @brief イメージフォーマットのサポートフラグ
enum V3D_IMAGE_FORMAT_FEATURE_FLAG
{
	//! @brief サンプリングできるイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED = 0x00000001,
	//! @brief ストレージイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_STORAGE = 0x00000002,
	//! @brief アトミック操作をサポートするストレージイメージとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_STORAGE_ATOMIC = 0x00000004,
	//! @brief カラーアタッチメントおよびインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT = 0x00000080,
	//! @brief ブレンディングをサポートするカラーアタッチメントおよびインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND = 0x00000100,
	//! @brief デプスステンシルアタッチメントおよびインプットアタッチメントとして使用できます。
	V3D_IMAGE_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT = 0x00000200,
	//! @brief IV3DCommandBuffer::BlitImage の pSrcImage として使用することができます。
	V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC = 0x00000400,
	//! @brief IV3DCommandBuffer::BlitImage の pDstImage として使用することができます。
	V3D_IMAGE_FORMAT_FEATURE_BLIT_DST = 0x00000800,
	//! @brief \link V3D_IMAGE_FORMAT_FEATURE_SAMPLED \endlink と一緒に指定されていた場合は、サンプラーの拡大フィルタおよび縮小フィルタに \link V3D_MIPMAP_MODE_LINEAR \endlink を使用することができます。<br>
	//! また \link V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC \endlink と一緒に指定されていた場合は、\link IV3DCommandBuffer::BlitImage \endlink の filter に \link V3D_FILTER_LINEAR \endlink を使用することができます。
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
	V3DFlags propertyFlags; //!< メモリ特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
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
	//! @brief インデックスバッファに 符号無し 32Bit 整数 ( \link V3D_INDEX_TYPE_UINT32 \endlink ) を使用することができます。
	//! @sa IV3DCommandBuffer::BindIndexBufferView
	V3D_CAP_FULL_DRAW_INDEX_UINT32 = 0x00000001,
	//! @brief 異方性フィルタリングを使用できることを表します。
	//! @sa V3DSamplerDesc::anisotropyEnable
	//! @sa V3DSamplerDesc::maxAnisotropy
	V3D_CAP_SAMPLER_ANISOTROPY = 0x00000002,
};

//! @enum V3D_QUERY_CAP_FLAG
//! @brief クエリの能力フラグ
enum V3D_QUERY_CAP_FLAG : V3DFlags
{
	//! @brief オクルージョンクエリ \link V3D_QUERY_TYPE_OCCLUSION \endlink で厳密な結果を取得することができます。<br>
	//! このフラグが指定されていなかった場合、オクルージョンクエリの結果はピクセルが書き込まれなかった場合は 0 ピクセルが書き込まれた場合は 1 のように返すことがあります。
	//! @sa IV3DCommandBuffer::BeginQuery
	V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE = 0x00000001,
	//! @brief パイプラインの統計クエリである \link V3D_QUERY_TYPE_PIPELINE_STATISTICS \endlink をサポートします。
	//! @sa IV3DDevice::CreateQueryPool
	V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY = 0x00000002,
	//! @brief プライマリコマンドバッファーでオクルージョンクエリがアクティブになっている間 ( IV3DCommandBuffer::BeginQuery ～ IV3DCommandBuffer::EndQuery ) に、セカンダリコマンドバッファを実行することができます。
	//! @sa IV3DCommandBuffer::BeginQuery
	//! @sa IV3DCommandBuffer::ExecuteCommandBuffers
	V3D_QUERY_CAP_INHERITED_QUERIES = 0x00000004,
};

//! @enum V3D_IMAGE_CAP_FLAG
//! @brief イメージの能力フラグ
enum V3D_IMAGE_CAP_FLAG : V3DFlags
{
	//! @brief イメージビューのタイプに \link V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY \endlink を指定することができます。
	//! @sa IV3DDevice::CreateImageView
	V3D_IMAGE_CAP_CUBE_ARRAY = 0x00000001,
	//! @brief ETC2 圧縮イメージをサポートします。<br>
	//! <br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8A1_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8A1_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8A8_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ETC2_R8G8B8A8_SRGB \endlink<br>
	//!< \link V3D_FORMAT_EAC_R11_UNORM \endlink<br>
	//!< \link V3D_FORMAT_EAC_R11_SNORM \endlink<br>
	//!< \link V3D_FORMAT_EAC_R11G11_UNORM \endlink<br>
	//!< \link V3D_FORMAT_EAC_R11G11_SNORM \endlink<br>
	V3D_IMAGE_CAP_COMPRESSION_ETC2 = 0x00000002,
	//! @brief ASTC LDR 圧縮イメージをサポートします。<br>
	//! <br>
	//!< \link V3D_FORMAT_ASTC_4X4_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_4X4_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_5X4_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_5X4_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_5X5_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_5X5_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_6X5_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_6X5_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_6X6_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_6X6_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X5_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X5_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X6_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X6_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X8_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_8X8_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X5_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X5_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X6_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X6_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X8_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X8_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X10_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_10X10_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_12X10_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_12X10_SRGB \endlink<br>
	//!< \link V3D_FORMAT_ASTC_12X12_UNORM \endlink<br>
	//!< \link V3D_FORMAT_ASTC_12X12_SRGB \endlink<br>
	V3D_IMAGE_CAP_COMPRESSION_ASTC_LDR = 0x00000004,
	//! @brief BC 圧縮イメージをサポートします。<br>
	//! <br>
	//!< \link V3D_FORMAT_BC1_RGB_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC1_RGB_SRGB \endlink<br>
	//!< \link V3D_FORMAT_BC1_RGBA_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC1_RGBA_SRGB \endlink<br>
	//!< \link V3D_FORMAT_BC2_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC2_SRGB \endlink<br>
	//!< \link V3D_FORMAT_BC3_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC3_SRGB \endlink<br>
	//!< \link V3D_FORMAT_BC4_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC4_SNORM \endlink<br>
	//!< \link V3D_FORMAT_BC5_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC5_SNORM \endlink<br>
	//!< \link V3D_FORMAT_BC6H_UFLOAT \endlink<br>
	//!< \link V3D_FORMAT_BC6H_SFLOAT \endlink<br>
	//!< \link V3D_FORMAT_BC7_UNORM \endlink<br>
	//!< \link V3D_FORMAT_BC7_SRGB \endlink<br>
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
	//! @brief テッセレーション制御、テッセレーション評価、およびジオメトリシェーダステージで PointSize ビルトイン装飾が使用できます。<br>
	//! この機能が有効になっていない場合は、PointSize で装飾されたメンバーを読み書きしないでください。<br>
	//! また、テッセレーションシェーダーまたはジオメトリシェーダーから書き込まれるポイントのサイズは 1.0 になります。<br>
	//! これは、シェーダモジュールがテッセレーション制御シェーダと評価シェーダの　TessellationPointSize 機能を宣言できるかどうか、
	//! またはシェーダモジュールがジオメトリシェーダの GeometryPointSize 機能を宣言できるかどうかを表します。<br>
	//! この機能をサポートする実装では、tessellationShader または geometryShader のいずれかの機能または両方をサポートする必要があります。
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
	//! この機能が有効になっていない場合 \link V3D_IMAGE_USAGE_STORAGE \endlink を含む使用方法でイメージを作成するときのサンプル数は \link V3D_SAMPLE_COUNT_1 \endlink にする必要があります。<br>
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
	//! この機能が有効になっていない場合、デスクリプタのタイプが \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER \endlink または \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC \endlink のリソースは、
	//! シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが UniformBufferArrayDynamicIndexing 機能を宣言できるかどうかを表します。<br>
	V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00001000,
	//! @brief シェーダコード内の動的に一様な整数式によってサンプラまたはサンプルイメージの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが \link V3D_DESCRIPTOR_TYPE_SAMPLER \endlink、\link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink、
	//! または \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink のリソースは、シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが SampledImageArrayDynamicIndexing 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00002000,
	//! @brief シェーダコード内の動的に一様な整数式によってストレージバッファの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER \endlink または \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC \endlink のリソースは、
	//! シェーダコードの配列に集約されたときに定数の積分式によってのみ索引付けする必要があります。<br>
	//! これは、シェーダモジュールが StorageBufferArrayDynamicIndexing 機能を宣言できるかどうかを表します。
	V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00004000,
	//! @brief シェーダコード内の動的に一様な整数式によって格納イメージの配列を索引付けできるかどうかを表します。<br>
	//! この機能が有効になっていない場合、デスクリプタのタイプが \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink のリソースは、
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
	//! @brief ポリゴンモードに \link V3D_POLYGON_MODE_LINE \endlink \link V3D_POLYGON_MODE_POINT \endlink を指定することができます。
	//! @sa V3DPipelineRasterizationDesc::polygonMode
	V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID = 0x00000002,
	//! @brief 深度のクランプをサポートします。
	//! @sa V3DPipelineRasterizationDesc::depthClampEnable
	V3D_RASTERIZER_CAP_DEPTH_CLAMP = 0x00000010,
	//! @brief 深度バイアスのクランプをサポートします。
	//! @sa V3DPipelineRasterizationDesc::depthBiasEnable
	//! @sa IV3DCommandBuffer::SetDepthBias
	V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP = 0x00000020,
};

//! @enum V3D_MULTISAMPLE_CAP_FLAG
//! @brief マルチサンプルの能力フラグ
enum V3D_MULTISAMPLE_CAP_FLAG : V3DFlags
{
	//! @brief サブパスのカラーアタッチメント、デプスステンシルアタッチメントのサンプル数が V3DPipelineMultisampleDesc::rasterizationSamples と同じ値である必要がないことを表します。<br>
	//! このフラグが指定されていなかった場合、サブパスの全てカラーアタッチメント、デプスステンシルアタッチメントのサンプル数は V3DPipelineMultisampleDesc::rasterizationSamples と同じ値でないといけません。
	V3D_MULTISAMPLE_CAP_VARIABLE_RATE = 0x00000001,
	//! @brief サンプル毎のシェーディングとマルチサンプルの補間をサポートします。<br>
	//! このフラグが指定されている場合は V3DPipelineMultisampleDesc::sampleShadingEnable に true に指定することができ、V3DPipelineMultisampleDesc::minSampleShading の値が有効になります。
	V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING = 0x00000002,
	//! @brief フラグメントシェーダから出力されたアルファ値を、固定小数点が表現できる最大値または 1.0 で置き換えることができます。<br>
	//! このフラグが指定されていた場合、V3DPipelineMultisampleDesc::alphaToOneEnable にtrue を指定することができます。
	V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE = 0x00000004,
};

//! @enum V3D_DEPTH_STENCIL_CAP_FLAG
//! @brief デプスステンシルの能力フラグ
enum V3D_DEPTH_STENCIL_CAP_FLAG : V3DFlags
{
	//! @brief 深度の境界テストをサポートします。
	//! @sa V3DPipelineDepthStencilDesc::depthBoundsTestEnable
	//! @sa IV3DCommandBuffer::SetDepthBounds
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
	//! \link V3D_BLEND_FACTOR_SRC1_COLOR \endlink<br>
	//! \link V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR \endlink<br>
	//! \link V3D_BLEND_FACTOR_SRC1_ALPHA \endlink<br>
	//! \link V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA \endlink<br>
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_DUAL_SRC = 0x00000002,
	//! @brief カラーブレンドでの論理オペレーターをサポートします。
	//! @sa V3DPipelineColorBlendDesc::logicOpEnable
	//! @sa V3DPipelineColorBlendDesc::logicOp
	V3D_COLOR_BLEND_CAP_LOGIC_OP = 0x00000004,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDeviceCaps
//! @brief デバイスの能力
struct V3DDeviceCaps
{
	V3DFlags flags; //!< 全般的な能力を表す \link V3D_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags queryFlags; //!< クエリに関するの能力を表す \link V3D_QUERY_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags imageFlags; //!< イメージに関するの能力を表す \link V3D_IMAGE_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags shaderFlags; //!< シェーダーに関するの能力を表す \link V3D_SHADER_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags rasterizerFlags; //!< ラスタライザーに関する能力を表す \link V3D_RASTERIZER_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags multisampleFlags; //!< マルチサンプルに関する能力を表す \link V3D_MULTISAMPLE_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags depthStencilFlags; //!< デプスステンシルに関する能力を表す \link V3D_DEPTH_STENCIL_CAP_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags colorBlendFlags; //!< カラーブレンドに関する能力を表す \link V3D_COLOR_BLEND_CAP_FLAG \endlink 列挙定数の組み合わせです。

	//! @brief 1Dイメージの幅の最大値です。
	uint32_t maxImageDimension1D;
	//! @brief 2Dイメージの幅、高さの最大値です。
	uint32_t maxImageDimension2D;
	//! @brief 3Dイメージの幅、高さ、深さの最大値です。
	uint32_t maxImageDimension3D;
	//! @brief キューブイメージの幅、高さ最大値です。
	uint32_t maxImageDimensionCube;
	//! @brief イメージの最大レイヤー数です。
	uint32_t maxImageArrayLayers;
	//! @brief テクセルバッファーの要素の最大数です。
	uint32_t maxTexelBufferElements;
	//! @brief ユニフォームバッファの最大サイズです。
	uint32_t maxUniformBufferSize;
	//! @brief ストレージバッファの最大サイズです。
	uint32_t maxStorageBufferSize;
	//! @brief IV3DCommandBuffer::PushConstant でプッシュできる定数の最大サイズをバイト単位で指定します。
	uint32_t maxPushConstantsSize;
	//! @brief デバイスが作成できるリソースメモリの最大数です。
	//! @sa IV3DDevice::AllocateResourceMemory
	uint32_t maxResourceMemoryCount;
	//! @brief IV3DDevice::CreateSampler で作成することのできるサンプラーの最大数です。
	uint32_t maxSamplerCreateCount;
	//! @brief エイリアシングのない同じメモリ内の隣接するバッファーまたは最適なイメージを結合することができるオフセットの粒度をバイト単位で指定します。
	uint64_t bufferImageGranularity;
	//! @brief パイプラインで設定できるデスクリプタセットの最大数です。
	uint32_t maxBoundDescriptorSets;

	//! @brief サンプラーの最大数です。
	uint32_t maxPerStageDescriptorSamplers;
	//! @brief ユニフォームバッファの最大数です。
	uint32_t maxPerStageDescriptorUniformBuffers;
	//! @brief ストレージバッファの最大数です。
	uint32_t maxPerStageDescriptorStorageBuffers;
	//! @brief サンプリングイメージの最大数です。
	uint32_t maxPerStageDescriptorSampledImages;
	//! @brief ストレージイメージの最大数です。
	uint32_t maxPerStageDescriptorStorageImages;
	//! @brief インプットアタッチメントの最大数です。
	uint32_t maxPerStageDescriptorInputAttachments;
	//! @brief パイプラインレイアウトでシェーダステージにアクセスできる最大リソース数です。<br>
	//! 以下のリソースに対して制限が適用されます。<br>
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC \endlink<br>
	//! <br>
	//! または<br>
	//! <br>
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	//! <br>
	//! またフラグメントシェーダステージの場合、フレームバッファのカラーアタッチメントにもこの制限が適用されます。
	uint32_t maxPerStageResources;

	//! @brief デスクリプタセットにバインドできるサンプラーの最大数です。
	uint32_t maxDescriptorSetSamplers;
	//! @brief デスクリプタセットにバインドできるユニフォームバッファの最大数です。
	uint32_t maxDescriptorSetUniformBuffers;
	//! @brief デスクリプタセットにバインドできるダイナミックユニフォームバッファの最大数です。
	uint32_t maxDescriptorSetUniformBuffersDynamic;
	//! @brief デスクリプタセットにバインドできるストレージバッファの最大数です。
	uint32_t maxDescriptorSetStorageBuffers;
	//! @brief デスクリプタセットにバインドできるダイナミックストレージバッファの最大数です。
	uint32_t maxDescriptorSetStorageBuffersDynamic;
	//! @brief デスクリプタセットにバインドできるサンプリングイメージの最大数です。
	uint32_t maxDescriptorSetSampledImages;
	//! @brief デスクリプタセットにバインドできるストレージイメージの最大数です。
	uint32_t maxDescriptorSetStorageImages;
	//! @brief デスクリプタセットにバインドできるインプットアタッチメントの最大数です。
	uint32_t maxDescriptorSetInputAttachments;

	//! @brief 頂点要素の最大数 ( V3DPipelineVertexInputDesc::elementCount ) です。
	uint32_t maxVertexInputElements;
	//! @brief 一度にバインドできる頂点バッファの最大数です。
	uint32_t maxVertexInputBindings;
	//! @brief 頂点要素の最大オフセットをバイト単位で指定します。
	uint32_t maxVertexInputElementOffset;
	//! @brief 頂点レイアウトの最大ストライド ( V3DPipelineVertexLayout::stride ) をバイト単位で指定します。
	uint32_t maxVertexInputBindingStride;
	//! @brief バーテックスシェーダーによって出力することのできる変数の成分の最大数です。
	uint32_t maxVertexOutputComponents;

	//! @brief テッセレーションプリミティブジェネレーターの最大テッセーレーション生成レベルです。
	uint32_t maxTessellationGenerationLevel;
	//! @brief テッセーレーション制御シェーダーおよびテッセーレーションプリミティブジェネレーターで処理可能な最大パッチサイズです。<br>
	//! グラフィックスパイプライン作成時に指定した V3DPipelineTessellationDesc 構造体の patchControlPoints の値と、シェーダーの OutputVertices 実行モードで指定された値は、この制限値でなければなりません。
	uint32_t maxTessellationPatchSize;
	//! @brief テッセレーション制御シェーダーのステージに入力できる頂点ごとの入力変数コンポーネントの最大数です。
	uint32_t maxTessellationControlPerVertexInputComponents;
	//! @brief テッセレーション制御シェーダのステージから出力できる頂点ごとの出力変数コンポーネントの最大数です。
	uint32_t maxTessellationControlPerVertexOutputComponents;
	//! @brief テッセレーション制御シェーダのステージから出力できるパッチごとの出力変数コンポーネントの最大数です。
	uint32_t maxTessellationControlPerPatchOutputComponents;
	//! @brief テッセレーション制御シェーダのステージから出力できる頂点ごと 及び パッチごとの出力変数の最大合計数です。
	uint32_t maxTessellationControlTotalOutputComponents;
	//! @brief テッセレーション評価シェーダのステージに入力できる頂点ごとの入力変数コンポーネントの最大数です。
	uint32_t maxTessellationEvaluationInputComponents;
	//! @brief テッセレーション評価シェーダのステージから出力できる頂点ごとの出力変数コンポーネントの最大数です。
	uint32_t maxTessellationEvaluationOutputComponents;

	//! @brief ジオメトリシェーダでサポートされる最大呼び出し回数です。<br>
	//! シェーダーモジュールの Invocations 実行モードで指定された値は、この制限値以下でなければなりません。
	uint32_t maxGeometryShaderInvocations;
	//! @brief ジオメトリシェーダのステージに入力できる入力変数のコンポーネントの最大数です。
	uint32_t maxGeometryInputComponents;
	//! @brief ジオメトリシェーダのステージから出力できる出力変数のコンポーネントの最大数です。
	uint32_t maxGeometryOutputComponents;
	//! @brief ジオメトリシェーダによって出力することができる最大頂数です。
	uint32_t maxGeometryOutputVertices;
	//! @brief ジオメトリシェーダのステージから出力できる、すべての頂点にわたるコンポーネントの最大数です。
	uint32_t maxGeometryTotalOutputComponents;

	//! @brief フラグメントシェーダーに入力することのできる変数の最大数です。
	uint32_t maxFragmentInputComponents;
	//! @brief フラグメントシェーダーが出力できるアタッチメントの最大数です。
	uint32_t maxFragmentOutputAttachments;
	//! @brief ブレンドが有効で、デュアルソースブレンドモードの1つが使用されているときに、フラグメントシェーダのステージによって書き込まれる出力アタッチメントの最大数です。
	uint32_t maxFragmentDualSrcAttachments;
	//! @brief フラグメントシェーダステージで使用できるストレージバッファ、ストレージイメージ、および出力バッファの総数です。
	uint32_t maxFragmentCombinedOutputResources;

	//! @brief コンピュートシェーダステージのシェーダーモジュールで WorkgroupLocal ストレージクラスで宣言されたすべての変数の最大合計ストレージサイズをバイト単位で指定します。
	uint32_t maxComputeSharedMemorySize;
	//! @brief 次元 ( x y z ) ごとにディスパッチできるグループ数です。
	uint32_t maxComputeDispatchGroupCount[3];
	//! @brief 単一のディスパッチグループで呼び出せるコンピュートシェーダーの最大回数です。
	uint32_t maxComputeDispatchGroupInvocations;
	//! @brief 次元 ( x y z ) ごとのディスパッチグループの最大サイズです。
	uint32_t maxComputeDispatchGroupSize[3];

	//! @brief フレームバッファ座標 Xfと Yf におけるサブピクセル精度を表すビット数です。
	uint32_t subPixelPrecisionBits;
	//! @brief 縮小および拡大フィルタに使用される画像の軸に沿った除算における精度を表すビット数です。<br>
	//! この値はイメージの各軸に沿った分割の実際の数です。<br>
	//! またフィルタリングされた結果を計算するときに、フィルタリングハードウェアがこれらの場所にスナップします。
	uint32_t subTexelPrecisionBits;
	//! @brief 各ミップレベルからフィルタリング結果への寄与を決定するときに、フェッチするための LOD 計算がスナップされる分割ビット数です。
	uint32_t mipmapPrecisionBits;

	//! @brief 描画の際にバインドするインデックスバッファに格納できるインデックスの最大値です。
	uint32_t maxDrawIndexedIndexValue;

	//! @brief LOD バイアスの絶対値です。<br>
	//! V3DSamplerDesc::mipLodBias の値は -maxSamplerLodBias ～ +maxSamplerLodBias の間にクランプします。
	float maxSamplerLodBias;
	//! @brief 異方性を表す V3DSamplerDesc::maxAnisotropy の最大値です。
	float maxSamplerAnisotropy;

	//! @brief ビューポート、シザリングの最大数です。
	uint32_t maxViewports;
	//! @brief ビューポートの最大サイズです。<br>
	//! maxViewportDimensions[2] のそれぞれの要素は [0]=幅、[1]=高さを表します。
	uint32_t maxViewportDimensions[2];
	//! @brief ビューポートの角が含まれていなければならない範囲です。<br>
	//! viewportBoundsRange[2] のそれぞれの要素は [0]=最小値、[1]=最大値を表します。
	float viewportBoundsRange[2];
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
	//! @brief InterpolateAtOffset 拡張命令に対する x および y オフセットが固定小数点値として丸められるサブピクセルの部分ビットの数です。
	uint32_t subPixelInterpolationOffsetBits;

	//! @brief フレームバッファのアタチメントの幅の最大です。
	uint32_t maxFrameBufferWidth;
	//! @brief フレームバッファのアタチメントの高さの最大です。
	uint32_t maxFrameBufferHeight;
	//! @brief フレームバッファのアタチメントのレイヤー数の最大です。
	uint32_t maxFrameBufferLayers;

	//! @brief フレームバッファーのカラーアタッチメントでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags framebufferColorSampleCounts;
	//! @brief フレームバッファーのデプスアタッチメントでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags framebufferDepthSampleCounts;
	//! @brief フレームバッファーのステンシルアタッチメントでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags framebufferStencilSampleCounts;
	//! @brief アタッチメントのないフレームバッファーでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags framebufferNoAttachmentsSampleCounts;

	//! @brief 非整数カラーフォーマットで作成された 2D イメージでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags sampledImageColorSampleCounts;
	//! @brief 整数カラーフォーマットで作成された 2D イメージでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags sampledImageIntegerSampleCounts;
	//! @brief デプスフォーマットで作成された 2D イメージでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags sampledImageDepthSampleCounts;
	//! @brief ステンシルフォーマットで作成された 2D イメージでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags sampledImageStencilSampleCounts;
	//! @brief ストレージとして作成された 2D イメージでサポートされるサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
	V3DFlags storageImageSampleCounts;

	//! @brief サブパスで使用することのできるカラーアタッチメントの最大数です。
	uint32_t maxColorAttachments;
	//! @brief SampleMask ビルトイン修飾で宣言された変数の配列の要素の最大数です。
	uint32_t maxSampleMaskWords;

	//! @brief すべてのグラフィックスとコンピュートキューのタイムスタンプをサポートします。
	bool timestampComputeAndGraphics;
	//! @brief タイムスタンプクエリをイクンリメントするために必要な時間をナノ秒単位で指定します。
	float timestampPeriod;

	//! @brief シェーダステージで使用できる最大クリップ距離の数です。<br>
	//! シェーダモジュールの ClipDistance 組み込み修飾で宣言された配列のサイズは、この制限値以下でなければなりません。
	uint32_t maxClipDistances;
	//! @brief シェーダステージで使用できる最大の間引き数です。<br>
	//! シェーダモジュールの CullDistance 組み込み装飾で宣言された配列のサイズは、この制限値以下でなければなりません。
	uint32_t maxCullDistances;
	//! @brief シェーダステージで使用できるクリップとカリング距離の合計最大数です。<br>
	//! シェーダモジュールの ClipDistance および CullDistance ビルトイン装飾で宣言された配列のペアの合計は、この制限値以下でなければなりません。
	uint32_t maxCombinedClipAndCullDistances;

	//! @brief サポートされているポイントのサイズの範囲です。<br>
	//! PointSize で装飾された変数に書き込まれた値は、この pointSizeRange[0] ～ pointSizeRange[1] の間にクランプされます。
	float pointSizeRange[2];
	//! @brief サポートされるポイントサイズの粒度です。<br>
	//! pointSizeRange によって定義された範囲内のすべてのポイントサイズがサポートされるわけではありません。<br>
	//! この制限は、連続してサポートされるポイントサイズ間の粒度(または増分) を指定します。
	float pointSizeGranularity;

	//! @brief デバイス作成時に指定するキューにスケージューリングとして割り当てることができる個別の優先順位の数です。
	uint32_t discreteQueuePriorities;
	//! @brief マルチサンプリングで記述されたラスタライゼーションが標準のサンプル位置を使用します。<br>
	//! true に設定されている場合、実装は記述されたサンプルの場所を使用します。<br>
	//! false に設定されている場合、実装は異なるサンプル位置を使用することがあります。
	bool standardSampleLocations;
	//! @brief IV3DCommandBuffer::CopyBufferToImage および IV3DCommandBuffer::CopyImageToBuffer の最適なバッファオフセットアラインメントです。<br>
	//! テクセルごとのアラインメントが適用されますが、アプリケーションは最適なパフォーマンスと消費電力を得るために最適なアライメントを使用する必要があります。
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
	V3DFlags queueFlags; //!< キューの操作を表す \link V3D_QUEUE_FLAG \endlink 列挙定数の組み合わせです。
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
	V3DFlags sampleCountFlags; //!< イメージのサンプル数を表す \link V3D_SAMPLE_COUNT_FLAG \endlink 列挙定数の組み合わせです。
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
	//! @param[in] family キューファミリーのインデックスです。
	//! @return キューファミリの記述を返します。
	virtual const V3DDeviceQueueFamilyDesc& GetQueueFamilyDesc(uint32_t family) const = 0;
	//! @brief キューを取得します。
	//! @param[in] family キューファミリーのインデックスです。
	//! @param[in] queue キューのインデックスです。
	//! @param[out] ppQueue 取得したキューを渡す IV3DQueue インターフェースのポインタのアドレスです。
	virtual void GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue) = 0;

	//! @brief 指定したイメージのフォーマットのプロパティを取得します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] type イメージのタイプです。
	//! @param[in] tiling イメージのタイリングです。
	//! @param[in] usageFlags 使用方法を表す \link V3D_IMAGE_USAGE \endlink 列挙定数の組み合わせです。
	//! @param[out] pDesc 取得したイメージフォーマットの記述を格納する V3DImageFormatDesc 構造体のポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @note pProperty に nullptr を指定することでフォーマットのサポート状況だけを確認することができます。
	virtual V3D_RESULT GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc) = 0;

	//! @brief バッファーとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format バッファーのフォーマットです。
	//! @param[in] featureFlags 確認する機能を表す \link V3D_BUFFER_FORMAT_FEATURE_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief イメージとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] featureFlags 確認する機能を表す \link V3D_IMAGE_FORMAT_FEATURE_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief イメージとして使用するフォーマットが指定した機能をサポートしているかどうかを確認します。
	//! @param[in] format イメージのフォーマットです。
	//! @param[in] tiling イメージのタイリングです。
	//! @param[in] featureFlags 確認する機能を表す \link V3D_IMAGE_FORMAT_FEATURE_FLAG \endlink 列挙定数の組み合わせです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const = 0;

	//! @brief コマンドプールーを作成します。
	//! @param[in] desc コマンドプールの記述です。
	//! @param[out] ppCommandPool 作成したコマンドプールを渡す IV3DCommandPool インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool) = 0;
	//! @brief コマンドバッファーを作成します。
	//! @param[in] pPool コマンドプールです。
	//! @param[in] bufferType コマンドバッファーのタイプです。
	//! @param[out] ppCommandBuffer 作成したコマンドバッファーを渡す IV3DCommandBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandBuffer(IV3DCommandPool* pPool, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer) = 0;
	//! @brief コマンドバッファーを作成します。
	//! @param[in] poolDesc コマンドプールの記述です。
	//! @param[in] bufferType コマンドバッファーのタイプです。
	//! @param[out] ppCommandBuffer 作成したコマンドバッファーを渡す IV3DCommandBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note コマンドプールも同時に作成します。
	virtual V3D_RESULT CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer) = 0;

	//! @brief スワップチェインを作成します。
	//! @param[in] swapChainDesc スワップチェインの記述です。
	//! @param[in] swapChainCallbacks スワップチェインのコールバックです。
	//! @param[out] ppSwapChain 作成したスワップチェインを渡す IV3DSwapChain インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	//! @note
	//! V3DSwapChainDesc 構造体の imageFormat で指定したフォーマットが見つからなかった場合は、自動でフォーマットを決定します。( \link V3D_FORMAT_UNDEFINED \endlink が指定できるということを表します )<br>
	//! IV3DSwapChain::GetDesc で取得できる記述は、作成時に指定したものとは異なる場合があるため、必要に応じて取得しなおしてください。<br>
	//! また IV3DSwapChain::GetImage で取得できるスワップチェイン作成直後のイメージは以下のような設定になっています。<br>
	//! <table>
	//! <tr><td>レイアウト</td><td>\link V3D_IMAGE_LAYOUT_UNDEFINED \endlink</td></tr>
	//! <tr><td>使用方法</td><td>\link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink<br>\link V3D_IMAGE_USAGE_TRANSFER_DST \endlink<br>\link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink</td></tr>
	//! </table>
	virtual V3D_RESULT CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain) = 0;

	//! @brief フェンスを作成します。
	//! @param[out] ppFence 作成したフェンスを渡す IV3DFence インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFence(IV3DFence** ppFence) = 0;

	//! @brief クエリプールを作成します。
	//! @param[in] desc クエリプールの記述です。
	//! @param[out] ppQueryPool 作成したクエリプールを渡す IV3DQueryPool インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool) = 0;

	//! @brief サンプラーを作成します。
	//! @param[in] desc サンプラーの記述です。
	//! @param[out] ppSampler 作成したサンプラーを渡す IV3DSampler インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler) = 0;

	//! @brief シェーダーモジュールを作成します。
	//! @param[in] codeSize コードのサイズです。
	//! @param[in] pCode シェーダのコードです。
	//! @param[out] ppShaderModule 作成したシェーダーモジュールを渡す IV3DShaderModule インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule) = 0;

	//! @brief 指定したメモリ特性をリソースがサポートしているかどうかを確認します。
	//! @param[in] memoryPropertyFlags リソースのメモリ特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] pResource メモリ特性を確認するリソースを表す IV3DResource インターフェースのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::AllocateResourceMemoryAndBind
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const = 0;
	//! @brief 指定したメモリ特性をリソースがサポートしているかどうかを確認します。
	//! @param[in] memoryPropertyFlags リソースのメモリ特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] resourceCount メモリ特性を確認するリソースの数です。
	//! @param[in] ppResources resourceCount の値の数の要素を持つ IV3DResource インターフェースのポインタの配列です。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::AllocateResourceMemoryAndBind
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const = 0;

	//! @brief リソースメモリを確保します。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] memorySize 確保するメモリのサイズをバイト単位で指定します。
	//! @param[out] ppResourceMemory 確保したリソースメモリを渡す IV3DResourceMemory インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! IV3DDevice::CreateBuffer または IV3DDevice::CreateImage によって作成された直後のリソースのメモリはまだ確保されておらず、使用できない状態になっています。<br>
	//! そのため IV3DDevice::AllocateResourceMemory でメモリを確保した後 IV3DDevice::BindResourceMemory でリソースをメモリにバインドする必要があります。<br>
	//! また確保することのできるリソースメモリは V3DDeviceCaps 構造体の maxResourceMemoryCount の値の数に制限されます。<br>
	//! <ul>
	//!   <li>メモリ特性</li><br>
	//!   memoryPropertyFlags に指定できるフラグの組み合わせは以下のものに限定されます。<br>
	//!   また、これらの組み合わせが必ずしも使えるわけではないため、リソースメモリを確保する前に IV3DDevice::CheckResourceMemoryProperty で使用可能かどうかを調べてください。<br>
	//!   <br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink ※<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink ※<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink ※<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink<br>
	//!   <br>
	//!   ※ \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink の組み合わせは、ほぼ使えないと考えたほうがよさそうです。<br>
	//!   <br>
	//!   <li>メモリサイズ</li><br>
	//!   リソースのメモリサイズは IV3DResource::GetResourceDesc で取得できる V3DResourceDesc 構造体のメンバである memorySize で確認することができます。<br>
	//! </ul>
	virtual V3D_RESULT AllocateResourceMemory(V3DFlags memoryPropertyFlags, uint64_t memorySize, IV3DResourceMemory** ppResourceMemory) = 0;
	//! @brief リソースをメモリにバインドします。
	//! @param[in] pResourceMemory バインド先のメモリです。
	//! @param[in] pResource バインドするリソースです。
	//! @param[in] memoryOffset バインドするメモリのオフセットをバイト単位で指定します。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! リソースをメモリにバインドすることによってメモリへのアクセスができるようになります。<br>
	//! また一度メモリにバインドするとリソースが解放されるまでバインドを解除することはできません。<br>
	//! <ul>
	//!   <li>メモリのオフセット</li><br>
	//!   memoryOffset はリソースの種類によってアライメントの制限があるため、以下のメンバの整数倍の値を指定する必要があります。<br>
	//!   <br>
	//!   <ul>
	//!     <li>テクセルバッファー</li> \link V3DDeviceCaps::minTexelBufferOffsetAlignment \endlink<br>
	//!     <li>ユニフォームバッファー</li> \link V3DDeviceCaps::minUniformBufferOffsetAlignment \endlink<br>
	//!     <li>ストレージバッファー</li> \link V3DDeviceCaps::minStorageBufferOffsetAlignment \endlink<br>
	//!   </ul>
	//!   <br>
	//!   さらにホストからアクセスできるメモリ \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink にもアライメントの制限があるため \link V3DDeviceCaps::minMemoryMapAlignment \endlink も考慮する必要があります。<br>
	//! </ul>
	virtual V3D_RESULT BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset) = 0;

	//! @brief リソースのメモリを確保し、バインドします。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] pResource バインドするリソースです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, IV3DResource* pResource) = 0;
	//! @brief 複数のリソースを一つのメモリとして確保し、バインドします。
	//! @param[in] memoryPropertyFlags リソースメモリの特性を表す \link V3D_MEMORY_PROPERTY_FLAG \endlink 列挙定数の組み合わせです。
	//! @param[in] resourceCount バインドするリソースの数です。
	//! @param[in] ppResources バインドするリソースを表す IV3DResource インターフェースのポインタの配列です。<br>
	//! 配列の要素の数は resourceCount の値である必要があります。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) = 0;

	//! @brief バッファーを作成します。
	//! @param[in] desc バッファーの記述です。
	//! @param[in] ppBuffer 作成したバッファーを渡す IV3DBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! バッファーに複数のサブリソースを含める場合は、サブリソースが配置されるメモリオフセットのアライメントに注意してバッファーのサイズを決定する必要があります。<br>
	//! サブリソースのアライメントは V3DDeviceCaps に記述されており、以下のようになります。<br>
	//! <br>
	//! <table>
	//! <tr><td>\link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink <br> \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>\link V3D_BUFFER_USAGE_UNIFORM \endlink</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>\link V3D_BUFFER_USAGE_STORAGE \endlink</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! また、ホストからのアクセス ( IV3DResource::Map ) がある場合は minMemoryMapAlignment も考慮する必要があります。
	virtual V3D_RESULT CreateBuffer(const V3DBufferDesc& desc, IV3DBuffer** ppBuffer) = 0;
	//! @brief イメージを作成します。
	//! @param[in] imageDesc イメージの記述です。
	//! @param[in] initialLayout イメージの初期レイアウトです。
	//! @param[out] ppImage 作成したイメージを渡す IV3DImage インターフェースのポインタです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note イメージの初期レイアウトとして指定できるのは \link V3D_IMAGE_LAYOUT_UNDEFINED \endlink または \link V3D_IMAGE_LAYOUT_PREINITIALIZED \endlink のみになります。<br>
	virtual V3D_RESULT CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage) = 0;

	//! @brief バッファービューを作成します。
	//! @param[in] pBuffer アクセス先のバッファーです。
	//! @param[in] desc バッファービューの記述です。
	//! @param[out] ppBufferView 作成したバッファービューを表す IV3DBufferView インターフェースのポインタのアドレスです。
	//! @retval D3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! アクセス先になるバッファーは使用法に \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink または \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink のいずれかが指定されている必要があります。<br>
	//! また、アクセス先になるバッファーは事前にメモリを確保しておく必要があります。<br>
	//! @sa IV3DDevice::CreateBuffer
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT CreateBufferView(IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, IV3DBufferView** ppBufferView) = 0;
	//! @brief イメージビューを作成します。
	//! @param[in] pImage アクセス先のイメージです。
	//! @param[in] desc イメージビューの記述です。
	//! @param[out] ppImageView 作成したイメージビューを渡す IV3DImageView インターフェースのポインタのアドレスです。
	//! @retval D3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! アクセス先のイメージは事前にメモリを確保しておく必要があります。
	//! @sa IV3DDevice::CreateImage
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView) = 0;

	//! @brief レンダーパスを作成します。
	//! @param[in] attachmentCount アタッチメントの数です。
	//! @param[in] pAttachments attachmentCount の値の数の要素を持つ V3DAttachmentDesc 構造体の配列です。
	//! @param[in] subpassCount サブパスの数です。
	//! @param[in] pSubpasses subpassCount の値の数の要素を持つ V3DSubpassDesc 構造体の配列です。
	//! @param[in] subpassDependencyCount サブパスの依存性の数です。
	//! @param[in] pSubpassDependencies subpassDependencyCount の値の数の要素を持つ V3DSubpassDependencyDesc 構造体の配列です。
	//! @param[out] ppRenderPass 作成したレンダーパスを表す IV3DRenderPass インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateRenderPass(
		uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments,
		uint32_t subpassCount, const V3DSubpassDesc* pSubpasses,
		uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies,
		IV3DRenderPass** ppRenderPass) = 0;

	//! @brief フレームバッファを作成します。
	//! @param[in] pRenderPass レンダーパス
	//! @param[in] attachmentCount アタッチメントの数です。
	//! @param[in] ppAttachments attachmentCount の値の数の要素を持つ IV3DImageView インターフェースのポインタの配列です。
	//! @param[out] ppFrameBuffer 作成したフレームバッファを渡す IV3DFrameBuffer インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFrameBuffer(IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer) = 0;

	//! @brief デスクリプタセットレイアウトを作成します。
	//! @param[in] descriptorCount デスクリプタの数です。
	//! @param[in] pDescriptors descriptorCount の値の数の要素を持つ V3DDescriptorDesc 構造体の配列です。
	//! @param[in] poolSize デスクリプタセットのプールの初期サイズです。
	//! @param[in] poolResizeStep デスクリプタセットのプールのリサイズステップです。
	//! @param[out] ppDescriptorSetLayout 作成したデスクリプタセットレイアウトを渡す IV3DDescriptorSetLayout インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateDescriptorSetLayout(
		uint32_t descriptorCount, const V3DDescriptorDesc* pDescriptors,
		uint32_t poolSize, uint32_t poolResizeStep,
		IV3DDescriptorSetLayout** ppDescriptorSetLayout) = 0;

	//! @brief デスクリプタセットを作成します。
	//! @param[in] pLayout デスクリプタセットのレイアウトのポインタです。
	//! @param[out] ppDescriptorSet 作成したデスクリプタセットを渡す IV3DDescriptorSet インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet) = 0;

	//! @brief パイプラインレイアウトを作成します。
	//! @param[in] constantCount 定数の数です。
	//! @param[in] pConstants constantCount の値の数だけ要素を持つ V3DConstantDesc 構造体の配列です。
	//! @param[in] descriptorSetLayoutCount デスクリプタセットレイアウトの数です。
	//! @param[in] ppDescriptorSetLayouts デスクリプタレイアウトを表す descriptorSetLayoutCount の値の数だけ要素を持つ IV3DDescriptorSetLayout インターフェースのポインタの配列です。
	//! @param[out] ppPipelineLayout 作成したパイプラインレイアウトを表す IV3DPipelineLayout インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreatePipelineLayout(
		uint32_t constantCount, V3DConstantDesc* pConstants,
		uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts,
		IV3DPipelineLayout** ppPipelineLayout) = 0;

	//! @brief グラフィックスパイプラインを作成します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] pipelineDesc グラフィックスパイプラインの記述です。
	//! @param[out] ppPipeline 作成したグラフィックスパイプラインを渡す IV3DPipeline インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline) = 0;

	//! @brief コンピュートパイプラインを作成します。
	//! @param[in] pPipelineLayout パイプラインレイアウトです。
	//! @param[in] pipelineDesc コンピュートパイプラインの記述です。
	//! @param[out] ppPipeline 作成したコンピュートパイプラインを渡す IV3DPipeline インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline) = 0;

	//! @brief デバイスがアイドル状態になるまで待機します
	//! @retval V3D_OK
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
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

//! @enum V3D_LAYER_TYPE
//! @brief レイヤーのタイプ
enum V3D_LAYER_TYPE : uint8_t
{
	//! @brief VK_LAYER_LUNARG_standard_validation を使用します。<br>
	//! デバッグ時は通常これを使用してください。
	V3D_LAYER_STANDARD_VALIDATION = 0,
	//! @brief 最適なレイヤーを使用します。<br>
	//! また NVIDIA の Nsight を使用したい場合はこのレイヤーを使用してください。
	V3D_LAYER_OPTIMUS = 1,
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
	V3DFlags flags; //!< ログのフラグを表す \link V3D_LOG_FLAG \endlink 列挙定数の組み合わせです。
	PV3DLogFunction pFunction; //!< ログを出力する関数のポインタです。
	void* pUserData; //!< コールバック関数に渡されるユーザーデータです。
};

//! @struct V3DInstanceDesc
//! @brief インスタンスの記述
struct V3DInstanceDesc
{
	V3D_LAYER_TYPE layerType; //!< レイヤーのタイプを表す \link V3D_LAYER_TYPE \endlink 列挙定数のいずれかを指定します。
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
	//! @brief レイヤーのタイプを取得します。
	//! @return レイヤーのタイプを返します。
	virtual V3D_LAYER_TYPE GetLayerType() const = 0;

	//! @brief アダプタの数を取得します。
	//! @return アダプタの数を返します。
	virtual uint32_t GetAdapterCount() const = 0;
	//! @brief アダプタを取得します。
	//! @param[in] adapterIndex アダプタのインデックスです。
	//! @param[out] ppAdapter 取得したアダプタを渡す IV3DAdapter インターフェースのポインタのアドレスです。
	virtual void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) = 0;

	//! @brief デバイスを作成します。
	//! @param[in] pAdapter アダプタです。
	//! @param[out] ppDevice 作成したデバイスを渡す IV3DDevice インターフェースのポインタのアドレスです。
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT CreateDevice(IV3DAdapter* pAdapter, IV3DDevice** ppDevice) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DInstance() {}
	//! @endcond
};

//! @}

//! @addtogroup v3d_func_group
//! @{

//! @brief インスタンスを作成します。
//! @param[in] instanceDesc インスタンスの記述を表す V3DInstanceDesc 構造体です。
//! @param[out] ppInstance 作成したインスタンスを渡す IV3DInstance インターフェースのポインタのアドレスです。
//! @retval V3D_OK
//! @retval V3D_ERROR_FAIL
//! @retval V3D_ERROR_INVALID_ARGUMENT
//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
V3D_DLL_API V3D_RESULT V3DCreateInstance(const V3DInstanceDesc& instanceDesc, IV3DInstance** ppInstance);

//! @}
