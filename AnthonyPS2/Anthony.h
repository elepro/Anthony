#pragma once

#include "resource.h"

//PlayStation 2 icon.sys format https://www.ps2savetools.com/documents/iconsys-format/
//PlayStation 2 Memory Card File System http://www.csclub.uwaterloo.ca:11068/mymc/ps2mcfs.html
#pragma pack(1)
struct MC2
{
	byte magic[0x1C];				// -Identifies the card as being formatted.Set to the ASCII string "Sony PS2 Memory Card Format ".
	byte version[12];// = { '1','.','X','.','0','.','0' };	// 	 	Version number of the format used.
														//Version 1.2 indicates full support for bad_block_list.
	uint16_t page_len;// = 512;				//Size in bytes of a memory card page.
	uint16_t pages_per_cluster;// = 2;		//The number of pages in a cluster.
	uint16_t pages_per_block;// = 16;		//The number of pages in an erase block.
	uint16_t unused2E;// = 0xFF00;			//Doesn't seem to be used
	uint32_t clusters_per_card;// = 8192;	//The total size of the card in clusters.
	uint32_t alloc_offset;// = 41;			//Cluster offset of the first allocatable cluster.Cluster values in the FAT and directory entries are relative to this.
									//This is the cluster immediately after the FAT
	uint32_t alloc_end;// = 8135;			//The cluster after the highest allocatable cluster.Relative to alloc_offset.
									//Not used.
	uint32_t rootdir_cluster;// = 0;			//First cluster of the root directory.Relative to alloc_offset.
										//Must be zero.
	uint32_t backup_block1;// = 1023;		//Erase block used as a backup area during programming.
									//Normally the the last block on the card, it may have a different value if that block was found to be bad.
	uint32_t backup_block2;// = 1022;		//This block should be erased to all ones.
									//Normally the the second last block on the card.
	uint32_t ifc_list[32];// = { 8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8 };
	//List of indirect FAT clusters.
	//On a standard 8M card there's only one indirect FAT cluster.
	uint32_t bad_block_list[32];// = { -1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, };
	//List of erase blocks that have errors and shouldn't be used.
	byte card_type;// = 2;				//Memory card type.
									//Must be 2, indicating that this is a PS2 memory card.
	byte card_flags;// = 0x52;			//Physical characteristics of the memory card.
};

struct ICON_SYS
{
	byte PS2D[4];
	uint16_t dummy;
	uint16_t offset_of_2nd_line_in_titlename;
	int32_t dummy8;
	int32_t background_transparency;	//background transparency when save is selected in ps2 browser. 0x00(transparent)to 0x80(opaque)
	byte background_color_upper_left[16];	//background color upper left(RGB - ) 0x00 – 0x80
	byte background_color_upper_right[16];	//background color upper right(RGB - ) 0x00 – 0x80
	byte background_color_lower_left[16];	//background color lower left(RGB - ) 0x00 – 0x80
	byte background_color_lower_right[16];	//background color lower right(RGB - ) 0x00 – 0x80
	byte light_1_direction[16];	//light 1 direction
	byte light_2_direction[16];	//light 2 direction
	byte light_3_direction[16];	//light 3 direction
	byte light_1_RGB[16];	//light 1 RGB -
	byte light_2_RGB[16];	//light 2 RGB -
	byte light_3_RGB[16];	//Light 3 RGB -
	byte ambient_light_RGB[16];	//ambient light RGB -
	byte title_name_of_savegame[68];	// title name of savegame(null terminated, S - JIS format)
	byte filename_of_normal_icon[64];	//filename of normal icon(null terminated)
	byte filename_of_icon_file_displayed_when_copying[64];	//filename of icon file displayed when copying(null terminated)
	byte filename_of_icon_file_displayed_when_deleting[64];	//filename of icon file displayed when deleting(null terminated)
	byte blank[512];
};
#pragma pack()

union PS2MEMORYCARD
{
	byte Byte[8650752];//[8 * 1024 * 1024];
	MC2 Superblock;
};

