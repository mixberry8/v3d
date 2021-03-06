# v3d
  
### はじめに
v3d は Vulkan をラップした 3D グラフィックスライブラリです。

現在開発中のため、動作チェックが完全ではなく、大幅な修正がかかる場合があります。  
また、実装されていない機能など多数ありますので、その辺はご了承願いします。  
  
![top](https://cloud.githubusercontent.com/assets/28208742/25562639/748a2752-2dc6-11e7-925e-c12c44e90a65.png)
  
### ビルド
v3d をビルドするには以下のものがインストールされている必要があります。  

* Microsoft Visual Studio 2015 Update 3 または Microsoft Visual Studio Community 2015 Update 3  
* Vulkan SDK 1.0.64.0

ビルドに必要なファイルは "build" フォルダに集約されおり、
またソリューションファイルは "build/runtime/solutions" フォルダに Visual Studio のバージョンごとに分かれて入っていますので
お使い環境にあわせてお選びください。( 今現在は vs2015 のみとなっております )  
  
まずはバッチビルドで test 以外のものをすべてビルドしてください。  
ビルドが完了するとルートディレクトリに "bin" フォルダが作成され、さらにサブフォルダとして  
"Visula Studio のバージョン" / "プラットフォーム" / "コンフィギュレーション" が作成され、その中にライブラリファイルが配置されます。  
  
* v3d.lib  
動的リンク用の LIB ファイルです。  
このファイルは "v3d.dll" とセットで使用します。  
* v3d.dll  
動的リンク用の DLL ファイルです。  
このファイルは "v3d.lib" とセットで使用します。  
* v3d_static.lib  
静的リンク用の LIB ファイルです。
このファイルは単体で使用します。 
  
その他詳しいことは "doc/v3d.chm" をご覧ください。  
  
### ライセンス
MIT
