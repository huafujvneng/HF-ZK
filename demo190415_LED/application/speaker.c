//*********************************************//
//                 WTV020-16S
//   P01---->WTV_CS---->PB1
//   P02---->WTV_CLK---->PB2   
//   P03---->WTV_DATA---->PB3
//   WTV_RESET---->RESET
//
//
//
//
//
//**********************************************//
#include <stdint.h>
#include <stdbool.h>

#include "../inc/hw_ints.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../inc/hw_gpio.h"

#include "../driverlib/rom.h"
#include "../driverlib/sysctl.h"
#include "../driverlib/gpio.h"
#include "../driverlib/pin_map.h"
#include "../driverlib/uart.h"
#include "../headfiles/speaker.h"
#include "../inc/hw_timer.h"
#include "../driverlib/timer.h"


#define WTV_rst_Clr  ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0,0<<0);
#define WTV_rst_Set  ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_0,1<<0);
#define WTV_rst  HWREG(0x42B27F80) //PB0

#define WTV_cs_Clr  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_1,0<<1);
#define WTV_cs_Set  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_1,1<<1);
#define WTV_cs HWREG(0x42B27F84)  //PB1

#define WTV_scl_Clr  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_2,0<<2);
#define WTV_scl_Set  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_2,1<<2);
#define WTV_scl HWREG(0x42B27F88)  //PB2

#define WTV_sda_Clr  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_3,0<<3);
#define WTV_sda_Set  ROM_GPIOPinWrite(GPIO_PORTB_AHB_BASE,GPIO_PIN_3,1<<3);
#define WTV_sda HWREG(0x42B27F8C)  //PB3
virtual_spk spk;
virtual_flagspk fspk[8];

void My_delayus(uint32_t us)
{
	SysCtlDelay(us *SysCtlClockGet() / 3000000);
}

void My_delayms(uint32_t ms)
{
	SysCtlDelay(ms *SysCtlClockGet() / 3000);
}

void Speaker(uint8_t id)
{
	int i;
	WTV_scl = 1;
	WTV_cs = 1;
	//WTV_sda = 1;
	WTV_rst = 1;
	My_delayms(7);
	WTV_rst = 1;
	My_delayms(60);
	WTV_cs = 0;
	My_delayms(5);
	for (i = 0; i < 8; i++)
	{
		WTV_scl = 0;
		if (id & 0x01)
		{
			WTV_sda = 1;
		}
		else
		{
			WTV_sda = 0;
		}
		id >>= 1;
		My_delayus(200);
		WTV_scl = 1;
		My_delayus(200);
	}
	WTV_cs = 1;
}






void SpeakerPool(uint8_t *arr, uint8_t num1)
{
	uint8_t i, j, id;
	uint8_t *p;
	p = arr;
	WTV_scl_Set;
	WTV_cs_Set;
//	WTV_sda_Set;
	WTV_rst_Clr;
	My_delayms(5);
	WTV_rst_Set;


	My_delayms(1);
	for (j = 0; j < num1; j++, p++)
	{
		id = *p;
		My_delayus(900);

		WTV_cs_Clr;

		My_delayus(800);

		for (i = 0; i < 8; i++)
		{

			WTV_scl_Clr;
			if (id & 0x01)
			{
				WTV_sda_Set;
			}
			else
			{
				WTV_sda_Clr;
			}
			id >>= 1;
			My_delayus(40);
			WTV_scl_Set;
			My_delayus(40);
		}

		WTV_cs_Set;
	}

}




void TIMER0A_Handler(void)
{
	uint8_t id;
	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if (spk.numbers > 0)
	{

		if (spk.timeracount < 0xffffffff)
		{
			spk.timeracount++;
		}
		if ((spk.timeracount > 0) && ((spk.timeracount - 1) % 10 == 0))
		{
			WTV_cs_Set;
		}
		if ((spk.timeracount > 0) && ((spk.timeracount - 3) % 10 == 0))
		{
			WTV_cs_Clr;
			WTV_scl_Clr;
		}
		if ((spk.timeracount > 0) && ((spk.timeracount - 8) % 10 == 0))
		{
			id = *spk.pt_con;
			*spk.pt_con++;
			spk.flag5ms++;
			//ROM_IntDisable(INT_TIMER0A);
			for (spk.flagbyte = 0; spk.flagbyte < 8; spk.flagbyte++)
			{
				WTV_scl_Clr;
				if (id & 0x01)
				{
					WTV_sda_Set;
				}
				else
				{
					WTV_sda_Clr;
				}
				id >>= 1;
				My_delayus(20);
				WTV_scl_Set;
				My_delayus(20);
			}
			//ROM_IntEnable(INT_TIMER0A);
			spk.numbers--;
		}
	}
}



//************************************************
//语音芯片初始化函数,初始化已经复位过芯片，
//故这里初始化不用再初始化芯片
//************************************************
void speak_lock(uint8_t *arr)
{
	spk.pt_con = arr;
	spk.timeracount = 0;
	spk.numbers = 3;

}
