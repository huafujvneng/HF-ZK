#include <stdint.h>
#include <stdbool.h>
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

#include "../headfiles/gpioinit.h"
#include "../driverlib/eeprom.h"
#include "../utils/uartstdio.h"
#include "../headfiles/infrared.h"
#include "../headfiles/eeprominit.h"
#include "../headfiles/speaker.h"
//有人接近 Near_P
uint8_t Near_P[3] = { 音量10,连码播放,本柜回路已带电 };

inf_detectors inf1;
void infrared(void)
{
	static int dengliang;
	static uint8_t firstOncePInfrared;
	firstOncePInfrared <<= 1;
	//读取红外对应的cpu的端口的值，如果是1则表明红外是触发的
	if (ROM_GPIOPinRead(GPIO_PORTG_AHB_BASE, GPIO_PIN_7))
	{
		firstOncePInfrared += 1;
		if ((firstOncePInfrared & 0x03) == 0x01)
		{
			Timer.infHold_timer = Timer.soft_timer + 60000;
			//Timer.infHold_timer = Timer.soft_timer + 10000;
			if ((dengliang == 1) && (spk.daidianflag == 1))
			{
				speak_lock(Near_P);
			}
		}
	}
	else
	{

	}

	//否则关闭数码管显示
	if (Timer.soft_timer - Timer.infHold_timer > 0)
	{
		ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 0 << 5);
		dengliang = 1;
	}

	//如果数码管显示开关是ON则数码管处于点亮状态
	else
	{
		ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 1 << 5);
		dengliang = 0;
		//	ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 0 << 5);
	}

}

//  闭锁 
void  Bisuo(void)
{
	if (ROM_GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_1) == 0)
	{
		if (ROM_GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) == 0)
		{
			ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 1 << 7);
		}
	}
	else
	{
		ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0 << 7);

	}


}





