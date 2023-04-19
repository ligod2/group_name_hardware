#ifndef PROJECT_PACKAGE
#define PROJECT_PACKAGE

#define MAIN_Fosc    22118400UL                         // 定义外部晶振的频率
#define Baudrate0    115200UL                           // 定义串口波特率频率
#define BRT_Reload   (256 - MAIN_Fosc / 16 / Baudrate0) // 1t波特率溢出数 //BRT = 256- SYSclk*(2^SMOD/32)/波特率
#define TIMECOUNT_MS (65536 - MAIN_Fosc / 1000)         // 设置ms级别的计时的溢出数
#define BUF_LENTH    256
#define FROMAT_BUF_LENTH 200
#define IAP_ADDRESS 0x0000

/*定义EEPROM操作的等待时间*/
// #define ENABLE_IAP 0x80           //if SYSCLK<30MHz
#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
// #define ENABLE_IAP  0x82         //if SYSCLK<20MHz
// #define ENABLE_IAP 0x83           //if SYSCLK<12MHz
// #define ENABLE_IAP 0x84           //if SYSCLK<6MHz
// #define ENABLE_IAP 0x85           //if SYSCLK<3MHz
// #define ENABLE_IAP 0x86           //if SYSCLK<2MHz
// #define ENABLE_IAP 0x87           //if SYSCLK<1MHz

/*定义EERPOM操作的类型*/
#define CMD_IDLE    0               //无操作
#define CMD_READ    1               //字节读
#define CMD_PROGRAM 2               //字节写
#define CMD_ERASE   3               //扇区擦除

#include <stdio.h>
#include <STRING.H>
#include <stdlib.h>
#include <intrins.h>
#include "STC12C5A60S2.h"
#include "MyTime.h"
#include "MyUART.h"
#include "MyEsp8266.h"
#include "MyEEPROM.h"
#include "MyADC.h"

extern unsigned char xdata RX_buffer[BUF_LENTH];
extern unsigned char xdata format_buffer[FROMAT_BUF_LENTH];
extern unsigned int RX_num; // 接收计数变量
extern bit is_listen;
#endif