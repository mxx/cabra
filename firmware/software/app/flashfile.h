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
	char type;
	unsigned char start_block;
	unsigned char total_block;
	unsigned char block_limit;
	unsigned int  write_pos;
};

struct block_info
{
	char type;
	unsigned char next_block;
	unsigned char write_count;
};

struct block_info block_map[256];

struct record_block_head
{
	unsigned int StartTimeTage;
	unsigned short RecorderNumber;
};

/*Insert a record
 *
 */
int insert_record(char type, const char* pData, short size);

int query_record(char type, unsigned int start_time, unsigned int number, char* pBuffer);


#endif /* FLASHFILE_H_ */
