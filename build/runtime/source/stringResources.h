#pragma once

// ----------------------------------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidArgument = L" : �s���Ȉ������n����܂��� :";

// ----------------------------------------------------------------------------------------------------
// IV3DDevice
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidSubpass = L"IV3DDevice::CreateGraphicsPipeline : �w�肳�ꂽ�T�u�p�X %d �͖����ł��B";
static constexpr wchar_t* Log_Error_MismatchSubpassColorAttachmentCount = L"IV3DDevice::CreateGraphicsPipeline : �T�u�p�X�̃J���[�A�^�b�`�����g�̐�����v���܂���B";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentCount = L"IV3DDevice::CreateFrameBuffer : �w�肳�ꂽ�����_�[�p�X�ƍ쐬���悤�Ƃ��Ă��郌���_�[�A�^�b�`�����g�R���e�i�̃A�^�b�`�����g�̐�����v���܂���B";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentSize = L"IV3DDevice::CreateFrameBuffer : �S�ẴA�^�b�`�����g�� ���A�����A���C���[���������łȂ��Ƃ����܂���B";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentFormatOrSamples = L"IV3DDevice::CreateFrameBuffer : �A�^�b�`�����g %d �̃t�H�[�}�b�g��������T���v��������v���܂���B";
static constexpr wchar_t* Log_Error_OutOfInputAttachment = L"IV3DDevice::CreateRenderPass : �C���v�b�g�A�^�b�`�����g ( [%u].attachment %u ) �̓A�^�b�`�����g�̐� ( %u ) �𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_OutOfColorAttachment = L"IV3DDevice::CreateRenderPass : �J���[�A�^�b�`�����g ( [%u].attachment %u ) �̓A�^�b�`�����g�̐� ( %u ) �𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_OutOfResolveAttachment = L"IV3DDevice::CreateRenderPass : ���]���u�A�^�b�`�����g ( [%u].attachment %u ) �̓A�^�b�`�����g�̐� ( %u ) �𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_MismatchResolveAttachmentSamples = L"IV3DDevice::CreateRenderPass : ���]���u�A�^�b�`�����g ( [%u].attachment %u ) �̃T���v������ V3D_SAMPLE_COUNT_1 �ɂ��Ă��������B";
static constexpr wchar_t* Log_Error_AlreadyBindResourceMemory = L"IV3DDevice::BindResourceMemory : ���\�[�X %s[%u] �͊��Ƀ��������m�ۂ��A�o�C���h����Ă��܂��B";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandBuffer
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : �v���C�}���R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肷�邱�Ƃ͂ł��܂���B";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : �R�}���h�o�b�t�@�[�ւ̋L�^�͊��ɊJ�n����Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : �R�}���h�o�b�t�@�[�͋L�^�͊��ɏI������Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"�R�}���h�o�b�t�@�[���J�n����Ă��Ȃ��̂ɃR�}���h�������������Ƃ��܂����B";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : �R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肵���ꍇ�� pRenderPass �� subpass �͕K���w�肵�Ă��������A";
static constexpr wchar_t* Log_Error_BarrierBufferOverflow = L"IV3DCommandBuffer::BarrierBuffer : �o�b�t�@�[�̃������͈̔͊O�Ƀo���A�𒣂낤�Ƃ��܂����B";
static constexpr wchar_t* Log_Error_CopyImageLayout = L"IV3DCommandBuffer::CopyImage : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_MismatchCopyImage = L"IV3DCommandBuffer::ResolveImage : �R�s�[��ƃR�s�[���̃C���[�W����v���܂���B";
static constexpr wchar_t* Log_Error_BlitImageLayout = L"IV3DCommandBuffer::BlitImage : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_BlitImageSrcSamples = L"IV3DCommandBuffer::BlitImage : �]����Ɠ]�����̃T���v��������v���܂���B IV3DCommandBuffer::ResolveImage ���g�p���Ă��������B";
static constexpr wchar_t* Log_Error_ResolveImageLayout = L"IV3DCommandBuffer::ResolveImage : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_ResolveImageDstSamples1 = L"IV3DCommandBuffer::ResolveImage : �ϊ���̃C���[�W�̃T���v������ V3D_SAMPLE_COUNT_1 �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_ResolveImageViewLayout = L"IV3DCommandBuffer::ResolveImageView : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_ResolveImageViewDstSamples1 = L"IV3DCommandBuffer::ResolveImageView : �ϊ���̃C���[�W�̃T���v������ V3D_SAMPLE_COUNT_1 �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_MismatchResolveImage = L"IV3DCommandBuffer::ResolveImage : �ϊ���ƕϊ����̃C���[�W����v���܂���B";
static constexpr wchar_t* Log_Error_MismatchResolveImageView = L"IV3DCommandBuffer::ResolveImageView : �ϊ���ƕϊ����̃C���[�W����v���܂���B";
static constexpr wchar_t* Log_Error_ClearImageLayout = L"IV3DCommandBuffer::ClearImage : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_ClearImageViewLayout = L"IV3DCommandBuffer::ClearImageView : %s �� %s �������� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_NoneClearAttachments = L"IV3DCommandBuffer::ClearAttachments : �N���A����A�^�b�`�����g���w�肳��Ă��܂��� :";
static constexpr wchar_t* Log_Error_NotVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer : �o�C���f�B���O %u �̓o�[�e�b�N�X�o�b�t�@�r���[�ł͂���܂���B";
static constexpr wchar_t* Log_Error_OutOfVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer : �o�C���f�B���O %u �̃I�t�Z�b�g %I64u �͔͈͊O�������Ă��܂��B";
static constexpr wchar_t* Log_Error_NotIndexBufferView = L"IV3DCommandBuffer::BindIndexBufferView : %s �C���f�b�N�X�o�b�t�@�r���[�ł͂���܂���B";
static constexpr wchar_t* Log_Error_OutOfIndexBuffer = L"IV3DCommandBuffer::BindIndexBuffer : �I�t�Z�b�g %I64u �͔͈͊O�������Ă��܂��B";
static constexpr wchar_t* Log_Error_WriteTimeStampQueryType = L"IV3DCommandBuffer::WriteTimestamp : �N�G���� %s �ł���K�v������܂��B";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : �R�}���h�o�b�t�@ %s[%u] �̓Z�J���_���ł͂���܂���B";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffers : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandQueue
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : �R�}���h�o�b�t�@ %s[%u] �̓v���C�}���ł͂���܂���B";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";

// ----------------------------------------------------------------------------------------------------
// V3DDescriptorSet
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : �o�C���f�B���O %u �Ƀ��\�[�X ( %s ) ���Z�b�g����Ă��܂���B";
