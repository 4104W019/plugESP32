#include "flash.h"
#define EEPROM_SIZE 512

int initFlash()
{
  return EEPROM.begin(EEPROM_SIZE);
}

void writeFlashDefault()
{
  DevCfg cfg={1,0,2,3,110,170, 0, "","IoT","11111111","Wenhao","11111111","1234321"};
  char *data_ = (char *)&cfg;
  data_[0] = 1;
  for(int i = 0 ; i < 256; i++)
  {
    EEPROM.write(i, data_[i]);
  }
  data_[0] = 2;
  Serial.print("\n\nStart Page2: ");Serial.println(data_[0]);
  for(int i = 0 ; i < 256; i++)
  {
    EEPROM.write(256+i, data_[i]);
  }
  EEPROM.commit();
  
  Serial.print(" 11 Page0 num: ");
  Serial.println(byte(EEPROM.read(0)));
  
  Serial.print(" 11 Page1 num: ");
  Serial.println(byte(EEPROM.read(256)));
}

DevCfg readCfg()
{
  DevCfg cfg={0};
  int offset=0;
  uint8_t page0 = EEPROM.read(0);
  uint8_t page1 = EEPROM.read(256);

  if(page0==0 && page1==0)
  {
    writeFlashDefault();
    page0 = 1;
    page1 = 2;
  }
  
  if(page0<page1){
    offset = 256;
  }
  EEPROM.get(offset,cfg);
//  char *data_ = (char *)&cfg;
//  for(int i = 0 ; i < 256; i++)
//  {
//    data_[i] = EEPROM.read(offset+i);
//  }

  return cfg;
}

void writeCfg(DevCfg cfg)
{
  int offset=0;
  uint8_t page0 = EEPROM.read(0);
  uint8_t page1 = EEPROM.read(256);
  
  if(page0 > page1) // 3, 2
  {
    offset = 256;
    if(page0 < 255)
      cfg.num = page0 + 1;  // page1 = 4
    else cfg.num = 2; 
  }
  else{
    if(page1 < 254)
      cfg.num = page1 + 1;  // page0 = 4
    else cfg.num = 1; 
  }
  EEPROM.put(offset,cfg);
  EEPROM.commit();

}
