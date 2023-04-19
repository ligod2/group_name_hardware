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
unsigned int count            = 0;

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

    delay_init();  //时钟T0开始，一直不关
    uart_init(); // 调用串口初始化函数
    adc_init();  // ADC初始化

    // 连接外网初始化，使用串口2与esp8266通信，串口1与电脑通信打印调试信息
    esp8266_init();
    esp8266_wifi();
    esp8266_mqtt_init();

    // 红灯熄灭，初始化结束
    P50 = 1;

    delay_ms(100);//加点延时，可要可不要，有时下面这个发送不成功

    // 获取掉电保存的的数据，并发送给服务器，提供给前端网页初始化阈值的位置
    threshold = IapReadByte(IAP_ADDRESS);   // 获取持久化在扇区1的第一个字节作为光敏的阈值
    memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据,作为格式化后字符串的存储位置
    sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
    // 用该语句发送的信息，会覆盖主题的topic信息，掉线设备重新登录也能获取该信息
    esp8266_mqtt_public_remain("led/cpu", format_buffer);
    // 打印调试信息
    uart1_sendStr(format_buffer);

    // 进入死循环，正式进入工作模式
    while (1) {
        // 声控模式，须按按键键开启（红灯亮），is_listen在T0溢出中断中更新
        if (is_listen == 1) {
            P50 = 0;
            while (1) {
                if (P16 == 0) {
                    // 消除抖动添加的延时
                    delay_ms(600);
                    P40 = ~P40;
                }

                // 再次按下按键，退出声控模式
                if (is_listen == 0) {
                    P50 = 1;
                    break;
                }
            }
        } else 
        {
            // 光敏模式（可以网络远程调节阈值）与手动模式（可以网络远程开关灯）
            // 根据接收到的信息更新各个标志位
            if (strstr(RX_buffer, "LEDK") != NULL) // 收到手动模式下的开灯命令
            {
                is_open = 1; // 点亮led
                uart2_clearBuf();
                uart1_sendStr("led is open!\r\n");

            } else if (strstr(RX_buffer, "LEDG") != NULL) // 收到手动模式下的关灯命令
            {
                is_open = 0; // 熄灭led
                uart2_clearBuf();
                uart1_sendStr("led is close!\r\n");

            } else if (strstr(RX_buffer, "threshold=[") != NULL) // 收到改变阈值的命令
            {
                // 清空格式化缓冲区
                memset(format_buffer, '\0', FROMAT_BUF_LENTH);
                // 获取threshold的数值
                threshold = getByteFromStr(strstr(RX_buffer, "threshold"));
                // 持久化阈值
                IapEraseSector(IAP_ADDRESS);
                IapProgramByte(IAP_ADDRESS, threshold);

                // 更新前端阈值
                memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据,作为格式化后字符串的存储位置
                sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
                esp8266_mqtt_public_remain("led/cpu", format_buffer);
                uart1_sendStr(format_buffer);
                // 重置接收区
                uart2_clearBuf();

            } else if (strstr(RX_buffer, "model=[") != NULL) // 收到更改控制模式的命令
            {
                // 清空格式化缓冲区
                memset(format_buffer, '\0', FROMAT_BUF_LENTH);
                // 获取model的数值
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
            }

            // 根据标志位改变灯光
            if (model == 0) {
                // 自动模式（根据ADC值和阈值自动判断是否开灯）
                if (adc >= threshold) {
                    // 太暗,开灯
                    P40 = 0;
                } else {
                    // 太亮，关灯
                    P40 = 1;
                }
            } else if (model == 1) {
                // 手动模式（根据远程发来的灯控信号开关灯）
                if (is_open) {
                    P40 = 0;
                } else {
                    P40 = 1;
                }
            }

            // adc数据不需要发送的那么频繁，每执行20次循环发送一次adc
            if (count == 20) {
                count = 0;

                // 持续发送ADC值，向前端提供光线变化的数据
                adc = GetADCResult(0);
                memset(format_buffer, '\0', BUF_LENTH); // 清缓存数据
                sprintf(format_buffer, "adc==[%s]", char_to_string(adc));
                esp8266_mqtt_sendStr("led/cpu", format_buffer);

                if (is_open) {
                    esp8266_mqtt_sendStr("led/cpu", "LEDK");
                } else {
                    esp8266_mqtt_sendStr("led/cpu", "LEDG");
                }
            }

            count++;
        }
    }
}

//  char的值转换为对应的十进制字符串
char* char_to_string(unsigned char val)
{
    number[0] = val % 1000 / 100 + '0';
    number[1] = val % 100 / 10 + '0';
    number[2] = val % 10 + '0';
    return number;
}
