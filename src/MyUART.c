#include "MyUART.h"

#define S2RI  0x01              //S2CON.0
#define S2TI  0x02              //S2CON.1
unsigned char xdata RX_buffer[BUF_LENTH];
unsigned int RX_num = 0; // 接收计数变量



void uart_init()
{
    // 串口1 初始化
    // B7/SMOD=1,则使串行通信方式1、2、3的波率加倍
    // B6/SMOD0=0,SM0和SM1设置工作模式
    PCON |= 0x80;
    // B7/SM0=0,P6/SM1=1：工作方式1
    // B5/REN=1:串行接收控制位
    // 工作方式1的波特率 = ( 2^SMOD/32 )×(SYSclk / ( 256 - BRT ))
    SCON = 0x50;
    // 把根据波特率计算的 定时器的值，装入波特率发生器
    BRT = BRT_Reload;
    // B4/BRTR=1:允许独立波特率发生器运行
    // B2/BRTx12=1:独立波特率发生器每1个时钟计数一次
    // B1/EXTRAM=0:允许使用内部扩展的1024字节扩展RAM
    // B0/S1BRS=1:独立波特率发生器作为串行口1的波特率发生器
    AUXR |= 0x1d;

    // 串口2 初始化
    S2CON = 0x50; // 8位数据，可变波特率
    AUXR |= 0x14; // 串口2使用独立波特率发生器  并且开启1T模式

    ES = 1; // IE寄存器的ES位为1 串口1中断允许开启
    IE2 = 0x01; // 串口2中断打开
    EA = 1; // IE寄存器的EA位为1 CPU总中断允许开启
}

// 串口1发送串口数据(字节)
void uart1_sendByte(unsigned char dat)
{
    SBUF = dat;
    while (TI == 0)
        ;
    TI = 0;
}

// 串口1发送字符串
void uart1_sendStr(unsigned char *str)
{
    while (*str) {
        uart1_sendByte(*str);
        str++;
    }
}

// 串口1发送指定长度字符串
void uart1_send(unsigned char *str, unsigned char len)
{
    while (len--) {
        uart1_sendByte(*str);
        str++;
    }
}

// 串口2发送串口数据(字节)
void uart2_sendByte(unsigned char dat)
{
    S2BUF = dat; // 写数据到UART2数据寄存器
    while (!(S2CON & S2TI))
        ;
    S2CON &= ~S2TI;
}

// 串口2发送字符串
void uart2_sendStr(unsigned char *str)
{
    IE2 = 0x00;
    while (*str) // 检测字符串结束标志
    {
        uart2_sendByte(*str++); // 发送当前字符
    }
    IE2 = 0x01;
}

void uart2_clearBuf(){
    memset(RX_buffer, '\0', BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0
}

/////////////////////////////////
/////////////////////////////////


// 串口1中断
void Uart1() interrupt 4
{
    ES = 0; // 关闭串口1中断
    if (RI) {
        RI                = 0; // 清除RI位
        RX_buffer[RX_num] = SBUF;
        RX_num++;
        if (RX_num > BUF_LENTH) RX_num = 0;
    }
    if (TI) {
        TI = 0; // 清除TI位
    }
    ES = 1; // 开启串口1中断
}

// 串口2中断
void Uart2() interrupt 8
{

    IE2 = 0x00; // 关闭串口2中断
    if (S2CON & S2RI) {
        S2CON &= ~S2RI;
        RX_buffer[RX_num] = S2BUF;
        RX_num++;
        if (RX_num > BUF_LENTH) RX_num = 0;
    }
    if (S2CON & S2TI) {
        S2CON &= ~S2TI;
    }
    IE2 = 0x01; // 开启串口2中断
}

