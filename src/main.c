/*
ESP8266 WiFiģ�� TCP/IPͨ��ʵ�飨��Ƭ����Ƶ��22.1184MHz��

����1 �䲨����Ϊ115200
����2�� wifiģ��ͨѶ �䲨����Ϊ115200
ESP8266����ΪAP+Stationģʽ server������ģʽ ������һ��wifi�ȵ�

˵��:  1. �ó���ESP8266����ΪAP+STATION����ģʽ��������TCP ��������Ĭ��IPΪ192.168.4.1�� ���Ŷ˿�5000
       2. �����ֻ���WIFI��������ESP_***�ȵ�󣬽������ӡ�
       3. �򿪰�װ���ֻ��������TCP/IP�������֣���TCP������192.168.4.1���˿�5000���������ӡ�
       4. �ֻ���TCP���������Ϳ�������
       Ŀǰ������Բ��� STCϵͳ���� P50��led��
       ���͡�LEDK���󣬽��������Ϻ�ɫLED��ͬʱģ�鷵�ء�led is open����
       ���͡�LEDG���󣬽�Ϩ����Ϻ�ɫLED��ͬʱģ�鷵�ء�led is close����

       ͬʱ���� ͨ������1ֱ�Ӷ�led����ͬ������

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
    // ������𣬳�ʼ����ʼ
    P50 = 0;

    delay_init();
    uart_init(); // ���ô��ڳ�ʼ������
    adc_init();  // ADC��ʼ��

    // ����������ʼ����ʹ�ô���2��esp8266ͨ�ţ�����1�����ͨ�Ŵ�ӡ������Ϣ
    esp8266_init();
    esp8266_wifi();
    esp8266_mqtt_init();

    // ���Ϩ�𣬳�ʼ������
    P50 = 1;

    threshold = IapReadByte(IAP_ADDRESS);   // ��ȡ�־û�������1�ĵ�һ���ֽ���Ϊ��������ֵ
    memset(format_buffer, '\0', BUF_LENTH); // �建������,��Ϊ��ʽ�����ַ����Ĵ洢λ��
    sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
    esp8266_mqtt_sendStr("led/client", format_buffer);
    uart1_sendStr(format_buffer);

    while (1) {
        if (model == 0) {
            // �Զ�ģʽ������ADCֵ����ֵ�Զ��ж��Ƿ񿪵ƣ�
            if (adc >= threshold) {
                // ̫��,����
                is_open = 1;
                P40     = 0;
            } else {
                // ̫�����ص�
                is_open = 0;
                P40     = 1;
            }

        } else {
            // �ֶ�ģʽ������Զ�̷����ĵƿ��źſ��صƣ�
            if (is_open) {
                P40 = 0;
            } else {
                P40 = 1;
            }
        }

        // ���ݽ��յ�����Ϣ���¸�����־λ���Թ������ж�
        if (is_containStr("LEDK")) // �յ��ֶ�ģʽ�µĿ�������
        {
            ES  = 0;
            IE2 = 0x00;

            is_open = 1; // ����led
            uart2_clearBuf();
            uart1_sendStr("led is open!\r\n");

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("LEDG")) // �յ��ֶ�ģʽ�µĹص�����
        {
            ES  = 0;
            IE2 = 0x00;

            is_open = 0; // Ϩ��led
            uart2_clearBuf();
            uart1_sendStr("led is close!\r\n");

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("threshold")) // �յ��ı���ֵ������
        {
            ES  = 0;
            IE2 = 0x00;

            // ��ո�ʽ��������
            memset(format_buffer, '\0', FROMAT_BUF_LENTH);
            // ��ȡthreshold����ֵ
            threshold = getByteFromStr(strstr(RX_buffer, "threshold"));
            uart1_sendStr("---");
            uart1_sendByte(threshold);
            uart1_sendStr("---");
            // �־û���ֵ
            IapEraseSector(IAP_ADDRESS);
            IapProgramByte(IAP_ADDRESS, threshold);
            // ���ý�����
            uart2_clearBuf();

            ES  = 1;
            IE2 = 0x01;
        } else if (is_containStr("model")) // �յ����Ŀ���ģʽ������
        {
            ES  = 0;
            IE2 = 0x00;

            // ��ո�ʽ��������
            memset(format_buffer, '\0', FROMAT_BUF_LENTH);
            // ��ȡthreshold����ֵ
            model = getByteFromStr(strstr(RX_buffer, "model"));
            // ���ý�����
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

        // ��������ADCֵ����ǰ���ṩ���߱仯������
        adc = GetADCResult(0);
        memset(format_buffer, '\0', BUF_LENTH); // �建������
        sprintf(format_buffer, "adc==[%s]", char_to_string(adc));
        esp8266_mqtt_sendStr("led/data", format_buffer);

        memset(format_buffer, '\0', BUF_LENTH); // �建������,��Ϊ��ʽ�����ַ����Ĵ洢λ��
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
