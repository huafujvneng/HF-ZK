#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "../inc/hw_gpio.h"
#include "../driverlib/rom.h"
#include "../driverlib/gpio.h"
#include "../inc/hw_memmap.h"
#include "../headfiles/led_display.h"
#include "../headfiles/modbus_rtu.h"
#include "../driverlib/uart.h"
#include "../utils/uartstdio.h"
#include "../driverlib/sysctl.h"
#include "../myinc/crc.h"
#include "../myinc/mystring.h"
#include "../headfiles/gui.h"
#include "../driverlib/eeprom.h"
#include "../headfiles/eeprominit.h"
#include "../headfiles/speaker.h"



extern tx_modbus mt;
int page, con123 = 0;
extern int8_t uart_flag;
volatile int led_light_timer;
int timer;
extern lcddisplay LcdState;
int page;
extern uint32_t test_btl[2];
extern 	uint32_t orgdata[2];
extern virtual_spk spk;
uint8_t temp;
uint8_t tempyuyinonce = 0;


virtualkey key;
mytime softclock;
virtualinput input[9], output[9];
virtualpassword password;
LL lcdcontrol;

/***********************定义动作语音输出数组***************************/
//请分断路器 Cbreakr_F
uint8_t Cbreakr_F[3] = { 音量10,连码播放,请分断路器 };
//请分接地刀 Gknife_F
uint8_t Gknife_F[3] = { 音量10,连码播放,请分接地刀 };
//请分接地刀 Gknife_F 请分断路器 Cbreakr_F
uint8_t CbreakrGknife_F[3] = { 音量10,连码播放,请分断路器_请分接地刀 };
////有人接近 Near_P
uint8_t Near_P1[3] = { 音量10,连码播放,本柜回路已带电 };
//正常带电提醒 Cbreakr_H
uint8_t Cbreakr_H[3] = { 音量10,连码播放,本柜回路已带电 };



