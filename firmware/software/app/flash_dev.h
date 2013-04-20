/*
 * flash_dev.h
 *
 *  Created on: Jan 27, 2013
 *      Author: mxx
 */

#ifndef FLASH_DEV_H_
#define FLASH_DEV_H_

//驱动接口函数，读指定的块
//在指定的块block上，偏移地址是offset处，读长度的size字节的数据写入ptrData开始的内存
int flash_read(const int block, const int offset, char* ptrData, const int size);

//驱动接口函数，写指定的块
//在指定的块block上，偏移地址是offset处，把内存ptrData开始，长度的size字节的数据写入
int flash_write(const int block, const int offset, const char* ptrData, const int size);

//驱动接口函数，擦除的块
int flash_erase_block(const int block);

#endif /* FLASH_DEV_H_ */
