/*
 * File      : led.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __LCD_H__
#define __LCD_H__

#include <rtthread.h>

#ifndef FONT_MATRIX
#define  FONT_MATRIX unsigned char
#endif
#define HAN_ZI 12
#define NUM  6
#define ZIMU 7
typedef enum
{
	line1 = 3,
	line2 = 1,
	line3 = 7,
	line4 = 5,
	lineall = 8
}LINE_CMD;
void rt_hw_lcd_init(void);
void rt_hw_lcd_on(void);
void rt_hw_lcd_off(void);
void lcd_write_matrix(rt_uint8_t row,rt_uint8_t column,FONT_MATRIX *pt,rt_uint8_t num);
void lcd_Reset(void);
void lcd_clear(LINE_CMD value);

#endif
