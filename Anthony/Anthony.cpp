// Win32Project1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Anthony.h"
#include "device.h"
#include "Atlconv.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HWND m_hWnd;									//ウインドウハンドル
MEMORYCARD byteMemDat;	//メモリーカードデータ格納先

// このコード モジュールに含まれる関数の宣言を転送します:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(
	_In_  HWND hwndDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
);

byte CreateXOR(short adr, byte* dat);
BOOL ReadFromFile();
BOOL ReadFromCard();
BOOL WriteToCard();
BOOL WriteToFile();
BOOL SetupWinUsb(DEVICE_DATA *deviceData);

void SetProgressBar(int);
void ProgressBar_Step();
BOOL UpdateDataList(MEMORYCARD* data);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	//	MyRegisterClass(hInstance);

		// アプリケーションの初期化を実行します:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT1);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//	return RegisterClassEx(&wcex);
//}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
	m_hWnd = hWnd;

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

INT_PTR CALLBACK DialogProc(
	_In_  HWND hDlg,
	_In_  UINT uMsg,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//全列削除
		while (Header_GetItemCount(ListView_GetHeader(GetDlgItem(hDlg, IDC_LIST2))))
			ListView_DeleteColumn(GetDlgItem(hDlg, IDC_LIST2), 0);

		//列作成
		{
			LV_COLUMN lvcol = { 0, };
			TCHAR Title[255];
			TCHAR StartBlock[255];

			lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvcol.fmt = LVCFMT_LEFT;
			lvcol.cx = 100;
			if (LoadString(hInst, IDS_COLUMN_TITLE, Title, sizeof(Title) / sizeof(Title[0])))
			{
				lvcol.pszText = Title;
			}
			lvcol.iSubItem = 0;
			ListView_InsertColumn(GetDlgItem(hDlg, IDC_LIST2), 0, &lvcol);
			if (LoadString(hInst, IDS_COLUMN_STARTBLOCK, StartBlock, sizeof(StartBlock) / sizeof(StartBlock[0])))
			{
				lvcol.pszText = StartBlock;
			}
			lvcol.iSubItem = 1;
			ListView_InsertColumn(GetDlgItem(hDlg, IDC_LIST2), 1, &lvcol);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_FROMFILE)
		{
			ReadFromFile();
		}
		else if (LOWORD(wParam) == IDC_BUTTON_FROMCARD)
		{
			ReadFromCard();
		}
		else if (LOWORD(wParam) == IDC_BUTTON_TOFILE)
		{
			WriteToFile();
		}
		else if (LOWORD(wParam) == IDC_BUTTON_TOCARD)
		{
			WriteToCard();
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return (INT_PTR)FALSE;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL ReadFromFile()
{
	IFileOpenDialog *pDlg;
	COMDLG_FILTERSPEC FileTypes[] = {
		{ L"MemoryCard files", L"*.mcr;*.mem;*.psm" },
		{ L"All files", L"*.*" }
	};

	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDlg));
	WCHAR cPath[MAX_PATH] = L"";
	GetCurrentDirectoryW(sizeof(cPath) / sizeof(cPath[0]), cPath);
	IShellItem *psiFolder, *psiParent;
	SHCreateItemFromParsingName(cPath, NULL, IID_PPV_ARGS(&psiFolder));
	psiFolder->GetParent(&psiParent);

	//初期フォルダの指定
	pDlg->SetFolder(psiFolder);
	//フィルターの指定
	pDlg->SetFileTypes(_countof(FileTypes), FileTypes);
	//ダイアログ表示
	hr = pDlg->Show(NULL);

	//ファイル名
	LPOLESTR pwsz = NULL;
	if (SUCCEEDED(hr))
	{
		IShellItem *pItem;
		hr = pDlg->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
			if (SUCCEEDED(hr))
			{
				HANDLE hFile;
				hFile = CreateFile(pwsz, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (hFile)
				{
					DWORD BytesRead;
					BOOL b = ReadFile(hFile, byteMemDat.Byte, sizeof(byteMemDat), &BytesRead, NULL);
					if (BytesRead)
					{
						CloseHandle(hFile);
					}
				}
			}
		}
	}
	UpdateDataList(&byteMemDat);
	return TRUE;
}