//开始按键处理程序，包括亮灯，发送串口报文以及更新保温状态
void KeyHander(void)
{
	uint8_t ge, shi, bai, qian;
	if (key.keyvalid == SHORTPRESSEFFECTIVE)
	{
		key.keyvalid = NONPRESSEFFECTIVE;						//发生有效按键
		LcdState.pagetimer = Timer.soft_timer + 30000;
		Timer.infHold_timer = Timer.soft_timer + 60000;
		if (LcdState.page == page_password)
		{
			switch (key.effectivevalue)
			{
			case KEY_1:
				if (LcdState.jinzhidown == 10)
				{
					LcdState.light >>= 1;
					LcdState.light = (LcdState.light == 0x00) ? 0x08 : LcdState.light;
				}
				break;

			case KEY_2:
				if (LcdState.jinzhidown == 10)
				{
					ge = LcdState.datadown / 1 % 10;
					shi = LcdState.datadown / 10 % 10;
					bai = LcdState.datadown / 100 % 10;
					qian = LcdState.datadown / 1000 % 10;
					switch (LcdState.light)
					{
					case 0x01:
						ge--;
						ge = (ge > 9) ? 9 : ge;
						break;
					case 0x02:
						shi--;
						shi = (shi > 9) ? 9 : shi;
						break;
					case 0x04:
						bai--;
						bai = (bai > 9) ? 9 : bai;
						break;
					case 0x08:
						qian--;
						qian = (qian > 9) ? 9 : qian;
						break;
					default:
						break;
					}
					LcdState.datadown = qian * 1000 + bai * 100 + shi * 10 + ge;
				}
				break;
			case KEY_3:
				if (LcdState.jinzhidown == 10)
				{
					ge = LcdState.datadown / 1 % 10;
					shi = LcdState.datadown / 10 % 10;
					bai = LcdState.datadown / 100 % 10;
					qian = LcdState.datadown / 1000 % 10;
					switch (LcdState.light)
					{
					case 0x01:
						ge++;
						ge = (ge > 9) ? 0 : ge;
						break;
					case 0x02:
						shi++;
						shi = (shi > 9) ? 0 : shi;
						break;
					case 0x04:
						bai++;
						bai = (bai > 9) ? 0 : bai;
						break;
					case 0x08:
						qian++;
						qian = (qian > 9) ? 0 : qian;
						break;
					default:
						break;
					}
					LcdState.datadown = qian * 1000 + bai * 100 + shi * 10 + ge;
				}
				break;
			case KEY_4:
				if (LcdState.datadown == PASSWORD)
				{
					password.ifeffective = PASSWORDISEFFECTIVE;
					password.password_effective_timer = Timer.soft_timer + PASSWORDLIFETIME;
					LcdState.dataup = LcdTempState.dataup;
					LcdState.datadown = LcdTempState.datadown;
					LcdState.dotsiteup = LcdTempState.dotsiteup;
					LcdState.dotsitedown = LcdTempState.dotsitedown;
					LcdState.jinzhiup = LcdTempState.jinzhiup;
					LcdState.jinzhidown = LcdTempState.jinzhidown;
					LcdState.light = LcdTempState.light;
					LcdState.page = LcdTempState.page;
					key.keyvalid = SHORTPRESSEFFECTIVE;
					key.effectivevalue = KEY_4;
				}
				else
				{
					if (LcdState.jinzhidown == 10)
					{
						LcdState.datadown = ledsum($_NA, $_E, $_r, $_r, 32);
						LcdState.jinzhidown = 32;
						LcdState.light = 0x0f;
					}
					else
					{
						key.effectivevalue = 0;
						goverifypassword(0);
					}
				}
				break;
			default:
				LcdState.dataup = ledsum($_NA, $_p, $_W, $_d, 32);
				LcdState.datadown = 0;
				LcdState.dotsiteup = 4;
				LcdState.jinzhidown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x08;
				break;
			}
			return;
		}
		switch (key.effectivevalue)						//根据不同的返回键值，处理不同的操作
		{
		case KEY_1:
			LcdState.page++;
			LcdState.page %= 21;
			LcdState.page = (LcdState.page == 0) ? 1 : LcdState.page;
			switch (LcdState.page - 1)
			{

				//设置装置地址ADD
			case page_Add0:
				LcdState.dataup = ledsum($_A, $_d, $_d, $_0, 32);
				LcdState.datadown = myee.Add0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置规约类型Cop
			case page_Cop0:
				LcdState.dataup = ledsum($_C, $_o, $_p, $_0, 32);
				LcdState.datadown = myee.Cop0;  // “0”默认为Modbus  与上位机通讯  “1 ” 默认为Master  外接智能设备
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置波特率Bau0
			case page_bAu0:
				LcdState.dataup = ledsum($_b, $_A, $_u, $_0, 32);
				LcdState.datadown = (myee.bAu0 > 9600) ? myee.bAu0 /= 10 : myee.bAu0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置校验位CHd0
			case page_CHd0:
				LcdState.dataup = ledsum($_C, $_H, $_d, $_0, 32);
				if (myee.CHd0 == 0x00)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else if (myee.CHd0 == 0x06)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (myee.CHd0 == 0x02)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				else
				{

				}
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 32;
				LcdState.light = 0x0F;
				break;

				//设置数据位dAb1
			case page_dAb0:
				LcdState.dataup = ledsum($_d, $_A, $_b, $_0, 32);
				switch (myee.dAb0)
				{
				case Datab1:
					LcdState.datadown = DATAB1_DISP;
					break;
				case Datab2:
					LcdState.datadown = DATAB2_DISP;
					break;
				case Datab3:
					LcdState.datadown = DATAB3_DISP;
					break;
				case Datab4:
					LcdState.datadown = DATAB4_DISP;
					break;
				default:
					break;
				}
				//				LcdState.datadown = myee.dAb1;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置停止位Stb0
			case page_Stb0:
				LcdState.dataup = ledsum($_5, $_t, $_b, $_0, 32);
				if (myee.Stb0 == 0x00)
				{
					LcdState.datadown = 1;
				}
				else if (myee.Stb0 == 0x08)
				{
					LcdState.datadown = 2;
				}
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置Add1
			case page_Add1:
				LcdState.dataup = ledsum($_A, $_d, $_d, $_1, 32);
				LcdState.datadown = myee.Add1;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置Cop1
			case page_Cop1:
				LcdState.dataup = ledsum($_C, $_o, $_p, $_1, 32);
				LcdState.datadown = myee.Cop1;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置Bau1
			case page_bAu1:
				LcdState.dataup = ledsum($_b, $_A, $_u, $_1, 32);
				LcdState.datadown = (myee.bAu1 > 9600) ? myee.bAu1 /= 10 : myee.bAu1;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置CHd1
			case page_CHd1:
				LcdState.dataup = ledsum($_C, $_H, $_d, $_1, 32);
				if (myee.CHd1 == 0x00)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else if (myee.CHd1 == 0x06)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (myee.CHd1 == 0x02)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				else
				{
					LcdState.datadown = ledsum($_YI, $_YI, $_YI, $_YI, 32);   //----
				}
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 32;
				LcdState.light = 0x0F;
				break;

				//设置dAb1
			case page_dAb1:
				LcdState.dataup = ledsum($_d, $_A, $_b, $_1, 32);
				switch (myee.dAb1)
				{
				case Datab1:
					LcdState.datadown = DATAB1_DISP;
					break;
				case Datab2:
					LcdState.datadown = DATAB2_DISP;
					break;
				case Datab3:
					LcdState.datadown = DATAB3_DISP;
					break;
				case Datab4:
					LcdState.datadown = DATAB4_DISP;
					break;
				default:
					LcdState.datadown = DATAB4_DISP;
					break;
				}
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置湿度加热开始值Stb1
			case page_Stb1:
				LcdState.dataup = ledsum($_5, $_t, $_b, $_1, 32);
				if (myee.Stb1 == 0x00)
				{
					LcdState.datadown = 1;
				}
				else if (myee.Stb1 == 0x08)
				{
					LcdState.datadown = 2;
				}
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 4;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置通风加热停止值Wt00
			case page_Wt00:
				LcdState.dataup = ledsum($_W, $_t, $_0, $_0, 32);
				LcdState.datadown = myee.Wt00;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置通风加热开始值WtA0
			case page_WtA0:
				LcdState.dataup = ledsum($_W, $_t, $_A, $_0, 32);
				LcdState.datadown = myee.WtA0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;


				//设置通风加热停止值WF00
			case page_WF00:
				LcdState.dataup = ledsum($_W, $_F, $_0, $_0, 32);
				LcdState.datadown = myee.WF00;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置通风加热开始值WFA0
			case page_WFA0:
				LcdState.dataup = ledsum($_W, $_F, $_A, $_0, 32);
				LcdState.datadown = myee.WFA0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;


				//设置通风加热停止值SFA0
			case page_St00:
				LcdState.dataup = ledsum($_5, $_t, $_0, $_0, 32);
				LcdState.datadown = myee.St00;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置通风加热开始值SFA0
			case page_StA0:
				LcdState.dataup = ledsum($_5, $_t, $_A, $_0, 32);
				LcdState.datadown = myee.StA0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;


				//设置通风加热停止值SF00
			case page_SF00:
				LcdState.dataup = ledsum($_5, $_F, $_0, $_0, 32);
				LcdState.datadown = myee.SF00;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

				//设置通风加热开始值SFA0
			case page_SFA0:
				LcdState.dataup = ledsum($_5, $_F, $_A, $_0, 32);
				LcdState.datadown = myee.SFA0;
				LcdState.dotsiteup = 4;
				LcdState.dotsitedown = 2;
				LcdState.jinzhiup = 32;
				LcdState.jinzhidown = 10;
				LcdState.light = 0x0F;
				break;

			default:
				break;
			}
			//if (page % 2 == 1)
			//{
			//	LcdState.dataup += 0;
			//	LcdState.datadown = con123;
			//	LcdState.dotsite = 2;
			//	LcdState.jinzhiup = 10;
			//	LcdState.jinzhidown = 10;
			//	LcdState.light = 0x0F;
			//}
			//else
			//{
			//	LcdState.dataup = con123;
			//	LcdState.datadown += 0;
			//	LcdState.dotsite = 2;
			//	LcdState.jinzhiup = 10;
			//	LcdState.jinzhidown = 10;
			//	LcdState.light = 1;

			//	page = 0;
			//}
			//page++;
			break;

		case KEY_2:
			switch (LcdState.page - 1)
			{
			case page_Add0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < 1) ? 247 : LcdState.datadown;
				break;

			case page_Add1:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < 1) ? 247 : LcdState.datadown;
				break;

			case page_Cop0:
				LcdState.datadown ^= 1;
				LcdState.datadown = (LcdState.datadown > 1) ? 1 : LcdState.datadown;

				break;

			case page_Cop1:
				LcdState.datadown ^= 1;
				LcdState.datadown = (LcdState.datadown > 1) ? 1 : LcdState.datadown;

				break;

			case page_bAu0:
				if (LcdState.datadown == 1920)
				{
					LcdState.datadown = 1440;
				}
				else if (LcdState.datadown == 1440)
				{
					LcdState.datadown = 9600;
				}
				else if (LcdState.datadown == 9600)
				{
					LcdState.datadown = 4800;
				}
				else if (LcdState.datadown == 4800)
				{
					LcdState.datadown = 2400;
				}
				else if (LcdState.datadown == 2400)
				{
					LcdState.datadown = 1920;
				}

				break;

			case page_bAu1:
				if (LcdState.datadown == 1920)
				{
					LcdState.datadown = 1440;
				}
				else if (LcdState.datadown == 1440)
				{
					LcdState.datadown = 9600;
				}
				else if (LcdState.datadown == 9600)
				{
					LcdState.datadown = 4800;
				}
				else if (LcdState.datadown == 4800)
				{
					LcdState.datadown = 2400;
				}
				else if (LcdState.datadown == 2400)
				{
					LcdState.datadown = 1920;
				}
				break;
			case page_CHd0:
				if (LcdState.datadown == CHECKD3_DISP)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (LcdState.datadown == CHECKD2_DISP)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else if (LcdState.datadown == CHECKD1_DISP)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				break;
			case page_CHd1:
				if (LcdState.datadown == CHECKD3_DISP)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (LcdState.datadown == CHECKD2_DISP)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else if (LcdState.datadown == CHECKD1_DISP)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				break;
			case page_dAb0:
				switch (LcdState.datadown)
				{
				case DATAB4_DISP:
					LcdState.datadown = DATAB3_DISP;
					break;
				case DATAB3_DISP:
					LcdState.datadown = DATAB2_DISP;
					break;
				case DATAB2_DISP:
					LcdState.datadown = DATAB1_DISP;
					break;
				case DATAB1_DISP:
					LcdState.datadown = DATAB4_DISP;
					break;
				default:
					break;
				}
				break;
			case page_dAb1:
				switch (LcdState.datadown)
				{
				case DATAB4_DISP:
					LcdState.datadown = DATAB3_DISP;
					break;
				case DATAB3_DISP:
					LcdState.datadown = DATAB2_DISP;
					break;
				case DATAB2_DISP:
					LcdState.datadown = DATAB1_DISP;
					break;
				case DATAB1_DISP:
					LcdState.datadown = DATAB4_DISP;
					break;
				default:
					break;
				}
				break;
			case page_Stb0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < 1) ? 2 : LcdState.datadown;
				break;
			case page_Stb1:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < 1) ? 2 : LcdState.datadown;
				break;
			case page_Wt00:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;   //20号之前的程序是错误的-1000 之后的修改成1000
				break;
			case page_WtA0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_WF00:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_WFA0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_St00:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_StA0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_SF00:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_SFA0:
				LcdState.datadown--;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			default:
				//LcdState.datadown++;
				break;
			}

			break;

		case KEY_3:
			switch (LcdState.page - 1)
			{
			case page_Add0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 247) ? 1 : LcdState.datadown;
				break;

			case page_Add1:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 247) ? 1 : LcdState.datadown;
				break;

			case page_Cop0:

				LcdState.datadown ^= 1;
				LcdState.datadown = (LcdState.datadown > 1) ? 1 : LcdState.datadown;

				break;

			case page_Cop1:
				LcdState.datadown ^= 1;
				LcdState.datadown = (LcdState.datadown > 1) ? 1 : LcdState.datadown;

				break;

			case page_bAu0:
				if (LcdState.datadown == 2400)
				{
					LcdState.datadown = 4800;
				}
				else if (LcdState.datadown == 4800)
				{
					LcdState.datadown = 9600;
				}
				else if (LcdState.datadown == 9600)
				{
					LcdState.datadown = 1440;
				}
				else if (LcdState.datadown == 1440)
				{
					LcdState.datadown = 1920;
				}
				else if (LcdState.datadown == 1920)
				{
					LcdState.datadown = 2400;
				}

				break;

			case page_bAu1:
				if (LcdState.datadown == 2400)
				{
					LcdState.datadown = 4800;
				}
				else if (LcdState.datadown == 4800)
				{
					LcdState.datadown = 9600;
				}
				else if (LcdState.datadown == 9600)
				{
					LcdState.datadown = 1440;
				}
				else if (LcdState.datadown == 1440)
				{
					LcdState.datadown = 1920;
				}
				else if (LcdState.datadown == 1920)
				{
					LcdState.datadown = 2400;
				}
				break;
			case page_CHd0:
				if (LcdState.datadown == CHECKD1_DISP)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (LcdState.datadown == CHECKD2_DISP)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				else if (LcdState.datadown == CHECKD3_DISP)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				break;
			case page_CHd1:
				if (LcdState.datadown == CHECKD1_DISP)
				{
					LcdState.datadown = CHECKD2_DISP;
				}
				else if (LcdState.datadown == CHECKD2_DISP)
				{
					LcdState.datadown = CHECKD3_DISP;
				}
				else if (LcdState.datadown == CHECKD3_DISP)
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				else
				{
					LcdState.datadown = CHECKD1_DISP;
				}
				break;
			case page_dAb0:
				switch (LcdState.datadown)
				{
				case DATAB1_DISP:
					LcdState.datadown = DATAB2_DISP;
					break;
				case DATAB2_DISP:
					LcdState.datadown = DATAB3_DISP;
					break;
				case DATAB3_DISP:
					LcdState.datadown = DATAB4_DISP;
					break;
				case DATAB4_DISP:
					LcdState.datadown = DATAB1_DISP;
					break;
				default:
					break;
				}
				break;
			case page_dAb1:
				switch (LcdState.datadown)
				{
				case DATAB1_DISP:
					LcdState.datadown = DATAB2_DISP;
					break;
				case DATAB2_DISP:
					LcdState.datadown = DATAB3_DISP;
					break;
				case DATAB3_DISP:
					LcdState.datadown = DATAB4_DISP;
					break;
				case DATAB4_DISP:
					LcdState.datadown = DATAB1_DISP;
					break;
				default:
					break;
				}
				break;
			case page_Stb0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 2) ? 1 : LcdState.datadown;

				break;
			case page_Stb1:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 2) ? 1 : LcdState.datadown;

				break;
			case page_Wt00:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_WtA0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_WF00:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_WFA0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_St00:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_StA0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_SF00:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			case page_SFA0:
				LcdState.datadown++;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;
			default:
				//LcdState.datadown++;
				break;
			}
			break;
		case KEY_4:
			if (LcdState.page > 0)					//如果不是默认页
			{
				if (password.ifeffective == PASSWORDNOEFFECTIVE)
				{
					key.effectivevalue = 0;
					LcdTempState.dataup = LcdState.dataup;
					LcdTempState.datadown = LcdState.datadown;
					LcdTempState.dotsiteup = LcdState.dotsiteup;
					LcdTempState.dotsitedown = LcdState.dotsitedown;
					LcdTempState.jinzhiup = LcdState.jinzhiup;
					LcdTempState.jinzhidown = LcdState.jinzhidown;
					LcdTempState.light = LcdState.light;
					LcdTempState.page = LcdState.page;
					goverifypassword(0);
					return;
				}
				switch (LcdState.page - 1)
				{
				case page_Add0:
					EEpromWrite(LcdState.datadown, ADD0_EE_ADDRESS);
					break;
				case page_Add1:
					EEpromWrite(LcdState.datadown, ADD1_EE_ADDRESS);
					break;
				case page_Cop0:
					EEpromWrite(LcdState.datadown, COP0_EE_ADDRESS);
					break;
				case page_Cop1:
					EEpromWrite(LcdState.datadown, COP1_EE_ADDRESS);
					break;
				case page_bAu0:
					if (LcdState.datadown == 2400)
					{
						EEpromWrite(LcdState.datadown, BAU0_EE_ADDRESS);
					}
					if (LcdState.datadown == 4800)
					{
						EEpromWrite(LcdState.datadown, BAU0_EE_ADDRESS);
					}
					if (LcdState.datadown == 9600)
					{
						EEpromWrite(LcdState.datadown, BAU0_EE_ADDRESS);
					}
					if (LcdState.datadown == 1440)
					{
						EEpromWrite(LcdState.datadown * 10, BAU0_EE_ADDRESS);
					}
					if (LcdState.datadown == 1920)
					{
						EEpromWrite(LcdState.datadown * 10, BAU0_EE_ADDRESS);
					}
					break;
				case page_bAu1:
					if (LcdState.datadown == 2400)
					{
						EEpromWrite(LcdState.datadown, BAU1_EE_ADDRESS);
					}
					if (LcdState.datadown == 4800)
					{
						EEpromWrite(LcdState.datadown, BAU1_EE_ADDRESS);
					}
					if (LcdState.datadown == 9600)
					{
						EEpromWrite(LcdState.datadown, BAU1_EE_ADDRESS);
					}
					if (LcdState.datadown == 1440)
					{
						EEpromWrite(LcdState.datadown * 10, BAU1_EE_ADDRESS);
					}
					if (LcdState.datadown == 1920)
					{
						EEpromWrite(LcdState.datadown * 10, BAU1_EE_ADDRESS);
					}
					break;
				case page_CHd0:
					if (LcdState.datadown == CHECKD1_DISP)
					{
						EEpromWrite(CheckD1, CHD0_EE_ADDRESS);
					}
					if (LcdState.datadown == CHECKD2_DISP)
					{
						EEpromWrite(CheckD2, CHD0_EE_ADDRESS);
					}
					if (LcdState.datadown == CHECKD3_DISP)
					{
						EEpromWrite(CheckD3, CHD0_EE_ADDRESS);
					}
					break;
				case page_CHd1:
					if (LcdState.datadown == CHECKD1_DISP)
					{
						EEpromWrite(CheckD1, CHD1_EE_ADDRESS);
					}
					if (LcdState.datadown == CHECKD2_DISP)
					{
						EEpromWrite(CheckD2, CHD1_EE_ADDRESS);
					}
					if (LcdState.datadown == CHECKD3_DISP)
					{
						EEpromWrite(CheckD3, CHD1_EE_ADDRESS);
					}
					break;
				case page_dAb0:
					switch (LcdState.datadown)
					{
					case DATAB1_DISP:
						EEpromWrite(Datab1, DAB0_EE_ADDRESS);
						break;
					case DATAB2_DISP:
						EEpromWrite(Datab2, DAB0_EE_ADDRESS);
						break;
					case DATAB3_DISP:
						EEpromWrite(Datab3, DAB0_EE_ADDRESS);
						break;
					case DATAB4_DISP:
						EEpromWrite(Datab4, DAB0_EE_ADDRESS);
						break;
					default:
						break;
					}
					break;
				case page_dAb1:
					switch (LcdState.datadown)
					{
					case DATAB1_DISP:
						EEpromWrite(Datab1, DAB1_EE_ADDRESS);
						break;
					case DATAB2_DISP:
						EEpromWrite(Datab2, DAB1_EE_ADDRESS);
						break;
					case DATAB3_DISP:
						EEpromWrite(Datab3, DAB1_EE_ADDRESS);
						break;
					case DATAB4_DISP:
						EEpromWrite(Datab4, DAB1_EE_ADDRESS);
						break;
					default:
						break;
					}
					break;
				case page_Stb0:
					if (LcdState.datadown == 1)
					{
						EEpromWrite(0x00, STB0_EE_ADDRESS);
					}
					else if (LcdState.datadown == 2)
					{
						EEpromWrite(0x08, STB0_EE_ADDRESS);
					}
					break;
				case page_Stb1:
					if (LcdState.datadown == 1)
					{
						EEpromWrite(0x00, STB1_EE_ADDRESS);
					}
					else if (LcdState.datadown == 2)
					{
						EEpromWrite(0x08, STB1_EE_ADDRESS);
					}
					break;
				case page_Wt00:
					EEpromWrite(LcdState.datadown, WT00_EE_ADDRESS);
					break;
				case page_WtA0:
					EEpromWrite(LcdState.datadown, WTA0_EE_ADDRESS);
					break;
				case page_WF00:
					EEpromWrite(LcdState.datadown, WF00_EE_ADDRESS);
					break;
				case page_WFA0:
					EEpromWrite(LcdState.datadown, WFA0_EE_ADDRESS);
					break;
				case page_St00:
					EEpromWrite(LcdState.datadown, ST00_EE_ADDRESS);
					break;
				case page_StA0:
					EEpromWrite(LcdState.datadown, STA0_EE_ADDRESS);
					break;
				case page_SF00:
					EEpromWrite(LcdState.datadown, SF00_EE_ADDRESS);
					break;
				case page_SFA0:
					EEpromWrite(LcdState.datadown, SFA0_EE_ADDRESS);
					break;
				default:
					break;
				}
				LcdState.oldlightstate = 0x0f;
				LcdState.newlightstate = 0xf0;
				LcdState.light = LcdState.newlightstate;
				Timer.restoreLedOrgState_timer = Timer.soft_timer + 2000;
				myEEpromInit();							//重新映射最新改写后的eeprom
				UartInit();								//重新配置串口
			}
			break;
		default:
			break;
		}
		//uart_flag = HAVESEND;
	}
	if (key.keyvalid == LONGPRESSEFFECTIVE)
	{
		LcdState.pagetimer = Timer.soft_timer + 30000;

		switch (key.effectivevalue)
		{
		case KEY_1:
			LcdState.pagetimer = Timer.soft_timer;
			break;

		case KEY_2:
			switch (LcdState.page - 1)
			{
			case page_Add0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < 1) ? 247 : LcdState.datadown;
				break;
			case page_Add1:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < 1) ? 247 : LcdState.datadown;
				break;

			case page_Wt00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_WtA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_WF00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_WFA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_St00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_StA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_SF00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;
			case page_SFA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown--;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown < -400) ? 1000 : LcdState.datadown;
				break;


			default:
				break;
			}
			break;
		case KEY_3:
			switch (LcdState.page - 1)
			{
			case page_Add0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 247) ? 1 : LcdState.datadown;
				break;

			case page_Add1:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 247) ? 1 : LcdState.datadown;
				break;

			case page_Wt00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_WtA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_WF00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_WFA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_St00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_StA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_SF00:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			case page_SFA0:
				if (Timer.soft_timer - Timer.longPushAux_timer > 50)
				{
					Timer.longPushAux_timer = Timer.soft_timer;
					LcdState.datadown++;
				}
				LcdState.light = 0;
				LcdState.datadown = (LcdState.datadown > 1000) ? -400 : LcdState.datadown;
				break;

			default:
				break;
			}
			break;
		case KEY_4:
			break;

		default:
			break;
		}
	}

	if (key.nolongpressonce == 1)
	{
		key.nolongpressonce = 0;
		if ((key.effectivevalue == KEY_2) || (key.effectivevalue == KEY_3))
		{
			LcdState.light = 0x0F;
		}
	}
}
//END开始按键处理程序，包括亮灯，发送串口报文以及更新保温状态


