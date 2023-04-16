#include "Package.h"

char is_containStr(unsigned char *p);
void esp8266_init();
void esp8266_wifi();
void esp8266_mqtt_init();
void esp8266_mqtt_sendStr(unsigned char *topic, unsigned char *payload);