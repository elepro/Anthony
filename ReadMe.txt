﻿========================================================================
    Anthony : PlayStation1/2 メモリーカードユーティリティー
========================================================================
このソフトは？
	Sony製家庭用ゲーム機PlayStation1/2用のメモリーカードのデータをPCで読み書きするソフトです。
	メモリーカードを読み書きするためのアダプタとして、Sony製PlayStation3用メモリーカードアダプターCECHZM1を使用します。
	対応しているメモリーカードはPlayStation1/2用です。
	
対応OS
	Win8以降
	ver0.3.0.1より、Win7はサポート外となりました。
使い方
	まずCECHZM1用ドライバーとしてWinUSBをインストールします。
		CECHZM1をUSBポートに接続し、ドライバーのインストールが求められたら
			「コンピュータを参照してドライバーを検索」
			「コンピュータ上の利用可能なドライバーの一覧から選択します」
			「ユニバーサル シリアル バス デバイス」
			WinUSBデバイス→WinUSBデバイス
			の順に選択してインストールします。

	次に、
	・PS1メモリーカードを使う場合は、Anthony.exeを起動します。
	・PS2メモリーカードを使う場合は、AnthonyPS2.exeを起動します。

		メモリカードからデータを読み込む場合は、"from Card"ボタンをクリックします。
		PC上のファイルからデータを読み込む場合は、"from File"ボタンをクリックし、読み込むファイルを選択します。

		メモリーカードにデータを書き込む場合は、メモリーカードかファイルからデータを読み込んだ後、"to Card"ボタンをクリックします。
		PCにデータを保存する場合は、メモリーカードかファイルからデータを読み込んだ後、"to File"ボタンをクリックし保存先を指定します。

	対応形式
		対応しているPS1用ファイル形式は、PSM形式(128KB)です。
		対応しているPS2用ファイル形式は、PS2形式(8MB)です。

開発動機
	既にある同様のソフトは、Win7以降で使う上で不便な面があるため。
	ソースコードを公開することで、後世に資料を残す。

開発環境
	Ver0.3.0.3現在:
		Visual Studio 2022(17.12.3)
			要追加インストール
			・最新の v143 ビルド ツール用 C++ ATL(x86 および x64)
			・MSVC v143 - VS 2022 C++ x64/x86 ビルドツール(最新)
謝辞
	PS2用メモリーカードの読み書きにjimmikaelkael氏のps3mca-toolのコードを利用しています。
