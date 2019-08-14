#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../inc/hw_ints.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../driverlib/uart.h"
#include "../driverlib/rom.h"
#include "../driverlib/sysctl.h"
#include "../driverlib/gpio.h"
#include "../driverlib/timer.h"
#include "../driverlib/interrupt.h"
#include "../driverlib/watchdog.h"
#include "../driverlib/pin_map.h"
#include "../headfiles/infrared.h"
#include "../headfiles/speaker.h"
#include "../headfiles/gpioinit.h"
#include "../headfiles/led_display.h"
#include "../headfiles/modbus_rtu.h"
#include "../driverlib/eeprom.h"
#include "../utils/uartstdio.h"
#include "../headfiles/gui.h"
#include "../headfiles/eeprominit.h"
#include "../driverlib/fpu.h"

//20180418 修改modbus_rtu.c文件，在529-533行加if函数。main函数

//
uint32_t SystemCoreClock, ggvalue;
extern volatile int updatelcdtimer;
extern uint8_t orgcontent[100];



extern inf_detectors inf1;
extern const uint32_t	SEG_CTL[8];
uint8_t b[1] = { 0x0d };//程序初始化成功
uint8_t temponce;//控制下发报文的辅助变量，保证1s下发一次
int fd = 0;
void SystemTickInit(void);
void myWatchDogInit(void);
void FeedDog(void);
void lcdPoweronInit(uint8_t e);
void Control_Relay(void);

//看门狗中断函数
void WDT0_Handler(void)
{
	WatchdogIntClear(WATCHDOG0_BASE);
}
uint8_t Near_P2[3] = { 音量10,连码播放,本柜回路已带电 };



//////////////////////////////////////////