//メモリーカードから読み込む。
//コマンドフォーマットは、
//アダプターへのコマンド4バイト+（メモリーカードへのコマンド140バイト）
//アダプターへのコマンド4バイトについて
//1バイト目：0xaa(固定）
//2バイト目：0x42(固定）
//1バイト目：0x8c(固定、後に続くデータサイズ）
//1バイト目：0x00(固定）
//メモリーカードへのコマンド140バイトについては、藤田資料を参照
BOOL ReadFromCard()
{
	DEVICE_DATA	deviceData;
	BOOL b = SetupWinUsb(&deviceData);
	if (b)
	{
		int frames = 64 * 16;
		//プログレスバー更新
		SetProgressBar(frames);

		ULONG Length;
		UCHAR OutData[144] = { 0xaa, 0x42, 0x8c, 0x00, 0x81, 0x52, 0x0 };
		UCHAR InData[144] = { 0x0 };
		//	OVERLAPPED ol;
		for (int i = 0; i < frames; i++)
		{
			//アドレス指定
			OutData[8] = (i & 0xff00) >> 8;
			OutData[9] = (i & 0xff);

			WinUsb_ResetPipe(deviceData.WinusbHandle, 2);
			WinUsb_ResetPipe(deviceData.WinusbHandle, 129);
			b = WinUsb_WritePipe(deviceData.WinusbHandle, 2, OutData, 144, &Length, NULL);
			DWORD d;
			d = GetLastError();
			//		Sleep(100);
			b = WinUsb_ReadPipe(deviceData.WinusbHandle, 129, InData, 144, &Length, NULL);

			//戻りの先頭2byteが0x55,0x5aじゃなかったらエラーとする
			if ((InData[0] != 0x55) || (InData[1] != 0x5a))
			{
				TCHAR strBuf[128];
				_stprintf_s(strBuf, sizeof(strBuf) / sizeof(strBuf[0]), _T("Error among Read Frame %d.\nPlease Reconnect Memory Card Adaptor."), i);
				MessageBox(NULL, strBuf, szTitle, MB_OK);
				break;
			}
			//if (Length != 144)
			//{
			//	ULONG LeftLength = 0;
			//	WinUsb_ReadPipe(deviceData.WinusbHandle, 129, &InData[Length], 144 - Length, &LeftLength, NULL);
			//}

			if (b)
			{
				//読み込んだデータを格納
				//実データは0x0eバイト目から始まる。
				memcpy(&byteMemDat.Frame[i], &InData[0xe], 128);
			}
			ProgressBar_Step();
		}
		CloseDevice(&deviceData);
		UpdateDataList(&byteMemDat);
		return TRUE;
	}
	return FALSE;
}

BOOL WriteToFile()
{
	IFileSaveDialog *pDlg;
	COMDLG_FILTERSPEC FileTypes[] = {
		{ L"MemoryCard files", L"*.mcr;*.mem;*.psm" },
		{ L"All files", L"*.*" }
	};

	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDlg));
	WCHAR cPath[MAX_PATH] = L"";
	GetCurrentDirectoryW(sizeof(cPath) / sizeof(cPath[0]), cPath);
	IShellItem *psiFolder = NULL, *psiParent = NULL;
	SHCreateItemFromParsingName(cPath, NULL, IID_PPV_ARGS(&psiFolder));
	psiFolder->GetParent(&psiParent);

	//初期フォルダの指定
	pDlg->SetFolder(psiFolder);
	//フィルターの指定
	pDlg->SetFileTypes(_countof(FileTypes), FileTypes);
	//拡張子がなかった場合に付加する拡張子
	pDlg->SetDefaultExtension(L"mem");
	//ダイアログ表示
	hr = pDlg->Show(m_hWnd);

	//ファイル名
	LPOLESTR pwsz = NULL;
	if (SUCCEEDED(hr))
	{
		IShellItem *pItem;
		hr = pDlg->GetResult(&pItem);
		if (SUCCEEDED(hr))
		{

			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
			if (SUCCEEDED(hr))
			{
				HANDLE hFile;
				hFile = CreateFile(pwsz, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if (hFile)
				{
					DWORD BytesWrite;
					WriteFile(hFile, byteMemDat.Byte, sizeof(byteMemDat), &BytesWrite, NULL);
					if (BytesWrite)
					{
						CloseHandle(hFile);
					}
				}
			}
		}
	}

	return TRUE;
}

