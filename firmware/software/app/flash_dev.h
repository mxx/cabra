/*
 * flash_dev.h
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */

#ifndef FLASH_DEV_H_
#define FLASH_DEV_H_

int flash_page_read(const int page_number,char* ptrData);
int flash_page_write(const int page_number, const char* ptrData, const char int size);

int flash_block_read(const int block_number,const int offset, char* ptrData,const int size);
int flash_block_write(const int block_number,const int offset, const char* ptrData, const char int size);

int flash_read(const int block, const int offset, char* ptrData, const int size);
int flash_write(const int block, const int offset, const char* ptrData, const int size);

int flash_erase_block(const int block);

#endif /* FLASH_DEV_H_ */
