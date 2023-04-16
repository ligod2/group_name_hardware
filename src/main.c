/*
ESP8266 WiFi模块 TCP/IP通信实验（单片机主频：22.1184MHz）

串口1 其波特率为115200
串口2与 wifi模块通讯 其波特率为115200
ESP8266设置为AP+Station模式 server服务器模式 并产生一个wifi热点

说明:  1. 该程序将ESP8266设置为AP+STATION工作模式，并建立TCP 服务器，默认IP为192.168.4.1， 开放端口5000
       2. 测试手机打开WIFI后，搜索到ESP_***热点后，进行连接。
       3. 打开安装在手机里的网络TCP/IP调试助手，与TCP服务器192.168.4.1，端口5000，进行连接。
       4. 手机向TCP服务器发送控制数据
       目前程序可以测试 STC系统板上 P50口led灯
       发送“LEDK”后，将点亮板上红色LED，同时模块返回“led is open！”
       发送“LEDG”后，将熄灭板上红色LED，同时模块返回“led is close！”

       同时可以 通过串口1直接对led进行同样控制

*/
#include "Package.h"

unsigned char threshold;
unsigned char xdata format_buffer[FROMAT_BUF_LENTH];
unsigned char xdata number[4] = {'\0'};
unsigned char adc             = 0;
bit model                     = 0;
bit is_open                   = 0;

char *char_to_string(unsigned char val);
unsigned char getByteFromStr(char *str)
{
    char *str1         = strchr(str, '[') + 1;
    unsigned char temp = (unsigned char)atoi(str1);
    return temp;
}

void main(void)
{
    // 红灯亮起，初始化开始
    P50 = 0;

    delay_init();
    uart_init(); // 调用串口初始化函数
    adc_init();  // ADC初始化

    // 连接外网初始化，使用串口2与esp8266通信，串口1与电脑通信打印调试信息
    esp8266_init();
    esp8266_wifi();
    esp8266_mqtt_init();

    // 红灯熄灭，初始化结束
    P50 = 1;

    threshold = IapReadByte(IAP_ADDRESS);   // 获取持久化在扇区1的第一个字节作为光敏的阈值
    memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据,作为格式化后字符串的存储位置
    sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
    esp8266_mqtt_sendStr("led/client", format_buffer);
    uart1_sendStr(format_buffer);

    while (1) {
        if (model == 0) {
            // 自动模式（根据ADC值和阈值自动判断是否开灯）
            if (adc >= threshold) {
                // 太暗,开灯
                is_open = 1;
                P40     = 0;
            } else {
                // 太亮，关灯
                is_open = 0;
                P40     = 1;
            }

        } else {
            // 手动模式（根据远程发来的灯控信号开关灯）
            if (is_open) {
                P40 = 0;
            } else {
                P40 = 1;
            }
        }

        // 根据接收到的信息更新各个标志位，以供上述判断
        if (is_containStr("LEDK")) // 收到手动模式下的开灯命令
        {
            ES  = 0;
            IE2 = 0x00;

            is_open = 1; // 点亮led
            uart2_clearBuf();
            uart1_sendStr("led is open!\r\n");

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("LEDG")) // 收到手动模式下的关灯命令
        {
            ES  = 0;
            IE2 = 0x00;

            is_open = 0; // 熄灭led
            uart2_clearBuf();
            uart1_sendStr("led is close!\r\n");

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("threshold")) // 收到改变阈值的命令
        {
            ES  = 0;
            IE2 = 0x00;

            // 清空格式化缓冲区
            memset(format_buffer, '\0', FROMAT_BUF_LENTH);
            // 获取threshold的数值
            threshold = getByteFromStr(strstr(RX_buffer, "threshold"));
            uart1_sendStr("---");
            uart1_sendByte(threshold);
            uart1_sendStr("---");
            // 持久化阈值
            IapEraseSector(IAP_ADDRESS);
            IapProgramByte(IAP_ADDRESS, threshold);
            // 重置接收区
            uart2_clearBuf();

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("model")) // 收到更改控制模式的命令
        {
            ES  = 0;
            IE2 = 0x00;

            // 清空格式化缓冲区
            memset(format_buffer, '\0', FROMAT_BUF_LENTH);
            // 获取threshold的数值
            model = getByteFromStr(strstr(RX_buffer, "model"));
            // 重置接收区
            uart2_clearBuf();
            if (model == 1) {
                P50 = 0;
                uart1_sendStr("setModel: manual mode\r\n");
            } else if (model == 0) {
                P50 = 1;
                uart1_sendStr("setModel: automatic mode\r\n");
            }

            ES  = 1;
            IE2 = 0x01;
        }

        // 持续发送ADC值，向前端提供光线变化的数据
        adc = GetADCResult(0);
        memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据
        sprintf(format_buffer, "adc==[%s]", char_to_string(adc));
        esp8266_mqtt_sendStr("led/data", format_buffer);

        memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据,作为格式化后字符串的存储位置
        sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
        esp8266_mqtt_sendStr("led/client", format_buffer);

        if (is_open) {
            esp8266_mqtt_sendStr("led/controller", "LEDK");
        } else {
            esp8266_mqtt_sendStr("led/controller", "LEDG");
        }

        delay_ms(500);
    }
}

char *char_to_string(unsigned char val)
{
    number[0] = val % 1000 / 100 + '0';
    number[1] = val % 100 / 10 + '0';
    number[2] = val % 10 + '0';
    return number;
}
