#include "smartPlug.h"

void parseCmd(String cmd)
{
  char sp[5][8]={};
  int i=0,j=0, idx=0;
  const char *strCmd = cmd.c_str();
  for(i=0;i<cmd.length();i++)
  {
    if(strCmd[i]==' ')
    {
      j=0;
      idx++;
      if(idx>=5)
        break;
    }
    else{
      if(j<8){
        sp[idx][j++]=strCmd[i];
      }
    } 
  }
  for(i=0;i<=idx;i++)
  {
    Serial.println(sp[i]);
  }
}

/*========================
 * Arduino Framework:
 * void setup()
 * void loop()
========================*/
String myString;
bool hasData=false;
void setup() {
  smartPlugInit();
}

void loop() {
  smartPlugLoop();
//  if(Serial.available()){
//        myString = Serial.readString();
//        parseCmd(myString);
//        //Serial.println(myString);
//    }
    while (Serial.available()) {
        char c = Serial.read();
        if(c!='\n'){
            myString += c;
        }
        delay(5);    // 沒有延遲的話 UART 串口速度會跟不上Arduino的速度，會導致資料不完整
    }
    if(myString.length()>0)
    {
      parseCmd(myString);
      myString="";
    }
  delay(50);
}