//开始按键的消抖处理程序，更新了两个结构体全局变量，其中key.keyvalid=代表发生有效按键动作，
//key.effectivevalue=传送按下的具体按键
void key_rjg(void)
{
	//如果当前无有效按键或者是长按有效状态
	if (key.keyvalid == NONPRESSEFFECTIVE || key.keyvalid == LONGPRESSEFFECTIVE)
	{
		key.keycurvalue = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
		//如果当前键盘状态发生改变
		if ((key.keycurvalue != key.keyhisvalue))
		{
			key.keychangtime = (Timer.soft_timer - key.keytimer);
			key.keytimer = Timer.soft_timer;
			key.effectivevalue = key.keyhisvalue;
			key.keyhisvalue = key.keycurvalue;
		}
		//如果所有按键已经弹起状态
		if (key.keycurvalue == ALLKEYRAISE)
		{
			key.nolongpressoncehelp <<= 1;
			//如果按下的时间大于40ms且小于500ms判定为短按有效
			if ((key.keychangtime > 40) && (key.keychangtime < 500))
			{

				//屏蔽系统刚带电时产生的第一个无效的短按信号
				if (key.effectivevalue != 0x00)
				{
					key.keyvalid = SHORTPRESSEFFECTIVE;
				}
				key.keychangtime = 0;
			}
			//如果是长按有效则终止长按有效状态
			if (key.keyvalid == LONGPRESSEFFECTIVE)
			{
				key.nolongpressoncehelp += 1;
				key.keyvalid = NONPRESSEFFECTIVE;
				if ((key.nolongpressoncehelp & 0x03) == 0x01)
				{
					key.nolongpressonce = 1;
				}
			}
		}
		//如果是有按键按下
		else
		{
			//如果按下时间超过1000ms
			if (Timer.soft_timer - key.keytimer > 1000)
			{
				key.keyvalid = LONGPRESSEFFECTIVE;
				key.effectivevalue = key.keycurvalue;
			}
		}
	}
}
//END开始按键的消抖处理程序，更新了两个结构体全局变量，其中key.keyavlid=代表发生有效按键动作，
//key.effectivevalue=传送按下的具体按键

