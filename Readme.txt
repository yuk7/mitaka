=======================================================
Mitaka  version 1.3.1 ソースコード
    2016/3/4
　　加藤恒彦
　　国立天文台 ４次元デジタル宇宙プロジェクト
=======================================================



■ソースコードの利用条件について
ソースコードは MIT ライセンスのもとで配布されます。
このソースコードを使用してソフトウェアを作成する場合は、
ライセンスに基づいた著作権表記をしてください。
また、ソースコードに関するお問い合わせには一切お答えできませんので、
ご了承ください。


■開発者
加藤恒彦（国立天文台４次元デジタル宇宙プロジェクト）


■開発環境
Windows 7 Ultimate (64bit版)
Microsoft Visual Studio 2015 Professional (Visual C++)
Intel C++ Composer XE 2016
Intel Core i7-2600K (Quad core @ 3.4GHz)


■コンパイルの仕方
以下では、Microsoft Visual Studio 2015 の場合を想定して
説明しています。他の環境の場合も、以下を参考にして
設定をしてください。

●プリプロセッサの定義
_USE_MATH_DEFINES
を定義してください

●リンクするライブラリ
libcmt.lib
libvcruntime.lib
libucrt.lib
vfw32.lib
wsock32.lib
winmm.lib
opengl32.lib
glu32.lib
mfuuid.lib
dxguid.lib  ...(1)
dinput8.lib   ...(1)
Strmiids.lib  ...(1)
libjpeg.lib   ...(2)
zlib.lib   ...(3)
libpng.lib   ...(4)
glew32s.lib   ...(5)
freetype26MT.lib  ...(6)

(1) の各ファイルは、Direct X のライブラリです。Microsoft の Web サイトから
Direct X SDK (June 2010) を入手し、インストールして使用してください。
strmiids.lib が無い場合は、別途 Platform SDK を入手してインストールしてください。
(2) は、Independent JPEG Group (http://www.ijg.org/) から入手してください。
(3) は、zlib home page (http://www.zlib.net/) から入手してください。
(4) は、libpng home page (http://www.libpng.org/pub/png/libpng.html) から入手してください。
(5) は、The OpenGL Extension Wrangler Library (http://glew.sourceforge.net/) から入手してください。
(6) は、FreeType 2.6 です。FreeType ptoject (http://www.freetype.org/) から入手してください。

●ランタイムライブラリ
ランタイムライブラリは、マルチスレッド（/MT)を使用してください。
また、「特定のライブラリの無視」で
libc.lib
と指定してください。

●文字セット
マルチバイト文字セットを使用してください。

●コンパイル
すべてのソースコードと上記ライブラリをプロジェクトに追加して
コンパイルしてください。


■実行
作成した実行ファイルは、配布しているバイナリ版の中の
実行ファイルと差し替えて使ってください。
