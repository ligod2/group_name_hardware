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



void main(void)
{
    delay_init();
    uart_init(); // 调用串口初始化函数

    esp8266_init();
    esp8266_wifi();
    mqtt_init();
    while (1) {
        on_receive_handle();
    }
}
