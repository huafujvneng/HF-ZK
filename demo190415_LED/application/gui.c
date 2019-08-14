#include "../inc/hw_ints.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../driverlib/rom.h"
#include "../headfiles/gui.h"
#include "../headfiles/basicdefine.h"
#include "../driverlib/gpio.h"
#include "../headfiles/modbus_rtu.h"
#include "../headfiles/eeprominit.h"

//����Timer.soft_timer�������ȥ�ⲿ�ļ����ң���������ǲ����Ż��ģ���32λ����

//�����ṹ�����LedState
lcddisplay LcdState;

//�����ṹ�����LedTempState
lcddisplay LcdTempState;

const uint32_t	LED_TAB[4] = {
								0x42C27F80,			//LED1 RUN
								0x42C27F84,			//LED2 COM
								0x42C27F88,			//LED3 SET
								0x42C27F8C			//LED4 ERR
};

//�ֱ����˿�������ܵ��˸�����źţ�

//ʱ��
#define SEG_CLK		HWREG(0x42B87F80)	//PE0 CLK

//����
#define SEG_DATA	HWREG(0x42B87F90)	//PE4 DATA

//ʹ��
#define SEG_EN		HWREG(0x42B87F94)	//PE5 EN


#define SEG_STR		HWREG(0x42B87F98)	//PE6 STR


//�����õ����ַ�����������ܵĶ���
const uint8_t		SEG_TAB[32] = {
			0xC0, 0xf9, 0xa4, 0xb0, 		// 0	00	|	1	01	|	2	02	|	3	03
			0x99, 0x92, 0x82, 0xf8, 		// 4	04	|	5	05	|	6	06	|	7	07
			0x80, 0x90, 0x88, 0x83, 		// 8	08	|	9	09	|	A	10	|	b	11
			0xc6, 0xa1, 0x86, 0x8e, 		// C	12	|	d	13	|	E	14	|	F	15
			0xc1, 0xce, 0x89, 0xcf,			// U	16	|	r	17	|	H	18	|	I	19
			0xe3, 0xab, 0x8c, 0xff,			// u	20	|	n	21	|	P	22	|		23	
			0xc7, 0x98, 0x81, 0x87,			// L	24	|	q	25	|	W	26	|	t	27
			0xA3, 0xBF, 0xF1, 0xB6,			// o	28	|	-	29	|	J	30	|	��	31
};

//��������ܵ�С������룬ע������ܵĹ������ӷ����ǹ�����
#define	SEG_DP		(0x7F)


//��������ܵ�λѡ��IO�ڵĵ�ַ
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
	//Ĭ����ʾ��ֵ���ֱ�Ϊ��ʪ�ȣ�10���ƣ�һλС��
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

//��ʾ�������ݣ�dot-С����λ��
void SEG_Write_data(int data01, int data02, uint8_t dotup, uint8_t dotdown, uint8_t typeup, uint8_t typedown, uint8_t display)
{
	//���徲̬����pst���˴�Ҫ����ʲô�¾�̬��������ʲô�ص㣬ΪʲôҪʹ��pst����������ô���ʲô��
	static uint8_t pst;
	uint8_t i, j;

	//������������ֱܷ����������data1��data2���͵�cd4094������pdata
	uint32_t pdata, data1, data2;

	//��������������´����ֱ�õ���������ܵ�ÿһλ��ֵ
	if (data01 > -1 && data02 > -1)
	{
		data1 = data01;
		data2 = data02;

		//���������������ʾ��Χ����ֱ�ӷ��ز���ʾ
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

	//���data1�Ǹ���Ӧ����ô����
	else if (data01 < 0 && data02 > -1)
	{
		//����ķ����ǵ�������������Ϊ�������������ǲ��룬��ô�����ҵ�ԭ����Ϸ��žͿ����ˣ�������ȡ����һ����ô����
		//���������㣬��һȡ��
		data1 = data01 - 1;
		data1 = ~data1;
		data2 = data02;

		//���ڵ�һλ��ʾ���ţ�������ʾ��Χ�϶��ͱ����-0��-999��data2����
		if ((data1 > (typeup*typeup*typeup - 1)) || (data2 > (typedown*typedown*typedown*typedown - 1)))  return;
		switch (pst % 4)
		{
		case 3:

			//��һλ��ʾ���ţ�����ֱ�Ӳ�������29������
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

	//����ͬ��
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

	//����ͬ��
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

	//�ϳ�pdata����������������ܵ����ݷŵ�һ��32λ�����Ϊ������������׼��
	//ע���������㷨
		pdata = ((SEG_TAB[i] & ((pst % 4 == dotup) ? SEG_DP : 0xFF)) << 8)
			| ((SEG_TAB[j] & ((pst % 4 == dotdown) ? SEG_DP : 0xFF)) << 0);
		pdata = ~pdata; //��ȡ����ȡ�������㹲������ʾҪ��

		SEG_EN = 0;	// disable 4094

		//������������
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

		//����������ܵ�λѡ���㣬��������ʾ
		HWREG(SEG_CTL[(pst + 3) % 4]) = 0;
		HWREG(SEG_CTL[(pst + 3) % 4 + 4]) = 0;

		//���������ʾ�İ�������ڣ�����Ӧ��λ����
		if (Timer.soft_timer % LcdState.lightconfig < (LcdState.lightconfig / 2))
		{
			HWREG(SEG_CTL[pst % 4]) = 1;
			HWREG(SEG_CTL[pst % 4 + 4]) = 1;
		}

		//���������˸���������������˸���������������Ӧ��λ����������
		else
		{
			HWREG(SEG_CTL[pst % 4]) = ~(((ledlightconvert(LcdState.light) >> (pst % 4))) & 0x01);
			HWREG(SEG_CTL[pst % 4 + 4]) = ~(((ledlightconvert(LcdState.light) >> (pst % 4) + 4)) & 0x01);
		}
		//delay();

		//ÿ��ѭ��+1
		pst++;
	}

	//�˺������ڳ�ʼ������ܵ���ʾ�����Ѿ�������
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

	//�˺������ڵ�����˸������λ��Ӧ��ϵ��������8λ����һһ��Ӧ����ܴ����ϵ����µ�8��λ��
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

