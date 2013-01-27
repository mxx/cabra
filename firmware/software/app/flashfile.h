/*
 * flashfile.h
 *
 *  Created on: Sep 15, 2012
 *      Author: mxx
 */

#ifndef FLASHFILE_H_
#define FLASHFILE_H_

struct flashfile
{
	char file_id;
	unsigned char start_block;
	unsigned char total_block;
	unsigned char block_limit;
	unsigned int  write_pos;
};

struct block_info
{
	unsigned short file_id:4;
	unsigned short first_write_size:12;
	unsigned char prev_block;
	unsigned char write_count;
};

typedef struct block_info FlashBlockHead;



struct record_block_head
{
	unsigned int StartTimeTag;
	unsigned short RecorderNumber;
};

int flashfile_system_init(void);
int flashfile_append(const char file_id,const char* ptrData,const int size);
int flashfile_read(const char file_id, const int offset, char* ptrData,const int size);
int flashfile_get_first_write_size(const char file_id);

#endif /* FLASHFILE_H_ */