void Led_light(void)
{
	if (led_light_timer == 0)
	{
		led_light_timer = Timer.soft_timer;
	}
	timer = Timer.soft_timer - led_light_timer;
	if (timer / 500 == 0)
	{
		lcdcontrol.LCD_ON_OFF = OFF;
		//ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 1 << 5);
		ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_AHB_BASE, 0x00);
	}
	else
	{
		lcdcontrol.LCD_ON_OFF = ON;
		ROM_GPIOPinTypeGPIOOutput(GPIO_PORTH_AHB_BASE, 0xff);
		//ROM_GPIOPinWrite(GPIO_PORTE_AHB_BASE, GPIO_PIN_5, 0 << 5);
	}
	led_light_timer = (timer > 1000) ? led_light_timer + 1000 : led_light_timer;
}


void input_rjg(void)
{
	uint8_t i;
	for (i = 0; i < 9; i++)
	{
		input[i].temp_value <<= 1;
	}
	input[0].temp_value += (((GPIOPinRead(GPIO_PORTJ_AHB_BASE, GPIO_PIN_2) & 0x04) >> 2) & 0x01);  //刀闸合位位置
	input[1].temp_value += (((GPIOPinRead(GPIO_PORTB_AHB_BASE, GPIO_PIN_5) & 0x20) >> 5) & 0x01);  //刀闸分位位置
	input[2].temp_value += (((GPIOPinRead(GPIO_PORTB_AHB_BASE, GPIO_PIN_4) & 0x10) >> 4) & 0x01);  //断路合位位置
	input[3].temp_value += (((GPIOPinRead(GPIO_PORTC_AHB_BASE, GPIO_PIN_4) & 0x10) >> 4) & 0x01);  //断路分位位置
	input[4].temp_value += (((GPIOPinRead(GPIO_PORTC_AHB_BASE, GPIO_PIN_5) & 0x20) >> 5) & 0x01);  //接地刀分位置
	input[5].temp_value += (((GPIOPinRead(GPIO_PORTC_AHB_BASE, GPIO_PIN_6) & 0x40) >> 6) & 0x01);  //远方就地开入
	input[6].temp_value += (((GPIOPinRead(GPIO_PORTJ_AHB_BASE, GPIO_PIN_0) & 0x01) >> 0) & 0x01);  //备用开入1
	input[7].temp_value += (((GPIOPinRead(GPIO_PORTJ_AHB_BASE, GPIO_PIN_1) & 0x02) >> 1) & 0x01);  //备用开入2
	input[8].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_1) & 0x02) >> 1) & 0x01);  //闭锁开入
	//input[9].temp_value += (((GPIOPinRead(GPIO_PORTF_AHB_BASE, GPIO_PIN_7) & 0x80) >> 7) & 0x01);  //储能开入
	for (i = 0; i < 9; i++)
	{
		if ((input[i].temp_value & 0x03) == 0x00 || (input[i].temp_value & 0x03) == 0x03)
		{
			if (input[i].nonjitter_times < 20)
			{
				input[i].nonjitter_times++;
			}
		}
		else
		{
			if ((input[i].temp_value & 0x06) == 0x00 || (input[i].temp_value & 0x06) == 0x06)
			{
				input[i].nonjitter_times = 5;
				if ((input[i].temp_value & 0x01) == 0x00)
				{
					input[i].start_time_p = Timer.soft_timer;
				}
				else
				{
					input[i].start_time_n = Timer.soft_timer;
				}
			}
			if (input[i].nonjitter_times > 0)
			{
				input[i].nonjitter_times--;
			}
		}
		if (input[i].nonjitter_times == 20)
		{
			if (i != 4)
			{
				input[i].value = ~(input[i].temp_value) & 0x01;
			}
			else
			{
				input[i].value = (input[i].temp_value) & 0x01;
			}
		}
		if (input[i].nonjitter_times == 0)
		{
			//input[i].value = 0xAA;
			input[i].nonjitter_times = 5;
		}
	}
}

