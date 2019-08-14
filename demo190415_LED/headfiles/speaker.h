#ifndef __APPLICATION_SPEAKER_H__
#define __APPLICATION_SPEAKER_H__
#define һ						0x00
#define ��						0x01
#define ��						0x02
#define ��						0x03
#define ��						0x04
#define ��						0x05
#define ��						0x06
#define ��						0x07
#define ��						0x08
#define ʮ						0x09
#define ��						0x0a
#define �ٷ�֮					0x0b
#define �����·�Ѵ���			0x0c
#define �����ʼ���ɹ�			0x0d
#define �����ʼ��ʧ��			0x0e
#define ����δ����		        0x0f	
#define �����Ѵ���		        0x10	
#define ��ǰ					0x11
#define ��						0x12
#define ��բ					0x13
#define ����					0x14
#define ��						0x15
#define ��բ					0x16
#define ����					0x17
#define ����					0x18
#define ����					0x19
#define ��						0x1a
#define ǧ						0x1b
#define ��						0x1c
#define ��ֶ�·��_��ֽӵص�   0x1d
#define ��ֶ�·��              0x1e
#define ��ֽӵص�              0x1f
#define ����ӽ�                0x20
#define ���϶�                  0x21
#define ʪ��	                0x22
#define ʱ��                    0x23
#define ��                      0x24
#define �¶�                    0x25
#define �����Լ�����            0x26
/**********************�����������************************/
#define �رչ���                0xf0
#define �򿪹���                0xf1
#define ѭ������                0xf2
#define ���벥��                0xf3
#define ����                    0xf8
#define ֹͣ����                0xfe
#define ����1                   0xe0
#define ����2                   0xe1
#define ����3                   0xe2
#define ����4                   0xe3
#define ����5                   0xe4
#define ����6                   0xe5
#define ����7                   0xe6
#define ����8                   0xe7
#define ����9                   0xe8
#define ����10                  0xe9
#define ����11                  0xea
#define ����12                  0xeb
#define ����13                  0xec
#define ����14                  0xed
#define ����15                  0xee
#define ����16                  0xef
/********************����ʱ��*************************/
#define ��ʱ100����                 0x64



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
	uint8_t daidianflag;//�����·�Ѿ�����������־λ
	uint8_t daidianyuyinflag;
}virtual_spk;
extern virtual_spk spk;
typedef struct
{
	uint8_t flagspk;
}virtual_flagspk;
extern virtual_flagspk fspk[8];
#endif // __APPLICATION_SPEAKER_H__
