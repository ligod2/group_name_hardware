#include "MyADC.h"

void adc_init()
{
    P1ASF     = 0x80; // ����1.7·ADC����
    // ADC_RES   = 0;    // ���֮ǰ�Ľ��
    // ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    // delay_ms(2); // ADC�ϵ����ʱ
}

unsigned char GetADCResult(unsigned char ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START | ADC_AISLE;
    _nop_(); // ��ѯǰ����ȴ�
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG))
        ;                   // �ȴ���ɱ�־
    ADC_CONTR &= ~ADC_FLAG; // �ر� ADC
    return ADC_RES;         // ����ADC���
}