//根据开入的值控制指示标志的亮灭
void inputcontroloutput(void)
{
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_0, input[0].value << 0);
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_1, input[1].value << 1);
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_2, input[2].value << 2);
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_3, input[3].value << 3);
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_4, input[4].value << 4);
	//ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_5, input[5].value << 5);

	//ledcontrol(1, 1, 1000);
	//ledcontrol(2, 1, 1000);
	//ledcontrol(3, 1, 1000);
	//ledcontrol(4, 1, 1000);
	//ledcontrol(5, 1, 1000);
	//ledcontrol(6, 1, 1000);
	if ((fspk[0].flagspk != 255) && (fspk[1].flagspk != 255) && (fspk[2].flagspk != 255))
	{
		if (input[0].value == 1&&input[1].value==0)
		{
			ledcontrol(1, 0, 0);
			ledcontrol(2, 1, 0);
		}
		else if(input[0].value==0&&input[1].value==1)
		{
			ledcontrol(1, 1, 0);
			ledcontrol(2, 0, 0);
		}
		else
		{
			ledcontrol(1, 0, 0);
			ledcontrol(2, 0, 0);
		}
	}
	if (input[2].value==0&&input[3].value == 1)
	{
		ledcontrol(3, 0, 0);
		ledcontrol(4, 1, 0);
	}
	else if (input[2].value == 1 && input[3].value == 0)
	{
		ledcontrol(3, 1, 0);
		ledcontrol(4, 0, 0);
	}
	else
	{
		ledcontrol(3, 0, 0);
		ledcontrol(4, 0, 0);
	}
	if ((fspk[1].flagspk != 255) && (fspk[2].flagspk != 255))
	{
		if (input[4].value == 1)
		{
			ledcontrol(5, 0, 0);
		}
		else
		{
			ledcontrol(5, 1, 0);
		}
	}
}


