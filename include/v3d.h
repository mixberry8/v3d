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

//! @def V3D_CALLBACK
//! @brief v3d �̃R�[���o�b�N��`
#define V3D_CALLBACK __stdcall

//! @def V3D_INFINITE
//! @brief �I�u�W�F�N�g���V�O�i����ԂɂȂ�̂�ҋ@���鎞�Ԃł��B
#define V3D_INFINITE UINT64_MAX

//! @def V3D_WHOLE_SIZE
//! @brief ���\�[�X�̃������̏I�[�܂ł��}�b�v���邱�Ƃ�\���܂��B
#define V3D_WHOLE_SIZE (~0ULL)

//! @def V3D_QUEUE_FAMILY_IGNORED
//! @brief �L���[�t�@�~���[�̃��\�[�X�̏��L���̕ύX���Ȃ����Ƃ�\���܂��B
#define V3D_QUEUE_FAMILY_IGNORED (~0U)

//! @def V3D_ATTACHMENT_UNUSED
//! @brief �g�p����Ă��Ȃ��A�^�b�`�����g��\���܂��B
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
	V3D_ERROR_INVALID_ARGUMENT = -2, //!< �����Ȉ������n����܂����B
	V3D_ERROR_NOT_FOUND = -3, //!< ������܂���B
	V3D_ERROR_NOT_SUPPORTED = -4, //!< �T�|�[�g����Ă��܂���B
	V3D_ERROR_OUT_OF_HOST_MEMORY = -5, //!< �z�X�g���������s�����Ă��܂��B
	V3D_ERROR_OUT_OF_DEVICE_MEMORY = -6, //!< �f�o�C�X���������s�����Ă��܂��B
	V3D_ERROR_DEVICE_LOST = -7, //!< �f�o�C�X�����X�g���Ă��܂��B
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
	V3D_FORMAT_D16_UNORM = 124, //!< 1 �����A16 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_X8_D24_UNORM = 125, //!< 2 �����A32 �r�b�g�����Ȃ����K���t�H�[�}�b�g�ł��B
	V3D_FORMAT_D32_SFLOAT = 126, //!< 1 �����A32 �r�b�g�����t�����������_�t�H�[�}�b�g�ł��B
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
	//! @brief �f�o�C�X����A�N�Z�X�ł��郁�������g�p���܂��B
	V3D_MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001,
	//! @brief �z�X�g����A�N�Z�X�ł��郁�������g�p���܂��B
	V3D_MEMORY_PROPERTY_HOST_VISIBLE = 0x00000002,
	//! @brief �z�X�g���珑�����񂾓��e���t���b�V������K�v�͂���܂���B<br>
	//! ���̃t���O���w�肵���ꍇ�� IV3DResource::Map ����� IV3DResource::Unmap ���Ƀf�o�C�X�܂��̓z�X�g����̃������̉��A�s���̕ύX�������s��Ȃ����߁A���x�̌��オ�����܂�܂��B<br>
	V3D_MEMORY_PROPERTY_HOST_COHERENT = 0x00000004,
	//! @brief �������̓��e���z�X�g�ŃL���b�V�����܂��B<br>
	//! �z�X�g�œ��e���L���b�V�����邱�ƂɃ������ւ̃A�N�Z�X���x�̌��オ�����܂�܂��B
	V3D_MEMORY_PROPERTY_HOST_CACHED = 0x00000008,
	//! @brief ���������x�����Ċ��蓖�Ă���ꍇ������܂��B<br>
	//! ���̃t���O�� �J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�A�C���v�b�g�A�^�b�`�����g�ɑ΂��ėL���ł��B<br>
	//! �܂��A�C���[�W�̎g�p���@�ɂ� \link V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT \endlink ���w�肵�Ă��������B
	//! @sa IV3DDevice::CreateImage
	V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED = 0x00000010,
};

//! @enum V3D_PIPELINE_STAGE_FLAG
//! @brief �p�C�v���C���X�e�[�W�t���O
enum V3D_PIPELINE_STAGE_FLAG : V3DFlags
{
	//! @brief �ŏ��̃R�}���h���L���[�ɂ���Ď�M�����p�C�v���C���̃X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
	//! @brief �o�[�e�b�N�X�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��������p�C�v���C���̃X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
	//! @brief �o�[�e�b�N�X�V�F�[�_�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
	//! @brief �e�b�Z���[�V��������V�F�[�_�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
	//! @brief �W�I���g���V�F�[�_�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
	//! @brief �t���O�����g�V�F�[�_�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
	//! @brief �t���O�����g�V�F�[�f�B���O�O�̃f�v�X�e�X�g�ƃX�e���V���e�X�g�����s�����p�C�v���C���̃X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃f�v�X�X�e���V���A�^�b�`�����g�̃��[�h�������܂܂�܂��B
	V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
	//! @brief �t���O�����g�V�F�[�f�B���O��̃f�v�X�e�X�g�ƃX�e���V���e�X�g�����s�����p�C�v���C���̃X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃f�v�X�X�e���V���A�^�b�`�����g�̃X�g�A�������܂܂�܂��B
	V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
	//! @brief �u�����h���ꂽ�J���[���o�͂����p�C�v���C���̃X�e�[�W�ł��B<br>
	//! �T�u�p�X�̃J���[�A�^�b�`�����g�̃��[�h�A�X�g�A�A���]���u ( �}���`�T���v�� ) ���܂܂�܂��B
	V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
	//! @brief �R���s���[�g�V�F�[�_�[�X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
	//! @brief �R�s�[���������X�e�[�W�ł��B<br>
	//! �o�b�t�@�[�A�C���[�W�̃R�s�[�Ȃǂ��܂܂�܂��B
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_PIPELINE_STAGE_TRANSFER = 0x00001000,
	//! @brief ���ׂẴR�}���h�ɂ���Đ������ꂽ�I�y���[�V���������s����������p�C�v���C���̍ŏI�X�e�[�W�ł��B
	//! @note ���̃A�N�Z�X���ʂ̃L���[�܂��̓v���[���e�[�V���� ( IV3DQueue::Present ) �ōs����ꍇ�̈ڍs�Ɏg�p���܂��B
	V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,
	//! @brief �f�o�C�X�������̓ǂݏ����̃z�X�g��ł̎��s�������[���X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_HOST = 0x00004000,
	//! @brief ���ׂẴO���t�B�b�N�p�C�v���C���X�e�[�W�ł��B<br>
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
	//! @brief ���ׂẴR�}���h�̃X�e�[�W�ł��B
	V3D_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
};

//! @enum V3D_ACCESS_FLAG
//! @brief �A�N�Z�X�t���O
enum V3D_ACCESS_FLAG : V3DFlags
{
	//! @brief �C���f�b�N�X�o�b�t�@�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_INDEX \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink
	//! @sa IV3DCommandBuffer::BindIndexBufferView
	V3D_ACCESS_INDEX_READ = 0x00000002,
	//! @brief �o�[�e�b�N�X�o�b�t�@�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_VERTEX \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_INPUT \endlink
	//! @sa IV3DCommandBuffer::BindVertexBufferViews
	V3D_ACCESS_VERTEX_READ = 0x00000004,
	//! @brief ���j�t�H�[���o�b�t�@�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM  \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	//! @sa IV3DDescriptorSet::SetBufferView
	V3D_ACCESS_UNIFORM_READ = 0x00000008,
	//! @brief �C���v�b�g�A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink
	//! @sa IV3DDevice::CreateRenderPass
	V3D_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
	//! @brief �X�g���[�W�o�b�t�@�A���j�t�H�[���e�N�Z���o�b�t�@�A�X�g���[�W�e�N�Z���o�b�t�@�A�T���v�����O�C���[�W�A�X�g���[�W�C���[�W�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_SAMPLED \endlink<br>
	//! \link V3D_IMAGE_USAGE_STORAGE \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	V3D_ACCESS_SHADER_READ = 0x00000020,
	//! @brief �X�g���[�W�o�b�t�@�A�X�g���[�W�e�N�Z���o�b�t�@�A�X�g���[�W�C���[�W�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink<br>
	//! \link V3D_BUFFER_USAGE_STORAGE \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_STORAGE \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_VERTEX_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_GEOMETRY_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_FRAGMENT_SHADER \endlink<br>
	//! \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink
	V3D_ACCESS_SHADER_WRITE = 0x00000040,
	//! @brief �u�����f�B���O�A���W�b�N�A�T�u�p�X�̃��[�h�Ȃǂ̃J���[�A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink
	V3D_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
	//! @brief �����_�����O�p�X���̃T�u�p�X�̃��[�h�ォ��X�g�A�܂ł̏����Ȃǂ̃J���[�A�^�b�`�����g�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT \endlink
	V3D_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
	//! @brief �f�v�X�A�X�e���V���̑���A�T�u�p�X�̃��[�h�Ȃǂ̃f�v�X�X�e���V���A�^�b�`�����g�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
	//! @brief �����_�����O�p�X���̃T�u�p�X�̃��[�h�ォ��X�g�A�܂ł̏����Ȃǂ̃f�v�X�X�e���V���A�^�b�`�����g�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS \endlink<br>
	//! \link V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS \endlink
	V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
	//! @brief �R�s�[�R�}���h�ł̃o�b�t�@�܂��̓C���[�W�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_TRANSFER_SRC \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_TRANSFER \endlink
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_ACCESS_TRANSFER_READ = 0x00000800,
	//! @brief �R�s�[�R�}���h�ł̃o�b�t�@�܂��̓C���[�W�ւ̏������݃A�N�Z�X�ł��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! \link V3D_BUFFER_USAGE_TRANSFER_DST \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! \link V3D_IMAGE_USAGE_TRANSFER_DST \endlink<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_TRANSFER \endlink
	//! @sa IV3DCommandBuffer::CopyBuffer
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_ACCESS_TRANSFER_WRITE = 0x00001000,
	//! @brief �z�X�g����̓ǂݎ��A�N�Z�X�ł��B<br>
	//! �������̎g�p���@�� \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink ���w�肳��Ă���K�v������܂��B<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_HOST \endlink
	V3D_ACCESS_HOST_READ = 0x00002000,
	//! @brief �z�X�g����̏������݃A�N�Z�X�ł��B<br>
	//! �������̎g�p���@�� \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink ���w�肳��Ă���K�v������܂��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! \link V3D_PIPELINE_STAGE_HOST \endlink
	V3D_ACCESS_HOST_WRITE = 0x00004000,
	//! @brief ����̃��\�[�X�ւ̓ǂݎ��A�N�Z�X�ł��B<br>
	//! �X���b�v�`�F�C���̃C���[�W�Ȃǂ����̓���̃��\�[�X�Ɋ܂܂�܂��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! N/A
	V3D_ACCESS_MEMORY_READ = 0x00008000,
	//! @brief ����̃��\�[�X�ւ̏������݃A�N�Z�X�ł��B<br>
	//! �X���b�v�`�F�C���̃C���[�W�Ȃǂ����̓���̃��\�[�X�Ɋ܂܂�܂��B<br>
	//! <br>
	//! �T�|�[�g����Ă���o�b�t�@�[�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���C���[�W�̎g�p���@<br>
	//! N/A<br>
	//! <br>
	//! �T�|�[�g����Ă���X�e�[�W<br>
	//! N/A
	V3D_ACCESS_MEMORY_WRITE = 0x00010000,
};

//! @enum V3D_DEPENDENCY_FLAG
//! @brief �ˑ����t���O
enum V3D_DEPENDENCY_FLAG : V3DFlags
{
	V3D_DEPENDENCY_BY_REGION = 0x00000001, //!< �ˑ��֌W���t���[���o�b�t�@�̃��[�J���ł��邱�Ƃ�\���܂��B
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
	V3D_IMAGE_LAYOUT_UNDEFINED = 0,
	//! @brief ���ׂẴA�N�Z�X���T�|�[�g���܂��B<br>
	V3D_IMAGE_LAYOUT_GENERAL = 1,
	//! @brief �J���[�A�^�b�`�����g�A���]���u�A�^�b�`�����g�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink ���w�肳��Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT = 2,
	//! @brief �f�v�X�X�e���V���A�^�b�`�����g�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink ���w�肳��Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT = 3,
	//! @brief �V�F�[�_�[����ǂݎ�邱�Ƃ̂ł���f�v�X�X�e���V���C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink ��
	//! \link V3D_IMAGE_USAGE_SAMPLED \endlink �������� \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink �̂����ꂩ���w�肳��Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY = 4,
	//! @brief �V�F�[�_�[����̓ǂݎ���p�C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ����̓T���v�����O���邱�Ƃ̂ł���C���[�W�A�C���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B<br>
	//! �܂����̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_SAMPLED \endlink �܂��� \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink �̂����ꂩ���w�肳��Ă���K�v������܂��B
	V3D_IMAGE_LAYOUT_SHADER_READ_ONLY = 5,
	//! @brief �R�s�[�R�}���h�̃R�s�[���C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink ���w�肳��Ă���K�v������܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_LAYOUT_TRANSFER_SRC = 6,
	//! @brief �R�s�[�R�}���h�̃R�s�[��C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	//! ���̃��C�A�E�g��K�p�ł���C���[�W�̎g�p�@�� \link V3D_IMAGE_USAGE_TRANSFER_DST \endlink ���w�肳��Ă���K�v������܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_LAYOUT_TRANSFER_DST = 7,
	//! @brief �f�o�C�X�̃A�N�Z�X���T�|�[�g���܂���<br>
	//! �C���[�W�쐬���̏������C�A�E�g�Ƃ��Ďg�p���邱�Ƃ��ł��܂��B<br>
	//! ���̃��C�A�E�g�̓C���[�W�쐬��A�����Ƀ������ւ̏������݂��ł��܂��B<br>
	//! �܂��قȂ郌�C�A�E�g�ֈڍs����Ƃ��A�������̓��e�͕ێ�����܂��B
	V3D_IMAGE_LAYOUT_PREINITIALIZED = 8,
	//! @brief �X���b�v�`�F�C���̃C���[�W�ɑ΂��ėL���ȃ��C�A�E�g�ł��B<br>
	V3D_IMAGE_LAYOUT_PRESENT_SRC = 9,
};

//! @enum V3D_SAMPLE_COUNT_FLAG
//! @brief �T���v���J�E���g�̃t���O
enum V3D_SAMPLE_COUNT_FLAG
{
	V3D_SAMPLE_COUNT_1 = 0x00000001, //!< 1 �T���v��
	V3D_SAMPLE_COUNT_2 = 0x00000002, //!< 2 �T���v��
	V3D_SAMPLE_COUNT_4 = 0x00000004, //!< 4 �T���v��
	V3D_SAMPLE_COUNT_8 = 0x00000008, //!< 8 �T���v��
	V3D_SAMPLE_COUNT_16 = 0x00000010, //!< 16 �T���v��
	V3D_SAMPLE_COUNT_32 = 0x00000020, //!< 32 �T���v��
	V3D_SAMPLE_COUNT_64 = 0x00000040, //!< 64 �T���v��
};

//! @enum V3D_COMPARE_OP
//! @brief ��r�I�y���[�^�[
//! <br>
//! <table>
//! <tr><td></td><td></td></tr>
//! </table>
enum V3D_COMPARE_OP
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
enum V3D_INDEX_TYPE
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

//! @union V3DPoint3D
//! @brief �������������� x y z ���W
struct V3DPoint3D
{
	uint32_t x; //!< X ���W�ł��B
	uint32_t y; //!< Y ���W�ł��B
	uint32_t z; //!< Z ���W�ł��B
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
//! @note �[�x�� minDepth < maxDepth �ł���K�v������܂��B
struct V3DViewport
{
	V3DRectangle2D rect; //!< �r���[�|�[�g�̗̈�ł��B
	float minDepth; //!< �ŏ��[�x�ł��B ( 0.0f�`1.0f )
	float maxDepth; //!< �ő�[�x�ł��B ( 0.0f�`1.0f )
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
	float depth; //!< �[�x�ł��B ( 0.0f�`1.0f )
	uint32_t stencil; //!< �X�e���V���ł��B
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
	//! @brief �V�O�i����Ԃ��ǂ������m�F���܂�
	//! @return �V�O�i����Ԃ̏ꍇ�� true ��Ԃ��܂�
	virtual bool IsSignaled() const = 0;

	//! @brief �t�F���X���V�O�i����Ԃɂ��܂�
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset() = 0;
	//! @brief �t�F���X���V�O�i����ԂɂȂ�܂őҋ@���܂�
	//! @param[in] timeOut �^�C���A�E�g���� ( �i�m�b�P�� )
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
// �N�G���v�[��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUERY_TYPE
//! @brief �N�G���̃^�C�v
enum V3D_QUERY_TYPE
{
	V3D_QUERY_TYPE_OCCLUSION = 0, //!< �I�N���[�W�����N�G���ł��B
	V3D_QUERY_TYPE_PIPELINE_STATISTICS = 1, //!< �p�C�v���C���̓��v�N�G���ł��B
	V3D_QUERY_TYPE_TIMESTAMP = 2, //!< �^�C���X�^���v�N�G���ł��B
};

//! @enum V3D_QUERY_RESULT_FLAG
//! @brief �N�G���̌��ʃt���O
enum V3D_QUERY_RESULT_FLAG
{
	V3D_QUERY_RESULT_WAIT = 0x00000001, //!< �w�肵�����ׂẴN�G���̌��ʂ��擾�ł���܂őҋ@���܂��B
	V3D_QUERY_RESULT_WITH_AVAILABILITY = 0x00000002, //!< ���ʂ��o�Ă���N�G�������邩�ǂ������m�F�ł���悤�ɂ��܂��B
	V3D_QUERY_RESULT_PARTIAL = 0x00000004, //!< �w�肵�����ׂăN�G���łȂ��Ă����ʂ��擾�ł���悤�ɂ��܂��B
};

//! @enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
//! @brief �N�G���̃p�C�v���C���̓��v�^�C�v
enum V3D_QUERY_PIPELINE_STATISTIC_TYPE
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
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DQueryPoolDesc
//! @brief �N�G���̋L�q
struct V3DQueryPoolDesc
{
	V3D_QUERY_TYPE queryType; //!< �N�G���̃^�C�v
	uint32_t queryCount; //!< �v�[���ɂ���ĊǗ������N�G���̐��ł�
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
	//! @param[in] queryResultFlags �N�G���̌��ʂ̎擾���@��\�� \link V3D_QUERY_RESULT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK �N�G���̌��ʂ��擾���܂����B
	//! @retval V3D_NOT_READY �N�G���̌��ʂ͂܂��擾�ł��Ă��܂���B
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	//! @note
	//! �L���[�����s ( IV3DCommandBuffer::ExecuteCommandBuffers ) ������Ɏg�p���Ă��������B<br>
	//! �Ԃ�l�� V3D_OK �łȂ��Ă��N�G���̌��ʂ��擾���悤�Ƃ������ IV3DCommandBuffer::ResetQueryPool �ŃN�G���v�[�������Z�b�g���Ă��������B<br>
	//! <br>
	//! �N�G���̃^�C�v�� \link V3D_QUERY_TYPE_OCCLUSION \endlink �������� \link V3D_QUERY_TYPE_TIMESTAMP \endlink �ł������ꍇ�� queryResultCount �̒l�� queryCount �ɂȂ�܂��B<br>
	//! �N�G���̃^�C�v�� \link V3D_QUERY_TYPE_PIPELINE_STATISTICS \endlink �ł������ꍇ�� queryResultCount �̒l�� queryCount * 11 �ɂȂ�A���ꂼ��� 0 �` 10 �v�f�̒l�̈Ӗ��� \link V3D_QUERY_PIPELINE_STATISTIC_TYPE \endlink �񋓌^���Q�Ƃ��Ă��������B<br>
	//! �N�G���̃^�C�v�� \link V3D_QUERY_TYPE_TIMESTAMP \endlink �ł������ꍇ�� pQueryResults �ɏ������܂�鐔�l�̒P�ʂ̓i�m�b�ɂȂ�܂��B<br>
	//! <br>
	//! flags �� \link V3D_QUERY_RESULT_WAIT \endlink ���w�肵���ꍇ�́A�N�G���̌��ʂ��擾�ł���܂őҋ@���܂��B<br>
	//! flags �� \link V3D_QUERY_RESULT_PARTIAL \endlink ���w�肵���ꍇ�́A�w�肵�����ׂẴN�G���łȂ��A�����I�ȃN�G���̓r���̌��ʂ��擾�ł���悤�ɂȂ�܂��B<br>
	//! �������A�N�G���̃^�C�v�� \link V3D_QUERY_TYPE_TIMESTAMP \endlink �ł������ꍇ�A���̃t���O���w�肷�邱�Ƃ͂ł��܂���B<br>
	//! flags �� \link V3D_QUERY_RESULT_WITH_AVAILABILITY \endlink ���w�肵���ꍇ�́AqueryResultCount �̒l�� queryCount + 1 ����K�v�Ƃ��A
	//! �Ō�̗v�f ( pQueryResults[queryResultCount - 1] ) �ɃN�G���̌��ʂ��擾�ł����ꍇ�� 1 �ȏ�̒l�A�擾�ł��Ȃ������ꍇ�� 0 ���������܂�܂��B
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
	//! @brief �������̃v���p�e�B�t���O��\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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

