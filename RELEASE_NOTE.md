### version 0.0.1
* 最初のリリースです。  
  
### version 0.0.2

* #### 変更
  * V3DGraphicsPipelineDesc 構造体のメンバであった
    vertexElementCount pVertexElements vertexLayoutCount pVertexLayouts
    を V3DPipelineVertexInputDesc 構造体に移動し、V3DGraphicsPipelineDesc 構造体に vertexInput として宣言するようにしました。  
  
  * V3DSwapChainDesc 構造体のイメージの使用方法である imageUsageFlags を追加しました。
    イメージの使用方法を任意で指定できます。  
    また出力先のカラーアタッチメントとして使用するはずなので、V3D_IMAGE_USAGE_COLOR_ATTACHMENT は必ず指定することになるはずです。  
  
  * 標準のログを表示する列挙定数組み合わせである V3D_LOG_STANDARD から V3D_LOG_PERFORMANCE_WARNING を外しました。  
  
* #### 修正
  * IV3DCommandBuffer::BlitImage の不具合を修正しました。  
  
  * IV3DCommandBuffer::ResolveImage および IV3DCommandBuffer::ResolveImageView の不具合を修正しました。  
  
  * コマンドバッファーのバリア系のメソッドの不具合を修正しました。  
  
  * マルチスレッド環境でログを表示した際に落ちる不具合を修正しました。  
  
  * サンプルで次のイメージの取得タイミングが間違っていたのを修正しました。  

* ### 改良
  * IV3DResource::Map の第二引数である size に V3D_WHOLE_SIZE を指定することで、第一引数の offset からメモリの終端までをマップできるようにしました。  
  
  * サンプルにある multiSample でレンダーパスを使用せず IV3DCommandBuffer::ResolveImageView を使用したマルチサンプリングもできるようにしました。
  
* ### 追加
  * サンプルのイメージの読み込み時に IV3DCommandBuffer::BlitImage を使用したミップマップを生成する機能を追加しました。( 2Dイメージに限定されます。 ) 
    
  * サンプルにマルチスレッドで描画する multithread を追加しました。  
  
  * シェーダーの特殊化定数に対応しました。 V3DPipelineShader 構造体を確認してください。  

### version 0.0.3
  
  * リリース情報は GitHub の機能を使用するようにしましたので、そちらをご覧ください。
