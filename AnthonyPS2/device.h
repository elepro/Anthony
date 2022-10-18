//
// Define below GUIDs
//
#include <initguid.h>

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
// a5dcbf10-6530-11d2-901f-00c04fb951ed
//
//旧ドライバで使っていたGUID
//DEFINE_GUID(GUID_DEVINTERFACE_USBApplication1,
//    0x436366c2,0x1c9c,0x408d,0x8a,0x18,0x79,0x4a,0x3c,0xed,0xce,0xfa);
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE,
    0xa5dcbf10, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);

typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    );

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
    );
