#pragma once

#include "resource.h"

union MEMORYCARD
{
	byte Block[16][8 * 1024];

	byte Frame[16 * 64][128];
	byte Byte[128 * 1024];
};

