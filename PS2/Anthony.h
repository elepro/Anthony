#pragma once

#include "resource.h"

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
#pragma pack()

union PS2MEMORYCARD
{
	byte Byte[8650752];//[8 * 1024 * 1024];
	MC2 Superblock;
};

