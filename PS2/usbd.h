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

#ifndef __USBD_H__
#define __USBD_H__

#include <inttypes.h>

#define USBIO_DATA_HDRLEN   4
#define USBIO_BUFFER_SIZE   1024

#pragma pack(1)
struct usbio_data {
	uint16_t dev_data;
	uint8_t  mc_data_len;
	uint8_t  pad;
	uint8_t  mc_data[USBIO_BUFFER_SIZE];
}/* __attribute__((packed))*/;
#pragma pack()

int usbd_detach_device(void);
int usbd_attach_device(uint16_t idVendor, uint16_t idProduct);
int usbd_bulk_write(uint8_t *buf, int bsize);
int usbd_bulk_read(uint8_t *buf, int bsize);

#endif

