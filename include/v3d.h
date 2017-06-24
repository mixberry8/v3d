#pragma once

//! @file v3d.h
//! @brief v3d

//! @defgroup v3d_interface_group �C���^�[�t�F�[�X
//! @defgroup v3d_callback_group �R�[���o�b�N
//! @defgroup v3d_func_group �֐�
//! @defgroup v3d_struct_group �\����
//! @defgroup v3d_enum_group �񋓌^
//! @defgroup v3d_constant_group �萔

#include <cstdint>
#include <Windows.h>

// ----------------------------------------------------------------------------------------------------
// �萔
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_constant_group
//! @{

#ifdef V3D_DLL
#ifdef V3D_DLL_EXPORTS
//! @def V3D_DLL_API
//! @brief v3d �� API ��`
#define V3D_DLL_API __declspec( dllexport )
#else // V3D_DLL_EXPORTS
//! @def V3D_DLL_API
//! @brief v3d �� API ��`
#define V3D_DLL_API __declspec( dllimport )
#endif // V3D_DLL_EXPORTS
#else //V3D_DLL
//! @def V3D_DLL_API
//! @brief v3d �� DLL ��`
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
//! @brief v3d �̃R�[���o�b�N��`
#define V3D_CALLBACK __stdcall

//! @def V3D_INFINITE
//! @brief �I�u�W�F�N�g���V�O�i����ԂɂȂ�̂�ҋ@���邱�Ƃ��ł���ő厞�Ԃł��B
#define V3D_INFINITE UINT64_MAX

//! @def V3D_QUEUE_FAMILY_IGNORED
//! @brief �L���[�t�@�~���[�̃��\�[�X�̏��L���̕ύX���Ȃ����Ƃ�\���܂��B
#define V3D_QUEUE_FAMILY_IGNORED (~0U)
//! @def V3D_ATTACHMENT_UNUSED
//! @brief ���O����A�^�b�`�����g��\���܂��B
#define V3D_ATTACHMENT_UNUSED (~0U)
//! @def V3D_SUBPASS_EXTERNAL
//! @brief �T�u�p�X�O��\���܂��B
#define V3D_SUBPASS_EXTERNAL (~0U)

//! @}

// ----------------------------------------------------------------------------------------------------
// �^
// ----------------------------------------------------------------------------------------------------

//! @typedef V3DFlags
//! @brief v3d �̃t���O�̌^
typedef uint32_t V3DFlags;

// ----------------------------------------------------------------------------------------------------
// �񋓌^
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_RESULT
//! @brief v3d �̃��\�b�h���Ԃ����Ƃ��ł���l
enum V3D_RESULT : int8_t
{
	V3D_OK = 0, //!< �������܂����B
	V3D_NOT_READY = 1, //!< �����͊������Ă��܂���B
	V3D_TIMEOUT = 2, //!< �^�C���A�E�g���܂����B
	V3D_ERROR_FAIL = -1, //!< ���s���܂����B
	V3D_ERROR_INVALID_ARGUMENT = -2, //!< �s���Ȉ������n����܂����B
	V3D_ERROR_NOT_FOUND = -3, //!< ������܂���B
	V3D_ERROR_NOT_SUPPORTED = -4, //!< �T�|�[�g����Ă��܂���B
	V3D_ERROR_OUT_OF_HOST_MEMORY = -5, //!< �z�X�g���������s�����Ă��܂��B
	V3D_ERROR_OUT_OF_DEVICE_MEMORY = -6, //!< �f�o�C�X���������s�����Ă��܂��B
	V3D_ERROR_DEVICE_LOST = -7, //!< �f�o�C�X�����X�g���܂����B
};

//! @enum V3D_FORMAT
//! @brief v3d �Ŏg�p�ł���t�H�[�}�b�g
enum V3D_FORMAT : uint8_t
{
	V3D_FORMAT_UNDEFINED = 0, //!< �t�H�[�}�b�g�͕s���ł��B
	V3D_FORMAT_R4G4_UNORM = 1, //!< 2 �����A8 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R4G4B4A4_UNORM = 2, //!<  4 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B4G4R4A4_UNORM = 3, //!< 4 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R5G6B5_UNORM = 4, //!< 3 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B5G6R5_UNORM = 5, //!< 3 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R5G5B5A1_UNORM = 6, //!< 4 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B5G5R5A1_UNORM = 7, //!< 4 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A1R5G5B5_UNORM = 8, //!<  4 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_UNORM = 9, //!< 1 �����A8 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_SNORM = 10, //!< 1 �����A8 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_USCALED = 11, //!< 1 �����A8 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_SSCALED = 12, //!< 1 �����A8 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_UINT = 13, //!< 1 �����A8 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_SINT = 14, //!< 1 �����A8 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8_SRGB = 15, //!< 1 �����A8 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_UNORM = 16, //!< 2 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_SNORM = 17, //!< 2 �����A16 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_USCALED = 18, //!< 2 �����A16 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_SSCALED = 19, //!< 2 �����A16 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_UINT = 20, //!< 2 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_SINT = 21, //!< 2 �����A16 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8_SRGB = 22, //!< 2 �����A16 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_UNORM = 23, //!< 3 �����A24 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_SNORM = 24, //!< 3 �����A24 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_USCALED = 25, //!< 3 �����A24 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_SSCALED = 26, //!< 3 �����A24 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_UINT = 27, //!< 3 �����A24 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_SINT = 28, //!< 3 �����A24 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8_SRGB = 29, //!< 3 �����A24 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_UNORM = 30, //!< 3 �����A24 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_SNORM = 31, //!< 3 �����A24 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_USCALED = 32, //!< 3 �����A24 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_SSCALED = 33, //!< 3 �����A24 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_UINT = 34, //!< 3 �����A24 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_SINT = 35, //!< 3 �����A24 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8_SRGB = 36, //!< 3 �����A24 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_UNORM = 37, //!< 4 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_SNORM = 38, //!< 4 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_USCALED = 39, //!< 4 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_SSCALED = 40, //!< 4 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_UINT = 41, //!< 4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_SINT = 42, //!< 4 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R8G8B8A8_SRGB = 43, //!< 4 �����A32 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_UNORM = 44, //!< 4 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_SNORM = 45, //!< 4 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_USCALED = 46, //!< 4 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_SSCALED = 47, //!< 4 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_UINT = 48, //!< 4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_SINT = 49, //!< 4 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_B8G8R8A8_SRGB = 50, //!< 4 �����A32 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_UNORM = 51, //!< 4 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_SNORM = 52, //!< 4 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_USCALED = 53, //!< 4 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_SSCALED = 54, //!< 4 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_UINT = 55, //!< 4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_SINT = 56, //!< 4 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A8B8G8R8_SRGB = 57, //!< 4 �����A32 �r�b�g�����Ȃ� sRGB ���K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_UNORM = 58, //!< 4 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_SNORM = 59, //!< 4 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_USCALED = 60, //!< 4 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_SSCALED = 61, //!< 4 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_UINT = 62, //!< 4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2R10G10B10_SINT = 63, //!< 4 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_UNORM = 64, //!< 4 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_SNORM = 65, //!< 4 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_USCALED = 66, //!< 4 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_SSCALED = 67, //!< 4 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_UINT = 68, //!< 4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_A2B10G10R10_SINT = 69, //!< 4 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_UNORM = 70, //!< 1 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_SNORM = 71, //!< 1 �����A16 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_USCALED = 72, //!< 1 �����A16 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_SSCALED = 73, //!< 1 �����A16 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_UINT = 74, //!< 1 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_SINT = 75, //!< 1 �����A16 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16_SFLOAT = 76, //!< 1 �����A16 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_UNORM = 77, //!< 2 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_SNORM = 78, //!< 2 �����A32 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_USCALED = 79, //!< 2 �����A32 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_SSCALED = 80, //!< 2 �����A32 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_UINT = 81, //!< 2 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_SINT = 82, //!< 2 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16_SFLOAT = 83, //!< 2 �����A32 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_UNORM = 84, //!< 3 �����A48 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_SNORM = 85, //!< 3 �����A48 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_USCALED = 86, //!< 3 �����A48 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_SSCALED = 87, //!< 3 �����A48 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_UINT = 88, //!< 3 �����A48 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_SINT = 89, //!< 3 �����A48 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16_SFLOAT = 90, //!< 3 �����A48 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_UNORM = 91, //!< 4 �����A64 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_SNORM = 92, //!< 4 �����A64 �r�b�g�����t�����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_USCALED = 93, //!< 4 �����A64 �r�b�g�����Ȃ��X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_SSCALED = 94, //!< 4 �����A64 �r�b�g�����t���X�P�[�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_UINT = 95, //!< 4 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_SINT = 96, //!< 4 �����A64 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R16G16B16A16_SFLOAT = 97, //!< 4 �����A64 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32_UINT = 98, //!< 1 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32_SINT = 99, //!< 1 �����A32 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32_SFLOAT = 100, //!< 1 �����A32 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32_UINT = 101, //!< 2 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32_SINT = 102, //!< 2 �����A64 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32_SFLOAT = 103, //!< 2 �����A64 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32_UINT = 104, //!< 3 �����A96 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32_SINT = 105, //!< 3 �����A96 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32_SFLOAT = 106, //!< 3 �����A96 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32A32_UINT = 107, //!< 4 �����A128 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32A32_SINT = 108, //!< 4 �����A128 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R32G32B32A32_SFLOAT = 109, //!< 4 �����A128 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64_UINT = 110, //!< 1 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64_SINT = 111, //!< 1 �����A64 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64_SFLOAT = 112, //!< 1 �����A64 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64_UINT = 113, //!< 2 �����A128 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64_SINT = 114, //!< 2 �����A128 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64_SFLOAT = 115, //!< 2 �����A128 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64_UINT = 116, //!< 3 �����A192 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64_SINT = 117, //!< 3 �����A192 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64_SFLOAT = 118, //!< 3 �����A192 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64A64_UINT = 119, //!< 4 �����A256 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64A64_SINT = 120, //!< 4 �����A256 �r�b�g�����t�������t�H�[�}�b�g�ł��B
	V3D_FORMAT_R64G64B64A64_SFLOAT = 121, //!< 4 �����A256 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_B10G11R11_UFLOAT = 122, //!< 3 �����A32 �r�b�g�����Ȃ����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_E5B9G9R9_UFLOAT = 123, //!< 4 �����A���L�w�����܂� 32 �r�b�g�����Ȃ����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_D16_UNORM = 124, //!< 1 �����A�[�x 16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_X8_D24_UNORM = 125, //!< 2 �����A���g�p 8 �r�b�g�A�[�x 24 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_D32_SFLOAT = 126, //!< 1 �����A�[�x 32 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
	V3D_FORMAT_S8_UINT = 127, //!< 1 �����A8 �r�b�g�����Ȃ������t�H�[�}�b�g�ł��B
	V3D_FORMAT_D16_UNORM_S8_UINT = 128, //!< �f�v�X�� 16 �r�b�g�����Ȃ����K�������A�X�e���V���� 8 �r�b�g���������������g�p����t�H�[�}�b�g�ł��B
	V3D_FORMAT_D24_UNORM_S8_UINT = 129, //!< �f�v�X�� 24 �r�b�g�����Ȃ����K�������A�X�e���V���� 8 �r�b�g���������������g�p����t�H�[�}�b�g�ł��B
	V3D_FORMAT_D32_SFLOAT_S8_UINT = 130, //!< �f�v�X�� 32 �r�b�g�����t�����������_�A�X�e���V���� 8 �r�b�g���������������g�p����t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC1_RGB_UNORM = 131, //!< 3 �����A64 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC1_RGB_SRGB = 132, //!< 3 �����A64 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC1_RGBA_UNORM = 133, //!< 4 �����A64 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC1_RGBA_SRGB = 134, //!< 4 �����A64 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC2_UNORM = 135, //!< 4 �����A128 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC2_SRGB = 136, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC3_UNORM = 137, //!< 4 �����A128 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC3_SRGB = 138, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC4_UNORM = 139, //!< 1 �����A64 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC4_SNORM = 140, //!< 1 �����A64 �r�b�g�����t�����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC5_UNORM = 141, //!< 4 �����A128 �r�b�g�����Ȃ����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC5_SNORM = 142, //!< 4 �����A128 �r�b�g�����t�����K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC6H_UFLOAT = 143, //!< 3 �����A128 �r�b�g�����Ȃ����������_ 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC6H_SFLOAT = 144, //!< 3 �����A128 �r�b�g�����t�����������_ 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC7_UNORM = 145, //!< 4 �����A128 �r�b�g�����Ȃ����������_ 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_BC7_SRGB = 146, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8_UNORM = 147, //!< 3 �����A64 �r�b�g�����Ȃ����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8_SRGB = 148, //!< 3 �����A64 �r�b�g�����Ȃ� sRGB ���K�� 4x4 �u���b�NETC2 ���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8A1_UNORM = 149, //!< 4 �����A64 �r�b�g�����Ȃ����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8A1_SRGB = 150, //!< 4 �����A64 �r�b�g�����Ȃ� sRGB ���K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8A8_UNORM = 151, //!< 4 �����A64 �r�b�g�����Ȃ����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ETC2_R8G8B8A8_SRGB = 152, //!< 4 �����A64 �r�b�g�����Ȃ� sRGB ���K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_EAC_R11_UNORM = 153, //!< 1 �����A64 �r�b�g�����Ȃ����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_EAC_R11_SNORM = 154, //!< 1 �����A64 �r�b�g�����t�����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_EAC_R11G11_UNORM = 155, //!< 2 �����A128 �r�b�g�����Ȃ����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_EAC_R11G11_SNORM = 156, //!< 2 �����A128 �r�b�g�����t�����K�� ETC2 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_4X4_UNORM = 157, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_4X4_SRGB = 158, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 4x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_5X4_UNORM = 159, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 5x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_5X4_SRGB = 160, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 5x4 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_5X5_UNORM = 161, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 5x5 �u���b�N���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_5X5_SRGB = 162, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 5x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_6X5_UNORM = 163, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 6x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_6X5_SRGB = 164, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 6x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_6X6_UNORM = 165, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 6x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_6X6_SRGB = 166, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 6x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X5_UNORM = 167, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 8x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X5_SRGB = 168, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 8x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X6_UNORM = 169, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 8x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X6_SRGB = 170, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 8x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X8_UNORM = 171, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 8x8 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_8X8_SRGB = 172, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 8x8 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X5_UNORM = 173, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 10x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X5_SRGB = 174, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 10x5 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X6_UNORM = 175, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 10x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X6_SRGB = 176, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 10x6 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X8_UNORM = 177, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 10x8 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X8_SRGB = 178, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 10x8 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X10_UNORM = 179, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 10x10 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_10X10_SRGB = 180, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 10x10 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_12X10_UNORM = 181, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 12x10 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_12X10_SRGB = 182, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 12x10 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_12X12_UNORM = 183, //!< 4 �����A128 �r�b�g�����Ȃ����K�� ASTC 12x12 �u���b�N���k���k�t�H�[�}�b�g�ł��B
	V3D_FORMAT_ASTC_12X12_SRGB = 184, //!< 4 �����A128 �r�b�g�����Ȃ� sRGB ���K�� ASTC 12x12 �u���b�N���k���k�t�H�[�}�b�g�ł��B
};

//! @enum V3D_MEMORY_PROPERTY_FLAG
//! @brief �������v���p�e�B�t���O
enum V3D_MEMORY_PROPERTY_FLAG : V3DFlags
{
	//! @brief �f�o�C�X����A�N�Z�X�ł��郁�����ł��B
	V3D_MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001,
	//! @brief �z�X�g����A�N�Z�X�ł��郁�����ł��B
	V3D_MEMORY_PROPERTY_HOST_VISIBLE = 0x00000002,
	//! @brief �z�X�g���珑�����񂾓��e���t���b�V������K�v�͂���܂���B<br>
	//! ���̃t���O���w�肵���ꍇ�� IV3DResource::Map ����� IV3DResource::Unmap ���Ƀf�o�C�X�܂��̓z�X�g����̃������̉��A�s���̕ύX�������s��Ȃ����߁A���x�̌��オ�����܂�܂��B<br>
	V3D_MEMORY_PROPERTY_HOST_COHERENT = 0x00000004,
	//! @brief �������̓��e���z�X�g�ŃL���b�V�����܂��B<br>
	//! �z�X�g�œ��e���L���b�V�����邱�ƂɃ������ւ̃A�N�Z�X���x�̌��オ�����܂�܂��B
	V3D_MEMORY_PROPERTY_HOST_CACHED = 0x00000008,
	//! @brief ���������x�����Ċ��蓖�Ă���ꍇ������܂��B<br>
	//! ���̃t���O�� �J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�A�C���v�b�g�A�^�b�`�����g�ɑ΂��ėL���ł��B<br>
	//! �܂��A�C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT ���w�肵�Ă��������B
	V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED = 0x00000010,
};

//! @enum V3D_PIPELINE_STAGE_FLAG
//! @brief �p�C�v���C���X�e�[�W�t���O
enum V3D_PIPELINE_STAGE_FLAG : V3DFlags
{
	//! @brief �ŏ��̃X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
	//! @brief �o�[�e�b�N�X�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��������X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
	//! @brief �o�[�e�b�N�X�V�F�[�_�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
	//! @brief �e�b�Z���[�V��������V�F�[�_�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
	//! @brief �W�I���g���V�F�[�_�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
	//! @brief �t���O�����g�V�F�[�_�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
	//! @brief �t���O�����g�V�F�[�f�B���O�O�̃f�v�X�e�X�g�ƃX�e���V���e�X�g�����s�����X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃f�v�X�X�e���V���A�^�b�`�����g�̃��[�h�������܂܂�܂��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
	//! @brief �t���O�����g�V�F�[�f�B���O��̃f�v�X�e�X�g�ƃX�e���V���e�X�g�����s�����X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃f�v�X�X�e���V���A�^�b�`�����g�̃X�g�A�������܂܂�܂��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
	//! @brief �u�����h���ꂽ�J���[���o�͂����p�C�v���C���X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃J���[�A�^�b�`�����g�̃��[�h�A�X�g�A�A���]���u ( �}���`�T���v�� ) ���܂܂�܂��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	//! @brief �R���s���[�g�V�F�[�_�[�X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_COMPUTE </td></tr></table>
	V3D_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
	//! @brief �R�s�[���������X�e�[�W�ł��B<br>
	//! �ȉ��̃R�}���h�͂��̃X�e�[�W�Ŏ��s����K�v������܂��B<br>
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
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br>�܂���<br> ::V3D_QUEUE_TRANSFER </td></tr></table>
	V3D_PIPELINE_STAGE_TRANSFER = 0x00001000,
	//! @brief �Ō�̃X�e�[�W�ł��B<br>
	//! �R�}���h�ɂ���Đ������ꂽ�I�y���[�V���������s����������X�e�[�W�ł���A
	//! ���̃A�N�Z�X���ʂ̃L���[�܂��̓v���[���e�[�V���� ( IV3DQueue::Present ) �ōs����ꍇ�̈ڍs�Ɏg�p���܂��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,

	//! @brief �z�X�g����̓ǂݍ��݁A�܂��͏������݂����s����X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_HOST = 0x00004000,
	//! @brief ���ׂẴO���t�B�b�N�p�C�v���C���X�e�[�W�ł��B<br>
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
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> ::V3D_QUEUE_GRAPHICS </td></tr></table>
	V3D_PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000,
	//! @brief ���ׂẴR�}���h�̃X�e�[�W�ł��B
	//! <br>
	//! <table class="pielineStage"><tr><th>�K�{�̃L���[�@�\</th></tr><tr><td> </td></tr></table>
	V3D_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
};

//! @enum V3D_ACCESS_FLAG
//! @brief �A�N�Z�X�t���O
enum V3D_ACCESS_FLAG : V3DFlags
{
	//! @brief �C���f�b�N�X�o�b�t�@�[�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_VERTEX_INPUT</td><td>::V3D_BUFFER_USAGE_INDEX</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_INDEX_READ = 0x00000002,
	//! @brief �o�[�e�b�N�X�o�b�t�@�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td class="td_stage">::V3D_PIPELINE_STAGE_VERTEX_INPUT</td><td>::V3D_BUFFER_USAGE_VERTEX</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_VERTEX_READ = 0x00000004,
	//! @brief ���j�t�H�[���o�b�t�@�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
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
	//! @brief �C���v�b�g�A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_FRAGMENT_SHADER</td><td>::V3D_IMAGE_USAGE_INPUT_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
	//! @brief �X�g���[�W�o�b�t�@�A���j�t�H�[���e�N�Z���o�b�t�@�A�X�g���[�W�e�N�Z���o�b�t�@�A�T���v�����O�C���[�W�A�X�g���[�W�C���[�W�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
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
	//! @brief �X�g���[�W�o�b�t�@�A�X�g���[�W�e�N�Z���o�b�t�@�A�X�g���[�W�C���[�W�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
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
	//! @brief �u�����f�B���O�A���W�b�N�A�T�u�p�X�̃��[�h�Ȃǂ̃J���[�A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td class="td_stage">::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT</td><td>::V3D_IMAGE_USAGE_COLOR_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
	//! @brief �����_�����O�p�X���̃T�u�p�X�̃��[�h�ォ��X�g�A�܂ł̏����Ȃǂ̃J���[�A�^�b�`�����g�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT</td><td>::V3D_IMAGE_USAGE_COLOR_ATTACHMENT</td><td>::V3D_QUEUE_GRAPHICS</td></tr>
	//! </table>
	V3D_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
	//! @brief �f�v�X�A�X�e���V���̑���A�T�u�p�X�̃��[�h�Ȃǂ̃f�v�X�X�e���V���A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
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
	//! @brief �����_�����O�p�X���̃T�u�p�X�̃��[�h�ォ��X�g�A�܂ł̏����Ȃǂ̃f�v�X�X�e���V���A�^�b�`�����g�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
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
	//! @brief �R�s�[�R�}���h�ł̃o�b�t�@�܂��̓C���[�W�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_TRANSFER</td><td> ::V3D_BUFFER_USAGE_TRANSFER_SRC<br> <br> ::V3D_IMAGE_USAGE_TRANSFER_SRC</td><td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE<br> ::V3D_QUEUE_TRANSFER</td>
	//!   </tr>
	//! </table>
	V3D_ACCESS_TRANSFER_READ = 0x00000800,
	//! @brief �R�s�[�R�}���h�ł̃o�b�t�@�܂��̓C���[�W�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_TRANSFER</td><td>::V3D_BUFFER_USAGE_TRANSFER_DST<br> <br> ::V3D_IMAGE_USAGE_TRANSFER_DST</td><td>::V3D_QUEUE_GRAPHICS<br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td></tr>
	//! </table>
	V3D_ACCESS_TRANSFER_WRITE = 0x00001000,
	//! @brief �z�X�g����̓ǂݎ��A�N�Z�X�ł��B<br>
	//! �o�C���h�������\�[�X�������̓����� ::V3D_MEMORY_PROPERTY_HOST_VISIBLE ���܂܂�Ă��郊�\�[�X��ΏۂƂ��܂��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_HOST</td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_HOST_READ = 0x00002000,
	//! @brief �z�X�g����̏������݃A�N�Z�X�ł��B<br>
	//! �o�C���h�������\�[�X�������̓����� ::V3D_MEMORY_PROPERTY_HOST_VISIBLE ���܂܂�Ă��郊�\�[�X��ΏۂƂ��܂��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td>::V3D_PIPELINE_STAGE_HOST</td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_HOST_WRITE = 0x00004000,
	//! @brief ����̃��\�[�X�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! �X���b�v�`�F�C���̃C���[�W�Ȃǂ����̓���̃��\�[�X�Ɋ܂܂�܂��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td> </td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_MEMORY_READ = 0x00008000,
	//! @brief ����̃��\�[�X�ւ̏������݃A�N�Z�X�ł��B<br>
	//! �X���b�v�`�F�C���̃C���[�W�Ȃǂ����̓���̃��\�[�X�Ɋ܂܂�܂��B<br>
	//! <table class="access">
	//!   <tr><th class="stage">�T�|�[�g����X�e�[�W</th><th class="usage">�T�|�[�g���郊�\�[�X�̎g�p�@</th><th class="queue">�K�{�̃L���[�@�\</th></tr>
	//!   <tr><td> </td><td> </td><td> </td></tr>
	//! </table>
	V3D_ACCESS_MEMORY_WRITE = 0x00010000,
};

//! @enum V3D_DEPENDENCY_FLAG
//! @brief �ˑ����t���O
enum V3D_DEPENDENCY_FLAG : V3DFlags
{
	//! @brief �T�u�p�X�̈ˑ��֌W���t���[���o�b�t�@�[���ł��邱�Ƃ�\���܂��B
	V3D_DEPENDENCY_BY_REGION = 0x00000001,
};

//! @enum V3D_SHADER_STAGE_FLAG
//! @brief �V�F�[�_�[�X�e�[�W�t���O
enum V3D_SHADER_STAGE_FLAG : V3DFlags
{
	V3D_SHADER_STAGE_VERTEX = 0x00000001, //!< �o�[�e�b�N�X�V�F�[�_�[�X�e�[�W�ł��B
	V3D_SHADER_STAGE_TESSELLATION_CONTROL = 0x00000002, //!< �e�b�Z���[�V��������V�F�[�_�[�X�e�[�W�ł��B
	V3D_SHADER_STAGE_TESSELLATION_EVALUATION = 0x00000004, //!< �e�b�Z���[�V�����]���V�F�[�_�[�X�e�[�W�ł��B
	V3D_SHADER_STAGE_GEOMETRY = 0x00000008, //!< �W�I���g���V�F�[�_�[�X�e�[�W�ł��B
	V3D_SHADER_STAGE_FRAGMENT = 0x00000010, //!< �t���O�����g�V�F�[�_�[�X�e�[�W�ł��B
	V3D_SHADER_STAGE_COMPUTE = 0x00000020, //!< �R���s���[�g�V�F�[�_�[�X�e�[�W�ł��B
};

//! @enum V3D_IMAGE_LAYOUT
//! @brief �C���[�W�̃��C�A�E�g
enum V3D_IMAGE_LAYOUT
{
	//! @brief �f�o�C�X�̃A�N�Z�X���T�|�[�g���܂���B<br>
	//! �C���[�W�쐬���̏������C�A�E�g�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B<br>
	//! �܂��A���̃��C�A�E�g����ڍs����Ƃ��A�������̓��e�̕ێ��͕ۏ؂���܂���B
	//! @sa IV3DDevice::CreateImage
	V3D_IMAGE_LAYOUT_UNDEFINED = 0,
	//! @brief ���ׂẴA�N�Z�X���T�|�[�g���܂��B<br>
	V3D_IMAGE_LAYOUT_GENERAL = 1,
	//! @brief �J���[�A�^�b�`�����g�A���]���u�A�^�b�`�����g�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT ���܂܂�Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT = 2,
	//! @brief �f�v�X�X�e���V���A�^�b�`�����g�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT ���܂܂�Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT = 3,
	//! @brief �V�F�[�_�[����ǂݎ�邱�Ƃ̂ł���f�v�X�X�e���V���C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT �A ::V3D_IMAGE_USAGE_SAMPLED �������� ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT �̂����ꂩ���܂܂�Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY = 4,
	//! @brief �V�F�[�_�[����̓ǂݎ���p�C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ����̓T���v�����O���邱�Ƃ̂ł���C���[�W�A�C���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B<br>
	//! �܂����̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_SAMPLED �܂��� ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT �̂����ꂩ���܂܂�Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_SHADER_READ_ONLY = 5,
	//! @brief �R�s�[�R�}���h�̃R�s�[���C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC �A�p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B<br>
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_LAYOUT_TRANSFER_SRC = 6,
	//! @brief �R�s�[�R�}���h�̃R�s�[��C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p����C���[�W�̎g�p�@�ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST �A�p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_LAYOUT_TRANSFER_DST = 7,
	//! @brief �f�o�C�X�̃A�N�Z�X���T�|�[�g���܂���<br>
	//! �C���[�W�쐬���̏������C�A�E�g�Ƃ��Ďg�p���邱�Ƃ��ł��A�C���[�W�쐬�シ���Ƀ������ւ̏������݂��ł��܂��B<br>
	//! �܂��قȂ郌�C�A�E�g�ֈڍs����Ƃ��A�������̓��e�͕ێ�����܂��B
	//! @sa IV3DDevice::CreateImage
	V3D_IMAGE_LAYOUT_PREINITIALIZED = 8,
	//! @brief �X���b�v�`�F�C���̃C���[�W��\������ۂ̃��C�A�E�g�ł��B<br>
	//! �X���b�v�`�F�C���̃C���[�W�̓v���[���g ( IV3DQueue::Present ) ����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	V3D_IMAGE_LAYOUT_PRESENT_SRC = 9,
};

//! @enum V3D_SAMPLE_COUNT_FLAG
//! @brief �T���v�����t���O
enum V3D_SAMPLE_COUNT_FLAG : V3DFlags
{
	V3D_SAMPLE_COUNT_1 = 0x00000001, //!< 1 �T���v���ł��B
	V3D_SAMPLE_COUNT_2 = 0x00000002, //!< 2 �T���v���ł��B
	V3D_SAMPLE_COUNT_4 = 0x00000004, //!< 4 �T���v���ł��B
	V3D_SAMPLE_COUNT_8 = 0x00000008, //!< 8 �T���v���ł��B
	V3D_SAMPLE_COUNT_16 = 0x00000010, //!< 16 �T���v���ł��B
	V3D_SAMPLE_COUNT_32 = 0x00000020, //!< 32 �T���v���ł��B
	V3D_SAMPLE_COUNT_64 = 0x00000040, //!< 64 �T���v���ł��B
};

//! @enum V3D_COMPARE_OP
//! @brief ��r���Z�q
//! @sa V3DSamplerDesc::compareOp
//! @sa V3DPipelineStencilOpDesc::compareOp
//! @sa V3DPipelineDepthStencilDesc::depthCompareOp
enum V3D_COMPARE_OP : uint8_t
{
	V3D_COMPARE_OP_NEVER = 0, //!< �e�X�g�͐�΂ɒʉ߂��܂���B
	V3D_COMPARE_OP_LESS = 1, //!< R<S �̂Ƃ��Ƀe�X�g�ɍ��i���܂��B
	V3D_COMPARE_OP_EQUAL = 2, //!< R=S �̂Ƃ��Ƀe�X�g�ɍ��i���܂��B
	V3D_COMPARE_OP_LESS_OR_EQUAL = 3, //!< R��S �̂Ƃ��Ƀe�X�g�ɍ��i���܂��B
	V3D_COMPARE_OP_GREATER = 4, //!< R>S �̂Ƃ��Ƀe�X�g�ɍ��i���܂��B
	V3D_COMPARE_OP_NOT_EQUAL = 5, //!< R��S �̂Ƃ��Ƀe�X�g�����i���܂��B
	V3D_COMPARE_OP_GREATER_OR_EQUAL = 6, //!< R��S �̂Ƃ��Ƀe�X�g�ɍ��i���܂��B
	V3D_COMPARE_OP_ALWAYS = 7, //!< �e�X�g�͏�ɒʉ߂��܂��B
};