int main(void)
{
	int i;
	uint8_t flag;
	SystemCoreClock = ROM_SysCtlClockGet();
	eepromdatainit();
	myEEpromInit();
	GPIOInit();
	myTimerInit();
	SystemTickInit();
	UartInit();
	lcddisplaystateinit();
	rx0_modbus_init();
	rx6_modbus_init();
	tx0_modbus_init();
	tx6_modbus_init();
	//myWatchDogInit();
	FPUEnable();
	FPULazyStackingEnable();
	lcdPoweronInit(10);

	//初始化数码管状态为全灭
	for (i = 0; i < 8; i++)
	{
		HWREG(SEG_CTL[i]) = 0;
	}

	//播放初始化成功语音
	Speaker(0x0d);
	Timer.infHold_timer = Timer.soft_timer + 12000;

	while (1)
	{
		//下发温湿度查询报文的辅助变量
		temponce <<= 1;

		//获得看门狗的当前值
		//ggvalue = WatchdogValueGet(WATCHDOG0_BASE);

		//闭锁轮询判断
		Bisuo();

		if (Timer.soft_timer - Timer.daidianyuyin_timer > 0)
		{
			if (spk.daidianyuyinflag == 1)
			{
				spk.daidianyuyinflag = 0;
				speak_lock(Near_P2);//--wushuisheng add 2018年4月18日 13:01:50

			}
		}


		//下发读取温湿度的报文，两路温湿度，相隔500ms轮流下发
		if (Timer.soft_timer % 1000 > 500)
		{
			temponce += 1;
			if ((temponce & 0x03) == 0x01)
			{
				ReadWSD0();
			}
		}
		else
		{
			if ((temponce & 0x03) == 0x02)
			{
				ReadWSD1();
			}
		}
		//END下发读取温湿度的报文


		//红外处理信息
		infrared();

		//按键消抖及处理程序
		key_rjg();

		//按键处理及显示处理
		KeyHander();

		//根据开入情况控制面板显示
		inputcontroloutput();

		//根据开入情况控制声音
		inputsperker();

//		if (Comable.ComAbleFlag0 == 1)
//		{
//			Timer.CommunicationAble0_timer_11s = Timer.soft_timer;
//			if (true)
//			{

//			}

//		}

		//根据定值和当前的温湿度控制出口
		Control_Relay();


		//根据当前对上通讯状态更新通讯指示灯的状态
		if (Timer.soft_timer - Timer.CommunicationToUp_timer > 0)
		{
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_0, 1 << 0);
		}
		else
		{
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_0, (Timer.soft_timer % 500 > 250 ? 0 : 1) << 0);
		}

		//保存更新的定值或设置后2s恢复原来的闪烁状态
		if (Timer.soft_timer - Timer.restoreLedOrgState_timer == 0)
		{

			LcdState.light = LcdState.oldlightstate;
		}

		//密码过期
		if (Timer.soft_timer - password.password_effective_timer == 0)
		{
			password.ifeffective = PASSWORDNOEFFECTIVE;
		}

		//10ms后回复主机的MODBUS报文并标记现有的mt报文寄存器数值无效
		if (Timer.soft_timer - uart6sendbuff.sendbufftimer == 0)
		{
			if (uart6sendbuff.sendbufflength > 0)
			{
				TX_MODBUS_SEND(uart6sendbuff.sendbufflength);
				uart6sendbuff.sendbufflength = 0;
			}
		}




		//每隔5ms刷新数码管的显示
		if (Timer.soft_timer - Timer.ledRefresh_timer > 5)
		{
			Timer.ledRefresh_timer = Timer.soft_timer;
			SEG_Write_data(LcdState.dataup, LcdState.datadown, LcdState.dotsiteup, LcdState.dotsitedown, LcdState.jinzhiup, LcdState.jinzhidown, LcdState.light);
		}

		//控制默认页面的显示，两路温湿度以5s为周期，交替轮流显示
		if (Timer.soft_timer > LcdState.pagetimer)
		{
			LcdState.dotsiteup = 2;
			LcdState.dotsitedown = 2;
			LcdState.jinzhiup = 10;
			LcdState.jinzhidown = 10;
			LcdState.light = 0x00;
			LcdState.page = 0;

			if (Comable.ComAbleFlag0 == 1 && Comable.ComAbleFlag1 == 1)
			{
				LcdState.dataup = wsd[(Timer.soft_timer % 10000 > 5000) ? 0 : 1].temperature;
				LcdState.datadown = wsd[(Timer.soft_timer % 10000 > 5000) ? 0 : 1].humidity;
				yk.power = 0x00;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else if (Comable.ComAbleFlag0 == 0 && Comable.ComAbleFlag1 == 1)
			{
				LcdState.dataup = wsd[1].temperature;
				LcdState.datadown = wsd[1].humidity;
				yk.power = 0xff;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else if (Comable.ComAbleFlag0 == 1 && Comable.ComAbleFlag1 == 0)
			{
				LcdState.dataup = wsd[0].temperature;
				LcdState.datadown = wsd[0].humidity;
				yk.power = 0xff;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else
			{
				LcdState.dataup = 0;
				LcdState.datadown = 0;
				LcdState.light = 0xFF;
				yk.power = 0xff;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
				flag = 4;
				ykIOOut(flag);

			}
		}
		//更新温湿度传感器的通讯状态
		CheckWsdComState();

		//喂狗函数
		//FeedDog();
	}
}


//*****************************************************************************
//
//! Initialize the systemtick.
//!
//! \return None.
//
//*****************************************************************************

void SystemTickInit(void)
{
	ROM_SysTickPeriodSet(SystemCoreClock / 1000);  //Sets the period of the SysTick counter
	ROM_IntPrioritySet(FAULT_SYSTICK, 1 << 5);   //Sets the priority of an interrupt
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
	ROM_IntMasterEnable();
}


//*****************************************************************************
//
//! Systemtick int handler.
//!
//! \return None.
//
//*****************************************************************************
void SysTick_Handler(void)
{
	Timer.soft_timer++;

	//更新开入状态
	input_rjg();
}






//看门狗初始化
void myWatchDogInit(void)
{
	//if (WatchdogLockState(WATCHDOG0_BASE) == true)
	//{
	//	WatchdogUnlock(WATCHDOG0_BASE);
	//}
	//WatchdogStallEnable(WATCHDOG0_BASE);
	//IntEnable(INT_WATCHDOG);
	//WatchdogReloadSet(WATCHDOG0_BASE, ROM_SysCtlClockGet());
	//WatchdogResetEnable(WATCHDOG0_BASE);
	//WatchdogEnable(WATCHDOG0_BASE);
	//WatchdogLock(WATCHDOG0_BASE);

	//
	// Enable the Watchdog 0 peripheral
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);
	//
	// Wait for the Watchdog 0 module to be ready.
	//
	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG0))
	{
	}
	//
	// Check to see if the registers are locked, and if so, unlock them.
	//
	if (WatchdogLockState(WATCHDOG0_BASE) == true)
	{
		WatchdogUnlock(WATCHDOG0_BASE);
	}
	//
	// Initialize the watchdog timer.
	//
	//WatchdogReloadSet(WATCHDOG0_BASE, 0xFEEFEE);
	WatchdogReloadSet(WATCHDOG0_BASE, ROM_SysCtlClockGet());

	//
	// Enable the reset.
	//
	WatchdogResetEnable(WATCHDOG0_BASE);
	//
	// Enable the watchdog timer.
	//
	WatchdogEnable(WATCHDOG0_BASE);
	//
}

