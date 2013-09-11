/*
 * File      : serial.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-02-05     Bernard      first version
 * 2009-10-25     Bernard      fix rt_serial_read bug when there is no data
 *                             in the buffer.
 * 2010-03-29     Bernard      cleanup code.
 */

#include "serial.h"
#include <stm32f10x_dma.h>
#include <stm32f10x_usart.h>
uint8_t uart3flag;
uint8_t uartcount = 0;
extern struct rt_device uart2_device;
extern struct rt_device uart3_device;
extern struct rt_device uart4_device;
typedef enum UART2_STATUS
{
	Get_sync_head,
	Start_head_end,
	Get_the_Command,
	Get_the_lenth_high,
	Get_the_lenth_low,
	Get_the_reserve,
	Get_the_data,
	Get_the_checksum

}Status_uart2;
Status_uart2 Uart2PackStatus= Get_sync_head;
typedef enum UART3_STATUS
{
	FIELD_NONE,
	FIELD_ONE,
	FIELD_TWO,
	FIELD_THREE,
	FIELD_FOUR,
	FIELD_FIVE,
	FIELD_SIX,
	FIELD_SEVEN,
	FIELD_EIGHT,
	FIELD_NIGHT,
	FIELD_TEN,
	FIELD_ELEVENT
}Status_uart3;
Status_uart3 SectionID;


static void rt_serial_enable_dma(DMA_Channel_TypeDef* dma_channel,
	rt_uint32_t address, rt_uint32_t size);

#if 0
static rt_err_t  rt_serial_rx(rt_device_t device ,unsigned char lenth)
{
	struct stm32_serial_device* uart = (struct stm32_serial_device*) device->user_data;
	switch (uart->int_rx->rx_buffer[uart->int_rx->read_index])
	{
		case  0xaa
	}


}
#endif

/**
 * @addtogroup STM32
 */
/*@{*/

/* RT-Thread Device Interface */
static rt_err_t rt_serial_init (rt_device_t dev)
{
	struct stm32_serial_device* uart = (struct stm32_serial_device*) dev->user_data;

	if (!(dev->flag & RT_DEVICE_FLAG_ACTIVATED))
	{
		if (dev->flag & RT_DEVICE_FLAG_INT_RX)
		{
			rt_memset(uart->int_rx->rx_buffer, 0,
				sizeof(uart->int_rx->rx_buffer));
			uart->int_rx->read_index = 0;
			uart->int_rx->save_index = 0;
			uart->int_rx->getcmd = 0;
		}

		if (dev->flag & RT_DEVICE_FLAG_DMA_TX)
		{
			RT_ASSERT(uart->dma_tx->dma_channel != RT_NULL);
			uart->dma_tx->list_head = uart->dma_tx->list_tail = RT_NULL;

			/* init data node memory pool */
			rt_mp_init(&(uart->dma_tx->data_node_mp), "dn",
				uart->dma_tx->data_node_mem_pool,
				sizeof(uart->dma_tx->data_node_mem_pool),
				sizeof(struct stm32_serial_data_node));
		}

		/* Enable USART */
		USART_Cmd(uart->uart_device, ENABLE);

		dev->flag |= RT_DEVICE_FLAG_ACTIVATED;
	}

	return RT_EOK;
}

static rt_err_t rt_serial_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_serial_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_serial_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;
	rt_err_t err_code;
	struct stm32_serial_device* uart;

	ptr = buffer;
	err_code = RT_EOK;
	uart = (struct stm32_serial_device*)dev->user_data;

	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		/* interrupt mode Rx */
		while (size)
		{
			rt_base_t level;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();

			if (uart->int_rx->read_index != uart->int_rx->save_index)
			{
				/* read a character */
				*ptr++ = uart->int_rx->rx_buffer[uart->int_rx->read_index];
				size--;

				/* move to next position */
				uart->int_rx->read_index ++;
				if (uart->int_rx->read_index >= UART_RX_BUFFER_SIZE)
					uart->int_rx->read_index = 0;
			}
			else
			{
				/* set error code */
				err_code = -RT_EEMPTY;

				/* enable interrupt */
				rt_hw_interrupt_enable(level);
				break;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);
		}
	}
	else
	{
		/* polling mode */
		while ((rt_uint32_t)ptr - (rt_uint32_t)buffer < size)
		{
			while (uart->uart_device->SR & USART_FLAG_RXNE)
			{
				*ptr = uart->uart_device->DR & 0xff;
				ptr ++;
			}
		}
	}

	/* set error code */
	rt_set_errno(err_code);
	return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