//! @enum V3D_INDEX_TYPE
//! @brief �C���f�b�N�X�̃^�C�v
enum V3D_INDEX_TYPE : uint8_t
{
	V3D_INDEX_TYPE_UINT16 = 0, //!< �������� 16Bit �����̃C���f�b�N�X�ł��B
	V3D_INDEX_TYPE_UINT32 = 1, //!< �������� 32Bit �����̃C���f�b�N�X�ł��B
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �\����
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @union V3DPoint2D
//! @brief �������������� x y ���W
struct V3DPoint2D
{
	uint32_t x; //!< X ���W�ł��B
	uint32_t y; //!< Y ���W�ł��B
};

//! @union V3DPoint3D
//! @brief �������������� x y z ���W
struct V3DPoint3D
{
	uint32_t x; //!< X ���W�ł��B
	uint32_t y; //!< Y ���W�ł��B
	uint32_t z; //!< Z ���W�ł��B
};

//! @union V3DSize2D
//! @brief �������������� width height �T�C�Y
struct V3DSize2D
{
	uint32_t width; //!< ���ł��B
	uint32_t height; //!< �����ł��B
};

//! @union V3DSize3D
//! @brief �������������� width height depth �T�C�Y
struct V3DSize3D
{
	uint32_t width; //!< ���ł��B
	uint32_t height; //!< �����ł��B
	uint32_t depth; //!< �[���ł��B
};

//! @union V3DRectangle2D
//! @brief �̈�
struct V3DRectangle2D
{
	uint32_t x; //!< X ���W�ł��B
	uint32_t y; //!< Y ���W�ł��B
	uint32_t width; //!< ���ł��B
	uint32_t height; //!< �����ł��B
};

//! @union V3DRectangle3D
//! @brief �̈�
struct V3DRectangle3D
{
	uint32_t x; //!< X ���W�ł��B
	uint32_t y; //!< Y ���W�ł��B
	uint32_t z; //!< Z ���W�ł��B
	uint32_t width; //!< ���ł��B
	uint32_t height; //!< �����ł��B
	uint32_t depth; //!< �[���ł��B
};

//! @union V3DViewport
//! @brief �r���[�|�[�g
//! @remarks �[�x�� minDepth < maxDepth �ł���K�v������܂��B
struct V3DViewport
{
	//! @brief �r���[�|�[�g�̗̈�ł��B
	V3DRectangle2D rect;
	//! @brief �ŏ��[�x�ł��B<br>
	//! �l�� { 0.0f <= minDepth <= 1.0f } �͈̔͂ɐ�������܂��B
	float minDepth;
	//! @brief �ő�[�x�ł��B<br>
	//! �l�� { minDepth <= maxDepth <= 1.0f } �͈̔͂ɐ�������܂��B
	float maxDepth;
};

//! @union V3DClearColorValue
//! @brief �J���[�̃N���A�l
union V3DClearColorValue
{
	float float32[4]; //!< �����t 32bit ���������_�ł��B
	int32_t int32[4]; //!< �����t 32bit �����ł��B
	uint32_t uint32[4]; //!< �������� 32bit �����ł��B
};

//! @struct V3DClearDepthStencilValue
//! @brief �f�v�X�X�e���V���̃N���A�l
struct V3DClearDepthStencilValue
{
	//! @brief �[�x���N���A����l�ł��B<br>
	//! ���̒l�� { 0.0f <= depth <= 1.0f } �͈̔͂ɐ�������܂��B
	float depth;
	//! @brief �X�e���V�����N���A����l�ł��B
	uint32_t stencil;
};

//! @union V3DClearValue
//! @brief �N���A�l
union V3DClearValue
{
	V3DClearColorValue color; //!< �J���[�̃N���A�l�ł��B
	V3DClearDepthStencilValue depthStencil; //!< �f�v�X�X�e���V���̃N���A�l�ł��B
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �I�u�W�F�N�g
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DObject
//! @brief �I�u�W�F�N�g�C���^�[�t�F�[�X
class V3D_DLL_API IV3DObject
{
public:
	//! @brief �Q�Ɛ����擾���܂��B
	//! @return �Q�Ɛ���Ԃ��܂��B
	virtual int64_t GetRefCount() const = 0;
	//! @brief �Q�Ɛ����C���N�������g���܂��B
	virtual void AddRef() = 0;
	//! @brief �Q�Ɛ����f�N�������g���A�Q�Ɛ��� 0 �ɂȂ������_�ŃI�u�W�F�N��j�����܂��B
	virtual void Release() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DObject() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �f�o�C�X�`���C���h
// ----------------------------------------------------------------------------------------------------

class IV3DDevice;

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDeviceChild
//! @brief �f�o�C�X�`���C���h�C���^�[�t�F�[�X
class V3D_DLL_API IV3DDeviceChild : public IV3DObject
{
public:
	//! @brief ���̃C���^�[�t�F�[�X���쐬���ꂽ�f�o�C�X���擾���܂�
	//! @param[out] ppDevice �f�o�C�X��\�� IV3DDevice �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X
	virtual void GetDevice(IV3DDevice** ppDevice) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDeviceChild() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �t�F���X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DFence
//! @brief �t�F���X�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateFence
class V3D_DLL_API IV3DFence : public IV3DDeviceChild
{
public:
	//! @brief �V�O�i����Ԃ��ǂ������m�F���܂��B
	//! @return �V�O�i����Ԃ̏ꍇ�� true ��Ԃ��܂��B
	virtual bool IsSignaled() const = 0;

	//! @brief �t�F���X�����Z�b�g���āA��V�O�i����Ԃɂ��܂��B
	//! @retval V3D_OK �t�F���X�̃��Z�b�g�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �t�F���X�̃��Z�b�g�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset() = 0;
	//! @brief �t�F���X���V�O�i����ԂɂȂ�܂őҋ@���܂��B
	//! @param[in] timeOut �^�C���A�E�g���Ԃ��i�m�b�P�ʂŎw�肵�܂��B
	//! @retval V3D_OK �t�F���X���V�O�i����ԂɂȂ�܂����B
	//! @retval V3D_TIMEOUT �t�F���X�͔�V�O�i����Ԃł��B
	//! @retval V3D_ERROR_FAIL �t�F���X�͑ҋ@�Ɏ��s���܂����B
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
// �N�G���v�[��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUERY_TYPE
//! @brief �N�G���̃^�C�v
enum V3D_QUERY_TYPE : uint8_t
{
	V3D_QUERY_TYPE_OCCLUSION = 0, //!< �I�N���[�W�����N�G���ł��B
	V3D_QUERY_TYPE_PIPELINE_STATISTICS = 1, //!< �p�C�v���C���̓��v�N�G���ł��B
	V3D_QUERY_TYPE_TIMESTAMP = 2, //!< �^�C���X�^���v�N�G���ł��B
};

//! @enum V3D_QUERY_RESULT_FLAG
//! @brief �N�G���̌��ʃt���O
enum V3D_QUERY_RESULT_FLAG : uint8_t
{
	V3D_QUERY_RESULT_WAIT = 0x00000001, //!< �w�肵�����ׂẴN�G���̌��ʂ��擾�ł���܂őҋ@���܂��B
	V3D_QUERY_RESULT_WITH_AVAILABILITY = 0x00000002, //!< ���ʂ��o�Ă���N�G�������邩�ǂ������m�F�ł���悤�ɂ��܂��B
	V3D_QUERY_RESULT_PARTIAL = 0x00000004, //!< �w�肵�����ׂẴN�G���łȂ��Ă����ʂ��擾�ł���悤�ɂ��܂��B
};

//! @enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
//! @brief �N�G���̃p�C�v���C���̓��v�^�C�v
enum V3D_QUERY_PIPELINE_STATISTIC_TYPE : uint8_t
{
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_VERTICES = 0, //!< ���͂��ꂽ���_�̐��ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_PRIMITIVES = 1, //!< ���͂��ꂽ�v���~�e�B�u�̐��ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_VERTEX_SHADER_INVOCATIONS = 2, //!< �o�[�e�b�N�X�V�F�[�_�[�̌Ăяo���񐔂ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_GEOMETRY_SHADER_INVOCATIONS = 3, //!< �W�I���g���V�F�[�_�[�̌Ăяo���񐔂ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_GEOMETRY_SHADER_PRIMITIVES = 4, //!< �W�I���g���V�F�[�_�[�ɂ���Đ������ꂽ�v���~�e�B�u�̐��ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_CLIPPING_INVOCATIONS = 5, //!< �N���b�s���O�X�e�[�W�ŏ������ꂽ�v���~�e�B�u�̐��ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_CLIPPING_PRIMITIVES = 6, //!< �N���b�s���O�X�e�[�W�ɂ���ďo�͂��ꂽ���ꂽ�v���~�e�B�u�̐��ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_FRAGMENT_SHADER_INVOCATIONS = 7, //!< �t���O�����g�V�F�[�_�[�̌Ăяo���񐔂ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_TESSELLATION_CONTROL_SHADER_PATCHES = 8, //!< �e�b�Z���[�V��������V�F�[�_�[�̂���ď��������p�b�`���ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_TESSELLATION_EVALUATION_SHADER_INVOCATIONS = 9, //!< �e�b�Z���[�V�����]���V�F�[�_�[�̌Ăяo���񐔂ł��B
	V3D_QUERY_PIPELINE_STATISTIC_TYPE_COMPUTE_SHADER_INVOCATIONS = 10, //!< �R���s���[�g�V�F�[�_�[�̌Ăяo���񐔂ł��B

	V3D_QUERY_PIPELINE_STATISTIC_TYPE_COUNT = 11, //!< �N�G���̃p�C�v���C���̓��v�^�C�v�̐��ł��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DQueryPoolDesc
//! @brief �N�G���̋L�q
struct V3DQueryPoolDesc
{
	V3D_QUERY_TYPE queryType; //!< �N�G���̃^�C�v�ł��B
	uint32_t queryCount; //!< �v�[���ɂ���ĊǗ������N�G���̐��ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DQueryPool
//! @brief �N�G���v�[���C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateQueryPool
class V3D_DLL_API IV3DQueryPool : public IV3DDeviceChild
{
public:
	//! @brief �N�G���v�[���̋L�q���擾���܂�
	//! @return �N�G���v�[���̋L�q��Ԃ��܂�
	virtual const V3DQueryPoolDesc& GetDesc() const = 0;

	//! @brief �N�G���̌��ʂ��擾���܂��B
	//! @param[in] firstQuery ���ʂ��擾����ŏ��̃N�G���̃C���f�b�N�X�ł��B
	//! @param[in] queryCount ���ʂ��擾����N�G���̐��ł��B
	//! @param[in] queryResultCount �擾����N�G���̌��ʂ̐��ł��B
	//! @param[in] pQueryResults queryResultCount �̒l�̗v�f�̐������N�G���̌��ʂ��i�[���� uint64_t �^�̔z��ł��B
	//! @param[in] queryResultFlags �N�G���̌��ʂ̎擾���@��\�� ::V3D_QUERY_RESULT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �N�G���̌��ʂ��擾���܂����B
	//! @retval V3D_NOT_READY �N�G���̌��ʂ͂܂��擾�ł��Ă��܂���B
	//! @retval V3D_ERROR_FAIL �N�G���̌��ʂ̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! - �Ԃ�l�� ::V3D_OK �łȂ��Ă��N�G���̌��ʂ��擾���悤�Ƃ������ IV3DCommandBuffer::ResetQueryPool �ŃN�G���v�[�������Z�b�g���Ă��������B
	//! .
	//! - �N�G���̃^�C�v�� ::V3D_QUERY_TYPE_OCCLUSION �܂��� ::V3D_QUERY_TYPE_TIMESTAMP �ł������ꍇ�� queryResultCount �̒l�� queryCount �ɂȂ�܂��B
	//! .
	//! - �N�G���̃^�C�v�� ::V3D_QUERY_TYPE_PIPELINE_STATISTICS �ł������ꍇ�� queryResultCount �̒l�� queryCount * 11 �ɂȂ�A���ꂼ��� 0 �` 10 �v�f�̒l�̈Ӗ��� ::V3D_QUERY_PIPELINE_STATISTIC_TYPE �񋓌^���Q�Ƃ��Ă��������B
	//! .
	//! - �N�G���̃^�C�v�� ::V3D_QUERY_TYPE_TIMESTAMP �ł������ꍇ�� pQueryResults �ɏ������܂�鐔�l�̒P�ʂ̓i�m�b�ɂȂ�܂��B
	//! .
	//! - flags �� ::V3D_QUERY_RESULT_WAIT ���܂܂�Ă���ꍇ�́A�N�G���̌��ʂ��擾�ł���܂őҋ@���܂��B<br>
	//! �������N�G�����L���[�ɑ��M����Ă��Ȃ���ԂŌ��ʂ��擾���悤�Ƃ���ƁA�f�b�h���b�N���Ђ��������܂��̂Œ��ӂ��K�v�ł��B
	//! .
	//! - flags �� ::V3D_QUERY_RESULT_PARTIAL ���܂܂�Ă���ꍇ�́A�w�肵�����ׂẴN�G���łȂ��A�����I�ȃN�G���̓r���̌��ʂ��擾�ł���悤�ɂȂ�܂��B<br>
	//! �������A�N�G���̃^�C�v�� ::V3D_QUERY_TYPE_TIMESTAMP �ł������ꍇ�A���̃t���O���w�肷�邱�Ƃ͂ł��܂���B<br>
	//! .
	//! - flags �� ::V3D_QUERY_RESULT_WITH_AVAILABILITY ���܂܂�Ă���ꍇ�́AqueryResultCount �� queryCount + 1 ��K�v�Ƃ��܂��B<br>
	//! �����čŌ�̗v�f ( pQueryResults[queryResultCount - 1] ) �ɃN�G���̌��ʂ��擾�ł����ꍇ�� 1 �ȏ�̒l�A�擾�ł��Ȃ������ꍇ�� 0 ���������܂�܂��B
	virtual V3D_RESULT GetResults(uint32_t firstQuery, uint32_t queryCount, uint32_t queryResultCount, uint64_t* pQueryResults, V3DFlags queryResultFlags) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DQueryPool() {}
	//! @endcond
};

//! @}


// ----------------------------------------------------------------------------------------------------
// �V�F�[�_�[���W���[��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DShaderModule
//! @brief �V�F�[�_�[���W���[���C���^�[�t�F�[�X
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
// ���\�[�X������
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DResourceMemoryDesc
//! @brief ���\�[�X�������̋L�q
struct V3DResourceMemoryDesc
{
	//! @brief �������̃v���p�e�B�t���O��\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags propertyFlags;
	//! @brief �������̃T�C�Y�ł��B
	uint64_t size;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DResourceMemory
//! @brief ���\�[�X�������C���^�[�t�F�[�X
class V3D_DLL_API IV3DResourceMemory : public IV3DDeviceChild
{
public:
	//! @brief ���\�[�X�������̋L�q���擾���܂��B
	//! @return ���\�[�X�������̋L�q��Ԃ��܂��B
	virtual const V3DResourceMemoryDesc& GetDesc() const = 0;

	//! @brief ���\�[�X�������̃}�b�v�J�n��ʒm���܂��B
	//! @retval V3D_OK ���\�[�X�������̃}�b�v�J�n�̒ʒm�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�������̃}�b�v�J�n�̒ʒm�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! ���̃��\�b�h�̓������S�̂̃A�N�Z�X��L���ȏ�Ԃɂ��܂��B<br>
	//! ��x�ɑ����̃��\�[�X�����̃��������A�N�Z�X����� ( IV3DResource::Map ����� IV3DResource::Unmap ) �̃p�t�H�[�}���X�����コ���邱�Ƃ��ł��܂��B
	virtual V3D_RESULT BeginMap() = 0;
	//! @brief ���\�[�X�������̃}�b�v�I����ʒm���܂��B
	//! @retval V3D_OK ���\�[�X�������̃}�b�v�I���̒ʒm�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�������̃}�b�v�I���̒ʒm�Ɏ��s���܂����B
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
// ���\�[�X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_RESOURCE_TYPE
//! @brief ���\�[�X�^�C�v
enum V3D_RESOURCE_TYPE : uint8_t
{
	V3D_RESOURCE_TYPE_BUFFER = 0, //!< �o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_RESOURCE_TYPE_IMAGE = 1, //!< �C���[�W�Ƃ��Ďg�p���܂��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DResourceDesc
//! @brief ���\�[�X�̋L�q
struct V3DResourceDesc
{
	//! @brief ���\�[�X�̃^�C�v�ł��B
	V3D_RESOURCE_TYPE type;

	//! @brief �������̓�����\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags memoryPropertyFlags;
	//! @brief �������^�C�v�̃r�b�g�}�X�N�ł��B
	//! @remarks �e�r�b�g�� IV3DAdapter::GetMemoryTypeDesc �̑������ł��� memoryType ��\���܂��B
	uint32_t memoryTypeBits;
	//! @brief �A���C�����g���K�p���ꂽ�������̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t memorySize;
	//! @brief �������̃T�C�Y�̃A���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t memoryAlignment;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DResource
//! @brief ���\�[�X�C���^�[�t�F�[�X
class V3D_DLL_API IV3DResource : public IV3DDeviceChild
{
public:
	//! @brief ���\�[�X�̋L�q���擾���܂��B
	//! @return ���\�[�X�̋L�q��Ԃ��܂��B
	virtual const V3DResourceDesc& GetResourceDesc() const = 0;

	//! @brief ���\�[�X���������擾���܂��B
	//! @param[out] ppMemory �擾�������\�[�X��������n�� IV3DResourceMemory �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK ���\�[�X�������̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�������̎擾�Ɏ��s���܂����B<br>
	//! ���\�[�X���������擾���悤�Ƃ������\�[�X�́A�X���b�v�`�F�C���̃C���[�W�ł���\��������܂��B
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) = 0;

	//! @brief ���\�[�X�̎w�肳�ꂽ�������̃|�C���^�L���ɂ��A�擾���܂��B
	//! @param[in] offset �L���ɂ��郁�����̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] size �L���ɂ��郁�����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B<br>
	//! �܂��A0 ���w�肷�邱�Ƃ� offset ���烁�����̍Ō�܂ł��}�b�v���܂��B
	//! @param[out] ppMemory �擾�����������̃|�C���^��n�� void �|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK ���\�[�X�̃������̃|�C���^�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�̃������̃|�C���^�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) = 0;
	//! @brief ���\�[�X�̎w�肳�ꂽ�������̃|�C���^�𖳌��ɁA�ԋp���܂��B
	//! @retval V3D_OK ���\�[�X�̃������̃|�C���^�̕ԋp�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�̃������̃|�C���^�̕ԋp�Ɏ��s���܂����B
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
// �o�b�t�@�[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_BUFFER_USAGE_FLAG
//! @brief �o�b�t�@�[�̎g�p�@�t���O
enum V3D_BUFFER_USAGE_FLAG : V3DFlags
{
	//! @brief �R�s�[���o�b�t�@�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	V3D_BUFFER_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief �R�s�[��o�b�t�@�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	V3D_BUFFER_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief ���j�t�H�[���e�N�Z���o�b�t�@�����Ďg�p���܂��B
	V3D_BUFFER_USAGE_UNIFORM_TEXEL = 0x00000004,
	//! @brief �X�g���[�W�e�N�Z���o�b�t�@�Ƃ��Ďg�p���܂��B
	V3D_BUFFER_USAGE_STORAGE_TEXEL = 0x00000008,
	//! @brief ���j�t�H�[���o�b�t�@�Ƃ��Ďg�p���܂��B
	V3D_BUFFER_USAGE_UNIFORM = 0x00000010,
	//! @brief �X�g���[�W�o�b�t�@�Ƃ��Ďg�p���܂��B
	V3D_BUFFER_USAGE_STORAGE = 0x00000020,
	//! @brief �C���f�b�N�X�o�b�t�@�Ƃ��Ďg�p���܂��B
	V3D_BUFFER_USAGE_INDEX = 0x00000040,
	//! @brief �o�[�e�b�N�X�o�b�t�@�Ƃ��Ďg�p���܂��B
	V3D_BUFFER_USAGE_VERTEX = 0x00000080,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBufferDesc
//! @brief �o�b�t�@�[�̋L�q
//! @sa IV3DDevice::CreateBuffer
//! @sa IV3DBuffer::GetDesc
struct V3DBufferDesc
{
	V3DFlags usageFlags; //!< �o�b�t�@�[�̎g�p�@��\�� ::V3D_BUFFER_USAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	uint64_t size; //!< �o�b�t�@�[�̃T�C�Y�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DBuffer
//! @brief �o�b�t�@�[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateBuffer
class V3D_DLL_API IV3DBuffer : public IV3DResource
{
public:
	//! @brief �o�b�t�@�[�̋L�q���擾���܂��B
	//! @return �o�b�t�@�[�̋L�q��Ԃ��܂��B
	virtual const V3DBufferDesc& GetDesc() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DBuffer() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �C���[�W
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_IMAGE_TYPE
//! @brief �C���[�W�̃^�C�v
enum V3D_IMAGE_TYPE : uint8_t
{
	V3D_IMAGE_TYPE_1D = 0, //!< 1D ( width ) �C���[�W�ł��B
	V3D_IMAGE_TYPE_2D = 1, //!< 2D ( width height ) �C���[�W�ł��B
	V3D_IMAGE_TYPE_3D = 2, //!< 3D ( width height depth )�C���[�W�ł��B
};

//! @enum V3D_IMAGE_TILING
//! @brief �C���[�W�̃^�C�����O
enum V3D_IMAGE_TILING : uint8_t
{
	//! @brief �œK�ȃ^�C�����O���g�p���܂��B<br>
	//! �e�N�Z���̔z�u�͎����Ɉˑ����邱�ƂɂȂ邽�߁A�T�u���\�[�X�ɃA�N�Z�X���邱�Ƃ͂ł��܂���B
	V3D_IMAGE_TILING_OPTIMAL = 0,
	//! @brief ���`�^�C�����O���g�p���܂��B<br>
	//! �e�N�Z���͒�߂�ꂽ�����Ŕz�u����邽�߁A�T�u���[�X�ւ̃A�N�Z�X���\�ɂ��܂��B
	//! @sa IV3DImage::GetSubresourceLayout
	V3D_IMAGE_TILING_LINEAR = 1,
};

//! @enum V3D_IMAGE_USAGE_FLAG
//! @brief �C���[�W�̎g�p�@�t���O
enum V3D_IMAGE_USAGE_FLAG : V3DFlags
{
	//! @brief �R�s�[���C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyImageToBuffer
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	V3D_IMAGE_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief �R�s�[��C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	//! @sa IV3DCommandBuffer::CopyBufferToImage
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	//! @sa IV3DCommandBuffer::ResolveImage
	//! @sa IV3DCommandBuffer::ResolveImageView
	//! @sa IV3DCommandBuffer::ClearImage
	V3D_IMAGE_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief �V�F�[�_�[����T���v�����O���邱�Ƃ��ł���C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DDescriptorSet::SetImageView
	//! @sa IV3DDescriptorSet::SetImageViewAndSampler
	V3D_IMAGE_USAGE_SAMPLED = 0x00000004,
	//! @brief �L���[�u�C���[�W�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_CUBE_COMPATIBLE = 0x00000008,
	//! @brief �R���s���[�g�V�F�[�_�[���珑�����ނ��Ƃ̂ł���X�g���[�W�C���[�W�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_STORAGE = 0x00000010,
	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000020,
	//! @brief �T�u�p�X�̃f�v�X�X�e���V���Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000040,
	//! @brief �C���[�W�̃������v���p�e�B�� ::V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED ���w�肳��Ă��邱�Ƃ�\���܂��B<br>
	//! �܂��ȉ��̎g�p�@��p���č쐬���ꂽ�C���[�W�ɍœK�ł��B<br>
	//! ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT<br>
	//! ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT<br>
	//! ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT<br>
	V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT = 0x0000080,
	//! @brief �T�u�p�X�̃C���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p���܂��B<br>
	//! ���̃C���[�W�̓t���O�����g�V�F�[�_�[�݂̂��ǂݎ�邱�Ƃ��ł��܂��B
	V3D_IMAGE_USAGE_INPUT_ATTACHMENT = 0x00000100,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageDesc
//! @brief �C���[�W�̋L�q
//! @sa IV3DDevice::CreateImage
//! @sa IV3DImage::GetDesc
struct V3DImageDesc
{
	V3D_IMAGE_TYPE type; //!< �C���[�W�̃^�C�v�ł��B
	V3D_FORMAT format; //!< �C���[�W�̃t�H�[�}�b�g�ł��B
	uint32_t width; //!< �C���[�W�̕��ł��B
	uint32_t height; //!< �C���[�W�̍����ł��B
	uint32_t depth; //!< �C���[�W�̐[���ł��B
	uint32_t levelCount; //!< �C���[�W�̃��x�����ł��B
	uint32_t layerCount; //!< �C���[�W�̃��C���[���ł��B
	V3D_SAMPLE_COUNT_FLAG samples; //!< �C���[�W�̃T���v�����ł��B
	V3D_IMAGE_TILING tiling; //!< �C���[�W�̃^�C�����O�ł��B
	V3DFlags usageFlags; //!< �C���[�W�̎g�p�@��\�� ::V3D_IMAGE_USAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DImageSubresourceLayout
//! @brief �C���[�W�̃T�u���\�[�X���C�A�E�g
//! @sa IV3DImage::GetSubresourceLayout
struct V3DImageSubresourceLayout
{
	uint64_t offset; //!< �������̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t size; //!< �������̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t rowPitch; //!< �s�̃s�b�`���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t layerPitch; //!< ���C���[�̃s�b�`���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t depthPitch; //!< �[���̃s�b�`�ł����o�C�g�P�ʂŎw�肵�܂��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DImage
//! @brief �C���[�W�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateImage
class V3D_DLL_API IV3DImage : public IV3DResource
{
public:
	//! @brief �C���[�W�̋L�q���擾���܂��B
	//! @return �C���[�W�̋L�q��Ԃ��܂��B
	virtual const V3DImageDesc& GetDesc() const = 0;
	//! @brief �C���[�W�̃T�u���\�[�X���C�A�E�g���擾���܂��B
	//! @param[in] level �T�u���\�[�X�̃��x���ł��B
	//! @param[in] layer �T�u���\�[�X�̃��C���[�ł��B
	//! @return �C���[�W�̃T�u���\�[�X���C�A�E�g��Ԃ��܂��B
	//! @remarks
	//! �T�u���\�[�X�̃��C�A�E�g���擾�ł���悤�ɂ��邽�߂ɂ́A�C���[�W�̃^�C�����O�� ::V3D_IMAGE_TILING_LINEAR ���w�肳��Ă���K�v������܂��B
	virtual const V3DImageSubresourceLayout& GetSubresourceLayout(uint32_t level, uint32_t layer) const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DImage() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �o�b�t�@�[�r���[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBufferViewDesc
//! @brief �o�b�t�@�[�r���[�̋L�q
//! @sa IV3DDevice::CreateBufferView
//! @sa IV3DBufferView::GetDesc
struct V3DBufferViewDesc
{
	V3D_FORMAT format; //!< �t�H�[�}�b�g�ł��B
	uint64_t offset; //!< �A�N�Z�X����o�b�t�@�̃������̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t size; //!< �A�N�Z�X����o�b�t�@�̃������̃I�t�Z�b�g����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DBufferView
//! @brief �o�b�t�@�[�r���[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateBufferView
class V3D_DLL_API IV3DBufferView : public IV3DDeviceChild
{
public:
	//! @brief �o�b�t�@�[�r���[�̋L�q���擾���܂��B
	//! @return �o�b�t�@�r���[�̋L�q��\�� V3DBufferViewDesc �\���̂̃A�h���X��Ԃ��܂��B
	virtual const V3DBufferViewDesc& GetDesc() const = 0;
	//! @brief �o�b�t�@�[�r���[���A�N�Z�X����o�b�t�@���擾���܂��B
	//! @param[out] ppBuffer �擾�����o�b�t�@�[�r���[��\�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetBuffer(IV3DBuffer** ppBuffer) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DBufferView() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �C���[�W�r���[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_IMAGE_VIEW_TYPE
//! @brief �C���[�W�r���[�̃^�C�v
enum V3D_IMAGE_VIEW_TYPE : uint8_t
{
	V3D_IMAGE_VIEW_TYPE_1D = 0, //!< 1D ( width ) �C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_2D = 1, //!< 2D ( width height ) �C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_3D = 2, //!< 3D ( width height depth ) �C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_CUBE = 3, //!< �L���[�u�C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_1D_ARRAY = 4, //!< 1D �z��C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_2D_ARRAY = 5, //!< 2D �z��C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY = 6, //!< �L���[�u�z��C���[�W�r���[�Ƃ��Ďg�p���܂��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageViewDesc
//! @brief �C���[�W�r���[�̋L�q
//! @sa IV3DDevice::CreateImageView
//! @sa IV3DImageView::GetDesc
struct V3DImageViewDesc
{
	V3D_IMAGE_VIEW_TYPE type; //!< �C���[�W�r���[�̃^�C�v�ł��B
	uint32_t baseLevel; //!< �ŏ��̃��x���ł��B
	uint32_t levelCount; //!< ���x�����ł��B
	uint32_t baseLayer; //!< �ŏ��̃��C���[�ł��B
	uint32_t layerCount; //!< ���[���[���ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DImageView
//! @brief �C���[�W�r���[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateImageView
class V3D_DLL_API IV3DImageView : public IV3DDeviceChild
{
public:
	//! @brief �C���[�W�r���[�̋L�q���擾���܂��B
	//! @return �C���[�W�r���[�̋L�q��\�� V3DImageViewDesc �\���̂̃A�h���X��Ԃ��܂��B
	virtual const V3DImageViewDesc& GetDesc() const = 0;
	//! @brief �A�N�Z�X����C���[�W���擾���܂��B
	//! @param[out] ppImage �擾�����C���[�W��n�� IV3DImage �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetImage(IV3DImage** ppImage) = 0;
	//! @brief �A�N�Z�X����C���[�W�̃T�u���\�[�X�̃T�C�Y���擾���܂��B
	//! @return �A�N�Z�X����C���[�W�̃T�u���\�[�X�̃T�C�Y��Ԃ��܂��B
	virtual const V3DSize3D& GetImageSubresourceSize() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DImageView() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �T���v���[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_FILTER
//! @brief �t�B���^�[
enum V3D_FILTER : uint8_t
{
	//! @brief �ł��߂����W�̃e�N�Z�����g�p���܂��B
	V3D_FILTER_NEAREST = 0,
	//! @brief �C���[�W�̎������ɉ������e�N�Z���̉��d���ς��g�p���܂��B<br>
	//! <ul>
	//! <li>1D�C���[�W</li><br>
	//! 2x1<br>
	//! <li>2D�C���[�W�A�L���[�u�C���[�W</li><br>
	//! 2x2<br>
	//! <li>3D�C���[�W</li><br>
	//! 2x2x2<br>
	//! </ul>
	V3D_FILTER_LINEAR = 1,
};

//! @enum V3D_MIPMAP_MODE
//! @brief �~�b�v�}�b�v���[�h
enum V3D_MIPMAP_MODE : uint8_t
{
	//! @brief �ł��߂����W�̃e�N�Z�����g�p���܂��B
	V3D_MIPMAP_MODE_NEAREST = 0,
	//! @brief �C���[�W�̎������ɉ������e�N�Z���̉��d���ς��g�p���܂��B
	V3D_MIPMAP_MODE_LINEAR = 1,
};

//! @enum V3D_ADDRESS_MODE
//! @brief �A�h���X���[�h
enum V3D_ADDRESS_MODE : uint8_t
{
	//! @brief ���s�[�g���܂��B
	V3D_ADDRESS_MODE_REPEAT = 0,
	//! @brief ���]���ă��s�[�g���܂��B
	V3D_ADDRESS_MODE_MIRRORED_REPEAT = 1,
	//! @brief �G�b�W���J��Ԃ��܂��B
	V3D_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
	//! @brief �w�肳�ꂽ���E���̐F���J��Ԃ��܂��B
	//! @sa V3D_BORDER_COLOR
	V3D_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
};

//! @enum V3D_BORDER_COLOR
//! @brief �{�[�_�[�J���[
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
//! @brief �T���v���̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
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
	V3D_FILTER magFilter; //!< �g��t�B���^�ł��B
	V3D_FILTER minFilter; //!< �k���t�B���^�ł��B
	V3D_MIPMAP_MODE mipmapMode; //!< �~�b�v�}�b�v���[�h
	V3D_ADDRESS_MODE addressModeU; //!< U ���W�̃A�h���b�V���O���[�h�ł��B
	V3D_ADDRESS_MODE addressModeV; //!< V ���W�̃A�h���b�V���O���[�h�ł��B
	V3D_ADDRESS_MODE addressModeW; //!< W ���W�̃A�h���b�V���O���[�h�ł��B
	float mipLodBias; //!< �~�b�v�}�b�v�̃��x���̃I�t�Z�b�g�ł��B<br>���̒l�� { - V3DDeviceCaps::maxSamplerLodBias <= mipLodBias <= + V3DDeviceCaps::maxSamplerLodBias } �͈̔͂ɐ�������܂��B
	bool anisotropyEnable; //!< �ٕ����t�B���^�����O��L���ɂ��邩�ǂ������w�肵�܂��B<br>���̒l�� V3DDeviceCaps::flags �� ::V3D_CAP_SAMPLER_ANISOTROPY ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	float maxAnisotropy; //!< �ٕ����l���N�����v����l�ł��B<br>���̒l�� anisotropyEnable �� true �̂Ƃ��ɗL���ɂȂ� V3DDeviceCaps::maxSamplerAnisotropy �ȉ��ɐ�������܂��B
	bool compareEnable; //!< ��r�I�y�[���[�^�[�ł��� compareOp ��L���ɂ��邩�ǂ������w�肵�܂��B
	V3D_COMPARE_OP compareOp; //!< ��r���Z�q�ł��B<br> ::V3D_COMPARE_OP �̐����ɂ��� R �̓T���v���[�ɂ���ē��͂����l�ł���AS �̓A�^�b�`�����g�̃e�N�Z���̒l��\���܂��B
	float minLod; //!< �~�b�v�}�b�v�̃��x�����N�����v����ŏ��l�ł��B<br>�ʏ킱�̒l�͍ŏ��̃~�b�v�}�b�v�̃��x�����w�肵�܂��B
	float maxLod; //!< �~�b�v�}�b�v�̃��x�����N�����v����ő�l�ł��B<br>�ʏ킱�̒l�͍Ō�~�b�v�}�b�v�̃��x�����w�肵�܂��B
	V3D_BORDER_COLOR borderColor; //!< ���E���̐F�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DSampler
//! @brief �T���v���[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateSampler
class V3D_DLL_API IV3DSampler : public IV3DDeviceChild
{
public:
	//! @brief �T���v���[�̋L�q���擾���܂��B
	//! @return �T���v���[�̋L�q��Ԃ��܂��B
	virtual const V3DSamplerDesc& GetDesc() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DSampler() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �����_�[�p�X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_ATTACHMENT_LOAD_OP
//! @brief �A�^�b�`�����g�̃��[�h����
enum V3D_ATTACHMENT_LOAD_OP : uint8_t
{
	//! @brief �ŏ��̃T�u�p�X���J�n���ꂽ�ۂɃA�^�b�`�����g�̈ȑO�̓��e���ێ�����Ă���K�v���Ȃ����Ƃ�\���܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE �܂��� ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_UNDEFINED = 0,
	//! @brief �ŏ��̃T�u�p�X���J�n���ꂽ�ۂɃA�^�b�`�����g�̈ȑO�̓��e��ǂݍ��݂܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A ::V3D_ACCESS_COLOR_ATTACHMENT_READ �܂��� ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_LOAD = 1,
	//! @brief �ŏ��̃T�u�p�X���J�n���ꂽ�ۂɃA�^�b�`�����g���N���A���܂��B
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE �܂��� ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_CLEAR = 2,
};

//! @enum V3D_ATTACHMENT_STORE_OP
//! @brief �A�^�b�`�����g�̃X�g�A����
enum V3D_ATTACHMENT_STORE_OP : uint8_t
{
	//! @brief �Ō�̃T�u�p�X���I�������ۂɃA�^�b�`�����g�̓��e���ۑ����ꂸ�A�j�������\�������邱�Ƃ�\���܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE �܂��� ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_STORE_OP_UNDEFINED = 0,
	//! @brief �Ō�̃T�u�p�X���I�������ۂɃA�^�b�`�����g�̓��e��ۑ����܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A ::V3D_ACCESS_COLOR_ATTACHMENT_WRITE �܂��� ::V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_STORE_OP_STORE = 1,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAttachmentDesc
//! @brief �A�^�b�`�����g�̋L�q
//! @sa IV3DDevice::CreateRenderPass
struct V3DAttachmentDesc
{
	//! @brief �A�^�b�`�����g�̃t�H�[�}�b�g�ł��B
	V3D_FORMAT format;
	//! @brief �A�^�b�`�����g�̃T���v�����ł��B
	V3D_SAMPLE_COUNT_FLAG samples;
	//! @brief �A�^�b�`�����g�̃��[�h�I�y���[�V�����ł��B
	V3D_ATTACHMENT_LOAD_OP loadOp;
	//! @brief �A�^�b�`�����g�̃X�g�A�I�y���[�V�����ł��B
	V3D_ATTACHMENT_STORE_OP storeOp;
	//! @brief �A�^�b�`�����g�̃X�e���V���̃��[�h�I�y���[�V�����ł��B
	V3D_ATTACHMENT_LOAD_OP stencilLoadOp;
	//! @brief �A�^�b�`�����g�̃X�e���V���̃X�g�A�I�y���[�V�����ł��B
	V3D_ATTACHMENT_STORE_OP stencilStoreOp;
	//! @brief �����_�[�p�X�J�n���̃A�^�b�`�����g�̃C���[�W���C�A�E�g�ł��B
	V3D_IMAGE_LAYOUT initialLayout;
	//! @brief �����_�[�p�X�I�����̃A�^�b�`�����g�̃C���[�W���C�A�E�g�ł��B
	V3D_IMAGE_LAYOUT finalLayout;
	//! @brief �����_�[�p�X�J�n���̃A�^�b�`�����g�̃N���A�l�ł��B<br>
	//! ���̒l�� loadOp �܂��� stencilLoadOp �� ::V3D_ATTACHMENT_LOAD_OP_CLEAR �̂Ƃ��ɗL���ł��B
	V3DClearValue clearValue;
};

//! @struct V3DAttachmentReference
//! @brief �A�^�b�`�����g�̎Q��
//! @sa V3DSubpassDesc
struct V3DAttachmentReference
{
	uint32_t attachment; //!< �Q�Ƃ���A�^�b�`�����g�ł��B<br>IV3DDevice::CreateRenderPass �Ŏw�肵�� V3DAttachmentDesc �\���̂̔z��̃C���f�b�N�X�ł��B
	V3D_IMAGE_LAYOUT layout; //!< �T�u�p�X�J�n���̃A�^�b�`�����g�̃C���[�W���C�A�E�g�ł��B
};

//! @struct V3DSubpassDesc
//! @brief �T�u�p�X�̋L�q
//! @sa IV3DDevice::CreateRenderPass
struct V3DSubpassDesc
{
	//! @brief �t���O�����g�V�F�[�_�[�ɓ��͂���C���v�b�g�A�^�b�`�����g�̐��ł��B
	uint32_t inputAttachmentCount;
	//! @brief �t���O�����g�V�F�[�_�[�ɓ��͂���C���v�b�g�A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! - inputAttachmentCount �� 1 �ȏ�̏ꍇ�A�z��̗v�f���� inputAttachmentCount �ł���K�v������A0 �̏ꍇ�� nullptr ���w�肵�܂��B<br>
	//! - �A�^�b�`�����g�̃C���[�W�̎g�p�@ ( V3DImageDesc::usageFlags ) �ɂ� ::V3D_IMAGE_USAGE_INPUT_ATTACHMENT ���܂܂�Ă���K�v������܂��B
	const V3DAttachmentReference* pInputAttachments;

	//! @brief �o�͐�̃J���[�A�^�b�`�����g�̐��ł��B
	uint32_t colorAttachmentCount;
	//! @brief �o�͐�̃J���[�A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! - colorAttachmentCount �� 1 �ȏ�̏ꍇ�A�z��̗v�f���� colorAttachmentCount �ł���K�v������A0 �̏ꍇ�� nullptr ���w�肵�܂��B<br>
	//! - �A�^�b�`�����g�̃C���[�W�̎g�p�@ ( V3DImageDesc::usageFlags ) �ɂ� ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT ���܂܂�Ă���K�v������܂��B
	const V3DAttachmentReference* pColorAttachments;
	//! @brief �o�͐�̃}���`�T���v���J���[�A�^�b�`�����g ( pColorAttachments ) �̕ϊ���̃A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! - �J���[�A�^�b�`�����g ( pColorAttachments ) �̃T���v������ ::V3D_SAMPLE_COUNT_1 �łȂ� colorAttachmentCount �� 1 �ȏ�̏ꍇ�A�z��̗v�f���� colorAttachmentCount �ł���K�v������A����ȊO�̏ꍇ�� nullptr ���w�肵�܂��B<br>
	//! - �A�^�b�`�����g�̃C���[�W�̎g�p�@ ( V3DImageDesc::usageFlags ) �ɂ� ::V3D_IMAGE_USAGE_COLOR_ATTACHMENT ���܂܂�Ă���K�v������A�T���v���� ( V3DImageDesc::samples ) �� ::V3D_SAMPLE_COUNT_1 �ł���K�v������܂��B<br>
	//! �܂��A�ϊ�����K�v���Ȃ��A�^�b�`�����g�̏ꍇ�� V3DAttachmentReference::attachment �� ::V3D_ATTACHMENT_UNUSED ���w�肵�Ă��������B<br>
	const V3DAttachmentReference* pResolveAttachments;

	//! @brief �o�͐�̃f�v�X�X�e���V���A�^�b�`�����g�̎Q�Ƃł��B<br>
	//! - �f�v�X�X�e���V���A�^�b�`�����g�����݂��Ȃ��ꍇ�� nullptr ���w�肵�Ă��������B<br>
	//! - �A�^�b�`�����g�̃C���[�W�̎g�p�@ ( V3DImageDesc::usageFlags ) �ɂ� ::V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT ���܂܂�Ă���K�v������A<br>
	//! �T���v���� ( V3DImageDesc::samples ) �̓J���[�A�^�b�`�����g�Ɠ����ł���K�v������܂��B<br>
	const V3DAttachmentReference* pDepthStencilAttachment;

	//! @brief ���̃T�u�p�X�Q�Ƃ���ĂȂ��A���e��ێ��������A�^�b�`�����g�̐��ł��B
	uint32_t preserveAttachmentCount;
	//! @brief ���̃T�u�p�X�ŎQ�Ƃ���ĂȂ��A���e��ێ��������A�^�b�`�����g�̃C���f�b�N�X�z��ł��B<br>
	//! - preserveAttachmentCount �� 1 �ȏ�̏ꍇ�A�z��̗v�f���� preserveAttachmentCount �ł���K�v������A0 �̏ꍇ�� nullptr ���w�肵�܂��B
	const uint32_t* pPreserveAttachments;
};

//! @struct V3DSubpassDependencyDesc
//! @brief �T�u�p�X�̈ˑ����̋L�q
//! @sa IV3DDevice::CreateRenderPass
struct V3DSubpassDependencyDesc
{
	uint32_t srcSubpass; //!< �ڍs���̃T�u�p�X�̃C���f�b�N�X�ł��B
	uint32_t dstSubpass; //!< �ڍs��̃T�u�p�X�̃C���f�b�N�X�ł��B

	V3DFlags srcStageMask; //!< �ڍs���̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< �ڍs���̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	//! �ˑ���\�� ::V3D_DEPENDENCY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B<br>
	//! srcSubpass �܂��� dstSubpass �� ::V3D_SUBPASS_EXTERNAL ���w�肳��Ă���ꍇ�� ::V3D_DEPENDENCY_BY_REGION ���܂߂Ă͂����܂���B
	V3DFlags dependencyFlags;
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DRenderPass
//! @brief �����_�[�p�X�C���^�[�t�F�[�X
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
// �t���[���o�b�t�@�[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DFrameBufferDesc
//! @brief �t���[���o�b�t�@�[�̋L�q
//! @sa IV3DFrameBuffer::GetDesc
struct V3DFrameBufferDesc
{
	uint32_t attachmentWidth; //!< �A�^�b�`�����g�̕��ł��B
	uint32_t attachmentHeight; //!< �A�^�b�`�����g�̍����ł��B
	uint32_t attachmentDepth; //!< �A�^�b�`�����g�̐[���ł��B
	uint32_t attachmentLayerCount; //!< �A�^�b�`�����g�̃��C���[���ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DFrameBuffer
//! @brief �t���[���o�b�t�@�[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateFrameBuffer
class V3D_DLL_API IV3DFrameBuffer : public IV3DDeviceChild
{
public:
	//! @brief �t���[���o�b�t�@�[�̋L�q���擾���܂��B
	//! @return �t���[���o�b�t�@�[�̋L�q��Ԃ��܂��B
	virtual const V3DFrameBufferDesc& GetDesc() const = 0;

	//! @brief �����_�[�p�X���擾���܂��B
	//! @param[out] ppRenderPass �擾���������_�[�p�X��n�� IV3DRenderPass �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetRenderPass(IV3DRenderPass** ppRenderPass) = 0;

	//! @brief �A�^�b�`�����g�̐����擾���܂��B
	//! @return �A�^�b�`�����g�̐���Ԃ��܂��B
	virtual uint32_t GetAttachmentCount() const = 0;
	//! @brief �A�^�b�`�����g���擾���܂�
	//! @param[in] attachmentIndex �A�^�b�`�����g�̃C���f�b�N�X�ł��B
	//! @param[out] ppImageView �擾�����A�^�b�`�����g��n�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetAttachment(uint32_t attachmentIndex, IV3DImageView** ppImageView) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DFrameBuffer() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �f�X�N���v�^�Z�b�g���C�A�E�g
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_DESCRIPTOR_TYPE
//! @brief �f�X�N���v�^�̃^�C�v
enum V3D_DESCRIPTOR_TYPE : uint8_t
{
	//! @brief �T���v���[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_SAMPLER = 0,
	//! @brief �T���v�����O�\�ȃC���[�W�ƃT���v���[�̃Z�b�g�Ŏg�p���܂��B
	V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
	//! @brief �T���v�����O�\�ȃC���[�W�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
	//! @brief �X�g���[�W�C���[�W�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
	//! @brief ���j�t�H�[���e�N�Z���o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
	//! @brief �X�g���[�W�e�N�Z���o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
	//! @brief ���j�t�H�[���o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
	//! @brief �X�g���[�W�o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
	//! @brief �_�C�i�~�b�N���j�t�H�[���o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
	//! @brief �_�C�i�~�b�N�X�g���[�W�o�b�t�@�[�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
	//! @brief �C���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p���܂��B
	V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
};

//! @enum V3D_DESCRIPTOR_SET_TYPE
//! @brief �f�X�N���v�^�Z�b�g�̃^�C�v
enum V3D_DESCRIPTOR_SET_TYPE : uint8_t
{
	V3D_DESCRIPTOR_SET_TYPE_STANDARD = 0, //!< IV3DCommandBuffer::BindDescriptorSets �Ńo�C���h����X�^���_�[�h�f�X�N���v�^�Z�b�g�ł��B
	V3D_DESCRIPTOR_SET_TYPE_PUSH = 1, //!< IV3DCommandBuffer::PushDescriptorSets �ōX�V����v�b�V���f�X�N���v�^�Z�b�g�ł��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDescriptorDesc
//! @brief �f�X�N���v�^�̋L�q
//! @sa IV3DDevice::CreateDescriptorSetLayout
struct V3DDescriptorDesc
{
	uint32_t binding; //!< �o�C���f�B���O�ł��B
	V3D_DESCRIPTOR_TYPE type; //!< �f�X�N���v�^�̃^�C�v�ł��B
	V3DFlags stageFlags; //!< �o�C���h����V�F�[�_�[�X�e�[�W��\�� ::V3D_SHADER_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DDescriptorSetLayoutStatistics
//! @brief �f�X�N���v�^�Z�b�g���C�A�E�g�̓��v
//! @sa IV3DDescriptorSetLayout::GetStatistics
struct V3DDescriptorSetLayoutStatistics
{
	uint32_t poolCount; //!< �쐬���ꂽ�v�[���̐��ł��B
	uint32_t setCount; //!< �쐬���ꂽ�f�X�N���v�^�Z�b�g�̐��ł��B
	uint32_t setCapacity; //!< �쐬�ł���f�X�N���v�^�Z�b�g�̍ő吔�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDescriptorSetLayout
//! @brief �f�X�N���v�^�Z�b�g���C�A�E�g�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateDescriptorSetLayout
class V3D_DLL_API IV3DDescriptorSetLayout : public IV3DDeviceChild
{
public:
	//! @brief �f�X�N���v�^�Z�b�g�̃^�C�v���擾���܂��B
	//! @return �f�X�N���v�^�Z�b�g�̃^�C�v��Ԃ��܂��B
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const = 0;
	//! @brief �f�X�N���^�Z�b�g���C�A�E�g�̓��v���擾���܂��B
	//! @return �f�X�N���^�Z�b�g���C�A�E�g�̓��v��Ԃ��܂��B
	virtual const V3DDescriptorSetLayoutStatistics& GetStatistics() const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDescriptorSetLayout() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �f�X�N���v�^�Z�b�g
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDescriptorSet
//! @brief �f�X�N���v�^�Z�b�g�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateDescriptorSet
class V3D_DLL_API IV3DDescriptorSet : public IV3DDeviceChild
{
public:
	//! @brief �f�X�N���v�^�Z�b�g�̃^�C�v���擾���܂��B
	//! @return �f�X�N���v�^�Z�b�g�̃^�C�v��Ԃ��܂��B
	virtual V3D_DESCRIPTOR_SET_TYPE GetType() const = 0;
	//! @brief �f�X�N���v�^�Z�b�g�̃��C�A�E�g���擾���܂��B
	//! @param[out] ppLayout �擾�����f�X�N���v�^�Z�b�g�̃��C�A�E�g��n�� IV3DDescriptorSetLayout �C���^�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetLayout(IV3DDescriptorSetLayout** ppLayout) = 0;

	//! @brief �o�b�t�@�[���擾���܂��B
	//! @param[in] binding �擾����o�b�t�@�[�̃o�C���f�B���O�ł��B
	//! @param[out] ppBuffer �擾�����o�b�t�@�[��n�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK �o�b�t�@�[�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�[�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBuffer(uint32_t binding, IV3DBuffer** ppBuffer) = 0;
	//! @brief �o�b�t�@�[��ݒ肵�܂��B
	//! @param[in] binding �o�b�t�@�[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pBuffer �ݒ肷��o�b�t�@�[�ł��B
	//! @param[in] offset �ݒ肷��o�b�t�@�[�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] size �ݒ肷��o�b�t�@�[�̃I�t�Z�b�g����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! @retval V3D_OK �o�b�t�@�[�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�̐ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �f�X�N���v�^�̎�ނɂ���ăA���C�����g���قȂ邽�߁Aoffset �ɂ͓K�؂Ȓl���w�肵�Ă��������B<br>
	//! �e�f�X�N���v�^�̃A���C�����g�� V3DDeviceCaps �ɋL�q����Ă���A�ȉ��̂悤�ɂȂ�܂��B<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER <br> ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! �܂��A�w�肵���o�C���f�B���O�̃f�X�N���v�^�� ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC �܂��� ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC �̏ꍇ�A<br>
	//! size �Ɏw�肷��l�́A�o�b�t�@�[���̂̃T�C�Y�ł͂Ȃ��A�V�F�[�_�[���ǂݏ�������͈͂̃T�C�Y���w�肵�܂��B<br>
	//! <br>
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	virtual V3D_RESULT SetBuffer(uint32_t binding, IV3DBuffer* pBuffer, uint64_t offset, uint64_t size) = 0;

	//! @brief �o�b�t�@�[�r���[���擾���܂��B
	//! @param[in] binding �擾����o�b�t�@�[�r���[�̃o�C���f�B���O�ł��B
	//! @param[out] ppBufferView �擾�����o�b�t�@�[�r���[��n�� IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK �o�b�t�@�[�r���[�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�[�r���[�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) = 0;
	//! @brief �o�b�t�@�[�r���[��ݒ肵�܂��B
	//! @param[in] binding �o�b�t�@�[�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pBufferView �ݒ肷��o�b�t�@�[�r���[�̃|�C���^�ł��B
	//! @retval V3D_OK �o�b�t�@�[�r���[�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�[�r���[�̐ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
	virtual V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) = 0;

	//! @brief �C���[�W�r���[���擾���܂��B
	//! @param[in] binding �擾����C���[�W�̃o�C���f�B���O�ł��B
	//! @param[out] ppImageView �擾�����C���[�W�r���[��n�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK �C���[�W�r���[�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W�r���[�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) = 0;
	//! @brief �f�X�N���v�^�Z�b�g���o�C���h���ꂽ�ۂ̃C���[�W���C�A�E�g���擾���܂��B
	//! @param[in] binding �擾����C���[�W���C�A�E�g�̃o�C���f�B���O�ł��B
	//! @param[out] pImageLayout �擾�����C���[�W���C�A�E�g���i�[���� ::V3D_IMAGE_LAYOUT �񋓒萔�̃|�C���^�ł��B
	//! @retval V3D_OK �C���[�W���C�A�E�g�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W���C�A�E�g�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) = 0;
	//! @brief �C���[�W�r���[��ݒ肵�܂��B
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @retval V3D_OK �C���[�W�r���[�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W�r���[�̐ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br> 
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) = 0;
	//! @brief �C���[�W�r���[��ݒ肵�܂��B<br>
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @param[in] imageLayout �f�X�N���v�^�Z�b�g���o�C���h����ۂ̃C���[�W���C�A�E�g�ł��B<br>
	//! �f�X�N���v�^�Z�b�g���o�C���h����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @retval V3D_OK �C���[�W�r���[�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W�r���[�̐ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) = 0;

	//! @brief �T���v���[���擾���܂��B
	//! @param[in] binding �擾����T���v���[�̃o�C���f�B���O�ł��B
	//! @param[out] ppSampler �擾�����T���v���[��n�� IV3DSampler �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK �T���v���[�̎擾�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �T���v���[�̎擾�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) = 0;
	//! @brief �T���v���[��ݒ肵�܂��B<br>
	//! @param[in] binding �T���v���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pSampler �ݒ肷��T���v���[�̃|�C���^�ł��B
	//! @retval V3D_OK �T���v���[�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �T���v���[�̐ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) = 0;

	//! @brief �C���[�W�r���[�ƃT���v���[��ݒ肵�܂��B<br>
	//! ���̃o�C���f�B���O�̃f�X�N���v�^�� 
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER �ł���K�v������܂��B
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @param[in] pSampler �ݒ肷��T���v���[�̃|�C���^�ł��B
	//! @retval V3D_OK ���[�W�r���[�ƃT���v���[��ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���[�W�r���[�ƃT���v���[��ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �ݒ肵���C���[�W�r���[�̓f�X�N���v�^�Z�b�g���o�C���h����O�ɃC���[�W���C�A�E�g�� ::V3D_IMAGE_LAYOUT_SHADER_READ_ONLY �Ɉڍs���Ă����K�v������܂��B<br>
	//! <br>
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, IV3DSampler* pSampler) = 0;

	//! @brief �C���[�W�r���[�ƃT�����v�[��ݒ肵�܂��B<br>
	//! ���̃o�C���f�B���O�̃f�X�N���v�^�� 
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER �ł���K�v������܂��B
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @param[in] imageLayout �f�X�N���v�^�Z�b�g���o�C���h����ۂ̃C���[�W���C�A�E�g�ł��B<br>
	//! �f�X�N���v�^�Z�b�g���o�C���h����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSampler �ݒ肷��T���v���[�̃|�C���^�ł��B
	//! @retval V3D_OK ���[�W�r���[�ƃT���v���[��ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���[�W�r���[�ƃT���v���[��ݒ�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @remarks
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���X�V���܂�
	//! @remarks
	//! - �f�X�N���v�^�Z�b�g�Ɋe�탊�\�[�X��ݒ肵���ۂɎ��s����K�v������܂��B<br>
	//! �������A�f�X�N���v�^�Z�b�g�̃^�C�v�� ::V3D_DESCRIPTOR_SET_TYPE_PUSH �̏ꍇ�A���̃��\�b�h�����s����K�v�͂���܂���B
	virtual void Update() = 0;

protected:
	//! @cond MISC
	virtual ~IV3DDescriptorSet() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �p�C�v���C�����C�A�E�g
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DConstantDesc
//! @brief �v�b�V���萔�̋L�q
//! @sa IV3DDevice::CreatePipelineLayout
struct V3DConstantDesc
{
	V3DFlags shaderStageFlags; //!< �萔���v�b�V������V�F�[�_�[�X�e�[�W��\�� ::V3D_SHADER_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	uint32_t offset; //!< �萔�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t size; //!< �萔�̃������T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DPipelineLayout
//! @brief �p�C�v���C�����C�A�E�g�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreatePipelineLayout
class V3D_DLL_API IV3DPipelineLayout : public IV3DDeviceChild
{
public:
	//! @brief �萔�̐����擾���܂��B
	//! @return �萔�̐���Ԃ��܂��B
	virtual uint32_t GetConstantCount() const = 0;
	//! @brief �萔�̋L�q���擾���܂��B
	//! @param[in] constant �萔�̃C���f�b�N�X�ł��B
	//! @return �萔�̋L�q��\�� V3DConstantDesc �\���̂̃A�h���X��Ԃ��܂��B
	virtual const V3DConstantDesc& GetConstantDesc(uint32_t constant) const = 0;

	//! @brief �f�X�N���v�^�Z�b�g�̐����擾���܂��B
	//! @return �f�X�N���v�^�Z�b�g�̐���Ԃ��܂��B
	virtual uint32_t GetDescriptorSetCount() const = 0;
	//! @brief �f�X�N���v�^�Z�b�g�̃��C�A�E�g���擾���܂��B
	//! @param[in] descriptorSetIndex �f�X�N���v�^�Z�b�g�̃C���f�b�N�X�ł��B
	//! @param[out] ppDescriptorSetLayout �擾�����f�X�N���v�^�Z�b�g�̃��C�A�E�g��n�� IV3DDescriptorSetLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetDescriptorSetLayout(uint32_t descriptorSetIndex, IV3DDescriptorSetLayout** ppDescriptorSetLayout) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DPipelineLayout() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �p�C�v���C��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_PIPELINE_TYPE
//! @brief �p�C�v���C���̃^�C�v
enum V3D_PIPELINE_TYPE : uint8_t
{
	V3D_PIPELINE_TYPE_GRAPHICS = 0, //!< �O���t�B�b�N�X�p�C�v���C���Ƃ��Ďg�p���܂��B
	V3D_PIPELINE_TYPE_COMPUTE = 1, //!< �R���s���[�g�p�C�v���C���Ƃ��Ďg�p���܂��B
};

//! @enum V3D_PRIMITIVE_TOPOLOGY
//! @brief �v���~�e�B�u�g�|���W
enum V3D_PRIMITIVE_TOPOLOGY : uint8_t
{
	V3D_PRIMITIVE_TOPOLOGY_POINT_LIST = 0, //!< �|�C���g�̃��X�g�ł��B
	V3D_PRIMITIVE_TOPOLOGY_LINE_LIST = 1, //!< ���C���̃��X�g�ł��B
	V3D_PRIMITIVE_TOPOLOGY_LINE_STRIP = 2, //!< ���C���̃X�g���b�v�ł��B
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3, //!< �O�p�`�̃��X�g�ł��B
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4, //!< �O�p�`�̃X�g���b�v�ł��B
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5, //!< �O�p�`�̃t�@���ł��B
	V3D_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
	V3D_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
	V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
	V3D_PRIMITIVE_TOPOLOGY_PATCH_LIST = 10, //!< �e�b�Z���[�V�����̃p�b�`���X�g�ł��B
};

//! @enum V3D_POLYGON_MODE
//! @brief �|���S�����[�h
enum V3D_POLYGON_MODE : uint8_t
{
	V3D_POLYGON_MODE_FILL = 0, //!< �|���S����h��Ԃ��ĕ`�悵�܂��B
	V3D_POLYGON_MODE_LINE = 1, //!< �|���S�����G�b�W��`�悵�܂��B
	V3D_POLYGON_MODE_POINT = 2, //!< �|���S���̒��_��`�悵�܂��B
};

//! @enum V3D_CULL_MODE
//! @brief �J�����O���[�h
enum V3D_CULL_MODE : uint8_t
{
	V3D_CULL_MODE_NONE = 0, //!< �J�����O�����܂���B
	V3D_CULL_MODE_FRONT = 1, //!< �O�ʂ��J�����O���܂��B
	V3D_CULL_MODE_BACK = 2, //!< ��ʂ��J�����O���܂��B
};

//! @enum V3D_STENCIL_OP
//! @brief �X�e���V���I�y���[�V����
enum V3D_STENCIL_OP : uint8_t
{
	V3D_STENCIL_OP_KEEP = 0, //!< ���݂̒l��ێ����܂��B
	V3D_STENCIL_OP_ZERO = 1, //!< �l�� 0 �ɐݒ肵�܂��B
	V3D_STENCIL_OP_REPLACE = 2, //!< IV3DCommandBuffer::SetStencilReference �Őݒ肳�ꂽ�l�ɒu�������܂��B
	V3D_STENCIL_OP_INCREMENT_AND_CLAMP = 3, //!< ���݂̒l���C���N�������g���A�ő�l�ɃN�����v���܂��B
	V3D_STENCIL_OP_DECREMENT_AND_CLAMP = 4, //!< ���݂̒l���f�N�������g���� 0 �ɃN�����v���܂��B
	V3D_STENCIL_OP_INVERT = 5, //!< ���݂̒l���r�b�g���]���܂��B
	V3D_STENCIL_OP_INCREMENT_AND_WRAP = 6, //!< ���݂̒l���C���N�������g���A�ő�l�𒴂����ꍇ�� 0 �Ƀ��b�v���܂��B
	V3D_STENCIL_OP_DECREMENT_AND_WRAP = 7, //!< ���݂̒l���f�N�������g���A�l�� 0 ��菬�����Ȃ�ƍő�l�Ƀ��b�v���܂��B
};

//! @enum V3D_BLEND_FACTOR
//! @brief �u�����h�W��<br>
//! <table class="basic">
//! <tr><td>Rs0 Gs0 Bs0 As0</td><td>�u�����h�����J���[�A�^�b�`�����g�̑�P�̃\�[�X�J���[</td></tr>
//! <tr><td>Rs1 Gs1 Bs1 As1</td><td>�u�����h�����J���[�A�^�b�`�����g�̑�Q�̃\�[�X�J���[</td></tr>
//! <tr><td>Rd Gd Bd Ad</td><td></td>�o�͐�̃J���[</tr>
//! <tr><td>Rc Gc Bc Ac</td><td></td>�R�}���h IV3DCommandBuffer::SetBlendConstants �Őݒ肷��u�����h�萔</tr>
//! </table>
//! @remarks
//! �ȉ��̂��̂� V3DDeviceCaps::colorBlendFlags �� ::V3D_COLOR_BLEND_CAP_DUAL_SRC ���܂܂�Ă���ꍇ�ɂ̂ݎg�p���邱�Ƃ��ł��܂��B<br>
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
//! @brief �u�����h���Z�q<br>
//! <table class="basic">
//! <tr><td>Cs</td><td>���͌��̃C���v�b�g�A�^�b�`�����g�Ȃǂ̃J���[</td></tr>
//! <tr><td>Cd</td><td>�o�͐�ɂȂ�J���[�A�^�b�`�����g�̃J���[</td></tr>
//! <tr><td>Fs</td><td>���͌��̃u�����h�W��</td></tr>
//! <tr><td>Fd</td><td>�o�͐�̃u�����h�W��</td></tr>
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
//! @brief �J���[�̃R���|�[�l���g�t���O
enum V3D_COLOR_COMPONENT_FLAG : V3DFlags
{
	V3D_COLOR_COMPONENT_R = 0x00000001, //!< �ԗv�f�ł��B
	V3D_COLOR_COMPONENT_G = 0x00000002, //!< �Ηv�f�ł��B
	V3D_COLOR_COMPONENT_B = 0x00000004, //!< �v�f�ł��B
	V3D_COLOR_COMPONENT_A = 0x00000008, //!< �A���t�@�v�f�ł��B
	V3D_COLOR_COMPONENT_ALL = V3D_COLOR_COMPONENT_R | V3D_COLOR_COMPONENT_G | V3D_COLOR_COMPONENT_B | V3D_COLOR_COMPONENT_A, //!< �S�Ă̗v�f�ł��B
};

//! @enum V3D_LOGIC_OP
//! @brief �_�����Z�q<br>
//! <table class="basic">
//! <tr><td>s</td><td>�t���O�����g�ɂ���ďo�͂���� RGBA �����ł��B</td></tr>
//! <tr><td>d</td><td>�J���[�A�^�b�`�����g�ɂ���ē��͂���� RGBA �����ł��B</td></tr>
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
//! @brief �p�C�v���C���̃V�F�[�_�[�̓��ꉻ�萔�̋L�q
//! @sa V3DPipelineShader
struct V3DPipelineShaderConstantDesc
{
	//! @brief �萔�̎��ʎq�𐮐��Ŏw�肵�܂��B<br>
	//! layout(constant_id = id) const
	uint32_t id;
	//! @brief �f�[�^��̒萔�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t offset;
	//! @brief �萔�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t size;
};

//! @struct V3DPipelineShader
//! @brief �p�C�v���C���̃V�F�[�_�[
struct V3DPipelineShader
{
	//! @name ���W���[��
	//! @{

	IV3DShaderModule* pModule; //!< �V�F�[�_�[���W���[���̃|�C���^�ł��B
	const char* pEntryPointName; //!< �V�F�[�_�[�̃G���g���[�|�C���g�̖��O�ł��B

	//! @}

	//! @name ���ꉻ�萔
	//! @{

	uint32_t constantCount; //!< ���ꉻ�萔�̐��ł��B
	V3DPipelineShaderConstantDesc* pConstants; //!< constantCount �̒l�̐��̗v�f������ V3DPipelineShaderConstantDesc �\���̂̔z��ł��B
	void* pConstantData; //!< ���ꉻ�萔�f�[�^�̃|�C���^�ł��B

	//! @}
};

//! @struct V3DPipelineVertexElement
//! @brief �p�C�v���C���̃o�[�e�b�N�X�G�������g
struct V3DPipelineVertexElement
{
	//! @brief ���P�[�V�����ł��B<br>
	//! layout(location = [location])
	uint32_t location;
	//! @brief �I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::maxVertexInputElementOffset �ȉ��ɐ�������܂��B
	uint32_t offset;
	//! @brief �t�H�[�}�b�g�ł��B
	V3D_FORMAT format;
};

//! @struct V3DPipelineVertexLayout
//! @brief �p�C�v���C���̃o�[�e�b�N�X���C�A�E�g
struct V3DPipelineVertexLayout
{
	//! @brief �o�[�e�b�N�X�o�b�t�@�[�̃o�C���h�ʒu�ł��B
	uint32_t binding;
	//! @brief �o�[�e�b�N�X�̃X�g���C�h���o�C�g�P�ʂŎw�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::maxVertexInputBindingStride �ȉ��ɐ�������܂��B
	uint32_t stride;
	//! @brief �o�[�e�b�N�X���C�A�E�g�̍ŏ��̃G�������g�̃C���f�b�N�X�ł��B
	//! @sa V3DPipelineVertexInputDesc::pElements
	uint32_t firstElement;
	//! @brief �G�������g�̐��ł��B
	uint32_t elementCount;
};

//! @struct V3DPipelineVertexInputDesc
//! @brief �o�[�e�b�N�X�C���v�b�g�̋L�q
struct V3DPipelineVertexInputDesc
{
	//! @name �G�������g
	//! @{

	uint32_t elementCount; //!< �o�[�e�b�N�X�G�������g�̐��ł��B
	V3DPipelineVertexElement* pElements; //!< elementCount �̒l�̐��̗v�f������ V3DPipelineVertexElement �\���̂̔z��ł��B

	//! @}

	//! @name ���C�A�E�g
	//! @{

	uint32_t layoutCount; //!< �o�[�e�b�N�X���C�A�E�g�̐��ł��B
	V3DPipelineVertexLayout* pLayouts; //!< layoutCount �̒l�̐��̗v�f������ V3DPipelineVertexLayout �\���̂̔z��ł��B

	//! @}
};

//! @struct V3DPipelineTessellationDesc
//! @brief �p�C�v���C���̃e�b�Z���[�V�����̋L�q
struct V3DPipelineTessellationDesc
{
	uint32_t patchControlPoints; //!< �p�b�`���Ƃ̃R���g���[���|�C���g�̐��ł��B<br>���̒l�� V3DDeviceCaps::maxTessellationPatchSize �ȉ��ɐ�������܂��B
};

//! @struct V3DPipelineRasterizationDesc
//! @brief �p�C�v���C���̃��X�^�[���̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
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
	//! @brief ���X�^���C�Y�X�e�[�W�̒��O�Ƀv���~�e�B�u��j�����邩�ǂ������w�肵�܂��B
	bool discardEnable;
	//! @brief �[�x���N�����v���邩�ǂ������w�肵�܂��B<br>���̒l�� V3DDeviceCaps::rasterizerFlags �� ::V3D_RASTERIZER_CAP_DEPTH_CLAMP ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool depthClampEnable;

	//! @brief �|���S�����[�h�ł��B<br>
	//! V3DDeviceCaps::rasterizerFlags �� ::V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID ���܂܂�Ă��Ȃ������ꍇ�A
	//! ::V3D_POLYGON_MODE_LINE ����� ::V3D_POLYGON_MODE_POINT ���w�肷�邱�Ƃ͂ł��܂���B
	V3D_POLYGON_MODE polygonMode;
	//! @brief �J�����O���[�h�ł��B
	V3D_CULL_MODE cullMode;

	//! @brief �[�x�o�C�A�X��L���ɂ��邩�ǂ������w�肵�܂��B
	bool depthBiasEnable;
	//! @brief ���̐[�x�l�𐧌䂷��W���ł��B
	float depthBiasConstantFactor;
	//! @brief �ő� �܂��� �ŏ��̐[�x�o�C�A�X�ł��B<br>
	//! V3DDeviceCaps::rasterizerFlags �� ::V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP ���܂܂�Ă��Ȃ������ꍇ�� 0.0f ���w�肷��K�v������܂��B
	float depthBiasClamp;
	//! @brief �[�x�o�C�A�X�̌v�Z�ɂ�����X���[�v�ɓK�p�����W���ł��B
	float depthBiasSlopeFactor;
};

//! @struct V3DPipelineMultisampleDesc
//! @brief �p�C�v���C���̃}���`�T���v���̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
//! <tr><td>rasterizationSamples</td><td>V3D_SAMPLE_COUNT_1</td></tr>
//! <tr><td>sampleShadingEnable</td><td>false</td></tr>
//! <tr><td>minSampleShading</td><td>0.0f</td></tr>
//! <tr><td>alphaToCoverageEnable</td><td>false</td></tr>
//! <tr><td>alphaToOneEnable</td><td>false</td></tr>
//! </table>
//! <br>
struct V3DPipelineMultisampleDesc
{
	//! @brief ���X�^�[���̃T���v�����ł��B<br>
	//! V3DDeviceCaps::multisampleFlags �� ::V3D_MULTISAMPLE_CAP_VARIABLE_RATE ���܂܂�Ă��Ȃ������ꍇ�A
	//! �T�u�p�X�̃J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v�����͂��̒l�ł���K�v������܂��B
	V3D_SAMPLE_COUNT_FLAG rasterizationSamples;
	//! @brief �t���O�����g���T���v�����ɃV�F�[�f�B���O�����s���邩�ǂ������w�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::multisampleFlags �� ::V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool sampleShadingEnable;
	//! @brief �T���v�����̃V�F�[�f�B���O�̍ŏ������ł��B<br>
	//! ���̒l�� sampleShadingEnable �� true �̂Ƃ��ɗL���ɂȂ�A0.0f �` 1.0f �̊Ԃɐ�������܂��B
	float minSampleShading;
	//! @brief �A���t�@�g�D�J�o���b�W���g�p���邩�ǂ������w�肵�܂��B
	bool alphaToCoverageEnable;
	//! @brief �t���O�����g�V�F�[�_����o�͂��ꂽ�A���t�@�l���A�Œ菬���_���\���\�ȍő�l�܂��� 1.0 �Œu�������邩�ǂ������w�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::multisampleFlags �� ::V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool alphaToOneEnable;
};

//! @struct V3DPipelineStencilOpDesc
//! @brief �p�C�v���C���̃X�e���V���I�y���[�V�����̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
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
	V3D_STENCIL_OP failOp; //!< �X�e���V���e�X�g�Ɏ��s�����T���v���ɑ΂��Ď��s�����I�y���[�V�����ł��B�B
	V3D_STENCIL_OP passOp; //!< �[���e�X�g�ƃX�e���V���e�X�g�̗����ɍ��i�����T���v���ɑ΂��Ď��s�����I�y���[�V�����ł��B
	V3D_STENCIL_OP depthFailOp; //!< �X�e���V���e�X�g�ɍ��i���A�[�x�e�X�g�ɍ��i���Ȃ������T���v���ɑ΂��Ď��s�����I�y���[�V�����ł��B

	//! @brief �X�e���V���e�X�g�Ŏg�p������r���Z�q�ł��B<br>
	//! ::V3D_COMPARE_OP �̐����ɂ��� R �̓}�X�N���ꂽ IV3DCommandBuffer::SetStencilReference �Őݒ肳�ꂽ�l�ł���AS �̓}�X�N���ꂽ�X�e���V���̒l��\���܂��B
	V3D_COMPARE_OP compareOp;

	//! @brief �X�e���V���e�X�g�ɂ���ēǂݍ��܂��l�̃r�b�g�}�X�N�ł��B
	uint32_t readMask;
	//! @brief �X�e���V���e�X�g�ɂ���ď������܂��l�̃r�b�g�}�X�N�ł��B
	uint32_t writeMask;
};

//! @struct V3DPipelineDepthStencilDesc
//! @brief �p�C�v���C���̃f�v�X�X�e���V���̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
//! <tr><td>depthTestEnable</td><td>true</td></tr>
//! <tr><td>depthWriteEnable</td><td>true</td></tr>
//! <tr><td>depthCompareOp</td><td>V3D_COMPARE_OP_LESS</td></tr>
//! <tr><td>stencilTestEnable</td><td>false</td></tr>
//! <tr><td>stencilFront</td><td>V3DPipelineStencilOpDesc ���Q�Ƃ��Ă��������B</td></tr>
//! <tr><td>stencilBack</td><td>V3DPipelineStencilOpDesc ���Q�Ƃ��Ă��������B</td></tr>
//! <tr><td>depthBoundsTestEnable</td><td>false</td></tr>
//! <tr><td>minDepthBounds</td><td>0.0f</td></tr>
//! <tr><td>maxDepthBounds</td><td>1.0f</td></tr>
//! </table>
//! <br>
struct V3DPipelineDepthStencilDesc
{
	//! @brief �[�x�e�X�g�����邩�ǂ������w�肵�܂��B
	bool depthTestEnable;
	//! @brief �[�x���������ނ��ǂ������w�肵�܂��B
	bool depthWriteEnable;
	//! @brief �[�x�e�X�g�̔�r���Z�q�ł��B<br>
	//! ::V3D_COMPARE_OP �̐����ɂ��� R �͓��͂����[�x�ł���AS �̓A�^�b�`�����g�̐[�x��\���܂��B
	V3D_COMPARE_OP depthCompareOp;

	//! @brief �X�e���V���e�X�g�����邩�ǂ������w�肵�܂��B
	bool stencilTestEnable;
	//! @brief �X�e���V���O�ʂ̃I�y���[�V�����ł��B
	V3DPipelineStencilOpDesc stencilFront;
	//! @brief �X�e���V����ʂ̃I�y���[�V�����ł��B
	V3DPipelineStencilOpDesc stencilBack;

	//! @brief �[�x�̋��E�e�X�g�����邩�ǂ������w�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::depthStencilFlags �� ::V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool depthBoundsTestEnable;
	//! @brief �[�x�̋��E�e�X�g�̍ŏ��l�ł��B<br>
	//! ���̒l�� depthBoundsTestEnable �� true �̂Ƃ��ɗL���ɂȂ�A0.0f �` 1.0f �̊Ԃɐ�������܂��B
	float minDepthBounds;
	//! @brief �[�x�̋��E�e�X�g�̍ő�l�ł��B<br>
	//! ���̒l�� depthBoundsTestEnable �� true �̂Ƃ��ɗL���ɂȂ�A0.0f �` 1.0f �̊Ԃɐ�������܂��B
	float maxDepthBounds;
};

//! @struct V3DPipelineColorBlendAttachment
//! @brief �p�C�v���C���̃J���[�A�^�b�`�����g�̃u�����h
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
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
	bool blendEnable; //!< �u�����h��L���ɂ��邩�ǂ������w�肵�܂��B
	V3D_BLEND_FACTOR srcColorFactor; //!< �u�����h����RGB�����̃u�����h�W���ł��B
	V3D_BLEND_FACTOR dstColorFactor; //!< �u�����h���RGB�����̃u�����h�W���ł��B
	V3D_BLEND_OP colorOp; //!< RGB�����̃u�����h���Z�q�ł��B
	V3D_BLEND_FACTOR srcAlphaFactor; //!< �u�����h����A�����̃u�����h�W���ł��B
	V3D_BLEND_FACTOR dstAlphaFactor; //!< �u�����h���A�����̃u�����h�W���ł��B
	V3D_BLEND_OP alphaOp; //!< A�����̃u�����h���Z�q�ł��B
	V3DFlags writeMask; //!< �J���[�̏������݃}�X�N��\�� ::V3D_COLOR_COMPONENT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DPipelineColorBlendDesc
//! @brief �p�C�v���C���̃J���[�u�����h�̋L�q
//! @remarks
//! �f�t�H���g�l<br>
//! <table class="basic">
//! <tr><th>�X�e�[�g</th><th>�f�t�H���g�l</th></tr>
//! <tr><td>logicOpEnable</td><td>false</td></tr>
//! <tr><td>logicOp</td><td>V3D_LOGIC_OP_COPY</td></tr>
//! <tr><td>attachmentCount</td><td>1 �ȏ�</td></tr>
//! <tr><td>pAttachments</td><td>attachmentCount �̒l�̐��̗v�f������ V3DPipelineColorBlendAttachment �\���̂̔z��</td></tr>
//! </table>
//! <br>
struct V3DPipelineColorBlendDesc
{
	//! @brief �_�����Z��L���ɂ��邩�ǂ������w�肵�܂��B<br>
	//! ���̒l�� V3DDeviceCaps::colorBlendFlags �� ::V3D_COLOR_BLEND_CAP_LOGIC_OP ���܂܂�Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool logicOpEnable;
	//! @brief �_�����Z�̎�ނł��B<br>
	//! ���̒l�� logicOpEnable �� true �̂Ƃ��ɗL���ɂȂ�܂��B
	V3D_LOGIC_OP logicOp;

	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�̐��ł��B
	uint32_t attachmentCount;
	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�̃u�����h�z��ł��B<br>
	//! �z��̗v�f�̐��� attachmentCount �l�Ɠ����ł���K�v����܂��B<br>
	//! �܂� V3DDeviceCaps::colorBlendFlags �� ::V3D_COLOR_BLEND_CAP_INDEPENDENT ���܂܂�Ă��Ȃ������ꍇ�A�A�^�b�`�����g���ɈقȂ�u�����h�����邱�Ƃ��ł��Ȃ����߁A
	//! �z��̗v�f�͂��ׂē����l�łȂ��Ƃ����܂���B<br>
	V3DPipelineColorBlendAttachment* pAttachments;
};

//! @struct V3DGraphicsPipelineDesc
//! @brief �O���t�B�b�N�X�p�C�v���C���̋L�q
struct V3DGraphicsPipelineDesc
{
	V3DPipelineShader vertexShader; //!< �o�[�e�b�N�X�V�F�[�_�[�ł��B
	V3DPipelineShader tessellationControlShader; //!< �e�b�Z�[���[�V��������V�F�[�_�[�ł��B
	V3DPipelineShader tessellationEvaluationShader; //!< �e�b�Z�[���[�V�����]���V�F�[�_�[�ł��B
	V3DPipelineShader geometryShader; //!< �W�I���g���V�F�[�_�[�ł��B
	V3DPipelineShader fragmentShader; //!< �t���O�����g�V�F�[�_�[�ł��B

	V3DPipelineVertexInputDesc vertexInput; //!< �o�[�e�b�N�X�C���v�b�g�ł��B

	V3D_PRIMITIVE_TOPOLOGY primitiveTopology; //!< �v���~�e�B�u�g�|���W�ł��B

	V3DPipelineTessellationDesc tessellation; //!< �e�b�Z�[���[�V�����ł��B
	V3DPipelineRasterizationDesc rasterization; //!< ���X�^�[���ł��B
	V3DPipelineMultisampleDesc multisample; //!< �}���`�T���v���ł��B

	V3DPipelineDepthStencilDesc depthStencil; //!< �f�v�X�X�e���V���ł��B
	V3DPipelineColorBlendDesc colorBlend; //!< �J���[�u�����h�ł��B

	IV3DRenderPass* pRenderPass; //!< �����_�[�p�X�ł��B
	uint32_t subpass; //!< �T�u�p�X�̃C���f�b�N�X�ł��B
};

//! @struct V3DComputePipelineDesc
//! @brief �R���s���[�g�p�C�v���C���̋L�q
struct V3DComputePipelineDesc
{
	V3DPipelineShader computeShader; //!< �R���s���[�g�V�F�[�_�[�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DPipeline
//! @brief �p�C�v���C���C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateGraphicsPipeline
//! @sa IV3DDevice::CreateComputePipeline
class V3D_DLL_API IV3DPipeline : public IV3DDeviceChild
{
public:
	//! @brief �p�C�v���C���̃^�C�v���擾���܂��B
	//! @return �p�C�v���C���̃^�C�v��\�� V3D_PIPELINE_TYPE �񋓒萔�̂����ꂩ��Ԃ��܂��B
	virtual V3D_PIPELINE_TYPE GetType() const = 0;
	//! @brief �p�C�v���C���̃��C�A�E�g���擾���܂��B
	//! @param[out] ppPipelineLayout �p�C�v���C���̃��C�A�E�g��\�� IV3DPipelineLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetLayout(IV3DPipelineLayout** ppPipelineLayout) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DPipeline() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �X���b�v�`�F�C��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_callback_group
//! @{

//! @fn void PV3DLostSwapChainFunction(void* pUserData)
//! @brief �X���b�v�`�F�C�������X�g�����ۂɌĂяo�����R�[���o�b�N�֐��̒�`�ł��B
//! @param pUserData ���[�U�[�f�[�^�ł��B
typedef void (V3D_CALLBACK *PV3DLostSwapChainFunction)(void* pUserData);
//! @fn void PV3DRestoreSwapChainFunction(void* pUserData)
//! @brief �X���b�v�`�F�C�������A�����ۂɌĂяo�����R�[���o�b�N�֐��̒�`�ł��B
//! @param pUserData ���[�U�[�f�[�^�ł��B
typedef void (V3D_CALLBACK *PV3DRestoreSwapChainFunction)(void* pUserData);

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DSwapChainCallbacks
//! @brief �X���b�v�`�F�C���̃R�[���o�b�N
struct V3DSwapChainCallbacks
{
	PV3DLostSwapChainFunction pLostFunction; //!< �X���b�v�`�F�C�������X�g�����ۂɌĂяo�����R�[���o�b�N�֐��̃|�C���^�ł��B
	PV3DRestoreSwapChainFunction pRestoreFunction; //!< �X���b�v�`�F�C�������A�����ۂɌĂяo�����R�[���o�b�N�֐��̃|�C���^�ł��B
	void* pUserData; //!< �R�[���o�b�N�֐��ɓn����郆�[�U�[�f�[�^�ł��B
};

//! @struct V3DSwapChainDesc
//! @brief �X���b�v�`�F�C���̋L�q
struct V3DSwapChainDesc
{
	HWND windowHandle; //!< �E�B���h�E�̃n���h���ł��B

	V3D_FORMAT imageFormat; //!< �C���[�W�̃t�H�[�}�b�g�ł��B
	uint32_t imageWidth; //!< �C���[�W�̕��ł��B
	uint32_t imageHeight; //!< �C���[�W�̍����ł��B
	uint32_t imageCount; //!< �C���[�W�̐��ł��B
	V3DFlags imageUsageFlags; //!< �C���[�W�̎g�p�@�ł��B

	//! @brief �L���[�t�@�~���[�̃C���f�b�N�X�ł��B
	uint32_t queueFamily;
	//! @brief �L���[�ɑ��M�����ۂɑҋ@����������p�C�v���C���X�e�[�W�̃}�X�N�ł��B
	//! �Ō�̃R�}���h�ɂ͂��̃p�C�v���C���X�e�[�W���܂܂�Ă���K�v������܂��B
	V3DFlags queueWaitDstStageMask;

	bool fullscreenAssistEnable; //!< �t���X�N���[���̐؂�ւ��� �^�C�g���o�[�̃_�u���N���b�N�A�ő剻�{�^���AALT + ENTER �L�[�ɂ���Ăł���悤�ɂ��܂��B
	bool vsyncEnable; //!< ����������ҋ@���邩�ǂ������w�肵�܂��B
	bool windowed; //!< �J�n���̕\�����[�h���E�B���h�E���[�h�ɂ��邩�ǂ������w�肵�܂��A
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DSwapChain
//! @brief �X���b�v�`�F�C���C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateSwapChain
class V3D_DLL_API IV3DSwapChain : public IV3DDeviceChild
{
public:
	//! @brief �X���b�v�`�F�C���̋L�q���擾���܂��B
	//! @return �X���b�v�`�F�C���̋L�q��\�� V3DSwapChainDesc �\���̂̃A�h���X��Ԃ��܂��B
	virtual const V3DSwapChainDesc& GetDesc() const = 0;

	//! @brief �����_�����O�C���[�W���l�����܂��B
	//! @retval V3D_OK �����_�����O�C���[�W�̊l���ɐ������܂����B
	//! @retval V3D_TIMEOUT �^�C���A�E�g�������߃����_�����O�C���[�W���l���ł��܂���ł����B
	//! @retval V3D_ERROR_FAIL �����_�����O�C���[�W�̊l���Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT AcquireNextImage() = 0;

	//! @brief ���݂̃C���[�W�̃C���f�b�N�X���擾���܂��B
	//! @return ���݂̃C���[�W�̃C���f�b�N�X��Ԃ��܂��B
	//! @remarks �Ԃ����l�� AcquireNextImage �ōX�V����܂��B
	virtual uint32_t GetCurrentImageIndex() const = 0;
	//! @brief �w�肵���C���f�b�N�X�̃C���[�W���擾���܂��B
	//! @param[in] imageIndex �擾����C���[�W�̃C���f�b�N�X�ł��B
	//! @param[out] ppImage �擾�����C���[�W��n�� IV3DImage �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetImage(uint32_t imageIndex, IV3DImage** ppImage) = 0;

	//! @brief �\�����[�h�̐ݒ���擾���܂��B
	//! @return �\�����[�h���S��ʃ��[�h�̏ꍇ�� true ��Ԃ��܂��B
	virtual bool GetFullscreenState() const = 0;
	//! @brief �\�����[�h���E�B���h�E���[�h�܂��͑S��ʃ��[�h�ɐݒ肵�܂��B
	//! @param[in] bFullscreen �S��ʃ��[�h�ɂ��邩�ǂ������w�肵�܂��B
	//! @retval V3D_OK �\�����[�h�̐ݒ�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �\�����[�h�̐ݒ�Ɏ��s���܂����B<br>
	//! ���łɑ��̃X���b�v�`�F�C�����S��ʃ��[�h�̏ꍇ�ɁA���̃X���b�v�`�F�C����S��ʃ��[�h�ɂ��邱�Ƃ͂ł��܂���B<br>
	//! ����ȊO�̏ꍇ�͓����G���[�����������\��������܂��B
	virtual V3D_RESULT SetFullscreenState(bool bFullscreen) = 0;

protected:
	//! @cond MISC
	virtual ~IV3DSwapChain() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �R�}���h�v�[��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DCommandPoolDesc
//! @brief �R�}���h�v�[���̋L�q
struct V3DCommandPoolDesc
{
	uint32_t queueFamily; //!< �L���[�t�@�~���[�ł��B
	V3DFlags usageFlags; //!< �g�p�@��\�� ::V3D_COMMAND_POOL_USAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @}

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_POOL_USAGE_FLAG
//! @brief �R�}���h�v�[���̎g�p�@�t���O
enum V3D_COMMAND_POOL_USAGE_FLAG : V3DFlags
{
	//! @brief �R�}���h�v�[���[���犄�蓖�Ă�ꂽ�R�}���h�o�b�t�@�[����r�I�Z���ԂɃ��Z�b�g�܂��͉������邱�Ƃ�\���܂��B
	V3D_COMMAND_POOL_USAGE_TRANSIENT = 0x00000001,
	//! @brief �R�}���h�v�[���[���犄�蓖�Ă�ꂽ�R�}���h�o�b�t�@�[���ʂɃ��Z�b�g�ł���悤�ɂ��܂��B<br>
	//! ���̃t���O���w�肳�ꂽ�ꍇ�� IV3DCommandBuffer::Reset �ɂ�郊�Z�b�g���ł� IV3DCommandBuffer::Begin �ɂ��ÖٓI�ȃ��Z�b�g���s���܂��B<br>
	//! �܂��A���̃t���O���w�肳��Ȃ������ꍇ IV3DCommandBuffer::Reset �͎g�p�ł��� IV3DCommandPool::Reset �ɂ���ăR�}���h�o�b�t�@�[���ꊇ�Ń��Z�b�g����悤�ɂȂ�܂��B
	V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER = 0x00000002,
};

//! @enum V3D_COMMAND_POOL_RESET_FLAG
//! @brief �R�}���h�v�[���̃��Z�b�g�t���O
enum V3D_COMMAND_POOL_RESET_FLAG : V3DFlags
{
	//! @brief ���Z�b�g�����ۂɃR�}���h�o�b�t�@�[�����L���郁�������\�[�X���R�}���h�v�[���[�ɕԋp���܂��B<br>
	//! �܂����̃t���O���w�肳��Ȃ������ꍇ�̓��������\�[�X��ԋp�����A�ė��p���܂��B
	V3D_COMMAND_POOL_RESET_RELEASE_RESOURCES = 0x00000001,
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DCommandPool
//! @brief �R�}���h�v�[���C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateCommandPool
class V3D_DLL_API IV3DCommandPool : public IV3DDeviceChild
{
public:
	//! @brief �R�}���h�v�[���̋L�q���擾���܂��B
	//! @return �R�}���h�v�[���̋L�q��Ԃ��܂��B
	virtual const V3DCommandPoolDesc& GetDesc() const = 0;

	//! @brief �R�}���h�v�[�������Z�b�g���܂�
	//! @param[in] resetFlags ���Z�b�g�t���O��\�� ::V3D_COMMAND_POOL_RESET_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �R�}���h�v�[���̃��Z�b�g�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�v�[���̃��Z�b�g�Ɏ��s���܂����B
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
// �R�}���h�o�b�t�@�[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_BUFFER_TYPE
//! @brief �R�}���h�o�b�t�@�[�̃��x��
enum V3D_COMMAND_BUFFER_TYPE : uint8_t
{
	//! @brief �L���[�ɑ��M���邱�Ƃ��ł���A�v���C�}���R�}���h�o�b�t�@�[�ł��B
	//! @sa IV3DQueue::Submit
	V3D_COMMAND_BUFFER_TYPE_PRIMARY = 0,
	//! @brief �v���C�}���R�}���h�o�b�t�@�[�Ŏ��s���邱�Ƃ��ł���A�Z�J���_���R�}���h�o�b�t�@�[�ł��B
	//! @sa IV3DCommandBuffer::ExecuteCommandBuffers
	V3D_COMMAND_BUFFER_TYPE_SECONDARY = 1,
};

//! @enum V3D_COMMAND_BUFFER_RESET_FLAG
//! @brief �R�}���h�o�b�t�@�[�̃��Z�b�g�t���O
enum V3D_COMMAND_BUFFER_RESET_FLAG : V3DFlags
{
	//! @brief ���Z�b�g�����ۂɃR�}���h�o�b�t�@�����L���郁�������\�[�X���R�}���h�v�[���[�ɕԋp���܂��B<br>
	//! �܂����̃t���O���w�肳��Ȃ������ꍇ�̓��������\�[�X��ԋp�����A�ė��p���܂��B
	V3D_COMMAND_BUFFER_RESET_RELEASE_RESOURCES = 0x00000001,
};

//! @enum V3D_COMMAND_BUFFER_USAGE_FLAG
//! @brief �R�}���h�o�b�t�@�̎g�p�@�t���O
enum V3D_COMMAND_BUFFER_USAGE_FLAG : V3DFlags
{
	//! @brief �R�}���h�o�b�t�@�[�͈�x�����L���[�ɑ��M����邱�Ƃ�\���܂��B
	V3D_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001,
	//! @brief �v���C�}���R�}���h�o�b�t�@�[�ŊJ�n���ꂽ�����_�[�p�X���Ŏ��s�����Z�J���_���R�}���h�o�b�t�@�[�������_�[�p�X�A�T�u�p�X�A�t���[���o�b�t�@�������p�����Ƃ�\���܂��B
	V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE = 0x00000002,
	//! @brief �R�}���h�o�b�t�@�[�������̃L���[�ɑ��M (IV3DQueue::Submit) �܂��̓v���C�}���R�}���h�o�b�t�@�[�ɋL�^ ( IV3DCommandBuffer::ExecuteCommandBuffers ) �ł��邱�Ƃ�\���܂��B<br>
	//! �܂��A�v���C�}���R�}���h�o�b�t�@�[�ɂ��̎g�p�@���܂܂�Ă���ꍇ�A�����Ŏ��s�����Z�J���_���R�}���h�o�b�t�@�[�����l�ɂ��̎g�p�@���܂܂�Ă���K�v������܂��B
	V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004,
};

//! @enum V3D_STENCIL_FACE_FLAG
//! @brief �X�e���V���̃t�F�C�X�t���O
enum V3D_STENCIL_FACE_FLAG : V3DFlags
{
	V3D_STENCIL_FACE_FRONT = 0x00000001, //!< �O���w�肵�܂��B
	V3D_STENCIL_FACE_BACK = 0x00000002, //!< �����w�肵�܂��B
	V3D_STENCIL_FACE_BOTH = V3D_STENCIL_FACE_FRONT | V3D_STENCIL_FACE_BACK, //!< �O�㗼�����w�肵�܂��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBarrierBufferDesc
//! @brief �o�b�t�@�[�o���A�̋L�q
struct V3DBarrierBufferDesc
{
	V3DFlags srcStageMask; //!< �ڍs���̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� ::V3D_DEPENDENCY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< �ڍs���̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< �ڍs���̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B

	uint64_t offset; //!< �������̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t size; //!< �������̃I�t�Z�b�g����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
};

//! @struct V3DBarrierBufferViewDesc
//! @brief �o�b�t�@�[�r���[�o���A�̋L�q
struct V3DBarrierBufferViewDesc
{
	V3DFlags srcStageMask; //!< �ڍs���̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� ::V3D_DEPENDENCY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< �ڍs���̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< �ڍs���̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B
};

//! @struct V3DBarrierImageDesc
//! @brief �C���[�W�o���A�̋L�q
struct V3DBarrierImageDesc
{
	V3DFlags srcStageMask; //!< �ڍs���̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� ::V3D_DEPENDENCY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< �ڍs���̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< �ڍs���̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B

	V3D_IMAGE_LAYOUT srcLayout; //!< �ڍs���̃C���[�W���C�A�E�g�ł��B
	V3D_IMAGE_LAYOUT dstLayout; //!< �ڍs��̃C���[�W���C�A�E�g�ł��B

	uint32_t baseLevel; //!< �ŏ��̃��x���ł��B
	uint32_t levelCount; //!< ���x�����ł��B<br>0 ���w�肷��� baseLevel ����Ō�̃��x���܂ł��w�肵�����ƂɂȂ�܂��B
	uint32_t baseLayer; //!< �ŏ��̃��C���[�ł��B
	uint32_t layerCount; //!< ���C���[���ł��B<br>0 ���w�肷��� baseLayer ����Ō�̃��C���[�܂ł��w�肵�����ƂɂȂ�܂��B
};

//! @struct V3DBarrierImageViewDesc
//! @brief �C���[�W�r���[�o���A�̋L�q
struct V3DBarrierImageViewDesc
{
	V3DFlags srcStageMask; //!< �ڍs���̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃p�C�v���C���X�e�[�W��\�� ::V3D_PIPELINE_STAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� ::V3D_DEPENDENCY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< �ڍs���̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� ::V3D_ACCESS_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< �ڍs���̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B

	V3D_IMAGE_LAYOUT srcLayout; //!< �ڍs���̃C���[�W���C�A�E�g�ł��B
	V3D_IMAGE_LAYOUT dstLayout; //!< �ڍs��̃C���[�W���C�A�E�g�ł��B
};

//! @struct V3DImageSubresourceRange
//! @brief �C���[�W�̃T�u���\�[�X���R�s�[����͈�
struct V3DImageSubresourceRange
{
	uint32_t level; //!< ���x���ł��B
	uint32_t baseLayer; //!< �ŏ��̃��C���[�ł��B
	uint32_t layerCount; //!< ���C���[���ł��B
};

//! @struct V3DCopyImageRange
//! @brief �C���[�W���R�s�[����͈�
struct V3DCopyImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< �R�s�[��̃T�u���\�[�X�ł��B
	V3DPoint3D dstOffset; //!< �R�s�[��̃I�t�Z�b�g�ł��B

	V3DImageSubresourceRange srcSubresource; //!< �R�s�[���̃T�u���\�[�X�ł��B
	V3DPoint3D srcOffset; //!< �R�s�[���̃I�t�Z�b�g�ł��B

	V3DSize3D size; //!< �R�s�[����T�C�Y�ł��B
};

//! @struct V3DCopyBufferToImageRange
//! @brief �o�b�t�@���C���[�W�ɃR�s�[����͈�
struct V3DCopyBufferToImageRange
{
	V3DImageSubresourceRange dstImageSubresource; //!< �R�s�[��C���[�W�̃T�u���\�[�X�ł��B
	V3DPoint3D dstImageOffset; //!< �R�s�[��C���[�W�̃I�t�Z�b�g�ł��B
	V3DSize3D dstImageSize; //!< �R�s�[��C���[�W�̃T�C�Y

	uint64_t srcBufferOffset; //!< �R�s�[���o�b�t�@�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
};

//! @struct V3DCopyImageToBufferRange
//! @brief �C���[�W���o�b�t�@�ɃR�s�[����͈�
struct V3DCopyImageToBufferRange
{
	uint64_t dstBufferOffset; //!< �R�s�[��o�b�t�@�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B

	V3DImageSubresourceRange srcImageSubresource; //!< �R�s�[���C���[�W�̃T�u���\�[�X�ł��B
	V3DPoint3D srcImageOffset; //!< �R�s�[���C���[�W�̃I�t�Z�b�g�ł��B
	V3DSize3D srcImageSize; //!< �R�s�[���C���[�W�̃T�C�Y�ł��B
};

//! @struct V3DBlitImageRange
//! @brief �C���[�W��]������͈�
struct V3DBlitImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< �]����̃T�u���\�[�X�ł��B
	V3DRectangle3D dstRect; //!< �]����̗̈�ł��B

	V3DImageSubresourceRange srcSubresource; //!< �]�����̃T�u���\�[�X�ł��B
	V3DRectangle3D srcRect; //!< �]�����̗̈�ł��B
};

//! @struct V3DResolveImageRange
//! @brief �C���[�W��ϊ�����͈�
struct V3DResolveImageRange
{
	V3DImageSubresourceRange dstSubresource; //!< �ϊ���̃T�u���\�[�X�ł��B
	V3DPoint3D dstOffset; //!< �ϊ���̃I�t�Z�b�g�ł��B

	V3DImageSubresourceRange srcSubresource; //!< �ϊ����̃T�u���\�[�X�ł��B
	V3DPoint3D srcOffset; //!< �ϊ����̃I�t�Z�b�g�ł��B

	V3DSize3D size; //!< �ϊ�����T�C�Y�ł��B
};

//! @struct V3DClearColorAttachmentDesc
//! @brief �J���[�A�^�b�`�����g���N���A���邽�߂̋L�q
struct V3DClearColorAttachmentDesc
{
	uint32_t attachment; //!< �T�u�p�X�̃J���[�A�^�b�`�����g�̃C���f�b�N�X�ł��B
	V3DClearColorValue value; //!< �N���A�l�ł��B
};

//! @struct V3DClearDepthStencilAttachmentDesc
//! @brief �f�X�v�X�e���V���A�^�b�`�����g���N���A���邽�߂̋L�q
struct V3DClearDepthStencilAttachmentDesc
{
	//! @brief �[�x���N���A���邩�ǂ������w�肵�܂��B<br>
	//! �N���A���悤�Ƃ��Ă���f�v�X�X�e���V���A�^�b�`�����g�̃t�H�[�}�b�g�ɐ[�x���܂܂�Ă���K�v������܂��B
	bool depthEnable;
	//! @brief �X�e���V�����N���A���邩�ǂ������w�肵�܂��B<br>
	//! �N���A���悤�Ƃ��Ă���f�v�X�X�e���V���A�^�b�`�����g�̃t�H�[�}�b�g�ɃX�e���V�����܂܂�Ă���K�v������܂��B
	bool stencilEnable;
	//! @brief �N���A�l�ł��B
	V3DClearDepthStencilValue value;
};

//! @struct V3DClearRange
//! @brief �N���A�͈�
struct V3DClearRange
{
	V3DRectangle2D rect; //!< �N���A����̈�ł��B
	uint32_t baseLayer; //!< �N���A����ŏ��̃��C���[�ł��B
	uint32_t layerCount; //!< �N���A���郌�C���[�̐��ł��B
};

//! @struct V3DCopyBufferRange
//! @brief �o�b�t�@�[�̃R�s�[�͈�
struct V3DCopyBufferRange
{
	uint64_t srcOffset; //!< �R�s�[���̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t dstOffset; //!< �R�s�[��̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t size; //!< �R�s�[���郁�����T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DCommandBuffer
//! @brief �R�}���h�o�b�t�@�[�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateCommandBuffer
class V3D_DLL_API IV3DCommandBuffer : public IV3DDeviceChild
{
public:
	//! @name �S��
	//! @{

	//! @brief �R�}���h�o�b�t�@�[�̃^�C�v���擾���܂��B
	//! @return �R�}���h�o�b�t�@�[�̃^�C�v��Ԃ��܂��B
	virtual V3D_COMMAND_BUFFER_TYPE GetType() const = 0;
	//! @brief �R�}���h�v�[�����擾���܂��B
	//! @param[out] ppPool �擾�����R�}���h�v�[����n�� IV3DCommandPool �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetPool(IV3DCommandPool** ppPool) = 0;

	//! @brief �R�}���h�o�b�t�@�[�����Z�b�g���܂��B
	//! @param[in] resetFlags �R�}���h�o�b�t�@�[�̃��Z�b�g�t���O��\�� ::V3D_COMMAND_BUFFER_RESET_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �R�}���h�o�b�t�@�[�̃��Z�b�g�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �R�}���o�b�t�@�[�̃��Z�b�g�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! ���̃R�}���h�o�b�t�@�[���A�N�Z�X����R�}���h�v�[���쐬���Ɏw�肵���g�p�@�ł��� V3DCommandPoolDesc::usageFlags �� ::V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER ���܂܂�Ă��Ȃ��ꍇ�A
	//! ���̃��\�b�h�͋@�\���܂���B
	virtual V3D_RESULT Reset(V3DFlags resetFlags) = 0;

	//! @brief �R�}���h�̏������݂��J�n���܂��B
	//! @retval V3D_OK �R�}���h�̏������݂̊J�n�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�̏������݂̊J�n�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! ���̃��\�b�h�̓Z�J���_���R�}���h�o�b�t�@�[�Ŏg�p���邱�Ƃ͂ł��܂���B
	virtual V3D_RESULT Begin() = 0;
	//! @brief �R�}���h�̏������݂��J�n���܂��B
	//! @param[in] usageFlags �R�}���h�o�b�t�@�[�̎g�p�@��\�� ::V3D_COMMAND_BUFFER_USAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pRenderPass �v���C�}���R�}���h�o�b�t�@�[��������p�������_�[�p�X�ł��B
	//! @param[in] subpass �v���C�}���R�}���h�o�b�t�@�[��������p���T�u�p�X�ł��B
	//! @param[in] pFrameBuffer �v���C�}���R�}���h�o�b�t�@�[��������p���t���[���o�b�t�@�[�ł��B
	//! @retval V3D_OK �R�}���h�̏������݂̊J�n�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�̏������݂̊J�n�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! usageFlags �� ::V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肵���ꍇ�� pRenderPass �� subpass �͕K���w�肵�Ă��������B<br>
	//! �܂� pFrameBuffer �� nullptr ���w�肷�邱�Ƃ��\�ł����A�p�t�H�[�}���X���l���������ꍇ�͗L���ȃt���[���o�b�t�@���w�肷�邱�Ƃ𐄏����܂��B
	virtual V3D_RESULT Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer) = 0;
	//! @brief �R�}���h�̏������݂��I�����܂��B
	//! @retval V3D_OK �R�}���h�̏������݂̏I���ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�̏������݂̏I���Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT End() = 0;

	//! @}

	//! @name �R�}���h
	//! @{

	//! @brief �o�b�t�@�[�Ƀo���A�𒣂�܂��B
	//! @param[in] pBuffer �o���A�𒣂�o�b�t�@�[�ł��B
	//! @param[in] barrier �o���A�𒣂�o�b�t�@�[�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBuffer(IV3DBuffer* pBuffer, const V3DBarrierBufferDesc& barrier) = 0;

	//! @brief �o�b�t�@�[�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] pBufferView �o���A�𒣂�o�b�t�@�r���[�ł��B
	//! @param[in] barrier �o���A�𒣂�o�b�t�@�[�r���[�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBufferView(IV3DBufferView* pBufferView, const V3DBarrierBufferViewDesc& barrier) = 0;

	//! @brief �o�b�t�@�[�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] bufferViewCount �o���A�𒣂�o�b�t�@�r���[�̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] ppBufferViews �o���A�𒣂�o�b�t�@�r���[�̔z��ł��B
	//! - �z��̗v�f���́AbufferViewCount �ł���K�v������܂��B
	//! @param[in] barrier �o���A�𒣂�o�b�t�@�[�r���[�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBufferViews(uint32_t bufferViewCount, IV3DBufferView** ppBufferViews, const V3DBarrierBufferViewDesc& barrier) = 0;

	//! @brief �C���[�W�Ƀo���A�𒣂�܂��B
	//! @param[in] pImage �o���A�𒣂�C���[�W�ł��B
	//! @param[in] barrier �o���A�𒣂�C���[�W�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImage(IV3DImage* pImage, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief �C���[�W�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] pImageView �o���A�𒣂�C���[�W�r���[�ł��B
	//! @param[in] barrier �o���A�𒣂�C���[�W�r���[�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImageView(IV3DImageView* pImageView, const V3DBarrierImageViewDesc& barrier) = 0;

	//! @brief �C���[�W�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] imageVewCount �o���A�𒣂�C���[�W�r���[�̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] ppImageViews �o���A�𒣂�C���[�W�r���[�̔z��ł��B
	//! - �z��̗v�f���́AimageVewCount �ł���K�v������܂��B
	//! @param[in] barrier �o���A�𒣂�C���[�W�r���[�̋L�q�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImageViews(uint32_t imageVewCount, IV3DImageView** ppImageViews, const V3DBarrierImageViewDesc& barrier) = 0;

	//! @brief �o�b�t�@�[���w�肵���l�œh��Ԃ��܂�
	//! @param[in] pDstBuffer �h��Ԃ��o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstOffset �h��Ԃ��o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= dstOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] size �h��Ԃ��T�C�Y���o�C�g�P�ʂŎw�肵�܂��B<br>
	//! - ���̒l�� 4 �̐����{�ł���K�v������A{ 4 <= size <= (V3DBufferDesc::size - dstOffset) } �͈̔͂ɐ�������܂��B
	//! @param[in] data �h��Ԃ��C�ӂ̒l���w�肵�܂��B
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void FillBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t size, uint32_t data) = 0;

	//! @brief �o�b�t�@�[���X�V���܂��B
	//! @param[in] pDstBuffer �X�V����o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstOffset �X�V����o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� 4 �̐����{�ł���K�v������ { 0 <= dstOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] srcSize �X�V����\�[�X�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� 4 �̐����{�ł���K�v������܂��B
	//! - ���̒l�� 65536 �o�C�g�ȏ�̏ꍇ�͓����ŕ������čX�V���邽�߁A�p�t�H�[�}���X���ቺ���邱�Ƃɒ��ӂ��Ă��������B
	//! @param[in] pSrc �X�V����\�[�X�ł��B
	//! - nullptr ���w�肷�邱�Ƃ͂ł��܂���B
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void UpdateBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, uint64_t srcSize, const void* pSrc) = 0;

	//! @brief �o�b�t�@�[���R�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstOffset �R�s�[��̃o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= dstOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcOffset �R�s�[���̃o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= dstOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] size �R�s�[����T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 1 <= size <= min(dstBufferSize - dstOffset, srcBufferSize - srcOffset) } �͈̔͂ɐ�������܂��B
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size) = 0;

	//! @brief �o�b�t�@�[���R�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ���w�肷��K�v������܂��B
	//! @param[in] pRanges �R�s�[����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @par
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges) = 0;

	//! @brief �C���[�W���R�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges �R�s�[����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @remarks
	//! �R�s�[���������C���[�W�́A�^�C�v�A�T���v��������v���Ă���K�v������܂��B<br>
	//! �܂��t�H�[�}�b�g�̓R�s�[���� V3D_FORMAT_R8G8B8A8_UNORM�A�R�s�[�悪 V3D_FORMAT_R32_UINT �̂悤�ɈقȂ��Ă��Ă��A�G�������g�̃T�C�Y����v���Ă���΃R�s�[���邱�Ƃ��\�ł��B<br>
	//! �������f�v�X�X�e���V���̏ꍇ�A�t�H�[�}�b�g�͈�v���Ă��Ȃ���΂Ȃ�܂���B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageRange* pRanges) = 0;

	//! @brief �C���[�W���R�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B<br>
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ�w�肵�܂��B
	//! �܂� pSrcImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @remarks
	//! �R�s�[���������C���[�W�́A�^�C�v�A�T�C�Y�A���x�����A���C���[���A�T���v��������v���Ă���K�v������܂��B<br>
	//! �܂��t�H�[�}�b�g�̓R�s�[���� V3D_FORMAT_R8G8B8A8_UNORM�A�R�s�[�悪 V3D_FORMAT_R32_UINT �̂悤�ɈقȂ��Ă��Ă��A�G�������g�̃T�C�Y����v���Ă���΃R�s�[���邱�Ƃ��\�ł��B<br>
	//! �������f�v�X�X�e���V���̏ꍇ�A�t�H�[�}�b�g�͈�v���Ă��Ȃ���΂Ȃ�܂���B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief �o�b�t�@�[���C���[�W�ɃR�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W�̃��C�A�E�g�ł��B
	//! - ���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] dstLevel �R�s�[��̃C���[�W�̃��x���ł��B<br>
	//! - ���̒l�� { 0 <= dstLevel < V3DImageDesc::levelCount } �͈̔͂ɐ�������܂��B
	//! @param[in] dstBaseLayer �R�s�[��̃C���[�W�̍ŏ��̃��C���[�ł��B
	//! - ���̒l�� { 0 <= dstBaseLayer < V3DImageDesc::layerCount } �͈̔͂ɐ�������܂��B
	//! @param[in] dstLayerCount �R�s�[��̃C���[�W�̃��C���[���ł��B
	//! - ���̒l�� { 1 <= dstLayerCount <= (V3DImageDesc::layerCount - dstBaseLayer) } �͈̔͂ɐ�������܂��B
	//! @param[in] dstOffset �R�s�[��̃C���[�W�̃I�t�Z�b�g�ł��B
	//! - ���̒l�� { (0, 0, 0) <= dstOffset < (V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) } �͈̔͂ɐ�������܂��B
	//! @param[in] dstSize �R�s�[��̃C���[�W�̃T�C�Y�ł��B
	//  - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcBufferOffset �R�s�[���̃o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= srcBufferOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! - ( V3DBufferDesc::size - srcBufferOffset ) �̒l�́A���ۂɃR�s�[�����T�C�Y�Ɠ����ł��邩�A�����Ă��Ȃ���΂Ȃ�܂���B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize,
		IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset) = 0;

	//! @brief �o�b�t�@�[���C���[�W�ɃR�s�[���܂�
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W�̃��C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �A ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges �R�s�[����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DBuffer* pSrcBuffer,
		uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges) = 0;

	//! @brief �C���[�W���o�b�t�@�[�ɃR�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstBufferOffset �R�s�[��̃o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= dstBufferOffset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W�̃��C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �A::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] srcLevel �R�s�[���̃C���[�W�̃��x���ł��B
	//! - ���̒l�� { 0 <= srcLevel < V3DImageDesc::levelCount } �͈̔͂ɐ�������܂��B
	//! @param[in] srcBaseLayer �R�s�[���̃C���[�W�̍ŏ��̃��C���[�ł��B
	//! - ���̒l�� { 0 <= srcBaseLayer < V3DImageDesc::layerCount } �͈̔͂ɐ�������܂��B
	//! @param[in] srcLayerCount �R�s�[���̃C���[�W�̃��C���[���ł��B
	//! - ���̒l�� { 1 <= srcLayerCount <= (V3DImageDesc::layerCount - srcBaseLayer) } �͈̔͂ɐ�������܂��B
	//! @param[in] srcOffset �R�s�[���̃C���[�W�̃I�t�Z�b�g�ł��B
	//! - ���̒l�� { (0, 0, 0) <= srcOffset < (V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) } �͈̔͂ɐ�������܂��B
	//! @param[in] srcSize �R�s�[���̃C���[�W�̃T�C�Y�ł��B
	//! - ���̒l�� { {1, 1, 1} <= srcSize <= ((V3DImageDesc::width, V3DImageDesc::height, V3DImageDesc::depth) - srcOffset) } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize) = 0;

	//! @brief �C���[�W���o�b�t�@�[�ɃR�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W�̃��C�A�E�g�ł��B
	//! - ���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImage �̓R�s�[���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges �R�s�[����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges) = 0;

	//! @brief �C���[�W��]�����܂��B
	//! @param[in] pDstImage �]����̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �]����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImage �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImage �]�����̃C���[�W�ł��B
	//! - �C���[�W�쐬���̎g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �]�����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImage �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] rangeCount �]������͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges rangeCount �l�̐��̗v�f������ V3DBlitImageRange �\���̂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @param[in] filter �t�B���^�[�ł��B
	//! @remarks
	//! �]�����������C���[�W�́A�t�H�[�}�b�g�ɂ���Ă͎g�p�ł��Ȃ��ꍇ������܂��̂� IV3DDevice::CheckImageFormatFeature ���g�p���Ď��O�Ɋm�F���Ă����K�v������܂��B<br>
	//! �܂� IV3DDevice::CheckImageFormatFeature �̑�Q���� �܂��� ��R�����ł��� featureFlags �Ɏw�肷��l�͈ȉ��̂悤�ɂȂ�܂��B<br>
	//! - �]����C���[�W ( pDstImage )<br>
	//! ::V3D_IMAGE_FORMAT_FEATURE_BLIT_DST<br>
	//! - �]�����C���[�W ( pSrcImage )<br>
	//! ::V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC<br>
	//! �܂� filter �� ::V3D_FILTER_LINEAR ���w�肷��ꍇ�� ::V3D_IMAGE_FORMAT_FEATURE_SAMPLED_FILTER_LINEAR ���܂߂Ă��������B<br>
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BlitImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, V3DBlitImageRange* pRanges,
		V3D_FILTER filter) = 0;

	//! @brief �C���[�W�r���[��]�����܂��B
	//! @param[in] pDstImageView �]����̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �]����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImageView �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] dstRect �]����̗̈�ł��B
	//! - ���̒l�́A�]����C���[�W�͈̔� ( width height ) �𒴂��Ďw�肵�Ă͂����܂���B
	//! @param[in] pSrcImageView �]�����̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �]�����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �A::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImageView �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] srcRect �]�����̗̈�ł��B
	//! - ���̒l�́A�]�����C���[�W�͈̔� ( width height ) �𒴂��Ďw�肵�Ă͂����܂���B
	//! @param[in] filter �t�B���^�[�ł��B
	//! @remarks
	//! �]�����������C���[�W�r���[�̓^�C�v�A�t�H�[�}�b�g�A�[���A���x�����A���C���[���A�T���v��������v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BlitImage
	virtual void BlitImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DRectangle2D& dstRect,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DRectangle2D& srcRect,
		V3D_FILTER filter) = 0;

	//! @brief �C���[�W�r���[��]�����܂��B
	//! @param[in] pDstImageView �]����̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �]����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImageView �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImageView �]�����̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���̎g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �]�����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImageView �͓]�����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���K�v������܂��B
	//! @param[in] filter �t�B���^�[�ł��B
	//! @remarks
	//! �]�����������C���[�W�r���[�̓^�C�v�A�t�H�[�}�b�g�A�[���A���x�����A���C���[���A�T���v��������v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BlitImage
	virtual void BlitImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout,
		V3D_FILTER filter) = 0;

	//! @brief �}���`�T���v���C���[�W���}���`�T���v���C���[�W�ɕϊ����܂��B
	//! @param[in] pDstImage �ϊ���̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ�ł���K�v������܂��B<br>
	//! �܂� pDstImage �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImage �ϊ����̃C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B
	//! - �C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImage �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] rangeCount �ϊ�����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges rangeCount �ϊ�����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @remarks
	//! �ϊ����������C���[�W�r���[�̓^�C�v�A�t�H�[�}�b�g����v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DResolveImageRange* pRanges) = 0;

	//! @brief �}���`�T���v���C���[�W�r���[���}���`�T���v���C���[�W�r���[�ɕϊ����܂��B
	//! @param[in] pDstImageView �ϊ���̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B<br>
	//! �܂��T���v�����ł��� V3DImageDesc::samples �ɂ� ::V3D_SAMPLE_COUNT_1 ���w�肳��Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImageView �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] dstOffset �ϊ���C���[�W�̃I�t�Z�b�g�ł��B<br>
	//! - ���̒l�́A�ϊ���C���[�W�͈̔� ( width height ) �𒴂��Ďw�肵�Ă͂����܂���B
	//! @param[in] pSrcImageView �ϊ����̃C���[�W�r���[�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B<br>
	//! �܂��T���v�����ł��� V3DImageDesc::samples �� ::V3D_SAMPLE_COUNT_1 �ł͂����Ă͂����܂���B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImageView �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] srcOffset �ϊ����C���[�W�̃I�t�Z�b�g�ł��B
	//! - ���̒l�́A�ϊ����C���[�W�͈̔� ( width height ) �𒴂��Ďw�肵�Ă͂����܂���B
	//! @param[in] size �ϊ�����C���[�W�̃T�C�Y�ł��B
	//! - ���̒l�͕ϊ����������C���[�W�͈̔� ( imageSize - offset ) �𒴂��Ďw�肵�Ă͂����܂���B
	//! @remarks
	//! �ϊ����������C���[�W�r���[�̓^�C�v�A�t�H�[�}�b�g�A�[���A���x�����A���C���[������v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(
		IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, const V3DPoint2D& dstOffset,
		IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout, const V3DPoint2D& srcOffset,
		const V3DSize2D& size ) = 0;

	//! @brief �}���`�T���v���C���[�W�r���[���}���`�T���v���C���[�W�r���[�ɕϊ����܂��B
	//! @param[in] pDstImageView �ϊ���̃C���[�W�r���[�ł��B
	//! - �A�N�Z�X����C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B<br>
	//! �܂��T���v�����ł��� V3DImageDesc::samples �ɂ� ::V3D_SAMPLE_COUNT_1 ���w�肳��Ă���K�v������܂��B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pDstImageView �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] pSrcImageView �ϊ����̃C���[�W�r���[�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_SRC ���܂܂�Ă���K�v������܂��B<br>
	//! �܂��T���v�����ł��� V3DImageDesc::samples �� ::V3D_SAMPLE_COUNT_1 �ł͂����Ă͂����܂���B
	//! - �A�N�Z�X����C���[�W�̃p�C�v���C���X�e�[�W�ɂ� ::V3D_PIPELINE_STAGE_TRANSFER ���܂܂�Ă���K�v������܂��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_SRC �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pSrcImageView �͕ϊ����J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @remarks
	//! �ϊ����������C���[�W�r���[�̓^�C�v�A�t�H�[�}�b�g�A�[���A���x�����A���C���[������v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief �����_�[�p�X���J�n���܂��B
	//! @param[in] pRenderPass �����_�[�p�X�ł��B
	//! @param[in] pFrameBuffer �w�肵�������_�[�p�X�ɑΉ�����t���[���o�b�t�@�[�ł��B
	//! @param[in] subpassContentInline �T�u�p�X�̃R�}���h�̋L�^���@�ł��B
	//! - �T�u�p�X�̃R�}���h�����̃R�}���h�o�b�t�@�[�ɋL�^����ꍇ�� true ���w�肵�܂��B<br>
	//! �܂��T�u�p�X�̃R�}���h���Z�J���_���R�}���h�o�b�t�@�[�ŋL�^���āA���̃R�}���h�o�b�t�@�[�Ŏ��s����ꍇ�� false ���w�肵�Ă��������B
	//! @param[in] pRenderArea �����_�����O�̈�ł��B
	//! - �����_�����O�̈�́A�A�^�b�`�����g�͈̔� ( width height ) �𒴂��Ďw�肵�Ă͂����܂���B<br>
	//! �܂� nullptr ���w�肵���ꍇ�́A�A�^�b�`�����g�S�̂������_�����O�̈�Ƃ��܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> ���� </td>
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
	//! @brief �����_�[�p�X���I�����܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> �����_�[�p�X���ł̂ݗL�� </td>
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

	//! @brief ���̃T�u�p�X�ֈڍs���܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> �����_�[�p�X���ł̂ݗL�� </td>
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

	//! @brief �C���[�W���N���A���܂��B
	//! @param[in] pImage �N���A����C���[�W�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] imageLayout �N���A����C���[�W�̃��C�A�E�g�ł��B<br>
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �A ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pImage �̓N���A���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] clearValue �N���A����l�ł��B
	//! - �N���A����C���[�W���J���[�̏ꍇ�� V3DClearValue::color �A�f�v�X�X�e���V���̏ꍇ�� V3DClearValue::depthStencil �ɔC�ӂ̒l���w�肵�܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
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
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER <b>��</b> <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!       <br>
	//!       <b>��</b> �N���A����C���[�W���f�v�X�X�e���V���̏ꍇ�͖����ł�
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief �C���[�W�r���[���N���A���܂��B
	//! @param[in] pImageView �N���A����C���[�W�̃r���[�ł��B
	//! - �C���[�W�쐬���Ɏw�肵���g�p�@�ł��� V3DImageDesc::usageFlags �ɂ� ::V3D_IMAGE_USAGE_TRANSFER_DST ���܂܂�Ă���K�v������܂��B
	//! @param[in] imageLayout �N���A����C���[�W���C�A�E�g�ł��B
	//! - �C���[�W���C�A�E�g�ɂ� ::V3D_IMAGE_LAYOUT_GENERAL �܂��� ::V3D_IMAGE_LAYOUT_TRANSFER_DST �̂����ꂩ���w�肵�܂��B<br>
	//! �܂� pImageView �̓N���A���J�n����O�ɂ��̃C���[�W���C�A�E�g�Ɉڍs���Ă����K�v������܂��B
	//! @param[in] clearValue �N���A����l�ł��B
	//! - �N���A����C���[�W���J���[�̏ꍇ�� V3DClearValue::color �A�f�v�X�X�e���V���̏ꍇ�� V3DClearValue::depthStencil �ɔC�ӂ̒l���w�肵�܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
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
	//!       ::V3D_PIPELINE_STAGE_COMPUTE_SHADER <b>��</b> <br>
	//!       ::V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE <br>
	//!       <br>
	//!       <b>��</b> �N���A����C���[�W���f�v�X�X�e���V���̏ꍇ�͖����ł�
	//!     </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief �T�u�p�X�̃A�^�b�`�����g���N���A���܂��B
	//! @param[in] colorAttachmentCount �N���A����A�^�b�`�����g�̐��ł��B
	//! - �N���A����J���[�A�^�b�`�����g�����݂��Ȃ��ꍇ�� 0 ���w�肷�邱�Ƃ��ł��܂��B
	//! @param[in] pColorAttachments �N���A����J���[�A�^�b�`�����g�̋L�q�̔z��ł��B
	//! - colorAttachmentCount �� 1�ȏ�̏ꍇ�́AcolorAttachmentCount �̒l�̗v�f�����z����w�肷��K�v������܂��B<br>
	//! �܂� colorAttachmentCount �̒l�� 0 �̏ꍇ�� nullptr ���w�肵�Ă��������B
	//! @param[in] pDepthStencilAttachment �N���A����f�v�X�X�e���V���A�^�b�`�����g�̋L�q�ł��B
	//! - �f�v�X�X�e���V���A�^�b�`�����g���N���A���Ȃ��A�܂��͑��݂��Ȃ��ꍇ�� nullptr ���w�肷�邱�Ƃ��ł��܂��B
	//! @param[in] rangeCount �N���A����͈͂̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] pRanges �N���A����͈͂̔z��ł��B
	//! - �z��̗v�f���� rangeCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �����_�[�p�X���ł̂ݗL�� </td>
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

	//! @brief �p�C�v���C�����o�C���h���܂��B
	//! @param[in] pPipeline �o�C���h����p�C�v���C���ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BindPipeline(IV3DPipeline* pPipeline) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���o�C���h���܂��B
	//! @param[in] pipelineType �p�C�v���C���̃^�C�v�ł��B
	//! @param[in] pPipelineLayout �p�C�v���C���̃��C�A�E�g�ł��B
	//! @param[in] firstSet �f�X�N���v�^�Z�b�g��ݒ肷��ŏ��̃Z�b�g�ԍ��ł��B
	//! - ���̒l�� { 0 <= firstSet < V3DDeviceCaps::maxBoundDescriptorSets } �͈̔͂ɐ�������܂��B
	//! @param[in] descriptorSetCount �o�C���h����f�X�N���v�^�Z�b�g�̐��ł��B
	//! - ���̒l�� { 1 <= descriptorSetCount <= (V3DDeviceCaps::maxBoundDescriptorSets - firstSet) } �͈̔͂ɐ�������܂��B
	//! @param[in] ppDescriptorSets �o�C���h����f�X�N���v�^�Z�b�g�̔z��ł��B
	//! - �z��̗v�f���� descriptorSetCount ����K�v������܂��B
	//! @param[in] dynamicOffsetCount �_�C�i�~�b�N�I�t�Z�b�g�̐��ł��B
	//! - ���̒l�̓f�X�N���v�^�Z�b�g�Ɋi�[����Ă���_�C�i�~�b�N�o�b�t�@�[�̐��ƈ�v���Ă���K�v������܂��B
	//! @param[in] pDynamicOffsets �_�C�i�~�b�N�I�t�Z�b�g�̔z��ł��B
	//! - dynamicOffsetCount �� 1�ȏ�̏ꍇ�́AdynamicOffsetCount �l�̗v�f�����z����w�肵�Ă��������B<br>
	//! �܂� dynamicOffsetCount �� 0 �̏ꍇ�� nullptr ���w�肵�Ă��������B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BindDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType,	IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets,
		uint32_t dynamicOffsetCount = 0, const uint32_t* pDynamicOffsets = nullptr) = 0;

	//! @brief �o�[�e�b�N�X�o�b�t�@�[���o�C���h���܂��B
	//! @param[in] firstBinding �ŏ��̃o�C���f�B���O�̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= firstBinding < V3DDeviceCaps::maxVertexInputBindings } �͈̔͂ɐ�������܂��B
	//! @param[in] bindingCount �o�C���f�B���O�̐��ł��B
	//! - ���̒l�� { 1 <= bindingCount < (V3DDeviceCaps::maxVertexInputBindings - firstBinding) } �͈̔͂ɐ�������܂��B
	//! @param[in] ppBuffers �o�[�e�b�N�X�o�b�t�@�[�̔z��ł��B
	//! - �z��̗v�f���� bindingCount �ł���K�v������܂��B<br>
	//! �܂��o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_VERTEX ���܂܂�Ă���K�v������܂��B
	//! @param[in] pOffsets �o�[�e�b�N�X�o�b�t�@�[�̃������I�t�Z�b�g�̔z��ł��B
	//! - �z��̗v�f���� bindingCount ����K�v������܂��B<br>
	//! �܂��z��̗v�f�ł��郁�����I�t�Z�b�g�́A�o�C�g�P�ʂŎw�肵�܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, IV3DBuffer** ppBuffers, const uint64_t* pOffsets) = 0;

	//! @brief �C���f�b�N�X�o�b�t�@�[���o�C���h���܂��B
	//! @param[in] pBuffer �o�C���h����C���f�b�N�X�o�b�t�@�[�ł��B
	//! - �o�b�t�@�[�쐬���Ɏw�肵���g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_INDEX ���܂܂�Ă���K�v������܂��B<br>
	//! �܂��o�b�t�@�[�ɏ������܂�Ă���C���f�b�N�X�̒l�� V3DDeviceCaps::maxDrawIndexedIndexValue �ȉ��ɐ�������܂��B
	//! @param[in] offset �o�C���h����C���f�b�N�X�o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= offset < V3DBufferDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] indexType �C���f�b�N�X�^�C�v��\�� ::V3D_INDEX_TYPE �񋓒萔�̂����ꂩ���w�肵�܂��B
	//! - ::V3D_INDEX_TYPE_UINT16 �͎����Ɉˑ����邱�ƂȂ��A�ǂ̊��ł��w�肷�邱�Ƃ��ł��܂��B<br>
	//! - ::V3D_INDEX_TYPE_UINT32 �� V3DDeviceCaps::flags �� ::V3D_CAP_FULL_DRAW_INDEX_UINT32 ���܂܂�Ă���ꍇ�ɂ̂ݎw�肷�邱�Ƃ��ł��܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void BindIndexBuffer(IV3DBuffer* pBuffer, uint64_t offset, V3D_INDEX_TYPE indexType) = 0;

	//! @brief �v�b�V���萔���X�V���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] slot �v�b�V���萔�̃X���b�g�ł��B<br>
	//! - pPipelineLayout �̍쐬���Ɏw�肵�� V3DConstantDesc �\���̂̔z��̃C���f�b�N�X���w�肵�܂��B
	//! @param[in] pData �v�b�V���萔���X�V����f�[�^�ł��B
	//! - �X�V����f�[�^�̃o�C�g�T�C�Y�� V3DConstantDesc::size �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData) = 0;

	//! @brief �v�b�V���萔���X�V���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] slot �v�b�V���萔�̃X���b�g�ł��B
	//! - pPipelineLayout �쐬���Ɏw�肵�� V3DConstantDesc �\���̂̔z��̃C���f�b�N�X���w�肵�܂��B
	//! @param[in] offset �v�b�V���萔�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 0 <= offset < V3DConstantDesc::size } �͈̔͂ɐ�������܂��B
	//! @param[in] size �v�b�V���萔���X�V����f�[�^�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� { 1 <= size <= (V3DConstantDesc::size - offset) } �͈̔͂ɐ�������܂��B
	//! @param[in] pData �v�b�V���萔���X�V����f�[�^�ł��B
	//! - �X�V����f�[�^�̃o�C�g�T�C�Y�� V3DConstantDesc::size �̒l�ł���K�v������K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData) = 0;

	//! @brief �v�b�V���f�X�N���v�^�Z�b�g���X�V���܂��B
	//! @param[in] pipelineType �p�C�v���C���̃^�C�v�ł��B
	//! @param[in] pPipelineLayout �p�C�v���C���̃��C�A�E�g�ł��B
	//! @param[in] firstSet �f�X�N���v�^�Z�b�g��ݒ肷��ŏ��̃Z�b�g�ԍ��ł��B
	//! - ���̒l�� { 0 <= firstSet < V3DDeviceCaps::maxBoundDescriptorSets } �͈̔͂ɐ�������܂��B
	//! @param[in] descriptorSetCount �o�C���h����f�X�N���v�^�Z�b�g�̐��ł��B
	//! - ���̒l�� { 1 <= descriptorSetCount <= (V3DDeviceCaps::maxBoundDescriptorSets - firstSet) } �͈̔͂ɐ�������܂��B
	//! @param[in] ppDescriptorSets �o�C���h����f�X�N���v�^�Z�b�g�̔z��ł��B
	//! - �z��̗v�f���� descriptorSetCount ����K�v������܂��B
	//! @param[in] dynamicOffsetCount �_�C�i�~�b�N�I�t�Z�b�g�̐��ł��B
	//! - ���̒l�̓f�X�N���v�^�Z�b�g�Ɋi�[����Ă���_�C�i�~�b�N�o�b�t�@�[�̐��ƈ�v���Ă���K�v������܂��B
	//! @param[in] pDynamicOffsets �_�C�i�~�b�N�I�t�Z�b�g�̔z��ł��B
	//! - dynamicOffsetCount �� 1�ȏ�̏ꍇ�́AdynamicOffsetCount �l�̗v�f�����z����w�肵�Ă��������B<br>
	//! �܂� dynamicOffsetCount �� 0 �̏ꍇ�� nullptr ���w�肵�Ă��������B
	//! @remarks
	//! - ���̃R�}���h�� V3DDeviceCaps::extensionFlags �� ::V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS ���܂܂�Ă���ꍇ�Ɏg�p���邱�Ƃ��ł��܂��B
	//! .
	//! <br>
	//! <table class="basic">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void PushDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType, IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets,
		uint32_t dynamicOffsetCount = 0, const uint32_t* pDynamicOffsets = nullptr) = 0;

	//! @brief �r���[�|�[�g��ݒ肵�܂��B
	//! @param[in] firstViewport �ŏ��̃r���[�|�[�g�̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= firstViewport < V3DDeviceCaps::maxViewports } �͈̔͂ɐ�������܂��B
	//! @param[in] viewportCount �r���[�|�[�g�̐��ł��B
	//! - ���̒l�� { 1 <= viewportCount < (V3DDeviceCaps::maxViewports - firstViewport) } �͈̔͂ɐ�������܂��B
	//! @param[in] pViewports �r���[�|�[�g�̔z��ł��B
	//! - �z��̗v�f���� viewportCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports) = 0;

	//! @brief �V�U�����O��ݒ肵�܂��B
	//! @param[in] firstScissor �ŏ��̃V�U�����O�̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= firstScissor < V3DDeviceCaps::maxViewports } �͈̔͂ɐ�������܂��B
	//! @param[in] scissorCount �U�����O�̈�̐��ł��B
	//! - ���̒l�� { 1 <= scissorCount <= (V3DDeviceCaps::maxViewports - firstScissor) } �͈̔͂ɐ�������܂��B
	//! @param[in] pScissors �V�U�����O�̈�̔z��ł��B
	//! - �z��̗v�f���� scissorCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	virtual void SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors) = 0;

	//! @brief �X�e���V���̔�r���Z�q�Ŏg�p�����l��ݒ肵�܂��B
	//! @param[in] faceMask �ݒ肷��t�F�C�X��\�� ::V3D_STENCIL_FACE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] reference ��r���Z�q�Ŏg�p�����l�ł��B
	//! - �X�e���V���̃r�b�g���𒴂���l���w�肷�邱�Ƃ͂ł��܂���B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_COMPARE_OP
	virtual void SetStencilReference(V3DFlags faceMask, uint32_t reference) = 0;

	//! @brief �u�����h�萔��ݒ肵�܂��B
	//! @param[in] blendConstants �u�����h�萔�ł��B
	//! - �z��̗v�f [0, 3] �� RGBA ��\���Ă��܂��B<br>
	//!   �܂��A�v�f�̒l�� { 0.0 <= blendConstants <= 1.0 } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br>::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_BLEND_FACTOR
	virtual void SetBlendConstants(const float blendConstants[4]) = 0;

	//! @brief �N�G���v�[�������Z�b�g���܂��B
	//! @param[in] pQueryPool ���Z�b�g����N�G���v�[���ł��B
	//! @param[in] firstQuery ���Z�b�g����ŏ��̃N�G���̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= firstQuery < V3DQueryPoolDesc::queryCount } �͈̔͂ɐ�������܂��B
	//! @param[in] queryCount ���Z�b�g����N�G���̐��ł��B
	//! - ���̒l�� { 1 <= queryCount <= (V3DQueryPoolDesc::queryCount - firstQuery) } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount) = 0;

	//! @brief �N�G�����J�n���܂��B
	//! @param[in] pQueryPool �N�G�����J�n����N�G���v�[���ł��B
	//! @param[in] query �J�n����N�G���̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= query < V3DQueryPoolDesc::queryCount } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;

	//! @brief �N�G�����I�����܂��B
	//! @param[in] pQueryPool �N�G�����I������N�G���v�[���ł��B
	//! @param[in] query �I������N�G���̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= query < V3DQueryPoolDesc::queryCount } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void EndQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;

	//! @brief �^�C�v�X�^���v���������݂܂��B
	//! @param[in] pQueryPool �^�C���X�^���v���Ǘ�����N�G���v�[���ł��B
	//! @param[in] pipelineStage �p�C�v���C���X�e�[�W�ł��B
	//! @param[in] query �^�C���X�^���v�̃N�G���̃C���f�b�N�X�ł��B
	//! - ���̒l�� { 0 <= query < V3DQueryPoolDesc::queryCount } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! - ���̃R�}���h�� V3DDeviceCaps::queryFlags �� ::V3D_QUERY_CAP_TIMESTAMP_QUERY_GRAPHICS_AND_COMPUTE ���܂܂�Ă���ꍇ�ɂ̂ݎg�p���邱�Ƃ��ł��܂��B
	//! .
	//! - �^�C���X�^���v�̒l�́A�w�肵���p�C�v���C���X�e�[�W�̎��s������������ɏ������܂�܂��B<br>
	//! �܂��������܂��l�́A�i�m�b�P�ʂł��B
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_TRANSFER </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query) = 0;

	//! @brief �`������܂��B
	//! @param[in] vertexCount ���_�̐��ł��B
	//! @param[in] instanceCount �C���X�^���X�̐��ł��B
	//! @param[in] firstVertex �ŏ��̒��_�̃C���f�b�N�X�ł��B
	//! @param[in] firstInstance �ŏ��̃C���X�^���X�̃C���f�b�N�X�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �����_�[�p�X���ł̂ݗL�� </td>
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

	//! @brief �C���f�b�N�X�o�b�t�@���g�p�����`������܂��B
	//! @param[in] indexCount �C���f�b�N�X�̐��ł��B
	//! @param[in] instanceCount �C���X�^���X�̐��ł��B
	//! @param[in] firstIndex �ŏ��̃C���f�b�N�X�ł��B
	//! @param[in] firstInstance �ŏ��̃C���X�^���X�̃C���f�b�N�X�ł��B
	//! @param[in] vertexOffset ���_�̃o�C�g�I�t�Z�b�g�ł��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �����_�[�p�X���ł̂ݗL�� </td>
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

	//! @brief �R���s���[�g�V�F�[�_�[�̍�Ƃ��f�B�X�p�b�`���܂��B
	//! @param[in] groupCountX X �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! - ���̒l�� { 1 <= groupCountX < V3DDeviceCaps::maxComputeDispatchGroupCount.x } �͈̔͂ɐ�������܂��B
	//! @param[in] groupCountY Y �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! - ���̒l�� { 1 <= groupCountY < V3DDeviceCaps::maxComputeDispatchGroupCount.y } �͈̔͂ɐ�������܂��B
	//! @param[in] groupCountZ Z �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! - ���̒l�� { 1 <= groupCountZ < V3DDeviceCaps::maxComputeDispatchGroupCount.z } �͈̔͂ɐ�������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> ���� </td>
	//!     <td> ::V3D_PIPELINE_STAGE_COMPUTE_SHADER </td>
	//!     <td> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

	//! @brief �Z�J���_���R�}���h�o�b�t�@�[�����s���܂��B
	//! @param[in] commandBufferCount ���s����Z�J���_���R�}���h�o�b�t�@�[�̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] ppCommandBuffers �Z�J���_���R�}���h�o�b�t�@�[�̔z��ł��B
	//! - �z��̗v�f���́AcommandBufferCount �ł���K�v������܂��B
	//! @remarks
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE <br> ::V3D_QUEUE_TRANSFER </td>
	//!   </tr>
	//! </table>
	virtual void ExecuteCommandBuffers(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers) = 0;

	//! @brief �f�o�b�O�}�[�J�[���J�n���܂��B
	//! @param[in] pName �f�o�b�O�}�[�J�[�̖��O�ł��B
	//! @param[in] color �f�o�b�O�}�[�J�[�̐F�ł��B
	//! - �z��̗v�f [0, 3] �� RGBA ��\���Ă��܂��B
	//! @remarks
	//! - ���̃R�}���h�� V3DDeviceCaps::extensionFlags �� ::V3D_DEVICE_EXTENSION_DEBUG_MARKER ���܂܂�Ă���ꍇ�Ɏg�p���邱�Ƃ��ł��܂��B
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void BeginDebugMarker(const char* pName, const float color[4]) = 0;
	//! @brief �f�o�b�O�}�[�J�[���I�����܂��B
	//! @remarks
	//! - ���̃R�}���h�� V3DDeviceCaps::extensionFlags �� ::V3D_DEVICE_EXTENSION_DEBUG_MARKER ���܂܂�Ă���ꍇ�Ɏg�p���邱�Ƃ��ł��܂��B
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
	//!     <td> </td>
	//!     <td> ::V3D_QUEUE_GRAPHICS <br> ::V3D_QUEUE_COMPUTE </td>
	//!   </tr>
	//! </table>
	virtual void EndDebugMarker() = 0;

	//! @brief �f�o�b�O�}�[�J�[��}�����܂��B
	//! @param[in] pName �f�o�b�O�}�[�J�[�̖��O�ł��B
	//! @param[in] color �f�o�b�O�}�[�J�[�̐F�ł��B
	//! - �z��̗v�f [0, 3] �� RGBA ��\���Ă��܂��B
	//! @remarks
	//! - ���̃R�}���h�� V3DDeviceCaps::extensionFlags �� ::V3D_DEVICE_EXTENSION_DEBUG_MARKER ���܂܂�Ă���ꍇ�Ɏg�p���邱�Ƃ��ł��܂��B
	//! .
	//! <br>
	//! <table class="cmdbuff">
	//!   <tr><th>�T�|�[�g����R�}���h�o�b�t�@�[</th><th>�����_�[�p�X���ł̎g�p</th><th>�T�|�[�g����p�C�v���C���X�e�[�W</th><th>�T�|�[�g����L���[</th></tr>
	//!   <tr>
	//!     <td> ::V3D_COMMAND_BUFFER_TYPE_PRIMARY <br> ::V3D_COMMAND_BUFFER_TYPE_SECONDARY </td>
	//!     <td> �L�� </td>
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
// �L���[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUEUE_FLAG
//! @brief �L���[�̋@�\�t���O
enum V3D_QUEUE_FLAG : V3DFlags
{
	V3D_QUEUE_GRAPHICS = 0x00000001, //!< �O���t�B�b�N�X������T�|�[�g���Ă��܂��B
	V3D_QUEUE_COMPUTE = 0x00000002, //!< �R���s���[�g������T�|�[�g���Ă��܂��B
	V3D_QUEUE_TRANSFER = 0x00000004, //!< �R�s�[������T�|�[�g���Ă��܂��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DQueue
//! @brief �L���[�C���^�[�t�F�[�X
// !@sa IV3DDevice::GetQueue
class V3D_DLL_API IV3DQueue : public IV3DDeviceChild
{
public:
	//! @brief �L���[�t�@�~���[�̃C���f�b�N�X���擾���܂��B
	//! @return �L���[�t�@�~���[�̃C���f�b�N�X��Ԃ��܂��B
	virtual uint32_t GetFamily() const = 0;

	//! @brief �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M���܂��B
	//! @param[in] commandBufferCount ���M����R�v���C�}���}���h�o�b�t�@�[�̐��ł��B
	//! @param[in] ppCommandBuffers ���M����v���C�}���R�}���h�o�b�t�@�[��\�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @param[in] pFence �R�}���h�o�b�t�@�[�̑��M�̊����̑ҋ@�Ɏg�p����t�F���X�ł��B<br>
	//! nullptr ���w�肵���ꍇ�́A���M�̊�����ҋ@���܂���B
	//! @retval V3D_OK �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M���܂����B
	//! @retval V3D_ERROR_FAIL �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M�ł��܂���ł����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;
	//! @brief �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M���܂��B
	//! @param[in] pSwapChain �������Ƃ�X���b�v�`�F�C���ł��B
	//! @param[in] commandBufferCount ���M����R�v���C�}���}���h�o�b�t�@�[�̐��ł��B
	//! @param[in] ppCommandBuffers ���M����v���C�}���R�}���h�o�b�t�@�[��\�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @param[in] pFence �R�}���h�o�b�t�@�[�̑��M�̊����̑ҋ@�Ɏg�p����t�F���X�ł��B<br>
	//! @retval V3D_OK �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M���܂����B
	//! @retval V3D_ERROR_FAIL �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M�ł��܂���ł����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! ���̃��\�b�h�̓X���b�v�`�F�C����p�ɂȂ�܂��B
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;

	//! @brief �����_�����O�C���[�W��\�����܂��B
	//! @param[in] pSwapChain �������Ƃ�X���b�v�`�F�C���ł��B
	//! @retval V3D_OK �����_�����O�C���[�W�̕\���ɐ������܂����B
	//! @retval V3D_ERROR_FAIL �����_�����O�C���[�W�̕\���Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) = 0;

	//! @brief �L���[���A�C�h����ԂɂȂ�܂őҋ@���܂��B
	//! @retval V3D_OK �L���[���A�C�h����ԂɂȂ�܂����B
	//! @retval V3D_ERROR_FAIL �L���[���A�C�h����ԂɂȂ�܂���ł����B
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
// �A�_�v�^
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_MEMORY_HEAP_TYPE
//! @brief �������q�[�v�̃^�C�v
enum V3D_MEMORY_HEAP_TYPE : uint8_t
{
	V3D_MEMORY_HEAP_TYPE_HOST = 0, //!< �z�X�g�̃������q�[�v�ł��B
	V3D_MEMORY_HEAP_TYPE_DEVICE = 1, //!< �f�o�C�X�̃������q�[�v�ł��B
};

//! @enum V3D_BUFFER_FORMAT_FEATURE_FLAG
//! @brief �o�b�t�@�[�t�H�[�}�b�g�̃T�|�[�g�t���O
enum V3D_BUFFER_FORMAT_FEATURE_FLAG : V3DFlags
{
	//! @brief ���j�t�H�[���e�N�Z���o�b�t�@�[�Ŏg�p���邱�Ƃ��ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_UNIFORM_TEXEL = 0x00000008,
	//! @brief �X�g���[�W�e�N�Z���o�b�t�@�[�Ŏg�p���邱�Ƃ��ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL = 0x00000010,
	//! @brief �A�g�~�b�N������T�|�[�g����X�g���[�W�e�N�Z���o�b�t�@�[�Ŏg�p���邱�Ƃ��ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL_ATOMIC = 0x00000020,
	//! @brief �o�[�e�b�N�X�o�b�t�@�[�Ŏg�p���邱�Ƃ��ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_VERTEX = 0x00000040,
};

//! @enum V3D_IMAGE_FORMAT_FEATURE_FLAG
//! @brief �C���[�W�t�H�[�}�b�g�̃T�|�[�g�t���O
enum V3D_IMAGE_FORMAT_FEATURE_FLAG : V3DFlags
{
	//! @brief �T���v�����O�ł���C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED = 0x00000001,
	//! @brief �X�g���[�W�C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_STORAGE = 0x00000002,
	//! @brief �A�g�~�b�N������T�|�[�g����X�g���[�W�C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_STORAGE_ATOMIC = 0x00000004,
	//! @brief �J���[�A�^�b�`�����g�y�уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT = 0x00000080,
	//! @brief �u�����f�B���O���T�|�[�g����J���[�A�^�b�`�����g�y�уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND = 0x00000100,
	//! @brief �f�v�X�X�e���V���A�^�b�`�����g�y�уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT = 0x00000200,
	//! @brief �]����̃C���[�W�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC = 0x00000400,
	//! @brief �]�����̃C���[�W�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::BlitImage
	//! @sa IV3DCommandBuffer::BlitImageView
	V3D_IMAGE_FORMAT_FEATURE_BLIT_DST = 0x00000800,
	//! @brief ���`�t�B���^�[���g�p���邱�Ƃ��ł��܂��B
	//! - ::V3D_IMAGE_FORMAT_FEATURE_SAMPLED �ƈꏏ�Ɏw�肳��Ă����ꍇ�́A�T���v���[�̊g��t�B���^ ( V3DSamplerDesc::magFilter ) �A�k���t�B���^ ( V3DSamplerDesc::minFilter ) �� ::V3D_FILTER_LINEAR ���w�肷�邱�Ƃ��ł��܂��B<br>
	//! �܂��A�T���v���[�̃~�b�v�}�b�v���[�h ( V3DSamplerDesc::mipmapMode ) �ɂ� ::V3D_MIPMAP_MODE_LINEAR ���w�肷�邱�Ƃ��ł��܂��B
	//! .
	//! - ::V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC �ƈꏏ�Ɏw�肳��Ă����ꍇ�́A IV3DCommandBuffer::BlitImage ����� IV3DCommandBuffer::BlitImageView �̈����ł��� filter �� ::V3D_FILTER_LINEAR ���w�肷�邱�Ƃ��ł��܂��B
	//! <br>
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED_FILTER_LINEAR = 0x00001000,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAdapterDesc
//! @brief �A�_�v�^�̋L�q
struct V3DAdapterDesc
{
	uint32_t apiVersion; //!< API �o�[�W�����ł��B
	uint32_t driverVersion; //!< �h���C�o�o�[�W�����ł��B
	uint32_t vendorID; //!< �x���_�[ ID �ł��B
	uint32_t deviceID; //!< �f�o�C�X ID �ł��B
	char deviceName[256]; //!< �h���C�o�[���ł��B
};

//! @struct V3DMemoryHeapDesc
//! @brief �������q�[�v�̋L�q
struct V3DMemoryHeapDesc
{
	V3D_MEMORY_HEAP_TYPE type; //!< �������q�[�v�̃^�C�v�ł��B
	uint64_t size; //!< �������q�[�v�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
};

//! @struct V3DMemoryTypeDesc
//! @brief �������^�C�v�̋L�q
struct V3DMemoryTypeDesc
{
	V3DFlags propertyFlags; //!< ������������\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	uint32_t heap; //!< �������q�[�v�̃C���f�b�N�X�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DAdapter
//! @brief �A�_�v�^�C���^�[�t�F�[�X
// !@sa IV3DInstance::GetAdapter
class V3D_DLL_API IV3DAdapter : public IV3DObject
{
public:
	//! @brief �A�_�v�^�̋L�q���擾���܂��B
	//! @return �A�_�v�^�̋L�q��Ԃ��܂��B
	virtual const V3DAdapterDesc& GetDesc() const = 0;

	//! @brief �������q�[�v�̐����擾���܂��B
	//! @return �������q�[�v�̐���Ԃ��܂��B
	virtual uint32_t GetMemoryHeapCount() const = 0;
	//! @brief �������q�[�v�̋L�q���擾���܂��B
	//! @param[in] memoryHeap �������q�[�v�̃C���f�b�N�X�ł��B
	//! @return �������q�[�v�̋L�q��Ԃ��܂��B
	virtual const V3DMemoryHeapDesc& GetMemoryHeapDesc(uint32_t memoryHeap) const = 0;

	//! @brief �������^�C�v�̐����擾���܂��B
	//! @return �������^�C�v�̐���Ԃ��܂�
	virtual uint32_t GetMemoryTypeCount() const = 0;
	//! @brief �������^�C�v�̋L�q���擾���܂��B
	//! @param[in] memoryType �������^�C�v�̃C���f�b�N�X�ł��B
	//! @return �������^�C�v�̋L�q��Ԃ��܂��B
	virtual const V3DMemoryTypeDesc& GetMemoryTypeDesc(uint32_t memoryType) const = 0;

protected:
	//! @cond MISC
	virtual ~IV3DAdapter() {}
	//! @endcond
};

//! @}

// ----------------------------------------------------------------------------------------------------
// �f�o�C�X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_CAP_FLAG
//! @brief �\�̓t���O
enum V3D_CAP_FLAG : V3DFlags
{
	//! @brief �C���f�b�N�X�o�b�t�@�[���o�C���h���� IV3DCommandBuffer::BindIndexBuffer �̑�R�����ł��� indexType �� ::V3D_INDEX_TYPE_UINT32 ���w�肷�邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::BindIndexBuffer
	V3D_CAP_FULL_DRAW_INDEX_UINT32 = 0x00000001,
	//! @brief �ٕ����t�B���^�����O���g�p�ł��邱�Ƃ�\���܂��B
	//! @sa V3DSamplerDesc::anisotropyEnable
	//! @sa V3DSamplerDesc::maxAnisotropy
	V3D_CAP_SAMPLER_ANISOTROPY = 0x00000002,
};

//! @enum V3D_QUERY_CAP_FLAG
//! @brief �N�G���̔\�̓t���O
enum V3D_QUERY_CAP_FLAG : V3DFlags
{
	//! @brief �I�N���[�W�����N�G�� ::V3D_QUERY_TYPE_OCCLUSION �Ō����Ȍ��ʂ��擾���邱�Ƃ��ł��܂��B<br>
	//! ���̃t���O���w�肳��Ă��Ȃ������ꍇ�A�I�N���[�W�����N�G���̌��ʂ̓s�N�Z�����������܂�Ȃ������ꍇ�� 0 �s�N�Z�����������܂ꂽ�ꍇ�� 1 �̂悤�ɕԂ����Ƃ�����܂��B
	V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE = 0x00000001,
	//! @brief �p�C�v���C���̓��v�N�G���ł��� ::V3D_QUERY_TYPE_PIPELINE_STATISTICS ���T�|�[�g���܂��B
	V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY = 0x00000002,
	//! @brief ���ׂẴO���t�B�b�N�X�L���[�ƃR���s���[�g�L���[�̃^�C���X�^���v���T�|�[�g���܂��B
	V3D_QUERY_CAP_TIMESTAMP_QUERY_GRAPHICS_AND_COMPUTE = 0x00000004,
	//! @brief �v���C�}���R�}���h�o�b�t�@�[�ŃI�N���[�W�����N�G�����A�N�e�B�u�ɂȂ��Ă���� ( IV3DCommandBuffer::BeginQuery �` IV3DCommandBuffer::EndQuery ) �Ŏ��s�����
	//! �Z�J���_���R�}���h�o�b�t�@�[ ( IV3DCommandBuffer::ExecuteCommandBuffers ) ���N�G���������p���A���̌��ʂ��擾���邱�Ƃ��ł��܂��B
	V3D_QUERY_CAP_INHERITED_QUERIES = 0x00000008,
};

//! @enum V3D_IMAGE_CAP_FLAG
//! @brief �C���[�W�̔\�̓t���O
enum V3D_IMAGE_CAP_FLAG : V3DFlags
{
	//! @brief �C���[�W�r���[�̃^�C�v�ł��� V3DImageViewDesc::type �� ::V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY ���w�肷�邱�Ƃ��ł��܂��B
	V3D_IMAGE_CAP_CUBE_ARRAY = 0x00000001,
	//! @brief ETC2 ���k�C���[�W���T�|�[�g���܂��B<br>
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
	//! @brief ASTC LDR ���k�C���[�W���T�|�[�g���܂��B<br>
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
	//! @brief BC ���k�C���[�W���T�|�[�g���܂��B<br>
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
//! @brief �V�F�[�_�[�̔\�̓t���O
enum V3D_SHADER_CAP_FLAG : V3DFlags
{
	//! @brief �W�I���g���V�F�[�_�[���g�p���邱�Ƃ��ł��܂��B
	V3D_SHADER_CAP_GEOMETRY = 0x00000001,
	//! @brief �e�b�Z���[�V�����V�F�[�_�[���g�p���邱�Ƃ��ł��܂��B
	V3D_SHADER_CAP_TESSELLATION = 0x00000002,
	//! @brief �o�b�t�@�ւ̃A�N�Z�X�ɑ΂��ċ��E���`�F�b�N����܂��B<br>
	//! �͈͊O�̃A�N�Z�X�ł̓A�v���P�[�V�����̏I�����������Ă͂Ȃ炸�A�V�F�[�_�̃��[�h�A�X�g�A�A����уA�g�~�b�N�̉e���́A��q��������ˑ��̓���ɏ������Ă���K�v������܂��B<br>
	//! <ul>
	//!   <li>���̂����ꂩ�����Ă͂܂�ꍇ�A�o�b�t�@�A�N�Z�X�͋��E�O�ł���Ƃ݂Ȃ���܂��B</li>
	//!   <ul>
	//!     <li>�|�C���^�[�� OpImageTexelPointer �ɂ���Č`������A���W�� 0 ��菬�������A�܂��͘A���͈͓��̗v�f�S�̂̐��ȏ�ł��B</li>
	//!     <li>OpImageTexelPointer �ɂ���ă|�C���^���`�����ꂸ�A�w���ꂽ�I�u�W�F�N�g���o�C���h���ꂽ�͈͓��Ɋ��S�Ɋ܂܂�Ă��Ȃ��B</li><br>
	//!     SPIR-V OpLoad ���߂��\���̂����[�h���A�\���̖̂��������E�O�ł���ꍇ�A�\���̂̂��ׂẴ����o�́A�����̃����o���ÓI�Ɏg�p����Ă��Ȃ��ꍇ�ł����E�O�Ƃ݂Ȃ���܂��B<br>
	//!     <li>����� SPIR-V �u���b�N���̃o�b�t�@�A�N�Z�X���͈͊O�ł���Ɣ��f���ꂽ�ꍇ�A<br>16 �o�C�g�����̃A�h���X�ɃA�N�Z�X���铯�� SPIR-V �u���b�N���̓����^�C�v ( ���[�h�A�X�g�A�A�܂��̓A�g�~�b�N ) �͈͊O�̃A�h���X���痣��Ă��A�͈͊O�ƌ��Ȃ����Ƃ��ł��܂��B</li>
	//!   </ul>
	//!   <li>�͈͊O�o�b�t�@���[�h�́A���̒l�̂����ꂩ��Ԃ��܂��B</li>
	//!   <ul>
	//!     <li>�o�b�t�@�Ƀo�C���h���ꂽ�������͈͓��̔C�ӂ̒l�B( �o�b�t�@�̍Ō���߂����������̃o�C�g���A�o�C���h���ꂽ�͈͂̏I���܂ł̃o�C�g�����܂މ\��������܂� )</li>
	//!     <li>�[���l�܂��̓x�N�g���ǂݎ��� (0, 0, 0, x) �x�N�g���B�����ŁAx�̓x�N�g�������̌^�ŕ\�����L���Ȓl�ł���A���̂����ꂩ�ɂȂ�܂��B</li>
	//!     <ul>
	//!       <li>�����t���܂��͕����Ȃ������R���|�[�l���g�̏ꍇ�A0,1�A�܂��͍ő�\���\�Ȑ��̐����l</li>
	//!       <li>���������_�̏ꍇ�� 0.0 �܂��� 1.0</li>
	//!     </ul>
	//!   </ul>
	//!   <li>�͈͊O�������݂́A�o�b�t�@�Ƀo�C���h���ꂽ�������͈͓��̒l��ύX�ł��܂����A���̃������͕ύX���Ȃ��ł��������B</li>
	//!   <li>�͈͊O�̃A�g�~�b�N�́A�o�b�t�@�Ƀo�C���h���ꂽ�������͈͓��̒l��ύX���邱�Ƃ��ł��܂����A���̃�������ύX���Ė���`�̒l��Ԃ����Ƃ͂ł��܂���B</li>
	//!   <li>���_���͑����́A�����̃A�h���X�ɑ����̃T�C�Y�����������̂��o�C���h���ꂽ�o�b�t�@�̃T�C�Y���傫���ꍇ�A�͈͊O�ƌ��Ȃ���܂��B<br>����ɁA����̒��_���̓o�C���f�B���O���g�p����C�ӂ̒��_���͑������͈͊O�ł���ꍇ�A<br>���̒��_�V�F�[�_�Ăяo���ɑ΂��邻�̒��_���̓o�C���f�B���O���g�p���邷�ׂĂ̒��_���͑����͔͈͊O�ƌ��Ȃ���܂��B</li>
	//!   <ul>
	//!     <li>���_���͑������͈͊O�ł���ꍇ�A���̂����ꂩ�̒l�����蓖�Ă��܂��B</li>
	//!     <ul>
	//!       <li>�o�b�t�@�Ƀo�C���h���ꂽ�������͈͓��̔C�ӂ̒l�B�����̌`���ɏ]���ĕϊ�����܂��B</li>
	//!       <li>0 �̒l�A�����̌`���ɏ]���ăt�H�[�}�b�g���ϊ�����܂��B</li>
	//!       <li>0 �̒l�A�܂��� (0, 0, 0, x�j�x�N�g���B</li>
	//!     </ul>
	//!   </ul>
	//!   <li>V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS ���L���ɂȂ��Ă��Ȃ��ƁA���E�O�A�N�Z�X�ɂ���ăv���Z�X���̃��������j�󂳂�A�A�v���P�[�V�����I���܂ł̖���`�̓��삪��������\��������܂��B</li>
	//! </ul>
	V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS = 0x00000004,
	//! @brief �X�g���[�W�o�b�t�@�܂��̓C���[�W���A�o�[�e�b�N�X�V�F�[�_�[�X�e�[�W�A�e�b�Z���[�V�����V�F�[�_�[�X�e�[�W�A�W�I���g���V�F�[�_�X�e�[�W�ɂ����āA
	//! �X�g�A�ƃA�g�~�b�N�I�y���[�V�������T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_���W���[���ł����̃X�e�[�W�Ŏg�p����邷�ׂẴX�g���[�W�C���[�W�A
	//! �X�g���[�W�e�N�Z���o�b�t�@�A����уX�g���[�W�o�b�t�@�ϐ��� NonWriteable readonly�i �܂��� GLSL�̃������C���q �j�𑕏�����K�v������܂��B
	V3D_SHADER_CAP_VERTEX_PIPELINE_STORES_AND_ATOMICS = 0x00000008,
	//! @brief �X�g���[�W�o�b�t�@�ƃC���[�W���t���O�����g�V�F�[�_�X�e�[�W�̃X�g�A�ƃA�g�~�b�N�I�y���[�V�������T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_���W���[���̃t���O�����g�X�e�[�W�Ŏg�p����邷�ׂẴX�g���[�W�C���[�W�A�X�g���[�W�e�N�Z���o�b�t�@�A����уX�g���[�W�o�b�t�@�ϐ��́ANonWriteable readonly�i �܂���GLSL�̃������C���q �j�ő�������Ă���K�v������܂��B
	V3D_SHADER_CAP_FRAGMENT_STORES_AND_ATOMICS = 0x00000010,
	//! @brief 1.0 ���傫���T�C�Y�̃|�C���g���g�p�ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_�[�ɂ���ď������܂ꂽ�|�C���g�T�C�Y 1.0 �݂̂��T�|�[�g����܂��B<br>
	//! �T�|�[�g����Ă���|�C���g�T�C�Y�͈̔͂Ɨ��x�́A���ꂼ�� V3DDeviceCaps �\���̂� pointSizeRange ����� pointSizeGranularity �ɂȂ�܂��B
	V3D_SHADER_CAP_LARGE_POINTS = 0x00000020,
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�A�e�b�Z���[�V�����]���V�F�[�_�[�A����уW�I���g���V�F�[�_�[�� gl_PointSize �ϐ����g�p�ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�́Agl_PointSize �ϐ���ǂݏ������Ȃ��ł��������B<br>
	//! �܂��A�e�b�Z���[�V�����V�F�[�_�[�܂��̓W�I���g���V�F�[�_�[���珑�����܂��|�C���g�̃T�C�Y�� 1.0 �ɂȂ�܂��B<br>
	//! ����́A�V�F�[�_���W���[�����e�b�Z���[�V��������V�F�[�_�[�ƃe�b�Z���[�V�����]���V�F�[�_�[�� TessellationPointSize �@�\�A
	//! �܂��̓W�I���g���V�F�[�_�[�� GeometryPointSize �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_TESSELLATION_AND_GEOMETRY_POINTSIZE = 0x00000040,
	//! @brief image gather ���߂̊g���Z�b�g���V�F�[�_�R�[�h�ŗ��p���邱�Ƃ��ł��܂��B<br>
	//! ���̋@�\���L���łȂ��ꍇ�AOpImage * Gather ���߂� Offset �I�y�����h�� ConstOffsets �I�y�����h���T�|�[�g���܂���B<br>
	//! ����́A�V�F�[�_���W���[���� ImageGatherExtended �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_IMAGE_GATHER_EXTENDED = 0x00000080,
	//! @brief �g���X�g���C�[�W�C���[�W�t�H�[�}�b�g���V�F�[�_�R�[�h�ŗ��p���邱�Ƃ��ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�AStorageImageExtendedFormats �@�\���K�v�Ȍ`���́A�X�g���[�W�C���[�W�ł̓T�|�[�g����Ă��܂���B<br>
	//! ����́A�V�F�[�_�[���W���[���� StorageImageExtendedFormats �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_IMAGE_EXTENDED_FORMATS = 0x00000100,
	//! @brief �}���`�T���v���̃X�g���[�W�C���[�W���T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ ::V3D_IMAGE_USAGE_STORAGE ���܂ގg�p�@�ŃC���[�W���쐬����Ƃ��̃T���v������ ::V3D_SAMPLE_COUNT_1 �ɂ���K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� StorageImageMultisample �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_IMAGE_MULTISAMPLE = 0x00000200,
	//! @brief �X�g���[�W�C���[�W����ǂݎ��Ƃ��ɁA�X�g���[�W�C���[�W�Ńt�H�[�}�b�g�C���q���w�肷��K�v�����邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�AOpImageRead ���߂� OpTypeImage �� Unknown �ł����Ă͂Ȃ�܂���B<br>
	//! ����́A�V�F�[�_�[���W���[���� StorageImageReadWithoutFormat �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_IMAGE_READ_WITH_OUT_FORMAT = 0x00000400,
	//! @brief �X�g���[�W�C���[�W�ɏ������ނƂ��� format �����q���w�肷��K�v�����邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�AOpImageWrite ���߂� OpTypeImage �� Unknown �ł����Ă͂Ȃ�܂���B<br>
	//! ����́A�V�F�[�_���W���[���� StorageImageWriteWithoutFormat �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_IMAGE_WRITE_WITH_OUT_FORMAT = 0x00000800,
	//! @brief ���j�t�H�[���o�b�t�@�̔z�񂪃V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���č����t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER �܂��� ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC �̃��\�[�X�́A
	//! �V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� UniformBufferArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B<br>
	V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00001000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���ăT���v���܂��̓T���v���C���[�W�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� ::V3D_DESCRIPTOR_TYPE_SAMPLER �A::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER �A
	//! �܂��� ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE �̃��\�[�X�́A�V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� SampledImageArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00002000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���ăX�g���[�W�o�b�t�@�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER �܂��� ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC �̃��\�[�X�́A
	//! �V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� StorageBufferArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00004000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���Ċi�[�C���[�W�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE �̃��\�[�X�́A
	//! �V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� StorageImageArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00008000,
	//! @brief �V�F�[�_�[�R�[�h�ŃN���b�v�������T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�AClipDistance �ő������ꂽ�����o�[�́A�V�F�[�_�[���W���[���Ƃ̊Ԃœǂݏ������邱�Ƃ͂ł��܂���B<br>
	//! ����́A�V�F�[�_���W���[���� ClipDistance �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_CLIP_DISTANCE = 0x00010000,
	//! @brief �V�F�[�_�R�[�h�ŃJ�����O�������T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�ACullDistance �ő������ꂽ�����o�[�́A�V�F�[�_�[���W���[���Ƃ̊Ԃœǂݏ������邱�Ƃ͂ł��܂���B<br>
	//! ����́A�V�F�[�_���W���[���� CullDistance �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_CULL_DISTANCE = 0x00020000,
	//! @brief �V�F�[�_�R�[�h�� 64 �r�b�g���������_���i double �^ �j���g�p���邱�Ƃ��ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�́A64 �r�b�g�̕��������_�^���V�F�[�_�R�[�h�Ŏg�p���Ȃ��ł��������B<br>
	//! ����́A�V�F�[�_���W���[���� Float64 �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_FLOAT64 = 0x00040000,
	//! @brief �V�F�[�_�R�[�h�� 64 �r�b�g�̐������g�p���邱�Ƃ��ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_�R�[�h�ł� 64 �r�b�g�����^���g�p���Ȃ��ł��������B<br>
	//! ����́A�V�F�[�_���W���[���� Int64 �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_INT64 = 0x00080000,
	//! @brief �V�F�[�_�R�[�h�� 16 �r�b�g�������g�p���邱�Ƃ��ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_�R�[�h�ł� 16 �r�b�g�����^���g�p���Ȃ��ł��������B<br>
	//! ����́A�V�F�[�_���W���[���� Int16 �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_INT16 = 0x00100000,
	//! @brief ���\�[�X�풓����Ԃ��C���[�W���삪�V�F�[�_�R�[�h�ŃT�|�[�g����Ă��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�́AOpImageSparse * ���߂��V�F�[�_�R�[�h�Ŏg�p���Ȃ��ł��������B<br>
	//! ����́A�V�F�[�_���W���[���� SparseResidency �@�\��錾�ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\�ł́A���Ȃ��Ƃ�1�� sparseResidency * �@�\���T�|�[�g����Ă���K�v������܂��B
	V3D_SHADER_CAP_RESOURCE_RESIDENCY = 0x00200000,
	//! @brief �ŏ����\�[�X���x���ڍ� (LOD) ���w�肷��C���[�W���삪�V�F�[�_�R�[�h���T�|�[�g���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�V�F�[�_�R�[�h�� MinLod �C���[�W�I�y�����h���g�p���Ȃ��ł��������B<br>
	//! ����́A�V�F�[�_���W���[���� MinLod �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_RESOURCE_MIN_LOD = 0x00400000,
};

//! @enum V3D_RASTERIZER_CAP_FLAG
//! @brief ���X�^���C�U�[�̔\�̓t���O
enum V3D_RASTERIZER_CAP_FLAG : V3DFlags
{
	//! @brief �����̃r���[�|�[�g�A�V�U�����O���T�|�[�g���܂��B<br>
	//! �܂��A�r���[�|�[�g�A�V�U�����O�̍ő吔�� V3DDeviceCaps::maxViewports �ɂȂ�܂��B
	//! @sa V3DPipelineRasterizationDesc
	//! @sa IV3DCommandBuffer::SetViewport
	//! @sa IV3DCommandBuffer::SetScissor
	V3D_RASTERIZER_CAP_MULTI_VIEWPORT = 0x00000001,
	//! @brief �|���S�����[�h�� ::V3D_POLYGON_MODE_LINE �A ::V3D_POLYGON_MODE_POINT ���w�肷�邱�Ƃ��ł��܂��B
	//! @sa V3DPipelineRasterizationDesc::polygonMode
	V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID = 0x00000002,
	//! @brief �[�x�̃N�����v���T�|�[�g���܂��B
	//! @sa V3DPipelineRasterizationDesc::depthClampEnable
	V3D_RASTERIZER_CAP_DEPTH_CLAMP = 0x00000010,
	//! @brief �[�x�o�C�A�X�̃N�����v���T�|�[�g���܂��B
	//! @sa V3DPipelineRasterizationDesc::depthBiasClamp
	V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP = 0x00000020,
};

//! @enum V3D_MULTISAMPLE_CAP_FLAG
//! @brief �}���`�T���v���̔\�̓t���O
enum V3D_MULTISAMPLE_CAP_FLAG : V3DFlags
{
	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v������ V3DPipelineMultisampleDesc::rasterizationSamples �Ɠ����l�ł���K�v���Ȃ����Ƃ�\���܂��B<br>
	//! ���̃t���O���w�肳��Ă��Ȃ������ꍇ�A�T�u�p�X�̑S�ẴJ���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v������ V3DPipelineMultisampleDesc::rasterizationSamples �Ɠ����l�łȂ��Ƃ����܂���B
	V3D_MULTISAMPLE_CAP_VARIABLE_RATE = 0x00000001,
	//! @brief �T���v�����̃V�F�[�f�B���O�ƃ}���`�T���v���̕�Ԃ��T�|�[�g���܂��B<br>
	//! ���̃t���O���w�肳��Ă���ꍇ�� V3DPipelineMultisampleDesc::sampleShadingEnable �� true �Ɏw�肷�邱�Ƃ��ł��܂��B
	V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING = 0x00000002,
	//! @brief �t���O�����g�V�F�[�_����o�͂��ꂽ�A���t�@�l���A�Œ菬���_���\���ł���ő�l�܂��� 1.0 �Œu�������邱�Ƃ��ł��܂��B<br>
	//! ���̃t���O���w�肳��Ă����ꍇ�A V3DPipelineMultisampleDesc::alphaToOneEnable ��true ���w�肷�邱�Ƃ��ł��܂��B
	V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE = 0x00000004,
};

//! @enum V3D_DEPTH_STENCIL_CAP_FLAG
//! @brief �f�v�X�X�e���V���̔\�̓t���O
enum V3D_DEPTH_STENCIL_CAP_FLAG : V3DFlags
{
	//! @brief �[�x�̋��E�e�X�g���T�|�[�g���܂��B<br>
	//! ���̃t���O���w�肳��Ă����ꍇ�AV3DPipelineDepthStencilDesc::depthBoundsTestEnable �� true ���w�肷�邱�Ƃ��ł��܂��B
	V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS = 0x00000001,
};

//! @enum V3D_COLOR_BLEND_CAP_FLAG
//! @brief �u�����h�̔\�̓t���O
enum V3D_COLOR_BLEND_CAP_FLAG : V3DFlags
{
	//! @brief �����̃J���[�A�^�b�`�����g�ɈقȂ�u�����h���w�肷�邱�Ƃ��ł��܂��B
	//! @sa V3DPipelineColorBlendDesc
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_INDEPENDENT = 0x00000001,
	//! @brief ��̃\�[�X���g�p����u�����h���T�|�[�g���܂��B<br>
	//! <br>
	//! ::V3D_BLEND_FACTOR_SRC1_COLOR<br>
	//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR<br>
	//! ::V3D_BLEND_FACTOR_SRC1_ALPHA<br>
	//! ::V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA<br>
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_DUAL_SRC = 0x00000002,
	//! @brief �J���[�̃u�����h�Ř_�����Z�q���T�|�[�g���܂��B
	//! @sa V3DPipelineColorBlendDesc::logicOpEnable
	//! @sa V3DPipelineColorBlendDesc::logicOp
	V3D_COLOR_BLEND_CAP_LOGIC_OP = 0x00000004,
};

//! @enum V3D_DEVICE_EXTENSION_FLAG
//! @brief �f�o�C�X�g���@�\�t���O
enum V3D_DEVICE_EXTENSION_FLAG : V3DFlags
{
	//! @brief �v�b�V���f�X�N���v�^�Z�b�g���g�p���邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::PushDescriptorSets
	V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS = 0x00000001,
	//! @brief �f�o�b�O�}�[�J�[���g�p���邱�Ƃ��ł��܂��B
	//! @remarks
	//! ���̊g���@�\���g�p�ł��郌�C���[�� ::V3D_LAYER_NSIGHT �܂��� ::V3D_LAYER_RENDERDOC �̂����ꂩ�ɂȂ�܂��B
	//! @sa IV3DCommandBuffer::BeginDebugMarker
	//! @sa IV3DCommandBuffer::EndDebugMarker
	//! @sa IV3DCommandBuffer::InsertDebugMarker
	V3D_DEVICE_EXTENSION_DEBUG_MARKER = 0x00000002,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDeviceCaps
//! @brief �f�o�C�X�̔\��
//! @sa IV3DDevice::GetCaps
struct V3DDeviceCaps
{
	V3DFlags flags; //!< �S�ʓI�Ȕ\�͂�\�� ::V3D_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags queryFlags; //!< �N�G���Ɋւ���̔\�͂�\�� ::V3D_QUERY_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags imageFlags; //!< �C���[�W�Ɋւ���̔\�͂�\�� ::V3D_IMAGE_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags shaderFlags; //!< �V�F�[�_�[�Ɋւ���̔\�͂�\�� ::V3D_SHADER_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags rasterizerFlags; //!< ���X�^���C�U�[�Ɋւ���\�͂�\�� ::V3D_RASTERIZER_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags multisampleFlags; //!< �}���`�T���v���Ɋւ���\�͂�\�� ::V3D_MULTISAMPLE_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags depthStencilFlags; //!< �f�v�X�X�e���V���Ɋւ���\�͂�\�� ::V3D_DEPTH_STENCIL_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags colorBlendFlags; //!< �J���[�u�����h�Ɋւ���\�͂�\�� ::V3D_COLOR_BLEND_CAP_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags extensionFlags; //!< �f�o�C�X�̊g���@�\��\�� ::V3D_DEVICE_EXTENSION_FLAG �񋓒萔�̑g�ݍ��킹�ł��B

	//! @brief 1D�C���[�W�̕��̍ő�l�ł��B
	uint32_t maxImageDimension1D;
	//! @brief 2D�C���[�W�̕��A�����̍ő�l�ł��B
	uint32_t maxImageDimension2D;
	//! @brief 3D�C���[�W�̕��A�����A�[���̍ő�l�ł��B
	uint32_t maxImageDimension3D;
	//! @brief �L���[�u�C���[�W�̕��A�����ő�l�ł��B
	uint32_t maxImageDimensionCube;
	//! @brief �C���[�W�̍ő僌�C���[���ł��B
	uint32_t maxImageLayers;
	//! @brief �e�N�Z���o�b�t�@�[�̗v�f�̍ő吔�ł��B
	uint32_t maxTexelBufferElements;
	//! @brief ���j�t�H�[���o�b�t�@�[�̍ő�T�C�Y�ł��B
	uint32_t maxUniformBufferSize;
	//! @brief �X�g���[�W�o�b�t�@�[�̍ő�T�C�Y�ł��B
	uint32_t maxStorageBufferSize;
	//! @brief IV3DCommandBuffer::PushConstant �ōX�V�ł���萔�̍ő�T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t maxPushConstantsSize;
	//! @brief IV3DCommandBuffer::PushDescriptorSets �ōX�V�ł���f�X�N���v�^�̐��ł��B<br>
	//! �f�X�N���v�^�Z�b�g�̐��ł͂Ȃ��A�f�X�N���v�^�Z�b�g�Ɋi�[����Ă���f�X�N���v�^�̐��ł���Ƃ������Ƃɒ��ӂ��Ă��������B
	uint32_t maxPushDescriptors;
	//! @brief �f�o�C�X���m�ۂł��郊�\�[�X�������̍ő吔�ł��B
	//! @sa IV3DDevice::AllocateResourceMemory
	uint32_t maxResourceMemoryCount;
	//! @brief �f�o�C�X���쐬�ł���T���v���[�̍ő吔�ł��B
	//! @sa IV3DDevice::CreateSampler
	uint32_t maxSamplerCreateCount;
	//! @brief �G�C���A�V���O�̂Ȃ��������������̗אڂ���o�b�t�@�[�܂��̓C���[�W���������邱�Ƃ��ł���I�t�Z�b�g�̗��x���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t bufferImageGranularity;
	//! @brief �p�C�v���C���Őݒ�ł���f�X�N���v�^�Z�b�g�̍ő吔�ł��B
	uint32_t maxBoundDescriptorSets;

	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł���T���v���[�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLER �A::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorSamplers;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł��郆�j�t�H�[���o�b�t�@�[�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER �A::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! </ul>
	uint32_t maxPerStageDescriptorUniformBuffers;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł���X�g���[�W�o�b�t�@�[�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER �A::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	//! </ul>
	uint32_t maxPerStageDescriptorStorageBuffers;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł���T���v�����O�C���[�W�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER �A::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE �܂��� ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorSampledImages;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł���X�g���[�W�C���[�W�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br> �܂��� ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER<br>
	//! </ul>
	uint32_t maxPerStageDescriptorStorageImages;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł���C���v�b�g�A�^�b�`�����g�̍ő吔�ł��B
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	//! </ul>
	uint32_t maxPerStageDescriptorInputAttachments;
	//! @brief �p�C�v���C�����C�A�E�g�ň�̃V�F�[�_�[�X�e�[�W�ɃA�N�Z�X�ł��郊�\�[�X�̍ő吔�ł��B<br>
	//! <ul>
	//! <li>�������K�p�����f�X�N���v�^�[</li><br>
	//! ::V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER<br>
	//! ::V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER<br>
	//! ::V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC<br>
	//! ::V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC<br>
	//! �܂���<br>
	//! ::V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT<br>
	//! <br>
	//! �܂��t���O�����g�V�F�[�_�X�e�[�W�̏ꍇ�A�t���[���o�b�t�@�[�̃J���[�A�^�b�`�����g�ɂ����̐������K�p����܂��B
	//! </ul>
	uint32_t maxPerStageResources;

	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���T���v���[�̍ő吔�ł��B
	uint32_t maxDescriptorSetSamplers;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł��郆�j�t�H�[���o�b�t�@�[�̍ő吔�ł��B
	uint32_t maxDescriptorSetUniformBuffers;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���_�C�i�~�b�N���j�t�H�[���o�b�t�@�[�̍ő吔�ł��B
	uint32_t maxDescriptorSetUniformBuffersDynamic;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���X�g���[�W�o�b�t�@�[�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageBuffers;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���_�C�i�~�b�N�X�g���[�W�o�b�t�@�[�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageBuffersDynamic;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���T���v�����O�C���[�W�̍ő吔�ł��B
	uint32_t maxDescriptorSetSampledImages;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���X�g���[�W�C���[�W�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageImages;
	//! @brief �f�X�N���v�^�Z�b�g�ɐݒ�ł���C���v�b�g�A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxDescriptorSetInputAttachments;

	//! @brief ���_�v�f�̐��ł��� V3DPipelineVertexInputDesc::elementCount �̍ő�l�ł��B
	uint32_t maxVertexInputElements;
	//! @brief ��x�Ƀo�C���h�ł��钸�_�o�b�t�@�̍ő吔�ł��B
	//! @sa IV3DCommandBuffer::BindVertexBuffers
	uint32_t maxVertexInputBindings;
	//! @brief ���_�v�f�̍ő�I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t maxVertexInputElementOffset;
	//! @brief ���_���C�A�E�g�̃X�g���C�h�ł��� V3DPipelineVertexLayout::stride �̍ő�T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t maxVertexInputBindingStride;
	//! @brief �o�[�e�b�N�X�V�F�[�_�[�Ő錾���邱�Ƃ̂ł���o�͕ϐ��̍ő吔�ł��B
	uint32_t maxVertexOutputComponents;

	//! @brief �e�b�Z���[�V�����v���~�e�B�u�W�F�l���[�^�[�̍ő�e�b�Z�[���[�V�����������x���ł��B
	uint32_t maxTessellationGenerationLevel;
	//! @brief �e�b�Z�[���[�V��������V�F�[�_�[����уe�b�Z�[���[�V�����v���~�e�B�u�W�F�l���[�^�[�ŏ����\�ȍő�p�b�`�T�C�Y�ł��B<br>
	//! �O���t�B�b�N�X�p�C�v���C���쐬���Ɏw�肵�� V3DPipelineTessellationDesc::patchControlPoints �̒l�ƁA�V�F�[�_�[�� OutputVertices ���s���[�h�Ŏw�肳�ꂽ�l�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxTessellationPatchSize;
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�Ő錾���邱�Ƃ̂ł��钸�_���̓��͕ϐ��̍ő吔�ł��B
	uint32_t maxTessellationControlPerVertexInputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�Ő錾���邱�Ƃ̂ł��钸�_���̏o�͕ϐ��̍ő吔�ł��B
	uint32_t maxTessellationControlPerVertexOutputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�Ő錾���邱�Ƃ̂ł���p�b�`���̏o�͕ϐ��̍ő吔�ł��B
	uint32_t maxTessellationControlPerPatchOutputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�Ő錾���邱�Ƃ̂ł��钸�_���A�y�уp�b�`���̏o�͕ϐ��̍ő升�v���ł��B
	uint32_t maxTessellationControlTotalOutputComponents;
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�[�Ő錾���邱�Ƃ̂ł��钸�_���̓��͕ϐ��̍ő吔�ł��B
	uint32_t maxTessellationEvaluationInputComponents;
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�[�Ő錾���邱�Ƃ̂ł��钸�_���̏o�͕ϐ��̍ő吔�ł��B
	uint32_t maxTessellationEvaluationOutputComponents;

	//! @brief �W�I���g���V�F�[�_�[�̍ő�Ăяo���񐔂ł��B<br>
	//! �V�F�[�_�[���W���[���� Invocations ���s���[�h�Ŏw�肳�ꂽ�l�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxGeometryShaderInvocations;
	//! @brief �W�I���g���V�F�[�_�[�Ő錾���邱�Ƃ̂ł�����͕ϐ��̍ő吔�ł��B
	uint32_t maxGeometryInputComponents;
	//! @brief �W�I���g���V�F�[�_�[�Ő錾���邱�Ƃ̂ł���o�͕ϐ��̍ő吔�ł��B
	uint32_t maxGeometryOutputComponents;
	//! @brief �W�I���g���V�F�[�_�[���o�͂��邱�Ƃ��ł��钸�_�̍ő吔�ł��B
	uint32_t maxGeometryOutputVertices;
	//! @brief �W�I���g���V�F�[�_�[�Ő錾���邱�Ƃ̂ł���o�͕ϐ��̍ő升�v���ł��B
	uint32_t maxGeometryTotalOutputComponents;

	//! @brief �t���O�����g�V�F�[�_�[�Ő錾���邱�Ƃ̂ł�����͕ϐ��̍ő吔�ł��B
	uint32_t maxFragmentInputComponents;
	//! @brief �t���O�����g�V�F�[�_�[���o�͂ł���A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxFragmentOutputAttachments;
	//! @brief �u�����h���L���ŁA�f���A���\�[�X�u�����h���[�h��1���g�p����Ă���Ƃ��ɁA�t���O�����g�V�F�[�_�̃X�e�[�W�ɂ���ď������܂��o�̓A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxFragmentDualSrcAttachments;
	//! @brief �t���O�����g�V�F�[�_�[�ŏo�͂ł���X�g���[�W�o�b�t�@�[�A�X�g���[�W�C���[�W�̑����ł��B
	uint32_t maxFragmentCombinedOutputResources;

	//! @brief �R���s���[�g�V�F�[�_�[�� WorkgroupLocal �X�g���[�W�N���X�Ő錾���ꂽ���ׂĂ̕ϐ��̍ő升�v�X�g���[�W�T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t maxComputeSharedMemorySize;
	//! @brief �R���s���[�g�V�F�[�_�[�Ŏ��� ( x y z ) ���ƂɃf�B�X�p�b�`�ł���O���[�v���ł��B
	V3DPoint3D maxComputeDispatchGroupCount;
	//! @brief �f�B�X�p�b�`�O���[�v�ŌĂяo����R���s���[�g�V�F�[�_�[�̍ő�񐔂ł��B
	uint32_t maxComputeDispatchGroupInvocations;
	//! @brief �R���s���[�g�V�F�[�_�[�Ŏ��� ( x y z ) ���Ƃ̃f�B�X�p�b�`�O���[�v�̍ő�T�C�Y�ł��B
	V3DPoint3D maxComputeDispatchGroupSize;

	//! @brief �t���[���o�b�t�@���W Xf�� Yf �ɂ�����T�u�s�N�Z�����x��\���r�b�g���ł��B
	uint32_t subPixelPrecisionBits;
	//! @brief �k������ъg��t�B���^�Ɏg�p�����摜�̎��ɉ��������Z�ɂ����鐸�x��\���r�b�g���ł��B<br>
	//! ���̒l�̓C���[�W�̊e���ɉ����������̎��ۂ̐��ł��B<br>
	//! �܂��t�B���^�����O���ꂽ���ʂ��v�Z����Ƃ��ɁA�t�B���^�����O�n�[�h�E�F�A�������̏ꏊ�ɃX�i�b�v���܂��B
	uint32_t subTexelPrecisionBits;
	//! @brief �e�~�b�v���x������t�B���^�����O���ʂւ̊�^�����肷��Ƃ��ɁA�t�F�b�`���邽�߂� LOD �v�Z���X�i�b�v�����r�b�g���ł��B
	uint32_t mipmapPrecisionBits;

	//! @brief �`��̍ۂɃo�C���h����C���f�b�N�X�o�b�t�@�Ɋi�[�ł���C���f�b�N�X�̍ő�l�ł��B
	uint32_t maxDrawIndexedIndexValue;

	//! @brief �T���v���[�� LOD �o�C�A�X�̐�Βl�ł��B<br>
	//! V3DSamplerDesc::mipLodBias �̒l�� -maxSamplerLodBias �` +maxSamplerLodBias �̊ԂɃN�����v���܂��B
	float maxSamplerLodBias;
	//! @brief �T���v���[�ٕ̈����l�ł��� V3DSamplerDesc::maxAnisotropy �̍ő�l�ł��B
	float maxSamplerAnisotropy;

	//! @brief �r���[�|�[�g�A�V�U�����O�̍ő吔�ł��B
	uint32_t maxViewports;
	//! @brief �r���[�|�[�g�̍ő�T�C�Y�ł��B
	V3DSize2D maxViewportDimension;
	//! @brief �r���[�|�[�g�̊p���܂܂�Ă��Ȃ���΂Ȃ�Ȃ��͈͂̍ŏ��l�ł��B
	float minViewportBounds;
	//! @brief �r���[�|�[�g�̊p���܂܂�Ă��Ȃ���΂Ȃ�Ȃ��͈͂̍ő�ł��B
	float maxViewportBounds;
	//! @brief �r���[�|�[�g���E�̃T�u�s�N�Z���̐��x��\���̃r�b�g���ł��B<br>
	//! �r���[�|�[�g���E�����f�����T�u�s�N�Z���̐��x�́A���̐����ɂ���ė^�����܂��B
	uint32_t viewportSubPixelBits;

	//! @brief IV3DResource::Map �Ń��\�[�X���}�b�v����ۂ̃I�t�Z�b�g�̃A���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	size_t minMemoryMapAlignment;
	//! @brief �e�N�Z���o�b�t�@�̃I�t�Z�b�g�̃A���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t minTexelBufferOffsetAlignment;
	//! @brief ���j�t�H�[���o�b�t�@�̃I�t�Z�b�g�̃A���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t minUniformBufferOffsetAlignment;
	//! @brief �X�g���[�W�o�b�t�@�̃I�t�Z�b�g�̃A���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t minStorageBufferOffsetAlignment;

	//! @brief OpImageSample * �܂��� OpImageFetch * �C���[�W���߂� ConstOffset �C���[�W�I�y�����h�̍ŏ��I�t�Z�b�g�l�ł��B
	int32_t minTexelOffset;
	//! @brief OpImageSample * �܂��� OpImageFetch * �C���[�W���߂� ConstOffset �C���[�W�I�y�����h�̍ő�I�t�Z�b�g�l�ł��B
	uint32_t maxTexelOffset;
	//! @brief OpImage * Gather �C���[�W���߂� Offset �܂��� ConstOffsets �C���[�W�I�y�����h�̍ŏ��I�t�Z�b�g�l�ł��B
	int32_t minTexelGatherOffset;
	//! @brief OpImage * Gather �C���[�W���߂� Offset �܂��� ConstOffsets �C���[�W�I�y�����h�̍ő�I�t�Z�b�g�l�ł��B
	uint32_t maxTexelGatherOffset;

	//! @brief InterpolateAtOffset �g�����߂� Offset �I�y�����h�̕��̍ŏ��I�t�Z�b�g�l�ł��B
	float minInterpolationOffset;
	//! @brief InterpolateAtOffset �g�����߂� Offset �I�y�����h�̐��̍ő�I�t�Z�b�g�l�ł��B
	float maxInterpolationOffset;
	//! @brief InterpolateAtOffset �g�����߂ɑ΂��� x ����� y �I�t�Z�b�g���Œ菬���_�l�Ƃ��Ċۂ߂���T�u�s�N�Z���̃r�b�g���ł��B
	uint32_t subPixelInterpolationOffsetBits;

	//! @brief �t���[���o�b�t�@�[�̃A�^�b�`�����g�̕��̍ő�ł��B
	uint32_t maxFrameBufferWidth;
	//! @brief �t���[���o�b�t�@�[�̃A�^�b�`�����g�̍����̍ő�ł��B
	uint32_t maxFrameBufferHeight;
	//! @brief �t���[���o�b�t�@�[�̃A�^�b�`�����g�̃��C���[�̍ő吔�ł��B
	uint32_t maxFrameBufferLayers;

	//! @brief �t���[���o�b�t�@�[�̃J���[�A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferColorSampleCounts;
	//! @brief �t���[���o�b�t�@�[�̃f�v�X�A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferDepthSampleCounts;
	//! @brief �t���[���o�b�t�@�[�̃X�e���V���A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferStencilSampleCounts;
	//! @brief �A�^�b�`�����g�̂Ȃ��t���[���o�b�t�@�[�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferNoAttachmentsSampleCounts;

	//! @brief �񐮐��J���[�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageColorSampleCounts;
	//! @brief �����J���[�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageIntegerSampleCounts;
	//! @brief �f�v�X�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageDepthSampleCounts;
	//! @brief �X�e���V���t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageStencilSampleCounts;
	//! @brief �X�g���[�W�Ƃ��č쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags storageImageSampleCounts;

	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxColorAttachments;

	//! @brief gl_SampleMask �z��̗v�f�̍ő吔�ł��B
	uint32_t maxSampleMaskWords;

	//! @brief �^�C���X�^���v�N�G�����C�N���������g���邽�߂ɕK�v�Ȏ��Ԃ��i�m�b�P�ʂŎw�肵�܂��B
	float timestampPeriod;

	//! @brief gl_ClipDistance �z��̗v�f�̍ő吔�ł��B
	uint32_t maxClipDistances;
	//! @brief gl_CullDistance �z��̗v�f�̍ő吔�ł��B
	uint32_t maxCullDistances;
	//! @brief gl_ClipDistance ����� gl_CullDistance �z��̃y�A�̍��v�̍ő吔�ł��B
	uint32_t maxCombinedClipAndCullDistances;

	//! @brief gl_PointSize �ŏ��l�ł��B<br>
	//! gl_PointSize �ϐ��̒l�́AminPointSize �ȏ�ł���K�v������܂��B
	float minPointSize;
	//! @brief gl_PointSize �̍ő�l�ł��B<br>
	//! gl_PointSize �ϐ��̒l�́AmaxPointSize �ȉ��ł���K�v������܂��B
	float maxPointSize;
	//! @brief gl_PointSize �̑����ł��B<br>
	//! minPointSize �`  maxPointSize �ɂ���Ē�`���ꂽ�͈͓��̂��ׂẴ|�C���g�T�C�Y���T�|�[�g����Ă��Ȃ����Ƃɒ��ӂ��Ă��������B
	float pointSizeGranularity;

	//! @brief �f�o�C�X�쐬���Ɏw�肷��L���[�ɃX�P�[�W���[�����O�Ƃ��Ċ��蓖�Ă邱�Ƃ��ł���ʂ̗D�揇�ʂ̐��ł��B
	uint32_t discreteQueuePriorities;
	//! @brief �}���`�T���v�����O�ŕW���̃T���v���ʒu���g�p���܂��B<br>
	//! �W���̃T���v���ʒu�͈ȉ��̂悤�ɂȂ�܂��B<br>
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
	//! @brief IV3DCommandBuffer::CopyBufferToImage ����� IV3DCommandBuffer::CopyImageToBuffer �Ŏw�肷��o�b�t�@�[�̂̍œK�ȃI�t�Z�b�g�A���C�������g�ł��B<br>
	//! �A�v���P�[�V�����͍œK�ȃp�t�H�[�}���X�𓾂邽�߂ɍœK�ȃA���C�����g���g�p����K�v������܂��B
	uint64_t optimalBufferCopyOffsetAlignment;
	//! @brief �z�X�g�Ń}�b�v���ꂽ�������ւ̓����A�N�Z�X�𐧌�����T�C�Y�ƃA���C�����g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t nonCoherentAtomSize;
};

//! @struct V3DDeviceStatistics
//! @brief �f�o�C�X�̓��v
struct V3DDeviceStatistics
{
	uint32_t samplerCount; //!< �쐬���ꂽ�T���v���[�̐��ł��B
	uint32_t resourceMemoryCount; //!< �쐬���ꂽ���\�[�X�������̐��ł��B
};

//! @struct V3DDeviceQueueFamilyDesc
//! @brief �L���[�t�@�~���[�̋L�q
struct V3DDeviceQueueFamilyDesc
{
	V3DFlags queueFlags; //!< �L���[�̑����\�� ::V3D_QUEUE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	uint32_t queueCount; //!< �L���[�̐��ł��B
};

//! @struct V3DImageFormatDesc
//! @brief �C���[�W�t�H�[�}�b�g�̋L�q
struct V3DImageFormatDesc
{
	uint32_t maxWidth; //!< �C���[�W�̍ő�̕��ł��B
	uint32_t maxHeight; //!< �C���[�W�̍ő�̍����ł��B
	uint32_t maxDepth; //!< �C���[�W�̍ő�̐[���ł��B
	uint32_t maxLevels; //!< �C���[�W�̍ő僌�x�����ł��B
	uint32_t maxLayers; //!< �C���[�W�̍ő僌�C���[���ł��B
	V3DFlags sampleCountFlags; //!< �C���[�W�̃T���v������\�� ::V3D_SAMPLE_COUNT_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DDevice
//! @brief �f�o�C�X�C���^�[�t�F�[�X
// !@sa IV3DInstance::CreateDevice
class V3D_DLL_API IV3DDevice : public IV3DObject
{
public:
	//! @brief �f�o�C�X�쐬���Ɏw�肵���A�_�v�^���擾���܂��B
	//! @param[out] ppAdapter �擾�����A�_�v�^��n�� IV3DAdapter �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetAdapter(IV3DAdapter** ppAdapter) = 0;

	//! @brief �f�o�C�X�̔\�͂��擾���܂��B
	//! @return �f�o�C�X�̔\�͂�Ԃ��܂��B
	virtual const V3DDeviceCaps& GetCaps() const = 0;

	//! @brief �f�o�C�X�̓��v���擾���܂��B
	//! @return �f�o�C�X�̓��v��Ԃ��܂��B
	virtual const V3DDeviceStatistics& GetStatistics() const = 0;

	//! @brief �L���[�t�@�~���[�̐����擾���܂��B
	//! @return �L���[�t�@�~���[�̐���Ԃ��܂��B
	virtual uint32_t GetQueueFamilyCount() const = 0;
	//! @brief �L���[�t�@�~���[�̋L�q���擾���܂��B
	//! @param[in] family �L���[�t�@�~���[�̃C���f�b�N�X�ł��B<br>
	//! ���̒l�� { 0 <= family < IV3DDevice::GetQueueFamilyCount } �͈̔͂ɐ�������܂��B
	//! @return �L���[�t�@�~���̋L�q��Ԃ��܂��B
	virtual const V3DDeviceQueueFamilyDesc& GetQueueFamilyDesc(uint32_t family) const = 0;
	//! @brief �L���[���擾���܂��B
	//! @param[in] family �L���[�t�@�~���[�̃C���f�b�N�X�ł��B
	//! @param[in] queue �L���[�̃C���f�b�N�X�ł��B
	//! @param[out] ppQueue �擾�����L���[��n�� IV3DQueue �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue) = 0;

	//! @brief �w�肵���C���[�W�̃t�H�[�}�b�g�̋L�q���擾���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] type �C���[�W�̃^�C�v�ł��B
	//! @param[in] tiling �C���[�W�̃^�C�����O�ł��B
	//! @param[in] usageFlags �g�p�@��\�� ::V3D_IMAGE_USAGE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[out] pDesc �擾�����C���[�W�t�H�[�}�b�g�̋L�q���i�[���� V3DImageFormatDesc �\���̂̃|�C���^�ł��B
	//! @retval V3D_OK �C���[�W�t�H�[�}�b�g�̋L�q���擾���܂����B
	//! @retval V3D_ERROR_NOT_SUPPORTED �w�肵�������̃C���[�W�̓T�|�[�g����Ă��܂���B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! pDesc �� nullptr ���w�肷�邱�ƂŃt�H�[�}�b�g�̃T�|�[�g�󋵂������m�F���邱�Ƃ��ł��܂��B
	virtual V3D_RESULT GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc) = 0;

	//! @brief �o�b�t�@�[�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �o�b�t�@�[�̃t�H�[�}�b�g�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� ::V3D_BUFFER_FORMAT_FEATURE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �t�H�[�}�b�g�͎w�肵���@�\���T�|�[�g���Ă��܂��B
	//! @retval V3D_ERROR_NOT_SUPPORTED �t�H�[�}�b�g�͎w�肵���@�\���T�|�[�g���Ă��܂���B
	virtual V3D_RESULT CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief �C���[�W�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� ::V3D_IMAGE_FORMAT_FEATURE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �t�H�[�}�b�g�͎w�肵���@�\���T�|�[�g���Ă��܂��B
	//! @retval V3D_ERROR_NOT_SUPPORTED �t�H�[�}�b�g�͎w�肵���@�\���T�|�[�g���Ă��܂���B
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief �C���[�W�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] tiling �C���[�W�̃^�C�����O�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� ::V3D_IMAGE_FORMAT_FEATURE_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �t�H�[�}�b�g�͎w�肵���^�C�����O�A�@�\���T�|�[�g���Ă��܂��B
	//! @retval V3D_ERROR_NOT_SUPPORTED �t�H�[�}�b�g�͎w�肵���^�C�����O�A�@�\���T�|�[�g���Ă��܂���B
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const = 0;

	//! @brief �R�}���h�v�[���[���쐬���܂��B
	//! @param[in] desc �R�}���h�v�[���̋L�q�ł��B
	//! @param[out] ppCommandPool �쐬�����R�}���h�v�[����n�� IV3DCommandPool �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �R�}���h�v�[�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�v�[���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief �R�}���h�o�b�t�@�[���쐬���܂��B
	//! @param[in] pPool �R�}���h�v�[���ł��B
	//! @param[in] bufferType �R�}���h�o�b�t�@�[�̃^�C�v�ł��B
	//! @param[out] ppCommandBuffer �쐬�����R�}���h�o�b�t�@�[��n�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �R�}���h�o�b�t�@�[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�o�b�t�@�[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandBuffer(IV3DCommandPool* pPool, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief �R�}���h�o�b�t�@�[���쐬���܂��B
	//! @param[in] poolDesc �R�}���h�v�[���̋L�q�ł��B
	//! @param[in] bufferType �R�}���h�o�b�t�@�[�̃^�C�v�ł��B
	//! @param[out] ppCommandBuffer �쐬�����R�}���h�o�b�t�@�[��n�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �R�}���h�o�b�t�@�[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �R�}���h�o�b�t�@�[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! �R�}���h�v�[���������ɍ쐬����܂��B
	virtual V3D_RESULT CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �X���b�v�`�F�C�����쐬���܂��B
	//! @param[in] swapChainDesc �X���b�v�`�F�C���̋L�q�ł��B
	//! @param[in] swapChainCallbacks �X���b�v�`�F�C���̃R�[���o�b�N�ł��B
	//! @param[out] ppSwapChain �쐬�����X���b�v�`�F�C����n�� IV3DSwapChain �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �X���b�v�`�F�C�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �X���b�v�`�F�C���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST @copydoc V3D_ERROR_DEVICE_LOST
	//! @remarks
	//! V3DSwapChainDesc �\���̂� imageFormat �Ŏw�肵���t�H�[�}�b�g��������Ȃ������ꍇ�́A�����Ńt�H�[�}�b�g�����肵�܂��B( ::V3D_FORMAT_UNDEFINED ���w��ł���Ƃ������Ƃ�\���܂� )<br>
	//! IV3DSwapChain::GetDesc �Ŏ擾�ł���L�q�́A�쐬���Ɏw�肵�����̂Ƃ͈قȂ�ꍇ�����邽�߁A�K�v�ɉ����Ď擾���Ȃ����Ă��������B<br>
	//! �܂� IV3DSwapChain::GetImage �Ŏ擾�ł���X���b�v�`�F�C���쐬����̃C���[�W�̃��C�A�E�g�� ::V3D_IMAGE_LAYOUT_UNDEFINED �ɂȂ��Ă��܂��B<br>
	virtual V3D_RESULT CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �t�F���X���쐬���܂��B
	//! @param[out] ppFence �쐬�����t�F���X��n�� IV3DFence �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �t�F���X���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �t�F���X�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFence(IV3DFence** ppFence, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �N�G���v�[�����쐬���܂��B
	//! @param[in] desc �N�G���v�[���̋L�q�ł��B
	//! @param[out] ppQueryPool �쐬�����N�G���v�[����n�� IV3DQueryPool �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �N�G���v�[�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �N�G���v�[���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �T���v���[���쐬���܂��B
	//! @param[in] desc �T���v���[�̋L�q�ł��B
	//! @param[out] ppSampler �쐬�����T���v���[��n�� IV3DSampler �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �T���v���[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �T���v���[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �쐬���邱�Ƃ̂ł���T���v���[�� V3DDeviceCaps �\���̂� maxSamplerCreateCount �̒l�̐��ɐ�������܂��B 
	virtual V3D_RESULT CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �V�F�[�_�[���W���[�����쐬���܂��B
	//! @param[in] codeSize �V�F�[�_�R�[�h�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! - ���̒l�� 4 �̐����{�ł���K�v������܂��B
	//! @param[in] pCode �V�F�[�_�R�[�h�ł��B
	//! @param[out] ppShaderModule �쐬�����V�F�[�_�[���W���[����n�� IV3DShaderModule �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �V�F�[�_�[���W���[�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �V�F�[�_�[���W���[���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �w�肵�����������������\�[�X���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�̃�����������\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pResource �������������m�F���郊�\�[�X��\�� IV3DResource �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @retval V3D_OK ���\�[�X�͎w�肵���������������T�|�[�g���Ă��܂��B
	//! @retval V3D_ERROR_NOT_SUPPORTED ���\�[�X�͎w�肵���������������T�|�[�g���Ă��܂���B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const = 0;
	//! @brief �w�肵�����������������\�[�X���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�̃�����������\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] resourceCount �������������m�F���郊�\�[�X�̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] ppResources �������������m�F���郊�\�[�X�̔z��ł��B
	//! - �z��̗v�f���� resourceCount �ł���K�v������܂��B
	//! @retval V3D_OK �S�Ẵ��\�[�X�͎w�肵���������������T�|�[�g���Ă��܂��B
	//! @retval V3D_ERROR_NOT_SUPPORTED �ꕔ�܂��͑S�Ẵ��\�[�X�͎w�肵���������������T�|�[�g���Ă��܂���B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const = 0;

	//! @brief ���\�[�X���������m�ۂ��܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] memorySize �m�ۂ��郁�����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[out] ppResourceMemory �m�ۂ������\�[�X��������n�� IV3DResourceMemory �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK ���\�[�X�������̊m�ۂɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�������̊m�ۂɎ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! IV3DDevice::CreateBuffer �܂��� IV3DDevice::CreateImage �ɂ���č쐬���ꂽ����̃��\�[�X�́A�����������蓖�Ă��Ă��Ȃ����߁A�g�p�ł��Ȃ���ԂɂȂ��Ă��܂��B<br>
	//! ���̂��� IV3DDevice::AllocateResourceMemory �Ń��������m�ۂ����� IV3DDevice::BindResourceMemory �Ń��\�[�X���������Ƀo�C���h����K�v������܂��B<br>
	//! <br>
	//! - �m�ۂ��邱�Ƃ̂ł��郊�\�[�X�������� V3DDeviceCaps::maxResourceMemoryCount �ȉ��ɐ�������܂��B
	//! .
	//! - ���������� ( memoryPropertyFlags ) �Ɏw��ł���t���O�̑g�ݍ��킹�͈ȉ��̂��̂Ɍ��肳��܂��B<br>
	//! <br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT ��<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_CACHED ��<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT | ::V3D_MEMORY_PROPERTY_HOST_CACHED ��<br>
	//! ::V3D_MEMORY_PROPERTY_DEVICE_LOCAL | ::V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_CACHED<br>
	//! ::V3D_MEMORY_PROPERTY_HOST_VISIBLE | ::V3D_MEMORY_PROPERTY_HOST_COHERENT | ::V3D_MEMORY_PROPERTY_HOST_CACHED<br>
	//! <br>
	//! �܂��A�����̑g�ݍ��킹���K�������g����킯�ł͂Ȃ����߁A���\�[�X�̃��������m�ۂ���O�� IV3DDevice::CheckResourceMemoryProperty �Ŏg�p�\���ǂ����𒲂ׂĂ��������B
	//! .
	//! - ���\�[�X�̃������T�C�Y�� IV3DResource::GetResourceDesc �Ŏ擾�ł��� V3DResourceDesc �\���̂̃����o�ł��� memorySize �Ŋm�F���邱�Ƃ��ł��܂��B<br>
	//! �܂����̒l�̓A���C�����g���K�p����Ă�����̂ɂȂ�܂��B
	virtual V3D_RESULT AllocateResourceMemory(V3DFlags memoryPropertyFlags, uint64_t memorySize, IV3DResourceMemory** ppResourceMemory, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief ���\�[�X���������Ƀo�C���h���܂��B
	//! @param[in] pResourceMemory �o�C���h��̃������ł��B
	//! @param[in] pResource �o�C���h���郊�\�[�X�ł��B
	//! @param[in] memoryOffset �o�C���h���郁�����̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @retval V3D_OK ���\�[�X���������Ƀo�C���h���܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X���������Ƀo�C���h�ł��܂���ł����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - ���\�[�X�̓������Ƀo�C���h���邱�Ƃɂ���ă������ւ̃A�N�Z�X���ł���悤�ɂȂ�܂��B<br>
	//! �܂���x�������Ƀo�C���h����ƃ��\�[�X����������܂Ńo�C���h���������邱�Ƃ͂ł��܂���B
	//! .
	//! - �������I�t�Z�b�g ( memoryOffset ) �̓��\�[�X�̎�ނɂ���ăA���C�����g�̐���������A�ȉ��̃����o�̐����{�̒l���w�肷��K�v������܂��B<br>
	//! <br>
	//! <table class="basic">
	//!   <tr><td>�e�N�Z���o�b�t�@�[</td><td>::V3DDeviceCaps::minTexelBufferOffsetAlignment</td></tr>
	//!   <tr><td>���j�t�H�[���o�b�t�@�[</td><td>::V3DDeviceCaps::minUniformBufferOffsetAlignment</td></tr>
	//!   <tr><td>�X�g���[�W�o�b�t�@�[</td><td>::V3DDeviceCaps::minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! ����Ƀz�X�g����A�N�Z�X�ł��郁���� ::V3D_MEMORY_PROPERTY_HOST_VISIBLE �ɂ��A���C�����g�̐��������邽�� ::V3DDeviceCaps::minMemoryMapAlignment ���l������K�v������܂��B
	virtual V3D_RESULT BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset) = 0;

	//! @brief ���\�[�X�̃��������m�ۂ��A���̃������Ƀo�C���h���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pResource �o�C���h���郊�\�[�X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK ���\�[�X�̃������m�ہA�o�C���h�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�̃������m�ہA�܂��̓o�C���h�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, IV3DResource* pResource, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief �����̃��\�[�X����̃������Ƃ��Ċm�ۂ��A���̃������Ƀo�C���h���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� ::V3D_MEMORY_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] resourceCount �o�C���h���郊�\�[�X�̐��ł��B
	//! - ���̒l�́A1 �ȏ�ł���K�v������܂��B
	//! @param[in] ppResources �o�C���h���郊�\�[�X��\�� IV3DResource �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! - �z��̗v�f���� resourceCount �ł���K�v������܂��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK ���\�[�X�̃������m�ہA�o�C���h�ɐ������܂����B
	//! @retval V3D_ERROR_FAIL ���\�[�X�̃������m�ہA�܂��̓o�C���h�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �o�b�t�@�[���쐬���܂��B
	//! @param[in] desc �o�b�t�@�[�̋L�q�ł��B
	//! @param[in] ppBuffer �쐬�����o�b�t�@�[��n�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �o�b�t�@�[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �o�b�t�@�[�ɕ����̃T�u���\�[�X���܂߂�ꍇ�́A�T�u���\�[�X��z�u���郁�����I�t�Z�b�g�̃A���C�����g�ɒ��ӂ��ăo�b�t�@�[�̃T�C�Y�����肷��K�v������܂��B<br>
	//! �T�u���\�[�X�̃A���C�����g�� V3DDeviceCaps �ɋL�q����Ă���A�ȉ��̂悤�ɂȂ�܂��B<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>::V3D_BUFFER_USAGE_UNIFORM_TEXEL <br> ::V3D_BUFFER_USAGE_STORAGE_TEXEL</td><td>minTexelBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_BUFFER_USAGE_UNIFORM</td><td>minUniformBufferOffsetAlignment</td></tr>
	//! <tr><td>::V3D_BUFFER_USAGE_STORAGE</td><td>minStorageBufferOffsetAlignment</td></tr>
	//! </table>
	//! <br>
	//! �܂��A�z�X�g����A�N�Z�X�ł��郁���� ::V3D_MEMORY_PROPERTY_HOST_VISIBLE �ɂ��A���C�����g�̐��������邽�� V3DDeviceCaps::minMemoryMapAlignment ���l������K�v������܂��B
	//! @sa IV3DDevice::CheckBufferFormatFeature
	virtual V3D_RESULT CreateBuffer(const V3DBufferDesc& desc, IV3DBuffer** ppBuffer, const wchar_t* pDebugName = nullptr) = 0;
	//! @brief �C���[�W���쐬���܂��B
	//! @param[in] imageDesc �C���[�W�̋L�q�ł��B
	//! @param[in] initialLayout �C���[�W�̏������C�A�E�g�ł��B
	//! - �C���[�W�̏������C�A�E�g�� ::V3D_IMAGE_LAYOUT_UNDEFINED �܂��� ::V3D_IMAGE_LAYOUT_PREINITIALIZED �̂����ꂩ�ł���K�v������܂��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @param[out] ppImage �쐬�����C���[�W��n�� IV3DImage �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @retval V3D_OK �C���[�W���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �C���[�W�̋L�q�ł��� V3DImageDesc �\���̂̃����o�̒l�ɂ͐���������A���̐����� IV3DDevice::GetImageFormatDesc ����� IV3DDevice::CheckImageFormatFeature ���g�p���Ċm�F���邱�Ƃ��ł��܂��B<br>
	//! �܂� V3DImageDesc �\���̂̃����o�ł��� tiling �� ::V3D_IMAGE_TILING_LINEAR ���w�肵���ꍇ�A�ȉ��̒l�Ɍ�������̐����͂���܂���B<br>
	//! <br>
	//! <table class="basic">
	//! <tr><td>imageType</td><td>::V3D_IMAGE_TYPE_2D</td></tr>
	//! <tr><td>format</td><td>�f�v�X�X�e���V���ȊO�̃t�H�[�}�b�g</td></tr>
	//! <tr><td>levelCount</td><td>1</td></tr>
	//! <tr><td>layerCount</td><td>1</td></tr>
	//! <tr><td>samples</td><td>::V3D_SAMPLE_COUNT_1</td></tr>
	//! <tr><td>usageFlags</td><td>::V3D_IMAGE_USAGE_TRANSFER_SRC �� ::V3D_IMAGE_USAGE_TRANSFER_DST �̗����A�܂��͂����ꂩ</td></tr>
	//! </table>
	//! <br>
	virtual V3D_RESULT CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �o�b�t�@�[�r���[���쐬���܂��B
	//! @param[in] pBuffer �A�N�Z�X��̃o�b�t�@�[�ł��B
	//! @param[in] desc �o�b�t�@�[�r���[�̋L�q�ł��B
	//! @param[out] ppBufferView �쐬�����o�b�t�@�[�r���[��\�� IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �o�b�t�@�[�r���[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �o�b�t�@�[�r���[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �A�N�Z�X��̃o�b�t�@�[�̓��\�[�X�������Ƀo�C���h����Ă���K�v������܂��B
	//! .
	//! - �V�F�[�_�[���C���[�W�Ƃ��ăA�N�Z�X���邱�Ƃ̂ł���f�X�N���v�^ ( ::V3D_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER �܂��� ::V3D_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER ) �Ƃ��Ďg�p�������ꍇ�́A�o�b�t�@�[�r���[���쐬����K�v������܂��B<br>
	//! �܂��A�A�N�Z�X��̃o�b�t�@�[�̎g�p�@�ł��� V3DBufferDesc::usageFlags �ɂ� ::V3D_BUFFER_USAGE_UNIFORM_TEXEL �܂��� ::V3D_BUFFER_USAGE_STORAGE_TEXEL �̂����ꂩ���܂܂�Ă���K�v������܂��B
	virtual V3D_RESULT CreateBufferView(IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, IV3DBufferView** ppBufferView, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �C���[�W�r���[���쐬���܂��B
	//! @param[in] pImage �A�N�Z�X��̃C���[�W�ł��B
	//! @param[in] desc �C���[�W�r���[�̋L�q�ł��B
	//! @param[out] ppImageView �쐬�����C���[�W�r���[��n�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �C���[�W�r���[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �C���[�W�r���[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �A�N�Z�X��̃C���[�W�̓��\�[�X�������Ƀo�C���h����Ă���K�v������܂��B
	//! .
	//! - �V�F�[�_�[���A�N�Z�X���邱�Ƃ̂ł���f�X�N���v�^�Ƃ��Ďg�p�������ꍇ�́A�C���[�W�r���[���쐬����K�v������܂��B
	virtual V3D_RESULT CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �����_�[�p�X���쐬���܂��B
	//! @param[in] attachmentCount �A�^�b�`�����g�̐��ł��B
	//! - ���̒l�� 1�ȏ�ł���K�v������܂��B
	//! @param[in] pAttachments �A�^�b�`�����g�̎Q�Ɣz��ł��B
	//! - �z��̗v�f���� attachmentCount �ł���K�v������܂��B
	//! @param[in] subpassCount �T�u�p�X�̐��ł��B
	//! - ���̒l�� 1�ȏ�ł���K�v������܂��B
	//! @param[in] pSubpasses �T�u�p�X�̔z��ł��B
	//! - �z��̗v�f���� subpassCount �ł���K�v������܂��B
	//! @param[in] subpassDependencyCount �T�u�p�X�̈ˑ����̐��ł��B
	//! - �ȉ��̂����ꂩ�ɊY������ꍇ�́A1 �ȏ�̒l���w�肷��K�v������܂��B
	//!  -# �T�u�p�X�̐� ( subpassCount ) �� 2 �ȏ�̏ꍇ�B
	//!  -# �����_�[�p�X�J�n�O�̃C���[�W���C�A�E�g�ƁA�����_�[�p�X�J�n���̃C���[�W���C�A�E�g ( V3DAttachmentDesc::initialLayout ) ���قȂ�ꍇ�B
	//!  -# �����_�[�p�X�J�n���̃C���[�W���C�A�E�g ( V3DAttachmentDesc::initialLayout ) �ƁA�����_�[�p�X�I�����̃C���[�W���C�A�E�g ( V3DAttachmentDesc::finalLayout ) ���قȂ�ꍇ�B
	//! @param[in] pSubpassDependencies �T�u�p�X�̈ˑ����̔z��ł��B
	//! - subpassDependencyCount �� 1 �ȏ�̏ꍇ�A�v�f���� subpassDependencyCount �ł���z����w�肵�܂��B
	//! - subpassDependencyCount �� 0 �̏ꍇ�� nullptr ���w�肵�܂��B
	//! @param[out] ppRenderPass �쐬���������_�[�p�X��\�� IV3DRenderPass �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �����_�[�p�X���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �����_�[�p�X�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateRenderPass(
		uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments,
		uint32_t subpassCount, const V3DSubpassDesc* pSubpasses,
		uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies,
		IV3DRenderPass** ppRenderPass,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �t���[���o�b�t�@�[���쐬���܂��B
	//! @param[in] pRenderPass �����_�[�p�X�ł��B
	//! @param[in] attachmentCount �A�^�b�`�����g�̐��ł��B
	//! - ���̒l�́A1�ȏ�ł���K�v������܂��B
	//! @param[in] ppAttachments �A�^�b�`�����g�Ƃ��Ďg�p����C���[�W�r���[�̔z��ł��B
	//! - �z��̗v�f���� attachmentCount �ł���K�v������܂��B
	//! - pRenderPass ���Q�Ƃ���A�^�b�`�����g�̃t�H�[�}�b�g�A�T���v�����͈�v���Ă���K�v������܂��B
	//! @param[out] ppFrameBuffer �쐬�����t���[���o�b�t�@��n�� IV3DFrameBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �t���[���o�b�t�@�[���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �t���[���o�b�t�@�[�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �A�^�b�`�����g�Ƃ��Ďg�p����S�ẴC���[�W�r���[�� ���A�����A�[���A���C���[������v���Ă���K�v������܂��B
	//! @sa IV3DDevice::CreateRenderPass
	virtual V3D_RESULT CreateFrameBuffer(IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���C�A�E�g���쐬���܂��B
	//! @param[in] descriptorCount �f�X�N���v�^�̐��ł��B
	//! - ���̒l�́A1 �ȏ�ł���K�v������܂��B
	//! @param[in] pDescriptors �f�X�N���v�^�̔z��ł��B
	//! - �z��̗v�f���� descriptorCount �ł���K�v������܂��B
	//! @param[in] poolSize �f�X�N���v�^�Z�b�g�̃v�[���̏����T�C�Y�ł��B
	//! @param[in] poolResizeStep �f�X�N���v�^�Z�b�g�̃v�[���̃��T�C�Y�X�e�b�v�ł��B
	//! @param[out] ppDescriptorSetLayout �쐬�����f�X�N���v�^�Z�b�g���C�A�E�g��n�� IV3DDescriptorSetLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �f�X�N���v�^�Z�b�g���C�A�E�g���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �f�X�N���v�^�Z�b�g���C�A�E�g�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED �v�b�V���f�X�N���v�^�Z�b�g���T�|�[�g���Ă��܂���B
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �f�X�N���v�^�Z�b�g�ɂ� IV3DCommandBuffer::BindDescriptorSets �Ńo�C���h����X�^���_�[�h�f�X�N���v�^�Z�b�g�ƁA
	//! IV3DCommandBuffer::PushDescriptorSets �ōX�V����v�b�V���f�X�N���v�^�Z�b�g�̂Q��ނ�����܂��B<br>
	//! �O�҂̃X�^���_�[�h�f�X�N���v�^�Z�b�g�ɑΉ��������C�A�E�g�́ApoolSize �� 1�ȏ�ApoolResizeStep �� 0 �܂��� 1�ȏ���w�肵���ꍇ�ɍ쐬���邱�Ƃ��ł��A<br>
	//! ��҂̃v�b�V���f�X�N���v�^�Z�b�g�ɑΉ��������C�A�E�g�́ApoolSize poolResizeStep �̗����� 0 ���w�肵���ꍇ�ɍ쐬���邱�Ƃ��ł��܂��B
	virtual V3D_RESULT CreateDescriptorSetLayout(
		uint32_t descriptorCount, const V3DDescriptorDesc* pDescriptors,
		uint32_t poolSize, uint32_t poolResizeStep,
		IV3DDescriptorSetLayout** ppDescriptorSetLayout,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���쐬���܂��B
	//! @param[in] pLayout �f�X�N���v�^�Z�b�g���C�A�E�g�ł��B
	//! @param[out] ppDescriptorSet �쐬�����f�X�N���v�^�Z�b�g��n�� IV3DDescriptorSet �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �f�X�N���v�^�Z�b�g���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �f�X�N���v�^�Z�b�g�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @remarks
	//! - �쐬�����f�X�N���v�^�Z�b�g�́A�w�肳�ꂽ�f�X�N���v�^�Z�b�g���C�A�E�g�̃^�C�v ( IV3DDescriptorSetLayout::GetType ) �ɂ���ăR�}���h�o�b�t�@�[�ւ̐ݒ���@���قȂ�܂��B<br>
	//! <br>
	//! <table class="basic">
	//!   <tr><th>�^�C�v</th><th>�ݒ���@</th></tr>
	//!   <tr><td>�X�^���_�[�h�f�X�N���v�^�Z�b�g<br> ( ::V3D_DESCRIPTOR_SET_TYPE_STANDARD )</td><td>IV3DCommandBuffer::BindDescriptorSets</td></tr>
	//!   <tr><td>�v�b�V���f�X�N���v�^�Z�b�g<br> ( ::V3D_DESCRIPTOR_SET_TYPE_PUSH )</td><td>IV3DCommandBuffer::PushDescriptorSets</td></tr>
	//! </table>
	//! <br>
	virtual V3D_RESULT CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �p�C�v���C�����C�A�E�g���쐬���܂��B
	//! @param[in] constantCount �v�b�V���萔�̐��ł��B
	//! @param[in] pConstants constantCount �v�b�V���萔�̔z��ł��B<br>
	//! - constantCount �� 1 �ȏ�̏ꍇ�A�v�f���� constantCount �̔z����w�肷��K�v������܂��B
	//! - constantCount �� 0 �̏ꍇ�́Anullptr ���w�肵�܂��B
	//! - �v�b�V���萔�̃T�C�Y ( V3DConstantDesc::offset + V3DConstantDesc::size �̍ő�l ) �� V3DDeviceCaps::maxPushConstantsSize �ȉ��ɐ�������܂��B
	//! @param[in] descriptorSetLayoutCount �f�X�N���v�^�Z�b�g���C�A�E�g�̐��ł��B
	//! @param[in] ppDescriptorSetLayouts �f�X�N���v�^���C�A�E�g�̔z��ł��B
	//! - descriptorSetLayoutCount �� 1 �ȏ�̏ꍇ�A�v�f���� descriptorSetLayoutCount �̔z����w�肷��K�v������܂��B
	//! - descriptorSetLayoutCount �� 0 �̏ꍇ�́Anullptr ���w�肵�܂��B
	//! @param[out] ppPipelineLayout �쐬�����p�C�v���C�����C�A�E�g��\�� IV3DPipelineLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �p�C�v���C�����C�A�E�g���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �p�C�v���C�����C�A�E�g�̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreatePipelineLayout(
		uint32_t constantCount, V3DConstantDesc* pConstants,
		uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts,
		IV3DPipelineLayout** ppPipelineLayout,
		const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �O���t�B�b�N�X�p�C�v���C�����쐬���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] pipelineDesc �O���t�B�b�N�X�p�C�v���C���̋L�q�ł��B
	//! @param[out] ppPipeline �쐬�����O���t�B�b�N�X�p�C�v���C����n�� IV3DPipeline �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �O���t�B�b�N�X�p�C�v���C�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �O���t�B�b�N�X�p�C�v���C���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �R���s���[�g�p�C�v���C�����쐬���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] pipelineDesc �R���s���[�g�p�C�v���C���̋L�q�ł��B
	//! @param[out] ppPipeline �쐬�����R���s���[�g�p�C�v���C����n�� IV3DPipeline �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �R���s���[�g�p�C�v���C�����쐬���܂����B
	//! @retval V3D_ERROR_FAIL �R���s���[�g�p�C�v���C���̍쐬�Ɏ��s���܂����B
	//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName = nullptr) = 0;

	//! @brief �f�o�C�X���A�C�h����ԂɂȂ�܂őҋ@���܂�
	//! @retval V3D_OK �f�o�C�X�̓A�C�h����ԂɂȂ�܂����B
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
// �C���X�^���X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_LAYER
//! @brief ���C���[
enum V3D_LAYER : uint8_t
{
	//! @brief ���؃��C���[�ł��B<br>
	//! �f�o�b�O���͂��̃��C���[���g�p���܂��B<br>
	//! <br>
	//! �g�p����� Vulkan �̃��C���[<br>
	//! VK_LAYER_LUNARG_standard_validation
	V3D_LAYER_VALIDATION = 0,
	//! @brief �œK�ȃ��C���[�ł��B<br>
	//! �����[�X���͂��̃��C���[���g�p���܂��B<br>
	//! <br>
	//! �g�p����� Vulkan �̃��C���[<br>
	//! N/A
	V3D_LAYER_OPTIMAL = 1,
	//! @brief Nsight �̃��C���[�ł��B<br>
	//! Nsight ���g�p���ă��j�^�����O����ꍇ�͂��̃��C���[���g�p���܂��B<br>
	//! <br>
	//! �g�p����� Vulkan �̃��C���[<br>
	//! N/A
	V3D_LAYER_NSIGHT = 2,
	//! @brief RenderDoc �̃��C���[�ł��B<br>
	//! RenderDoc ���g�p���ăL���v�`�������O����ꍇ�͂��̃��C���[���g�p���܂��B<br>
	//! <br>
	//! �g�p����� Vulkan �̃��C���[<br>
	//! VK_LAYER_RENDERDOC_Capture
	//! @warning
	//! RenderDoc �̌��݂̎����ł́A�g�p�ł���L���[��������ɂȂ�܂��̂Œ��ӂ��Ă��������B
	V3D_LAYER_RENDERDOC = 3,
};

//! @enum V3D_LOG_FLAG
//! @brief ���O�̃t���O
enum V3D_LOG_FLAG : V3DFlags
{
	V3D_LOG_INFORMATION = 0x00000001, //!< ���ł��B
	V3D_LOG_WARNING = 0x00000002, //!< �x���ł��B
	V3D_LOG_PERFORMANCE_WARNING = 0x00000004, //!< �p�t�H�[�}���X�Ɋւ���x���ł��B
	V3D_LOG_ERROR = 0x00000008, //!< �G���[�ł��B
	V3D_LOG_DEBUG = 0x00000010, //!< �f�o�b�O�ł��B

	V3D_LOG_STANDARD = V3D_LOG_WARNING | V3D_LOG_ERROR, //!< �W���̃��O��\�����܂��B
	V3D_LOG_ALL = V3D_LOG_INFORMATION | V3D_LOG_WARNING | V3D_LOG_PERFORMANCE_WARNING | V3D_LOG_ERROR | V3D_LOG_DEBUG, //!< ���ׂẴ��O��\�����܂��B
};

//! @}

//! @addtogroup v3d_callback_group
//! @{

//! @fn void PV3DAllocateMemoryFunction(size_t size, size_t alignment, void* pUserData)
//! @brief �������̊m�ۂ�����R�[���o�b�N�֐��̒�`�ł��B
//! @param[in] size �m�ۂ��郁�����̃T�C�Y�ł��B
//! @param[in] alignment �m�ۂ��郁�����̃A���C�������g�ł��B
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData �ł��B
//! @return �m�ۂ����������̃|�C���^��Ԃ��܂��B
typedef void* (V3D_CALLBACK *PV3DAllocateMemoryFunction)(size_t size, size_t alignment, void* pUserData);
//! @fn void PV3DReallocateMemoryFunction(void* pMemory, size_t size, size_t alignment, void* pUserData)
//! @brief �������̊m�ۂ�����R�[���o�b�N�֐��̒�`�ł��B
//! @param[in] pMemory �Ċm�ۂ��錳�̃������̃|�C���^�ł��B
//! @param[in] size �m�ۂ��郁�����̃T�C�Y�ł��B
//! @param[in] alignment �m�ۂ��郁�����̃A���C�������g�ł��B
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData �ł��B
//! @return �Ċm�ۂ����������̃|�C���^��Ԃ��܂��B
typedef void* (V3D_CALLBACK *PV3DReallocateMemoryFunction)(void* pMemory, size_t size, size_t alignment, void* pUserData);
//! @fn void PV3DFreeMemoryFunction(void* pMemory, void* pUserData)
//! @brief �������̊m�ۂ�����R�[���o�b�N�֐��̒�`�ł��B
//! @param[in] pMemory ������郁�����̃|�C���^�ł��B
//! @param[in] pUserData V3DInstanceMemoryDesc::pUserData �ł��B
typedef void (V3D_CALLBACK *PV3DFreeMemoryFunction)(void* pMemory, void* pUserData);

//! @fn void PV3DLogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData)
//! @brief �������̊m�ۂ�����֐��̒�`�ł��A
//! @param[in] type ���O�̃^�C�v�ł��B
//! @param[in] pMessage ���O�̃��b�Z�[�W�ł��B
//! @param[in] pUserData V3DInstanceLogDesc::pUserData �ł��B
typedef void (V3D_CALLBACK *PV3DLogFunction)(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData);

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DInstanceMemoryDesc
//! @brief �C���X�^���X�̃������̋L�q
struct V3DInstanceMemoryDesc
{
	PV3DAllocateMemoryFunction pAllocateFunction; //!< ���������m�ۂ���֐��̃|�C���^�ł��B
	PV3DReallocateMemoryFunction pReallocateFunction; //!< ���������Ċm�ۂ���֐��̃|�C���^�ł��B
	PV3DFreeMemoryFunction pFreeFunction; //!< ���������������֐��̃|�C���^�ł��B
	void* pUserData; //!< �R�[���o�b�N�֐��ɓn����郆�[�U�[�f�[�^�ł��B
};

//! @struct V3DInstanceLogDesc
//! @brief �C���X�^���X�̃��O�̋L�q
struct V3DInstanceLogDesc
{
	V3DFlags flags; //!< ���O�̃t���O��\�� ::V3D_LOG_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
	PV3DLogFunction pFunction; //!< ���O���o�͂���֐��̃|�C���^�ł��B
	void* pUserData; //!< �R�[���o�b�N�֐��ɓn����郆�[�U�[�f�[�^�ł��B
};

//! @struct V3DInstanceDesc
//! @brief �C���X�^���X�̋L�q
struct V3DInstanceDesc
{
	V3D_LAYER layer; //!< ���C���[��\�� ::V3D_LAYER �񋓒萔�̂����ꂩ���w�肵�܂��B
	V3DInstanceMemoryDesc memory; //!< �������̋L�q�ł��B
	V3DInstanceLogDesc log; //!< ���O�̋L�q�ł��B
};

//! @}

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DInstance
//! @brief �C���X�^���X�C���^�[�t�F�[�X
//! @sa V3DCreateInstance
class V3D_DLL_API IV3DInstance : public IV3DObject
{
public:
	//! @brief ���C���[���擾���܂��B
	//! @return ���C���[��Ԃ��܂��B
	virtual V3D_LAYER GetLayer() const = 0;

	//! @brief �A�_�v�^�̐����擾���܂��B
	//! @return �A�_�v�^�̐���Ԃ��܂��B
	virtual uint32_t GetAdapterCount() const = 0;
	//! @brief �A�_�v�^���擾���܂��B
	//! @param[in] adapterIndex �A�_�v�^�̃C���f�b�N�X�ł��B<br>
	//! ���̒l�� { 0 <= adapterIndex < IV3DInstance::GetAdapterCount } �͈̔͂ɐ�������܂��B
	//! @param[out] ppAdapter �擾�����A�_�v�^��n�� IV3DAdapter �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) = 0;

	//! @brief �f�o�C�X���쐬���܂��B
	//! @param[in] pAdapter �A�_�v�^�ł��B
	//! @param[out] ppDevice �쐬�����f�o�C�X��n�� IV3DDevice �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @param[in] pDebugName �f�o�b�O���ł��B
	//! @retval V3D_OK �f�o�C�X���쐬���܂����B
	//! @retval V3D_ERROR_FAIL �f�o�C�X�̍쐬�Ɏ��s���܂����B
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

//! @brief �w�肵�����C���[���T�|�[�g����Ă��邩�ǂ������m�F���܂��B
//! @param[in] layer �m�F���郌�C���[�ł��B
//! @retval V3D_OK �w�肵�����C���[�̓T�|�[�g���Ă��܂��B
//! @retval V3D_ERROR_FAIL �w�肵�����C���[�̓T�|�[�g����Ă��܂���B
//! @retval V3D_ERROR_NOT_SUPPORTED
V3D_DLL_API V3D_RESULT V3DCheckLayer(V3D_LAYER layer);

//! @brief �C���X�^���X���쐬���܂��B
//! @param[in] instanceDesc �C���X�^���X�̋L�q��\�� V3DInstanceDesc �\���̂ł��B
//! @param[out] ppInstance �쐬�����C���X�^���X��n�� IV3DInstance �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
//! @retval V3D_OK �C���X�^���X���쐬���܂����B
//! @retval V3D_ERROR_FAIL �C���X�^���X�̍쐬�Ɏ��s���܂����B
//! @retval V3D_ERROR_INVALID_ARGUMENT @copydoc V3D_ERROR_INVALID_ARGUMENT
//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY @copydoc V3D_ERROR_OUT_OF_HOST_MEMORY
//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY @copydoc V3D_ERROR_OUT_OF_DEVICE_MEMORY
V3D_DLL_API V3D_RESULT V3DCreateInstance(const V3DInstanceDesc& instanceDesc, IV3DInstance** ppInstance);

//! @}
