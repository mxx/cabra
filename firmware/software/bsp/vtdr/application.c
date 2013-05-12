
/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#include<stm32f10x_gpio.h>

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#endif

#include "led.h"
#include "lcd.h"
#include "i2c_drv.h"
#include "menu.h"
//#include "font_lib.h"
#include "atmel_dataflash.h"
#include<rtdef.h>
#include<rtconfig.h>
#include<string.h>

/**iclude the host usb lib**/ //modify by leiyq 20120318
#include <usbh_core.h>
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST             USB_Host;
 USBH_Class_cb_TypeDef USBH_MSC_cb;
 USBH_Usr_cb_TypeDef   USR_cb;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
//static rt_uint8_t usb_stack[ 512 ];//modify by leiyq 20130215
static struct rt_thread led_thread;
//static struct rt_thread usb_thread;//modify by leiyq 20130215
u8 testwritebuff[512];
u8 testreadbuff[512];
unsigned long  jonhbak;
static void led_thread_entry(void* parameter)
{
    unsigned int count=0;
    unsigned int i;
    unsigned long jonh;
//    rt_hw_led_init();

    while (1)
    {
        /* led1 on */
#ifndef RT_USING_FINSH
        rt_kprintf("led on, count : %d\r\n",count);
#endif
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
#ifndef RT_USING_FINSH
        rt_kprintf("led off\r\n");
#endif
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
       // testreadbuff=I2C_Master_BufferWrite(I2C1,OwnAddress1,0,1,&testwritebuff);
        //SPI_FLASH_Sector4kErase(0x00);
#if 0
        SPI_FLASH_BulkErase(SPI2);
        for (jonh = 0;jonh<0x100000;jonh=jonh+512)
        {
			for (i = 0;i<512;i++ )
			{
				if (i< 256)
				{
					testwritebuff[i] = i;
				}
				else
				{
					testwritebuff[i] = i-256;
				}
			}
			SPI_FLASH_BufferWrite(SPI2,&testwritebuff,jonh,512);
	        SPI_FLASH_BufferRead(SPI2,&testreadbuff,jonh,512);
	        if (strcmp(testwritebuff,testreadbuff)== 0)
	        {
	               	rt_hw_led_off(0);
	               	jonhbak = jonh;

	        }
	        else
	        	break;
        }
        rt_hw_led_on(0);
        jonhbak = jonh;
#endif
        if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14) == 0)
        {
        	MenutKeyHandler();
        }
        else if(GPIO_ReadInputDataBit( GPIOD, GPIO_Pin_15) == 0 )
		{
        	SelectKeyHandler(1);
		}
        else if(GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_6) == 0 )
        {
        	SelectKeyHandler(0);
        }
        else if(GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_7) == 0 )
		{
        	OKKeyHandler();
		}

       DisplayNormalUI();


    }
}
/*usb_thread_entry */ //add by leiyq 20120516
static void usb_thread_entry(void* parameter)
{
/* init the usbhost mode *///first
#if 1
	//GPIO_SetBits(GPIOB,USB_PWR_ON);
	USBH_Init( &USB_OTG_Core,USB_OTG_FS_CORE_ID,
			&USB_Host,&USBH_MSC_cb,&USR_cb);
#endif
////code area //////////////
////code area //////////////

	while(1)
	{
		/* dectect the usb plugin *///second
		USBH_Process( &USB_OTG_Core, &USB_Host);
		rt_thread_delay(1);

	}
////code area //////////////
////code area //////////////




}
void rt_init_thread_entry(void* parameter)
{
   // rt_hw_buzz_on();
    rt_thread_delay(10);
    rt_hw_buzz_off();
/* Filesystem Initialization */
#ifdef RT_USING_DFS
	{
		/* init the device filesystem */
		dfs_init();

#ifdef RT_USING_DFS_ELMFAT
		/* init the elm chan FatFs filesystam*/
		elm_init();

		/* mount sd card fat partition 1 as root directory */
		if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
		{
			rt_kprintf("\nFile System initialized!\n");
		}
		else
			rt_kprintf("\nFile System initialzation failed!\n");
#endif
	}
#endif
/*
	lcd_write_matrix(0,0,(unsigned char*)distance_cheng);
*/
#ifdef RT_USING_RTGUI
	{
	    extern void rtgui_startup();
	    extern void rt_hw_lcd_init();
	    extern void rtgui_touch_hw_init(void);

		rt_device_t lcd;

		/* init lcd */
		rt_hw_lcd_init();

		/* init touch panel */
		rtgui_touch_hw_init();

		/* re-init device driver */
		rt_device_init_all();

		/* find lcd device */
		lcd = rt_device_find("lcd");

		/* set lcd device as rtgui graphic driver */
		rtgui_graphic_set_device(lcd);

		/* startup rtgui */
		rtgui_startup();
	}
#endif /* #ifdef RT_USING_RTGUI */

	while(1) { rt_thread_delay(1000);};
}

int rt_application_init()
{
	rt_thread_t init_thread;
	rt_thread_t usb_thread;

	rt_err_t result;
#if 0
	//rt_hw_interrupt_disable();
	USBH_Init( &USB_OTG_Core,USB_OTG_FS_CORE_ID,
				&USB_Host,&USBH_MSC_cb,&USR_cb);

#endif
    /* init led thread */
	result = rt_thread_init(&led_thread,
		"led",
		led_thread_entry, RT_NULL,
		(rt_uint8_t*)&led_stack[0], sizeof(led_stack), 20, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&led_thread);
	}
	/*init usb thread*/ //modify by leiyq 20130216
	/*
	result = rt_thread_init(&usb_thread,
			"usb",
			usb_thread_entry, RT_NULL,
			(rt_uint8_t*)&usb_stack[0], sizeof(usb_stack), 21, 5);
	if (result == RT_EOK)
	{
		rt_thread_startup(&usb_thread);
	}
	*/
	usb_thread = rt_thread_create("usb",
			usb_thread_entry, RT_NULL,
								2048, 10, 15);

	if (usb_thread != RT_NULL)
		rt_thread_startup(usb_thread);

#if (RT_THREAD_PRIORITY_MAX == 32)
	init_thread = rt_thread_create("init",
								rt_init_thread_entry, RT_NULL,
								2048, 8, 20);
#else
	init_thread = rt_thread_create("init",
								rt_init_thread_entry, RT_NULL,
								2048, 80, 20);
#endif

	if (init_thread != RT_NULL)
		rt_thread_startup(init_thread);

	return 0;
}

/*@}*/