//喂狗
void FeedDog(void)
{
	WatchdogUnlock(WATCHDOG0_BASE);
	WatchdogReloadSet(WATCHDOG0_BASE, ROM_SysCtlClockGet());
	WatchdogLock(WATCHDOG0_BASE);
}


//数码管初始化
void lcdPoweronInit(uint8_t e)
{
	int i;
	for (i = 0; i < e; i++)
	{
		LcdState.dataup = ledsum(i, i, i, i, e);
		LcdState.datadown = ledsum(i, i, i, i, e);
		LcdState.dotsiteup = i % 4;
		LcdState.dotsitedown = i % 4;
		LcdState.jinzhiup = e;
		LcdState.jinzhidown = e;
		LcdState.light = 0;
		LcdState.lightconfig = 1000;
		while (Timer.soft_timer < 500)
		{
			ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 1 << 5);
			if (Timer.soft_timer - Timer.ledRefresh_timer > 5)
			{
				Timer.ledRefresh_timer = Timer.soft_timer;
				SEG_Write_data(LcdState.dataup, LcdState.datadown, LcdState.dotsiteup, LcdState.dotsitedown, LcdState.jinzhiup, LcdState.jinzhidown, LcdState.light);

				//喂狗，防止重置MCU
				//FeedDog();
			}
		}
		Timer.soft_timer = 0;
		Timer.ledRefresh_timer = 0;
	}

}


