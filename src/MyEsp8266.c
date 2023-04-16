#include "MyEsp8266.h"

// ������������
unsigned char code esp_at[]        = "AT\r\n";                  // ��������ָ�����"OK"
unsigned char code esp_cwmode[]    = "AT+CWMODE=3\r\n";         // ����ESP8266�Ĺ���ģʽ3 AP+Station������"OK"����"no change"
unsigned char code esp_cifsr[]     = "AT+CIFSR\r\n";            // ����IP��ַ��ѯָ��
unsigned char code esp_cipsend[]   = "AT+CIPSEND=6\r\n";        // ���÷������ݳ���
unsigned char code esp_test[]      = "sunny\r\n";               // ��������
unsigned char code esp_cipserver[] = "AT+CIPSERVER=1,5000\r\n"; // ����TCP�����������Ŷ˿�5000
unsigned char code esp_cipmux[]    = "AT+CIPMUX=1\r\n";         // �򿪶�����
unsigned char code esp_rst[]       = "AT+RST\r\n";              // �����λ
// MQTT��������������
unsigned char code mqtt_user[] = "AT+MQTTUSERCFG=0,1,\"bm565bai4RLi\",\"mcpu\",\"123\",0,1,\"\"\r\n";
// unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"broker-cn.emqx.io\",1883,0\r\n";
unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"192.168.43.226\",1883,0\r\n";
unsigned char code mqtt_send[]         = "AT+MQTTPUB=0,\"led\/client\",\"led_sign\",1,0\r\n";

// ָ���ַ����뻺���������ݽ������ݱȽ�
//*p Ҫ�Ƚϵ�ָ���ַ���ָ������
// ���أ�1 ����һ��  0 ���ݲ�һ��
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
    // ����AT ��������
    while (1) {
        uart2_sendStr(esp_at); // ����2��wifiģ�鷢������ָ�� ��AT
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

    // ����wifi����ģʽΪap+staģʽ
    while (1) {
        uart2_sendStr(esp_cwmode); // ����2��wifiģ�鹤��ģʽ��������
        if (is_containStr("OK") || is_containStr("no change")) {
            break;
        } else {
            uart1_sendStr("ERROR2,some problems with ESP8266 \r\n");
        }
        delay_ms(600);
    }
    uart1_sendStr("OK,set mode as AP+Station with ESP8266! \r\n");
    uart2_clearBuf();

    // ����wifiΪ��·������ģʽ
    while (1) {
        uart2_sendStr(esp_cipmux); // ����2��wifiģ�� ���ö����� ����·����ģʽ��
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR3,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    uart2_clearBuf();

    // ����wifiΪ������ģʽ ���ö˿ں�Ϊ5000
    while (1) {
        uart2_sendStr(esp_cipserver); // ����2����wifiģ�� ΪTCP������ģʽ�������ö˿�Ϊ5000
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR4,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    uart2_clearBuf();

    // ��ѯģ��ip
    while (1) {
        uart2_sendStr(esp_cifsr); // ����2��ѯwifiģ�� ��ǰip��ַ
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
    // �Ͽ����е�MQTT���ӣ����������˺źͷ������ӻ����
    uart2_sendStr("AT+MQTTCLEAN=0\r\n"); // ����2��wifiģ�鷢������ָ�� ��AT
    delay_ms(1000);
    uart2_clearBuf();
    // ����MQTT�˺�
    while (1) {
        uart2_sendStr(mqtt_user); // ����2��wifiģ�鷢������ָ�� ��AT
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
    // MQTT����������(��һ�η���OK��������Ӵ���)
    uart2_sendStr(mqtt_connect);
    delay_ms(1000);
    uart1_sendStr("connect to the mqtt server... \r\n");
    uart2_clearBuf();

    // ��������
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

    // ��Ƭ����¼�ɹ���������Ϣ��led/client
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
    if (is_containStr("LEDK")) // ���������˵�led2
    {
        ES  = 0;
        IE2 = 0x00;
        P40 = 0;                         // ����led
        uart2_clearBuf();
        uart1_sendStr("led is open��\r\n");
        ES  = 1;
        IE2 = 0x01;
    } else if (is_containStr("LEDG")) // �رհ����˵�led
    {
        ES  = 0;
        IE2 = 0x00;
        P40 = 1;                         // Ϩ��led
        uart2_clearBuf();
        uart1_sendStr("led is close��\r\n");
        ES  = 1;
        IE2 = 0x01;
    }
}
