/*
 * File      : lcd.c
 */
#include <rtthread.h>
#include <lcd.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>


#define lcd_data 					RCC_APB2Periph_GPIOB
#define lcd_gpio_data				GPIOB
#define lcd_data_pin                ((uint16_t)0xFF00)

#define lcd_ctrl					RCC_APB2Periph_GPIOC
#define lcd_gpio_ctrl              GPIOC
#define lcd_A0						(GPIO_Pin_8)
#define lcd_E1						(GPIO_Pin_7)
#define lcd_E2						(GPIO_Pin_6)
#define lcd_RW						(GPIO_Pin_9)

#define lcd_bk_ctrl				    GPIOB
#define lcd_BK						(GPIO_Pin_5)

#define lcd_reset					RCC_APB2Periph_GPIOA
#define lcd_gpio_reset				GPIOA
#define lcd_RST						(GPIO_Pin_8)


enum LCD_CMD
{
	Display_Off = 0xAE,
	Display_On = 0xAF,
	Start_Line = 0xC0,
	Static_Drive = 0xA4,
	Column_Set = 0x00,
	Page_Set = 0xB8,
	Duty_Set = 0xA8,
	ADC_Select = 0xA0,
	Read_Modi = 0xE0,
	Read_ModiEnd = 0xEE,
	Reset = 0xE2
};