//メモリーカードに書き込む。
//コマンドフォーマットは、
//アダプターへのコマンド4バイト+（メモリーカードへのコマンド138バイト）
//アダプターへのコマンド4バイトについて
//1バイト目：0xaa(固定）
//2バイト目：0x42(固定）
//1バイト目：0x8a(固定、後に続くデータサイズ）
//1バイト目：0x00(固定）
//メモリーカードへのコマンド138バイトについては、藤田資料を参照
BOOL WriteToCard()
{
	DEVICE_DATA	deviceData;
	TCHAR msg[255];
	if (LoadString(hInst, IDS_WRITE_CARD, msg, sizeof(msg) / sizeof(msg[0])))
	{
		if (MessageBox(m_hWnd, msg, szTitle, MB_OKCANCEL) == IDOK)
		{
			BOOL b = SetupWinUsb(&deviceData);
			if (b)
			{
				int frames = 64 * 16;
				//プログレスバー更新
				SetProgressBar(frames);

				ULONG Length;
				UCHAR OutData[142] = { 0xaa, 0x42, 0x8a, 0x00, 0x81, 0x57, 0x0 };
				UCHAR InData[142] = { 0x0 };
				//	OVERLAPPED ol;
				for (int i = 0; i < frames; i++)
				{
					//アドレス指定
					OutData[8] = (i & 0xff00) >> 8;
					OutData[9] = (i & 0xff);
					//データ128バイト
					memcpy(&OutData[10], &byteMemDat.Frame[i][0], 128);
					//XORフラグを取得
					OutData[10 + 128] = CreateXOR((short)i, byteMemDat.Frame[i]);

					WinUsb_FlushPipe(deviceData.WinusbHandle, 2);
					WinUsb_FlushPipe(deviceData.WinusbHandle, 129);
					b = WinUsb_WritePipe(deviceData.WinusbHandle, 2, OutData, sizeof(OutData), &Length, NULL);
					DWORD d = 0;
					d = GetLastError();
					Sleep(50);
					memset(InData, 0, sizeof(InData));
					b = WinUsb_ReadPipe(deviceData.WinusbHandle, 129, InData, sizeof(InData), &Length, NULL);
					if (Length != sizeof(InData))
					{
						TCHAR strBuf[128];
						_stprintf_s(strBuf, sizeof(strBuf) / sizeof(strBuf[0]), _T("Error among Write Frame %d.\nPlease Reconnect Memory Card Adaptor."), i);
						MessageBox(NULL, strBuf, szTitle, MB_OK);
						CloseDevice(&deviceData);

						return FALSE;
					}

					if (b)
					{
						//読み込んだデータを格納
					}

					ProgressBar_Step();
				}

				CloseDevice(&deviceData);
				MessageBox(m_hWnd, _T("Finish!"), szTitle, MB_OK);
				return TRUE;
			}
		}
	}
	return FALSE;

}

//XOR値を求める
//与えるデータは、アドレス2byte+データ128byte
//各バイトデータについてXORをかけた値を返す。
byte CreateXOR(short adr, byte* dat)
{
	byte xor = 0;
	xor ^= (adr >> 8) & 0xff;
	xor ^= adr & 0xff;
	for (int i = 0; i < 128; i++)
	{
		xor ^= dat[i];
	}
	return xor;
}

