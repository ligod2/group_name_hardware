#ifndef PROJECT_PACKAGE
#define PROJECT_PACKAGE

#define MAIN_Fosc    22118400UL                         // �����ⲿ�����Ƶ��
#define Baudrate0    115200UL                           // ���崮�ڲ�����Ƶ��
#define BRT_Reload   (256 - MAIN_Fosc / 16 / Baudrate0) // 1t����������� //BRT = 256- SYSclk*(2^SMOD/32)/������
#define TIMECOUNT_MS (65536 - MAIN_Fosc / 1000)         // ����ms����ļ�ʱ�������
#define BUF_LENTH 256   

#include <stdio.h>
#include <STRING.H>
#include "STC12C5A60S2.h"
#include "MyTime.h"
#include "MyUART.h"
#include "MyEsp8266.h"

extern unsigned char xdata RX_buffer[BUF_LENTH];
extern unsigned int RX_num; // ���ռ�������
#endif