#include "MyTime.h"

static unsigned long countdown_time; // 定义时间倒计时

// 定时器0的初始化
void delay_init()
{
    // 不分频，允许使用内部拓展的1024字节ram
    AUXR = 0x80;
    // 设置T0的模式,16位全用
    TMOD = 0x01;
    // 装入定时器0的寄存器
    TL0 = TIMECOUNT_MS;
    TH0 = TIMECOUNT_MS >> 8;

    // 允许全部中断
    EA = 1;
    // 允许寄存器0中断
    ET0 = 1;
}

void delay_ms(unsigned long time)
{
    countdown_time = time;
    TR0            = 1; // 启动T0计时器
    while (countdown_time > 0)
        ;
    TR0 = 0; // 关闭T0计时器
}

void time0_interrupt() interrupt 1
{
    TL0 = TIMECOUNT_MS;
    TH0 = TIMECOUNT_MS >> 8;
    countdown_time--;
}