static void rt_serial_enable_dma(DMA_Channel_TypeDef* dma_channel,
	rt_uint32_t address, rt_uint32_t size)
{
	RT_ASSERT(dma_channel != RT_NULL);

	/* disable DMA */
	DMA_Cmd(dma_channel, DISABLE);

	/* set buffer address */
	dma_channel->CMAR = address;
	/* set size */
	dma_channel->CNDTR = size;

	/* enable DMA */
	DMA_Cmd(dma_channel, ENABLE);
}

static rt_size_t rt_serial_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;
	rt_err_t err_code;
	struct stm32_serial_device* uart;

	err_code = RT_EOK;
	ptr = (rt_uint8_t*)buffer;
	uart = (struct stm32_serial_device*)dev->user_data;

	if (dev->flag & RT_DEVICE_FLAG_INT_TX)
	{
		/* interrupt mode Tx, does not support */
		RT_ASSERT(0);
	}
	else if (dev->flag & RT_DEVICE_FLAG_DMA_TX)
	{
		/* DMA mode Tx */

		/* allocate a data node */
		struct stm32_serial_data_node* data_node = (struct stm32_serial_data_node*)
			rt_mp_alloc (&(uart->dma_tx->data_node_mp), RT_WAITING_FOREVER);
		if (data_node == RT_NULL)
		{
			/* set error code */
			err_code = -RT_ENOMEM;
		}
		else
		{
			rt_uint32_t level;

			/* fill data node */
			data_node->data_ptr 	= ptr;
			data_node->data_size 	= size;

			/* insert to data link */
			data_node->next = RT_NULL;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();

			data_node->prev = uart->dma_tx->list_tail;
			if (uart->dma_tx->list_tail != RT_NULL)
				uart->dma_tx->list_tail->next = data_node;
			uart->dma_tx->list_tail = data_node;

			if (uart->dma_tx->list_head == RT_NULL)
			{
				/* start DMA to transmit data */
				uart->dma_tx->list_head = data_node;

				/* Enable DMA Channel */
				rt_serial_enable_dma(uart->dma_tx->dma_channel,
					(rt_uint32_t)uart->dma_tx->list_head->data_ptr,
					uart->dma_tx->list_head->data_size);
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);
		}
	}
	else
	{
		/* polling mode */
		if (dev->flag & RT_DEVICE_FLAG_STREAM)
		{
			/* stream mode */
			while (size)
			{
				if (*ptr == '\n')
				{
					while (!(uart->uart_device->SR & USART_FLAG_TXE));
					uart->uart_device->DR = '\r';
				}

				while (!(uart->uart_device->SR & USART_FLAG_TXE));
				uart->uart_device->DR = (*ptr & 0x1FF);

				++ptr; --size;
			}
		}
		else
		{
			/* write data directly */
			while (size)
			{
				while (!(uart->uart_device->SR & USART_FLAG_TXE));
				uart->uart_device->DR = (*ptr & 0x1FF);

				++ptr; --size;
			}
		}
	}

	/* set error code */
	rt_set_errno(err_code);

	return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

static rt_err_t rt_serial_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
	struct stm32_serial_device* uart;

	RT_ASSERT(dev != RT_NULL);

	uart = (struct stm32_serial_device*)dev->user_data;
	switch (cmd)
	{
	case RT_DEVICE_CTRL_SUSPEND:
		/* suspend device */
		dev->flag |= RT_DEVICE_FLAG_SUSPENDED;
		USART_Cmd(uart->uart_device, DISABLE);
		break;

	case RT_DEVICE_CTRL_RESUME:
		/* resume device */
		dev->flag &= ~RT_DEVICE_FLAG_SUSPENDED;
		USART_Cmd(uart->uart_device, ENABLE);
		break;
	}

	return RT_EOK;
}

/*
 * serial register for STM32
 * support STM32F103VB and STM32F103ZE
 */
