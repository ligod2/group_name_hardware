#include "MyEEPROM.h"


void IapIdle()
{
    IAP_CONTR = 0;                  //Close IAP function
    IAP_CMD = 0;                    //Clear command to standby
    IAP_TRIG = 0;                   //Clear trigger register
    IAP_ADDRH = 0x80;               //Data ptr point to non-EEPROM area
    IAP_ADDRL = 0;                  //Clear IAP address to prevent misuse
}

unsigned char IapReadByte(unsigned int addr)
{
    unsigned char dat;                       //Data buffer

    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_READ;             //Set ISP/IAP/EEPROM READ command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x5a;                //Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;                //Send trigger command2 (0xa5)

    _nop_();
    dat = IAP_DATA;                 //Read ISP/IAP/EEPROM data
    _nop_();
    IapIdle();                      //Close ISP/IAP/EEPROM function

    return dat;                     //Return Flash data
}

void IapProgramByte(unsigned int addr, unsigned char dat)
{
    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_PROGRAM;          //Set ISP/IAP/EEPROM PROGRAM command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_DATA = dat;                 //Write ISP/IAP/EEPROM data
    IAP_TRIG = 0x5a;                //Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;                //Send trigger command2 (0xa5)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();

}

void IapEraseSector(unsigned int addr)
{
    IAP_CONTR = ENABLE_IAP;         //Open IAP function, and set wait time
    IAP_CMD = CMD_ERASE;            //Set ISP/IAP/EEPROM ERASE command
    IAP_ADDRL = addr;               //Set ISP/IAP/EEPROM address low
    IAP_ADDRH = addr >> 8;          //Set ISP/IAP/EEPROM address high
    IAP_TRIG = 0x5a;                //Send trigger command1 (0x5a)
    IAP_TRIG = 0xa5;                //Send trigger command2 (0xa5)
    _nop_();                        //MCU will hold here until ISP/IAP/EEPROM operation complete
    IapIdle();
}