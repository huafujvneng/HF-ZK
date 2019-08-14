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
#include "../headfiles/modbus_rtu.h"
#include "../headfiles/gpioinit.h"

#include "../driverlib/interrupt.h"
#include "../inc/hw_timer.h"
#include "../driverlib/timer.h"
#include "../inc/hw_i2c.h"
#include "../driverlib/i2c.h"

//*****************************************************************************
//
//! Initialize the GPIO.
//!
//! \return None.
//
//*****************************************************************************
uint8_t tmppa3;
extern uint8_t flag;


void GPIOInit(void)
{
	//使能看门狗
//	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);


	//使能所有的端口
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

	//把下面的端口初始化为IO口
	//SysCtlGPIOEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOC);
	//SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOG);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOH);
	SysCtlGPIOAHBEnable(SYSCTL_PERIPH_GPIOJ);


	//END使能所有的GPIO口

	//RUN LED
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_AHB_BASE, GPIO_PIN_7);				//储能指示灯LED

	// SEG CONTROL 使能显示的数据口PH
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_AHB_BASE, 0xFF);

	// SEG DATA
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	//ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 0<<5);

/******************************红外控制初始化***********************/

	//设置红外输入PG7口为输入模式,并且初始化赋值为0
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTG_AHB_BASE, GPIO_PIN_7);

	//装置刚上电时先给一次红外信号，保证显示界面点亮
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_7, 0<< 7);

	//END设置红外输入PG4口为输入模式,并且初始化赋值为0

	//设置PA2，PA3，PA4，PA5为输入端口 此4个端口为4个按键
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
	//END设置PA2，PA3，PA4，PA5为输入端口









	/********************************指示灯初始化****************************************/
	//PF0解锁，用作通讯灯，通讯正常闪烁
	HWREG(GPIO_PORTF_AHB_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_AHB_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_AHB_BASE + GPIO_O_LOCK) = 0;

	//初始化出口继电器指示灯全灭
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	ROM_GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, 0xff);
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0xff);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_AHB_BASE, GPIO_PIN_1);

	//初始化8个开入，可去led_display.c中找开入防抖的函数去对应8个开入
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTJ_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_AHB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTC_AHB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);


	//闭锁开入信号输入
	ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_AHB_BASE, GPIO_PIN_1);
	//闭锁继电器输出端口  初始化高电平  不动作  --wushuisheng add 2018年4月19日 18:46:27
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0xff);

	//初始化7个开出，此版程序指示部分采用经过CPU的方式控制的
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTG_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

	//初始化PK1，暂时作备用
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_1);
	ROM_GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, 1 << 1);

	////语音芯片复位
	//ROM_GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
	//ROM_GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 1 << 0);

	//初始化语音芯片三线串口+语音芯片复位控制的IO口
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_AHB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

}
//初始化定时器TIMER0A，用作语音芯片的传数据控制
void myTimerInit(void)
{
	//初始化TIMER0
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	//设置TIMER0采用16位宽度，周期减计数
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT | TIMER_CFG_A_PERIODIC);

	//设置TIMER0A的装载值为8000，即1ms计数
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 16000);

	//使能TIMER0A
	ROM_IntEnable(INT_TIMER0A);

	//使能TIMER0A的溢出终端
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//使能计数器TIMER0A
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}





