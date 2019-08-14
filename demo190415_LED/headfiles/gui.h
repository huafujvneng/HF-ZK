#ifndef __APPLICATION_GUI_H__
#define __APPLICATION_GUI_H__

#define $_0			0
#define $_1			1
#define $_2			2
#define $_3			3
#define $_4			4
#define $_5			5
#define $_6			6
#define $_7			7
#define $_8			8
#define $_9			9
#define $_A			10
#define $_b			11
#define $_C			12
#define $_d			13
#define $_E			14
#define $_F			15
#define $_U			16
#define $_r			17
#define $_H			18
#define $_I			19
#define $_u			20
#define $_n			21
#define $_p			22
#define $_NA		23
#define $_L			24
#define $_q			25
#define $_W			26
#define $_t			27
#define $_o			28
#define $_YI		29
#define $_J			30
#define $_SAN		31

#include <stdint.h>
#include <stdbool.h>
uint8_t ledlightconvert(uint8_t e);
void lcddisplaystateinit(void);
void SEG_Write_data(int data1, int data2, uint8_t dotup, uint8_t dotdown, uint8_t typeup, uint8_t typedown, uint8_t display);
typedef struct
{
	uint32_t 	key_value;
	uint32_t 	key_used;
	uint8_t		key_stack[4];
	uint16_t	key_timer[4];
	
	uint32_t  key_plus10flag;
	uint8_t		key_plus10_timer[4];
} KEYSTRUCT;


#define		LED_RUN			0
#define		LED_COM			1
#define		LED_SET			2
#define		LED_ERR			3


#define		KEY_UP		0x00000001
#define		KEY_DOWN	0x00000002
#define		KEY_CR		0x00000004
#define		KEY_EXIT	0x00000008



#define	MENU_INDEX_

typedef struct
{
	uint32_t	tick_timer;
	
	uint8_t		led[4];
	uint8_t   cdata[4];
	
	uint8_t		menu_index;
	uint8_t		char_index;
	uint8_t		set_index;
	uint8_t		flash_f;
	
	uint8_t   handle_f;
	uint8_t		N_f;

} GUISTATUS;
typedef struct
{
	int dataup;
	int datadown;
	uint8_t dotsiteup;
	uint8_t dotsitedown;
	uint8_t jinzhiup;
	uint8_t jinzhidown;
	uint8_t light;
	uint32_t lightconfig;
	uint8_t page;
	uint8_t oldlightstate;
	uint8_t newlightstate;
	int pagetimer;
}lcddisplay;
extern lcddisplay LcdState;
extern lcddisplay LcdTempState;
void GUIMain(void);

#endif // __APPLICATION_GUI_H__
