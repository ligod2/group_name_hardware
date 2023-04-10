#include "MyEsp8266.h"

// 本地联网设置
unsigned char code esp_at[]        = "AT\r\n";                  // 握手连接指令，返回"OK"
unsigned char code esp_cwmode[]    = "AT+CWMODE=3\r\n";         // 设置ESP8266的工作模式3 AP+Station，返回"OK"或者"no change"
unsigned char code esp_cifsr[]     = "AT+CIFSR\r\n";            // 本机IP地址查询指令
unsigned char code esp_cipsend[]   = "AT+CIPSEND=6\r\n";        // 设置发送数据长度
unsigned char code esp_test[]      = "sunny\r\n";               // 数据内容
unsigned char code esp_cipserver[] = "AT+CIPSERVER=1,5000\r\n"; // 建立TCP服务器，开放端口5000
unsigned char code esp_cipmux[]    = "AT+CIPMUX=1\r\n";         // 打开多连接
unsigned char code esp_rst[]       = "AT+RST\r\n";              // 软件复位
// MQTT服务器联网设置
unsigned char code mqtt_user[]         = "AT+MQTTUSERCFG=0,1,\"bm565bai4RLi\",\"username\",\"123\",0,0,\"\"\r\n";
unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"broker-cn.emqx.io\",1883,0\r\n";
unsigned char code mqtt_subscription[] = "AT+MQTTSUB=0,\"2913064141\",0\r\n";
unsigned char code mqtt_send[]         = "AT+MQTTPUB=0,\"2913064141\",\"hello\",1,0\r\n";

// 指定字符串与缓存数组数据进行数据比较
//*p 要比较的指定字符串指针数据
// 返回：1 数据一致  0 数据不一致
char is_containStr(unsigned char *p)
{
    if (strstr(RX_buffer, p) != NULL) {
        return 1;
    } else {
        return 0;
    }
}

void esp8266_init()
{
    // 发送AT 进行握手
    while (1) {
        uart2_sendStr(esp_at); // 串口2对wifi模块发送握手指令 即AT
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR1,esp8266 start... \r\n");
        delay_ms(600);
    }
    uart1_sendStr("OK,mcu connection success with ESP8266! \r\n");
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0
}

void esp8266_wifi()
{
    // 配置wifi工作模式为ap+sta模式
    while (1) {
        uart2_sendStr(esp_cwmode); // 串口2对wifi模块工作模式进行设置
        if (is_containStr("OK") || is_containStr("no change")) {
            break;
        } else {
            uart1_sendStr("ERROR2,some problems with ESP8266 \r\n");
        }
        delay_ms(600);
    }
    uart1_sendStr("OK,set mode as AP+Station with ESP8266! \r\n");
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    // 配置wifi为多路可连接模式
    while (1) {
        uart2_sendStr(esp_cipmux); // 串口2对wifi模块 设置多连接 （多路连接模式）
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR3,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    // 配置wifi为服务器模式 配置端口号为5000
    while (1) {
        uart2_sendStr(esp_cipserver); // 串口2设置wifi模块 为TCP服务器模式，并配置端口为5000
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR4,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    // 查询模块ip
    while (1) {
        uart2_sendStr(esp_cifsr); // 串口2查询wifi模块 当前ip地址
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0
}

void mqtt_init()
{
    // 设置MQTT账号
    while (1) {
        uart2_sendStr(mqtt_user); // 串口2查询wifi模块 当前ip地址
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT11111 \r\n");
        delay_ms(3000);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    while (1) {
        uart2_sendStr(mqtt_connect);
        if (is_containStr("OK") || is_containStr("+MQTTCONNECTED") || is_containStr("SUBRECV"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT222222 \r\n");
        delay_ms(10000);
        uart1_sendStr(RX_buffer);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    while (1) {
        uart2_sendStr(mqtt_subscription);
        if (is_containStr("OK") || is_containStr("ogv") || is_containStr("SUBRECV"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT \r\n");
        delay_ms(600);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    while (1) {
        uart2_sendStr(mqtt_send);
        if (is_containStr("OK") )
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT \r\n");
        delay_ms(600);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
    RX_num = 0;                      // 接收计数变量清0

    P40 = 0;
}

void on_receive_handle()
{
    if (is_containStr("LEDK")) // 点亮板上了的led2
    {
        ES  = 0;
        IE2 = 0x00;
        P50 = 0;                         // 点亮led
        memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
        RX_num = 0;                      // 接收计数变量?0
        uart1_sendStr("led is open！\r\n");
        ES  = 1;
        IE2 = 0x01;
    } else if (is_containStr("LEDG")) // 关闭板上了的led
    {
        ES  = 0;
        IE2 = 0x00;
        P50 = 1;                         // 熄灭led
        memset(RX_buffer, 0, BUF_LENTH); // 清缓存数据
        RX_num = 0;                      // 接收计数变量?0
        uart1_sendStr("led is close！\r\n");
        ES  = 1;
        IE2 = 0x01;
    }
}
