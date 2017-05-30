#pragma once

// ----------------------------------------------------------------------------------------------------
// Common
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidArgument = L" : 不正な引数が渡されました :";

// ----------------------------------------------------------------------------------------------------
// IV3DDevice
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_InvalidSubpass = L"IV3DDevice::CreateGraphicsPipeline : 指定されたサブパス %d は無効です。";
static constexpr wchar_t* Log_Error_MismatchSubpassColorAttachmentCount = L"IV3DDevice::CreateGraphicsPipeline : サブパスのカラーアタッチメントの数が一致しません。";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentCount = L"IV3DDevice::CreateFrameBuffer : 指定されたレンダーパスと作成しようとしているフレームバッファーのアタッチメントの数が一致しません。";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentSize = L"IV3DDevice::CreateFrameBuffer : 全てのアタッチメントは 幅、高さ、レイヤー数が同じでないといけません。";
static constexpr wchar_t* Log_Error_MismatchRAContainerAttachmentFormatOrSamples = L"IV3DDevice::CreateFrameBuffer : アタッチメント %d のフォーマットもしくわサンプル数が一致しません。";
static constexpr wchar_t* Log_Error_OutOfInputAttachment = L"IV3DDevice::CreateRenderPass : インプットアタッチメント ( [%u].attachment %u ) はアタッチメントの数 ( %u ) を超えて指定されています。";
static constexpr wchar_t* Log_Error_OutOfColorAttachment = L"IV3DDevice::CreateRenderPass : カラーアタッチメント ( [%u].attachment %u ) はアタッチメントの数 ( %u ) を超えて指定されています。";
static constexpr wchar_t* Log_Error_OutOfResolveAttachment = L"IV3DDevice::CreateRenderPass : リゾルブアタッチメント ( [%u].attachment %u ) はアタッチメントの数 ( %u ) を超えて指定されています。";
static constexpr wchar_t* Log_Error_MismatchResolveAttachmentSamples = L"IV3DDevice::CreateRenderPass : リゾルブアタッチメント ( [%u].attachment %u ) のサンプル数は V3D_SAMPLE_COUNT_1 にしてください。";
static constexpr wchar_t* Log_Error_AlreadyBindResourceMemory = L"IV3DDevice::BindResourceMemory : リソース %s[%u] は既にメモリを確保し、バインドされています。";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandBuffer
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定することはできません。";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : コマンドバッファーへの記録は既に開始されています。";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : コマンドバッファーは記録は既に終了されています。";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"コマンドバッファーが開始されていないのにコマンドを書き込もうとしました。";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : コマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定した場合は pRenderPass と subpass は必ず指定してください、";
static constexpr wchar_t* Log_Error_BarrierBufferOverflow = L"IV3DCommandBuffer::BarrierBuffer : バッファーのメモリの範囲外にバリアを張ろうとしました。";
static constexpr wchar_t* Log_Error_CopyImageLayout = L"IV3DCommandBuffer::CopyImage : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_MismatchCopyImage = L"IV3DCommandBuffer::ResolveImage : コピー先とコピー元のイメージが一致しません。";
static constexpr wchar_t* Log_Error_BlitImageLayout = L"IV3DCommandBuffer::BlitImage : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_BlitImageSrcSamples = L"IV3DCommandBuffer::BlitImage : 転送先と転送元のサンプル数が一致しません。 IV3DCommandBuffer::ResolveImage を使用してください。";
static constexpr wchar_t* Log_Error_ResolveImageLayout = L"IV3DCommandBuffer::ResolveImage : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_ResolveImageDstSamples1 = L"IV3DCommandBuffer::ResolveImage : 変換先のイメージのサンプル数は V3D_SAMPLE_COUNT_1 である必要があります。";
static constexpr wchar_t* Log_Error_ResolveImageViewLayout = L"IV3DCommandBuffer::ResolveImageView : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_ResolveImageViewDstSamples1 = L"IV3DCommandBuffer::ResolveImageView : 変換先のイメージのサンプル数は V3D_SAMPLE_COUNT_1 である必要があります。";
static constexpr wchar_t* Log_Error_MismatchResolveImage = L"IV3DCommandBuffer::ResolveImage : 変換先と変換元のイメージが一致しません。";
static constexpr wchar_t* Log_Error_MismatchResolveImageView = L"IV3DCommandBuffer::ResolveImageView : 変換先と変換元のイメージが一致しません。";
static constexpr wchar_t* Log_Error_ClearImageLayout = L"IV3DCommandBuffer::ClearImage : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_ClearImageViewLayout = L"IV3DCommandBuffer::ClearImageView : %s は %s もしくわ %s である必要があります。";
static constexpr wchar_t* Log_Error_NoneClearAttachments = L"IV3DCommandBuffer::ClearAttachments : クリアするアタッチメントが指定されていません :";
static constexpr wchar_t* Log_Error_NotVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer : バインディング %u はバーテックスバッファビューではありません。";
static constexpr wchar_t* Log_Error_OutOfVertexBuffer = L"IV3DCommandBuffer::BindVertexBuffer : バインディング %u のオフセット %I64u は範囲外をさしています。";
static constexpr wchar_t* Log_Error_NotIndexBufferView = L"IV3DCommandBuffer::BindIndexBufferView : %s インデックスバッファビューではありません。";
static constexpr wchar_t* Log_Error_OutOfIndexBuffer = L"IV3DCommandBuffer::BindIndexBuffer : オフセット %I64u は範囲外をさしています。";
static constexpr wchar_t* Log_Error_WriteTimeStampQueryType = L"IV3DCommandBuffer::WriteTimestamp : クエリは %s である必要があります。";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : コマンドバッファ %s[%u] はセカンダリではありません。";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffers : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";
static constexpr wchar_t* Log_Error_CommandBuffersSimultaneousUse = L"IV3DCommandBuffer::ExecuteCommandBuffers : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE が指定されている場合は、セカンダリコマンドバッファーも同様にこの使用法を指定する必要があります。";

// ----------------------------------------------------------------------------------------------------
// IV3DCommandQueue
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : コマンドバッファ %s[%u] はプライマリではありません。";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";

// ----------------------------------------------------------------------------------------------------
// V3DDescriptorSet
// ----------------------------------------------------------------------------------------------------

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : バインディング %u にリソース ( %s ) がセットされていません。";
