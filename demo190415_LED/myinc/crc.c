
//MODBUS CRC16У�����
//����ֵΪУ����ĸߵ�λ�����ֽ���ǰ�����ֽ��ں�
//*pCRCbuffΪ���ļĴ������׵�ַ
//lengthΪ��ǰ���ļĴ�������Ч�ֽڸ���
//�������ھ��ܿƼ����޹�˾������
unsigned int Modbus_Crc(unsigned char *pCRCbuff,int length)
{
    unsigned char *p=pCRCbuff;
    unsigned int crc=0xffff;
    int i,j=0;

    for(i=0;i<length;i++)
    {
        crc ^= *(p+i);
        for(j=0;j<8;j++)
        {
            if((crc&0x0001)==0x0001)
            {
                crc=(crc>>1)&0xffff;
                crc^=0xa001;
            }
            else
            {
                crc=(crc>>1)&0xffff;
            }
        }
    }
    return crc;
}


