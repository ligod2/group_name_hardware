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
unsigned char code mqtt_user[]         = "AT+MQTTUSERCFG=0,1,\"bm565bai4RLi\",\"username\",\"123\",0,0,\"\"\r\n";
unsigned char code mqtt_connect[]      = "AT+MQTTCONN=0,\"broker-cn.emqx.io\",1883,0\r\n";
unsigned char code mqtt_subscription[] = "AT+MQTTSUB=0,\"2913064141\",0\r\n";
unsigned char code mqtt_send[]         = "AT+MQTTPUB=0,\"2913064141\",\"hello\",1,0\r\n";

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
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR1,esp8266 start... \r\n");
        delay_ms(600);
    }
    uart1_sendStr("OK,mcu connection success with ESP8266! \r\n");
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0
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
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

    // ����wifiΪ��·������ģʽ
    while (1) {
        uart2_sendStr(esp_cipmux); // ����2��wifiģ�� ���ö����� ����·����ģʽ��
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR3,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

    // ����wifiΪ������ģʽ ���ö˿ں�Ϊ5000
    while (1) {
        uart2_sendStr(esp_cipserver); // ����2����wifiģ�� ΪTCP������ģʽ�������ö˿�Ϊ5000
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR4,some problems with ESP8266 \r\n");
        delay_ms(600);
    }
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

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
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0
}

void mqtt_init()
{
    // ����MQTT�˺�
    while (1) {
        uart2_sendStr(mqtt_user); // ����2��ѯwifiģ�� ��ǰip��ַ
        if (is_containStr("OK"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT11111 \r\n");
        delay_ms(3000);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

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
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

    while (1) {
        uart2_sendStr(mqtt_subscription);
        if (is_containStr("OK") || is_containStr("ogv") || is_containStr("SUBRECV"))
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT \r\n");
        delay_ms(600);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

    while (1) {
        uart2_sendStr(mqtt_send);
        if (is_containStr("OK") )
            break;
        else
            uart1_sendStr("ERROR5,some problems with MQTT \r\n");
        delay_ms(600);
    }
    uart1_sendStr(RX_buffer);
    memset(RX_buffer, 0, BUF_LENTH); // �建������
    RX_num = 0;                      // ���ռ���������0

    P40 = 0;
}

void on_receive_handle()
{
    if (is_containStr("LEDK")) // ���������˵�led2
    {
        ES  = 0;
        IE2 = 0x00;
        P50 = 0;                         // ����led
        memset(RX_buffer, 0, BUF_LENTH); // �建������
        RX_num = 0;                      // ���ռ�������?0
        uart1_sendStr("led is open��\r\n");
        ES  = 1;
        IE2 = 0x01;
    } else if (is_containStr("LEDG")) // �رհ����˵�led
    {
        ES  = 0;
        IE2 = 0x00;
        P50 = 1;                         // Ϩ��led
        memset(RX_buffer, 0, BUF_LENTH); // �建������
        RX_num = 0;                      // ���ռ�������?0
        uart1_sendStr("led is close��\r\n");
        ES  = 1;
        IE2 = 0x01;
    }
}