void rt_hw_lcd_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin   = lcd_data_pin;
    GPIO_Init(lcd_gpio_data, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin   = lcd_A0 | lcd_E1 | lcd_E2 | lcd_RW ;
    GPIO_Init(lcd_gpio_ctrl, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = lcd_BK;
    GPIO_Init(lcd_bk_ctrl, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin   = lcd_RST;
    GPIO_Init(lcd_gpio_reset, &GPIO_InitStructure);

    lcd_Reset();

}

// delay us
void lcd_delay(int n)
{
volatile int m,i,j;
  for( i =0;i<n;i++)
  {
    for( j=0;j<3;j++)
    {
    	m++;
    }
  }	
}

unsigned char lcd_read_status(int bank)
{
	uint16_t pinEnable;
	uint16_t value = 0xFFFF;
	pinEnable  = (bank == 0)?  lcd_E1 : lcd_E2 ;
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
	GPIO_SetBits(lcd_gpio_ctrl,lcd_RW);
	GPIO_ResetBits(lcd_gpio_ctrl,lcd_A0 );
	GPIO_Write(lcd_gpio_data,(GPIO_ReadOutputData(lcd_gpio_data) & 0x00FF)|(0xFF00));
	GPIO_SetBits(lcd_gpio_ctrl,pinEnable);
    lcd_delay(1);
    value = GPIO_ReadInputData(lcd_gpio_data);
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);

	return (unsigned char)((value>>8) & 0x00FF);
}

unsigned char lcd_read_data(int bank)
{
	uint16_t pinEnable;
	uint16_t value = 0xFFFF;
	pinEnable  = (bank == 0)?  lcd_E1 : lcd_E2 ;
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
	GPIO_SetBits(lcd_gpio_ctrl,lcd_RW|lcd_A0);
	GPIO_Write(lcd_gpio_data,(GPIO_ReadOutputData(lcd_gpio_data) & 0x00FF)|(0xFF00));
	GPIO_SetBits(lcd_gpio_ctrl,pinEnable);
    lcd_delay(1);
    value = GPIO_ReadInputData(lcd_gpio_data);
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	return (unsigned char)((value>>8) & 0x00FF);
}

void lcd_wait(int bank)
{
	unsigned char status = 0xFF;
	do
	{
		lcd_delay(1);
		status = lcd_read_status(bank);
	}while(status & 0x80);
	return;
}

void lcd_write_cmd(int bank, unsigned char cCmd, int oprand)
{
	rt_uint16_t pinEnable;

	pinEnable  = (bank == 0)?  lcd_E1 : lcd_E2 ;
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
	GPIO_ResetBits(lcd_gpio_ctrl,lcd_A0 | lcd_RW);
	lcd_delay(1);

	rt_uint8_t value = cCmd | oprand;
	GPIO_Write(lcd_gpio_data,((GPIO_ReadOutputData(lcd_gpio_data) & 0x00FF)|((value<<8) & 0xFF00)));
	lcd_delay(1);
	GPIO_SetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
}

void lcd_detect_write_cmd(int bank, unsigned char cCmd, int oprand)
{
	lcd_wait(bank);
	lcd_write_cmd(bank, cCmd,oprand);
}

void lcd_write_data(int bank, unsigned char data)
{
	uint16_t pinEnable;
	pinEnable  = (bank == 0)?  lcd_E1 : lcd_E2 ;
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	GPIO_ResetBits(lcd_gpio_ctrl,lcd_RW);
	GPIO_SetBits(lcd_gpio_ctrl,lcd_A0);
	lcd_delay(1);
	GPIO_Write(lcd_gpio_data,((GPIO_ReadOutputData(lcd_gpio_data) & 0x00FF)|((data<<8) & 0xFF00)));
    lcd_delay(1);
	GPIO_SetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
	GPIO_ResetBits(lcd_gpio_ctrl,pinEnable);
	lcd_delay(1);
}

void lcd_detect_write_data(int bank, unsigned char data)
{
	lcd_wait(bank);
	lcd_write_data(bank, data);
}

void lcd_Reset(void)
{
	GPIO_ResetBits(lcd_gpio_reset,lcd_RST);
	lcd_delay(60);
	GPIO_SetBits(lcd_gpio_reset,lcd_RST);
	lcd_delay(30);
}

void rt_hw_lcd_clear(unsigned char patten)
{
	rt_uint8_t x,y;
	for (x = 0; x < 4; x++)
	{
		lcd_detect_write_cmd(0, Page_Set, x);
		lcd_detect_write_cmd(0, Column_Set, 0);
		for (y = 0; y < 61; y += 1)
		{
			lcd_detect_write_data(0,patten );
		}
	}
	for (x = 0; x < 4; x++)
	{
		lcd_detect_write_cmd(1, Page_Set, x);
		lcd_detect_write_cmd(1, Column_Set, 0);
		for (y = 0; y < 61; y += 1)
		{
			lcd_detect_write_data(1, patten);
		}
	}
}

void rt_hw_lcd_on(void)
{
	lcd_Reset();

		lcd_detect_write_cmd(0,Reset,0);
		lcd_detect_write_cmd(1,Reset,0);

		while(lcd_read_status(0) & 0x10);
		while(lcd_read_status(1) & 0x10);
		lcd_detect_write_cmd(0, Static_Drive, 0);
		lcd_detect_write_cmd(1, Static_Drive, 0);

		lcd_detect_write_cmd(0, Duty_Set, 1);
		lcd_detect_write_cmd(1, Duty_Set, 1);

		lcd_detect_write_cmd(0, ADC_Select, 0);
		lcd_detect_write_cmd(1, ADC_Select, 0);

		lcd_detect_write_cmd(0, Start_Line, 0);
		lcd_detect_write_cmd(1, Start_Line, 0);

		lcd_detect_write_cmd(0, Display_On, 0);
		lcd_detect_write_cmd(1, Display_On, 0);

		rt_hw_lcd_clear(0);
}

void rt_hw_lcd_off(void)
{

}




void lcd_set_column(char nCol)
{
	lcd_detect_write_cmd(0,Column_Set,nCol & 0x7F);
}

void lcd_write_matrix(rt_uint8_t row,rt_uint8_t column,FONT_MATRIX *pt)
{
	rt_uint8_t i,x,y;
	rt_uint8_t  temp;

	lcd_set_column(0);

	for (i = 0; i < 24; i++)
	{
		lcd_detect_write_data(0, (*(unsigned char*) pt++));
	}
}

#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t lcd_inited = 0;
void lcd(rt_uint32_t value)
{
	if (value==0)
	{
		lcd_detect_write_cmd(0, Display_Off, 0);
		lcd_detect_write_cmd(1, Display_Off, 0);
	}
	else
	{
		lcd_detect_write_cmd(0, Display_On, 0);
		lcd_detect_write_cmd(1, Display_On, 0);
	}
}

void lcd_bk(rt_uint32_t value)
{
	if (value==0)
	{
		GPIO_ResetBits(lcd_bk_ctrl,lcd_BK);
	}
	else
	{
		GPIO_SetBits(lcd_bk_ctrl,lcd_BK);
	}
}
void lcd_clear(rt_uint8_t value)
{
	rt_hw_lcd_clear(value);
}

FINSH_FUNCTION_EXPORT(lcd, set lcd on[1] or off[0].)
FINSH_FUNCTION_EXPORT(lcd_bk, set lcd backlight on[1] or off[0].)
FINSH_FUNCTION_EXPORT(lcd_clear, set patten x.)

#endif

