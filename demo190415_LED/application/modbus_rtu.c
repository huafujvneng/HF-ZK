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
#include "../myinc/crc.h"
#include "../headfiles/eeprominit.h"
#include "../driverlib/interrupt.h"
#include "../driverlib/eeprom.h"
#include "../headfiles/led_display.h"

#define START02ADDRESS 0x0000
#define NUMBERSABOUT02 9
#define START03ADDRESS 0x1000
#define NUMBERSABOUT03 10
#define START04ADDRESS 0x2000
#define NUMBERSABOUT04 4
#define START05ADDRESS 0x3000
#define NUMBERSABOUT05 7
#define START06ADDRESS 0x4000
#define NUMBERSABOUT06 8
#define START10ADDRESS 0x5000
#define NUMBERSABOUT10 8


uint32_t ui32Status;
rx_modbus mr0, mr6;
tx_modbus mt0, mt6;
tempint32arr a32;
tempuint32arr ua32;
tempuint8arr ua8;
tempint8arr a8;
tempuint16arr ua16;
tempint16arr a16;
virtualyk yk;
virtualsend uart6sendbuff;
structComAble Comable;
//int numbs;
//int see;
virtualwsd wsd[2];

//void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count);

void TX_MODBUS(uint8_t *p, uint8_t length)
{
	//UARTIntDisable(UART6_BASE, UART_INT_RX);
	mt0.dt[0] = mr0.dr[0];
	mt0.dt[length] = ((Modbus_Crc(p, length)) >> 0) & 0xff;
	length++;
	mt0.dt[length] = ((Modbus_Crc(p, length - 1)) >> 8) & 0xff;
	length++;
	uart6sendbuff.sendbufflength = length;				//记忆发送报文寄存器中有效报文长度
	uart6sendbuff.sendbufftimer = Timer.soft_timer + 10;		//延时10ms后回复主机报文
}


//把发送寄存机器数据通过串口0发送出去
void TX_MODBUS_SEND(uint8_t sendbufflength)
{
	int i;
	for (i = 0; i < sendbufflength; i++)
	{
		UARTCharPut(UART0_BASE, mt0.dt[i]);
	}
	mr0.lock_state = UNLOCK;
	//UARTIntEnable(UART6_BASE, UART_INT_RX);
}

//查询或修改的地址错误
void addr_error(void)
{
	mt0.dt[1] = mr0.dr[1] + 0x80;
	mt0.dt[2] = 0x82;
	TX_MODBUS(mt0.dt, 3);
}

//查询数据的数量或者写入的修改值错误
void data_error(void)
{
	mt0.dt[1] = mr0.dr[1] + 0x80;
	mt0.dt[2] = 0x83;
	TX_MODBUS(mt0.dt, 3);
}


//功能码错误
void code_error(void)
{
	mt0.dt[1] = mr0.dr[1] + 0x80;
	mt0.dt[2] = 0x81;
	TX_MODBUS(mt0.dt, 3);
}


//装置内存错误
void internal_error(void)
{
	mt0.dt[1] = mr0.dr[1] + 0x80;
	mt0.dt[2] = 0x84;
	TX_MODBUS(mt0.dt, 3);
}

//按位查询遥信量，起始地址为0000H
void tx_modbus_02(void)
{
	uint16_t tempaddr;
	uint16_t tempnums;
	int i;
	//uint32_t trueaddress;
	tempaddr = (mr0.dr[2] << 8) + mr0.dr[3];
	tempnums = (mr0.dr[4] << 8) + mr0.dr[5];
	if (tempaddr > (START02ADDRESS + NUMBERSABOUT02 - 1))
	{
		addr_error();
		return;
	}
	if (tempnums > (START02ADDRESS + NUMBERSABOUT02 - tempaddr))
	{
		data_error();
		return;
	}
	mt0.dt[1] = mr0.dr[1];
	mt0.dt[2] = tempnums / 8;
	if (tempnums % 8 != 0)
	{
		mt0.dt[2]++;
	}
	for (i = 0; i < tempnums; i++)
	{
		//按照先低位后高位的方式把查询的开入量放置到对应的报文字节上，
		//按字节回，每字节代表8个开入，不足8个高位无效
		mt0.dt[3 + (i / 8)] &= (~(0x01 << (i % 8)));
		mt0.dt[3 + (i / 8)] |= (((input[i + tempaddr].value) & 0x01) << (i % 8));
	}

	TX_MODBUS(mt0.dt, mt0.dt[2] + 3);
}