BOOL SetupWinUsb(DEVICE_DATA *deviceData)
{
	HRESULT               hr;
	USB_DEVICE_DESCRIPTOR deviceDesc;
	BOOL                  bResult;
	BOOL                  noDevice;
	ULONG                 lengthReceived;
	USB_INTERFACE_DESCRIPTOR interfaceDesc;
	TCHAR strBuf[128];	//作業用バッファ

	//
	// Find a device connected to the system that has WinUSB installed using our
	// INF
	//
	hr = OpenDevice(deviceData, &noDevice);

	if (FAILED(hr)) {

		if (noDevice) {

			MessageBox(NULL, _T("Device not connected or driver not installed\n"), NULL, MB_OK);

		}
		else {

			MessageBox(NULL, _T("Failed looking for device, HRESULT 0x%x\n"), NULL, MB_OK);
		}

		return FALSE;
	}

	//
	// Get device descriptor
	//
	bResult = WinUsb_GetDescriptor(deviceData->WinusbHandle,
		USB_DEVICE_DESCRIPTOR_TYPE,
		0,
		0,
		(PBYTE)&deviceDesc,
		sizeof(deviceDesc),
		&lengthReceived);

	if (FALSE == bResult || lengthReceived != sizeof(deviceDesc))
	{
		_stprintf_s(strBuf, sizeof(strBuf) / sizeof(strBuf[0]), _T("Error among LastError %d or lengthReceived %d\n"),
			FALSE == bResult ? GetLastError() : 0,
			lengthReceived);
		MessageBox(NULL, strBuf, NULL, MB_OK);
		CloseDevice(deviceData);
		return FALSE;
	}

	//
	// Print a few parts of the device descriptor
	//
	//printf(_T("Device found: VID_%04X&PID_%04X; bcdUsb %04X\n"),
	//	deviceDesc.idVendor,
	//	deviceDesc.idProduct,
	//	deviceDesc.bcdUSB);

	WinUsb_QueryInterfaceSettings(deviceData->WinusbHandle, 0, &interfaceDesc);

	for (int i = 0; i < interfaceDesc.bNumEndpoints; i++)
	{
		WINUSB_PIPE_INFORMATION pipeInfo;
		BOOL bResult;

		//[3]
		bResult = WinUsb_QueryPipe(deviceData->WinusbHandle,
			0,
			(UCHAR)i,
			&pipeInfo);
		//[4]
		if (pipeInfo.PipeType == UsbdPipeTypeBulk &&
			USB_ENDPOINT_DIRECTION_IN(pipeInfo.PipeId))
		{
			int bulkInPipe;
			bulkInPipe = pipeInfo.PipeId;
		}
		else if (pipeInfo.PipeType == UsbdPipeTypeBulk &&
			USB_ENDPOINT_DIRECTION_OUT(pipeInfo.PipeId))
		{
			int bulkOutPipe;
			bulkOutPipe = bulkOutPipe = pipeInfo.PipeId;
		}
		else if (pipeInfo.PipeType == UsbdPipeTypeInterrupt)
		{
			//devInfo.interruptPipe = pipeInfo.PipeId;
		}
		else
		{
			bResult = FALSE;
			break;
		}
	}
	return bResult;
}

//プログレスバーの設定
void SetProgressBar(int max)
{
	HWND hProg = GetDlgItem(m_hWnd, IDC_PROGRESS1);
	SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, max));
	SendMessage(hProg, PBM_SETSTEP, 1, 0);

}

void ProgressBar_Step()
{
	HWND hProg = GetDlgItem(m_hWnd, IDC_PROGRESS1);
	SendMessage(hProg, PBM_STEPIT, 0, 0);
	UpdateWindow(m_hWnd);
}

//データリストを更新
BOOL UpdateDataList(MEMORYCARD* data)
{
	HWND hWnd = GetDlgItem(m_hWnd, IDC_LIST2);
	//リストクリア
	ListView_DeleteAllItems(hWnd);

	//データ名表示
	for (int i = 0; i < 16; i++)
	{
		//ブロックが使用中で、セーブデータの先頭ブロックか
		if ((data->Block[0][128 * i]) == 0x51)
		{
			if (data->Block[i][0] == 'S' && data->Block[i][1] == 'C')
			{
				char strTitle[0x5f - 0x03];
				LVITEM lvi = { 0, };
				//strcpy_s(strTitle, sizeof(strTitle), (char*)&data->Block[i][4]);
				StringCchCopyA(strTitle, sizeof(strTitle), (char*)&data->Block[i][4]);
				//文字コード変換
				CA2T wstrTitle(strTitle);

				lvi.pszText = wstrTitle;
				lvi.mask = LVIF_TEXT;
				lvi.iItem = ListView_GetItemCount(hWnd);
				ListView_InsertItem(hWnd, (LPLVITEM)&lvi);
				TCHAR BlockNo[3];
				_stprintf_s(BlockNo, _T("%d"), i);
				ListView_SetItemText(hWnd, (WPARAM)ListView_GetItemCount(hWnd) - 1, 1, BlockNo);
			}
		}
	}
	ListView_SetColumnWidth(hWnd, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hWnd, 1, LVSCW_AUTOSIZE_USEHEADER);
	return TRUE;
}