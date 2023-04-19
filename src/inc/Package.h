#ifndef PROJECT_PACKAGE
#define PROJECT_PACKAGE

#define MAIN_Fosc    22118400UL                         // �����ⲿ�����Ƶ��
#define Baudrate0    115200UL                           // ���崮�ڲ�����Ƶ��
#define BRT_Reload   (256 - MAIN_Fosc / 16 / Baudrate0) // 1t����������� //BRT = 256- SYSclk*(2^SMOD/32)/������
#define TIMECOUNT_MS (65536 - MAIN_Fosc / 1000)         // ����ms����ļ�ʱ�������
#define BUF_LENTH    256
#define FROMAT_BUF_LENTH 200
#define IAP_ADDRESS 0x0000

/*����EEPROM�����ĵȴ�ʱ��*/
// #define ENABLE_IAP 0x80           //if SYSCLK<30MHz
#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
// #define ENABLE_IAP  0x82         //if SYSCLK<20MHz
// #define ENABLE_IAP 0x83           //if SYSCLK<12MHz
// #define ENABLE_IAP 0x84           //if SYSCLK<6MHz
// #define ENABLE_IAP 0x85           //if SYSCLK<3MHz
// #define ENABLE_IAP 0x86           //if SYSCLK<2MHz
// #define ENABLE_IAP 0x87           //if SYSCLK<1MHz

/*����EERPOM����������*/
#define CMD_IDLE    0               //�޲���
#define CMD_READ    1               //�ֽڶ�
#define CMD_PROGRAM 2               //�ֽ�д
#define CMD_ERASE   3               //��������

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
extern unsigned int RX_num; // ���ռ�������
extern bit is_listen;
#endif