//查询定值，起始地址为1000H
void tx_modbus_03(void)
{
	uint16_t tempaddr;
	uint16_t tempnums;
	uint32_t trueaddress;
	int i;
	tempaddr = (mr0.dr[2] << 8) + mr0.dr[3];
	tempnums = (mr0.dr[4] << 8) + mr0.dr[5];
	if ((tempaddr < START03ADDRESS) || (tempaddr > (START03ADDRESS + NUMBERSABOUT03 - 1)))
	{
		addr_error();
		return;
	}
	if (tempnums > (START03ADDRESS + NUMBERSABOUT03 - tempaddr))
	{
		data_error();
		return;
	}
	if (EEPROMStatusGet() == 0)
	{
		tempint32init();
		trueaddress = 4 * (tempaddr - 0x1000 + 12);
		EEPROMRead((uint32_t *)a32.data, trueaddress, 4 * tempnums);
	}
	else
	{
		internal_error();
		return;
	}
	mt0.dt[1] = mr0.dr[1];
	mt0.dt[2] = 2 * mr0.dr[5];
	for (i = 0; i < tempnums; i++)
	{
		mt0.dt[2 * i + 3 + 0] = (a32.data[i] >> 8) & 0xff;
		mt0.dt[2 * i + 3 + 1] = (a32.data[i] >> 0) & 0xff;
	}
	TX_MODBUS(mt0.dt, mt0.dt[2] + 3);
}

void tx_modbus_04(void)
{
	uint16_t tempaddr;
	uint16_t tempnums;
	int i;
	tempaddr = (mr0.dr[2] << 8) + mr0.dr[3];
	tempnums = (mr0.dr[4] << 8) + mr0.dr[5];
	if ((tempaddr < START04ADDRESS) || (tempaddr > (START04ADDRESS + NUMBERSABOUT04 - 1)))
	{
		addr_error();
		return;
	}
	if (tempnums > (START04ADDRESS + NUMBERSABOUT04 - tempaddr))
	{
		data_error();
		return;
	}
	mt0.dt[1] = mr0.dr[1];
	mt0.dt[2] = 2 * tempnums;
	tempint16init();
	a16.data[0] = wsd[0].humidity;
	a16.data[1] = wsd[0].temperature;
	a16.data[2] = wsd[1].humidity;
	a16.data[3] = wsd[1].temperature;
	for (i = 0; i < tempnums; i++)
	{
		mt0.dt[2 * i + 3 + 0] = (a16.data[i + tempaddr - START04ADDRESS] >> 8) & 0xff;
		mt0.dt[2 * i + 3 + 1] = (a16.data[i + tempaddr - START04ADDRESS] >> 0) & 0xff;
	}
	TX_MODBUS(mt0.dt, mt0.dt[2] + 3);
}
//此处已经加入远方就地控制信号接入，当在就地挡位时，远方信号无法下发控制就地侧
void tx_modbus_05(void)
{
	uint16_t tempaddr;
	uint16_t tempdata;
	uint8_t flag;
	if (input[5].value == 0x01)  //
	{
		tempaddr = (mr0.dr[2] << 8) + mr0.dr[3];
		tempdata = (mr0.dr[4] << 8) + mr0.dr[5];
		if ((tempdata != 0x0000) && (tempdata != 0xffff))
		{
			data_error();
			return;
		}
		if (tempaddr < START05ADDRESS || tempaddr >(START05ADDRESS + NUMBERSABOUT05 - 1))
		{
			addr_error();
			return;
		}
		flag = (tempaddr - 0x3000) & 0xff;
		yk.power = (tempdata & 0xff);
		ykIOOut(flag);
		mt0.dt[1] = mr0.dr[1];
		mt0.dt[2] = mr0.dr[2];
		mt0.dt[3] = mr0.dr[3];
		mt0.dt[4] = mr0.dr[4];
		mt0.dt[5] = mr0.dr[5];
		TX_MODBUS(mt0.dt, 6);
	}
}

