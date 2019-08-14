#ifndef __APPLICATION_KEYPOLL_H__
#define __APPLICATION_KEYPOLL_H__

#define KEY_1			0x1c	  //设置对应的按键返回值，KEY_1,KEY_2,KEY_3,KEY_4分别代表自左至右4个按键。
#define KEY_2			0x2c	  //设置对应的按键返回值，KEY_1,KEY_2,KEY_3,KEY_4分别代表自左至右4个按键。
#define KEY_3			0x34	  //设置对应的按键返回值，KEY_1,KEY_2,KEY_3,KEY_4分别代表自左至右4个按键。
#define KEY_4			0x38	  //设置对应的按键返回值，KEY_1,KEY_2,KEY_3,KEY_4分别代表自左至右4个按键。
#define ALLKEYRAISE		0x3c
#define ON 0xff
#define OFF 0x00

#define SHORTPRESSEFFECTIVE 1
#define LONGPRESSEFFECTIVE	2
#define NONPRESSEFFECTIVE   0

#define PASSWORDISEFFECTIVE 1
#define PASSWORDNOEFFECTIVE 0


#define page_Add0 0
#define page_Cop0 1
#define page_bAu0 2
#define page_CHd0 3
#define page_dAb0 4
#define page_Stb0 5

#define page_Add1 6
#define page_Cop1 7
#define page_bAu1 8
#define page_CHd1 9
#define page_dAb1 10
#define page_Stb1 11

#define page_Wt00 12
#define page_WtA0 13
#define page_WF00 14
#define page_WFA0 15
#define page_St00 16
#define page_StA0 17
#define page_SF00 18
#define page_SFA0 19

#define page_VEp 20
#define page_CpC 21

#define page_password 255
#define PASSWORDLIFETIME 60000*5
#define PASSWORD    1000

void KeyHander(void);
void key_rjg(void);
void Led_light(void);
void input_rjg(void);
void diffseconds_to_clock(void);
void goverifypassword(uint8_t org);
void inputcontroloutput(void);
void inputsperker(void);
uint32_t ledsum(uint8_t q, uint8_t b, uint8_t s, uint8_t g, uint8_t e);
void ledcontrol(int ledIndex, uint8_t ledState, uint32_t ledPeriod);
typedef struct
{
	uint32_t keytimer;
	uint8_t	keycurvalue;
	uint8_t keychanged;
	uint8_t keyhisvalue;
	uint16_t keychangtime;
	uint8_t keyvalid;
	uint8_t effectivevalue;
	uint8_t nolongpressonce;
	uint8_t nolongpressoncehelp;
}virtualkey;

typedef struct
{
	uint8_t LCD_ON_OFF;
}LL;


typedef struct
{
	uint32_t temp_value;
	uint8_t nonjitter_times;
	uint32_t start_time_p;
	uint32_t start_time_n;
	uint8_t value;
}virtualinput;
extern virtualinput input[9],output[9];

typedef struct
{
	uint32_t diffseconds;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}mytime;
extern mytime softclock;

typedef struct
{
	uint8_t ifeffective;
	int password_effective_timer;
}virtualpassword;
extern virtualpassword password;
#endif // __APPLICATION_KEYPOLL_H__