void Control_Relay(void)
{
	int i;
	//uint8_t flag;
	for (i = 0; i < 8; i++)
	{
		output[i].temp_value <<= 1;
	}
	output[0].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_1) & 0x02) >> 1) & 0x01);
	output[1].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_2) & 0x04) >> 2) & 0x01);
	output[2].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_3) & 0x08) >> 3) & 0x01);
	output[3].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_4) & 0x10) >> 4) & 0x01);
	output[4].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_5) & 0x20) >> 5) & 0x01);
	output[5].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_6) & 0x40) >> 6) & 0x01);
	output[6].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_7) & 0x80) >> 7) & 0x01);
	//output[7].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_7) & 0x80) >> 7) & 0x01);

	for (i = 0; i < 7; i++)
	{
		if ((output[i].temp_value & 0x03) == 0x00 || (output[i].temp_value & 0x03) == 0x03)
		{
			if (output[i].nonjitter_times < 20)
			{
				output[i].nonjitter_times++;
			}
		}
		else
		{
			if ((output[i].temp_value & 0x06) == 0x00 || (output[i].temp_value & 0x06) == 0x06)
			{
				output[i].nonjitter_times = 5;
				if ((output[i].temp_value & 0x01) == 0x00)
				{
					output[i].start_time_p = Timer.soft_timer;
				}
				else
				{
					output[i].start_time_n = Timer.soft_timer;
				}
			}
			if (output[i].nonjitter_times > 0)
			{
				output[i].nonjitter_times--;
			}
		}
		if (output[i].nonjitter_times == 10)
		{
			output[i].value = ~(output[i].temp_value) & 0x01;
		}
		if (output[i].nonjitter_times == 0)
		{
			output[i].nonjitter_times = 5;
		}
	}


	if (Comable.ComAbleFlag0 == 1)
	{
		if ((wsd[0].humidity > 800) || (wsd[0].temperature < -400) || (wsd[0].temperature > 800)) //极端环境下直接报警
		{
			//yk.power = 0xff;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
			//flag = 4;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_2, 0x00);  //报警  00是亮  ff灭

		}

		if (((wsd[0].humidity > (myee.StA0)) || (wsd[0].temperature < (myee.WtA0))) && (output[5].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 0;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0x00);  //加热A
		}
		else if (((wsd[0].humidity <= (myee.St00)) && (wsd[0].temperature >= (myee.Wt00))) && (output[5].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 0;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //加热A

		}
		else
		{
			if ((output[5].value == 0x01))
			{

				//yk.power = 0x00;
				//flag = 0;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //加热A

			}

		}



		if (((wsd[0].humidity > (myee.SFA0)) || (wsd[0].temperature > (myee.WFA0))) && (output[3].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 2;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0x00);  //通风1
		}
		else if (((wsd[0].humidity <= (myee.SF00)) && (wsd[0].temperature <= (myee.WF00))) && (output[3].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 2;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0xff);  //通风1
		}
		else
		{
			if (output[3].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 2;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0xff);  //通风1

			}

		}
	}
	else
	{
		//yk.power = 0xff;
		//flag = 2;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0x00);  //通风1

		//yk.power = 0x00;
		//flag = 0;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //加热A

	}

	if (Comable.ComAbleFlag1 == 1)
	{
		if ((wsd[1].humidity > 800) || (wsd[1].temperature < -400) || (wsd[1].temperature > 800)) //极端环境下直接报警
		{
			//yk.power = 0xff;//报警输出 --wushuisheng add 2018年4月16日 14:36:51
			//flag = 4;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_2, 0x00);  //报警
		}


		if (((wsd[1].humidity > (myee.StA0)) || (wsd[1].temperature < (myee.WtA0))) && (output[4].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 1;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0x00);  //加热B
		}
		else if (((wsd[1].humidity <= (myee.St00)) && (wsd[1].temperature >= (myee.Wt00))) && (output[4].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 1;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //加热B
		}
		else
		{
			if (output[4].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 1;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //加热B
			}

		}


		if (((wsd[1].humidity > (myee.SFA0)) || (wsd[1].temperature > (myee.WFA0))) && (output[2].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 3;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0x00);  //通风2

		}
		else if (((wsd[1].humidity <= (myee.SF00)) && (wsd[1].temperature <= (myee.WF00))) && (output[2].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 3;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0xff);  //通风2
		}
		else
		{
			if (output[2].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 3;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0xff);  //通风2

			}

		}

	}
	else
	{
		//yk.power = 0xff;   
		//flag = 3;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0x00);  //通风2
		//yk.power = 0x00;
		//flag = 1;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //加热B
	}
	//储能指示灯控制PD1
	if (output[6].value == 0x01)
	{

		ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);//亮
	}
	else
	{
		ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0xff);//灭

	}
}

