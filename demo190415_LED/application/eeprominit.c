#include <stdint.h>
#include <stdbool.h>

#include "../headfiles/eeprominit.h"
#include "../driverlib/eeprom.h"
#include "../inc/hw_eeprom.h"
#include "../driverlib/eeprom.h"
#include "../driverlib/pin_map.h"
#include "../inc/hw_memmap.h"
#include "../driverlib/sysctl.h"

eeprominit myee, eepromdata;
structTimer Timer;
//初始化EEPROM的值
void myEEpromInit(void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	EEPROMInit();

	//如果EEPROM当前非忙碌状态
	if (EEPROMStatusGet() == 0)
	{

		//读取Add0的装置地址
		EEPROMRead(&myee.Add0, ADD0_EE_ADDRESS, 4);
		//如果读取到的地址不合法
		if (myee.Add0 > 247 || myee.Add0 == 0)
		{
			//给Add0设置一个合法的地址
			EEpromWrite(eepromdata.Add0, ADD0_EE_ADDRESS);
		}

		EEPROMRead(&myee.Cop0, COP0_EE_ADDRESS, 4);
		if (myee.Cop0 > 247 || myee.Add0 == 0)
		{
			EEpromWrite(eepromdata.Cop0, COP0_EE_ADDRESS);
		}

		EEPROMRead(&myee.bAu0, BAU0_EE_ADDRESS, 4);
		if (myee.bAu0 != 2400 && myee.bAu0 != 4800 && myee.bAu0 != 9600 && myee.bAu0 != 14400 && myee.bAu0 != 19200)
		{
			EEpromWrite(eepromdata.bAu0, BAU0_EE_ADDRESS);
		}

		EEPROMRead(&myee.CHd0, CHD0_EE_ADDRESS, 4);
		if (myee.CHd0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.CHd0, CHD0_EE_ADDRESS);
		}

		EEPROMRead(&myee.dAb0, DAB0_EE_ADDRESS, 4);
		if (myee.dAb0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.dAb0, DAB0_EE_ADDRESS);
		}
		EEPROMRead(&myee.Stb0, STB0_EE_ADDRESS, 4);
		if (myee.Stb0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.Stb0, STB0_EE_ADDRESS);
		}
		EEPROMRead(&myee.Add1, ADD1_EE_ADDRESS, 4);
		if (myee.Add1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.Add1, ADD1_EE_ADDRESS);
		}
		EEPROMRead(&myee.Cop1, COP1_EE_ADDRESS, 4);
		if (myee.Cop1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.Cop1, COP1_EE_ADDRESS);
		}
		EEPROMRead(&myee.bAu1, BAU1_EE_ADDRESS, 4);
		if (myee.bAu1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.bAu1, BAU1_EE_ADDRESS);
		}

		EEPROMRead(&myee.CHd1, CHD1_EE_ADDRESS, 4);
		if (myee.CHd1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.CHd1, CHD1_EE_ADDRESS);
		}
		EEPROMRead(&myee.dAb1, DAB1_EE_ADDRESS, 4);
		if (myee.dAb1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.dAb1, DAB1_EE_ADDRESS);
		}
		EEPROMRead(&myee.Stb1, STB1_EE_ADDRESS, 4);
		if (myee.Stb1 == 0xffffffff)
		{
			EEpromWrite(eepromdata.Stb1, STB1_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.Wt00, WT00_EE_ADDRESS, 4);
		if (myee.Wt00 == 0xffffffff)
		{
			EEpromWrite(eepromdata.Wt00, WT00_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.WtA0, WTA0_EE_ADDRESS, 4);
		if (myee.WtA0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.WtA0, WTA0_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.WF00, WF00_EE_ADDRESS, 4);
		if (myee.WF00 == 0xffffffff)
		{
			EEpromWrite(eepromdata.WF00, WF00_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.WFA0, WFA0_EE_ADDRESS, 4);
		if (myee.WFA0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.WFA0, WFA0_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.St00, ST00_EE_ADDRESS, 4);
		if (myee.St00 == 0xffffffff)
		{
			EEpromWrite(eepromdata.St00, ST00_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.StA0, STA0_EE_ADDRESS, 4);
		if (myee.StA0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.StA0, STA0_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.SF00, SF00_EE_ADDRESS, 4);
		if (myee.SF00 == 0xffffffff)
		{
			EEpromWrite(eepromdata.SF00, SF00_EE_ADDRESS);
		}
		EEPROMRead((uint32_t *)&myee.SFA0, SFA0_EE_ADDRESS, 4);
		if (myee.SFA0 == 0xffffffff)
		{
			EEpromWrite(eepromdata.SFA0, SFA0_EE_ADDRESS);
		}
		EEPROMRead(&myee.VER, VEP_EE_ADDRESS, 4);
		if (myee.VER == 0xffffffff)
		{
			EEpromWrite(eepromdata.VER, VEP_EE_ADDRESS);
		}
		EEPROMRead(&myee.CRC, CPC_EE_ADDRESS, 4);
		if (myee.CRC == 0xffffffff)
		{
			EEpromWrite(eepromdata.CRC, CPC_EE_ADDRESS);
		}
	}

}

void eepromdatainit(void)
{

	//给定装置初始化时候的默认值
	eepromdata.Add0 = 0x01;
	eepromdata.Cop0 = comprl_0;
	eepromdata.bAu0 = BaudR2;
	eepromdata.CHd0 = CheckD1;
	eepromdata.dAb0 = Datab4;
	eepromdata.Stb0 = StopB1;
	eepromdata.Add1 = 0x01;
	eepromdata.Cop1 = comprl_1;
	eepromdata.bAu1 = BaudR2;
	eepromdata.CHd1 = CheckD1;
	eepromdata.dAb1 = Datab4;
	eepromdata.Stb1 = StopB1;
	eepromdata.Wt00 = 0x1F4;		   //温度加热停止温度值		WtO0	   500
	eepromdata.WtA0 = 0x064;		   //温度加热开始温度值		WtA0	   100
	eepromdata.WF00 = 0x12C;		   //温度通风停止温度值		WFO0	   300
	eepromdata.WFA0 = 0x320;		   //温度通风开始温度值		WFA0	   800
	eepromdata.St00 = 0x12C;		   //湿度加热停止湿度值		StO0	   300
	eepromdata.StA0 = 0x2BC;		   //湿度加热开始湿度值		StA0	   700
	eepromdata.SF00 = 0x12C;		   //湿度通风停止湿度值		SFO0	   300
	eepromdata.SFA0 = 0x2BC;		   //湿度通风开始湿度值		SFA0	   700
	eepromdata.VER = 0x64;
	eepromdata.CRC = 0x11;
}



//定义一个写EEPROM的函数
void EEpromWrite(uint32_t pui32Data, uint32_t ui32Address)
{
	uint32_t temp[1];
	temp[0] = pui32Data;

	if (EEPROMStatusGet() == 0)
	{
		EEPROMProgram(temp, ui32Address, 4);		 //读取内存地址0x0开始 pui32Read长度的数值
	}

}

int Virtual_EEpromWrite_Check(uint8_t pui8Data[], uint32_t ui32Address, uint8_t num)
{

	int i;
	int16_t tempValue;
	int32_t tempAdd;
	for (i = 0; i < num; i++)
	{
		tempValue = pui8Data[i * 2 + 0] * 0x100 + pui8Data[i * 2 + 1];
		tempAdd = ui32Address + i;
		switch (tempAdd-0x5000)
		{

		case 0:
		case 1:
		case 2:
		case 3:
			if (tempValue<WT00_TEPM_L || tempValue>WT00_TEPM_H)
			{
				return i;
			}
			break;

		case 4:
		case 5:
		case 6:
		case 7:
			if (tempValue<ST00_PERCENT_L || tempValue>ST00_PERCENT_H)
			{
				return i;
			}
			break;

		default:
			return 100;
			//break;
		}
	}
//{
//	static uint32_t Virtual_ee[2][8];
//	uint8_t i, j;
//	for (i = 0; i < 2; i++)
//	{
//		for (j = 0; j < 8; j++)
//		{
//			Virtual_ee[i][j] = 0x00;
//		}
//	}
//
//	for (j = 0; j < num; j++)
//	{
//		Virtual_ee[0][j] = ui32Address;
//		ui32Address = 4 * (ui32Address - 0x4000 + 12);
//	}
//
//	for (j = 0; j < num; j++)
//	{
//		Virtual_ee[1][j] = pui32Data[j];
//	}
//	for (j = 0; j < num; j++)
//	{
//		switch (Virtual_ee[0][j])
//		{
//		case 4 * 12:
//			if ((pui32Data[0] < WT00_TEPM_H) && (pui32Data[0] > WT00_TEPM_L))
//			{
//				Virtual_ee[0] = pui32Data[0];
//			}
//			else
//			{
//				return(0);
//			}
//			break;
//
//		default:
//			break;
//		}
//
//	}
//
//
return 100;
}


