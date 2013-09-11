/**
  ******************************************************************************
  * @file OptimizedI2Cexamples/src/I2CRoutines.c
  * @author  MCD Application Team
  * @version  V4.0.0
  * @date  06/18/2010
  * @brief  Contains the I2Cx slave/Master read and write routines.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c_drv.h"
#include<stm32f10x_i2c.h>
#include<stm32f10x_gpio.h>
#include<stm32f10x_rcc.h>


/** @addtogroup Optimized I2C examples
  * @{
  */
#define TIMEOUT_MAX 300

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t I2CDirection = I2C_DIRECTION_TX;
__IO uint32_t NumbOfBytes1;
__IO uint32_t NumbOfBytes2;
__IO uint8_t Address;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Reads buffer of bytes  from the slave.
  * @param pBuffer: Buffer of bytes to be read from the slave.
  * @param NumByteToRead: Number of bytes to be read by the Master.
  * @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
  * @param SlaveAddress: The address of the slave to be addressed by the Master.
  * @retval : None.
  */

Status I2C_Master_BufferRead(I2C_TypeDef* I2Cx,
                              uint8_t SlaveAddress,
                              uint8_t RegisterAddr,
                              uint16_t NumByteToRead,
                              uint8_t* pBuffer)
{
   __IO uint32_t TimeOut = TIMEOUT_MAX;
   __IO uint32_t temp;
   uint8_t RegAdress[2];
   RegAdress[0] = (uint8_t )(RegisterAddr);
   RegAdress[1] = (uint8_t ) (RegisterAddr>>8);
 static uint16_t loop = 0;

   I2Cx->SR1 &= 0xFBFF;

   IIC_READ_START0:
   if (loop++ > 50)
   {
     loop = 0;
     return ERROR;
   }

   /* Send START condition */
   I2C_GenerateSTART(I2Cx, ENABLE);
   /* Test on EV5 and clear it */
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
   {

     if (TimeOut-- == 0)
     {
       TimeOut = TIMEOUT_MAX;
       goto IIC_READ_START0;
     }

   }
   TimeOut = TIMEOUT_MAX;
   /* Send slave address for read */
   I2C_Send7bitAddress(I2Cx,  SlaveAddress, I2C_Direction_Transmitter);
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
   {

     if (TimeOut-- == 0)
     {
       TimeOut = TIMEOUT_MAX;
       I2Cx->SR1 &= 0xFBFF;
       I2C_GenerateSTOP(I2Cx, ENABLE);
       goto IIC_READ_START0;
     }

   }
   /* Clear EV6 by setting again the PE bit */
   I2C_Cmd(I2Cx, ENABLE);

   I2C_SendData(I2Cx, RegAdress[1]);

   /* Test on EV8 and clear it */
   TimeOut = TIMEOUT_MAX;
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
   {

     if (TimeOut-- == 0)
     {
       TimeOut = TIMEOUT_MAX;
       goto IIC_READ_START0;
      }

   }

   I2C_SendData(I2Cx, RegAdress[0]);

   /* Test on EV8 and clear it */
   TimeOut = TIMEOUT_MAX;
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
   {

     if (TimeOut-- == 0)
     {
       TimeOut = TIMEOUT_MAX;
       goto IIC_READ_START0;
      }

   }



   if (NumByteToRead == 0x01)
   {

     loop = 0;
     IIC_READ_START1:
     if (loop++ > 50)
     {
       loop = 0;
       return ERROR;
     }

     /* Send START condition */
     I2C_GenerateSTART(I2Cx, ENABLE);
     while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
     {
      // if (TimeOut-- == 0) return ERROR;
     }
     /* Send Slave address for read */
     I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
     TimeOut = TIMEOUT_MAX;
     while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
     {

       if (TimeOut-- == 0)
       {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            I2Cx->SR1 &= 0xFBFF;
            goto IIC_READ_START1;
            //return ERROR;
       }
     }

     /* Clear ACK */
     I2C_AcknowledgeConfig(I2Cx, DISABLE);
     __disable_irq();
     /* Clear ADDR flag */
     temp = I2Cx->SR2;
     /* Program the STOP */
     I2C_GenerateSTOP(I2Cx, ENABLE);
     __enable_irq();
     while ((I2C_GetLastEvent(I2Cx) & 0x0040) != 0x000040); /* Poll on RxNE */
     /* Read the data */
     *pBuffer = I2C_ReceiveData(I2Cx);
     /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
     while ((I2Cx->CR1&0x200) == 0x200);
     /* Enable Acknowledgement to be ready for another reception */
     I2C_AcknowledgeConfig(I2Cx, ENABLE);

     loop = 0;

     return SUCCESS;
   }
   else if(NumByteToRead == 0x02)
     {

       loop = 0;
       IIC_READ_START2:
       if (loop++ > 50)
       {
         loop = 0;
         return ERROR;
       }

       /* Send START condition */
       I2C_GenerateSTART(I2Cx, ENABLE);
       while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
       /* Send EEPROM address for read */
       I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Receiver);
       I2Cx->CR1 = 0xC01; /* ACK=1; POS =1 */

       TimeOut = TIMEOUT_MAX;
       while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR))
       {
         if (TimeOut-- == 0)
         {
           I2C_GenerateSTOP(I2Cx, ENABLE);
            I2Cx->SR1 &= 0xFBFF;
           TimeOut = TIMEOUT_MAX;
           goto IIC_READ_START2;
         }
       }
       __disable_irq();
       /* Clear ADDR */
       temp = I2Cx->SR2;
       /* Disable ACK */
       I2C_AcknowledgeConfig(I2Cx, DISABLE);
       __enable_irq();
       while ((I2C_GetLastEvent(I2Cx) & 0x0004) != 0x00004); /* Poll on BTF */
        __disable_irq();
       /* Program the STOP */
       I2C_GenerateSTOP(I2Cx, ENABLE);
       /* Read first data */
       *pBuffer = I2Cx->DR;
       pBuffer++;
       /* Read second data */
       *pBuffer = I2Cx->DR;
       __enable_irq();
       I2Cx->CR1 = 0x0401; /* POS = 0, ACK = 1, PE = 1 */
       loop = 0;
       return SUCCESS;
     }
   else
   {


     loop = 0;
     IIC_READ_START3:
     if (loop++ > 50)
     {
       loop = 0;
       return ERROR;
     }

     TimeOut = TIMEOUT_MAX;
     /* Send START condition */
     I2C_GenerateSTART(I2Cx, ENABLE);
     /* Test on EV5 and clear it */
     while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
     {
       if (TimeOut-- == 0)
         //return ERROR;
       {
             I2C_GenerateSTOP(I2Cx, ENABLE);
           goto IIC_READ_START3;

       }
     }
     TimeOut = TIMEOUT_MAX;
     /* Send slave address for read */
     I2C_Send7bitAddress(I2Cx,  SlaveAddress, I2C_Direction_Receiver);

     while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
     {
		 if (TimeOut-- == 0)
		 {
		   TimeOut = TIMEOUT_MAX;
			 /* Program the STOP */
			 I2C_GenerateSTOP(I2Cx, ENABLE);
			 I2Cx->SR1 &= 0xFBFF;
		   goto IIC_READ_START3;
		 }
     }

     /* While there is data to be read; here the safe procedure is implemented */
     while (NumByteToRead)
     {

       if (NumByteToRead != 3) /* Receive bytes from first byte until byte N-3 */
       {
         while ((I2C_GetLastEvent(I2Cx) & 0x00004) != 0x000004); /* Poll on BTF */
         /* Read data */
         *pBuffer = I2C_ReceiveData(I2Cx);
         pBuffer++;
         /* Decrement the read bytes counter */
         NumByteToRead--;
       }

       if (NumByteToRead == 3)  /* it remains to read three data: data N-2, data N-1, Data N */
       {

         /* Data N-2 in DR and data N -1 in shift register */
         while ((I2C_GetLastEvent(I2Cx) & 0x000004) != 0x0000004); /* Poll on BTF */
         /* Clear ACK */
         I2C_AcknowledgeConfig(I2Cx, DISABLE);
         __disable_irq();
         /* Read Data N-2 */
         *pBuffer = I2C_ReceiveData(I2Cx);
         pBuffer++;
         /* Program the STOP */
         I2C_GenerateSTOP(I2Cx, ENABLE);
         /* Read DataN-1 */
         *pBuffer = I2C_ReceiveData(I2Cx);
         __enable_irq();
         pBuffer++;
         while ((I2C_GetLastEvent(I2Cx) & 0x00000040) != 0x0000040); /* Poll on RxNE */
         /* Read DataN */
         *pBuffer = I2Cx->DR;
         /* Reset the number of bytes to be read by master */
         NumByteToRead = 0;
       }
     }
     /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
     while ((I2Cx->CR1&0x200) == 0x200);
     /* Enable Acknowledgement to be ready for another reception */
     I2C_AcknowledgeConfig(I2Cx, ENABLE);

     loop = 0;
     return SUCCESS;
   }
 }


