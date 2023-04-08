#include "MyTime.h"

static unsigned long countdown_time; // ����ʱ�䵹��ʱ

// ��ʱ��0�ĳ�ʼ��
void delay_init()
{
    // ����Ƶ������ʹ���ڲ���չ��1024�ֽ�ram
    AUXR = 0x80;
    // ����T0��ģʽ,16λȫ��
    TMOD = 0x01;
    // װ�붨ʱ��0�ļĴ���
    TL0 = TIMECOUNT_MS;
    TH0 = TIMECOUNT_MS >> 8;

    // ����ȫ���ж�
    EA = 1;
    // ����Ĵ���0�ж�
    ET0 = 1;
}

void delay_ms(unsigned long time)
{
    countdown_time = time;
    TR0            = 1; // ����T0��ʱ��
    while (countdown_time > 0)
        ;
    TR0 = 0; // �ر�T0��ʱ��
}

void time0_interrupt() interrupt 1
{
    TL0 = TIMECOUNT_MS;
    TH0 = TIMECOUNT_MS >> 8;
    countdown_time--;
}