void tx_modbus_06(void)
{
	uint16_t tempaddr;

	int16_t tempdata;
	uint32_t trueaddress;

	tempaddr = (mr0.dr[2] << 8) + mr0.dr[3];
	tempdata = (mr0.dr[4] << 8) + mr0.dr[5];
	if ((tempaddr < START06ADDRESS) || (tempaddr > (START06ADDRESS + NUMBERSABOUT06 - 1)))
	{
		addr_error();
		return;
	}
	trueaddress = 4 * (tempaddr - START06ADDRESS + 12);
	switch (trueaddress)
	{
	case WT00_EE_ADDRESS:
		if (tempdata < -400 || tempdata >1000)
		{
			data_error();
			return;
		}
		break;

	case WTA0_EE_ADDRESS:
		if (tempdata < -400 || tempdata >1000)
		{
			data_error();
			return;
		}
		break;

	case WF00_EE_ADDRESS:
		if (tempdata < -400 || tempdata >1000)
		{
			data_error();
			return;
		}
		break;

	case WFA0_EE_ADDRESS:
		if (tempdata < -400 || tempdata >1000)
		{
			data_error();
			return;
		}
		break;

	case ST00_EE_ADDRESS:
		if (tempdata < 200 || tempdata >900)
		{
			data_error();
			return;
		}
		break;

	case STA0_EE_ADDRESS:
		if (tempdata < 200 || tempdata >900)
		{
			data_error();
			return;
		}
		break;

	case SF00_EE_ADDRESS:
		if (tempdata < 200 || tempdata >900)
		{
			data_error();
			return;
		}
		break;

	case SFA0_EE_ADDRESS:
		if (tempdata < 200 || tempdata >900)
		{
			data_error();
			return;
		}
		break;

	default:
		break;
	}
	EEpromWrite(tempdata, trueaddress);
	myEEpromInit();
	//	UartInit();
	mt0.dt[1] = mr0.dr[1];
	mt0.dt[2] = mr0.dr[2];
	mt0.dt[3] = mr0.dr[3];
	mt0.dt[4] = mr0.dr[4];
	mt0.dt[5] = mr0.dr[5];
	TX_MODBUS(mt0.dt, 6);
}

void tx_modbus_10(void)
{
	uint16_t tempaddr;
	uint16_t dataNumbers;
	uint8_t tempdata[16];
	uint8_t i;
	uint32_t TDWriteEE[8];
	tempaddr = mr0.dr[2] * 0x100 + mr0.dr[3];
	dataNumbers = mr0.dr[6] / 2;
	if ((tempaddr < START10ADDRESS) || (tempaddr > (START10ADDRESS + NUMBERSABOUT10 - 1)))
	{
		addr_error();
		return;
	}
	if (dataNumbers > (START10ADDRESS + NUMBERSABOUT10 - tempaddr))
	{
		data_error();
		return;
	}

	for (i = 0; i < dataNumbers; i++)
	{
		tempdata[i * 2 + 0] = mr0.dr[i * 2 + 7];
		tempdata[i * 2 + 1] = mr0.dr[i * 2 + 8];
	}
	if (Virtual_EEpromWrite_Check(tempdata, tempaddr, dataNumbers) == 100)
	{
		for (i = 0; i < dataNumbers; i++)
		{
			TDWriteEE[i] = tempdata[i * 2 + 0] * 0x100 + tempdata[i * 2 + 1];
		}
		EEPROMProgram(TDWriteEE, 4 * (tempaddr + 12), 4 * dataNumbers);//读取内存地址0x0开始 pui32Read长度的数值
		mt0.dt[1] = mr0.dr[1];
		mt0.dt[2] = mr0.dr[2];
		mt0.dt[3] = mr0.dr[3];
		mt0.dt[4] = mr0.dr[4];
		mt0.dt[5] = mr0.dr[5];
		myEEpromInit();
		TX_MODBUS(mt0.dt, 6);

	}
	else
	{
		data_error();
		return;
	}
}

