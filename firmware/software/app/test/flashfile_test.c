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

int flash_erase_block(const int block)
{
	char buf[BLOCK_SIZE];
	memset(buf,0xFF,BLOCK_SIZE);
	return flash_write(block,0,buf,BLOCK_SIZE);
}


int flash_read_raw(FILE* fp, const int offset, char* ptrData, int size)
{
	if (fseek(fp, SEEK_SET, offset))
	{
		perror("seek");
		return -1;
	}

	if (fread(ptrData, size, 1, fp) != 1)
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
	FILE* fp = fopen("test.dat", "a+b");
	if (fp)
	{
		int rt = 0;
		if (fseek(fp, SEEK_SET, abs_offset))
		{
			perror("seek");
			return -1;
		}
		char* ptrRaw = (char*)malloc(size);
		if (flash_read_raw(fp, abs_offset, ptrRaw, size))
		{
			perror("read raw");
			rt = -1;
		}
		for (int i = 0; i < size; i++)
		{
			ptrRaw[i] &= ptrData[i];
		}

		if (!rt && (fwrite(ptrRaw, size, 1, fp) != 1))
		{
			perror("write");
			rt = -1;
		}
		if (ptrRaw)
			free(ptrRaw);
		fclose(fp);
		return rt;
	}
	return -1;
}

int flash_read(const int block, const int offset, const char* ptrData,
		const int size)
{
	FILE* fp = fopen("test.dat", "rb");
	int abs_offset = block * BLOCK_SIZE + offset;

	if (fp)
	{
		int rt = 0;
		if (fseek(fp, SEEK_SET,abs_offset))
		{
			perror("seek");
			return -1;
		}

		if (fwrite(ptrData, size, 1, fp) != 1)
		{
			perror("write");
			rt = -1;
		}
		fclose(fp);
		return rt;
	}
	return -1;
}

void flash_build(void)
{
	char buf[BLOCK_SIZE];
	memset(buf,0xFF,BLOCK_SIZE);
	for(int i =0;i < 256;i++)
	{
		flash_write(i,0,buf,BLOCK_SIZE);
	}
}

int main(int argc,char** argv)
{
	flash_build();
	flashfile_system_init();
	char buf[64];
	memset(buf,0x55,63);
	buf[63]=0;
	while(1)
	{
		flashfile_append_record(SpeedFile,time(NULL),buf);
		sleep(1);
	};

}
