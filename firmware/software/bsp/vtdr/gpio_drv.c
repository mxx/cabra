/*
 * gpio_drv.c
 *
 *  Created on: Sep 5, 2012
 *      Author: mxx
 */
#include <rtthread.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include<stm32f10x_exti.h>
#include<stm32f10x_tim.h>
#include<stm32f10x_dac.h>
#include "gpio_drv.h"
#include<stm32f10x_spi.h>


#define speed_plus_pin  (GPIO_Pin_11)
#define speed_plus_port (GPIOC)

void rt_hw_EXTI_cfg()
{
	EXTI_InitTypeDef EXTI_InitStructure;

	//清空中断标志
	EXTI_ClearITPendingBit(EXTI_Line0);
	//选择中断管脚PC11
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0 );

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;

	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; //设置为中断请求，非事件请求

	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //设置中断触发方式为上沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                           //外部中断使能
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x0f;//强占优先级

	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;

	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//通道中断使能

	NVIC_Init(&NVIC_InitStructure);//初始化中断
}

void rt_hw_tim3_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  TIM_Cmd(TIM3,DISABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);



  TIM_TimeBaseStructure.TIM_Period = 11999;

  TIM_TimeBaseStructure.TIM_Prescaler = 5;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);

}
void Time3_enalble()
{
	  NVIC_InitTypeDef NVIC_InitStructure;

	  /* Set the Vector Table base address at 0x08000000 */
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);

	  /* Configure the Priority Group to 2 bits */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	  /* Enable the TIM3 gloabal Interrupt */
	  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	  NVIC_Init(&NVIC_InitStructure);

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

void rt_hw_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin   = speed_plus_pin;
    GPIO_Init(speed_plus_port, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 |GPIO_Pin_7|GPIO_Pin_8;
    GPIO_Init(speed_plus_port, &GPIO_InitStructure);


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15 |GPIO_Pin_14;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //////gps power control///////////////
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD,GPIO_Pin_11);//da kai gps dianyu

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ////////////gprs pin initial/////////////
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_14;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOD,GPIO_Pin_14);
    GPIO_SetBits(GPIOD,GPIO_Pin_13);
    lcd_delay(10);
    GPIO_ResetBits(GPIOD,GPIO_Pin_13);



    rt_hw_EXTI_cfg();
}

void rt_public_pin_init(unsigned char dir)
{
	 GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  ,ENABLE);
	//lcd and priter
	if(dir == 1)
	{
		    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15 | GPIO_Pin_13;
		    GPIO_Init(GPIOB, &GPIO_InitStructure);
		    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
		    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
		    GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else
	{
		/* Configure SPI pins: SCK, MISO and MOSI */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_14;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_Init(GPIOB, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOB, &GPIO_InitStructure);

			SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
			SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
			if(dir == 0)
			{
				SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
				SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
			}
			else
			{
				SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
				SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
			}
			SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
			SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
			SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

			SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
			SPI_InitStructure.SPI_CRCPolynomial = 7;
			//SPI_Init(DATAFLASH_SPI, &SPI_InitStructure);
			SPI_Init(SPI2, &SPI_InitStructure);

	}

}
