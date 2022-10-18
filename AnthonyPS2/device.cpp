#include "stdafx.h"

#include <SetupAPI.h>
#include "device.h"

HRESULT
RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound
);

HRESULT
OpenDevice(
	_Out_     PDEVICE_DATA DeviceData,
	_Out_opt_ PBOOL        FailureDeviceNotFound
)
/*++

Routine description:

	Open all needed handles to interact with the device.

	If the device has multiple USB interfaces, this function grants access to
	only the first interface.

	If multiple devices have the same device interface GUID, there is no
	guarantee of which one will be returned.

Arguments:

	DeviceData - Struct filled in by this function. The caller should use the
		WinusbHandle to interact with the device, and must pass the struct to
		CloseDevice when finished.

	FailureDeviceNotFound - TRUE when failure is returned due to no devices
		found with the correct device interface (device not connected, driver
		not installed, or device is disabled in Device Manager); FALSE
		otherwise.

Return value:

	HRESULT

--*/
{
	HRESULT hr = S_OK;
	BOOL    bResult;

	DeviceData->HandlesOpen = FALSE;

	hr = RetrieveDevicePath(DeviceData->DevicePath,
		sizeof(DeviceData->DevicePath),
		FailureDeviceNotFound);

	if (FAILED(hr)) {

		return hr;
	}

	DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	bResult = WinUsb_Initialize(DeviceData->DeviceHandle,
		&DeviceData->WinusbHandle);

	if (FALSE == bResult) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		CloseHandle(DeviceData->DeviceHandle);
		return hr;
	}

	DeviceData->HandlesOpen = TRUE;
	return hr;
}

VOID
CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData
)
/*++

Routine description:

	Perform required cleanup when the device is no longer needed.

	If OpenDevice failed, do nothing.

Arguments:

	DeviceData - Struct filled in by OpenDevice

Return value:

	None

--*/
{
	if (FALSE == DeviceData->HandlesOpen) {

		//
		// Called on an uninitialized DeviceData
		//
		return;
	}

	WinUsb_Free(DeviceData->WinusbHandle);
	CloseHandle(DeviceData->DeviceHandle);
	DeviceData->HandlesOpen = FALSE;

	return;
}

HRESULT
RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound
)
/*++

Routine description:

	Retrieve the device path that can be used to open the WinUSB-based device.

	If multiple devices have the same device interface GUID, there is no
	guarantee of which one will be returned.

Arguments:

	DevicePath - On successful return, the path of the device (use with CreateFile).

	BufLen - The size of DevicePath's buffer, in bytes

	FailureDeviceNotFound - TRUE when failure is returned due to no devices
		found with the correct device interface (device not connected, driver
		not installed, or device is disabled in Device Manager); FALSE
		otherwise.

Return value:

	HRESULT

--*/
{
	BOOL                             bResult = FALSE;
	HDEVINFO                         deviceInfo;
	SP_DEVINFO_DATA                  infoData;
	SP_DEVICE_INTERFACE_DATA         interfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
	ULONG                            length;
	ULONG                            requiredLength = 0;
	HRESULT                          hr = HRESULT_FROM_WIN32(S_OK);
	BOOL							 found = false;
	UINT							 infoIndex = 0;

	if (NULL != FailureDeviceNotFound) {

		*FailureDeviceNotFound = FALSE;
	}

	//
	// Enumerate all devices exposing the interface
	//
	deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (deviceInfo == INVALID_HANDLE_VALUE) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	infoData.cbSize = sizeof(infoData);

	int memberIndex = 0;
	while (!found)
	{
		//
		// Get the interface in the result set
		//
		bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
			NULL,
			&GUID_DEVINTERFACE_USB_DEVICE,
			memberIndex,
			&interfaceData);
		memberIndex++;

		if (FALSE == bResult) {

			//
			// We would see this error if no devices were found
			//
			if (ERROR_NO_MORE_ITEMS == GetLastError() &&
				NULL != FailureDeviceNotFound) {

				*FailureDeviceNotFound = TRUE;
			}
			hr = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		//
		// Get the size of the path string
		// We expect to get a failure with insufficient buffer
		//
		bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
			&interfaceData,
			NULL,
			0,
			&requiredLength,
			NULL);

		if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

			hr = HRESULT_FROM_WIN32(GetLastError());
			continue;
		}

		//
		// Allocate temporary space for SetupDi structure
		//
		detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, requiredLength);

		if (NULL == detailData)
		{
			hr = E_OUTOFMEMORY;
			continue;
		}

		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		length = requiredLength;

		//
		// Get the interface's path string
		//
		bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
			&interfaceData,
			detailData,
			length,
			&requiredLength,
			NULL);

		if (FALSE == bResult)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			LocalFree(detailData);
			continue;
		}

		//
		// Give path to the caller. SetupDiGetDeviceInterfaceDetail ensured
		// DevicePath is NULL-terminated.
		//
		hr = StringCbCopy(DevicePath,
			BufLen,
			detailData->DevicePath);

		LocalFree(detailData);
		if (StrStr(DevicePath, _T("vid_054c&pid_02ea")) != NULL)
		{
			found = TRUE;
			break;
		}
	}
	SetupDiDestroyDeviceInfoList(deviceInfo);
	DevicePath[((static_cast<unsigned long long>(BufLen)) - 1) / sizeof(TCHAR)] = 0;
	return hr;
}
