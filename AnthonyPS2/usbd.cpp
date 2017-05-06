/*
 * ps3mca-tool - PlayStation 3 Memory Card Adaptor Software
 * Copyright (C) 2011 - jimmikaelkael <jimmikaelkael@wanadoo.fr>
 * Copyright (C) 2011 - "someone who wants to stay anonymous"
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "device.h"
#include "usbd.h"

#include <stdio.h>
#include <usb.h>

#define USBD_DEVICE_NOT_FOUND            -1
#define USBD_OPEN_FAILED                 -2
#define USBD_CLAIM_IF_FAILED             -3
#define USBD_DEVICE_NOT_ATTACHED         -4
#define USBD_BULK_WRITE_FAILED           -5
#define USBD_BULK_READ_FAILED            -6
#define USBD_FAILED_RETRIEVE_DESCRIPTORS -7

static DEVICE_DATA	deviceData;
BOOL SetupWinUsb(DEVICE_DATA *deviceData);

//struct usbd_interface_desc {
//	usb_dev_handle *devh;
//	int interface;
//};

//struct usbd_interface_desc g_interface_descriptor;
//struct usbd_interface_desc *interface_descriptor = &g_interface_descriptor;

/*
 * probeVSCinterface: check an USB interface is Vendor Specific Class
 * returns USB interface number or -1 if not probed
 */
//static int probeVSCinterface(struct usb_device *dev)
//{
//	int i;
//	for (i=0; i<dev->config->bNumInterfaces; i++) {
//		/* filter Vendor Specific Class */
//		if (dev->config->interface[i].altsetting->bInterfaceClass == USB_CLASS_VENDOR_SPEC) {
//			return dev->config->interface[i].altsetting->bInterfaceNumber;
//		}
//	}
//
//	return -1;
//}

/*
 * usbd_detach_device: detach previously attached device
 * returns 0 if success, otherwise negative value
 */
int usbd_detach_device(void)
{
	CloseDevice(&deviceData);
	//if (interface_descriptor->devh == NULL) {
	//	return USBD_DEVICE_NOT_ATTACHED;
	//}

	//usb_release_interface(interface_descriptor->devh, interface_descriptor->interface);
	//usb_close(interface_descriptor->devh);

	//interface_descriptor->devh = NULL;
	//interface_descriptor->interface = -1;

	return 0;
}

/*
 * usbd_attach_device: bound the code to a specific device
 * idVendor: USB product Vendor ID
 * idProduct: USB product ID
 * returns 0 if success, otherwise negative value
 */
int usbd_attach_device(uint16_t idVendor, uint16_t idProduct)
{
	if (SetupWinUsb(&deviceData))
	{
		return 0;
	}
	else
	{
		return -1;
	}

	//int ret;

	//if (interface_descriptor->devh != NULL)
	//	usbd_detach_device();

	//usb_init();
	//usb_find_busses();
	//usb_find_devices();

	//struct usb_bus *busses = usb_get_busses();

	//struct usb_bus *bus;
	//for (bus = busses; bus; bus = bus->next) { /* busses loop start */

	//	struct usb_device *dev;
	//	for (dev = bus->devices; dev; dev = dev->next) { /* bus devices loop start */

	//		/* probe Vendor Specific Class interface type */
	//		int iface = probeVSCinterface(dev);
	//		if (iface < 0)
	//			continue;

	//		usb_dev_handle *udev;

	//		if ((dev->descriptor.idVendor == idVendor) && \
	//			(dev->descriptor.idProduct == idProduct)) {

	//			if (!dev->config) {
	//				return USBD_FAILED_RETRIEVE_DESCRIPTORS;
	//			}

	//			udev = usb_open(dev);
	//			if (!udev) {
	//				return USBD_OPEN_FAILED;
	//			}

	//			#ifndef _WIN32 /* Linux system */
	//			/* detach other drivers        */
	//			char name[128];
	//			usb_get_driver_np(udev, iface, name, 128);
	//			usb_detach_kernel_driver_np(udev, iface);
	//			#endif

	//			/* bConfigurationValue is 1 */
	//			usb_set_configuration(udev, 1);

	//			ret = usb_claim_interface(udev, iface);
	//			if (ret < 0) {
	//				return USBD_CLAIM_IF_FAILED;
	//			}

	//			/* fill interface descriptor */
	//			interface_descriptor->devh = udev;
	//			interface_descriptor->interface = iface;

	//			return 0;
	//		}

	//	} /* end bus devices loop */
	//} /* end busses loop              */

	//return USBD_DEVICE_NOT_FOUND;
}

