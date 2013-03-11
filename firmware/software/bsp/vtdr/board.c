/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2010-09-29     Bernard      add priority grouping configuration
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f10x.h"
#include "stm32f10x_fsmc.h"
#include "misc.h"
#include "board.h"
#include "gpio_drv.h"

/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	/*
	 * Configures the priority grouping:
	 * NVIC_PriorityGroup_2: 2 bits for pre-emption priority
	 *                       2 bits for subpriority
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}


/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

void rt_hw_buzz_off(void)
{
	GPIO_ResetBits(GPIOD,BUZZ_PIN);
}

void rt_hw_buzz_on(void)
{
	GPIO_SetBits(GPIOD,BUZZ_PIN);
}

void rt_hw_buzz_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin   = BUZZ_PIN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void rt_hw_usb_init()
{

	 GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);


    //GPIO_SetBits(GPIOB,USB_PWR_ON);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Pin   = USB_PWR_ON ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = USB_OC ;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#if 0
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   =   USB_ID | USB_DM |USB_DP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = USB_VBUS;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,USB_VBUS);

    GPIO_ResetBits(GPIOB,USB_PWR_ON);
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	/* NVIC Configuration */
	NVIC_Configuration();

	/* Configure the SysTick */
	SysTick_Configuration();

#if STM32_EXT_SRAM
	EXT_SRAM_Configuration();
#endif

	rt_hw_usart_init();
	rt_hw_led_init();
	rt_hw_lcd_init();
	rt_hw_buzz_init();
	rt_hw_usb_init();
	rt_hw_gpio_init();
	I2C_LowLevel_Init(I2C1);
	rt_console_set_device(CONSOLE_DEVICE);
}

/*@}*/
