#ifndef PROJECT_PACKAGE
#define PROJECT_PACKAGE

#define MAIN_Fosc    22118400UL                         // 定义外部晶振的频率
#define Baudrate0    115200UL                           // 定义串口波特率频率
#define BRT_Reload   (256 - MAIN_Fosc / 16 / Baudrate0) // 1t波特率溢出数 //BRT = 256- SYSclk*(2^SMOD/32)/波特率
#define TIMECOUNT_MS (65536 - MAIN_Fosc / 1000)         // 设置ms级别的计时的溢出数
#define BUF_LENTH 256   

#include <stdio.h>
#include <STRING.H>
#include "STC12C5A60S2.h"
#include "MyTime.h"
#include "MyUART.h"
#include "MyEsp8266.h"

extern unsigned char xdata RX_buffer[BUF_LENTH];
extern unsigned int RX_num; // 接收计数变量
#endif