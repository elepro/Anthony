// Win32Project1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "mcio.h"
#include "usbd.h"
#include "Anthony.h"
#include "device.h"
#include "Atlconv.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HWND m_hWnd;									//ウインドウハンドル
PS2MEMORYCARD byteMemDat;	//メモリーカードデータ格納先

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

BOOL ReadFromFile();
BOOL ReadFromCard();
BOOL WriteToCard();
BOOL WriteToFile();
BOOL SetupWinUsb(DEVICE_DATA *deviceData);

void SetProgressBar(int);
void ProgressBar_Step();
BOOL UpdateFileInfo(PS2MEMORYCARD* data);
BOOL UpdateDataList(PS2MEMORYCARD* data);

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
		{ L"PS2 MemoryCard files", L"*.ps2" },
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
					BOOL b = ReadFile(hFile, &(byteMemDat.Byte), sizeof(byteMemDat), &BytesRead, NULL);
					if (BytesRead)
					{
						CloseHandle(hFile);
					}
				}
			}
		}
	}
	//ファイルから読んだ場合はリスト表示しない
	UpdateFileInfo(&byteMemDat);
	return TRUE;
}

//PS2メモリーカードから読み込む。
BOOL ReadFromCard()
{
	BOOL res = TRUE;
	int r;
	int pagesize, blocksize, cardsize, cardflags;

	r = usbd_attach_device(0, 0);
	r = mcio_init();

	r = mcio_mcGetInfo(&pagesize, &blocksize, &cardsize, &cardflags);

	if (!r)
	{
		int pages = cardsize / pagesize;
		int eccsize = pagesize >> 5;
		//プログレスバー更新
		SetProgressBar(pages);

		//8MB以上だったら読まない
		if (((unsigned long long)pages* ((unsigned long long)pagesize + eccsize)) > sizeof(byteMemDat))
		{
			res = FALSE;
			TCHAR strBuf[128];
			if (LoadString(hInst, IDS_NOTSUPPORTOVER8MB, strBuf, sizeof(strBuf) / sizeof(strBuf[0])))
			{
				MessageBox(NULL, strBuf, szTitle, MB_OK);
			}
		}
		else
		{
			for (int i = 0; i < pages; i++)
			{
				byte *pagebuf = &(byteMemDat.Byte[i * (pagesize + eccsize)]);
				byte *eccbuf = pagebuf + pagesize;

				int r = mcio_mcReadPageWithEcc(i, pagebuf, eccbuf);
				if (r)
				{
					res = FALSE;
					TCHAR strBuf[128];
					if (LoadString(hInst, IDS_ERROR_READPAGE, strBuf, sizeof(strBuf) / sizeof(strBuf[0])))
					{
						TCHAR strBuf2[128];
						_stprintf_s(strBuf2, sizeof(strBuf2) / sizeof(strBuf2[0]), strBuf, i);
						MessageBox(NULL, strBuf, szTitle, MB_OK);
					}
					break;
				}
				ProgressBar_Step();
			}

			UpdateDataList(&byteMemDat);
			if (res)
			{
				TCHAR strBuf[128];
				if (LoadString(hInst, IDS_READ_FINISH, strBuf, sizeof(strBuf) / sizeof(strBuf[0])))
				{
					MessageBox(NULL, strBuf, szTitle, MB_OK);
				}
			}
		}
	}
	else
	{
		TCHAR strBuf[128];
		if (LoadString(hInst, IDS_ERROR_CARDINFO, strBuf, sizeof(strBuf) / sizeof(strBuf[0])))
		{
			MessageBox(m_hWnd, strBuf, szTitle, MB_OK);
		}
	}
	r = usbd_detach_device();

	return res;
}