void ledcontrol(int ledIndex, uint8_t ledState, uint32_t ledPeriod)
{
	switch (ledIndex)
	{
	case 1:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_0, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 0);
		break;
	case 2:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_1, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 1);
		break;
	case 3:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_2, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 2);
		break;
	case 4:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_3, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 3);
		break;
	case 5:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_4, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 4);  //判断接地刀合位位置--wushuisheng add 2018年4月20日 6:56:15
		break;
	case 6:
		ROM_GPIOPinWrite(GPIO_PORTG_AHB_BASE, GPIO_PIN_5, ~(ledState*(ledPeriod == 0 || ((Timer.soft_timer%ledPeriod) > (ledPeriod / 2)) ? 1 : 0)) << 5);
		break;
	default:
		break;
	}
}

void inputsperker(void)
{
	fspk[0].flagspk <<= 1;
	fspk[1].flagspk <<= 1;
	fspk[2].flagspk <<= 1;
	tempyuyinonce <<= 1;
	//本柜回路已经带电
	if ((input[2].value == 0x01) && (input[1].value == 0x00) && (input[4].value == 0x01))
	{
		if (input[2].start_time_p > input[1].start_time_n)
		{
			fspk[0].flagspk += 1;
			if ((fspk[0].flagspk & 0x03) == 0x01)
			{
				speak_lock(Near_P1);//--wushuisheng add 2018年4月18日 13:01:50
				spk.daidianflag = 1;
			}
		}
	}

	//if ((input[2].value == 0x00) && (input[1].value == 0x01) && (input[4].value == 0x01))

	//请分断路器
	if ((input[2].value == 0x01) && (input[0].value == 0x01) && (input[4].value == 0x01))
	{
		if (input[0].start_time_p > input[2].start_time_p)
			//if (input[1].start_time_n - input[2].start_time_p > 10)
		{
			fspk[0].flagspk += 1;
			if ((fspk[0].flagspk & 0x03) == 0x01)
			{
				speak_lock(Cbreakr_F);
				spk.daidianflag = 0;

			}
			ledcontrol(1, 1, 1000);
			ledcontrol(2, 1, 1000);
			ledcontrol(4, 1, 1000);
		}
		else if (input[0].start_time_p == 0 && input[2].start_time_p == 0)
		{
			tempyuyinonce += 1;
			//speak_lock(Near_P1);//--wushuisheng add 2018年4月18日 13:01:50
			if (tempyuyinonce < 0x7f)
			{

				spk.daidianflag = 1;
				Timer.daidianyuyin_timer = Timer.soft_timer + 2000;
				spk.daidianyuyinflag = 1;
			}
		}
	}
	if ((input[2].value == 0x00))
	{
		spk.daidianflag = 0;
	}

	//请分接地刀
	if ((input[4].value == 0x00) && (input[1].value == 0x00) && (input[2].value == 0x00))
	{
		if (input[4].start_time_n < input[1].start_time_n)
		{
			fspk[1].flagspk += 1;
			if ((fspk[1].flagspk & 0x03) == 0x01)
			{
				speak_lock(Gknife_F);
				spk.daidianflag = 0;
			}
			ledcontrol(1, 1, 1000);
			ledcontrol(2, 1, 1000);
			ledcontrol(5, 1, 1000);


		}
	}
	//请分断路器，请分接地刀
	if ((input[4].value == 0x00) && (input[1].value == 0x00) && (input[2].value == 0x01))
	{
		if (input[4].start_time_n < input[1].start_time_n)
		{
			if (input[2].start_time_p < input[1].start_time_n)
			{
				fspk[2].flagspk += 1;
				if ((fspk[2].flagspk & 0x03) == 0x01)
				{
					speak_lock(CbreakrGknife_F);
					spk.daidianflag = 0;
				}
				ledcontrol(1, 1, 1000);
				ledcontrol(2, 1, 1000);
				ledcontrol(4, 1, 1000);
				ledcontrol(5, 1, 1000);

			}
		}
	}

}