rt_err_t rt_hw_serial_register(rt_device_t device, const char* name, rt_uint32_t flag, struct stm32_serial_device *serial)
{
	RT_ASSERT(device != RT_NULL);

	if ((flag & RT_DEVICE_FLAG_DMA_RX) ||
		(flag & RT_DEVICE_FLAG_INT_TX))
	{
		RT_ASSERT(0);
	}

	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= rt_serial_init;
	device->open		= rt_serial_open;
	device->close		= rt_serial_close;
	device->read 		= rt_serial_read;
	device->write 		= rt_serial_write;
	device->control 	= rt_serial_control;
	device->user_data	= serial;

	/* register a character device */
	return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);
}
void leidebug()
{
	return;
}
static unsigned char gprmcbuf[400];
/* ISR for serial interrupt */
void rt_hw_serial_isr(rt_device_t device)
{
	struct stm32_serial_device* uart = (struct stm32_serial_device*) device->user_data;
    static unsigned char checksum = 0;
    static uint16_t lenth;
   // static unsigned char gprmcbuf[400];
    static unsigned char isgprmc=0;
    static unsigned short gprmccnt = 0;

	if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
	{
		/* interrupt mode receive */
		RT_ASSERT(device->flag & RT_DEVICE_FLAG_INT_RX);

		/* save on rx buffer */
		while (uart->uart_device->SR & USART_FLAG_RXNE)
		{
			rt_base_t level;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();

			/* save character */

			if((&uart2_device )== device)
			{
				uartcount++;
				uart->int_rx->rx_buffer[uart->int_rx->save_index] = uart->uart_device->DR & 0xff;
				rt_device_write(&uart2_device, 0,&(uart->int_rx->rx_buffer[uart->int_rx->save_index]), 1);
				checksum = checksum^(uart->int_rx->rx_buffer[uart->int_rx->save_index]);
				switch (Uart2PackStatus )
				{
					case Get_sync_head:
						if ((uart->int_rx->rx_buffer[uart->int_rx->save_index])== 0xaa)
						{
							checksum = uart->int_rx->rx_buffer[uart->int_rx->save_index];
							Uart2PackStatus = Start_head_end;
						}
						else
						{
							uart->int_rx->getcmd = 0x7f;
						}
						break;
					case Start_head_end:
						if((uart->int_rx->rx_buffer[uart->int_rx->save_index])== 0x75)
						{
							Uart2PackStatus = Get_the_Command;
							//rt_device_write(&uart2_device, 0,&(uart->int_rx->rx_buffer[uart->int_rx->save_index]), 1);
						}
						else
						{
							uart->int_rx->getcmd = 0x7f;
							Uart2PackStatus = Get_sync_head;
						}
						break;
					case Get_the_Command:
						if(((uart->int_rx->rx_buffer[uart->int_rx->save_index]) <0x16)
							||((uart->int_rx->rx_buffer[uart->int_rx->save_index])>0x81
							 &&(uart->int_rx->rx_buffer[uart->int_rx->save_index])<0x85 )
							||((uart->int_rx->rx_buffer[uart->int_rx->save_index])>0x81
							&&(uart->int_rx->rx_buffer[uart->int_rx->save_index])<0x85 )
							||((uart->int_rx->rx_buffer[uart->int_rx->save_index])>0xc1
							&&(uart->int_rx->rx_buffer[uart->int_rx->save_index])<0xc5 ))
						{
							Uart2PackStatus = Get_the_lenth_high;
						}
						else
						{
							uart->int_rx->getcmd = 0x7f;
							Uart2PackStatus = Get_sync_head;
						}

						break;
					case Get_the_lenth_high:
						lenth = uart->int_rx->rx_buffer[uart->int_rx->save_index];
						Uart2PackStatus = Get_the_lenth_low;
						break;
					case Get_the_lenth_low:
						lenth = ((lenth<<8)&0xff00)+(uart->int_rx->rx_buffer[uart->int_rx->save_index]);
						Uart2PackStatus = Get_the_reserve;
						break;
					case Get_the_reserve:
						if (lenth != 0)
						{
							Uart2PackStatus = Get_the_data;
						}
						else
							Uart2PackStatus = Get_the_checksum;
						break;
					case Get_the_data:
						if(lenth )
							lenth--;
						if(lenth == 0)
							Uart2PackStatus = Get_the_checksum;

						break;
					case Get_the_checksum:
						if(checksum == 0)
						{
							uart->int_rx->getcmd ++;
						}
						else
						{
							uart->int_rx->getcmd = 0x7f;
						}
						Uart2PackStatus = Get_sync_head;
						break;
					default:
						break;
				}

				uart->int_rx->save_index ++;
				if (uart->int_rx->save_index >= UART_RX_BUFFER_SIZE)
					uart->int_rx->save_index = 0;

				/* if the next position is read index, discard this 'read char' */
				if (uart->int_rx->save_index == uart->int_rx->read_index)
				{
					uart->int_rx->read_index ++;
					if (uart->int_rx->read_index >= UART_RX_BUFFER_SIZE)
						uart->int_rx->read_index = 0;
				}

			}
			if((&uart3_device)== device)
		    {
		    	if((uart->uart_device->DR & 0xff) == '$')
		    	{

		    	       SectionID=0;
		    	       isgprmc = 0;
		    	       gprmccnt =0;
		    	       uart3flag =1;
		    	 }
		    	 else if((uart->uart_device->DR & 0xff)==',')
		    	 {
		    		 if((isgprmc == 1)&&(SectionID== FIELD_NIGHT))
					 {
		    			   //get the time
						 GetGPSLocation3(gprmcbuf,gprmccnt);
					 }
		    		 else if((isgprmc == 2) &&(SectionID == FIELD_SEVEN ))
		    		 {
		    				GetGPSSpeed(gprmcbuf,gprmccnt);

		    		 }
		    	           SectionID++;
		    	           gprmccnt = 0;
		    	           uart3flag =2;

		    	 }

		    	 else
		    	 {
		    		   if(gprmccnt == 400)
		    		   {
		    			   gprmccnt = 0;
		    		   }
					   gprmcbuf[gprmccnt] = uart->uart_device->DR & 0xff;
					   gprmccnt++;
					   switch(SectionID)
					   {
					         case FIELD_NONE:
					   	   		 if(gprmccnt == 5)
					   	   		 {
					   	   			 uart3flag =2;
					   	   			gprmccnt =0 ;
									if ( (gprmcbuf[0] == 'G')
										&&(gprmcbuf[1] == 'P')&&(gprmcbuf[2] =='G')
										&&(gprmcbuf[3] =='G')&&(gprmcbuf[4] == 'A'))
									 {
										 isgprmc =1;//位置信息

									 }
									if ( (gprmcbuf[0] == 'G')
										&&(gprmcbuf[1] == 'P')&&(gprmcbuf[2] =='V')
										&&(gprmcbuf[3] == 'T')&&(gprmcbuf[4] == 'G'))
									 {
										 isgprmc =2;//速度信息
										 uart3flag =2;
									 }
					   	   		 }

					   	   		   break;
							 case FIELD_ONE://提取时间
								 uart3flag =1;
								 if(isgprmc == 1)
								 {
										 if(gprmccnt == 10  )
										 {
											 //get the time
											 GetTheGPSTime(gprmcbuf);
										 }
								 }
								  break;
							 case FIELD_TWO: //判断数据是否可信(当GPS天线能接收到有3颗GPS卫星时为A，可信
								 if(isgprmc == 1)
								 {
									 if(gprmccnt == 9 )
									 {
										 //get the time
										GetGPSLocation1(gprmcbuf);
									 }
								 }
								 break;
							 case FIELD_THREE://提取出纬度
								  break;
							 case FIELD_FOUR://提取出速度
								 if(isgprmc == 1)
								 {
									 if(gprmccnt == 10 )
									 {
										 //get the time
										 GetGPSLocation2(gprmcbuf);
									 }
								 }
								  break;
							 case FIELD_FIVE://提取出经度
							 	  break;
							 case FIELD_SEVEN:
								if(isgprmc == 1)
								{
									 if(gprmccnt == 2 )
									 {
										 //get the singal
										 GettheSinaldata(gprmcbuf);
									 }
								}
								break;
							 case FIELD_NIGHT://提取高度
								  break;
								  default:
								  break;
						}
				 }
		    }
			if((&uart4_device )== device)
			{
				gprmcbuf[gprmccnt] = uart->uart_device->DR & 0xff;
				gprmccnt++;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);
		}

		/* clear interrupt */
		USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);

		/* invoke callback */
		if (device->rx_indicate != RT_NULL)
		{
			rt_size_t rx_length;

			/* get rx length */
			rx_length = uart->int_rx->read_index > uart->int_rx->save_index ?
				UART_RX_BUFFER_SIZE - uart->int_rx->read_index + uart->int_rx->save_index :
				uart->int_rx->save_index - uart->int_rx->read_index;

			device->rx_indicate(device, rx_length);
		}
	}

	if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
	{
		/* clear interrupt */
		USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
	}
}

