// Win32Project1.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "Anthony.h"
#include "device.h"
#include "Atlconv.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��
HWND m_hWnd;									//�E�C���h�E�n���h��
MEMORYCARD byteMemDat;	//�������[�J�[�h�f�[�^�i�[��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
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

	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	//	MyRegisterClass(hInstance);

		// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	// ���C�� ���b�Z�[�W ���[�v:
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
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
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
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

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
		//�S��폜
		while (Header_GetItemCount(ListView_GetHeader(GetDlgItem(hDlg, IDC_LIST2))))
			ListView_DeleteColumn(GetDlgItem(hDlg, IDC_LIST2), 0);

		//��쐬
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

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
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

	//�����t�H���_�̎w��
	pDlg->SetFolder(psiFolder);
	//�t�B���^�[�̎w��
	pDlg->SetFileTypes(_countof(FileTypes), FileTypes);
	//�_�C�A���O�\��
	hr = pDlg->Show(NULL);

	//�t�@�C����
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

//�������[�J�[�h����ǂݍ��ށB
//�R�}���h�t�H�[�}�b�g�́A
//�A�_�v�^�[�ւ̃R�}���h4�o�C�g+�i�������[�J�[�h�ւ̃R�}���h140�o�C�g�j
//�A�_�v�^�[�ւ̃R�}���h4�o�C�g�ɂ���
//1�o�C�g�ځF0xaa(�Œ�j
//2�o�C�g�ځF0x42(�Œ�j
//1�o�C�g�ځF0x8c(�Œ�A��ɑ����f�[�^�T�C�Y�j
//1�o�C�g�ځF0x00(�Œ�j
//�������[�J�[�h�ւ̃R�}���h140�o�C�g�ɂ��ẮA���c�������Q��
BOOL ReadFromCard()
{
	DEVICE_DATA	deviceData;
	BOOL b = SetupWinUsb(&deviceData);
	if (b)
	{
		int frames = 64 * 16;
		//�v���O���X�o�[�X�V
		SetProgressBar(frames);

		ULONG Length;
		UCHAR OutData[144] = { 0xaa, 0x42, 0x8c, 0x00, 0x81, 0x52, 0x0 };
		UCHAR InData[144] = { 0x0 };
		//	OVERLAPPED ol;
		for (int i = 0; i < frames; i++)
		{
			//�A�h���X�w��
			OutData[8] = (i & 0xff00) >> 8;
			OutData[9] = (i & 0xff);

			WinUsb_ResetPipe(deviceData.WinusbHandle, 2);
			WinUsb_ResetPipe(deviceData.WinusbHandle, 129);
			b = WinUsb_WritePipe(deviceData.WinusbHandle, 2, OutData, 144, &Length, NULL);
			DWORD d;
			d = GetLastError();
			//		Sleep(100);
			b = WinUsb_ReadPipe(deviceData.WinusbHandle, 129, InData, 144, &Length, NULL);

			//�߂�̐擪2byte��0x55,0x5a����Ȃ�������G���[�Ƃ���
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
				//�ǂݍ��񂾃f�[�^���i�[
				//���f�[�^��0x0e�o�C�g�ڂ���n�܂�B
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

	//�����t�H���_�̎w��
	pDlg->SetFolder(psiFolder);
	//�t�B���^�[�̎w��
	pDlg->SetFileTypes(_countof(FileTypes), FileTypes);
	//�g���q���Ȃ������ꍇ�ɕt������g���q
	pDlg->SetDefaultExtension(L"mem");
	//�_�C�A���O�\��
	hr = pDlg->Show(m_hWnd);

	//�t�@�C����
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

//�������[�J�[�h�ɏ������ށB
//�R�}���h�t�H�[�}�b�g�́A
//�A�_�v�^�[�ւ̃R�}���h4�o�C�g+�i�������[�J�[�h�ւ̃R�}���h138�o�C�g�j
//�A�_�v�^�[�ւ̃R�}���h4�o�C�g�ɂ���
//1�o�C�g�ځF0xaa(�Œ�j
//2�o�C�g�ځF0x42(�Œ�j
//1�o�C�g�ځF0x8a(�Œ�A��ɑ����f�[�^�T�C�Y�j
//1�o�C�g�ځF0x00(�Œ�j
//�������[�J�[�h�ւ̃R�}���h138�o�C�g�ɂ��ẮA���c�������Q��
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
				//�v���O���X�o�[�X�V
				SetProgressBar(frames);

				ULONG Length;
				UCHAR OutData[142] = { 0xaa, 0x42, 0x8a, 0x00, 0x81, 0x57, 0x0 };
				UCHAR InData[142] = { 0x0 };
				//	OVERLAPPED ol;
				for (int i = 0; i < frames; i++)
				{
					//�A�h���X�w��
					OutData[8] = (i & 0xff00) >> 8;
					OutData[9] = (i & 0xff);
					//�f�[�^128�o�C�g
					memcpy(&OutData[10], &byteMemDat.Frame[i][0], 128);
					//XOR�t���O���擾
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
						//�ǂݍ��񂾃f�[�^���i�[
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

//XOR�l�����߂�
//�^����f�[�^�́A�A�h���X2byte+�f�[�^128byte
//�e�o�C�g�f�[�^�ɂ���XOR���������l��Ԃ��B
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
	TCHAR strBuf[128];	//��Ɨp�o�b�t�@

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

//�v���O���X�o�[�̐ݒ�
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

//�f�[�^���X�g���X�V
BOOL UpdateDataList(MEMORYCARD* data)
{
	HWND hWnd = GetDlgItem(m_hWnd, IDC_LIST2);
	//���X�g�N���A
	ListView_DeleteAllItems(hWnd);

	//�f�[�^���\��
	for (int i = 0; i < 16; i++)
	{
		//�u���b�N���g�p���ŁA�Z�[�u�f�[�^�̐擪�u���b�N��
		if ((data->Block[0][128 * i]) == 0x51)
		{
			if (data->Block[i][0] == 'S' && data->Block[i][1] == 'C')
			{
				char strTitle[0x5f - 0x03];
				LVITEM lvi = { 0, };
				//strcpy_s(strTitle, sizeof(strTitle), (char*)&data->Block[i][4]);
				StringCchCopyA(strTitle, sizeof(strTitle), (char*)&data->Block[i][4]);
				//�����R�[�h�ϊ�
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