
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
#include <application.h>

#include<stm32f10x_gpio.h>
#include<stm32f10x_dac.h>

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
extern struct rt_device uart2_device;
extern CLOCK curTime;
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST             USB_Host;
extern StructPara Parameter;
 USBH_Class_cb_TypeDef USBH_MSC_cb;
 USBH_Usr_cb_TypeDef   USR_cb;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
//static rt_uint8_t usb_stack[ 512 ];//modify by leiyq 20130215
static struct rt_thread led_thread;
//static struct rt_thread usb_thread;//modify by leiyq 20130215
u8 testwritebuff[10];
u8 testreadbuff[10];
unsigned char status2;
unsigned long  jonhbak;
static void led_thread_entry(void* parameter)
{
    unsigned int i;
    unsigned long jonh;
    unsigned char num=0;
    extern struct rt_device uart4_device;
    while (1)
    {
        rt_thread_delay( 1); /* sleep 0.5 second and switch to other thread */
        KepPressHandler();
        rs232_handle_application(&uart2_device);
       //rt_device_write(&uart4_device, 0,"AT+CIPMUX?\r\n", 12);


    }

}
/*usb_thread_entry */ //add by leiyq 20120516
static void usb_thread_entry(void* parameter)
{
/* init the usbhost mode *///first
	USBH_Init( &USB_OTG_Core,USB_OTG_FS_CORE_ID,
			&USB_Host,&USBH_MSC_cb,&USR_cb);
	//GPIO_ResetBits(GPIOC,USB_PWR_ON);


	while(1)
	{
		/* dectect the usb plugin *///second
		USBH_Process( &USB_OTG_Core, &USB_Host);
		rt_thread_delay(1);
	}


}
void rt_init_thread_entry(void* parameter)
{
	unsigned char bddd[18];
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
		rt_hw_led_init();
		Time3_enalble();
		rt_hw_tim3_init();
		GPIO_SetBits(GPIOE,GPIO_Pin_2);//da kai speek
		InitialValue();
	    InitializeTable();
		DisplayNormalUI();
		GPIO_SetBits(GPIOD,GPIO_Pin_12);//da kai gps dianyu
		GPIO_SetBits(GPIOD,GPIO_Pin_13);//da kai gprs dinayuan
	while(1) {
        GetSpeedandTime();
        ValueStatusHandler();
        AlarmHandler();
        BaseDataHandler();
        LocationHandler();
        OverDriverHandler();
        //DoubltPointHandler();
        DrvierRegisterHandler();
        PowerHandle();
        ModifyHandle();
       // JournalHandle();
		rt_thread_delay(1);
	}
}

int rt_application_init()
{
	rt_thread_t init_thread;
	rt_thread_t usb_thread;

	rt_err_t result;

    /* init led thread */
	result = rt_thread_init(&led_thread,
		"led",
		led_thread_entry, RT_NULL,
		(rt_uint8_t*)&led_stack[0], sizeof(led_stack), 20, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&led_thread);
	}

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
