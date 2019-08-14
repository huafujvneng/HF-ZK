#include "stdint.h"
#ifndef __APPLICATION_UARTINIT_H__
#define __APPLICATION_UARTINIT_H__
#define LOCKED 0xff
#define UNLOCK 0x00
#define HAVESEND	0xff
#define NONESEND	0x00
void UartInit(void);
void UART0Send(const uint8_t *pui8Buffer, uint32_t ui32Count);
void UART6Send(const uint8_t *pui8Buffer, uint32_t ui32Count);
void rx0_modbus_init(void);
void rx6_modbus_init(void);
void tx0_modbus_init(void);
void tx6_modbus_init(void);
void tempuint32init(void);
void tempint32init(void);
void tempuint8init(void);
void tempint8init(void);
void tempuint16init(void);
void tempint16init(void);
void ReadWSD0(void);
void ReadWSD1(void);
void TX_MODBUS_SEND(uint8_t sendbufflength);
void TX_MODBUS(uint8_t *p, uint8_t length);
void rx0_modbus_init(void);
void rx6_modbus_init(void);
void CheckWsdComState(void);

void ykIOOut(uint8_t flag);

typedef struct
{
	uint8_t dt[255];
	uint8_t *rxpt;
	uint8_t count;
	uint8_t loopnum;
	uint8_t lock_state;

}tx_modbus;
typedef struct
{
	uint32_t data[100];
	uint32_t *pt;
}tempuint32arr;
typedef struct
{
	int data[100];
	int *pt;
}tempint32arr;
typedef struct
{
	uint8_t data[100];
	uint8_t *pt;
}tempuint8arr;
typedef struct
{
	int8_t data[100];
	int8_t *pt;
}tempint8arr;
typedef struct
{
	uint16_t data[100];
	uint16_t *pt;
}tempuint16arr;
typedef struct
{
	int16_t data[100];
	int16_t *pt;
}tempint16arr;

typedef struct
{
	uint8_t dr[255];
	//uint8_t dr[9];//--wushuisheng add 2018年4月18日 15:15:08
	uint8_t *rxpt;
	uint8_t count;
	uint8_t loopnum;
	uint8_t lock_state;
	uint8_t ifdone;
}rx_modbus;

typedef struct
{
	uint8_t power;
}virtualyk;
extern virtualyk yk;
typedef struct
{
	int temperature;
	int humidity;
}virtualwsd;

typedef struct
{
	uint8_t sendbufflength;
	int sendbufftimer;
}virtualsend;
typedef struct
{
	uint8_t ComAbleFlag0;
	uint8_t ComAbleFlag1;
	uint8_t ComAbleFlag0_11s;//通迅正常等待11s开始数据判断
	uint8_t ComAbleFlag1_11s;//通迅正常等待11s开始数据判断

}structComAble;
extern structComAble Comable;
extern virtualsend uart6sendbuff;
extern virtualwsd wsd[2];
extern virtualyk yk;
extern tx_modbus mt0;
extern rx_modbus mr0;
extern rx_modbus mr6;
extern tempuint32arr ua32;
extern tempint32arr a32;
extern tempuint8arr ua8;
extern tempint8arr a8;
extern tempuint16arr ua16;
extern tempint16arr a16;
#endif // __APPLICATION_UARTINIT_H__


