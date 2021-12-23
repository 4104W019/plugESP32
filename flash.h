#ifndef _FLASH_
#define _FLASH_

#include "Arduino.h"
#include "EEPROM.h"


struct DevCfg{
  uint8_t num;  //
  uint8_t refreshTimeval;  // [0] 0:manual, 1:pre 1sec,...,
  uint8_t powerMode;       // [2] 0:Off, 1:On, 2:Auto
  uint8_t PirSense;        // [3] 0:Off, 1:Hi 2:Middle .... 10:Low 
  uint16_t darkThreshold;  // [110]
  uint16_t lightThreshold; // [180]
  uint8_t enableLineBot;
  int8_t reverse[23];
  
  char devName[16];   // WenhaoIoT
  char devPasswd[16]; // 11111111
  char ssid[32];      // Wenhao
  char ssidPasswd[32]; // 11111111 TBD: 64-->32
  char lineToken[128];
};

int initFlash();
void writeFlashDefault();
DevCfg readCfg();
void writeCfg(DevCfg cfg);

#endif
