/*
 * flashfile.c
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */
#include "flashfile.h"

struct block_info block_map[256];

int flashfile_system_init(void)
{
	return 0;
}

int flashfile_append(const char file_id,const char* ptrData,const int size)
{
	return 0;
}
int flashfile_read(const char file_id, const int offset, char* ptrData,const int size)
{
	return 0;
}

int flashfile_get_first_write_size(const char file_id)
{
	return 0;
}