/*
 * ISR for DMA mode Tx
 */
void rt_hw_serial_dma_tx_isr(rt_device_t device)
{
	rt_uint32_t level;
	struct stm32_serial_data_node* data_node;
	struct stm32_serial_device* uart = (struct stm32_serial_device*) device->user_data;

	/* DMA mode receive */
	RT_ASSERT(device->flag & RT_DEVICE_FLAG_DMA_TX);

	/* get the first data node */
	data_node = uart->dma_tx->list_head;
	RT_ASSERT(data_node != RT_NULL);

	/* invoke call to notify tx complete */
	if (device->tx_complete != RT_NULL)
		device->tx_complete(device, data_node->data_ptr);

	/* disable interrupt */
	level = rt_hw_interrupt_disable();

	/* remove list head */
	uart->dma_tx->list_head = data_node->next;
	if (uart->dma_tx->list_head == RT_NULL) /* data link empty */
		uart->dma_tx->list_tail = RT_NULL;

	/* enable interrupt */
	rt_hw_interrupt_enable(level);

	/* release data node memory */
	rt_mp_free(data_node);

	if (uart->dma_tx->list_head != RT_NULL)
	{
		/* transmit next data node */
		rt_serial_enable_dma(uart->dma_tx->dma_channel,
			(rt_uint32_t)uart->dma_tx->list_head->data_ptr,
			uart->dma_tx->list_head->data_size);
	}
	else
	{
		/* no data to be transmitted, disable DMA */
		DMA_Cmd(uart->dma_tx->dma_channel, DISABLE);
	}
}

/*@}*/
