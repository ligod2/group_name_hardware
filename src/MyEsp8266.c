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
unsigned char code mqtt_user[] = "AT+MQTTUSERCFG=0,1,\"bm565bai4RLi\",\"mcpu\",\"123\",0,1,\"\"\r\n";
// unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"broker-cn.emqx.io\",1883,0\r\n";
unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"192.168.43.226\",1883,0\r\n";
unsigned char code mqtt_send[]         = "AT+MQTTPUB=0,\"led\/client\",\"led_sign\",1,0\r\n";

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
        delay_ms(600);
        if (strstr(RX_buffer, "OK") != NULL) {
            uart1_sendStr("OK,mcu connection success with ESP8266! \r\n");
            uart2_clearBuf();
            break;
        } else {
            uart1_sendStr("waiting for esp8266 to start... \r\n");
        }
    }
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
    uart2_clearBuf();

    // 配置wifi为多路可连接模式
    while (1) {
        uart2_sendStr(esp_cipmux); // 串口2对wifi模块 设置多连接 （多路连接模式）
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR3,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    uart2_clearBuf();

    // 配置wifi为服务器模式 配置端口号为5000
    while (1) {
        uart2_sendStr(esp_cipserver); // 串口2设置wifi模块 为TCP服务器模式，并配置端口为5000
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR4,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    uart2_clearBuf();

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
    uart2_clearBuf();
}

void esp8266_mqtt_init()
{
    // 断开已有的MQTT连接，否则设置账号和服务连接会出错
    uart2_sendStr("AT+MQTTCLEAN=0\r\n"); // 串口2对wifi模块发送握手指令 即AT
    delay_ms(1000);
    uart2_clearBuf();
    // 设置MQTT账号
    while (1) {
        uart2_sendStr(mqtt_user); // 串口2对wifi模块发送握手指令 即AT
        delay_ms(600);
        if (strstr(RX_buffer, "OK") != NULL) {
            uart1_sendStr("OK,the mqtt account information is set successfully! \r\n");
            break;
        } else {
            uart1_sendStr("network not connected! \r\n");
            uart1_sendStr(RX_buffer);
            uart1_sendStr("\r\n");
        }
        uart2_clearBuf();
    }
    // MQTT服务器连接(第一次返回OK，多次连接错误)
    uart2_sendStr(mqtt_connect);
    delay_ms(1000);
    uart1_sendStr("connect to the mqtt server... \r\n");
    uart2_clearBuf();

    // 订阅主题
    while (1) {
        uart2_sendStr("AT+MQTTSUB=0,\"led\/client\",0\r\n"); 
        uart2_sendStr("AT+MQTTSUB=0,\"led\/controller\",0\r\n"); 
        uart2_sendStr("AT+MQTTSUB=0,\"led\/data\",0\r\n"); 
        delay_ms(600);
        if (strstr(RX_buffer, "OK") != NULL) {
            uart1_sendStr("OK,The mqtt server is connected! \r\n");
            uart1_sendStr("OK,Topic subscription completed! \r\n");
            break;
        } else {
            uart1_sendStr("network not connected! \r\n");
            uart1_sendStr(RX_buffer);
            uart1_sendStr("\r\n");
        }
        uart2_clearBuf();
    }

    // 单片机登录成功，发送信息到led/client
    uart2_sendStr(mqtt_send);
    delay_ms(1000);
    uart2_clearBuf();
    uart1_sendStr("mqtt init is finished\r\n");
}

void esp8266_mqtt_sendStr(unsigned char *topic, unsigned char *payload)
{
    uart2_sendStr("AT+MQTTPUB=0,\"");
    uart2_sendStr(topic);
    uart2_sendStr("\",\"");
    uart2_sendStr(payload);
    uart2_sendStr("\",1,0\r\n");
}

void on_receive_handle()
{
    if (is_containStr("LEDK")) // 点亮板上了的led2
    {
        ES  = 0;
        IE2 = 0x00;
        P40 = 0;                         // 点亮led
        uart2_clearBuf();
        uart1_sendStr("led is open！\r\n");
        ES  = 1;
        IE2 = 0x01;
    } else if (is_containStr("LEDG")) // 关闭板上了的led
    {
        ES  = 0;
        IE2 = 0x00;
        P40 = 1;                         // 熄灭led
        uart2_clearBuf();
        uart1_sendStr("led is close！\r\n");
        ES  = 1;
        IE2 = 0x01;
    }
}
