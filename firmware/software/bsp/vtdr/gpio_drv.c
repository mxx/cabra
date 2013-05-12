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
#include "gpio_drv.h"


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

	NVIC_InitStructure.NVIC_IRQChannel=EXTI15_10_IRQn;
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

  /* TIM2 enable counter */
  TIM_Cmd(TIM3, ENABLE);
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

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 |GPIO_Pin_7;
    GPIO_Init(speed_plus_port, &GPIO_InitStructure);


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15 |GPIO_Pin_14|GPIO_Pin_4;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    rt_hw_EXTI_cfg();
}