/*
 * usbd_bulk_write: does bulk write on device
 * buf: data buffer to write
 * bsize: data buffer size
 * returns number of bytes written if success, otherwise negative value
 */
int usbd_bulk_write(uint8_t *buf, int bsize)
{
	ULONG Transfered = 0;
	WinUsb_WritePipe(deviceData.WinusbHandle, 0x02, (uint8_t *)buf, bsize, &Transfered, NULL);
	return Transfered;
	//return usb_bulk_write(interface_descriptor->devh, 0x02, (char *)buf, bsize, 500);
}

/*
 * usbd_bulk_read:  does bulk read on device
 * buf: buffer to read datas
 * bsize: max data buffer size
 * returns number of bytes readed if success, otherwise negative value
 */
int usbd_bulk_read(uint8_t *buf, int bsize)
{
	ULONG Transfered = 0;
	WinUsb_ReadPipe(deviceData.WinusbHandle, 0x81, (uint8_t *)buf, bsize, &Transfered, NULL);
	return Transfered;
	//return usb_bulk_read(interface_descriptor->devh, 0x81, (char *)buf, bsize, 500);
}


/*
 * Bus 002 Device 005: ID 054c:02ea Sony Corp.
 * Device Descriptor:
 *   bLength                18
 *   bDescriptorType         1
 *   bcdUSB               1.10
 *   bDeviceClass          255 Vendor Specific Class
 *   bDeviceSubClass         0
 *   bDeviceProtocol       255
 *   bMaxPacketSize0        64
 *   idVendor           0x054c Sony Corp.
 *   idProduct          0x02ea
 *   bcdDevice            1.00
 *   iManufacturer           0
 *   iProduct                0
 *   iSerial                 0
 *   bNumConfigurations      1
 *   Configuration Descriptor:
 *     bLength                 9
 *     bDescriptorType         2
 *     wTotalLength           39
 *     bNumInterfaces          1
 *     bConfigurationValue     1
 *     iConfiguration          0
 *     bmAttributes         0x80
 *       (Bus Powered)
 *     MaxPower              200mA
 *     Interface Descriptor:
 *       bLength                 9
 *       bDescriptorType         4
 *       bInterfaceNumber        0
 *       bAlternateSetting       0
 *       bNumEndpoints           3
 *       bInterfaceClass       255 Vendor Specific Class
 *       bInterfaceSubClass      0
 *       bInterfaceProtocol    255
 *       iInterface              0
 *       Endpoint Descriptor:
 *         bLength                 7
 *         bDescriptorType         5
 *         bEndpointAddress     0x81  EP 1 IN
 *         bmAttributes            2
 *           Transfer Type            Bulk
 *           Synch Type               None
 *           Usage Type               Data
 *         wMaxPacketSize     0x0040  1x 64 bytes
 *         bInterval               0
 *       Endpoint Descriptor:
 *         bLength                 7
 *         bDescriptorType         5
 *         bEndpointAddress     0x02  EP 2 OUT
 *         bmAttributes            2
 *           Transfer Type            Bulk
 *           Synch Type               None
 *           Usage Type               Data
 *         wMaxPacketSize     0x0040  1x 64 bytes
 *         bInterval               0
 *       Endpoint Descriptor:
 *         bLength                 7
 *         bDescriptorType         5
 *         bEndpointAddress     0x83  EP 3 IN
 *         bmAttributes            3
 *           Transfer Type            Interrupt
 *           Synch Type               None
 *           Usage Type               Data
 *         wMaxPacketSize     0x0001  1x 1 bytes
 *         bInterval             100
 */