BOOL WriteToFile()
{
	IFileSaveDialog *pDlg;
	COMDLG_FILTERSPEC FileTypes[] = {
		{ L"PS2 MemoryCard files", L"*.ps2" },
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
	pDlg->SetDefaultExtension(L"ps2");
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
					WriteFile(hFile, &(byteMemDat.Byte), sizeof(byteMemDat), &BytesWrite, NULL);
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
BOOL WriteToCard()
{
	//確認メッセージ
	TCHAR msg[255];
	if (LoadString(hInst, IDS_WRITE_CARD, msg, sizeof(msg) / sizeof(msg[0])))
	{
		if (MessageBox(m_hWnd, msg, szTitle, MB_OKCANCEL) != IDOK)
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	//ブロック毎のページ数が0だったら書き込まない
	if (byteMemDat.Superblock.pages_per_block == 0)
	{
		if (LoadString(hInst, IDS_CANNOT_WRITE, msg, sizeof(msg) / sizeof(msg[0])))
		{
			MessageBox(m_hWnd, msg, szTitle, MB_OK | MB_ICONEXCLAMATION);
		}
		return FALSE;
	}

	int r;
	BOOL res = TRUE;
	//メモリーカードアダプタ接続
	r = usbd_attach_device(0, 0);

	if (!r)
	{
		TCHAR strBuf[128] = {0};	//メッセージ表示用

		mcio_init();	//返り値は無視

		int srcpages = byteMemDat.Superblock.clusters_per_card * byteMemDat.Superblock.pages_per_cluster;	//カード全体のページ数
		int srcblocks = srcpages / byteMemDat.Superblock.pages_per_block;	//カード全体のブロック数

		//プログレスバー更新
		SetProgressBar(srcblocks);

		int srcpagesize = (byteMemDat.Superblock.page_len + (byteMemDat.Superblock.page_len >> 5));	//ページサイズは512+16
		int srcblocksize = srcpagesize * byteMemDat.Superblock.pages_per_block;	//ブロックサイズはpagesize*pages_per_block
		int srccardsize = srcblocksize*srcblocks;

		//イメージファイルが8MB以上だったら書かない
		if (srccardsize > sizeof(byteMemDat))
		{
			res = FALSE;
			LoadString(hInst, IDS_NOTSUPPORTOVER8MB, strBuf, sizeof(strBuf) / sizeof(strBuf[0]));
		}
		else
		{
			//書き込む先のカード情報取得
			int dstpagesize, dstblocksize, dstcardsize, dstcardflags;
			r = mcio_mcGetInfo(&dstpagesize, &dstblocksize, &dstcardsize, &dstcardflags);

			if (!r)
			{
				//イメージファイルのページサイズ(ecc抜き)*ページ数と書き込み先サイズが合わなかったら書かない
				if ((byteMemDat.Superblock.page_len * srcpages) != dstcardsize)
				{
					res = FALSE;
					LoadString(hInst, IDS_NOTMATCH_CARDSIZE, strBuf, sizeof(strBuf) / sizeof(strBuf[0]));
				}
				else
				{
					//書き込み処理
					uint8_t** pagebufarray = (uint8_t**)malloc(sizeof(uint8_t*)*byteMemDat.Superblock.pages_per_block);	//バッファ配列
					uint8_t** eccbufarray = (uint8_t**)malloc(sizeof(uint8_t*)*byteMemDat.Superblock.pages_per_block);	//バッファ配列
					for (int i = 0; i < srcblocks; i++)
					{
						for (int p = 0; p < byteMemDat.Superblock.pages_per_block; p++)
						{
							pagebufarray[p] = &(byteMemDat.Byte[(i*srcblocksize) + p*srcpagesize]);	//ページバッファ
							eccbufarray[p] = &(byteMemDat.Byte[(i*srcblocksize) + (p*srcpagesize) + byteMemDat.Superblock.page_len]);	//ECCバッファはページデータに続いた場所にある
						}

						//書き込みはブロック単位
						int r = mcio_mcWriteBlock(i, pagebufarray, eccbufarray);
						if (r)
						{
							//失敗したときの処理
							res = FALSE;
							TCHAR strBuf2[128];
							if (LoadString(hInst, IDS_ERROR_WRITEBLOCK, strBuf2, sizeof(strBuf2) / sizeof(strBuf2[0])))
							{
								_stprintf_s(strBuf, sizeof(strBuf) / sizeof(strBuf[0]), strBuf2, i);
							}
							break;
						}
						ProgressBar_Step();
					}
					//後処理
					free(pagebufarray);
					free(eccbufarray);
				}
			}
			else
			{
				res = FALSE;
				LoadString(hInst, IDS_ERROR_TARGETCARDSIZE, strBuf, sizeof(strBuf) / sizeof(strBuf[0]));
			}
		}
		if (res)
		{
			//成功時のメッセージ
			LoadString(hInst, IDS_WRITE_FINISH, strBuf, sizeof(strBuf) / sizeof(strBuf[0]));
		}
		MessageBox(NULL, strBuf, szTitle, MB_OK);
	}
	else
	{
		TCHAR strBuf[128];
		if (LoadString(hInst, IDS_ERROR_DEVICE_ATTACH, strBuf, sizeof(strBuf) / sizeof(strBuf[0])))
		{
			MessageBox(m_hWnd, strBuf, szTitle, MB_OK);
		}
	}
	r = usbd_detach_device();
	return res;
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

//ファイルから読んだ場合のファイルの情報表示
BOOL UpdateFileInfo(PS2MEMORYCARD* data)
{
	HWND hWnd = GetDlgItem(m_hWnd, IDC_LIST2);
	//リストクリア
	ListView_DeleteAllItems(hWnd);

	//メモリーカード情報表示
	//本当はゲームリストを表示したい

	//カードのバージョン
	TCHAR strDesc[] = _T("Version");
	LVITEM lvi = { 0, };
	lvi.pszText = strDesc;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(hWnd);
	ListView_InsertItem(hWnd, (LPLVITEM)& lvi);

	char strVersion[12];
	StringCchCopyA(strVersion, sizeof(strVersion) / sizeof(strVersion[0]), (char*)& data->Superblock.version);
	//文字コード変換
	CA2T wstrVersion(strVersion);
	ListView_SetItemText(hWnd, (WPARAM)ListView_GetItemCount(hWnd) - 1, 1, wstrVersion);

	//Clusters/Card
	lvi = { 0, };
	lvi.pszText = _T("Clusters/Card");
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(hWnd);
	ListView_InsertItem(hWnd, (LPLVITEM)& lvi);

	TCHAR strClusters_per_card[10];
	_stprintf_s(strClusters_per_card, sizeof(strClusters_per_card) / sizeof(strClusters_per_card[0]), _T("%d"), data->Superblock.clusters_per_card);
	ListView_SetItemText(hWnd, (WPARAM)ListView_GetItemCount(hWnd) - 1, 1, strClusters_per_card);

	//Pages/Cluster
	lvi = { 0, };
	lvi.pszText = _T("Pages/Cluster");
	lvi.mask = LVIF_TEXT;
	lvi.iItem = ListView_GetItemCount(hWnd);
	ListView_InsertItem(hWnd, (LPLVITEM)& lvi);

	TCHAR strPages_per_cluster[10];
	_stprintf_s(strPages_per_cluster, sizeof(strPages_per_cluster) / sizeof(strPages_per_cluster[0]), _T("%d"), data->Superblock.pages_per_cluster);
	ListView_SetItemText(hWnd, (WPARAM)ListView_GetItemCount(hWnd) - 1, 1, strPages_per_cluster);

	ListView_SetColumnWidth(hWnd, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hWnd, 1, LVSCW_AUTOSIZE_USEHEADER);
	return TRUE;
}

//カードから読んだ場合のデータリスト更新
BOOL UpdateDataList(PS2MEMORYCARD* data)
{
	HWND hWnd = GetDlgItem(m_hWnd, IDC_LIST2);
	//リストクリア
	ListView_DeleteAllItems(hWnd);

	int fd;
	int r;
	fd = mcio_mcDopen(".");
	if (fd >= 0) {
		struct io_dirent dirent;
		_RPTN(_CRT_WARN, "---------- Filename ----------  |  Type  |  Size  | Last Modification (UTC)\n", NULL);
		do {
			r = mcio_mcDread(fd, &dirent);
			if ((r) && (strcmp(dirent.name, ".")) && (strcmp(dirent.name, ".."))) { 
				int tabnum = (32 / 7) - (strlen(dirent.name) / 7);
				_RPTN(_CRT_WARN, "%s ", dirent.name);
				int i;
				for (i = 0; i<tabnum; i++)
					_RPTN(_CRT_WARN, "\t", NULL);
				if (!(dirent.stat.mode & sceMcFileAttrSubdir))
				{
					_RPTN(_CRT_WARN, "| <file> | ", NULL);
					char strTitle[256];
					StringCchCopyA(strTitle, sizeof(strTitle) / sizeof(strTitle[0]), dirent.name);
					//文字コード変換
					CA2T wstrTitle(strTitle);
					LVITEM lvi = { 0, };
					lvi.pszText = wstrTitle;
					lvi.mask = LVIF_TEXT;
					lvi.iItem = ListView_GetItemCount(hWnd);
					ListView_InsertItem(hWnd, (LPLVITEM)& lvi);
				}
				else
				{
					_RPTN(_CRT_WARN, "| <dir>  | ", NULL);
					char strPath[256];
					StringCbPrintfA(strPath, sizeof(strPath) / sizeof(strPath[0]), dirent.name);
					//フォルダごとの処理
					int fd;
					fd = mcio_mcDopen(strPath);
					if (fd >= 0) {
						int r;
						struct io_dirent subdirent;
						//フォルダ内の各ファイルを探す
						do {
							r = mcio_mcDread(fd, &subdirent);
							if ((r) && (strcmp(subdirent.name, ".")) && (strcmp(subdirent.name, "..")))
							{
								//ファイルごとの処理
								char strPath2[520];
								StringCbPrintfA(strPath2, sizeof(strPath2) / sizeof(strPath2[0]), "%s/%s", strPath, subdirent.name);
								//ファイルを開く
								int fd;
								fd = mcio_mcOpen(strPath2, sceMcFileAttrReadable | sceMcFileAttrFile);
								if (fd >= 0) {
									char buf[sizeof(ICON_SYS)];
									mcio_mcRead(fd, &buf, sizeof(ICON_SYS));
									mcio_mcClose(fd);
									//PS2Dで始まったらPS2データ
									if (!strncmp((char*)((ICON_SYS*)buf)->PS2D, "PS2D", sizeof(((ICON_SYS*)buf)->PS2D)))
									{
										ICON_SYS* buf_icon_sys = (ICON_SYS*)buf;
										char strTitle[sizeof(buf_icon_sys->title_name_of_savegame)];
										StringCbPrintfA(strTitle, sizeof(buf_icon_sys->title_name_of_savegame), "%s", (buf_icon_sys->title_name_of_savegame));
										//文字コード変換
										CA2T wstrTitle(strTitle);
										LVITEM lvi = { 0, };
										lvi.pszText = wstrTitle;
										lvi.mask = LVIF_TEXT;
										lvi.iItem = ListView_GetItemCount(hWnd);
										ListView_InsertItem(hWnd, (LPLVITEM)& lvi);
									}
									else if (!strncmp((char*)buf, "SC", 2))
									{
										//"SC"で始まったらPS1データ
										char strTitle[520];
										StringCbPrintfA(strTitle, 64, "%s", ((char*)& buf) + 0x04);
										//文字コード変換
										CA2T wstrTitle(strTitle);
										LVITEM lvi = { 0, };
										lvi.pszText = wstrTitle;
										lvi.mask = LVIF_TEXT;
										lvi.iItem = ListView_GetItemCount(hWnd);
										ListView_InsertItem(hWnd, (LPLVITEM)& lvi);
									}
								}
							}
						} while (r);
						mcio_mcDclose(fd);
					}
				}
				_RPTN(_CRT_WARN, "%-7d", dirent.stat.size);
				_RPTN(_CRT_WARN, "%02d/%02d/%04d-", dirent.stat.mtime.Month, dirent.stat.mtime.Day, dirent.stat.mtime.Year);
				_RPTN(_CRT_WARN, "%02d:%02d:%02d", dirent.stat.mtime.Hour, dirent.stat.mtime.Min, dirent.stat.mtime.Sec);
				_RPTN(_CRT_WARN, "\n", NULL);
			}
		} while (r);

		mcio_mcDclose(fd);
	}

	ListView_SetColumnWidth(hWnd, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hWnd, 1, LVSCW_AUTOSIZE_USEHEADER);
	return TRUE;
}