void tx_modbus_18(void)
{

}

void UART0_Handler(void)
{
	uint8_t *pt1;
	uint8_t func1;
	//uint8_t addr;
	if (mr0.lock_state == UNLOCK)
	{
		rx0_modbus_init();
		mr0.rxpt = (uint8_t *)mr0.dr;
		mr0.ifdone = 0;
		mr0.lock_state = LOCKED;
	}
	ui32Status = ROM_UARTIntStatus(UART0_BASE, true);
	ROM_UARTIntClear(UART0_BASE, ui32Status);
	while (ROM_UARTCharsAvail(UART0_BASE))
	{
		*(mr0.rxpt) = UARTCharGet(UART0_BASE);
		mr0.rxpt++;
		(mr0.count)++;
	}
	if (mr0.dr[0] != myee.Add0)
	{
		mr0.lock_state = UNLOCK;
		return;
	}
	if (mr0.dr[1] == 0x02 || mr0.dr[1] == 0x03 || mr0.dr[1] == 0x04 || mr0.dr[1] == 0x05 || mr0.dr[1] == 0x06)
	{
		if (mr0.count == 8)
		{
			mr0.ifdone = 1;
		}
		else if (mr0.count > 8)
		{
			mr0.lock_state = UNLOCK;
			return;
		}
	}
	if (mr0.dr[1] == 0x10)
	{
		if (mr0.count == mr0.dr[6] + 9)
		{
			mr0.ifdone = 1;
		}
		else if (mr0.count > mr0.dr[6] + 9)
		{
			mr0.lock_state = UNLOCK;
			return;
		}
	}
	if (mr0.count > 30)
	{
		mr0.lock_state = UNLOCK;
		return;
	}


	//检验校验码是否合法
	if (mr0.ifdone == 1)
	{
		pt1 = (uint8_t *)mr0.dr;
		func1 = *(pt1 + 1);
		//addr = *(pt);
		if ((*(pt1 + mr0.count - 2) != ((Modbus_Crc(pt1, mr0.count - 2) >> 0) & 0xff)) || (*(pt1 + mr0.count - 1) != ((Modbus_Crc(pt1, mr0.count - 2) >> 8) & 0xff)))
		{
			mr0.lock_state = UNLOCK;
			return;
		}


		//用于判断设备对上的通讯是否建立
		Timer.CommunicationToUp_timer = Timer.soft_timer + 3000;

		//根据功能码选择回应的报文
		switch (func1)
		{
		case 0x02:
			tx_modbus_02();
			break;
		case 0x03:
			tx_modbus_03();
			break;
		case 0x04:
			tx_modbus_04();
			break;
		case 0x05:
			tx_modbus_05();
			break;
		case 0x06:
			tx_modbus_06();
			break;
		case 0x10:
			tx_modbus_10();
			break;
		case 0x18:
			tx_modbus_18();
			break;
		default:
			code_error();
			break;
		}
	}
}

