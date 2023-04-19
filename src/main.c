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
    // ������𣬳�ʼ����ʼ
    P50 = 0;

    delay_init();  //ʱ��T0��ʼ��һֱ����
    uart_init(); // ���ô��ڳ�ʼ������
    adc_init();  // ADC��ʼ��

    // ����������ʼ����ʹ�ô���2��esp8266ͨ�ţ�����1�����ͨ�Ŵ�ӡ������Ϣ
    esp8266_init();
    esp8266_wifi();
    esp8266_mqtt_init();

    // ���Ϩ�𣬳�ʼ������
    P50 = 1;

    delay_ms(100);//�ӵ���ʱ����Ҫ�ɲ�Ҫ����ʱ����������Ͳ��ɹ�

    // ��ȡ���籣��ĵ����ݣ������͸����������ṩ��ǰ����ҳ��ʼ����ֵ��λ��
    threshold = IapReadByte(IAP_ADDRESS);   // ��ȡ�־û�������1�ĵ�һ���ֽ���Ϊ��������ֵ
    memset(format_buffer, '\0', BUF_LENTH); // �建������,��Ϊ��ʽ�����ַ����Ĵ洢λ��
    sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
    // �ø���䷢�͵���Ϣ���Ḳ�������topic��Ϣ�������豸���µ�¼Ҳ�ܻ�ȡ����Ϣ
    esp8266_mqtt_public_remain("led/cpu", format_buffer);
    // ��ӡ������Ϣ
    uart1_sendStr(format_buffer);

    // ������ѭ������ʽ���빤��ģʽ
    while (1) {
        // ����ģʽ���밴���������������������is_listen��T0����ж��и���
        if (is_listen == 1) {
            P50 = 0;
            while (1) {
                if (P16 == 0) {
                    // ����������ӵ���ʱ
                    delay_ms(600);
                    P40 = ~P40;
                }

                // �ٴΰ��°������˳�����ģʽ
                if (is_listen == 0) {
                    P50 = 1;
                    break;
                }
            }
        } else 
        {
            // ����ģʽ����������Զ�̵�����ֵ�����ֶ�ģʽ����������Զ�̿��صƣ�
            // ���ݽ��յ�����Ϣ���¸�����־λ
            if (strstr(RX_buffer, "LEDK") != NULL) // �յ��ֶ�ģʽ�µĿ�������
            {
                is_open = 1; // ����led
                uart2_clearBuf();
                uart1_sendStr("led is open!\r\n");

            } else if (strstr(RX_buffer, "LEDG") != NULL) // �յ��ֶ�ģʽ�µĹص�����
            {
                is_open = 0; // Ϩ��led
                uart2_clearBuf();
                uart1_sendStr("led is close!\r\n");

            } else if (strstr(RX_buffer, "threshold=[") != NULL) // �յ��ı���ֵ������
            {
                // ��ո�ʽ��������
                memset(format_buffer, '\0', FROMAT_BUF_LENTH);
                // ��ȡthreshold����ֵ
                threshold = getByteFromStr(strstr(RX_buffer, "threshold"));
                // �־û���ֵ
                IapEraseSector(IAP_ADDRESS);
                IapProgramByte(IAP_ADDRESS, threshold);

                // ����ǰ����ֵ
                memset(format_buffer, '\0', BUF_LENTH); // �建������,��Ϊ��ʽ�����ַ����Ĵ洢λ��
                sprintf(format_buffer, "threshold==[%s]", char_to_string(threshold));
                esp8266_mqtt_public_remain("led/cpu", format_buffer);
                uart1_sendStr(format_buffer);
                // ���ý�����
                uart2_clearBuf();

            } else if (strstr(RX_buffer, "model=[") != NULL) // �յ����Ŀ���ģʽ������
            {
                // ��ո�ʽ��������
                memset(format_buffer, '\0', FROMAT_BUF_LENTH);
                // ��ȡmodel����ֵ
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
            }

            // ���ݱ�־λ�ı�ƹ�
            if (model == 0) {
                // �Զ�ģʽ������ADCֵ����ֵ�Զ��ж��Ƿ񿪵ƣ�
                if (adc >= threshold) {
                    // ̫��,����
                    P40 = 0;
                } else {
                    // ̫�����ص�
                    P40 = 1;
                }
            } else if (model == 1) {
                // �ֶ�ģʽ������Զ�̷����ĵƿ��źſ��صƣ�
                if (is_open) {
                    P40 = 0;
                } else {
                    P40 = 1;
                }
            }

            // adc���ݲ���Ҫ���͵���ôƵ����ÿִ��20��ѭ������һ��adc
            if (count == 20) {
                count = 0;

                // ��������ADCֵ����ǰ���ṩ���߱仯������
                adc = GetADCResult(0);
                memset(format_buffer, '\0', BUF_LENTH); // �建������
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

//  char��ֵת��Ϊ��Ӧ��ʮ�����ַ���
char* char_to_string(unsigned char val)
{
    number[0] = val % 1000 / 100 + '0';
    number[1] = val % 100 / 10 + '0';
    number[2] = val % 10 + '0';
    return number;
}
