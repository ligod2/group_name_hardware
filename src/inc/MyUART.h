#ifndef MYUART_H
#define MYUART_H

#include "Package.h"
                     // 定义串口接收缓冲长度

void uart_init(void);
void uart1_sendByte(unsigned char dat);
void uart1_sendStr(unsigned char *str);
void uart1_send(unsigned char *str ,unsigned char len);
void uart2_sendByte(unsigned char dat);
void uart2_sendStr(unsigned char *str);

#endif // !MYUART_H