void UART6_Handler(void)
{
	uint8_t *pt;
	//uint8_t func;
	//uint8_t addr;
	if (mr6.lock_state == UNLOCK)
	{
		rx6_modbus_init();
		mr6.rxpt = (uint8_t *)mr6.dr;
		mr6.ifdone = 0;
		mr6.lock_state = LOCKED;
	}
	ui32Status = ROM_UARTIntStatus(UART6_BASE, true);
	ROM_UARTIntClear(UART6_BASE, ui32Status);
	while (ROM_UARTCharsAvail(UART6_BASE))
	{
		*(mr6.rxpt) = UARTCharGet(UART6_BASE);
		mr6.rxpt++;
		(mr6.count)++;
	}
	if ((mr6.dr[0] != 0x0B && mr6.dr[0] != 0x0C) || mr6.dr[1] != 0x04 || mr6.dr[2] != 0x04)
	{
		mr6.lock_state = UNLOCK;
		return;
	}
	if((mr6.dr[3]==0xff) && (mr6.dr[4]==0xff) && (mr6.dr[5]==0xff)&&(mr6.dr[6]==0xff)) //20180418
	{
		mr6.lock_state = UNLOCK;
		return;
	}
	if (mr6.count == 9)
	{
		mr6.ifdone = 1;
	}
	else if (mr6.count > 9)
	{
		mr6.lock_state = UNLOCK;
		return;
	}


	//检验校验码是否合法
	if (mr6.ifdone == 1)
	{
		pt = (uint8_t *)mr6.dr;
		//func = *(pt + 1);
		//addr = *(pt);
		if ((*(pt + mr6.count - 2) != ((Modbus_Crc(pt, mr6.count - 2) >> 0) & 0xff)) || (*(pt + mr6.count - 1) != ((Modbus_Crc(pt, mr6.count - 2) >> 8) & 0xff)))
		{
			mr6.lock_state = UNLOCK;
			return;
		}
		//读取温湿度的数据
		wsd[mr6.dr[0] - 0x0B].humidity = ((*(pt + 5) * 0xff) + *(pt + 6))/10;
		if (mr6.dr[0] == 0x0B)
		{
			Timer.CommunicationAble0_timer = Timer.soft_timer + 3000;
		}
		if (mr6.dr[0] == 0x0C)
		{
			Timer.CommunicationAble1_timer = Timer.soft_timer + 3000;
		}
		switch (*(pt + 3) & 0x80)
		{
			//如果是正数的温度
		case 0x00:
			wsd[mr6.dr[0] - 0x0B].temperature = ((*(pt + 3) & 0x7f) * 0x100 + *(pt + 4))/10;

			//wsd.temperature = -((*(pt + 5) * 0x7f) + *(pt + 6));
			break;

			//如果是负数的温度
		case 0x80:
			wsd[mr6.dr[0] - 0x0B].temperature = -((*(pt + 3) & 0x7f) * 0x100 + *(pt + 4)) / 10;
			//wsd[mr6.dr[0] - 0x0B].temperature = ((*(pt + 3) & 0x7f) * 0x100 + *(pt + 4)) / 1;
			break;
		default:
			break;
		}
		mr6.lock_state = UNLOCK;
		return;
	}
}

//定义发送串口0的函数
void UART0Send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	while (ui32Count--)
	{
		ROM_UARTCharPutNonBlocking(UART0_BASE, *pui8Buffer++);
	}
}
//END定义发送串口的函数


//定义发送串口6的函数
void UART6Send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
	while (ui32Count--)
	{
		ROM_UARTCharPutNonBlocking(UART6_BASE, *pui8Buffer++);
	}
}
//END定义发送串口的函数

//初始化串口6并发送提示信息
void UartInit(void)
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	//ROM_UARTFIFOLevelSet(UART6_BASE, UART_FIFO_TX4_8, UART_FIFO_RX7_8);
	ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), myee.bAu0, (myee.dAb0 | myee.Stb0 | myee.CHd0));
	ROM_IntEnable(INT_UART0);
	ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	UART0Send("0scuss", 8);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);
	ROM_GPIOPinConfigure(GPIO_PD4_U6RX);
	ROM_GPIOPinConfigure(GPIO_PD5_U6TX);
	ROM_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	//ROM_UARTFIFOLevelSet(UART6_BASE, UART_FIFO_TX4_8, UART_FIFO_RX7_8);
	ROM_UARTConfigSetExpClk(UART6_BASE, SysCtlClockGet(), myee.bAu1, (myee.dAb1 | myee.Stb1 | myee.CHd1));
	ROM_IntEnable(INT_UART6);
	ROM_UARTIntEnable(UART6_BASE, UART_INT_RX | UART_INT_RT);

	UART0Send("6scuss", 8);

}
//END初始化串口6并发送提示信息



//初始化串口接受寄存器
void rx0_modbus_init(void)
{
	uint8_t i;
	for (i = 0; i < 255; i++)
	{
		mr0.dr[i] = 0x00;
	}
	mr0.count = 0;
	mr0.loopnum = 0;
	mr0.lock_state = UNLOCK;
}
//END初始化串口接受寄存器

