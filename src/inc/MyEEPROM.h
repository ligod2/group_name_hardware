#include "Package.h"

// ±ÜÃâEEPROMÎó²Ù×÷
void IapIdle();
// EEPROM×Ö½Ú¶Á
unsigned char IapReadByte(unsigned int addr);
// EEPROM×Ö½ÚĞ´
void IapProgramByte(unsigned int addr, unsigned char dat);
// EEPROMÉÈÇø²Á³ı£¨ÖÃ1£©
void IapEraseSector(unsigned int addr);