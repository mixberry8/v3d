#pragma once

// ----------------------------------------------------------------------------------------------------
// ログ
// ----------------------------------------------------------------------------------------------------

#ifdef _DEBUG

/********/
/* 共通 */
/********/

static constexpr wchar_t* Log_Error_InvalidArgument = L" : 不正な引数が渡されました :";

/**************/
/* IV3DDevice */
/**************/

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

/*********************/
/* IV3DCommandBuffer */
/*********************/

static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierBuffer = L"IV3DCommandBuffer::BarrierBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierBufferView = L"IV3DCommandBuffer::BarrierBufferView";
static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierBufferViews = L"IV3DCommandBuffer::BarrierBufferViews";
static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierImage = L"IV3DCommandBuffer::BarrierImage";
static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierImageView = L"IV3DCommandBuffer::BarrierImageView";
static constexpr wchar_t* Log_IV3DCommandBuffer_BarrierImageViews = L"IV3DCommandBuffer::BarrierImageViews";
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
static constexpr wchar_t* Log_IV3DCommandBuffer_ClearAttachments = L"IV3DCommandBuffer::ClearAttachments";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindPipeline = L"IV3DCommandBuffer::BindPipeline";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindDescriptorSets = L"IV3DCommandBuffer::BindDescriptorSets";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindVertexBuffers = L"IV3DCommandBuffer::BindVertexBuffers";
static constexpr wchar_t* Log_IV3DCommandBuffer_BindIndexBuffer = L"IV3DCommandBuffer::BindIndexBuffer";
static constexpr wchar_t* Log_IV3DCommandBuffer_PushConstant = L"IV3DCommandBuffer::PushConstant";
static constexpr wchar_t* Log_IV3DCommandBuffer_PushDescriptorSets = L"IV3DCommandBuffer::PushDescriptorSets";
static constexpr wchar_t* Log_IV3DCommandBuffer_SetViewport = L"IV3DCommandBuffer::SetViewport";
static constexpr wchar_t* Log_IV3DCommandBuffer_SetScissor = L"IV3DCommandBuffer::SetScissor";
static constexpr wchar_t* Log_IV3DCommandBuffer_ResetQueryPool = L"IV3DCommandBuffer::ResetQueryPool";
static constexpr wchar_t* Log_IV3DCommandBuffer_BeginQuery = L"IV3DCommandBuffer::BeginQuery";
static constexpr wchar_t* Log_IV3DCommandBuffer_EndQuery = L"IV3DCommandBuffer::EndQuery";
static constexpr wchar_t* Log_IV3DCommandBuffer_WriteTimestamp = L"IV3DCommandBuffer::WriteTimestamp";
static constexpr wchar_t* Log_IV3DCommandBuffer_Draw = L"IV3DCommandBuffer::Draw";
static constexpr wchar_t* Log_IV3DCommandBuffer_DrawIndexed = L"IV3DCommandBuffer::DrawIndexed";
static constexpr wchar_t* Log_IV3DCommandBuffer_Dispatch = L"IV3DCommandBuffer::Dispatch";
static constexpr wchar_t* Log_IV3DCommandBuffer_ExecuteCommandBuffers = L"IV3DCommandBuffer::ExecuteCommandBuffers";
static constexpr wchar_t* Log_IV3DCommandBuffer_BeginDebugMarker = L"IV3DCommandBuffer::BeginDebugMarker";
static constexpr wchar_t* Log_IV3DCommandBuffer_InsertDebugMarker = L"IV3DCommandBuffer::InsertDebugMarker";

static constexpr wchar_t* Log_Warning_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定することはできません。";

static constexpr wchar_t* Log_Error_CommandBufferAlreadyBegin = L"IV3DCommandBuffer::Begin : %s : コマンドバッファーへの記録は既に開始されています。";
static constexpr wchar_t* Log_Error_CommandBufferAlreadyEnd = L"IV3DCommandBuffer::End : %s : コマンドバッファーは記録は既に終了されています。";
static constexpr wchar_t* Log_Error_CommandBufferNotBegin = L"IV3DCommandBuffer : %s : コマンドバッファーが開始されていないのにコマンドを書き込もうとしました。";
static constexpr wchar_t* Log_Error_CommandBufferRenderPassContinue = L"IV3DCommandBuffer::Begin : %s : コマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE を指定した場合は pRenderPass と subpass は必ず指定してください、";
static constexpr wchar_t* Log_Error_NotStandardDescriptorSet = L"IV3DCommandBuffer::BindDescriptorSets : %s[%u] はスタンダードデスクリプタセットではありません。";
static constexpr wchar_t* Log_Error_NotPushDescriptorSet = L"IV3DCommandBuffer::PushDescriptorSets : %s[%u] はプッシュデスクリプタセットではありません。";
static constexpr wchar_t* Log_Error_NotSecondaryCommandBuffer = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : コマンドバッファ %s[%u] はセカンダリではありません。";
static constexpr wchar_t* Log_Error_SecondaryCommandBufferNotEnd = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";
static constexpr wchar_t* Log_Error_CommandBuffersSimultaneousUse = L"IV3DCommandBuffer::ExecuteCommandBuffers : %s : プライマリコマンドバッファーの使用法に V3D_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE が指定されている場合は、セカンダリコマンドバッファーも同様にこの使用法を指定する必要があります。";
static constexpr wchar_t* Log_Error_UnavailablePushDescriptorSets = L"IV3DCommandBuffer::PushDescriptorSets : %s : 拡張機能が有効でないため PushDescriptorSets は使用できません。";
static constexpr wchar_t* Log_Error_UnavailableBeginDebugMarker = L"IV3DCommandBuffer::BeginDebugMarker : %s : 拡張機能が有効でないため BeginDebugMarker は使用できません。";
static constexpr wchar_t* Log_Error_UnavailableEndDebugMarker = L"IV3DCommandBuffer::EndDebugMarker : %s : 拡張機能が有効でないため EndDebugMarker は使用できません。";
static constexpr wchar_t* Log_Error_UnavailableInsertDebugMarker = L"IV3DCommandBuffer::InsertDebugMarker : %s : 拡張機能が有効でないため InsertDebugMarker は使用できません。";

/*************/
/* IV3DQueue */
/*************/

static constexpr wchar_t* Log_IV3DQueue_Submit = L"IV3DQueue::Submit";
static constexpr wchar_t* Log_IV3DQueue_Present = L"IV3DQueue::Present";

static constexpr wchar_t* Log_Error_NotPrimaryCommandBuffer = L"IV3DCommandQueue::Submit : %s : コマンドバッファ %s[%u] はプライマリではありません。";
static constexpr wchar_t* Log_Error_PrimaryCommandBufferNotEnd = L"IV3DCommandQueue::Submit : %s : コマンドバッファ %s[%u] は IV3DCommandBuffer::End で終了されていません";

/********************/
/* V3DDescriptorSet */
/********************/

static constexpr wchar_t* Log_Error_DescriptorNotSet = L"IV3DDescriptorSet::Update : %s : バインディング %u にリソース ( %s ) がセットされていません。";
static constexpr wchar_t* Log_Error_DescriptorSetNoNeedUpdate = L"IV3DDescriptorSet::Update : プッシュデスクリプタセットは更新する必要はありません。";

#endif //_DEBUG