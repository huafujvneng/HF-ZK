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

//20180418 �޸�modbus_rtu.c�ļ�����529-533�м�if������main����

//
uint32_t SystemCoreClock, ggvalue;
extern volatile int updatelcdtimer;
extern uint8_t orgcontent[100];



extern inf_detectors inf1;
extern const uint32_t	SEG_CTL[8];
uint8_t b[1] = { 0x0d };//�����ʼ���ɹ�
uint8_t temponce;//�����·����ĵĸ�����������֤1s�·�һ��
int fd = 0;
void SystemTickInit(void);
void myWatchDogInit(void);
void FeedDog(void);
void lcdPoweronInit(uint8_t e);
void Control_Relay(void);

//���Ź��жϺ���
void WDT0_Handler(void)
{
	WatchdogIntClear(WATCHDOG0_BASE);
}
uint8_t Near_P2[3] = { ����10,���벥��,�����·�Ѵ��� };



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

	//��ʼ�������״̬Ϊȫ��
	for (i = 0; i < 8; i++)
	{
		HWREG(SEG_CTL[i]) = 0;
	}

	//���ų�ʼ���ɹ�����
	Speaker(0x0d);
	Timer.infHold_timer = Timer.soft_timer + 12000;

	while (1)
	{
		//�·���ʪ�Ȳ�ѯ���ĵĸ�������
		temponce <<= 1;

		//��ÿ��Ź��ĵ�ǰֵ
		//ggvalue = WatchdogValueGet(WATCHDOG0_BASE);

		//������ѯ�ж�
		Bisuo();

		if (Timer.soft_timer - Timer.daidianyuyin_timer > 0)
		{
			if (spk.daidianyuyinflag == 1)
			{
				spk.daidianyuyinflag = 0;
				speak_lock(Near_P2);//--wushuisheng add 2018��4��18�� 13:01:50

			}
		}


		//�·���ȡ��ʪ�ȵı��ģ���·��ʪ�ȣ����500ms�����·�
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
		//END�·���ȡ��ʪ�ȵı���


		//���⴦����Ϣ
		infrared();

		//�����������������
		key_rjg();

		//����������ʾ����
		KeyHander();

		//���ݿ���������������ʾ
		inputcontroloutput();

		//���ݿ��������������
		inputsperker();

//		if (Comable.ComAbleFlag0 == 1)
//		{
//			Timer.CommunicationAble0_timer_11s = Timer.soft_timer;
//			if (true)
//			{

//			}

//		}

		//���ݶ�ֵ�͵�ǰ����ʪ�ȿ��Ƴ���
		Control_Relay();


		//���ݵ�ǰ����ͨѶ״̬����ͨѶָʾ�Ƶ�״̬
		if (Timer.soft_timer - Timer.CommunicationToUp_timer > 0)
		{
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_0, 1 << 0);
		}
		else
		{
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_0, (Timer.soft_timer % 500 > 250 ? 0 : 1) << 0);
		}

		//������µĶ�ֵ�����ú�2s�ָ�ԭ������˸״̬
		if (Timer.soft_timer - Timer.restoreLedOrgState_timer == 0)
		{

			LcdState.light = LcdState.oldlightstate;
		}

		//�������
		if (Timer.soft_timer - password.password_effective_timer == 0)
		{
			password.ifeffective = PASSWORDNOEFFECTIVE;
		}

		//10ms��ظ�������MODBUS���Ĳ�������е�mt���ļĴ�����ֵ��Ч
		if (Timer.soft_timer - uart6sendbuff.sendbufftimer == 0)
		{
			if (uart6sendbuff.sendbufflength > 0)
			{
				TX_MODBUS_SEND(uart6sendbuff.sendbufflength);
				uart6sendbuff.sendbufflength = 0;
			}
		}




		//ÿ��5msˢ������ܵ���ʾ
		if (Timer.soft_timer - Timer.ledRefresh_timer > 5)
		{
			Timer.ledRefresh_timer = Timer.soft_timer;
			SEG_Write_data(LcdState.dataup, LcdState.datadown, LcdState.dotsiteup, LcdState.dotsitedown, LcdState.jinzhiup, LcdState.jinzhidown, LcdState.light);
		}

		//����Ĭ��ҳ�����ʾ����·��ʪ����5sΪ���ڣ�����������ʾ
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
				yk.power = 0x00;//������� --wushuisheng add 2018��4��16�� 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else if (Comable.ComAbleFlag0 == 0 && Comable.ComAbleFlag1 == 1)
			{
				LcdState.dataup = wsd[1].temperature;
				LcdState.datadown = wsd[1].humidity;
				yk.power = 0xff;//������� --wushuisheng add 2018��4��16�� 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else if (Comable.ComAbleFlag0 == 1 && Comable.ComAbleFlag1 == 0)
			{
				LcdState.dataup = wsd[0].temperature;
				LcdState.datadown = wsd[0].humidity;
				yk.power = 0xff;//������� --wushuisheng add 2018��4��16�� 14:36:51
				flag = 4;
				ykIOOut(flag);
			}
			else
			{
				LcdState.dataup = 0;
				LcdState.datadown = 0;
				LcdState.light = 0xFF;
				yk.power = 0xff;//������� --wushuisheng add 2018��4��16�� 14:36:51
				flag = 4;
				ykIOOut(flag);

			}
		}
		//������ʪ�ȴ�������ͨѶ״̬
		CheckWsdComState();

		//ι������
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

	//���¿���״̬
	input_rjg();
}






