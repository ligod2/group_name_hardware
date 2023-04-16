#include "MyUART.h"

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
unsigned char xdata RX_buffer[BUF_LENTH];
unsigned int RX_num = 0; // ���ռ�������



void uart_init()
{
    // ����1 ��ʼ��
    // B7/SMOD=1,��ʹ����ͨ�ŷ�ʽ1��2��3�Ĳ��ʼӱ�
    // B6/SMOD0=0,SM0��SM1���ù���ģʽ
    PCON |= 0x80;
    // B7/SM0=0,P6/SM1=1��������ʽ1
    // B5/REN=1:���н��տ���λ
    // ������ʽ1�Ĳ����� = ( 2^SMOD/32 )��(SYSclk / ( 256 - BRT ))
    SCON = 0x50;
    // �Ѹ��ݲ����ʼ���� ��ʱ����ֵ��װ�벨���ʷ�����
    BRT = BRT_Reload;
    // B4/BRTR=1:������������ʷ���������
    // B2/BRTx12=1:���������ʷ�����ÿ1��ʱ�Ӽ���һ��
    // B1/EXTRAM=0:����ʹ���ڲ���չ��1024�ֽ���չRAM
    // B0/S1BRS=1:���������ʷ�������Ϊ���п�1�Ĳ����ʷ�����
    AUXR |= 0x1d;

    // ����2 ��ʼ��
    S2CON = 0x50; // 8λ���ݣ��ɱ䲨����
    AUXR |= 0x14; // ����2ʹ�ö��������ʷ�����  ���ҿ���1Tģʽ

    ES = 1; // IE�Ĵ�����ESλΪ1 ����1�ж�������
    IE2 = 0x01; // ����2�жϴ�
    EA = 1; // IE�Ĵ�����EAλΪ1 CPU���ж�������
}

// ����1���ʹ�������(�ֽ�)
void uart1_sendByte(unsigned char dat)
{
    SBUF = dat;
    while (TI == 0)
        ;
    TI = 0;
}

// ����1�����ַ���
void uart1_sendStr(unsigned char *str)
{
    while (*str) {
        uart1_sendByte(*str);
        str++;
    }
}

// ����1����ָ�������ַ���
void uart1_send(unsigned char *str, unsigned char len)
{
    while (len--) {
        uart1_sendByte(*str);
        str++;
    }
}

// ����2���ʹ�������(�ֽ�)
void uart2_sendByte(unsigned char dat)
{
    S2BUF = dat; // д���ݵ�UART2���ݼĴ���
    while (!(S2CON & S2TI))
        ;
    S2CON &= ~S2TI;
}

// ����2�����ַ���
void uart2_sendStr(unsigned char *str)
{
    IE2 = 0x00;
    while (*str) // ����ַ���������־
    {
        uart2_sendByte(*str++); // ���͵�ǰ�ַ�
    }
    IE2 = 0x01;
}

void uart2_clearBuf(){
    memset(RX_buffer, '\0', BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0
}

/////////////////////////////////
/////////////////////////////////


// ����1�ж�
void Uart1() interrupt 4
{
    ES = 0; // �رմ���1�ж�
    if (RI) {
        RI                = 0; // ���RIλ
        RX_buffer[RX_num] = SBUF;
        RX_num++;
        if (RX_num > BUF_LENTH) RX_num = 0;
    }
    if (TI) {
        TI = 0; // ���TIλ
    }
    ES = 1; // ��������1�ж�
}

// ����2�ж�
void Uart2() interrupt 8
{

    IE2 = 0x00; // �رմ���2�ж�
    if (S2CON & S2RI) {
        S2CON &= ~S2RI;
        RX_buffer[RX_num] = S2BUF;
        RX_num++;
        if (RX_num > BUF_LENTH) RX_num = 0;
    }
    if (S2CON & S2TI) {
        S2CON &= ~S2TI;
    }
    IE2 = 0x01; // ��������2�ж�
}

