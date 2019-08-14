#include "../inc/hw_ints.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../driverlib/rom.h"
#include "../headfiles/gui.h"
#include "../headfiles/basicdefine.h"
#include "../driverlib/gpio.h"
#include "../headfiles/modbus_rtu.h"
#include "../headfiles/eeprominit.h"

//声明Timer.soft_timer这个变量去外部文件查找，这个变量是不可优化的，是32位整形

//声明结构体变量LedState
lcddisplay LcdState;

//声明结构体变量LedTempState
lcddisplay LcdTempState;

const uint32_t	LED_TAB[4] = {
								0x42C27F80,			//LED1 RUN
								0x42C27F84,			//LED2 COM
								0x42C27F88,			//LED3 SET
								0x42C27F8C			//LED4 ERR
};

//分别定义了控制数码管的人格控制信号，

//时钟
#define SEG_CLK		HWREG(0x42B87F80)	//PE0 CLK

//数据
#define SEG_DATA	HWREG(0x42B87F90)	//PE4 DATA

//使能
#define SEG_EN		HWREG(0x42B87F94)	//PE5 EN


#define SEG_STR		HWREG(0x42B87F98)	//PE6 STR


//定义用到的字符集，即数码管的段码
const uint8_t		SEG_TAB[32] = {
			0xC0, 0xf9, 0xa4, 0xb0, 		// 0	00	|	1	01	|	2	02	|	3	03
			0x99, 0x92, 0x82, 0xf8, 		// 4	04	|	5	05	|	6	06	|	7	07
			0x80, 0x90, 0x88, 0x83, 		// 8	08	|	9	09	|	A	10	|	b	11
			0xc6, 0xa1, 0x86, 0x8e, 		// C	12	|	d	13	|	E	14	|	F	15
			0xc1, 0xce, 0x89, 0xcf,			// U	16	|	r	17	|	H	18	|	I	19
			0xe3, 0xab, 0x8c, 0xff,			// u	20	|	n	21	|	P	22	|		23	
			0xc7, 0x98, 0x81, 0x87,			// L	24	|	q	25	|	W	26	|	t	27
			0xA3, 0xBF, 0xF1, 0xB6,			// o	28	|	-	29	|	J	30	|	三	31
};

//定义数码管的小数点段码，注意数码管的共阴极接法还是共阳极
#define	SEG_DP		(0x7F)


//定义数码管的位选端IO口的地址
const uint32_t	SEG_CTL[8] = {
										0x42BE7F80,			//SEG1
										0x42BE7F84,			//SEG2
										0x42BE7F88,			//SEG3
										0x42BE7F8C,			//SEG4
										0x42BE7F90,			//SEG5
										0x42BE7F94,			//SEG6
										0x42BE7F98,			//SEG7
										0x42BE7F9C			//SEG8
};

void SEG_Write_data(int data1, int data2, uint8_t dotup, uint8_t dotdown, uint8_t typeup, uint8_t typedown, uint8_t display);
// 7-SEG
void SEG_Write(uint16_t data1, uint16_t data2)
{
	//默认显示的值，分别为温湿度，10进制，一位小数
	SEG_Write_data(data1, data2, 2, 2, 10, 10, 2);
}
void delay()
{
	int x, y;
	for (x = 0; x < 20; x++)
	{
		for (y = 0; y < 200; y++);

	}
}