void diffseconds_to_clock(void)
{
	uint8_t temp4years, months, hours, minutes, chayears;
	uint16_t days;
	uint32_t sseconds;
	temp4years = softclock.diffseconds / 60 / 60 / 24 / (365 * 4 + 1);
	sseconds = softclock.diffseconds - (temp4years*(365 * 4 + 1) * 24 * 60 * 60);
	chayears = sseconds / 60 / 60 / 24 / 365;
	sseconds -= (chayears * 60 * 60 * 24 * 365);
	days = sseconds / 60 / 60 / 24;
	sseconds -= days * 60 * 60 * 24;
	hours = sseconds / 60 / 60;
	sseconds -= hours * 60 * 60;
	minutes = sseconds / 60;
	sseconds -= minutes * 60;
	if (chayears == 0)
	{
		if (days < 31)
		{
			months = 1;
		}
		else if (days < 60)
		{
			months = 2;
			days -= 31;
		}
		else if (days < 91)
		{
			months = 3;
			days -= 60;
		}
		else if (days < 121)
		{
			months = 4;
			days -= 91;
		}
		else if (days < 152)
		{
			months = 5;
			days -= 121;
		}
		else if (days < 182)
		{
			months = 6;
			days -= 152;
		}
		else if (days < 213)
		{
			months = 7;
			days -= 182;
		}
		else if (days < 244)
		{
			months = 8;
			days -= 213;
		}
		else if (days < 274)
		{
			months = 9;
			days -= 244;
		}
		else if (days < 305)
		{
			months = 10;
			days -= 274;
		}
		else if (days < 335)
		{
			months = 11;
			days -= 305;
		}
		else if (days < 366)
		{
			months = 12;
			days -= 335;
		}
		else
		{

		}
	}
	else
	{
		if (days < 31)
		{
			months = 1;
		}
		else if (days < 59)
		{
			months = 2;
			days -= 31;
		}
		else if (days < 90)
		{
			months = 3;
			days -= 59;
		}
		else if (days < 120)
		{
			months = 4;
			days -= 90;
		}
		else if (days < 151)
		{
			months = 5;
			days -= 120;
		}
		else if (days < 181)
		{
			months = 6;
			days -= 151;
		}
		else if (days < 212)
		{
			months = 7;
			days -= 181;
		}
		else if (days < 243)
		{
			months = 8;
			days -= 212;
		}
		else if (days < 273)
		{
			months = 9;
			days -= 243;
		}
		else if (days < 304)
		{
			months = 10;
			days -= 273;
		}
		else if (days < 334)
		{
			months = 11;
			days -= 304;
		}
		else if (days < 365)
		{
			months = 12;
			days -= 334;
		}
		else
		{

		}

	}
	softclock.year = 4 * temp4years + chayears + 2000;
	softclock.month = months;
	softclock.day = days + 1;
	softclock.hour = hours;
	softclock.minute = minutes;
	softclock.second = sseconds;
}

void goverifypassword(uint8_t org)
{
	key.keyvalid = SHORTPRESSEFFECTIVE;
	LcdState.page = page_password;
}

uint32_t ledsum(uint8_t q, uint8_t b, uint8_t s, uint8_t g, uint8_t e)
{
	return e * e*e*q + e * e*b + e * s + g;
}