//���Ź���ʼ��
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

//ι��
void FeedDog(void)
{
	WatchdogUnlock(WATCHDOG0_BASE);
	WatchdogReloadSet(WATCHDOG0_BASE, ROM_SysCtlClockGet());
	WatchdogLock(WATCHDOG0_BASE);
}


//����ܳ�ʼ��
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

				//ι������ֹ����MCU
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
		if ((wsd[0].humidity > 800) || (wsd[0].temperature < -400) || (wsd[0].temperature > 800)) //���˻�����ֱ�ӱ���
		{
			//yk.power = 0xff;//������� --wushuisheng add 2018��4��16�� 14:36:51
			//flag = 4;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_2, 0x00);  //����  00����  ff��

		}

		if (((wsd[0].humidity > (myee.StA0)) || (wsd[0].temperature < (myee.WtA0))) && (output[5].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 0;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0x00);  //����A
		}
		else if (((wsd[0].humidity <= (myee.St00)) && (wsd[0].temperature >= (myee.Wt00))) && (output[5].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 0;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //����A

		}
		else
		{
			if ((output[5].value == 0x01))
			{

				//yk.power = 0x00;
				//flag = 0;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //����A

			}

		}



		if (((wsd[0].humidity > (myee.SFA0)) || (wsd[0].temperature > (myee.WFA0))) && (output[3].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 2;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0x00);  //ͨ��1
		}
		else if (((wsd[0].humidity <= (myee.SF00)) && (wsd[0].temperature <= (myee.WF00))) && (output[3].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 2;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0xff);  //ͨ��1
		}
		else
		{
			if (output[3].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 2;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0xff);  //ͨ��1

			}

		}
	}
	else
	{
		//yk.power = 0xff;
		//flag = 2;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, 0x00);  //ͨ��1

		//yk.power = 0x00;
		//flag = 0;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, 0xff);  //����A

	}

	if (Comable.ComAbleFlag1 == 1)
	{
		if ((wsd[1].humidity > 800) || (wsd[1].temperature < -400) || (wsd[1].temperature > 800)) //���˻�����ֱ�ӱ���
		{
			//yk.power = 0xff;//������� --wushuisheng add 2018��4��16�� 14:36:51
			//flag = 4;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_2, 0x00);  //����
		}


		if (((wsd[1].humidity > (myee.StA0)) || (wsd[1].temperature < (myee.WtA0))) && (output[4].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 1;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0x00);  //����B
		}
		else if (((wsd[1].humidity <= (myee.St00)) && (wsd[1].temperature >= (myee.Wt00))) && (output[4].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 1;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //����B
		}
		else
		{
			if (output[4].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 1;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //����B
			}

		}


		if (((wsd[1].humidity > (myee.SFA0)) || (wsd[1].temperature > (myee.WFA0))) && (output[2].value == 0x00))
		{
			//yk.power = 0xff;
			//flag = 3;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0x00);  //ͨ��2

		}
		else if (((wsd[1].humidity <= (myee.SF00)) && (wsd[1].temperature <= (myee.WF00))) && (output[2].value == 0x01))
		{
			//yk.power = 0x00;
			//flag = 3;
			//ykIOOut(flag);
			ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0xff);  //ͨ��2
		}
		else
		{
			if (output[2].value == 0x01)
			{

				//yk.power = 0x00;
				//flag = 3;
				//ykIOOut(flag);
				ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0xff);  //ͨ��2

			}

		}

	}
	else
	{
		//yk.power = 0xff;   
		//flag = 3;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, 0x00);  //ͨ��2
		//yk.power = 0x00;
		//flag = 1;
		//ykIOOut(flag);
		ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, 0xff);  //����B
	}
	//����ָʾ�ƿ���PD1
	if (output[6].value == 0x01)
	{

		ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);//��
	}
	else
	{
		ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0xff);//��

	}
}

