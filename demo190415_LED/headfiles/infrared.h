#ifndef __APPLICATION_HONGWAI_H__
#define __APPLICATION_HONGWAI_H__
#define INF_ON 0xff
#define INF_OFF 0x00


	 
void infrared(void);	
void  Bisuo(void);
typedef struct
{
	uint8_t LCD_ON_OFF;			//��������������ʾ����
	uint8_t inf_flag;			//������⿪�صĸ�������
}inf_detectors;
extern inf_detectors inf1;

#endif // __APPLICATION_HONGWAI_H__

