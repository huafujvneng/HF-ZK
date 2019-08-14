#ifndef __APPLICATION_EEPROMINIT_H__
#define __APPLICATION_EEPROMINIT_H__

#include <stdint.h>
#include <stdbool.h>
void myEEpromInit(void);
void EEpromWrite(uint32_t pui32Data, uint32_t ui32Address);
void eepromdatainit(void);
int Virtual_EEpromWrite_Check(uint8_t pui32Data[], uint32_t ui32Address, uint8_t num);
/***********************��ֵ�궨��***************************/
#define comprl_0       0       //Modbus
#define comprl_1       1       //Master
/********����������************/
#define BaudR0      2400     //2400
#define BaudR1      4800     //4800
#define BaudR2      9600     //9600
#define BaudR3      14400    //14400
#define BaudR4      19200    //19200
/********У��λ����************/
//#define CheckD0		0x86	 //MASK
#define CheckD1		0x00	 //NONE
#define CHECKD1_DISP  21*32*32*32+28*32*32+21*32+14  // ��ʾ�ַ���nonE��
#define CheckD2		0x06	 //EVEN
#define CHECKD2_DISP  14*32*32*32+20*32*32+14*32+21 // ��ʾ�ַ���EVEN��
#define CheckD3		0x02	 //ODD
#define CHECKD3_DISP 23*32*32*32+28*32*32+13*32+13  // ��ʾ�ַ���_odd��

//#define CheckD4		0x82	 //ONE
//#define CheckD5		0x86	 //ZERO
/********����λ����************/
#define Datab1  0x00         //5
#define DATAB1_DISP    5
#define Datab2  0x20         //6
#define DATAB2_DISP    6
#define Datab3  0x40         //7
#define DATAB3_DISP    7
#define Datab4  0x60         //8
#define DATAB4_DISP    8
//#define Datab0  0x60         //MASK
/********ֹͣλ����************/
//#define StopB0  0x08         //MASK
#define StopB1  0x00         //1
#define StopB2  0x08         //2

/********EE��ַ����************/
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








/**************װ�ö�ֵ���ýṹ��******************/
typedef struct
{
	uint32_t Add0;				//װ�õ�ַ					Add0		�洢��ַ0x00		
	uint32_t Cop0;				//װ�ù�Լ					Cop0		�洢��ַ0x04		
	uint32_t bAu0;				//ͨѶ������				bAu0		�洢��ַ0x08		
	uint32_t CHd0;				//У��λ					CHd0		�洢��ַ0x0c		
	uint32_t dAb0;				//����λ					dAb0		�洢��ַ0x10		
	uint32_t Stb0;				//ֹͣλ					Stb0		�洢��ַ0x14

	uint32_t Add1;				//װ�õ�ַ					Add1		�洢��ַ0x40		
	uint32_t Cop1;				//װ�ù�Լ					Cop1		�洢��ַ0x44	
	uint32_t bAu1;				//ͨѶ������				Bau1		�洢��ַ0x48	
	uint32_t CHd1;				//У��λ					CHd1		�洢��ַ0x4c	
	uint32_t dAb1;				//����λ					dAb1		�洢��ַ0x50		
	uint32_t Stb1;				//ֹͣλ					Stb1		�洢��ַ0x54

	int Wt00;				//�¶ȼ���ֹͣ�¶�ֵ		WtO0		�洢��ַ0x18		
	int WtA0;				//�¶ȼ��ȿ�ʼ�¶�ֵ		WtA0		�洢��ַ0x1c		
	int WF00;				//�¶�ͨ��ֹͣ�¶�ֵ		WFO0		�洢��ַ0x20			  
	int WFA0;				//�¶�ͨ�翪ʼ�¶�ֵ		WFA0		�洢��ַ0x24			  
	int St00;				//ʪ�ȼ���ֹͣʪ��ֵ		StO0		�洢��ַ0x28			  
	int StA0;				//ʪ�ȼ��ȿ�ʼʪ��ֵ		StA0		�洢��ַ0x2c			  
	int SF00;				//ʪ��ͨ��ֹͣʪ��ֵ		SFO0		�洢��ַ0x30			  
	int SFA0;				//ʪ��ͨ�翪ʼʪ��ֵ		SFA0		�洢��ַ0x34

	uint32_t VER;				//����汾��		        VEp			�洢��ַ0x38
	uint32_t CRC;				//����У����		        CpC			�洢��ַ0x3c


}eeprominit;

extern eeprominit myee;

//int regionsdown[20] = { 1,0,0,0,5,1,1,0,0,0,5,1,-40,-40,-40,-40,20,20,20,20, };
//int regionsup[20] = { 247,1,0,1,8,2, 247,1,0,1,8,2,100,100,100,100,90,90,90,90, };

typedef struct
{
	volatile long soft_timer;							//ϵͳ���ʱ��
	volatile long ledRefresh_timer;						//��������ܵ�ˢ��ʱ�����
	volatile long infHold_timer;						//���ں���̽��ı���ʱ��
	volatile long longPushAux_timer;					//���ڿ��Ƴ�������ֵ�仯�ٶȵĸ���
	volatile long restoreLedOrgState_timer;				//���ڱ�����������˸���ظ���ԭ��״̬�ļ�ʱ
	volatile long CommunicationAble0_timer;				//�����ж���ʪ�ȴ�������ͨѶ�Ƿ�����
	volatile long CommunicationAble1_timer;				//�����ж���ʪ�ȴ�������ͨѶ�Ƿ�����
	volatile long CommunicationAble0_timer_11s;				//�����ж���ʪ�ȴ�������ͨѶ��������11s
	volatile long CommunicationAble1_timer_11s;				//�����ж���ʪ�ȴ�������ͨѶ��������11s
	volatile long CommunicationToUp_timer;				//�����ж���ǰװ�ö���ͨѶ״̬
	volatile long daidianyuyin_timer;				//

}structTimer;
extern structTimer Timer;


#endif  // __APPLICATION_EEPROMINIT_H__
