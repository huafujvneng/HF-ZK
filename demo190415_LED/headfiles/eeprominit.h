#ifndef __APPLICATION_EEPROMINIT_H__
#define __APPLICATION_EEPROMINIT_H__

#include <stdint.h>
#include <stdbool.h>
void myEEpromInit(void);
void EEpromWrite(uint32_t pui32Data, uint32_t ui32Address);
void eepromdatainit(void);
int Virtual_EEpromWrite_Check(uint8_t pui32Data[], uint32_t ui32Address, uint8_t num);
/***********************定值宏定义***************************/
#define comprl_0       0       //Modbus
#define comprl_1       1       //Master
/********波特率设置************/
#define BaudR0      2400     //2400
#define BaudR1      4800     //4800
#define BaudR2      9600     //9600
#define BaudR3      14400    //14400
#define BaudR4      19200    //19200
/********校验位设置************/
//#define CheckD0		0x86	 //MASK
#define CheckD1		0x00	 //NONE
#define CHECKD1_DISP  21*32*32*32+28*32*32+21*32+14  // 显示字符“nonE”
#define CheckD2		0x06	 //EVEN
#define CHECKD2_DISP  14*32*32*32+20*32*32+14*32+21 // 显示字符“EVEN”
#define CheckD3		0x02	 //ODD
#define CHECKD3_DISP 23*32*32*32+28*32*32+13*32+13  // 显示字符“_odd”

//#define CheckD4		0x82	 //ONE
//#define CheckD5		0x86	 //ZERO
/********数据位设置************/
#define Datab1  0x00         //5
#define DATAB1_DISP    5
#define Datab2  0x20         //6
#define DATAB2_DISP    6
#define Datab3  0x40         //7
#define DATAB3_DISP    7
#define Datab4  0x60         //8
#define DATAB4_DISP    8
//#define Datab0  0x60         //MASK
/********停止位设置************/
//#define StopB0  0x08         //MASK
#define StopB1  0x00         //1
#define StopB2  0x08         //2

/********EE地址定义************/
#define ADD0_EE_ADDRESS			4*0     
#define COP0_EE_ADDRESS			4*1    
#define BAU0_EE_ADDRESS			4*2   
#define CHD0_EE_ADDRESS			4*3   
#define DAB0_EE_ADDRESS			4*4 
#define STB0_EE_ADDRESS			4*5

#define ADD1_EE_ADDRESS			4*6   
#define COP1_EE_ADDRESS			4*7  
#define BAU1_EE_ADDRESS			4*8 
#define CHD1_EE_ADDRESS			4*9
#define DAB1_EE_ADDRESS			4*10
#define STB1_EE_ADDRESS			4*11

#define WT00_EE_ADDRESS			4*12
#define WTA0_EE_ADDRESS			4*13
#define WF00_EE_ADDRESS			4*14
#define WFA0_EE_ADDRESS			4*15
#define ST00_EE_ADDRESS			4*16
#define STA0_EE_ADDRESS			4*17
#define SF00_EE_ADDRESS			4*18
#define SFA0_EE_ADDRESS			4*19

#define VEP_EE_ADDRESS			4*20
#define CPC_EE_ADDRESS			4*21

#define WT00_TEPM_L -400
#define WT00_TEPM_H 1000
#define ST00_PERCENT_L 0
#define ST00_PERCENT_H 1000








/**************装置定值设置结构体******************/
typedef struct
{
	uint32_t Add0;				//装置地址					Add0		存储地址0x00		
	uint32_t Cop0;				//装置规约					Cop0		存储地址0x04		
	uint32_t bAu0;				//通讯波特率				bAu0		存储地址0x08		
	uint32_t CHd0;				//校验位					CHd0		存储地址0x0c		
	uint32_t dAb0;				//数据位					dAb0		存储地址0x10		
	uint32_t Stb0;				//停止位					Stb0		存储地址0x14

	uint32_t Add1;				//装置地址					Add1		存储地址0x40		
	uint32_t Cop1;				//装置规约					Cop1		存储地址0x44	
	uint32_t bAu1;				//通讯波特率				Bau1		存储地址0x48	
	uint32_t CHd1;				//校验位					CHd1		存储地址0x4c	
	uint32_t dAb1;				//数据位					dAb1		存储地址0x50		
	uint32_t Stb1;				//停止位					Stb1		存储地址0x54

	int Wt00;				//温度加热停止温度值		WtO0		存储地址0x18		
	int WtA0;				//温度加热开始温度值		WtA0		存储地址0x1c		
	int WF00;				//温度通风停止温度值		WFO0		存储地址0x20			  
	int WFA0;				//温度通风开始温度值		WFA0		存储地址0x24			  
	int St00;				//湿度加热停止湿度值		StO0		存储地址0x28			  
	int StA0;				//湿度加热开始湿度值		StA0		存储地址0x2c			  
	int SF00;				//湿度通风停止湿度值		SFO0		存储地址0x30			  
	int SFA0;				//湿度通风开始湿度值		SFA0		存储地址0x34

	uint32_t VER;				//程序版本号		        VEp			存储地址0x38
	uint32_t CRC;				//程序校验码		        CpC			存储地址0x3c


}eeprominit;

extern eeprominit myee;

//int regionsdown[20] = { 1,0,0,0,5,1,1,0,0,0,5,1,-40,-40,-40,-40,20,20,20,20, };
//int regionsup[20] = { 247,1,0,1,8,2, 247,1,0,1,8,2,100,100,100,100,90,90,90,90, };

typedef struct
{
	volatile long soft_timer;							//系统软件时间
	volatile long ledRefresh_timer;						//用于数码管的刷新时间控制
	volatile long infHold_timer;						//用于红外探测的保持时间
	volatile long longPushAux_timer;					//用于控制长按的数值变化速度的辅助
	volatile long restoreLedOrgState_timer;				//用于保存后数码管闪烁并回复其原来状态的计时
	volatile long CommunicationAble0_timer;				//用于判定温湿度传感器的通讯是否正常
	volatile long CommunicationAble1_timer;				//用于判定温湿度传感器的通讯是否正常
	volatile long CommunicationAble0_timer_11s;				//用于判定温湿度传感器的通讯是正常后11s
	volatile long CommunicationAble1_timer_11s;				//用于判定温湿度传感器的通讯是正常后11s
	volatile long CommunicationToUp_timer;				//用于判定当前装置对上通讯状态
	volatile long daidianyuyin_timer;				//

}structTimer;
extern structTimer Timer;


#endif  // __APPLICATION_EEPROMINIT_H__
