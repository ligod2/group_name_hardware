#include "Package.h"

// ����EEPROM�����
void IapIdle();
// EEPROM�ֽڶ�
unsigned char IapReadByte(unsigned int addr);
// EEPROM�ֽ�д
void IapProgramByte(unsigned int addr, unsigned char dat);
// EEPROM������������1��
void IapEraseSector(unsigned int addr);