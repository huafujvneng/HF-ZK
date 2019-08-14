#ifndef __APPLICATION_SPEAKER_H__
#define __APPLICATION_SPEAKER_H__
#define 一						0x00
#define 二						0x01
#define 三						0x02
#define 四						0x03
#define 五						0x04
#define 六						0x05
#define 七						0x06
#define 八						0x07
#define 九						0x08
#define 十						0x09
#define 百						0x0a
#define 百分之					0x0b
#define 本柜回路已带电			0x0c
#define 程序初始化成功			0x0d
#define 程序初始化失败			0x0e
#define 弹簧未储能		        0x0f	
#define 弹簧已储能		        0x10	
#define 当前					0x11
#define 点						0x12
#define 分闸					0x13
#define 分钟					0x14
#define 负						0x15
#define 合闸					0x16
#define 报警					0x17
#define 开入					0x18
#define 零下					0x19
#define 秒						0x1a
#define 千						0x1b
#define 请						0x1c
#define 请分断路器_请分接地刀   0x1d
#define 请分断路器              0x1e
#define 请分接地刀              0x1f
#define 人体接近                0x20
#define 摄氏度                  0x21
#define 湿度	                0x22
#define 时间                    0x23
#define 万                      0x24
#define 温度                    0x25
#define 语音自检正常            0x26
/**********************语音功能码库************************/
#define 关闭功放                0xf0
#define 打开功放                0xf1
#define 循环播放                0xf2
#define 连码播放                0xf3
#define 静音                    0xf8
#define 停止播放                0xfe
#define 音量1                   0xe0
#define 音量2                   0xe1
#define 音量3                   0xe2
#define 音量4                   0xe3
#define 音量5                   0xe4
#define 音量6                   0xe5
#define 音量7                   0xe6
#define 音量8                   0xe7
#define 音量9                   0xe8
#define 音量10                  0xe9
#define 音量11                  0xea
#define 音量12                  0xeb
#define 音量13                  0xec
#define 音量14                  0xed
#define 音量15                  0xee
#define 音量16                  0xef
/********************静音时间*************************/
#define 延时100毫秒                 0x64



void Speaker(uint8_t id);
void My_delayus(uint32_t us);
void My_delayms(uint32_t ms);
//void Speak_Pool(void);
void SpeakerPool(uint8_t *arr, uint8_t num1);
//void Speak_Poolone(void);
void TIMER0A_Handler(void);
//void pushspeakercontent(int x);
void speak_lock(uint8_t *arr);
typedef struct
{
	uint8_t flag5ms;
	uint8_t flag10ms;
	uint8_t flag70ms;
	uint8_t flagbyte;
	uint8_t numbers;
	uint8_t *pt_con;
	uint8_t content[100];
	uint8_t pushflag;
	uint32_t timeracount;
	uint8_t daidianflag;//本柜回路已经带电语音标志位
	uint8_t daidianyuyinflag;
}virtual_spk;
extern virtual_spk spk;
typedef struct
{
	uint8_t flagspk;
}virtual_flagspk;
extern virtual_flagspk fspk[8];
#endif // __APPLICATION_SPEAKER_H__
