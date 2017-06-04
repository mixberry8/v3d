#pragma once

// ----------------------------------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidArgument = L" : �s���Ȉ������n����܂��� :";

// ----------------------------------------------------------------------------------------------------
// IV3DDevice
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidSubpass = L"IV3DDevice::CreateGraphicsPipeline : %s : �w�肳�ꂽ�T�u�p�X %u �͔͈͂𒴂��Ă��܂��B";
static constexpr wchar_t* Log_Error_MismatchSubpassColorAttachmentCount = L"IV3DDevice::CreateGraphicsPipeline : %s : �w�肳�ꂽ�����_�[�p�X %s �̃T�u�p�X %u �̃J���[�A�^�b�`�����g�̐�����v���܂���B";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentCount = L"IV3DDevice::CreateFrameBuffer : %s : �w�肳�ꂽ�����_�[�p�X %s �ƍ쐬���悤�Ƃ��Ă���t���[���o�b�t�@�[�̃A�^�b�`�����g�̐�����v���܂���B";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentSize = L"IV3DDevice::CreateFrameBuffer : %s : �S�ẴA�^�b�`�����g�� ���A�����A�[���A���C���[���������łȂ��Ƃ����܂���B";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentFormatOrSamples = L"IV3DDevice::CreateFrameBuffer : %s : �A�^�b�`�����g %u �̃t�H�[�}�b�g��������T���v��������v���܂���B";
static constexpr wchar_t* Log_Error_OutOfInputAttachment = L"IV3DDevice::CreateRenderPass : %s : �T�u�p�X %u �̃C���v�b�g�A�^�b�`�����g ( pInputAttachments[%u].attachment = %u < %u ) ���͈͂𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_OutOfColorAttachment = L"IV3DDevice::CreateRenderPass : %s : �T�u�p�X %u �̃J���[�A�^�b�`�����g ( pColorAttachments[%u].attachment = %u < %u ) ���͈͂𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_OutOfResolveAttachment = L"IV3DDevice::CreateRenderPass : %s : �T�u�p�X %u �̃��]���u�A�^�b�`�����g ( pResolveAttachments[%u].attachment = %u < %u ) ���͈͂𒴂��Ďw�肳��Ă��܂��B";
static constexpr wchar_t* Log_Error_MismatchResolveAttachmentSamples = L"IV3DDevice::CreateRenderPass : %s : �T�u�p�X %u �̃��]���u�A�^�b�`�����g ( pResolveAttachments[%u].attachment = %u ) �̃T���v������ V3D_SAMPLE_COUNT_1 �ɂ��Ă��������B";
static constexpr wchar_t* Log_Error_AlreadyBindResourceMemory = L"IV3DDevice::BindResourceMemory : %s : ���\�[�X %s[%u] = %s �͊��Ƀ��������m�ۂ��A�o�C���h����Ă��܂��B";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandBuffer
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : �v���C�}���R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肷�邱�Ƃ͂ł��܂���B";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : %s : �R�}���h�o�b�t�@�[�ւ̋L�^�͊��ɊJ�n����Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : %s : �R�}���h�o�b�t�@�[�͋L�^�͊��ɏI������Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"IV3DCommandBuffer : %s : �R�}���h�o�b�t�@�[���J�n����Ă��Ȃ��̂ɃR�}���h�������������Ƃ��܂����B";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : �R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肵���ꍇ�� pRenderPass �� subpass �͕K���w�肵�Ă��������A";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : �R�}���h�o�b�t�@ %s[%u] �̓Z�J���_���ł͂���܂���B";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";
static constexpr wchar_t* Log_Error_CommandBuffersSimultaneousUse = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : �v���C�}���R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE ���w�肳��Ă���ꍇ�́A�Z�J���_���R�}���h�o�b�t�@�[�����l�ɂ��̎g�p�@���w�肷��K�v������܂��B";
static constexpr wchar_t* Log_Error_UnavailablePushDescriptorSets = L"IV3DCommandBuffer::PushDescriptorSets : %s : �g���@�\���L���łȂ����� PushDescriptorSets �͎g�p�ł��܂���B";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandQueue
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : %s : �R�}���h�o�b�t�@ %s[%u] �̓v���C�}���ł͂���܂���B";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : %s : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";

// ----------------------------------------------------------------------------------------------------
// V3DDescriptorSet
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : %s : �o�C���f�B���O %u �Ƀ��\�[�X ( %s ) ���Z�b�g����Ă��܂���B";
