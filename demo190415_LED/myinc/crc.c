
//MODBUS CRC16校验程序
//返回值为校验码的高地位，高字节在前，低字节在后
//*pCRCbuff为报文寄存器的首地址
//length为当前报文寄存器的有效字节个数
//北京华孚聚能科技有限公司朱鹏程
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


