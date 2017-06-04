#pragma once

// ----------------------------------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidArgument = L" : 不正な引数が渡されました :";

// ----------------------------------------------------------------------------------------------------
// IV3DDevice
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidSubpass = L"IV3DDevice::CreateGraphicsPipeline : %s : 指定されたサブパス %u は範囲を超えています。";
static constexpr wchar_t* Log_Error_MismatchSubpassColorAttachmentCount = L"IV3DDevice::CreateGraphicsPipeline : %s : 指定されたレンダーパス %s のサブパス %u のカラーアタッチメントの数が一致しません。";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentCount = L"IV3DDevice::CreateFrameBuffer : %s : 指定されたレンダーパス %s と作成しようとしているフレームバッファーのアタッチメントの数が一致しません。";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentSize = L"IV3DDevice::CreateFrameBuffer : %s : 全てのアタッチメントは 幅、高さ、深さ、レイヤー数が同じでないといけません。";
static constexpr wchar_t* Log_Error_MismatchFrameBufferAttachmentFormatOrSamples = L"IV3DDevice::CreateFrameBuffer : %s : アタッチメント %u のフォーマットもしくわサンプル数が一致しません。";
static constexpr wchar_t* Log_Error_OutOfInputAttachment = L"IV3DDevice::CreateRenderPass : %s : サブパス %u のインプットアタッチメント ( pInputAttachments[%u].attachment = %u < %u ) が範囲を超えて指定されています。";
static constexpr wchar_t* Log_Error_OutOfColorAttachment = L"IV3DDevice::CreateRenderPass : %s : サブパス %u のカラーアタッチメント ( pColorAttachments[%u].attachment = %u < %u ) が範囲を超えて指定されています。";
static constexpr wchar_t* Log_Error_OutOfResolveAttachment = L"IV3DDevice::CreateRenderPass : %s : サブパス %u のリゾルブアタッチメント ( pResolveAttachments[%u].attachment = %u < %u ) が範囲を超えて指定されています。";
static constexpr wchar_t* Log_Error_MismatchResolveAttachmentSamples = L"IV3DDevice::CreateRenderPass : %s : サブパス %u のリゾルブアタッチメント ( pResolveAttachments[%u].attachment = %u ) のサンプル数は V3D_SAMPLE_COUNT_1 にしてください。";
static constexpr wchar_t* Log_Error_AlreadyBindResourceMemory = L"IV3DDevice::BindResourceMemory : %s : リソース %s[%u] = %s は既にメモリを確保し、バインドされています。";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandBuffer
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定することはできません。";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : %s : コマンドバッファーへの記録は既に開始されています。";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : %s : コマンドバッファーは記録は既に終了されています。";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"IV3DCommandBuffer : %s : コマンドバッファーが開始されていないのにコマンドを書き込もうとしました。";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : コマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定した場合は pRenderPass と subpass は必ず指定してください、";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : コマンドバッファ %s[%u] はセカンダリではありません。";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";
static constexpr wchar_t* Log_Error_CommandBuffersSimultaneousUse = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE が指定されている場合は、セカンダリコマンドバッファーも同様にこの使用法を指定する必要があります。";
static constexpr wchar_t* Log_Error_UnavailablePushDescriptorSets = L"IV3DCommandBuffer::PushDescriptorSets : %s : 拡張機能が有効でないため PushDescriptorSets は使用できません。";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandQueue
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : %s : コマンドバッファ %s[%u] はプライマリではありません。";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : %s : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";

// ----------------------------------------------------------------------------------------------------
// V3DDescriptorSet
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : %s : バインディング %u にリソース ( %s ) がセットされていません。";
