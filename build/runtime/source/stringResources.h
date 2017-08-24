#pragma once

// ----------------------------------------------------------------------------------------------------
// ���O
// ----------------------------------------------------------------------------------------------------

#ifdef V3D_DEBUG

/********/
/* ���� */
/********/

static constexpr wchar_t* Log_Error_InvalidArgument = L" : �s���Ȉ������n����܂��� :";

/****************/
/* IV3DInstance */
/****************/

static constexpr char* Log_Debug_InstanceLayer = "Instance : Layer %s";
static constexpr char* Log_Debug_InstanceExtension = "Instance : Extension %s";

/**************/
/* IV3DDevice */
/**************/

static constexpr wchar_t* Log_Error_MixingDifferentDescSetLayout = L"IV3DDevice::CreatePipelineLayout : %s : �قȂ�^�C�v�̃f�X�N���v�^�Z�b�g���C�A�E�g�����݂��Ă��܂��B�^�C�v�͂��ׂē����ł���K�v������܂��B";
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

static constexpr char* Log_Debug_DeviceLayer = "%s : Layer %s";
static constexpr char* Log_Debug_DeviceExtension = "%s : Extension %s";

/*********************/
/* IV3DCommandBuffer */
/*********************/

static constexpr wchar_t* Log_IV3DCommandBuffer_Barrier = L"IV3DCommandBuffer::Barrier";
static constexpr wchar_t* Log_IV3DCommandBuffer_ResetEvent = L"IV3DCommandBuffer::ResetEvent";
static constexpr wchar_t* Log_IV3DCommandBuffer_SetEvent = L"IV3DCommandBuffer::SetEvent";
static constexpr wchar_t* Log_IV3DCommandBuffer_WaitEvent = L"IV3DCommandBuffer::WaitEvents";
static constexpr wchar_t* Log_IV3DCommandBuffer_FillBuffer = L"IV3DCommandBuffer::FillBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_UpdateBuffer = L"IV3DCommandBuffer::UpdateBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_CopyBuffer = L"IV3DCommandBuffer::CopyBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_CopyImage = L"IV3DCommandBuffer::CopyImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_CopyBufferToImage = L"IV3DCommandBuffer::CopyBufferToImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_CopyImageToBuffer = L"IV3DCommandBuffer::CopyImageToBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_BlitImage = L"IV3DCommandBuffer::BlitImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_BlitImageView = L"IV3DCommandBuffer::BlitImageView";
static constexpr wchar_t* Log_IV3DCommandBuffer_ResolveImage = L"IV3DCommandBuffer::ResolveImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_ResolveImageView = L"IV3DCommandBuffer::ResolveImageView";
static constexpr wchar_t* Log_IV3DCommandBuffer_BeginRenderPass = L"IV3DCommandBuffer::BeginRenderPass";
static constexpr wchar_t* Log_IV3DCommandBuffer_ClearImage = L"IV3DCommandBuffer::ClearImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_ClearImageView = L"IV3DCommandBuffer::ClearImageView";
static constexpr wchar_t* Log_IV3DCommandBuffer_ClearAttachment = L"IV3DCommandBuffer::ClearAttachments";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindPipeline = L"IV3DCommandBuffer::BindPipeline";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindDescriptorSet = L"IV3DCommandBuffer::BindDescriptorSet";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindIndexBuffer = L"IV3DCommandBuffer::BindIndexBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_PushConstant = L"IV3DCommandBuffer::PushConstant";
static constexpr wchar_t* Log_IV3DCommandBuffer_PushDescriptorSet = L"IV3DCommandBuffer::PushDescriptorSet";
static constexpr wchar_t* Log_IV3DCommandBuffer_SetViewport = L"IV3DCommandBuffer::SetViewport";
static constexpr wchar_t* Log_IV3DCommandBuffer_SetScissor = L"IV3DCommandBuffer::SetScissor";
static constexpr wchar_t* Log_IV3DCommandBuffer_ResetQueryPool = L"IV3DCommandBuffer::ResetQueryPool";
static constexpr wchar_t* Log_IV3DCommandBuffer_BeginQuery = L"IV3DCommandBuffer::BeginQuery";
static constexpr wchar_t* Log_IV3DCommandBuffer_EndQuery = L"IV3DCommandBuffer::EndQuery";
static constexpr wchar_t* Log_IV3DCommandBuffer_WriteTimestamp = L"IV3DCommandBuffer::WriteTimestamp";
static constexpr wchar_t* Log_IV3DCommandBuffer_CopyQueryPoolResult = L"IV3DCommandBuffer::CopyQueryPoolResult";
static constexpr wchar_t* Log_IV3DCommandBuffer_Draw = L"IV3DCommandBuffer::Draw";
static constexpr wchar_t* Log_IV3DCommandBuffer_DrawIndexed = L"IV3DCommandBuffer::DrawIndexed";
static constexpr wchar_t* Log_IV3DCommandBuffer_Dispatch = L"IV3DCommandBuffer::Dispatch";
static constexpr wchar_t* Log_IV3DCommandBuffer_ExecuteCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers";
static constexpr wchar_t* Log_IV3DCommandBuffer_BeginDebugMarker = L"IV3DCommandBuffer::BeginDebugMarker";
static constexpr wchar_t* Log_IV3DCommandBuffer_InsertDebugMarker = L"IV3DCommandBuffer::InsertDebugMarker";

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : �v���C�}���R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肷�邱�Ƃ͂ł��܂���B";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : %s : �R�}���h�o�b�t�@�[�ւ̋L�^�͊��ɊJ�n����Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : %s : �R�}���h�o�b�t�@�[�ւ̋L�^�͊��ɏI�����Ă��܂��B";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"IV3DCommandBuffer : %s : �R�}���h�o�b�t�@�[�ւ̋L�^���J�n����Ă��Ȃ��̂ɁA�R�}���h�������������Ƃ��܂����B";
static constexpr wchar_t* Log_Error_OverFlowViewports = L"IV3DCommandBuffer::SetViewport : %s : �ݒ�ł���r���[�|�[�g�̍ő吔�� %u ( maxViewports ) �ł��B : firstViewport[%u] viewportCount[%u]";
static constexpr wchar_t* Log_Error_OverFlowScissors = L"IV3DCommandBuffer::SetScissor : %s : �ݒ�ł���V�U�[�̍ő吔�� %u ( maxViewports ) �ł��B : firstScissor[%u] scissorCount[%u]";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : �R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE ���w�肵���ꍇ�� pRenderPass �� subpass �͕K���w�肵�Ă��������A";
static constexpr wchar_t* Log_Error_NotStandardDescriptorSets = L"IV3DCommandBuffer::BindDescriptorSet : %s : %s[%u] �̓X�^���_�[�h�f�X�N���v�^�Z�b�g�ł͂���܂���B";
static constexpr wchar_t* Log_Error_NotStandardDescriptorSet = L"IV3DCommandBuffer::BindDescriptorSet : %s : %s �̓X�^���_�[�h�f�X�N���v�^�Z�b�g�ł͂���܂���B";
static constexpr wchar_t* Log_Error_OverFlowBindVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer : %s : �o�C���h�ł���o�[�e�b�N�X�o�b�t�@�̍ő吔�� %u ( maxVertexInputBindings ) �ł��B : firstBinding[%u] bindingCount[%u]";
static constexpr wchar_t* Log_Error_OverFlowBindDescriptorSet = L"IV3DCommandBuffer::BindDescriptorSet : %s : �o�C���h�ł���f�X�N���v�^�Z�b�g�̍ő吔�� %u ( maxBoundDescriptorSets ) �ł��B : firstSet[%u] descriptorSetCount[%u]";
static constexpr wchar_t* Log_Error_NotPushDescriptorSet = L"IV3DCommandBuffer::PushDescriptorSet : %s[%u] �̓v�b�V���f�X�N���v�^�Z�b�g�ł͂���܂���B";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : �R�}���h�o�b�t�@ %s[%u] �̓Z�J���_���ł͂���܂���B";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffer : %s : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";
static constexpr wchar_t* Log_Error_CommandBuffersSimultaneousUse = L"IV3DCommandBuffer::ExecuteCommandBuffer : %s : �v���C�}���R�}���h�o�b�t�@�[�̎g�p�@�� V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE ���w�肳��Ă���ꍇ�́A�Z�J���_���R�}���h�o�b�t�@�[�����l�ɂ��̎g�p�@���w�肷��K�v������܂��B";
static constexpr wchar_t* Log_Error_UnavailablePushDescriptorSet = L"IV3DCommandBuffer::PushDescriptorSet : %s : �g���@�\ V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SET ���L���łȂ����� PushDescriptorSet �͎g�p�ł��܂���B";
static constexpr wchar_t* Log_Error_UnavailableBeginDebugMarker = L"IV3DCommandBuffer::BeginDebugMarker : %s : �g���@�\ V3D_DEVICE_EXTENSION_DEBUG_MARKER ���L���łȂ����� BeginDebugMarker �͎g�p�ł��܂���B";
static constexpr wchar_t* Log_Error_UnavailableEndDebugMarker = L"IV3DCommandBuffer::EndDebugMarker : %s : �g���@�\ V3D_DEVICE_EXTENSION_DEBUG_MARKER ���L���łȂ����� EndDebugMarker �͎g�p�ł��܂���B";
static constexpr wchar_t* Log_Error_UnavailableInsertDebugMarker = L"IV3DCommandBuffer::InsertDebugMarker : %s : �g���@�\ V3D_DEVICE_EXTENSION_DEBUG_MARKER ���L���łȂ����� InsertDebugMarker �͎g�p�ł��܂���B";

/*************/
/* IV3DQueue */
/*************/

static constexpr wchar_t* Log_IV3DQueue_Submit = L"IV3DQueue::Submit";
static constexpr wchar_t* Log_IV3DQueue_Present = L"IV3DQueue::Present";

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : %s : �R�}���h�o�b�t�@ %s[%u] �̓v���C�}���ł͂���܂���B";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : %s : �R�}���h�o�b�t�@ %s[%u] �� IV3DCommandBuffer::End �ŏI������Ă��܂���";

/********************/
/* V3DDescriptorSet */
/********************/

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : %s : �o�C���f�B���O %u �Ƀ��\�[�X ( %s ) ���Z�b�g����Ă��܂���B";
static constexpr wchar_t* Log_Error_DescriptorSetNoNeedUpdate = L"IV3DDescriptorSet::Update : %s : �v�b�V���f�X�N���v�^�Z�b�g�̍X�V�͕K�v�͂���܂���B";

#endif //V3D_DEBUG
