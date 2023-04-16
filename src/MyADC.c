#include "MyADC.h"

void adc_init()
{
    P1ASF     = 0x80; // 开启1.7路ADC功能
    // ADC_RES   = 0;    // 清楚之前的结果
    // ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    // delay_ms(2); // ADC上电和延时
}

unsigned char GetADCResult(unsigned char ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START | ADC_AISLE;
    _nop_(); // 查询前必须等待
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG))
        ;                   // 等待完成标志
    ADC_CONTR &= ~ADC_FLAG; // 关闭 ADC
    return ADC_RES;         // 返回ADC结果
}