	//! @brief ���\�[�X�������̃}�b�v���J�n���邱�Ƃ�ʒm���܂��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! ���̃��\�b�h�̓������S�̂̃A�N�Z�X��L���ȏ�Ԃɂ��܂��B<br>
	//! ��x�ɑ����̃��\�[�X�����̃��������A�N�Z�X����� ( IV3DResource::Map ����� IV3DResource::Unmap ) �̃p�t�H�[�}���X�����コ���邱�Ƃ��ł��܂��B
	virtual V3D_RESULT BeginMap() = 0;
	//! @brief ���\�[�X�������̃}�b�v���I�����邱�Ƃ�ʒm���܂��B
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
// ���\�[�X
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_RESOURCE_TYPE
//! @brief ���\�[�X�^�C�v
enum V3D_RESOURCE_TYPE
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

	//! @brief �������̃v���p�e�B�t���O��\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags memoryPropertyFlags;
	//! @brief �������^�C�v�̃r�b�g�}�X�N�ł��B
	//! @note �e�r�b�g�� IV3DAdapter::GetMemoryTypeDesc �̑������ł��� memoryType ��\���܂��B
	uint32_t memoryTypeBits;
	//! @brief �������̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
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
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) = 0;

	//! @brief ���\�[�X�̎w�肳�ꂽ�������̃|�C���^�L���ɂ��A�擾���܂��B
	//! @param[in] offset �L���ɂ��郁�����̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] size �L���ɂ��郁�����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[out] ppMemory �擾�����������̃|�C���^��n�� void �|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note size �� V3D_WHOLE_SIZE ���w�肷�邱�Ƃ� offset ���烁�����̏I�[�܂ł��}�b�v���܂��B
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) = 0;
	//! @brief ���\�[�X�̎w�肳�ꂽ�������̃|�C���^�𖳌��ɂ��܂��B
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
// �o�b�t�@�[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_BUFFER_USAGE
//! @brief �o�b�t�@�[�̎g�p���@�̃t���O
enum V3D_BUFFER_USAGE : V3DFlags
{
	//! @brief �R�s�[���o�b�t�@�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyBuffer
	V3D_BUFFER_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief �R�s�[��o�b�t�@�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyBuffer
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

//! @struct V3DBufferSubresourceDesc
//! @brief �o�b�t�@�[�̃T�u���\�[�X�̋L�q
struct V3DBufferSubresourceDesc
{
	//! @brief �T�u���\�[�X�̎g�p�@��\�� \link V3D_BUFFER_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags usageFlags;
	//! @brief �T�u���\�[�X�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B<br>
	//! �܂� usageFlags �� \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink �܂��� \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink ���w�肳��Ă����ꍇ�́A
	//! �T�u���\�[�X�̃T�C�Y�� IV3DDevice::CreateBufferView �� format �̗v�f�̃T�C�Y�Ŋ������l�� V3DDeviceCaps::maxTexelBufferElements �ȉ��ł���K�v������܂��B
	uint64_t size;
};

//! @struct V3DBufferSubresourceLayout
//! @brief �o�b�t�@�[�̃T�u���\�[�X�̃��C�A�E�g
struct V3DBufferSubresourceLayout
{
	V3DFlags usageFlags; //!< �T�u���\�[�X�̎g�p�@��\�� \link V3D_BUFFER_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	uint64_t offset; //!< �T�u���\�[�X�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t size; //!< �T�u���\�[�X�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
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
	//! @brief �o�b�t�@�̎g�p���@���擾���܂��B
	//! @return �o�b�t�@�̎g�p���@��\�� \link V3D_BUFFER_USAGE \endlink �񋓒萔�̑g�ݍ��킹��Ԃ��܂��B
	virtual V3DFlags GetUsageFlags() const = 0;

	//! @brief �T�u���\�[�X�̐����擾���܂��B
	//! @return �T�u���\�[�X�̐���Ԃ��܂��B
	//! @note �Ԃ����l�� �o�b�t�@�쐬���ɓn���� V3DBufferSubresourceDesc �\���̂̔z��̗v�f�̐��ɂȂ�܂��B
	virtual uint32_t GetSubresourceCount() const = 0;
	//! @brief �o�b�t�@�[�̃T�u���\�[�X�̃��C�A�E�g���擾���܂��B
	//! @param[in] subresource �o�b�t�@�쐬���ɓn���� V3DBufferSubresourceDesc �\���̂̔z��̃C���f�b�N�X�ł��B
	virtual const V3DBufferSubresourceLayout& GetSubresourceLayout(uint32_t subresource) const = 0;

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
enum V3D_IMAGE_TYPE
{
	V3D_IMAGE_TYPE_1D = 0, //!< 1D ( width ) �C���[�W�ł��B
	V3D_IMAGE_TYPE_2D = 1, //!< 2D ( width height ) �C���[�W�ł��B
	V3D_IMAGE_TYPE_3D = 2, //!< 3D ( width height depth )�C���[�W�ł��B
};

//! @enum V3D_IMAGE_TILING
//! @brief �C���[�W�̃^�C�����O
enum V3D_IMAGE_TILING
{
	//! @brief �œK�ȃ^�C�����O���w�肵�܂��B<br>
	//! �e�N�Z���̔z�u�͎����Ɉˑ����邱�ƂɂȂ邽�߁A�T�u���\�[�X�ɃA�N�Z�X���邱�Ƃ͂ł��܂���B
	V3D_IMAGE_TILING_OPTIMAL = 0,
	//! @brief ���`�^�C�����O���g�p���܂��B<br>
	//! �e�N�Z���͒�߂�ꂽ�����Ŕz�u����邽�߁A�T�u���[�X�ւ̃A�N�Z�X���\�ɂ��܂��B
	//! @sa IV3DImage::GetSubresourceLayout
	V3D_IMAGE_TILING_LINEAR = 1,
};

//! @enum V3D_IMAGE_USAGE
//! @brief �C���[�W�̎g�p���@��\���t���O
enum V3D_IMAGE_USAGE : V3DFlags
{
	//! @brief �R�s�[���C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_USAGE_TRANSFER_SRC = 0x00000001,
	//! @brief �R�s�[��C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DCommandBuffer::ClearImage
	//! @sa IV3DCommandBuffer::CopyImage
	V3D_IMAGE_USAGE_TRANSFER_DST = 0x00000002,
	//! @brief �V�F�[�_�[����T���v�����O���邱�Ƃ��ł���C���[�W�Ƃ��Ďg�p���܂��B
	//! @sa IV3DDescriptorSet::SetImageView
	//! @sa IV3DDescriptorSet::SetImageViewAndSampler
	V3D_IMAGE_USAGE_SAMPLED = 0x00000004,
	//! @brief �L���[�u�C���[�W�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_CUBE_COMPATIBLE = 0x00000008,
	//! @brief �V�F�[�_�[���珑�����ނ��Ƃ̂ł���X�g���[�W�C���[�W�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_USAGE_STORAGE = 0x00000010,
	//! @brief �t���[���o�b�t�@�̃J���[�A�^�b�`�����g�Ƃ��Ďg�p���܂��B
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_COLOR_ATTACHMENT = 0x00000020,
	//! @brief �t���[���o�b�t�@�̃f�v�X�X�e���V���Ƃ��Ďg�p���܂��B
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x00000040,
	//! @brief �C���[�W�̃������v���p�e�B�� \link V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED \endlink ���w�肳��Ă��邱�Ƃ�\���܂��B<br>
	//! �܂��ȉ��̎g�p�@��p���č쐬���ꂽ�C���[�W�ɍœK�ł��B<br>
	//! \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink<br>
	//! \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink<br>
	//! \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink<br>
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_TRANSIENT_ATTACHMENT = 0x0000080,
	//! @brief �t���[���o�b�t�@�̃C���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p���܂��B<br>
	//! ���̃C���[�W�̓t���O�����g�V�F�[�_�[�݂̂��ǂݎ�邱�Ƃ��ł��܂��B
	//! @sa IV3DDevice::CreateFrameBuffer
	V3D_IMAGE_USAGE_INPUT_ATTACHMENT = 0x00000100,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DImageDesc
//! @brief �C���[�W�̋L�q
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
	V3DFlags usageFlags; //!< �C���[�W�̎g�p���@��\�� \link V3D_IMAGE_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DImageSubresourceLayout
//! @brief �C���[�W�̃T�u���\�[�X���C�A�E�g
struct V3DImageSubresourceLayout
{
	uint64_t offset; //!< �������̃I�t�Z�b�g�ł��B ( �o�C�g�P�� )
	uint64_t size; //!< �������̃T�C�Y�ł��B ( �o�C�g�P�� )
	uint64_t rowPitch; //!< �s�̃s�b�`�ł��B ( �o�C�g�P�� )
	uint64_t layerPitch; //!< ���C���[�̃s�b�`�ł��B ( �o�C�g�P�� )
	uint64_t depthPitch; //!< �[���̃s�b�`�ł��B ( �o�C�g�P�� )
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
	//! @note
	//! �T�u���\�[�X�̃��C�A�E�g���擾�ł���悤�ɂ��邽�߂ɂ́A�C���[�W�̃^�C�����O�� \link V3D_IMAGE_TILING_LINEAR \endlink ���w�肳��Ă���K�v������܂��B
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
struct V3DBufferViewDesc
{
	V3DFlags usageFlags; //!< �o�b�t�@�[�r���[�̎g�p���@��\�� \link V3D_BUFFER_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	uint64_t offset; //!< �A�N�Z�X����o�b�t�@�̃������I�t�Z�b�g�ł��B ( �o�C�g�P�� )
	uint64_t size; //!< �A�N�Z�X����o�b�t�@�̃������T�C�Y�ł��B ( �o�C�g�P�� )
	V3D_FORMAT format; //!< �t�H�[�}�b�g�ł��B
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
enum V3D_IMAGE_VIEW_TYPE
{
	V3D_IMAGE_VIEW_TYPE_1D = 0, //!< 1D �C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_2D = 1, //!< 2D �C���[�W�r���[�Ƃ��Ďg�p���܂��B
	V3D_IMAGE_VIEW_TYPE_3D = 2, //!< 3D �C���[�W�r���[�Ƃ��Ďg�p���܂��B
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
enum V3D_FILTER
{
	//! @brief �ł��߂����W�̃e�N�Z�����g�p���܂��B
	V3D_FILTER_NEAREST = 0,
	//! @brief �C���[�W�̎������ɉ������e�N�Z���̉��d���ς��v�Z���܂��B<br>
	//! <table>
	//! <tr><td>1D�C���[�W</td><td>2x1</td></tr>
	//! <tr><td>2D�C���[�W<br>�L���[�u�C���[�W</td><td>2x2</td></tr>
	//! <tr><td>3D�C���[�W</td><td>2x2x2</td></tr>
	//! </table>
	V3D_FILTER_LINEAR = 1,
};

//! @enum V3D_MIPMAP_MODE
//! @brief �~�b�v�}�b�v���[�h
enum V3D_MIPMAP_MODE
{
	//! @brief �ł��߂����W�̃e�N�Z�����g�p���܂��B
	V3D_MIPMAP_MODE_NEAREST = 0,
	//! @brief �C���[�W�̎������ɉ������e�N�Z���̉��d���ς��v�Z���܂��B
	V3D_MIPMAP_MODE_LINEAR = 1,
};

//! @enum V3D_ADDRESS_MODE
//! @brief �A�h���X���[�h
enum V3D_ADDRESS_MODE
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
//! @brief �T���v���̋L�q
//! @note
//! �f�t�H���g�l<br>
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
	V3D_FILTER magFilter; //!< �g��t�B���^�ł��B
	V3D_FILTER minFilter; //!< �k���t�B���^�ł��B
	V3D_MIPMAP_MODE mipmapMode; //!< �~�b�v�}�b�v���[�h
	V3D_ADDRESS_MODE addressModeU; //!< U ���W�̃A�h���b�V���O���[�h�ł��B
	V3D_ADDRESS_MODE addressModeV; //!< V ���W�̃A�h���b�V���O���[�h�ł��B
	V3D_ADDRESS_MODE addressModeW; //!< W ���W�̃A�h���b�V���O���[�h�ł��B
	float mipLodBias; //!< �v�Z���ꂽ�~�b�v�}�b�v���x������̃I�t�Z�b�g�ł��B
	bool anisotropyEnable; //!< �ٕ����t�B���^�����O��L���ɂ��邩�ǂ������w�肵�܂��B
	float maxAnisotropy; //!< �ٕ����l�N�����v�l�ł��B
	bool compareEnable; //!< ��r�I�y�[���[�V�����ł��� compareOp ��L���ɂ��邩�ǂ������w�肵�܂��B
	V3D_COMPARE_OP compareOp; //!< ��r�I�y���[�^�[�ł��B
	float minLod; //!< �v�Z���ꂽ�~�b�v���x�����N�����v����ŏ��l�ł��B
	float maxLod; //!< �v�Z���ꂽ�~�b�v���x�����N�����v����ő�l�ł��B
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
//! @brief �����_�����O�p�X�̃��[�h�I�y���[�^�[
enum V3D_ATTACHMENT_LOAD_OP
{
	//! @brief �����_�����O�p�X�̊J�n���ɃA�^�b�`�����g�̈ȑO�̓��e���ێ�����Ă���K�v���Ȃ����Ƃ�\���܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink �������� \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_UNDEFINED = 0,
	//! @brief �����_�����O�p�X�̊J�n���ɃA�^�b�`�����g�̈ȑO�̓��e��ǂݍ��݂܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A\link V3D_ACCESS_COLOR_ATTACHMENT_READ \endlink �������� \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ \endlink ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_LOAD = 1,
	//! @brief �����_�����O�p�X�̊J�n���ɃA�^�b�`�����g���N���A���܂��B
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink �������� \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_LOAD_OP_CLEAR = 2,
};

//! @enum V3D_ATTACHMENT_STORE_OP
//! @brief �����_�����O�p�X�̃X�g�A�I�y���[�^�[
enum V3D_ATTACHMENT_STORE_OP
{
	//! @brief �����_�����O�p�X�̏I�����ɃA�^�b�`�����g�̓��e���ۑ����ꂸ�A�j�������\�������邱�Ƃ�\���܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink �������� \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_STORE_OP_UNDEFINED = 0,
	//! @brief �����_�����O�p�X�̏I�����ɃA�^�b�`�����g�̓��e��ۑ����܂��B<br>
	//! �܂��A�^�b�`�����g�̃A�N�Z�X�ɂ́A\link V3D_ACCESS_COLOR_ATTACHMENT_WRITE \endlink �������� \link V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE \endlink ���܂܂�Ă���K�v������܂��B
	V3D_ATTACHMENT_STORE_OP_STORE = 1,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DAttachmentDesc
//! @brief �A�^�b�`�����g�̋L�q
struct V3DAttachmentDesc
{
	//! @brief �C���[�W�̃t�H�[�}�b�g�ł��B<br>
	//! @sa V3DImageDesc::format
	V3D_FORMAT format;
	//! @brief �C���[�W�̃T���v�����ł��B<br>
	//! @sa V3DImageDesc::samples
	V3D_SAMPLE_COUNT_FLAG samples;
	//! @brief �����_�����O�p�X�̃��[�h�I�y���[�^�[�ł��B
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_ATTACHMENT_LOAD_OP loadOp;
	//! @brief �����_�����O�p�X�̃X�g�A�I�y���[�^�[�ł��B
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_ATTACHMENT_STORE_OP storeOp;
	//! @brief �����_�����O�p�X�̃X�e���V���̃��[�h�I�y���[�^�[�ł��B
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_ATTACHMENT_LOAD_OP stencilLoadOp;
	//! @brief �����_�����O�p�X�̃X�e���V���̃X�g�A�I�y���[�^�[�ł��B
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_ATTACHMENT_STORE_OP stencilStoreOp;
	//! @brief �����_�����O�p�X�J�n���̃��C�A�E�g�ł��B<br>
	//! �����_�����O�p�X�J�n�O�ɂ��̃��C�A�E�g�ֈڍs���Ă����K�v������܂��B
	//! @sa IV3DCommandBuffer::BeginRenderPass
	V3D_IMAGE_LAYOUT initialLayout;
	//! @brief �����_�����O�p�X�I�����̃��C�A�E�g�ł��B
	//! @sa IV3DCommandBuffer::EndRenderPass
	V3D_IMAGE_LAYOUT finalLayout;
	//! @brief �����_�����O�p�X�J�n���̃A�^�b�`�����g�̃N���A�l�ł��B<br>
	//! ���̒l�� loadOp �܂��� stencilLoadOp �� \link V3D_ATTACHMENT_LOAD_OP_CLEAR \endlink �̂Ƃ��ɗL���ł��B
	V3DClearValue clearValue;
};

//! @struct V3DAttachmentReference
//! @brief �A�^�b�`�����g�̎Q��
struct V3DAttachmentReference
{
	uint32_t attachment; //!< IV3DDevice::CreateRenderPass �Ŏw�肵�� V3DAttachmentDesc �\���̂̔z��̃C���f�b�N�X�ł��B
	V3D_IMAGE_LAYOUT layout; //!< �T�u�p�X�J�n���̃A�^�b�`�����g�̃��C�A�E�g�ł��B
};

//! @struct V3DSubpassDesc
//! @brief �T�u�p�X�̋L�q
struct V3DSubpassDesc
{
	//! @brief ���͂Ƃ��Ďg�p����C���v�b�g�A�^�b�`�����g�̐��ł��B
	uint32_t inputAttachmentCount;
	//! @brief ���͂Ƃ��Ďg�p����C���v�b�g�A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! �z��̗v�f�̐��� inputAttachmentCount �̒l�Ɠ����ł���K�v������܂��B
	//! @note
	//! �����Ŏw�肷��A�^�b�`�����g V3DAttachmentReference::attachment �̓C���[�W�̎g�p���@�� \link V3D_IMAGE_USAGE_INPUT_ATTACHMENT \endlink ���w�肳��Ă���K�v������܂��B
	const V3DAttachmentReference* pInputAttachments;

	//! @brief �o�͐�̃J���[�A�^�b�`�����g�̐��ł��B
	uint32_t colorAttachmentCount;
	//! @brief �o�͐�̃J���[�A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! �z��̗v�f�̐��� colorAttachmentCount �̒l�Ɠ����ł���K�v������܂��B<br>
	//! @note
	//! �����Ŏw�肷��A�^�b�`�����g V3DAttachmentReference::attachment �̓C���[�W�̎g�p���@ V3DImageDesc::usageFlags �� \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink ���w�肳��Ă���K�v������܂��B
	const V3DAttachmentReference* pColorAttachments;
	//! @brief �o�͐�̃J���[�A�^�b�`�����g ( pColorAttachments ) ���}���`�T���v�����O�������ʂ������_�����O����A�^�b�`�����g�̎Q�Ɣz��ł��B<br>
	//! �z��̗v�f�̐��� colorAttachmentCount �̒l�Ɠ����ł���K�v������܂��B
	//! @note
	//! �����Ŏw�肷��A�^�b�`�����g V3DAttachmentReference::attachment �̓C���[�W�̎g�p���@ V3DImageDesc::usageFlags �� \link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink ���w�肳��Ă���K�v������A
	//! �T���v���� ( V3DImageDesc::samples ) �� \link V3D_SAMPLE_COUNT_1 \endlink �ł���K�v������܂��B<br>
	//! �}���`�T���v�����O�C���[�W�̕ϊ���̃A�^�b�`�����g�Ƃ��Ďg�p���Ȃ��ꍇ�� V3DAttachmentReference::attachment �� \link V3D_ATTACHMENT_UNUSED \endlink ���w�肵�Ă��������B<br>
	const V3DAttachmentReference* pResolveAttachments;

	//! @brief �o�͐�̃f�v�X�X�e���V���A�^�b�`�����g�̎Q�Ƃł��B
	//! @note
	//! �����Ŏw�肷��A�^�b�`�����g V3DAttachmentReference::attachment �̓C���[�W�̎g�p���@�� \link V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT \endlink ���w�肳��Ă���K�v������A<br>
	//! �T���v���� V3DImageDesc::samples �̓J���[�A�^�b�`�����g�Ɠ����ł���K�v������܂��B<br>
	//! �܂��A�f�v�X�X�e���V���A�^�b�`�����g�����݂��Ȃ��ꍇ�� nullptr ���w�肵�Ă��������B<br>
	const V3DAttachmentReference* pDepthStencilAttachment;

	//! @brief ���̃T�u�p�X�Ŏg�p����ĂȂ��A���e��ێ��������A�^�b�`�����g�̐��ł��B
	uint32_t preserveAttachmentCount;
	//! @brief ���̃T�u�p�X�Ŏg�p����ĂȂ��A���e��ێ��������A�^�b�`�����g�̃C���f�b�N�X�z��ł��B<br>
	//! �z��̗v�f�̐��� preserveAttachmentCount �̒l�Ɠ����ł���K�v������܂��B
	const uint32_t* pPreserveAttachments;
};

//! @struct V3DSubpassDependencyDesc
//! @brief �T�u�p�X�̈ˑ����̋L�q
struct V3DSubpassDependencyDesc
{
	uint32_t srcSubpass; //!< ���݂̃T�u�p�X�̃C���f�b�N�X�ł��B
	uint32_t dstSubpass; //!< ���̃T�u�p�X�̃C���f�b�N�X�ł��B
	V3DFlags srcStageMask; //!< ���݂̃p�C�v���C���X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< ���̃p�C�v���C���X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags srcAccessMask; //!< ���݂̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< ���̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ���\�� \link V3D_DEPENDENCY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
// �t���[���o�b�t�@
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_interface_group
//! @{

//! @interface IV3DFrameBuffer
//! @brief �t���[���o�b�t�@�C���^�[�t�F�[�X
//! @sa IV3DDevice::CreateFrameBuffer
class V3D_DLL_API IV3DFrameBuffer : public IV3DDeviceChild
{
public:
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

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDescriptorDesc
//! @brief �f�X�N���v�^�̋L�q
struct V3DDescriptorDesc
{
	uint32_t binding; //!< �o�C���f�B���O�ł��B
	V3D_DESCRIPTOR_TYPE type; //!< �f�X�N���v�^�̃^�C�v�ł��B
	V3DFlags stageFlags; //!< �o�C���h����V�F�[�_�[�X�e�[�W��\�� \link V3D_SHADER_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DDescriptorSetLayoutStatistics
//! @brief �f�X�N���v�^�Z�b�g���C�A�E�g�̓��v
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
	//! @brief �f�X�N���v�^�Z�b�g�̃��C�A�E�g���擾���܂��B
	//! @param[out] ppLayout �擾�����f�X�N���v�^�Z�b�g�̃��C�A�E�g��n�� IV3DDescriptorSetLayout �C���^�t�F�[�X�̃|�C���^�̃A�h���X�B
	virtual void GetLayout(IV3DDescriptorSetLayout** ppLayout) = 0;

	//! @brief �C���[�W�r���[���擾���܂��B
	//! @param[in] binding �擾����C���[�W�̃o�C���f�B���O�ł��B
	//! @param[out] ppImageView �擾�����C���[�W�r���[��n�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageView(uint32_t binding, IV3DImageView** ppImageView) = 0;
	//! @brief �f�X�N���v�^�Z�b�g���o�C���h���ꂽ�ۂ̃C���[�W���C�A�E�g���擾���܂��B
	//! @param[in] binding �擾����C���[�W���C�A�E�g�̃o�C���f�B���O�ł��B
	//! @param[out] pImageLayout �擾�����C���[�W���C�A�E�g���i�[���� \link V3D_IMAGE_LAYOUT \endlink �񋓒萔�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetImageLayout(uint32_t binding, V3D_IMAGE_LAYOUT* pImageLayout) = 0;
	//! @brief �C���[�W�r���[��ݒ肵�܂��B<br>
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @note
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink<br> 
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView) = 0;
	//! @brief �C���[�W�r���[��ݒ肵�܂��B<br>
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @param[in] imageLayout �f�X�N���v�^�Z�b�g���o�C���h�����ۂɈڍs����C���[�W�̃��C�A�E�g�ł��B<br>
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @note
	//! �ݒ肵���C���[�W�͑���I����Ƀo�C���h�O�̃C���[�W���C�A�E�g�ɖ߂�܂��B
	//! <br>
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink<br>
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	virtual V3D_RESULT SetImageView(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout) = 0;

	//! @brief �T���v���[���擾���܂��B
	//! @param[in] binding �擾����T���v���[�̃o�C���f�B���O�ł��B
	//! @param[out] ppSampler �擾�����T���v���[��n�� IV3DSampler �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetSampler(uint32_t binding, IV3DSampler** ppSampler) = 0;
	//! @brief �T���v���[��ݒ肵�܂��B<br>
	//! @param[in] binding �T���v���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pSampler �ݒ肷��T���v���[�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! <br>
	//! �T�|�[�g���Ă���f�X�N���v�^�^�C�v<br><br>
	//! \link V3D_DESCRIPTOR_TYPE_SAMPLER \endlink �A 
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink �̂����ꂩ�ł���K�v������܂��B
	virtual V3D_RESULT SetSampler(uint32_t binding, IV3DSampler* pSampler) = 0;

	//! @brief �C���[�W�r���[�ƃT�����v�[��ݒ肵�܂��B<br>
	//! ���̃o�C���f�B���O�̃f�X�N���v�^�� 
	//! \link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink �ł���K�v������܂��B
	//! @param[in] binding �C���[�W�r���[��ݒ肷��o�C���f�B���O�ł��B
	//! @param[in] pImageView �ݒ肷��C���[�W�r���[�̃|�C���^�ł��B
	//! @param[in] imageLayout �f�X�N���v�^�Z�b�g���o�C���h���ꂽ�ۂ̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSampler �ݒ肷��T���v���[�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @sa V3DDescriptorDesc
	virtual V3D_RESULT SetImageViewAndSampler(uint32_t binding, IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, IV3DSampler* pSampler) = 0;

	//! @brief �o�b�t�@�[�r���[���擾���܂�
	//! @param[in] binding �擾����o�b�t�@�[�r���[�̃o�C���f�B���O
	//! @param[out] ppBufferView �擾�����o�b�t�@�[�r���[��n�� IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT GetBufferView(uint32_t binding, IV3DBufferView** ppBufferView) = 0;
	//! @brief �o�b�t�@�[�r���[��ݒ肵�܂�
	//! @param[in] binding �o�b�t�@�[�r���[��ݒ肷��o�C���f�B���O
	//! @param[in] pBufferView �ݒ肷��o�b�t�@�[�r���[�̃|�C���^
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	virtual V3D_RESULT SetBufferView(uint32_t binding, IV3DBufferView* pBufferView) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���X�V���܂�
	//! @note �f�X�N���v�^�Z�b�g�Ɋe�탊�\�[�X��ݒ肵���ۂɎ��s���Ă��������B
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
//! @brief �萔�̋L�q
//! @sa IV3DCommandBuffer::PushConstant
struct V3DConstantDesc
{
	V3DFlags shaderStageFlags; //!< �萔���v�b�V������V�F�[�_�[�X�e�[�W��\�� \link V3D_SHADER_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
	//! @brief �萔�͈̔͂��擾���܂��B
	//! @param[in] constantIndex �萔�̃C���f�b�N�X�ł��B
	//! @return �萔�͈̔͂�\�� V3DConstantDesc �\���̂̃A�h���X��Ԃ��܂��B
	virtual const V3DConstantDesc& GetConstantDesc(uint32_t constantIndex) const = 0;

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
enum V3D_PIPELINE_TYPE
{
	V3D_PIPELINE_TYPE_GRAPHICS = 0, //!< �O���t�B�b�N�X�p�C�v���C���Ƃ��Ďg�p���܂��B
	V3D_PIPELINE_TYPE_COMPUTE = 1, //!< �R���s���[�g�p�C�v���C���Ƃ��Ďg�p���܂��B
};

//! @enum V3D_PRIMITIVE_TOPOLOGY
//! @brief �v���~�e�B�u�g�|���W
enum V3D_PRIMITIVE_TOPOLOGY
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
enum V3D_POLYGON_MODE
{
	V3D_POLYGON_MODE_FILL = 0, //!< �|���S����h��Ԃ��ĕ`�悵�܂��B
	V3D_POLYGON_MODE_LINE = 1, //!< �|���S�����G�b�W��`�悵�܂��B
	V3D_POLYGON_MODE_POINT = 2, //!< �|���S���̒��_��`�悵�܂��B
};

//! @enum V3D_CULL_MODE
//! @brief �J�����O���[�h
enum V3D_CULL_MODE
{
	V3D_CULL_MODE_NONE = 0, //!< �J�����O�����܂���B
	V3D_CULL_MODE_FRONT = 1, //!< �O�ʂ��J�����O���܂��B
	V3D_CULL_MODE_BACK = 2, //!< ��ʂ��J�����O���܂��B
};

//! @enum V3D_STENCIL_OP
//! @brief �X�e���V���I�y���[�^�[
enum V3D_STENCIL_OP
{
	V3D_STENCIL_OP_KEEP = 0, //!< ���݂̒l��ێ����܂��B
	V3D_STENCIL_OP_ZERO = 1, //!< �l��0�ɐݒ肵�܂��B
	V3D_STENCIL_OP_REPLACE = 2, //!< �l�� V3DPipelineStencilOpDesc::reference �ɐݒ肵�܂��B
	V3D_STENCIL_OP_INCREMENT_AND_CLAMP = 3, //!< ���݂̒l���C���N�������g���A�ő�l�ɃN�����v���܂��B
	V3D_STENCIL_OP_DECREMENT_AND_CLAMP = 4, //!< ���݂̒l���f�N�������g���� 0 �ɃN�����v���܂��B
	V3D_STENCIL_OP_INVERT = 5, //!< ���݂̒l�̃r�b�g���]���܂��B
	V3D_STENCIL_OP_INCREMENT_AND_WRAP = 6, //!< ���݂̒l���C���N�������g���A�ő�l�𒴂����ꍇ�� 0 �Ƀ��b�v���܂��B
	V3D_STENCIL_OP_DECREMENT_AND_WRAP = 7, //!< ���݂̒l���f�N�������g���A�l�� 0 ��菬�����Ȃ�ƍő�l�Ƀ��b�v���܂��B
};

//! @enum V3D_BLEND_FACTOR
//! @brief �u�����h�W��<br>
//! <table>
//! <tr><td>Rs0 Gs0 Bs0 As0</td><td>�u�����h�����J���[�A�^�b�`�����g�̑�P�̃\�[�X�J���[</td></tr>
//! <tr><td>Rs1 Gs1 Bs1 As1</td><td>�u�����h�����J���[�A�^�b�`�����g�̑�Q�̃\�[�X�J���[</td></tr>
//! <tr><td>Rd Gd Bd Ad</td><td></td>�o�͐�̃J���[</tr>
//! <tr><td>Rc Gc Bc Ac</td><td></td>�R�}���h IV3DCommandBuffer::SetBlendConstants �Őݒ肷��u�����h�萔</tr>
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
//! @brief �u�����h�I�y���[�^�[<br>
//! <table>
//! <tr><td>Cs</td><td>���͌��̃C���v�b�g�A�^�b�`�����g�Ȃǂ̃J���[</td></tr>
//! <tr><td>Cd</td><td>�o�͐�ɂȂ�J���[�A�^�b�`�����g�̃J���[</td></tr>
//! <tr><td>Fs</td><td>���͌��̃u�����h�W��</td></tr>
//! <tr><td>Fd</td><td>�o�͐�̃u�����h�W��</td></tr>
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
//! @brief �_���I�y���[�^�[<br>
//! <table>
//! <tr><td>s</td><td>�t���O�����g�ɂ���ďo�͂���� RGBA �����ł��B</td></tr>
//! <tr><td>d</td><td>�J���[�A�^�b�`�����g�ɂ���ē��͂���� RGBA �����ł��B</td></tr>
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
//! @brief �p�C�v���C���̃V�F�[�_�[�萔�̋L�q
struct V3DPipelineShaderConstantDesc
{
	//! @brief �萔�̎��ʎq�𐮐��Ŏw�肵�܂��B<br>
	//! layout(constant_id = [id]) const
	uint32_t id;
	//! @brief �萔�f�[�^��̒萔�̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
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

	uint32_t constantCount; //!< �萔�̐��ł��B
	V3DPipelineShaderConstantDesc* pConstants; //!< constantCount �̒l�̐��̗v�f������ V3DPipelineShaderConstantDesc �\���̂̔z��ł��B
	void* pConstantData; //!< �萔�f�[�^�̃|�C���^�ł��B

	//! @}
};

//! @struct V3DPipelineVertexElement
//! @brief �p�C�v���C���̃o�[�e�b�N�X�G�������g
struct V3DPipelineVertexElement
{
	//! @brief ���P�[�V�����ł��B<br>
	//! layout(location = [location])
	uint32_t location;
	//! @brief �I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t offset;
	//! @brief �t�H�[�}�b�g�ł��B
	V3D_FORMAT format;
};

//! @struct V3DPipelineVertexLayout
//! @brief �p�C�v���C���̃o�[�e�b�N�X���C�A�E�g
struct V3DPipelineVertexLayout
{
	//! @brief �o�[�e�b�N�X�o�b�t�@�̃o�C���h�ʒu�ł��B
	uint32_t binding;
	//! @brief �o�[�e�b�N�X�̃X�g���C���o�C�g�P�ʂŎw�肵�܂��B
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
	uint32_t patchControlPoints; //!< �p�b�`���Ƃ̃R���g���[���|�C���g�̐��ł��B
};

//! @struct V3DPipelineRasterizationDesc
//! @brief �p�C�v���C���̃��X�^���C�[�[�V�����̋L�q
//! @note
//! �f�t�H���g�l<br>
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
	//! @brief ���X�^���C�Y�X�e�[�W�̒��O�Ƀv���~�e�B�u��j�����邩�ǂ������w�肵�܂��B<br>
	//! ���̒l�� false �̏ꍇ�́A�r���[�|�[�g�A�V�U�����O�͕K���w�肷��K�v������܂��B
	bool discardEnable;
	//! @brief �[�x���N�����v���邩�ǂ������w�肵�܂��B
	bool depthClampEnable;

	//! @brief �|���S�����[�h�ł��B
	V3D_POLYGON_MODE polygonMode;
	//! @brief �J�����O���[�h�ł��B
	V3D_CULL_MODE cullMode;

	//! @brief �[�x�o�C�A�X��L���ɂ��邩�ǂ������w�肵�܂��B
	bool depthBiasEnable;
	//! @brief ���̐[�x�l�𐧌䂷��X�J���[���q�ł��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetDepthBias �ŕύX���邱�Ƃ��ł��܂��B
	float depthBiasConstantFactor;
	//! @brief �ő� �܂��� �ŏ��̐[�x�o�C�A�X�ł��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetDepthBias �ŕύX���邱�Ƃ��ł��܂��B
	float depthBiasClamp;
	//! @brief �[�x�o�C�A�X�̌v�Z�ɂ�����X���[�v�ɓK�p�����X�J���[�W���ł��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetDepthBias �ŕύX���邱�Ƃ��ł��܂��B
	float depthBiasSlopeFactor;
};

//! @struct V3DPipelineMultisampleDesc
//! @brief �p�C�v���C���̃}���`�T���v���̋L�q
//! @note
//! �f�t�H���g�l<br>
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
	//! @brief �J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v�����ł��B<br>
	//! V3DDeviceCaps::multisampleFlags �� \link V3D_MULTISAMPLE_CAP_VARIABLE_RATE \endlink ���w�肳��Ă��Ȃ������ꍇ�A
	//! �T�u�p�X�̃J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v�����͂��̒l�ł���K�v������܂��B
	V3D_SAMPLE_COUNT_FLAG rasterizationSamples;
	//! @brief �t���O�����g���T���v�����ɃV�F�[�f�B���O�����s���邩�ǂ������w�肵�܂��B<br>
	//! V3DDeviceCaps::multisampleFlags �� \link V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING \endlink ���w�肳��Ă���ꍇ�ɗL���ɂȂ�܂��B
	bool sampleShadingEnable;
	//! @brief �T���v�����̃V�F�[�f�B���O�̍ŏ������ł��B<br>
	//! sampleShadingEnable �� true �ɂ����ꍇ�͕K���w�肵�Ă��������B<br>
	//! �܂��l�͈̔͂� 0.0f �` 1.0f �̊Ԃł���K�v������܂��B
	float minSampleShading;
	//! @brief �A���t�@�g�D�J�o���b�W���g�p���邩�ǂ������w�肵�܂��B
	bool alphaToCoverageEnable;
	//! @brief �t���O�����g�V�F�[�_����o�͂��ꂽ�A���t�@�l���A�Œ菬���_���\���\�ȍő�l�܂��� 1.0 �Œu�������邩�ǂ������w�肵�܂��B<br>
	//! ���̑���� V3DDeviceCaps::multisampleFlags �� \link V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE \endlink ���w�肳��Ă���ꍇ�ɂ̂� true ���w�肷�邱�Ƃ��ł��܂��B
	bool alphaToOneEnable;
};

//! @struct V3DPipelineStencilOpDesc
//! @brief �p�C�v���C���̃X�e���V���I�y���[�^�[�̋L�q
//! @note
//! �f�t�H���g�l<br>
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
	V3D_STENCIL_OP failOp; //!< �X�e���V���e�X�g�Ɏ��s�����T���v���ɑ΂��Ď��s�����A�N�V�����ł��B
	V3D_STENCIL_OP passOp; //!< �[���e�X�g�ƃX�e���V���e�X�g�̗����ɍ��i�����T���v���ɑ΂��Ď��s�����A�N�V�����ł��B
	V3D_STENCIL_OP depthFailOp; //!< �X�e���V���e�X�g�ɍ��i���A�[�x�e�X�g�ɍ��i���Ȃ������T���v���ɑ΂��Ď��s�����A�N�V�����ł��B
	V3D_COMPARE_OP compareOP; //!< �X�e���V���e�X�g�Ŏg�p������r�I�y���[�^�[�ł��B

	//! @brief �X�e���V���e�X�g�ɂ���ēǂݍ��܂��l�̃r�b�g�}�X�N���w�肵�܂��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetStencilReadMask �ŕύX���邱�Ƃ��ł��܂��B
	uint32_t readMask;
	//! @brief �X�e���V���e�X�g�ɂ���ď������܂��l�̃r�b�g�}�X�N���w�肵�܂��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetStencilWriteMask �ŕύX���邱�Ƃ��ł��܂��B
	uint32_t writeMask;
	//! @brief �X�e���V���̔�r�I�y���[�^�[�Ŏg�p�����l�ł��B<br>
	//! ���̒l�� IV3DCommandBuffer::SetStencilReference �ŕύX���邱�Ƃ��ł��܂��B
	//! @sa V3D_COMPARE_OP
	uint32_t reference;
};

//! @struct V3DPipelineDepthStencilDesc
//! @brief �p�C�v���C���̃f�v�X�X�e���V���̋L�q
//! @note
//! �f�t�H���g�l<br>
//! <table>
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
	//! @brief �[�x�e�X�g�̔�r�I�y���[�^�[���w�肵�܂��B
	V3D_COMPARE_OP depthCompareOp;

	//! @brief �X�e���V���e�X�g�����邩�ǂ������w�肵�܂��B
	bool stencilTestEnable;
	//! @brief �X�e���V���̑O�ʂ̃I�y���[�^�[�ł��B
	V3DPipelineStencilOpDesc stencilFront;
	//! @brief �X�e���V���e�X�g�̌�ʂ̃I�y���[�^�[�ł��B
	V3DPipelineStencilOpDesc stencilBack;

	//! @brief �[�x�̋��E�e�X�g�����邩�ǂ������w�肵�܂��B
	bool depthBoundsTestEnable;
	//! @brief �[�x�̋��E�e�X�g�̍ŏ��l�ł��B<br>
	//! �l�� 0.0f �` 1.0f �̊ԂŎw�肷��K�v������܂��B<br>
	//! �܂����̒l�� IV3DCommandBuffer::SetDepthBounds �ŕύX���邱�Ƃ��ł��܂��B
	float minDepthBounds;
	//! @brief �[�x�̋��E�e�X�g�̍ő�l�ł��B<br>
	//! �l�� 0.0f �` 1.0f �̊ԂŎw�肷��K�v������܂��B<br>
	//! �܂����̒l�� IV3DCommandBuffer::SetDepthBounds �ŕύX���邱�Ƃ��ł��܂��B
	float maxDepthBounds;
};

//! @struct V3DPipelineColorBlendAttachment
//! @brief �p�C�v���C���̃J���[�A�^�b�`�����g�̃u�����h
//! @note
//! �f�t�H���g�l<br>
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
	bool blendEnable; //!< �u�����h��L���ɂ��邩�ǂ������w�肵�܂��B
	V3D_BLEND_FACTOR srcColorFactor; //!< �u�����h����RGB�����̃u�����h�W���ł��B
	V3D_BLEND_FACTOR dstColorFactor; //!< �u�����h���RGB�����̃u�����h�W���ł��B
	V3D_BLEND_OP colorOp; //!< RGB�����̃u�����h�I�y���[�^�[�ł��B
	V3D_BLEND_FACTOR srcAlphaFactor; //!< �u�����h����A�����̃u�����h�W���ł��B
	V3D_BLEND_FACTOR dstAlphaFactor; //!< �u�����h���A�����̃u�����h�W���ł��B
	V3D_BLEND_OP alphaOp; //!< A�����̃u�����h�I�y���[�^�[�ł��B
	V3DFlags writeMask; //!< �J���[�̏������݃}�X�N��\�� \link V3D_COLOR_COMPONENT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DPipelineColorBlendDesc
//! @brief �p�C�v���C���̃J���[�u�����h�̋L�q
//! @note
//! �f�t�H���g�l<br>
//! <table>
//! <tr><td>logicOpEnable</td><td>false</td></tr>
//! <tr><td>logicOp</td><td>V3D_LOGIC_OP_COPY</td></tr>
//! <tr><td>attachmentCount</td><td>1 �ȏ�</td></tr>
//! <tr><td>pAttachments</td><td>attachmentCount �̒l�̐��̗v�f������ V3DPipelineColorBlendAttachment �\���̂̔z��</td></tr>
//! </table>
//! <br>
struct V3DPipelineColorBlendDesc
{
	bool logicOpEnable; //!< �_�����Z ( logicOp ) ��L���ɂ��邩�ǂ������w�肵�܂��B
	V3D_LOGIC_OP logicOp; //!< �_�����Z�̎�ނł��B

	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�̐��ł��B
	uint32_t attachmentCount;
	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�̃u�����h�z��ł��B<br>
	//! �z��̗v�f�̐��� attachmentCount �l�Ɠ����ł���K�v����܂��B
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
	V3DPipelineRasterizationDesc rasterization; //!< ���X�^���C�[�[�V�����ł��B
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
	V3DFlags imageUsageFlags; //!< �C���[�W�̎g�p���@�ł��B

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

	//! @brief �C���[�W�������_�����O����C���[�W���l�����܂��B
	//! @retval V3D_OK
	//! @retval V3D_TIMEOUT
	//! @retval V3D_NOT_READY
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT AcquireNextImage() = 0;

	//! @brief ���݂̃C���[�W�̃C���f�b�N�X���擾���܂��B
	//! @return ���݂̃C���[�W�̃C���f�b�N�X��Ԃ��܂��B
	//! @note �Ԃ����l�� AcquireNextImage �ōX�V����܂��B
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
// �R�}���h�v�[��
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DCommandPoolDesc
//! @brief �R�}���h�v�[���̋L�q
struct V3DCommandPoolDesc
{
	uint32_t queueFamily; //!< �L���[�t�@�~���[�ł��B
	V3DFlags propertyFlags; //!< ������\�� V3D_COMMAND_POOL_PROPERTY_FLAG �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @}

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_POOL_PROPERTY_FLAG
//! @brief �R�}���h�v�[���v���p�e�B�t���O
enum V3D_COMMAND_POOL_PROPERTY_FLAG : V3DFlags
{
	//! @brief �R�}���h�v�[���[���犄�蓖�Ă�ꂽ�R�}���h�o�b�t�@�[����r�I�Z���ԂɃ��Z�b�g�܂��͉������邱�Ƃ�\���܂��B
	V3D_COMMAND_POOL_PROPERTY_TRANSIENT = 0x00000001,
	//! @brief �R�}���h�v�[���[���犄�蓖�Ă�ꂽ�R�}���h�o�b�t�@�[���ʂɃ��Z�b�g�ł���悤�ɂ��܂��B<br>
	//! ���̃t���O���w�肳�ꂽ�ꍇ�� IV3DCommandBuffer::Reset �ɂ�郊�Z�b�g���ł� IV3DCommandBuffer::Begin �ɂ��ÖٓI�ȃ��Z�b�g���s���܂��B<br>
	//! �܂��A���̃t���O���w�肳��Ȃ������ꍇ IV3DCommandBuffer::Reset �͎g�p�ł��� IV3DCommandPool::Reset �ɂ���ăR�}���h�o�b�t�@�[���ꊇ�Ń��Z�b�g����悤�ɂȂ�܂��B
	V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER = 0x00000002,
};

//! @enum V3D_COMMAND_ALLOCATOR_RESET_FLAG
//! @brief �R�}���h�v�[���̃��Z�b�g�t���O
enum V3D_COMMAND_ALLOCATOR_RESET_FLAG : V3DFlags
{
	//! @brief ���Z�b�g�����ۂɃR�}���h�o�b�t�@�����L���郁�������\�[�X���R�}���h�v�[���[�ɕԋp���܂��B<br>
	//! �܂����̃t���O���w�肳��Ȃ������ꍇ�̓��������\�[�X��ԋp�����A�ė��p���܂��B
	V3D_COMMAND_ALLOCATOR_RESET_RELEASE_RESOURCES = 0x00000001,
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
	//! @param[in] resetFlags ���Z�b�g�t���O��\�� \link V3D_COMMAND_ALLOCATOR_RESET_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
// �R�}���h�o�b�t�@�[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_COMMAND_BUFFER_TYPE
//! @brief �R�}���h�o�b�t�@�[�̃��x��
enum V3D_COMMAND_BUFFER_TYPE
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

//! @enum V3D_COMMAND_BUFFER_USAGE
//! @brief �R�}���h�o�b�t�@�̎g�p���@
enum V3D_COMMAND_BUFFER_USAGE
{
	//! @brief �R�}���h�o�b�t�@�[�͈�x�����L���[�ɑ��M����邱�Ƃ�\���܂��B
	V3D_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x00000001,
	//! @brief �v���C�}���R�}���h�o�b�t�@�[�ŊJ�n���ꂽ�����_�[�p�X���Ŏ��s�����Z�J���_���R�}���h�o�b�t�@�[�������_�[�p�X�A�T�u�p�X�A�t���[���o�b�t�@�������p�����Ƃ�\���܂��B
	V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE = 0x00000002,
	V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE = 0x00000004,
};

//! @enum V3D_STENCIL_FACE_FLAG
//! @brief �X�e���V���̃t�F�C�X�t���O
enum V3D_STENCIL_FACE_FLAG
{
	V3D_STENCIL_FACE_FRONT = 0x00000001, //!< �O���w�肵�܂��B
	V3D_STENCIL_FACE_BACK = 0x00000002, //!< �����w�肵�܂��B
	V3D_STENCIL_FRONT_AND_BACK = V3D_STENCIL_FACE_FRONT | V3D_STENCIL_FACE_BACK, //!< �O�㗼�����w�肵�܂��B
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DBarrierMemoryDesc
//! @brief �������o���A�̋L�q
struct V3DBarrierMemoryDesc
{
	V3DFlags srcStageMask; //!< ���݂̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� \link V3D_DEPENDENCY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< ���݂̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
};

//! @struct V3DBarrierBufferDesc
//! @brief �o�b�t�@�[�o���A�̋L�q
struct V3DBarrierBufferDesc
{
	V3DFlags srcStageMask; //!< ���݂̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� \link V3D_DEPENDENCY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< ���݂̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< ���݂̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B
};

//! @struct V3DBarrierImageDesc
//! @brief �C���[�W�o���A�̋L�q
struct V3DBarrierImageDesc
{
	V3DFlags srcStageMask; //!< ���݂̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstStageMask; //!< �ڍs��̃X�e�[�W��\�� \link V3D_PIPELINE_STAGE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dependencyFlags; //!< �ˑ�����\�� \link V3D_DEPENDENCY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	V3DFlags srcAccessMask; //!< ���݂̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags dstAccessMask; //!< �ڍs��̃A�N�Z�X���@��\�� \link V3D_ACCESS_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	uint32_t srcQueueFamily; //!< ���݂̃L���[�t�@�~���[�ł��B
	uint32_t dstQueueFamily; //!< �ڍs��̃L���[�t�@�~���[�ł��B

	V3D_IMAGE_LAYOUT srcLayout; //!< ���݂̃C���[�W���C�A�E�g�ł��B
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

	uint64_t srcBufferOffset; //!< �R�s�[���o�b�t�@�̃I�t�Z�b�g�ł��B( �o�C�g�P�� )
};

//! @struct V3DCopyImageToBufferRange
//! @brief �C���[�W���o�b�t�@�ɃR�s�[����͈�
struct V3DCopyImageToBufferRange
{
	uint64_t dstBufferOffset; //!< �R�s�[��o�b�t�@�̃I�t�Z�b�g�ł��B( �o�C�g�P�� )

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
	//! @brief �f�v�X���N���A���邩�ǂ������w�肵�܂��B<br>
	//! �N���A���悤�Ƃ��Ă���f�v�X�X�e���V���A�^�b�`�����g�̃t�H�[�}�b�g�Ƀf�v�X���܂܂�Ă���K�v������܂��B
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
	//! @param[in] resetFlags �R�}���h�o�b�t�@�[�̃��Z�b�g�t���O��\�� \link V3D_COMMAND_BUFFER_RESET_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT Reset(V3DFlags resetFlags) = 0;

	//! @brief �R�}���h�̏������݂��J�n���܂��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! ���̃��\�b�h�̓Z�J���_���R�}���h�o�b�t�@�[�Ŏg�p���邱�Ƃ͂ł��܂���B
	virtual V3D_RESULT Begin() = 0;
	//! @brief �R�}���h�̏������݂��J�n���܂��B
	//! @param[in] usageFlags �R�}���h�o�b�t�@�[�̎g�p�@��\�� \link V3D_COMMAND_BUFFER_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pRenderPass �v���C�}���R�}���h�o�b�t�@�[��������p�������_�[�p�X�ł��B
	//! @param[in] subpass �v���C�}���R�}���h�o�b�t�@�[��������p���T�u�p�X�ł��B
	//! @param[in] pFrameBuffer �v���C�}���R�}���h�o�b�t�@�[��������p���t���[���o�b�t�@�[�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! usageFlags �� \link V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE \endlink ���w�肵���ꍇ�� pRenderPass �� subpass �͕K���w�肵�Ă��������B<br>
	//! �܂� pFrameBuffer �� nullptr ���w�肷�邱�Ƃ��\�ł����A�p�t�H�[�}���X���l���������ꍇ�͗L���ȃt���[���o�b�t�@���w�肷�邱�Ƃ𐄏����܂��B
	virtual V3D_RESULT Begin(V3DFlags usageFlags, IV3DRenderPass* pRenderPass, uint32_t subpass, IV3DFrameBuffer* pFrameBuffer) = 0;
	//! @brief �R�}���h�̏������݂��I�����܂��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT End() = 0;

	//! @}

	//! @name �R�}���h
	//! @{

	//! @brief �������Ƀo���A�𒣂�܂��B
	//! @param[in] barrier �o���A�𒣂郁�����̋L�q�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierMemory(const V3DBarrierMemoryDesc& barrier) = 0;

	//! @brief �o�b�t�@�[�Ƀo���A�𒣂�܂��B
	//! @param[in] pBuffer �o���A�𒣂�o�b�t�@�[�ł��B
	//! @param[in] barrier �o���A�𒣂�o�b�t�@�[�̋L�q�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBuffer(IV3DBuffer* pBuffer, const V3DBarrierBufferDesc& barrier) = 0;

	//! @brief �o�b�t�@�[�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] pBufferView �o���A�𒣂�o�b�t�@�r���[�ł��B
	//! @param[in] barrier �o���A�𒣂�o�b�t�@�[�̋L�q�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierBufferView(IV3DBufferView* pBufferView, const V3DBarrierBufferDesc& barrier) = 0;

	//! @brief �C���[�W�Ƀo���A�𒣂�܂��B
	//! @param[in] pImage �o���A�𒣂�C���[�W�ł��B
	//! @param[in] barrier �o���A�𒣂�C���[�W�̋L�q�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImage(IV3DImage* pImage, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief �C���[�W�r���[�Ƀo���A�𒣂�܂��B
	//! @param[in] pImageView �o���A�𒣂�C���[�W�r���[�ł��B
	//! @param[in] barrier �o���A�𒣂�C���[�W�̋L�q�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BarrierImageView(IV3DImageView* pImageView, const V3DBarrierImageDesc& barrier) = 0;

	//! @brief �o�b�t�@�[���R�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@��\�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] dstOffset �R�s�[��̃o�b�t�@�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@��\�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] srcOffset �R�s�[���̃o�b�t�@�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] size �R�s�[����T�C�Y
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, uint64_t dstOffset, IV3DBuffer* pSrcBuffer, uint64_t srcOffset, uint64_t size) = 0;

	//! @brief �o�b�t�@�[���R�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@��\�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@��\�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �̒l�̐��̗v�f������ V3DCopyBufferRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBuffer(IV3DBuffer* pDstBuffer, IV3DBuffer* pSrcBuffer, uint32_t rangeCount, const V3DCopyBufferRange* pRanges) = 0;

	//! @brief �C���[�W���R�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W���C�A�E�g�ł��B
	//! @note �R�s�[��A�R�s�[���̃C���[�W�̃^�C�v�A�t�H�[�}�b�g�A�T�C�Y�A���x�����A���C���[���A�A�X�y�N�g�̂��ׂĂ���v���Ă���K�v������܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief �C���[�W���R�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W���C�A�E�g�ł��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �l�̐��̗v�f������ V3DCopyImageRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageRange* pRanges) = 0;

	//! @brief �o�b�t�@���C���[�W�ɃR�s�[���܂��B
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W�̃��C�A�E�g�ł��B
	//! @param[in] dstLevel �R�s�[��̃C���[�W�̃��x���ł��B
	//! @param[in] dstBaseLayer �R�s�[��̃C���[�W�̍ŏ��̃��x���ł��B
	//! @param[in] dstLayerCount �R�s�[��̃C���[�W�̃��x�����ł��B
	//! @param[in] dstOffset �R�s�[��̃C���[�W�̃I�t�Z�b�g�ł��B
	//! @param[in] dstSize �R�s�[��̃C���[�W�̃T�C�Y�ł��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�ł��B
	//! @param[in] srcBufferOffset �R�s�[���̃o�b�t�@�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, uint32_t dstLevel, uint32_t dstBaseLayer, uint32_t dstLayerCount, const V3DPoint3D& dstOffset, const V3DSize3D& dstSize,
		IV3DBuffer* pSrcBuffer, uint32_t srcBufferOffset) = 0;

	//! @brief �o�b�t�@���C���[�W�ɃR�s�[���܂�
	//! @param[in] pDstImage �R�s�[��̃C���[�W�ł��B
	//! @param[in] dstImageLayout �R�s�[��̃C���[�W�̃��C�A�E�g�ł��B
	//! @param[in] pSrcBuffer �R�s�[���̃o�b�t�@�ł��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �̒l�̗v�f�̐������� V3DCopyBufferToImageRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyBufferToImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DBuffer* pSrcBuffer,
		uint32_t rangeCount, const V3DCopyBufferToImageRange* pRanges) = 0;

	//! @brief �C���[�W���o�b�t�@�[�ɃR�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! @param[in] dstBufferOffset �R�s�[��̃o�b�t�@�[�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W�̃��C�A�E�g�ł��B
	//! @param[in] srcLevel �R�s�[���̃C���[�W�̃��x���ł��B
	//! @param[in] srcBaseLayer �R�s�[���̃C���[�W�̍ŏ��̃��C���[�ł��B
	//! @param[in] srcLayerCount �R�s�[���̃C���[�W�̃��C���[���ł��B
	//! @param[in] srcOffset �R�s�[���̃C���[�W�̃I�t�Z�b�g�ł��B
	//! @param[in] srcSize �R�s�[���̃C���[�W�̃T�C�Y�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer, uint32_t dstBufferOffset,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout, uint32_t srcLevel, uint32_t srcBaseLayer, uint32_t srcLayerCount, const V3DPoint3D& srcOffset, const V3DSize3D& srcSize) = 0;

	//! @brief �C���[�W���o�b�t�@�[�ɃR�s�[���܂��B
	//! @param[in] pDstBuffer �R�s�[��̃o�b�t�@�[�ł��B
	//! @param[in] pSrcImage �R�s�[���̃C���[�W�ł��B
	//! @param[in] srcImageLayout �R�s�[���̃C���[�W�̃��C�A�E�g�ł��B
	//! @param[in] rangeCount �R�s�[����͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �̒l�̗v�f�̐������� V3DCopyImageToBufferRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void CopyImageToBuffer(
		IV3DBuffer* pDstBuffer,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DCopyImageToBufferRange* pRanges) = 0;

	//! @brief �C���[�W��]�����܂��B
	//! @param[in] pDstImage �]����̃C���[�W�ł��B
	//! @param[in] dstImageLayout �]����̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImage �]�����̃C���[�W�ł��B
	//! @param[in] srcImageLayout �]�����̃C���[�W���C�A�E�g�ł��B
	//! @param[in] rangeCount �]������͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �l�̐��̗v�f������ V3DBlitImageRange �\���̂̔z��ł��B
	//! @param[in] filter �t�B���^�[�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BlitImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, V3DBlitImageRange* pRanges,
		V3D_FILTER filter) = 0;

	//! @brief �}���`�T���v���C���[�W���}���`�T���v���C���[�W�̕ϊ����܂��B
	//! @param[in] pDstImage �ϊ���̃C���[�W�ł��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImage �ϊ����̃C���[�W�ł��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! @note
	//! �ϊ���A�ϊ����̃C���[�W�̃^�C�v�A�t�H�[�}�b�g�A�T�C�Y�A���x�����A���C���[���̂��ׂĂ���v���Ă���K�v������܂��B<br>
	//! <br>
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief �}���`�T���v���C���[�W���}���`�T���v���C���[�W�̕ϊ����܂��B
	//! @param[in] pDstImage �ϊ���̃C���[�W�ł��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImage �ϊ����̃C���[�W�ł��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! @param[in] rangeCount �ϊ�����͈͂̐��ł��B
	//! @param[in] pRanges rangeCount �l�̐��̗v�f������ V3DResolveImageRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImage(
		IV3DImage* pDstImage, V3D_IMAGE_LAYOUT dstImageLayout,
		IV3DImage* pSrcImage, V3D_IMAGE_LAYOUT srcImageLayout,
		uint32_t rangeCount, const V3DResolveImageRange* pRanges) = 0;

	//! @brief �}���`�T���v���C���[�W���}���`�T���v���C���[�W�̕ϊ����܂��B
	//! @param[in] pDstImageView �ϊ���̃C���[�W�r���[�ł��B
	//! @param[in] dstImageLayout �ϊ���̃C���[�W���C�A�E�g�ł��B
	//! @param[in] pSrcImageView �ϊ����̃C���[�W�r���[�ł��B
	//! @param[in] srcImageLayout �ϊ����̃C���[�W���C�A�E�g�ł��B
	//! @note
	//! �ϊ���A�ϊ����̃C���[�W�r���[�̃^�C�v�A�t�H�[�}�b�g�A�T�C�Y�A���x�����A���C���[���̂��ׂĂ���v���Ă���K�v������܂��B<br>
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResolveImageView(IV3DImageView* pDstImageView, V3D_IMAGE_LAYOUT dstImageLayout, IV3DImageView* pSrcImageView, V3D_IMAGE_LAYOUT srcImageLayout) = 0;

	//! @brief �����_�[�p�X���J�n���܂��B
	//! @param[in] pRenderPass �J�n���郌���_�[�p�X��\�� IV3DRenderPass �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] pFrameBuffer �T�u�p�X���Q�Ƃ���A�^�b�`�����g�������Ă���t���[���o�b�t�@�ł��B
	//! @param[in] subpassContentInline �T�u�p�X�̃R�}���h�����̃R�}���h�o�b�t�@�[�ɋL�^����ꍇ�� true ���w�肵�܂��B<br>
	//! �܂��T�u�p�X�̃R�}���h���Z�J���_���R�}���h�o�b�t�@�[�ŋL�^���Ă��̃R�}���h�o�b�t�@�[�Ŏ��s�������ꍇ�� false ���w�肵�Ă��������B
	//! @param[in] pRenderArea �����_�����O�̈�ł��B<br>
	//! nullptr �̏ꍇ�� �A�^�b�`�����g�S�̂�\���܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>

	virtual void BeginRenderPass(IV3DRenderPass* pRenderPass, IV3DFrameBuffer* pFrameBuffer, bool subpassContentInline, const V3DRectangle2D* pRenderArea = nullptr) = 0;
	//! @brief �����_�[�p�X���I�����܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �����_�[�p�X���ł̂ݗL��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void EndRenderPass() = 0;

	//! @brief ���̃T�u�p�X�ֈڍs���܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �����_�[�p�X���ł̂ݗL��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void NextSubpass() = 0;

	//! @brief �C���[�W���N���A���܂��B
	//! @param[in] pImage �N���A����C���[�W�ł��B
	//! @param[in] imageLayout �N���A����C���[�W�̃��C�A�E�g�ł��B<br>
	//! ���̒l�� \link V3D_IMAGE_LAYOUT_GENERAL \endlink �A \link V3D_IMAGE_LAYOUT_TRANSFER_DST \endlink �̂����ꂩ�ł���K�v������܂��B
	//! @param[in] clearValue �N���A����l�ł��B
	//! @note 
	//! �N���A����O�ɃC���[�W���w�肵�����C�A�E�g�Ɉڍs���Ă���K�v������܂��B<br>
	//! <br>
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!       \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink �� <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!       <br>
	//!       �� �N���A����C���[�W���f�v�X�X�e���V���̏ꍇ�͖����ł�
	//!     <td>
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BarrierImage
	virtual void ClearImage(IV3DImage* pImage, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief �C���[�W�r���[���N���A���܂��B
	//! @param[in] pImageView �N���A����C���[�W�̃r���[��\�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] imageLayout �N���A����C���[�W�̃��C�A�E�g�ł��B<br>
	//! ���̒l�� \link V3D_IMAGE_LAYOUT_GENERAL \endlink �A \link V3D_IMAGE_LAYOUT_TRANSFER_DST \endlink �̂����ꂩ�ł���K�v������܂��B
	//! @param[in] clearValue �N���A����l�ł��B
	//! @note 
	//! �N���A����O�ɃC���[�W���w�肵�����C�A�E�g�Ɉڍs���Ă���K�v������܂��B<br>
	//! <br>
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!       \link V3D_PIPELINE_STAGE_COMPUTE_SHADER \endlink �� <br>
	//!       \link V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE \endlink <br>
	//!       <br>
	//!       �� �N���A����C���[�W���f�v�X�X�e���V���̏ꍇ�͖����ł�
	//!     <td>
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DCommandBuffer::BarrierImageView
	virtual void ClearImageView(IV3DImageView* pImageView, V3D_IMAGE_LAYOUT imageLayout, const V3DClearValue& clearValue) = 0;

	//! @brief �T�u�p�X�̃A�^�b�`�����g���N���A���܂��B
	//! @param[in] colorAttachmentCount �N���A����A�^�b�`�����g�̐��ł��B
	//! @param[in] pColorAttachments colorAttachmentCount �̒l�̐��̗v�f������ V3DClearAttachmentDesc �\���̂̔z��ł��B
	//! @param[in] pDepthStencilAttachment �f�v�X�X�e���V���A�^�b�`�����g�̃N���A�̋L�q�̃|�C���^�ł��B ( �f�v�X�X�e���V���������ꍇ�� nullptr )
	//! @param[in] rangeCount �N���A����͈͂̐��ł��B
	//! @param[in] pRanges rectCount �̒l�̐��̗v�f������ V3DClearRange �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �����_�[�p�X���ł̂ݗL��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ClearAttachments(
		uint32_t colorAttachmentCount, const V3DClearColorAttachmentDesc* pColorAttachments,
		const V3DClearDepthStencilAttachmentDesc* pDepthStencilAttachment,
		uint32_t rangeCount, const V3DClearRange* pRanges) = 0;

	//! @brief �p�C�v���C�����o�C���h���܂��B
	//! @param[in] pPipeline �o�C���h����p�C�v���C����\�� IV3DPipeline �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindPipeline(IV3DPipeline* pPipeline) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���o�C���h���܂��B
	//! @param[in] pipelineType �p�C�v���C���̃^�C�v�ł��B
	//! @param[in] pPipelineLayout �p�C�v���C���̃��C�A�E�g�ł��B
	//! @param[in] firstSet �f�X�N���v�^�Z�b�g��ݒ肷��ŏ��̃Z�b�g�ԍ��ł��B
	//! @param[in] descriptorSetCount �o�C���h����f�X�N���v�^�Z�b�g�̐��ł��B
	//! @param[in] ppDescriptorSets descriptorSetCount �̒l�̐��̗v�f������ IV3DDescriptorSet �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindDescriptorSets(
		V3D_PIPELINE_TYPE pipelineType,	IV3DPipelineLayout* pPipelineLayout,
		uint32_t firstSet, uint32_t descriptorSetCount, IV3DDescriptorSet** ppDescriptorSets) = 0;

	//! @brief �o�[�e�b�N�X�o�b�t�@�r���[���o�C���h���܂��B
	//! @param[in] firstBinding �ŏ��̃o�C���f�B���O�ł��B
	//! @param[in] bindingCount �o�C���f�B���O���ł��B
	//! @param[in] pBufferViews bindingCount �̒l�̐��̗v�f������ IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindVertexBufferViews(uint32_t firstBinding, uint32_t bindingCount, IV3DBufferView** pBufferViews) = 0;

	//! @brief �C���f�b�N�X�o�b�t�@�r���[���o�C���h���܂��B
	//! @param[in] pBufferView �o�C���h����C���f�b�N�X�o�b�t�@�̃r���[��\�� IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @param[in] indexType �C���f�b�N�X�^�C�v��\�� \link V3D_INDEX_TYPE \endlink �񋓒萔�̂����ꂩ���w�肵�܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BindIndexBufferView(IV3DBufferView* pBufferView, V3D_INDEX_TYPE indexType) = 0;

	//! @brief �萔���v�b�V�����܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] slot �萔���v�b�V������X���b�g�ł��B<br>
	//! �p�C�v���C�����C�A�E�g�쐬���Ɏw�肵�� V3DConstantDesc �\���̂̔z��̃C���f�b�N�X�ɂȂ�܂��B
	//! @param[in] pData �v�b�V������f�[�^�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, const void* pData) = 0;

	//! @brief �萔���v�b�V�����܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] slot �萔���v�b�V������X���b�g�ł��B<br>
	//! �p�C�v���C�����C�A�E�g�쐬���Ɏw�肵�� V3DConstantDesc �\���̂̔z��̃C���f�b�N�X�ł��B
	//! @param[in] offset �v�b�V����̒萔�̃������I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] size �v�b�V������f�[�^�̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[in] pData �v�b�V������f�[�^�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa IV3DDevice::CreatePipelineLayout
	virtual void PushConstant(IV3DPipelineLayout* pPipelineLayout, uint32_t slot, uint32_t offset, uint32_t size, const void* pData) = 0;

	//! @brief �r���[�|�[�g��ݒ肵�܂��B
	//! @param[in] firstViewport �ݒ肷��ŏ��̃r���[�|�[�g�̃C���f�b�N�X�ł��B
	//! @param[in] viewportCount �ݒ肷��r���[�|�[�g�̐��ł��B<br>
	//! �r���[�|�[�g�̍ő吔�� V3DDeviceCaps::maxViewports �̒l�ɂȂ�܂��B
	//! @param[in] pViewports viewportCount �̒l�̐��̗v�f������ V3DViewport �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetViewport(uint32_t firstViewport, uint32_t viewportCount, const V3DViewport* pViewports) = 0;

	//! @brief �V�U�����O��ݒ肵�܂��B
	//! @param[in] firstScissor �ݒ肷��ŏ��̃V�U�����O�̃C���f�b�N�X�ł��B
	//! @param[in] scissorCount �ݒ肷��V�U�����O�̐��ł��B<br>
	//! �V�U�����O�̍ő吔�� V3DDeviceCaps::maxViewports �̒l�ɂȂ�܂��B
	//! @param[in] pScissors scissorCount �̒l�̐��̗v�f������ V3DRectangle2D �\���̂̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetScissor(uint32_t firstScissor, uint32_t scissorCount, const V3DRectangle2D* pScissors) = 0;

	//! @brief �[�x�o�C�A�X��ݒ肵�܂��B
	//! @param[in] depthBiasConstantFactor ���̐[�x�l�𐧌䂷��X�J���[���q�ł��B
	//! @param[in] depthBiasClamp �ő�܂��͍ŏ��̐[�x�o�C�A�X�ł��B
	//! @param[in] depthBiasSlopeFactor �[�x�o�C�A�X�̌v�Z�ɂ�����X���[�v�ɓK�p�����X�J���[�W���ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;

	//! @brief �[�x���E�e�X�g��ݒ肵�܂��B
	//! @param[in] minDepthBounds �ŏ��[�x�� 0.0f �` 1.0f �̊ԂŎw�肵�܂��B
	//! @param[in] maxDepthBounds �ő�[�x�� 0.0f �` 1.0f �̊ԂŎw�肵�܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetDepthBounds(float minDepthBounds, float maxDepthBounds) = 0;

	//! @brief �X�e���V���e�X�g�ɂ���ēǂݍ��܂��l�̃r�b�g�}�X�N��ݒ肵�܂��B
	//! @param[in] faceMask �ݒ肷��t�F�C�X��\�� \link V3D_STENCIL_FACE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] readMask �ǂݍ��܂��l�̃r�b�g�}�X�N�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetStencilReadMask(V3DFlags faceMask, uint32_t readMask) = 0;

	//! @brief �X�e���V���e�X�g�ɂ���ď������܂��l�̃r�b�g�}�X�N��ݒ肵�܂��B
	//! @param[in] faceMask �ݒ肷��t�F�C�X��\�� \link V3D_STENCIL_FACE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] writeMask �������܂��l�̃r�b�g�}�X�N�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void SetStencilWriteMask(V3DFlags faceMask, uint32_t writeMask) = 0;

	//! @brief �X�e���V���̔�r�I�y���[�^�[�Ŏg�p�����l��ݒ肵�܂��B
	//! @param[in] faceMask �ݒ肷��t�F�C�X��\�� \link V3D_STENCIL_FACE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] reference ��r�I�y���[�^�[�Ŏg�p�����l�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_COMPARE_OP
	virtual void SetStencilReference(V3DFlags faceMask, uint32_t reference) = 0;

	//! @brief �u�����h�萔��ݒ肵�܂��B
	//! @param[in] blendConstants RGBA �̃u�����h�萔�� 0.0f �` 1.0f �̊ԂŎw�肵�܂��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	//! @sa V3D_BLEND_FACTOR
	//! @sa V3DPipelineColorBlendAttachment
	virtual void SetBlendConstants(const float blendConstants[4]) = 0;

	//! @brief �N�G���v�[�������Z�b�g���܂��B
	//! @param[in] pQueryPool ���Z�b�g����N�G���v�[���ł��B
	//! @param[in] firstQuery ���Z�b�g����ŏ��̃N�G���̃C���f�b�N�X�ł��B
	//! @param[in] queryCount ���Z�b�g����N�G���̐��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void ResetQueryPool(IV3DQueryPool* pQueryPool, uint32_t firstQuery, uint32_t queryCount) = 0;
	//! @brief �N�G�����J�n���܂��B
	//! @param[in] pQueryPool �N�G���̌��ʂ��Ǘ�����N�G���v�[���ł��B
	//! @param[in] query ���ʂ̎擾���J�n����N�G���̃C���f�b�N�X�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void BeginQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;
	//! @brief �N�G�����I�����܂��B
	//! @param[in] pQueryPool �N�G���̌��ʂ��Ǘ�����N�G���v�[���ł��B
	//! @param[in] query ���ʂ��i�[����Ă���N�G���̃C���f�b�N�X�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void EndQuery(IV3DQueryPool* pQueryPool, uint32_t query) = 0;
	//! @brief �^�C�v�X�^���v���������݂܂��B
	//! @param[in] pQueryPool �^�C���X�^���v���Ǘ�����N�G���v�[���ł��B
	//! @param[in] pipelineStage �p�C�v���C���X�e�[�W�ł��B
	//! @param[in] query �^�C���X�^���v�̃N�G���̃C���f�b�N�X�ł��B
	//! @note
	//! �w�肵���p�C�v���C���X�e�[�W�̎��s�����������ۂɃ^�C���X�^���v�̒l���������݂܂��B<br>
	//! <br>
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void WriteTimestamp(IV3DQueryPool* pQueryPool, V3D_PIPELINE_STAGE_FLAG pipelineStage, uint32_t query) = 0;

	//! @brief �`������܂��B
	//! @param[in] vertexCount ���_�̐��ł��B
	//! @param[in] instanceCount �C���X�^���X�̐��ł��B
	//! @param[in] firstVertex �ŏ��̒��_�̃C���f�b�N�X�ł��B
	//! @param[in] firstInstance �ŏ��̃C���X�^���X�̃C���f�b�N�X�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �����_�[�p�X���ł̂ݗL��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

	//! @brief �C���f�b�N�X�o�b�t�@���g�p�����`������܂��B
	//! @param[in] indexCount �C���f�b�N�X�̐��ł��B
	//! @param[in] instanceCount �C���X�^���X�̐��ł��B
	//! @param[in] firstIndex �ŏ��̃C���f�b�N�X�ł��B
	//! @param[in] firstInstance �ŏ��̃C���X�^���X�̃C���f�b�N�X�ł��B
	//! @param[in] vertexOffset ���_�̃o�C�g�I�t�Z�b�g�ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �����_�[�p�X���ł̂ݗL��
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance, int32_t vertexOffset) = 0;

	//! @brief �R���s���[�g�V�F�[�_�[�̍�Ƃ��f�B�X�p�b�`���܂��B
	//! @param[in] groupCountX X �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! @param[in] groupCountY Y �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! @param[in] groupCountZ Z �����Ƀf�B�X�p�b�`�����O���[�v�̐��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     ����
	//!     </td>
	//!   </tr>
	//! </table>
	virtual void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

	//! @brief �Z�J���_���R�}���h�o�b�t�@�[�����s���܂��B
	//! @param[in] commandBufferCount ���s����Z�J���_���R�}���h�o�b�t�@�[�̐��ł��B
	//! @param[in] ppCommandBuffers commandBufferCount �̒l�̐��̗v�f������ IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @note
	//! <table>
	//!   <tr><th>�T�|�[�g�����R�}���h�o�b�t�@�[</th><th>�T�|�[�g�����L���[</th><th>�T�|�[�g�����p�C�v���C���X�e�[�W</th><th>�����_�[�p�X���ł̎g�p</th></tr>
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
	//!     �L��
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
// �L���[
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_QUEUE_FLAG
//! @brief �L���[�t���O
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
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;
	//! @brief �v���C�}���R�}���h�o�b�t�@�[���L���[�ɑ��M���܂��B
	//! @param[in] pSwapChain �������Ƃ�X���b�v�`�F�C���ł��B
	//! @param[in] commandBufferCount ���M����R�v���C�}���}���h�o�b�t�@�[�̐��ł��B
	//! @param[in] ppCommandBuffers ���M����v���C�}���R�}���h�o�b�t�@�[��\�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @param[in] pFence �R�}���h�o�b�t�@�[�̑��M�̊����̑ҋ@�Ɏg�p����t�F���X�ł��B<br>
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORYDrawModels
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) = 0;

	//! @brief �����_�����O���ꂽ�C���[�W��\�����܂��B
	//! @param[in] pSwapChain �������Ƃ�X���b�v�`�F�C���ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) = 0;

	//! @brief �L���[���A�C�h����ԂɂȂ�܂őҋ@���܂��B
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
// �A�_�v�^
// ----------------------------------------------------------------------------------------------------

//! @addtogroup v3d_enum_group
//! @{

//! @enum V3D_MEMORY_HEAP_TYPE
//! @brief �������q�[�v�̃^�C�v
enum V3D_MEMORY_HEAP_TYPE
{
	V3D_MEMORY_HEAP_TYPE_HOST = 0, //!< �z�X�g�̃������q�[�v�ł��B
	V3D_MEMORY_HEAP_TYPE_DEVICE = 1, //!< �f�o�C�X�̃������q�[�v�ł��B
};

//! @enum V3D_BUFFER_FORMAT_FEATURE_FLAG
//! @brief �o�b�t�@�[�t�H�[�}�b�g�̃T�|�[�g�t���O
enum V3D_BUFFER_FORMAT_FEATURE_FLAG
{
	//! @brief ���j�t�H�[���e�N�Z���o�b�t�@�Ƃ��Ďg�p�ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_UNIFORM_TEXEL = 0x00000008,
	//! @brief �X�g���[�W�e�N�Z���o�b�t�@�Ƃ��Ďg�p�ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL = 0x00000010,
	//! @brief �A�g�~�b�N������T�|�[�g����X�g���[�W�e�N�Z���o�b�t�@�Ƃ��Ďg�p�ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_STORAGE_TEXEL_ATOMIC = 0x00000020,
	//! @brief �o�[�e�b�N�X�o�b�t�@�Ƃ��Ďg�p�ł��܂��B
	V3D_BUFFER_FORMAT_FEATURE_VERTEX = 0x00000040,
};

//! @enum V3D_IMAGE_FORMAT_FEATURE_FLAG
//! @brief �C���[�W�t�H�[�}�b�g�̃T�|�[�g�t���O
enum V3D_IMAGE_FORMAT_FEATURE_FLAG
{
	//! @brief �T���v�����O�ł���C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_SAMPLED = 0x00000001,
	//! @brief �X�g���[�W�C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_STORAGE = 0x00000002,
	//! @brief �A�g�~�b�N������T�|�[�g����X�g���[�W�C���[�W�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_STORAGE_ATOMIC = 0x00000004,
	//! @brief �J���[�A�^�b�`�����g����уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT = 0x00000080,
	//! @brief �u�����f�B���O���T�|�[�g����J���[�A�^�b�`�����g����уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND = 0x00000100,
	//! @brief �f�v�X�X�e���V���A�^�b�`�����g����уC���v�b�g�A�^�b�`�����g�Ƃ��Ďg�p�ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT = 0x00000200,
	//! @brief IV3DCommandBuffer::BlitImage �� pSrcImage �Ƃ��Ďg�p���邱�Ƃ��ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC = 0x00000400,
	//! @brief IV3DCommandBuffer::BlitImage �� pDstImage �Ƃ��Ďg�p���邱�Ƃ��ł��܂��B
	V3D_IMAGE_FORMAT_FEATURE_BLIT_DST = 0x00000800,
	//! @brief \link V3D_IMAGE_FORMAT_FEATURE_SAMPLED \endlink �ƈꏏ�Ɏw�肳��Ă����ꍇ�́A�T���v���[�̊g��t�B���^����яk���t�B���^�� \link V3D_MIPMAP_MODE_LINEAR \endlink ���g�p���邱�Ƃ��ł��܂��B<br>
	//! �܂� \link V3D_IMAGE_FORMAT_FEATURE_BLIT_SRC \endlink �ƈꏏ�Ɏw�肳��Ă����ꍇ�́A\link IV3DCommandBuffer::BlitImage \endlink �� filter �� \link V3D_FILTER_LINEAR \endlink ���g�p���邱�Ƃ��ł��܂��B
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
	V3DFlags propertyFlags; //!< ������������\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
	//! @brief �C���f�b�N�X�o�b�t�@�� �������� 32Bit ���� ( \link V3D_INDEX_TYPE_UINT32 \endlink ) ���g�p���邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::BindIndexBufferView
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
	//! @brief �I�N���[�W�����N�G�� \link V3D_QUERY_TYPE_OCCLUSION \endlink �Ō����Ȍ��ʂ��擾���邱�Ƃ��ł��܂��B<br>
	//! ���̃t���O���w�肳��Ă��Ȃ������ꍇ�A�I�N���[�W�����N�G���̌��ʂ̓s�N�Z�����������܂�Ȃ������ꍇ�� 0 �s�N�Z�����������܂ꂽ�ꍇ�� 1 �̂悤�ɕԂ����Ƃ�����܂��B
	//! @sa IV3DCommandBuffer::BeginQuery
	V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE = 0x00000001,
	//! @brief �p�C�v���C���̓��v�N�G���ł��� \link V3D_QUERY_TYPE_PIPELINE_STATISTICS \endlink ���T�|�[�g���܂��B
	//! @sa IV3DDevice::CreateQueryPool
	V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY = 0x00000002,
	//! @brief �v���C�}���R�}���h�o�b�t�@�[�ŃI�N���[�W�����N�G�����A�N�e�B�u�ɂȂ��Ă���� ( IV3DCommandBuffer::BeginQuery �` IV3DCommandBuffer::EndQuery ) �ɁA�Z�J���_���R�}���h�o�b�t�@�����s���邱�Ƃ��ł��܂��B
	//! @sa IV3DCommandBuffer::BeginQuery
	//! @sa IV3DCommandBuffer::ExecuteCommandBuffers
	V3D_QUERY_CAP_INHERITED_QUERIES = 0x00000004,
};

//! @enum V3D_IMAGE_CAP_FLAG
//! @brief �C���[�W�̔\�̓t���O
enum V3D_IMAGE_CAP_FLAG : V3DFlags
{
	//! @brief �C���[�W�r���[�̃^�C�v�� \link V3D_IMAGE_VIEW_TYPE_CUBE_ARRAY \endlink ���w�肷�邱�Ƃ��ł��܂��B
	//! @sa IV3DDevice::CreateImageView
	V3D_IMAGE_CAP_CUBE_ARRAY = 0x00000001,
	//! @brief ETC2 ���k�C���[�W���T�|�[�g���܂��B<br>
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
	//! @brief ASTC LDR ���k�C���[�W���T�|�[�g���܂��B<br>
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
	//! @brief BC ���k�C���[�W���T�|�[�g���܂��B<br>
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
//! @brief �V�F�[�_�[�̔\�̓t���O
enum V3D_SHADER_CAP_FLAG : V3DFlags
{
	//! @brief �W�I���g���V�F�[�_�[���g�p���邱�Ƃ��ł��܂��B
	V3D_SHADER_CAP_GEOMETRY = 0x00000001,
	//! @brief �e�b�Z���[�V�����V�F�[�_�[���g�p���邱�Ƃ��ł��܂��B
	V3D_SHADER_CAP_TESSELLATION = 0x00000002,
	//! @brief �o�b�t�@�ւ̃A�N�Z�X���AV3DBufferSubresourceDesc::size �ɑ΂��ċ��E���`�F�b�N����܂��B<br>
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
	//! @brief �e�b�Z���[�V��������A�e�b�Z���[�V�����]���A����уW�I���g���V�F�[�_�X�e�[�W�� PointSize �r���g�C���������g�p�ł��܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�́APointSize �ő������ꂽ�����o�[��ǂݏ������Ȃ��ł��������B<br>
	//! �܂��A�e�b�Z���[�V�����V�F�[�_�[�܂��̓W�I���g���V�F�[�_�[���珑�����܂��|�C���g�̃T�C�Y�� 1.0 �ɂȂ�܂��B<br>
	//! ����́A�V�F�[�_���W���[�����e�b�Z���[�V��������V�F�[�_�ƕ]���V�F�[�_�́@TessellationPointSize �@�\��錾�ł��邩�ǂ����A
	//! �܂��̓V�F�[�_���W���[�����W�I���g���V�F�[�_�� GeometryPointSize �@�\��錾�ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���T�|�[�g��������ł́AtessellationShader �܂��� geometryShader �̂����ꂩ�̋@�\�܂��͗������T�|�[�g����K�v������܂��B
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
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ \link V3D_IMAGE_USAGE_STORAGE \endlink ���܂ގg�p���@�ŃC���[�W���쐬����Ƃ��̃T���v������ \link V3D_SAMPLE_COUNT_1 \endlink �ɂ���K�v������܂��B<br>
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
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER \endlink �܂��� \link V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC \endlink �̃��\�[�X�́A
	//! �V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� UniformBufferArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B<br>
	V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00001000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���ăT���v���܂��̓T���v���C���[�W�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� \link V3D_DESCRIPTOR_TYPE_SAMPLER \endlink�A\link V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER \endlink�A
	//! �܂��� \link V3D_DESCRIPTOR_TYPE_SAMPLED_IMAGE \endlink �̃��\�[�X�́A�V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� SampledImageArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING = 0x00002000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���ăX�g���[�W�o�b�t�@�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER \endlink �܂��� \link V3D_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC \endlink �̃��\�[�X�́A
	//! �V�F�[�_�R�[�h�̔z��ɏW�񂳂ꂽ�Ƃ��ɒ萔�̐ϕ����ɂ���Ă̂ݍ����t������K�v������܂��B<br>
	//! ����́A�V�F�[�_���W���[���� StorageBufferArrayDynamicIndexing �@�\��錾�ł��邩�ǂ�����\���܂��B
	V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING = 0x00004000,
	//! @brief �V�F�[�_�R�[�h���̓��I�Ɉ�l�Ȑ������ɂ���Ċi�[�C���[�W�̔z��������t���ł��邩�ǂ�����\���܂��B<br>
	//! ���̋@�\���L���ɂȂ��Ă��Ȃ��ꍇ�A�f�X�N���v�^�̃^�C�v�� \link V3D_DESCRIPTOR_TYPE_STORAGE_IMAGE \endlink �̃��\�[�X�́A
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
	//! @brief �|���S�����[�h�� \link V3D_POLYGON_MODE_LINE \endlink \link V3D_POLYGON_MODE_POINT \endlink ���w�肷�邱�Ƃ��ł��܂��B
	//! @sa V3DPipelineRasterizationDesc::polygonMode
	V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID = 0x00000002,
	//! @brief �[�x�̃N�����v���T�|�[�g���܂��B
	//! @sa V3DPipelineRasterizationDesc::depthClampEnable
	V3D_RASTERIZER_CAP_DEPTH_CLAMP = 0x00000010,
	//! @brief �[�x�o�C�A�X�̃N�����v���T�|�[�g���܂��B
	//! @sa V3DPipelineRasterizationDesc::depthBiasEnable
	//! @sa IV3DCommandBuffer::SetDepthBias
	V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP = 0x00000020,
};

//! @enum V3D_MULTISAMPLE_CAP_FLAG
//! @brief �}���`�T���v���̔\�̓t���O
enum V3D_MULTISAMPLE_CAP_FLAG : V3DFlags
{
	//! @brief �T�u�p�X�̃J���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v������ V3DPipelineMultisampleDesc::rasterizationSamples �Ɠ����l�ł���K�v���Ȃ����Ƃ�\���܂��B<br>
	//! ���̃t���O���w�肳��Ă��Ȃ������ꍇ�A�T�u�p�X�̑S�ăJ���[�A�^�b�`�����g�A�f�v�X�X�e���V���A�^�b�`�����g�̃T���v������ V3DPipelineMultisampleDesc::rasterizationSamples �Ɠ����l�łȂ��Ƃ����܂���B
	V3D_MULTISAMPLE_CAP_VARIABLE_RATE = 0x00000001,
	//! @brief �T���v�����̃V�F�[�f�B���O�ƃ}���`�T���v���̕�Ԃ��T�|�[�g���܂��B<br>
	//! ���̃t���O���w�肳��Ă���ꍇ�� V3DPipelineMultisampleDesc::sampleShadingEnable �� true �Ɏw�肷�邱�Ƃ��ł��AV3DPipelineMultisampleDesc::minSampleShading �̒l���L���ɂȂ�܂��B
	V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING = 0x00000002,
	//! @brief �t���O�����g�V�F�[�_����o�͂��ꂽ�A���t�@�l���A�Œ菬���_���\���ł���ő�l�܂��� 1.0 �Œu�������邱�Ƃ��ł��܂��B<br>
	//! ���̃t���O���w�肳��Ă����ꍇ�AV3DPipelineMultisampleDesc::alphaToOneEnable ��true ���w�肷�邱�Ƃ��ł��܂��B
	V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE = 0x00000004,
};

//! @enum V3D_DEPTH_STENCIL_CAP_FLAG
//! @brief �f�v�X�X�e���V���̔\�̓t���O
enum V3D_DEPTH_STENCIL_CAP_FLAG : V3DFlags
{
	//! @brief �[�x�̋��E�e�X�g���T�|�[�g���܂��B
	//! @sa V3DPipelineDepthStencilDesc::depthBoundsTestEnable
	//! @sa IV3DCommandBuffer::SetDepthBounds
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
	//! \link V3D_BLEND_FACTOR_SRC1_COLOR \endlink<br>
	//! \link V3D_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR \endlink<br>
	//! \link V3D_BLEND_FACTOR_SRC1_ALPHA \endlink<br>
	//! \link V3D_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA \endlink<br>
	//! @sa V3DPipelineColorBlendAttachment
	V3D_COLOR_BLEND_CAP_DUAL_SRC = 0x00000002,
	//! @brief �J���[�u�����h�ł̘_���I�y���[�^�[���T�|�[�g���܂��B
	//! @sa V3DPipelineColorBlendDesc::logicOpEnable
	//! @sa V3DPipelineColorBlendDesc::logicOp
	V3D_COLOR_BLEND_CAP_LOGIC_OP = 0x00000004,
};

//! @}

//! @addtogroup v3d_struct_group
//! @{

//! @struct V3DDeviceCaps
//! @brief �f�o�C�X�̔\��
struct V3DDeviceCaps
{
	V3DFlags flags; //!< �S�ʓI�Ȕ\�͂�\�� \link V3D_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags queryFlags; //!< �N�G���Ɋւ���̔\�͂�\�� \link V3D_QUERY_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags imageFlags; //!< �C���[�W�Ɋւ���̔\�͂�\�� \link V3D_IMAGE_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags shaderFlags; //!< �V�F�[�_�[�Ɋւ���̔\�͂�\�� \link V3D_SHADER_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags rasterizerFlags; //!< ���X�^���C�U�[�Ɋւ���\�͂�\�� \link V3D_RASTERIZER_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags multisampleFlags; //!< �}���`�T���v���Ɋւ���\�͂�\�� \link V3D_MULTISAMPLE_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags depthStencilFlags; //!< �f�v�X�X�e���V���Ɋւ���\�͂�\�� \link V3D_DEPTH_STENCIL_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags colorBlendFlags; //!< �J���[�u�����h�Ɋւ���\�͂�\�� \link V3D_COLOR_BLEND_CAP_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B

	//! @brief 1D�C���[�W�̕��̍ő�l�ł��B
	uint32_t maxImageDimension1D;
	//! @brief 2D�C���[�W�̕��A�����̍ő�l�ł��B
	uint32_t maxImageDimension2D;
	//! @brief 3D�C���[�W�̕��A�����A�[���̍ő�l�ł��B
	uint32_t maxImageDimension3D;
	//! @brief �L���[�u�C���[�W�̕��A�����ő�l�ł��B
	uint32_t maxImageDimensionCube;
	//! @brief �C���[�W�̍ő僌�C���[���ł��B
	uint32_t maxImageArrayLayers;
	//! @brief �e�N�Z���o�b�t�@�[�̗v�f�̍ő吔�ł��B
	uint32_t maxTexelBufferElements;
	//! @brief ���j�t�H�[���o�b�t�@�̍ő�T�C�Y�ł��B
	uint32_t maxUniformBufferSize;
	//! @brief �X�g���[�W�o�b�t�@�̍ő�T�C�Y�ł��B
	uint32_t maxStorageBufferSize;
	//! @brief IV3DCommandBuffer::PushConstant �Ńv�b�V���ł���萔�̍ő�T�C�Y�ł��B( �o�C�g�P�� )
	uint32_t maxPushConstantsSize;
	//! @brief �f�o�C�X���쐬�ł��郊�\�[�X�������̍ő吔�ł��B
	//! @sa IV3DDevice::AllocateResourceMemory
	uint32_t maxResourceMemoryCount;
	//! @brief IV3DDevice::CreateSampler �ō쐬���邱�Ƃ̂ł���T���v���[�̍ő吔�ł��B
	uint32_t maxSamplerCreateCount;
	//! @brief �G�C���A�V���O�̂Ȃ��������������̗אڂ���o�b�t�@�[�܂��͍œK�ȃC���[�W���������邱�Ƃ��ł���I�t�Z�b�g�̗��x���o�C�g�P�ʂŎw�肵�܂��B
	uint64_t bufferImageGranularity;
	//! @brief �p�C�v���C���Őݒ�ł���f�X�N���v�^�Z�b�g�̍ő吔�ł��B
	uint32_t maxBoundDescriptorSets;

	//! @brief �T���v���[�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorSamplers;
	//! @brief ���j�t�H�[���o�b�t�@�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorUniformBuffers;
	//! @brief �X�g���[�W�o�b�t�@�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorStorageBuffers;
	//! @brief �T���v�����O�C���[�W�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorSampledImages;
	//! @brief �X�g���[�W�C���[�W�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorStorageImages;
	//! @brief �C���v�b�g�A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxPerStageDescriptorInputAttachments;
	//! @brief �p�C�v���C�����C�A�E�g�ŃV�F�[�_�X�e�[�W�ɃA�N�Z�X�ł���ő僊�\�[�X���ł��B<br>
	//! �ȉ��̃��\�[�X�ɑ΂��Đ������K�p����܂��B<br>
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
	//! �܂���<br>
	//! <br>
	//! \link V3D_DESCRIPTOR_TYPE_INPUT_ATTACHMENT \endlink<br>
	//! <br>
	//! �܂��t���O�����g�V�F�[�_�X�e�[�W�̏ꍇ�A�t���[���o�b�t�@�̃J���[�A�^�b�`�����g�ɂ����̐������K�p����܂��B
	uint32_t maxPerStageResources;

	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���T���v���[�̍ő吔�ł��B
	uint32_t maxDescriptorSetSamplers;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł��郆�j�t�H�[���o�b�t�@�̍ő吔�ł��B
	uint32_t maxDescriptorSetUniformBuffers;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���_�C�i�~�b�N���j�t�H�[���o�b�t�@�̍ő吔�ł��B
	uint32_t maxDescriptorSetUniformBuffersDynamic;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���X�g���[�W�o�b�t�@�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageBuffers;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���_�C�i�~�b�N�X�g���[�W�o�b�t�@�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageBuffersDynamic;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���T���v�����O�C���[�W�̍ő吔�ł��B
	uint32_t maxDescriptorSetSampledImages;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���X�g���[�W�C���[�W�̍ő吔�ł��B
	uint32_t maxDescriptorSetStorageImages;
	//! @brief �f�X�N���v�^�Z�b�g�Ƀo�C���h�ł���C���v�b�g�A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxDescriptorSetInputAttachments;

	//! @brief ���_�v�f�̍ő吔 ( V3DPipelineVertexInputDesc::elementCount ) �ł��B
	uint32_t maxVertexInputElements;
	//! @brief ��x�Ƀo�C���h�ł��钸�_�o�b�t�@�̍ő吔�ł��B
	uint32_t maxVertexInputBindings;
	//! @brief ���_�v�f�̍ő�I�t�Z�b�g�ł��B( �o�C�g�P�� )
	uint32_t maxVertexInputElementOffset;
	//! @brief ���_���C�A�E�g�̍ő�X�g���C�h ( V3DPipelineVertexLayout::stride ) �ł��B( �o�C�g�P�� )
	uint32_t maxVertexInputBindingStride;
	//! @brief �o�[�e�b�N�X�V�F�[�_�[�ɂ���ďo�͂��邱�Ƃ̂ł���ϐ��̐����̍ő吔�ł��B
	uint32_t maxVertexOutputComponents;

	//! @brief �e�b�Z���[�V�����v���~�e�B�u�W�F�l���[�^�[�̍ő�e�b�Z�[���[�V�����������x���ł��B
	uint32_t maxTessellationGenerationLevel;
	//! @brief �e�b�Z�[���[�V��������V�F�[�_�[����уe�b�Z�[���[�V�����v���~�e�B�u�W�F�l���[�^�[�ŏ����\�ȍő�p�b�`�T�C�Y�ł��B<br>
	//! �O���t�B�b�N�X�p�C�v���C���쐬���Ɏw�肵�� V3DPipelineTessellationDesc �\���̂� patchControlPoints �̒l�ƁA�V�F�[�_�[�� OutputVertices ���s���[�h�Ŏw�肳�ꂽ�l�́A���̐����l�łȂ���΂Ȃ�܂���B
	uint32_t maxTessellationPatchSize;
	//! @brief �e�b�Z���[�V��������V�F�[�_�[�̃X�e�[�W�ɓ��͂ł��钸�_���Ƃ̓��͕ϐ��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxTessellationControlPerVertexInputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�̃X�e�[�W����o�͂ł��钸�_���Ƃ̏o�͕ϐ��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxTessellationControlPerVertexOutputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�̃X�e�[�W����o�͂ł���p�b�`���Ƃ̏o�͕ϐ��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxTessellationControlPerPatchOutputComponents;
	//! @brief �e�b�Z���[�V��������V�F�[�_�̃X�e�[�W����o�͂ł��钸�_���� �y�� �p�b�`���Ƃ̏o�͕ϐ��̍ő升�v���ł��B
	uint32_t maxTessellationControlTotalOutputComponents;
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�̃X�e�[�W�ɓ��͂ł��钸�_���Ƃ̓��͕ϐ��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxTessellationEvaluationInputComponents;
	//! @brief �e�b�Z���[�V�����]���V�F�[�_�̃X�e�[�W����o�͂ł��钸�_���Ƃ̏o�͕ϐ��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxTessellationEvaluationOutputComponents;

	//! @brief �W�I���g���V�F�[�_�ŃT�|�[�g�����ő�Ăяo���񐔂ł��B<br>
	//! �V�F�[�_�[���W���[���� Invocations ���s���[�h�Ŏw�肳�ꂽ�l�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxGeometryShaderInvocations;
	//! @brief �W�I���g���V�F�[�_�̃X�e�[�W�ɓ��͂ł�����͕ϐ��̃R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxGeometryInputComponents;
	//! @brief �W�I���g���V�F�[�_�̃X�e�[�W����o�͂ł���o�͕ϐ��̃R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxGeometryOutputComponents;
	//! @brief �W�I���g���V�F�[�_�ɂ���ďo�͂��邱�Ƃ��ł���ő咸���ł��B
	uint32_t maxGeometryOutputVertices;
	//! @brief �W�I���g���V�F�[�_�̃X�e�[�W����o�͂ł���A���ׂĂ̒��_�ɂ킽��R���|�[�l���g�̍ő吔�ł��B
	uint32_t maxGeometryTotalOutputComponents;

	//! @brief �t���O�����g�V�F�[�_�[�ɓ��͂��邱�Ƃ̂ł���ϐ��̍ő吔�ł��B
	uint32_t maxFragmentInputComponents;
	//! @brief �t���O�����g�V�F�[�_�[���o�͂ł���A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxFragmentOutputAttachments;
	//! @brief �u�����h���L���ŁA�f���A���\�[�X�u�����h���[�h��1���g�p����Ă���Ƃ��ɁA�t���O�����g�V�F�[�_�̃X�e�[�W�ɂ���ď������܂��o�̓A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxFragmentDualSrcAttachments;
	//! @brief �t���O�����g�V�F�[�_�X�e�[�W�Ŏg�p�ł���X�g���[�W�o�b�t�@�A�X�g���[�W�C���[�W�A����яo�̓o�b�t�@�̑����ł��B
	uint32_t maxFragmentCombinedOutputResources;

	//! @brief �R���s���[�g�V�F�[�_�X�e�[�W�̃V�F�[�_�[���W���[���� WorkgroupLocal �X�g���[�W�N���X�Ő錾���ꂽ���ׂĂ̕ϐ��̍ő升�v�X�g���[�W�T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	uint32_t maxComputeSharedMemorySize;
	//! @brief ���� ( x y z ) ���ƂɃf�B�X�p�b�`�ł���O���[�v���ł��B
	uint32_t maxComputeDispatchGroupCount[3];
	//! @brief �P��̃f�B�X�p�b�`�O���[�v�ŌĂяo����R���s���[�g�V�F�[�_�[�̍ő�񐔂ł��B
	uint32_t maxComputeDispatchGroupInvocations;
	//! @brief ���� ( x y z ) ���Ƃ̃f�B�X�p�b�`�O���[�v�̍ő�T�C�Y�ł��B
	uint32_t maxComputeDispatchGroupSize[3];

	//! @brief �t���[���o�b�t�@���W Xf�� Yf �ɂ�����T�u�s�N�Z�����x��\���r�b�g���ł��B
	uint32_t subPixelPrecisionBits;
	//! @brief �k������ъg��t�B���^�Ɏg�p�����摜�̎��ɉ��������Z�ɂ����鐸�x��\���r�b�g���ł��B<br>
	//! ���̒l�̓C���[�W�̊e���ɉ����������̎��ۂ̐��ł��B<br>
	//! �܂��t�B���^�����O���ꂽ���ʂ��v�Z����Ƃ��ɁA�t�B���^�����O�n�[�h�E�F�A�������̏ꏊ�ɃX�i�b�v���܂��B
	uint32_t subTexelPrecisionBits;
	//! @brief �e�~�b�v���x������t�B���^�����O���ʂւ̊�^�����肷��Ƃ��ɁA�t�F�b�`���邽�߂� LOD �v�Z���X�i�b�v����镪���r�b�g���ł��B
	uint32_t mipmapPrecisionBits;

	//! @brief �`��̍ۂɃo�C���h����C���f�b�N�X�o�b�t�@�Ɋi�[�ł���C���f�b�N�X�̍ő�l�ł��B
	uint32_t maxDrawIndexedIndexValue;

	//! @brief LOD �o�C�A�X�̐�Βl�ł��B<br>
	//! V3DSamplerDesc::mipLodBias �̒l�� -maxSamplerLodBias �` +maxSamplerLodBias �̊ԂɃN�����v���܂��B
	float maxSamplerLodBias;
	//! @brief �ٕ�����\�� V3DSamplerDesc::maxAnisotropy �̍ő�l�ł��B
	float maxSamplerAnisotropy;

	//! @brief �r���[�|�[�g�A�V�U�����O�̍ő吔�ł��B
	uint32_t maxViewports;
	//! @brief �r���[�|�[�g�̍ő�T�C�Y�ł��B<br>
	//! maxViewportDimensions[2] �̂��ꂼ��̗v�f�� [0]=���A[1]=������\���܂��B
	uint32_t maxViewportDimensions[2];
	//! @brief �r���[�|�[�g�̊p���܂܂�Ă��Ȃ���΂Ȃ�Ȃ��͈͂ł��B<br>
	//! viewportBoundsRange[2] �̂��ꂼ��̗v�f�� [0]=�ŏ��l�A[1]=�ő�l��\���܂��B
	float viewportBoundsRange[2];
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
	//! @brief InterpolateAtOffset �g�����߂ɑ΂��� x ����� y �I�t�Z�b�g���Œ菬���_�l�Ƃ��Ċۂ߂���T�u�s�N�Z���̕����r�b�g�̐��ł��B
	uint32_t subPixelInterpolationOffsetBits;

	//! @brief �t���[���o�b�t�@�̃A�^�`�����g�̕��̍ő�ł��B
	uint32_t maxFrameBufferWidth;
	//! @brief �t���[���o�b�t�@�̃A�^�`�����g�̍����̍ő�ł��B
	uint32_t maxFrameBufferHeight;
	//! @brief �t���[���o�b�t�@�̃A�^�`�����g�̃��C���[���̍ő�ł��B
	uint32_t maxFrameBufferLayers;

	//! @brief �t���[���o�b�t�@�[�̃J���[�A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferColorSampleCounts;
	//! @brief �t���[���o�b�t�@�[�̃f�v�X�A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferDepthSampleCounts;
	//! @brief �t���[���o�b�t�@�[�̃X�e���V���A�^�b�`�����g�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferStencilSampleCounts;
	//! @brief �A�^�b�`�����g�̂Ȃ��t���[���o�b�t�@�[�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags framebufferNoAttachmentsSampleCounts;

	//! @brief �񐮐��J���[�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageColorSampleCounts;
	//! @brief �����J���[�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageIntegerSampleCounts;
	//! @brief �f�v�X�t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageDepthSampleCounts;
	//! @brief �X�e���V���t�H�[�}�b�g�ō쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags sampledImageStencilSampleCounts;
	//! @brief �X�g���[�W�Ƃ��č쐬���ꂽ 2D �C���[�W�ŃT�|�[�g�����T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	V3DFlags storageImageSampleCounts;

	//! @brief �T�u�p�X�Ŏg�p���邱�Ƃ̂ł���J���[�A�^�b�`�����g�̍ő吔�ł��B
	uint32_t maxColorAttachments;
	//! @brief SampleMask �r���g�C���C���Ő錾���ꂽ�ϐ��̔z��̗v�f�̍ő吔�ł��B
	uint32_t maxSampleMaskWords;

	//! @brief ���ׂẴO���t�B�b�N�X�ƃR���s���[�g�L���[�̃^�C���X�^���v���T�|�[�g���܂��B
	bool timestampComputeAndGraphics;
	//! @brief �^�C���X�^���v�N�G�����C�N���������g���邽�߂ɕK�v�Ȏ��Ԃ��i�m�b�P�ʂŎw�肵�܂��B
	float timestampPeriod;

	//! @brief �V�F�[�_�X�e�[�W�Ŏg�p�ł���ő�N���b�v�����̐��ł��B<br>
	//! �V�F�[�_���W���[���� ClipDistance �g�ݍ��ݏC���Ő錾���ꂽ�z��̃T�C�Y�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxClipDistances;
	//! @brief �V�F�[�_�X�e�[�W�Ŏg�p�ł���ő�̊Ԉ������ł��B<br>
	//! �V�F�[�_���W���[���� CullDistance �g�ݍ��ݑ����Ő錾���ꂽ�z��̃T�C�Y�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxCullDistances;
	//! @brief �V�F�[�_�X�e�[�W�Ŏg�p�ł���N���b�v�ƃJ�����O�����̍��v�ő吔�ł��B<br>
	//! �V�F�[�_���W���[���� ClipDistance ����� CullDistance �r���g�C�������Ő錾���ꂽ�z��̃y�A�̍��v�́A���̐����l�ȉ��łȂ���΂Ȃ�܂���B
	uint32_t maxCombinedClipAndCullDistances;

	//! @brief �T�|�[�g����Ă���|�C���g�̃T�C�Y�͈̔͂ł��B<br>
	//! PointSize �ő������ꂽ�ϐ��ɏ������܂ꂽ�l�́A���� pointSizeRange[0] �` pointSizeRange[1] �̊ԂɃN�����v����܂��B
	float pointSizeRange[2];
	//! @brief �T�|�[�g�����|�C���g�T�C�Y�̗��x�ł��B<br>
	//! pointSizeRange �ɂ���Ē�`���ꂽ�͈͓��̂��ׂẴ|�C���g�T�C�Y���T�|�[�g�����킯�ł͂���܂���B<br>
	//! ���̐����́A�A�����ăT�|�[�g�����|�C���g�T�C�Y�Ԃ̗��x(�܂��͑���) ���w�肵�܂��B
	float pointSizeGranularity;

	//! @brief �f�o�C�X�쐬���Ɏw�肷��L���[�ɃX�P�[�W���[�����O�Ƃ��Ċ��蓖�Ă邱�Ƃ��ł���ʂ̗D�揇�ʂ̐��ł��B
	uint32_t discreteQueuePriorities;
	//! @brief �}���`�T���v�����O�ŋL�q���ꂽ���X�^���C�[�[�V�������W���̃T���v���ʒu���g�p���܂��B<br>
	//! true �ɐݒ肳��Ă���ꍇ�A�����͋L�q���ꂽ�T���v���̏ꏊ���g�p���܂��B<br>
	//! false �ɐݒ肳��Ă���ꍇ�A�����͈قȂ�T���v���ʒu���g�p���邱�Ƃ�����܂��B
	bool standardSampleLocations;
	//! @brief IV3DCommandBuffer::CopyBufferToImage ����� IV3DCommandBuffer::CopyImageToBuffer �̍œK�ȃo�b�t�@�I�t�Z�b�g�A���C�������g�ł��B<br>
	//! �e�N�Z�����Ƃ̃A���C�������g���K�p����܂����A�A�v���P�[�V�����͍œK�ȃp�t�H�[�}���X�Ə���d�͂𓾂邽�߂ɍœK�ȃA���C�����g���g�p����K�v������܂��B
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
	V3DFlags queueFlags; //!< �L���[�̑����\�� \link V3D_QUEUE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
	V3DFlags sampleCountFlags; //!< �C���[�W�̃T���v������\�� \link V3D_SAMPLE_COUNT_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
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
	//! @param[in] family �L���[�t�@�~���[�̃C���f�b�N�X�ł��B
	//! @return �L���[�t�@�~���̋L�q��Ԃ��܂��B
	virtual const V3DDeviceQueueFamilyDesc& GetQueueFamilyDesc(uint32_t family) const = 0;
	//! @brief �L���[���擾���܂��B
	//! @param[in] family �L���[�t�@�~���[�̃C���f�b�N�X�ł��B
	//! @param[in] queue �L���[�̃C���f�b�N�X�ł��B
	//! @param[out] ppQueue �擾�����L���[��n�� IV3DQueue �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue) = 0;

	//! @brief �w�肵���C���[�W�̃t�H�[�}�b�g�̃v���p�e�B���擾���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] type �C���[�W�̃^�C�v�ł��B
	//! @param[in] tiling �C���[�W�̃^�C�����O�ł��B
	//! @param[in] usageFlags �g�p���@��\�� \link V3D_IMAGE_USAGE \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[out] pDesc �擾�����C���[�W�t�H�[�}�b�g�̋L�q���i�[���� V3DImageFormatDesc �\���̂̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @note pProperty �� nullptr ���w�肷�邱�ƂŃt�H�[�}�b�g�̃T�|�[�g�󋵂������m�F���邱�Ƃ��ł��܂��B
	virtual V3D_RESULT GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc) = 0;

	//! @brief �o�b�t�@�[�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �o�b�t�@�[�̃t�H�[�}�b�g�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� \link V3D_BUFFER_FORMAT_FEATURE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief �C���[�W�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� \link V3D_IMAGE_FORMAT_FEATURE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const = 0;
	//! @brief �C���[�W�Ƃ��Ďg�p����t�H�[�}�b�g���w�肵���@�\���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] format �C���[�W�̃t�H�[�}�b�g�ł��B
	//! @param[in] tiling �C���[�W�̃^�C�����O�ł��B
	//! @param[in] featureFlags �m�F����@�\��\�� \link V3D_IMAGE_FORMAT_FEATURE_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_NOT_SUPPORTED
	virtual V3D_RESULT CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const = 0;

	//! @brief �R�}���h�v�[���[���쐬���܂��B
	//! @param[in] desc �R�}���h�v�[���̋L�q�ł��B
	//! @param[out] ppCommandPool �쐬�����R�}���h�v�[����n�� IV3DCommandPool �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool) = 0;
	//! @brief �R�}���h�o�b�t�@�[���쐬���܂��B
	//! @param[in] pPool �R�}���h�v�[���ł��B
	//! @param[in] bufferType �R�}���h�o�b�t�@�[�̃^�C�v�ł��B
	//! @param[out] ppCommandBuffer �쐬�����R�}���h�o�b�t�@�[��n�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateCommandBuffer(IV3DCommandPool* pPool, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer) = 0;
	//! @brief �R�}���h�o�b�t�@�[���쐬���܂��B
	//! @param[in] poolDesc �R�}���h�v�[���̋L�q�ł��B
	//! @param[in] bufferType �R�}���h�o�b�t�@�[�̃^�C�v�ł��B
	//! @param[out] ppCommandBuffer �쐬�����R�}���h�o�b�t�@�[��n�� IV3DCommandBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note �R�}���h�v�[���������ɍ쐬���܂��B
	virtual V3D_RESULT CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer) = 0;

	//! @brief �X���b�v�`�F�C�����쐬���܂��B
	//! @param[in] swapChainDesc �X���b�v�`�F�C���̋L�q�ł��B
	//! @param[in] swapChainCallbacks �X���b�v�`�F�C���̃R�[���o�b�N�ł��B
	//! @param[out] ppSwapChain �쐬�����X���b�v�`�F�C����n�� IV3DSwapChain �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @retval V3D_ERROR_DEVICE_LOST
	//! @note
	//! V3DSwapChainDesc �\���̂� imageFormat �Ŏw�肵���t�H�[�}�b�g��������Ȃ������ꍇ�́A�����Ńt�H�[�}�b�g�����肵�܂��B( \link V3D_FORMAT_UNDEFINED \endlink ���w��ł���Ƃ������Ƃ�\���܂� )<br>
	//! IV3DSwapChain::GetDesc �Ŏ擾�ł���L�q�́A�쐬���Ɏw�肵�����̂Ƃ͈قȂ�ꍇ�����邽�߁A�K�v�ɉ����Ď擾���Ȃ����Ă��������B<br>
	//! �܂� IV3DSwapChain::GetImage �Ŏ擾�ł���X���b�v�`�F�C���쐬����̃C���[�W�͈ȉ��̂悤�Ȑݒ�ɂȂ��Ă��܂��B<br>
	//! <table>
	//! <tr><td>���C�A�E�g</td><td>\link V3D_IMAGE_LAYOUT_UNDEFINED \endlink</td></tr>
	//! <tr><td>�g�p���@</td><td>\link V3D_IMAGE_USAGE_TRANSFER_SRC \endlink<br>\link V3D_IMAGE_USAGE_TRANSFER_DST \endlink<br>\link V3D_IMAGE_USAGE_COLOR_ATTACHMENT \endlink</td></tr>
	//! </table>
	virtual V3D_RESULT CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain) = 0;

	//! @brief �t�F���X���쐬���܂��B
	//! @param[out] ppFence �쐬�����t�F���X��n�� IV3DFence �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFence(IV3DFence** ppFence) = 0;

	//! @brief �N�G���v�[�����쐬���܂��B
	//! @param[in] desc �N�G���v�[���̋L�q�ł��B
	//! @param[out] ppQueryPool �쐬�����N�G���v�[����n�� IV3DQueryPool �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool) = 0;

	//! @brief �T���v���[���쐬���܂��B
	//! @param[in] desc �T���v���[�̋L�q�ł��B
	//! @param[out] ppSampler �쐬�����T���v���[��n�� IV3DSampler �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler) = 0;

	//! @brief �V�F�[�_�[���W���[�����쐬���܂��B
	//! @param[in] codeSize �R�[�h�̃T�C�Y�ł��B
	//! @param[in] pCode �V�F�[�_�̃R�[�h�ł��B
	//! @param[out] ppShaderModule �쐬�����V�F�[�_�[���W���[����n�� IV3DShaderModule �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule) = 0;

	//! @brief �w�肵�����������������\�[�X���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�̃�����������\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pResource �������������m�F���郊�\�[�X��\�� IV3DResource �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::AllocateResourceMemoryAndBind
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const = 0;
	//! @brief �w�肵�����������������\�[�X���T�|�[�g���Ă��邩�ǂ������m�F���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�̃�����������\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] resourceCount �������������m�F���郊�\�[�X�̐��ł��B
	//! @param[in] ppResources resourceCount �̒l�̐��̗v�f������ IV3DResource �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_NOT_SUPPORTED
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::AllocateResourceMemoryAndBind
	virtual V3D_RESULT CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const = 0;

	//! @brief ���\�[�X���������m�ۂ��܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] memorySize �m�ۂ��郁�����̃T�C�Y���o�C�g�P�ʂŎw�肵�܂��B
	//! @param[out] ppResourceMemory �m�ۂ������\�[�X��������n�� IV3DResourceMemory �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! IV3DDevice::CreateBuffer �܂��� IV3DDevice::CreateImage �ɂ���č쐬���ꂽ����̃��\�[�X�̃������͂܂��m�ۂ���Ă��炸�A�g�p�ł��Ȃ���ԂɂȂ��Ă��܂��B<br>
	//! ���̂��� IV3DDevice::AllocateResourceMemory �Ń��������m�ۂ����� IV3DDevice::BindResourceMemory �Ń��\�[�X���������Ƀo�C���h����K�v������܂��B<br>
	//! �܂��m�ۂ��邱�Ƃ̂ł��郊�\�[�X�������� V3DDeviceCaps �\���̂� maxResourceMemoryCount �̒l�̐��ɐ�������܂��B<br>
	//! <ul>
	//!   <li>����������</li><br>
	//!   memoryPropertyFlags �Ɏw��ł���t���O�̑g�ݍ��킹�͈ȉ��̂��̂Ɍ��肳��܂��B<br>
	//!   �܂��A�����̑g�ݍ��킹���K�������g����킯�ł͂Ȃ����߁A���\�[�X���������m�ۂ���O�� IV3DDevice::CheckResourceMemoryProperty �Ŏg�p�\���ǂ����𒲂ׂĂ��������B<br>
	//!   <br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink ��<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink ��<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink ��<br>
	//!   \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_LAZILY_ALLOCATED \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink<br>
	//!   \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink | \link V3D_MEMORY_PROPERTY_HOST_COHERENT \endlink | \link V3D_MEMORY_PROPERTY_HOST_CACHED \endlink<br>
	//!   <br>
	//!   �� \link V3D_MEMORY_PROPERTY_DEVICE_LOCAL \endlink | \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink �̑g�ݍ��킹�́A�قڎg���Ȃ��ƍl�����ق����悳�����ł��B<br>
	//!   <br>
	//!   <li>�������T�C�Y</li><br>
	//!   ���\�[�X�̃������T�C�Y�� IV3DResource::GetResourceDesc �Ŏ擾�ł��� V3DResourceDesc �\���̂̃����o�ł��� memorySize �Ŋm�F���邱�Ƃ��ł��܂��B<br>
	//! </ul>
	virtual V3D_RESULT AllocateResourceMemory(V3DFlags memoryPropertyFlags, uint64_t memorySize, IV3DResourceMemory** ppResourceMemory) = 0;
	//! @brief ���\�[�X���������Ƀo�C���h���܂��B
	//! @param[in] pResourceMemory �o�C���h��̃������ł��B
	//! @param[in] pResource �o�C���h���郊�\�[�X�ł��B
	//! @param[in] memoryOffset �o�C���h���郁�����̃I�t�Z�b�g���o�C�g�P�ʂŎw�肵�܂��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! ���\�[�X���������Ƀo�C���h���邱�Ƃɂ���ă������ւ̃A�N�Z�X���ł���悤�ɂȂ�܂��B<br>
	//! �܂���x�������Ƀo�C���h����ƃ��\�[�X����������܂Ńo�C���h���������邱�Ƃ͂ł��܂���B<br>
	//! <ul>
	//!   <li>�������̃I�t�Z�b�g</li><br>
	//!   memoryOffset �̓��\�[�X�̎�ނɂ���ăA���C�����g�̐��������邽�߁A�ȉ��̃����o�̐����{�̒l���w�肷��K�v������܂��B<br>
	//!   <br>
	//!   <ul>
	//!     <li>�e�N�Z���o�b�t�@�[</li> \link V3DDeviceCaps::minTexelBufferOffsetAlignment \endlink<br>
	//!     <li>���j�t�H�[���o�b�t�@�[</li> \link V3DDeviceCaps::minUniformBufferOffsetAlignment \endlink<br>
	//!     <li>�X�g���[�W�o�b�t�@�[</li> \link V3DDeviceCaps::minStorageBufferOffsetAlignment \endlink<br>
	//!   </ul>
	//!   <br>
	//!   ����Ƀz�X�g����A�N�Z�X�ł��郁���� \link V3D_MEMORY_PROPERTY_HOST_VISIBLE \endlink �ɂ��A���C�����g�̐��������邽�� \link V3DDeviceCaps::minMemoryMapAlignment \endlink ���l������K�v������܂��B<br>
	//! </ul>
	virtual V3D_RESULT BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset) = 0;

	//! @brief ���\�[�X�̃��������m�ۂ��A�o�C���h���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] pResource �o�C���h���郊�\�[�X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, IV3DResource* pResource) = 0;
	//! @brief �����̃��\�[�X����̃������Ƃ��Ċm�ۂ��A�o�C���h���܂��B
	//! @param[in] memoryPropertyFlags ���\�[�X�������̓�����\�� \link V3D_MEMORY_PROPERTY_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	//! @param[in] resourceCount �o�C���h���郊�\�[�X�̐��ł��B
	//! @param[in] ppResources �o�C���h���郊�\�[�X��\�� IV3DResource �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B<br>
	//! �z��̗v�f�̐��� resourceCount �̒l�ł���K�v������܂��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT AllocateResourceMemoryAndBind(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) = 0;

	//! @brief �o�b�t�@�[���쐬���܂��B
	//! @param[in] subresourceCount �o�b�t�@�[�̃T�u���\�[�X�̐��ł��B
	//! @param[in] pSubresources subresourceCount �̒l�̐��̗v�f������ V3DBufferSubresourceDesc �\���̂̔z��ł��B
	//! @param[in] ppBuffer �쐬�����o�b�t�@�[��n�� IV3DBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! �e�T�u���\�[�X�̎g�p���@ ( V3DBufferSubresourceDesc::usageFlags ) �ɂ�� IV3DResource::Map �̃I�t�Z�b�g�̃A���C�����g�̉e�����󂯂邽�߁A
	//! �쐬�����o�b�t�@�̃T�C�Y���w�肵���T�u���\�[�X�̃T�C�Y�̍��v�����傫���Ȃ�ꍇ������܂��B
	//! @sa V3DDeviceCaps::minMemoryMapAlignment
	//! @sa V3DDeviceCaps::minTexelBufferOffsetAlignment
	//! @sa V3DDeviceCaps::minUniformBufferOffsetAlignment
	//! @sa V3DDeviceCaps::minStorageBufferOffsetAlignment
	virtual V3D_RESULT CreateBuffer(uint32_t subresourceCount, const V3DBufferSubresourceDesc* pSubresources, IV3DBuffer** ppBuffer) = 0;
	//! @brief �C���[�W���쐬���܂��B
	//! @param[in] imageDesc �C���[�W�̋L�q�ł��B
	//! @param[in] initialLayout �C���[�W�̏������C�A�E�g�ł��B
	//! @param[out] ppImage �쐬�����C���[�W��n�� IV3DImage �C���^�[�t�F�[�X�̃|�C���^�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note �C���[�W�̏������C�A�E�g�Ƃ��Ďw��ł���̂� \link V3D_IMAGE_LAYOUT_UNDEFINED \endlink �܂��� \link V3D_IMAGE_LAYOUT_PREINITIALIZED \endlink �݂̂ɂȂ�܂��B<br>
	virtual V3D_RESULT CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage) = 0;

	//! @brief �o�b�t�@�[�r���[���쐬���܂��B
	//! @param[in] pBuffer �A�N�Z�X��̃o�b�t�@�[�ł��B
	//! @param[in] subresource �A�N�Z�X��o�b�t�@�[�̃T�u���\�[�X�̃C���f�b�N�X�ł��B
	//! @param[in] format �o�b�t�@�[�r���[�̃t�H�[�}�b�g�ł��B
	//! @param[out] ppBufferView �쐬�����o�b�t�@�[�r���[��\�� IV3DBufferView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval D3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! ���O�Ƀo�b�t�@�[�̃��������m�ۂ��Ă����K�v������܂��B<br>
	//! format �͎w�肵�� subresource �̎g�p���@�� \link V3D_BUFFER_USAGE_UNIFORM_TEXEL \endlink �������� \link V3D_BUFFER_USAGE_STORAGE_TEXEL \endlink ���w�肳��Ă���ꍇ�ɂ̂ݗL���ł��B<br>
	//! ����ȊO�̏ꍇ�� \link V3D_FORMAT_UNDEFINED \endlink ���w�肵�Ă��������B
	//! @sa IV3DDevice::CreateBuffer
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT CreateBufferView(IV3DBuffer* pBuffer, uint32_t subresource, V3D_FORMAT format, IV3DBufferView** ppBufferView) = 0;
	//! @brief �C���[�W�r���[���쐬���܂��B
	//! @param[in] pImage �A�N�Z�X����C���[�W�ł��B
	//! @param[in] desc �C���[�W�r���[�̋L�q�ł��B
	//! @param[out] ppImageView �쐬�����C���[�W�r���[��n�� IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval D3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	//! @note
	//! ���O�ɃC���[�W�̃��������m�ۂ��Ă����K�v������܂��B
	//! @sa IV3DDevice::CreateImage
	//! @sa IV3DDevice::AllocateResourceMemory
	//! @sa IV3DDevice::BindResourceMemory
	virtual V3D_RESULT CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView) = 0;

	//! @brief �����_�[�p�X���쐬���܂��B
	//! @param[in] attachmentCount �A�^�b�`�����g�̐��ł��B
	//! @param[in] pAttachments attachmentCount �̒l�̐��̗v�f������ V3DAttachmentDesc �\���̂̔z��ł��B
	//! @param[in] subpassCount �T�u�p�X�̐��ł��B
	//! @param[in] pSubpasses subpassCount �̒l�̐��̗v�f������ V3DSubpassDesc �\���̂̔z��ł��B
	//! @param[in] subpassDependencyCount �T�u�p�X�̈ˑ����̐��ł��B
	//! @param[in] pSubpassDependencies subpassDependencyCount �̒l�̐��̗v�f������ V3DSubpassDependencyDesc �\���̂̔z��ł��B
	//! @param[out] ppRenderPass �쐬���������_�[�p�X��\�� IV3DRenderPass �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
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

	//! @brief �t���[���o�b�t�@���쐬���܂��B
	//! @param[in] pRenderPass �����_�[�p�X
	//! @param[in] attachmentCount �A�^�b�`�����g�̐��ł��B
	//! @param[in] ppAttachments attachmentCount �̒l�̐��̗v�f������ IV3DImageView �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @param[out] ppFrameBuffer �쐬�����t���[���o�b�t�@��n�� IV3DFrameBuffer �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateFrameBuffer(IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���C�A�E�g���쐬���܂��B
	//! @param[in] descriptorCount �f�X�N���v�^�̐��ł��B
	//! @param[in] pDescriptors descriptorCount �̒l�̐��̗v�f������ V3DDescriptorDesc �\���̂̔z��ł��B
	//! @param[in] poolSize �f�X�N���v�^�Z�b�g�̃v�[���̏����T�C�Y�ł��B
	//! @param[in] poolResizeStep �f�X�N���v�^�Z�b�g�̃v�[���̃��T�C�Y�X�e�b�v�ł��B
	//! @param[out] ppDescriptorSetLayout �쐬�����f�X�N���v�^�Z�b�g���C�A�E�g��n�� IV3DDescriptorSetLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateDescriptorSetLayout(
		uint32_t descriptorCount, const V3DDescriptorDesc* pDescriptors,
		uint32_t poolSize, uint32_t poolResizeStep,
		IV3DDescriptorSetLayout** ppDescriptorSetLayout) = 0;

	//! @brief �f�X�N���v�^�Z�b�g���쐬���܂��B
	//! @param[in] pLayout �f�X�N���v�^�Z�b�g�̃��C�A�E�g�̃|�C���^�ł��B
	//! @param[out] ppDescriptorSet �쐬�����f�X�N���v�^�Z�b�g��n�� IV3DDescriptorSet �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet) = 0;

	//! @brief �p�C�v���C�����C�A�E�g���쐬���܂��B
	//! @param[in] constantCount �萔�̐��ł��B
	//! @param[in] pConstants constantCount �̒l�̐������v�f������ V3DConstantDesc �\���̂̔z��ł��B
	//! @param[in] descriptorSetLayoutCount �f�X�N���v�^�Z�b�g���C�A�E�g�̐��ł��B
	//! @param[in] ppDescriptorSetLayouts �f�X�N���v�^���C�A�E�g��\�� descriptorSetLayoutCount �̒l�̐������v�f������ IV3DDescriptorSetLayout �C���^�[�t�F�[�X�̃|�C���^�̔z��ł��B
	//! @param[out] ppPipelineLayout �쐬�����p�C�v���C�����C�A�E�g��\�� IV3DPipelineLayout �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreatePipelineLayout(
		uint32_t constantCount, V3DConstantDesc* pConstants,
		uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts,
		IV3DPipelineLayout** ppPipelineLayout) = 0;

	//! @brief �O���t�B�b�N�X�p�C�v���C�����쐬���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] pipelineDesc �O���t�B�b�N�X�p�C�v���C���̋L�q�ł��B
	//! @param[out] ppPipeline �쐬�����O���t�B�b�N�X�p�C�v���C����n�� IV3DPipeline �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline) = 0;

	//! @brief �R���s���[�g�p�C�v���C�����쐬���܂��B
	//! @param[in] pPipelineLayout �p�C�v���C�����C�A�E�g�ł��B
	//! @param[in] pipelineDesc �R���s���[�g�p�C�v���C���̋L�q�ł��B
	//! @param[out] ppPipeline �쐬�����R���s���[�g�p�C�v���C����n�� IV3DPipeline �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	//! @retval V3D_OK
	//! @retval V3D_ERROR_FAIL
	//! @retval V3D_ERROR_INVALID_ARGUMENT
	//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
	//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
	virtual V3D_RESULT CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline) = 0;

	//! @brief �f�o�C�X���A�C�h����ԂɂȂ�܂őҋ@���܂�
	//! @retval V3D_OK �f�o�C�X���A�C�h����ԂɂȂ���
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

//! @enum V3D_LAYER_TYPE
//! @brief ���C���[�̃^�C�v
enum V3D_LAYER_TYPE : uint8_t
{
	//! @brief VK_LAYER_LUNARG_standard_validation ���g�p���܂��B<br>
	//! �f�o�b�O���͒ʏ킱����g�p���Ă��������B
	V3D_LAYER_STANDARD_VALIDATION = 0,
	//! @brief �œK�ȃ��C���[���g�p���܂��B
	V3D_LAYER_OPTIMUS = 1,
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

	V3D_LOG_STANDARD = V3D_LOG_WARNING | V3D_LOG_ERROR, //!< �W���̃��O��\�\���܂��B
	V3D_LOG_ALL = V3D_LOG_INFORMATION | V3D_LOG_WARNING | V3D_LOG_PERFORMANCE_WARNING | V3D_LOG_ERROR | V3D_LOG_DEBUG, //!< ���ׂẴ��O��\�\���܂��B
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
	V3DFlags flags; //!< ���O�̃t���O��\�� \link V3D_LOG_FLAG \endlink �񋓒萔�̑g�ݍ��킹�ł��B
	PV3DLogFunction pFunction; //!< ���O���o�͂���֐��̃|�C���^�ł��B
	void* pUserData; //!< �R�[���o�b�N�֐��ɓn����郆�[�U�[�f�[�^�ł��B
};

//! @struct V3DInstanceDesc
//! @brief �C���X�^���X�̋L�q
struct V3DInstanceDesc
{
	V3D_LAYER_TYPE layerType; //!< ���C���[�̃^�C�v��\�� \link V3D_LAYER_TYPE \endlink �񋓒萔�̂����ꂩ���w�肵�܂��B
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
	//! @brief ���C���[�̃^�C�v���擾���܂��B
	//! @return ���C���[�̃^�C�v��Ԃ��܂��B
	virtual V3D_LAYER_TYPE GetLayerType() const = 0;

	//! @brief �A�_�v�^�̐����擾���܂��B
	//! @return �A�_�v�^�̐���Ԃ��܂��B
	virtual uint32_t GetAdapterCount() const = 0;
	//! @brief �A�_�v�^���擾���܂��B
	//! @param[in] adapterIndex �A�_�v�^�̃C���f�b�N�X�ł��B
	//! @param[out] ppAdapter �擾�����A�_�v�^��n�� IV3DAdapter �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
	virtual void GetAdapter(uint32_t adapterIndex, IV3DAdapter** ppAdapter) = 0;

	//! @brief �f�o�C�X���쐬���܂��B
	//! @param[in] pAdapter �A�_�v�^�ł��B
	//! @param[out] ppDevice �쐬�����f�o�C�X��n�� IV3DDevice �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
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

//! @brief �C���X�^���X���쐬���܂��B
//! @param[in] instanceDesc �C���X�^���X�̋L�q��\�� V3DInstanceDesc �\���̂ł��B
//! @param[out] ppInstance �쐬�����C���X�^���X��n�� IV3DInstance �C���^�[�t�F�[�X�̃|�C���^�̃A�h���X�ł��B
//! @retval V3D_OK
//! @retval V3D_ERROR_FAIL
//! @retval V3D_ERROR_INVALID_ARGUMENT
//! @retval V3D_ERROR_OUT_OF_HOST_MEMORY
//! @retval V3D_ERROR_OUT_OF_DEVICE_MEMORY
V3D_DLL_API V3D_RESULT V3DCreateInstance(const V3DInstanceDesc& instanceDesc, IV3DInstance** ppInstance);

//! @}