/**
  * @brief  Send a buffer of bytes to the slave using polling mode.
  * @param SlaveAddress: slave address.
  * @param pBuffer: Buffer of bytes to be sent to the slave.
  * @param NumByteToRead: Number of bytes to be sent to the slave.
  * @retval : None.
  */
Status I2C_Master_BufferWrite(I2C_TypeDef* I2Cx,
                               uint8_t SlaveAddress,
                               uint16_t RegisterAddr,
                               uint16_t NumByteToWrite,
                               uint8_t* pBuffer)
{
  __IO uint32_t TimeOut = TIMEOUT_MAX;
  uint8_t RegAdress[2];
  RegAdress[0] = (uint8_t )(RegisterAddr);
  RegAdress[1] = (uint8_t ) (RegisterAddr>>8);

    TimeOut = TIMEOUT_MAX;

  static uint16_t loop = 0;

 I2Cx->SR1 &= 0xFBFF;


  IIC_WRITE_START0:
  if (loop++ > 50)
  {
    loop = 0;
    return ERROR;
  }


  /* Send START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if (TimeOut-- == 0)
    {
        TimeOut = TIMEOUT_MAX;
        I2Cx->SR1 &= 0xFBFF;
       I2C_GenerateSTOP(I2Cx, ENABLE);
        goto IIC_WRITE_START0;
        // return ERROR;
    }

  }
  /* Send slave address for write */
  I2C_Send7bitAddress(I2Cx, SlaveAddress, I2C_Direction_Transmitter);

  TimeOut = TIMEOUT_MAX;
  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {


    if (TimeOut-- == 0) //return ERROR;
    {
      TimeOut = TIMEOUT_MAX;
       I2Cx->SR1 &= 0xFBFF;
       I2C_GenerateSTOP(I2Cx, ENABLE);
      goto IIC_WRITE_START0;
    }

  }

  TimeOut = TIMEOUT_MAX;

  /* Transmit the first address for r/w operations */
  I2C_SendData(I2Cx, RegAdress[1]);

  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if (TimeOut-- == 0)
      return ERROR;
  }
  /* Transmit the first address for r/w operations */
   I2C_SendData(I2Cx, RegAdress[0]);

   /* Test on EV8 and clear it */
   while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
   {
     if (TimeOut-- == 0)
       return ERROR;
   }

  if (NumByteToWrite == 0x01)
  {
    TimeOut = TIMEOUT_MAX;
    /* Prepare the register value to be sent */
    I2C_SendData(I2Cx, *pBuffer);

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
      if (TimeOut-- == 0)
        return ERROR;
    }

    /* End the configuration sequence */
    I2C_GenerateSTOP(I2Cx, ENABLE);
    return SUCCESS;
  }
  I2C_SendData(I2Cx, *pBuffer);
  pBuffer++;
  NumByteToWrite--;
  /* While there is data to be written */
  while (NumByteToWrite--)
  {
    while ((I2C_GetLastEvent(I2Cx) & 0x04) != 0x04);  /* Poll on BTF */
    /* Send the current byte */
    I2C_SendData(I2Cx, *pBuffer);
    /* Point to the next byte to be written */
    pBuffer++;

  }
  TimeOut = TIMEOUT_MAX;
  /* Test on EV8_2 and clear it, BTF = TxE = 1, DR and shift registers are
   empty */
  while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if (TimeOut-- == 0) return ERROR;
  }
  /* Send STOP condition */
  I2C_GenerateSTOP(I2Cx, ENABLE);

  loop = 0;


  return SUCCESS;
}

void I2C_LowLevel_Init(I2C_TypeDef* I2Cx)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* GPIOB clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    if (I2Cx == I2C1)
    {
        /* I2C1 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOD, &GPIO_InitStructure);//IC卡电源开关
		GPIO_ResetBits(GPIOD,GPIO_Pin_7);//打开ic卡的电源

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		/* I2C1 SDA and SCL configuration */

        /* Enable I2C1 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        /* Release I2C1 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }

    else /* I2Cx = I2C2 */

    {

        /* I2C2 clock enable */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        /* I2C1 SDA and SCL configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Enable I2C2 reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
        /* Release I2C2 from reset state */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    }

    /* I2C1 and I2C2 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = OwnAddress1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
    I2C_Init(I2Cx, &I2C_InitStructure);


}






/**
  * @}
  */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
