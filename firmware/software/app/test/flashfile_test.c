/*
 * flashfile_test.c
 *
 *  Created on: Feb 3, 2013
 *      Author: mxx
 */

#include "../flash_dev.h"
#include "../flashfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


extern  FlashFile flashFile[];
extern FlashBlockHead block_map[BLOCK_NUMBER];

extern unsigned char nextBlockChain[];

void dump_file_info(int i)
{
	FlashFile* FILE = &flashFile[i];
	printf("\nfileID:%d,lastTime:%u@%d->%d,%d/%d,START:%d\n", FILE->file_id,
			FILE->last_time_tag.time_tag, FILE->last_time_tag_block,
			FILE->last_time_tag_offset, FILE->total_block, FILE->block_limit,
			FILE->start_block);
}

void dump_file_map(int i)
{
	printf("\nFileID:%d, Total:%d\n",i,flashFile[i].total_block);
	if (flashFile[i].total_block)
	{
		printf("Block Chain:");
		int block = flashFile[i].start_block;
		int next = nextBlockChain[block];
		printf(" %d[%d(%d)]->",block,block_map[block].prev_block,block_map[block].write_count);
		while(next != block)
		{
			printf("%d[%d(%d)]->",next,block_map[next].prev_block,block_map[next].write_count);
			block = next;
			next = nextBlockChain[block];
		}
	}
	printf("\n");

}

void flashfile_dump(void)
{
	for (int i = 0; i < FileIDMax; i++)
	{
		dump_file_info(i);
	}
}

// 驱动接口函数，删除指定的块
int flash_erase_block(const int block)
{
	int abs_offset = block * BLOCK_SIZE ;
	int fp = open("test.dat", O_CREAT | O_RDWR);
	char* ptrRaw[BLOCK_SIZE];
	memset(ptrRaw,0xFF,BLOCK_SIZE);
	int rt = 0;
	while (fp > 0)
	{
		if (lseek(fp, abs_offset, SEEK_SET) < 0)
		{
			perror("seek");
			rt = -1;
			break;
		}
		if (!rt && ((rt = write(fp, ptrRaw, BLOCK_SIZE)) != BLOCK_SIZE))
		{
			perror("write");
			rt = -1;
			break;
		}

		break;
	}

	if (fp)
		close(fp);
	return rt;
}

int flash_read_raw(int fp, const int offset, char* ptrData, int size)
{
	if (lseek(fp, offset, SEEK_SET) < 0)
	{
		perror("seek");
		return -1;
	}

	if (read(fp, ptrData, size) < 0)
	{
		perror("read");
		return -1;
	}
	return 0;
}


int flash_write(const int block, const int offset, const char* ptrData,
		const int size)
{
	int abs_offset = block * BLOCK_SIZE + offset;
	int fp = open("test.dat", O_CREAT | O_RDWR);
	char* ptrRaw = (char*) malloc(size);
	int rt = 0;
	while (fp > 0)
	{
		if (flash_read_raw(fp, abs_offset, ptrRaw, size))
		{
			perror("read raw");
			rt = -1;
			break;
		}
		for (int i = 0; i < size; i++)
		{
			ptrRaw[i] &= ptrData[i];
		}
		if (lseek(fp, abs_offset, SEEK_SET) < 0)
		{
			perror("seek");
			rt = -1;
			break;
		}
		if (!rt && ((rt = write(fp, ptrRaw, size)) != size))
		{
			perror("write");
			rt = -1;
			break;
		}

		break;
	}
	if (ptrRaw)
		free(ptrRaw);
	if (fp)
		close(fp);
	return rt;
}

int flash_read(const int block, const int offset, char* ptrData, const int size)
{
	int fp = open("test.dat", O_RDWR);
	int abs_offset = block * BLOCK_SIZE + offset;

	if (fp)
	{
		int rt = 0;
		if (lseek(fp, abs_offset, SEEK_SET) < 0)
		{
			perror("seek");
			close(fp);
			return -1;
		}

		if ((rt = read(fp, ptrData, size)) < 0)
		{
			rt = -1;
		}
		close(fp);
		return rt;
	}
	return -1;
}

void flash_build(void)
{
	char buf[BLOCK_SIZE];
	memset(buf, 0xFF, BLOCK_SIZE);
	FILE* fp = fopen("test.dat", "wb");
	for (int i = 0; i < 256; i++)
	{
		fwrite(buf, BLOCK_SIZE, 1, fp);
	}
	fclose(fp);
}

int main(int argc, char** argv)
{
	flash_build();
	flashfile_system_init();
	flashfile_set_param(SpeedFile, 64, 1, 60);
	char buf[64];
	memset(buf, 0x55, 63);
	buf[0] = 0xAA;
	buf[63] = 0;
	time_t now = time(NULL);

	while (1)
	{
		buf[1]=now%60;
		int rt = flashfile_append_record(SpeedFile, now, buf);
		dump_file_info(SpeedFile);
		dump_file_map(SpeedFile);
		if (rt < 0)
			return -1;
		now++;
		//sleep(1);
	};

}