//显示两行数据，dot-小数点位置
void SEG_Write_data(int data01, int data02, uint8_t dotup, uint8_t dotdown, uint8_t typeup, uint8_t typedown, uint8_t display)
{
	//定义静态变量pst，此处要明白什么事静态变量，有什么特点，为什么要使用pst这个变量，好处是什么？
	static uint8_t pst;
	uint8_t i, j;

	//定义上下数码管分别送入的数据data1，data2和送到cd4094的数据pdata
	uint32_t pdata, data1, data2;

	//如果都是正数如下处理，分别得到上下数码管的每一位的值
	if (data01 > -1 && data02 > -1)
	{
		data1 = data01;
		data2 = data02;

		//如果超出了最大的显示范围，则直接返回不显示
		if ((data1 > (typeup*typeup*typeup*typeup - 1)) || (data2 > (typedown*typedown*typedown*typedown - 1)))  return;
		switch (pst % 4)
		{
		case 3:
			i = data1 % typeup;
			j = data2 % typedown;
			break;
		case 2:
			i = (data1 / typeup) % typeup;
			j = (data2 / typedown) % typedown;
			break;
		case 1:
			i = (data1 / typeup / typeup) % typeup;
			j = (data2 / typedown / typedown) % typedown;
			break;
		case 0:
			i = (data1 / typeup / typeup / typeup) % typeup;
			j = (data2 / typedown / typedown / typedown) % typedown;
			break;
		}
	}

	//如果data1是负数应该怎么处理？
	else if (data01 < 0 && data02 > -1)
	{
		//处理的方法是当做正数处理，因为计算机负数存的是补码，那么我们找到原码加上符号就可以了，补码是取反加一，那么这里
		//就是逆运算，减一取反
		data1 = data01 - 1;
		data1 = ~data1;
		data2 = data02;

		//由于第一位显示负号，所以显示范围肯定就变成了-0到-999，data2不变
		if ((data1 > (typeup*typeup*typeup - 1)) || (data2 > (typedown*typedown*typedown*typedown - 1)))  return;
		switch (pst % 4)
		{
		case 3:

			//第一位显示负号，所以直接查段码表，第29个就是
			i = data1 % typeup;
			j = data2 % typedown;
			break;
		case 2:
			i = (data1 / typeup) % typeup;
			j = (data2 / typedown) % typedown;
			break;
		case 1:
			i = (data1 / typeup / typeup) % typeup;
			j = (data2 / typedown / typedown) % typedown;
			break;
		case 0:
			i = 29;
			j = (data2 / typedown / typedown / typedown) % typedown;
			break;
		}
	}

	//与上同理
	else if (data01 > -1 && data02 < 0)
	{
		data1 = data01;
		data2 = data02;
		data2--;
		data2 = ~data2;
		if ((data1 > (typeup*typeup*typeup*typeup - 1)) || (data2 > (typedown*typedown*typedown - 1)))  return;
		switch (pst % 4)
		{
		case 3:
			i = data1 % typeup;
			j = data2 % typedown;
			break;
		case 2:
			i = (data1 / typeup) % typeup;
			j = (data2 / typedown) % typedown;
			break;
		case 1:
			i = (data1 / typeup / typeup) % typeup;
			j = (data2 / typedown / typedown) % typedown;
			break;
		case 0:
			i = (data1 / typeup / typeup / typeup) % typeup;
			j = 29;
			break;
		}
	}

	//与上同理
	else if (data01 < 0 && data02 < 0)
	{
		data1 = data01;
		data1--;
		data1 = ~data1;
		data2 = data02;
		data2--;
		data2 = ~data2;
		if ((data1 > (typeup*typeup*typeup - 1)) || (data2 > (typedown*typedown*typedown - 1)))  return;
		switch (pst % 4)
		{
		case 3:
			i = data1 % typeup;
			j = data2 % typedown;
			break;
		case 2:
			i = (data1 / typeup) % typeup;
			j = (data2 / typedown) % typedown;
			break;
		case 1:
			i = (data1 / typeup / typeup) % typeup;
			j = (data2 / typedown / typedown) % typedown;
			break;
		case 0:
			i = 29;
			j = 29;
			break;

		}
	}

	//合成pdata，把上下两个数码管的数据放到一个32位数据里，为下面送数据做准备
	//注意理解这个算法
		pdata = ((SEG_TAB[i] & ((pst % 4 == dotup) ? SEG_DP : 0xFF)) << 8)
			| ((SEG_TAB[j] & ((pst % 4 == dotdown) ? SEG_DP : 0xFF)) << 0);
		pdata = ~pdata; //获取数据取反，满足共阴极显示要求

		SEG_EN = 0;	// disable 4094

		//串行送入数据
		for (j = 0; j < 16; j++)
		{
			if (pdata & 0x8000)
				SEG_DATA = 1;
			else
				SEG_DATA = 0;
			SEG_CLK = 0;
			SEG_CLK = 1;
			pdata <<= 1;

		}

		//把上下数码管的位选清零，即均不显示
		HWREG(SEG_CTL[(pst + 3) % 4]) = 0;
		HWREG(SEG_CTL[(pst + 3) % 4 + 4]) = 0;

		//如果处于显示的半个周期内，则相应的位常亮
		if (Timer.soft_timer % LcdState.lightconfig < (LcdState.lightconfig / 2))
		{
			HWREG(SEG_CTL[pst % 4]) = 1;
			HWREG(SEG_CTL[pst % 4 + 4]) = 1;
		}

		//如果处于闪烁的周期内则根据闪烁参数控制数码管相应的位是亮还是灭
		else
		{
			HWREG(SEG_CTL[pst % 4]) = ~(((ledlightconvert(LcdState.light) >> (pst % 4))) & 0x01);
			HWREG(SEG_CTL[pst % 4 + 4]) = ~(((ledlightconvert(LcdState.light) >> (pst % 4) + 4)) & 0x01);
		}
		//delay();

		//每次循环+1
		pst++;
	}

	//此函数用于初始化数码管的显示，现已经不适用
	void lcddisplaystateinit(void)
	{
		LcdState.dataup = Timer.soft_timer / 10;
		LcdState.datadown = Timer.soft_timer / 100;
		LcdState.dotsiteup = 0;
		LcdState.dotsitedown = 2;
		LcdState.jinzhiup = 10;
		LcdState.jinzhidown = 10;
		LcdState.light = 0x69;
		LcdState.lightconfig = 1000;

	}

	//此函数用于调整闪烁参数的位对应关系，以满足8位正好一一对应数码管从左上到右下的8个位；
	uint8_t ledlightconvert(uint8_t e)
	{
		uint8_t result;
		uint8_t a1, a2, a3, a4, a5, a6, a7, a8;
		a1 = (e >> 4) & 0x01;
		a2 = (e >> 5) & 0x01;
		a3 = (e >> 6) & 0x01;
		a4 = (e >> 7) & 0x01;
		a5 = (e >> 0) & 0x01;
		a6 = (e >> 1) & 0x01;
		a7 = (e >> 2) & 0x01;
		a8 = (e >> 3) & 0x01;
		result = 0;
		result <<= 1;
		result += a1;
		result <<= 1;
		result += a2;
		result <<= 1;
		result += a3;
		result <<= 1;
		result += a4;
		result <<= 1;
		result += a5;
		result <<= 1;
		result += a6;
		result <<= 1;
		result += a7;
		result <<= 1;
		result += a8;
		return result;
	}