//初始化串口接受寄存器 温湿度传感器用
void rx6_modbus_init(void)
{
	uint8_t i;
	for (i = 0; i < 9; i++)
	{
		mr6.dr[i] = 0xFF;//--wushuisheng add 2018年4月18日 15:11:16
	}
	mr6.count = 0;
	mr6.loopnum = 0;
	mr6.lock_state = UNLOCK;
}
//END初始化串口接受寄存器


//初始化发送报文寄存器
void tx0_modbus_init(void)
{
	uint8_t i;
	for (i = 0; i < 255; i++)
	{
		mt0.dt[i] = 0x00;
	}
	mt0.count = 0;
	mt0.loopnum = 0;
	mt0.lock_state = UNLOCK;
}
//END初始化发送报文寄存器


//初始化发送报文寄存器
void tx6_modbus_init(void)
{
	uint8_t i;
	for (i = 0; i < 9; i++)
	{
		mt6.dt[i] = 0x00;
	}
	mt6.count = 0;
	mt6.loopnum = 0;
	mt6.lock_state = UNLOCK;
}
//END初始化发送报文寄存器

//初始化ua32寄存器
void tempuint32init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		ua32.data[i] = 0;
		ua32.pt = ua32.data;
	}
}
//END初始化ua32寄存器

//初始化ua32寄存器
void tempint32init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		a32.data[i] = 0;
		a32.pt = a32.data;
	}
}
//END初始化ua32寄存器

//初始化ua8寄存器
void tempuint8init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		ua8.data[i] = 0;
		ua8.pt = ua8.data;
	}
}
//END初始化ua8寄存器

//初始化a8寄存器
void tempint8init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		a8.data[i] = 0;
		a8.pt = a8.data;
	}
}
//END初始化a8寄存器


//初始化ua16寄存器
void tempuint16init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		ua16.data[i] = 0;
		ua16.pt = ua16.data;
	}
}
//END初始化ua16寄存器

//初始化a16寄存器
void tempint16init(void)
{
	uint8_t i;
	for (i = 0; i < 100; i++)
	{
		a16.data[i] = 0;
		a16.pt = a16.data;
	}
}
//END初始化a16寄存器


//下发查询温湿度的报文
void ReadWSD0(void)
{
	uint8_t TXWSD[8] = { 0x0B,0x04,0x00,0x00,0x00,0x02,0x71,0x61 };
	UART6Send(TXWSD, 8);
}
void ReadWSD1(void)
{
	uint8_t TXWSD[8] = { 0x0C,0x04,0x00,0x00,0x00,0x02,0x70,0xd6 };
	//uint8_t TXWSD[8] = { 0x11,0x06,0x00,0x00,0x00,0x12,0x0b,0x57 };
	UART6Send(TXWSD, 8);
}
//END下发查询温湿度的报文


//执行下发的强制单线圈指令控制出口输出
void ykIOOut(uint8_t flag)
{
	switch (flag + 1)
	{
	case 1:
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_6, ~yk.power);  //加热A
		break;													  //
																  //
	case 2:														  //
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_5, ~yk.power);  //加热B
		break;													  //
																  //
	case 3:														  //
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_4, ~yk.power);  //通风1
		break;													  //
																  //
	case 4:														  //
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_3, ~yk.power);  //通风2
		break;													  //
																  //
	case 5:														  //
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_2, ~yk.power);  //报警
		break;													  //
																  //
	case 6:														  //
		GPIOPinWrite(GPIO_PORTF_AHB_BASE, GPIO_PIN_1, ~yk.power);  //高压闭锁
		break;													  //
																  //
	case 7:														  //
		GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_7, ~yk.power);  //备用出口
		break;													  //
	default:													  //
		break;
	}
}
//END执行下发的强制单线圈指令控制出口输出

void CheckWsdComState(void)
{
	//刷新温湿度传感器的通讯状态
	if (Timer.soft_timer - Timer.CommunicationAble0_timer > 0)
	{
		Comable.ComAbleFlag0 = 0;
	}
	else
	{
		Comable.ComAbleFlag0 = 1;
	}
	if (Timer.soft_timer - Timer.CommunicationAble1_timer > 0)
	{
		Comable.ComAbleFlag1 = 0;
	}
	else
	{
		Comable.ComAbleFlag1 = 1;
	}

}
