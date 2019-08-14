#ifndef __APPLICATION_HONGWAI_H__
#define __APPLICATION_HONGWAI_H__
#define INF_ON 0xff
#define INF_OFF 0x00


	 
void infrared(void);	
void  Bisuo(void);
typedef struct
{
	uint8_t LCD_ON_OFF;			//定义控制数码管显示开关
	uint8_t inf_flag;			//定义红外开关的辅助功能
}inf_detectors;
extern inf_detectors inf1;

#endif // __APPLICATION_HONGWAI_H__

