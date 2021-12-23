#include "smartPlug.h"

#include "mesh.h"
#include "flash.h"
#include "sensor.h"
#include "webSvr.h"
#include "lineNotify.h"

/*=============================
 * Defines
 =============================*/
 
#define BUTTON1 22
#define BUTTON2 23

#define TIMEVAL_ADC0 20
#define TIMEVAL_PIR_ON  100 //300 // 15sec
#define TIMEVAL_WIFI 200  // 10sec


DevCfg cfg={0};
/*=============================
 * EXTERN Variables
 =============================*/
extern AsyncWebServer server;
float adc0Value=0;

/*=============================
 * Variables
 =============================*/
String homeHtml;
int chipId = 0;


int timeval_adc0 = 20; // 50ms/unit
int timeval_pir_on = 0; // 50ms/unit
int timeval_wifi_reconnect = TIMEVAL_WIFI; // 50ms/unit


Button button1(BUTTON1);
Button button2(BUTTON2);

/*=============================
 * Utilities
 =============================*/

void dumpCfg(DevCfg cfg)
{
    Serial.print("num ");Serial.println(cfg.num);
    Serial.print("refreshTimeval ");Serial.println(cfg.refreshTimeval);
    Serial.print("powerMode ");Serial.println(cfg.powerMode);
    Serial.print("PirSense ");Serial.println(cfg.PirSense);
    Serial.print("darkThreshold ");Serial.println(cfg.darkThreshold);
    Serial.print("lightThreshold ");Serial.println(cfg.lightThreshold);
    Serial.print("enableLineBot ");Serial.println(cfg.enableLineBot);
    
    Serial.println("devName " +String(cfg.devName));
    //String("Iot_001").toCharArray(cfg.devName,16);
    Serial.println("devPasswd " +String(cfg.devPasswd));
    //String("11111111").toCharArray(cfg.devPasswd,16);
    Serial.println("ssid " +String(cfg.ssid));
    Serial.println("ssidPasswd " +String(cfg.ssidPasswd));
    Serial.println("lineToken " +String(cfg.lineToken));
}

int getChipID()
{
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
}


/**=============================
 *  main operation flow.
 =============================*/
void doMultiTask()
{
  if(--timeval_adc0<=0)
  {
    timeval_adc0 = TIMEVAL_ADC0;
    adc0Value = getADC0();
    //Serial.println("Voltage = "+String(adc0Value));
    button1.checkPressed();
    button2.checkPressed();
  }
#if 1
  if(wanStatus()==false){
    if(button2.isTrigged){
      button2.isTrigged = false;
    }
    if(--timeval_wifi_reconnect<=0)
    {
      timeval_wifi_reconnect= TIMEVAL_WIFI;
    }
  }
  else{
    timeval_wifi_reconnect = TIMEVAL_WIFI;
  }
#endif
  if(cfg.powerMode == 2)
  {
    float dark = cfg.darkThreshold/100; 
    float light = cfg.lightThreshold/100;
    if(button1.isTrigged){
        timeval_pir_on = TIMEVAL_PIR_ON;
        button1.isTrigged = false;
        setLedState(1);
        sendMsgByLine("PIR Trigged, turn ON (x 15sec -->5sec)15sec.");
    }
    if(timeval_pir_on>0){
        if(--timeval_pir_on<=0)
        {
          setLedState(0);
          sendMsgByLine("PIR Trigged Finished, turn OFF the lights."); 
        }
    }
    else{
      if(adc0Value>light && ledState() == 1){
        setLedState(0);
         sendMsgByLine("get to LIGHT, turn OFF the lights.");
      }
      else if(adc0Value<dark && ledState() == 0){
        setLedState(1);
        sendMsgByLine("get to DARK, turn ON the lights.");
      }
    }
  } 
}



/*=============================
 * RESTinit
 =============================*/

void RESTinit()
{

  //=======================================
  server.on("/settingX", HTTP_POST, [](AsyncWebServerRequest *request){
    bool changeWiFi = false;
    
    if (request->hasParam("sysPasswd", true)) {
      String pass = request->getParam("sysPasswd", true)->value();
      if(pass!=String(cfg.devPasswd) && pass!=String("11111111")){
        return;
      }
    }
    else{
      return;
    }
    
    if (request->hasParam("dark", true)) {
      cfg.darkThreshold = request->getParam("dark", true)->value().toInt();
    } 
    if (request->hasParam("light", true)) {
      cfg.lightThreshold = request->getParam("light", true)->value().toInt();
    }
    if (request->hasParam("ssid", true)) {
      String ssid_ = request->getParam("ssid", true)->value();
      if(String(cfg.ssid)!=ssid_){
        ssid_.toCharArray(cfg.ssid,32);
        changeWiFi = true;
      }
    }
    if (request->hasParam("ssid_passwd", true)) {
      String ssid_passwd_ = request->getParam("ssid_passwd", true)->value();
      if(String(cfg.ssidPasswd)!=ssid_passwd_){
        ssid_passwd_.toCharArray(cfg.ssidPasswd,32);
        changeWiFi = true;
      }
      
    }
    if (request->hasParam("token", true)) {
      String lineToken = request->getParam("token", true)->value();
      lineToken.toCharArray(cfg.lineToken,128);
    }
    
    if(changeWiFi == true){
      //mesh.stop();
      //meshInit();
      //mesh.stationManual(ssid, ssid_passwd);
      //mesh.initStation();
      WiFi.reconnect();
    }
    
    
  String("Iot_001").toCharArray(cfg.devName,16);
  String("11111111").toCharArray(cfg.devPasswd,16);
    writeCfg(cfg);
    
    homeHtml = settingPage(String(cfg.ssid)
                ,String(cfg.ssidPasswd)
                ,String(cfg.lineToken)
                ,String(cfg.darkThreshold)
                ,String(cfg.lightThreshold));
    
    request->send(200, "text/html", homeHtml.c_str());
  });
  //=======================================
  
  //Async webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    homeHtml = sysInfoPage(getAccessPointIP().toString(), getlocalIP().toString(), chipId);
    request->send(200, "text/html", homeHtml.c_str());
  });
       
  server.on("/system", HTTP_GET, [](AsyncWebServerRequest *request){
    homeHtml = sysInfoPage(getAccessPointIP().toString(),getlocalIP().toString(), chipId);
    request->send(200, "text/html", homeHtml.c_str());
  });
  
  server.on("/powerMode", HTTP_GET, [](AsyncWebServerRequest *request){
    homeHtml = powerModePage(cfg.powerMode);
    request->send(200, "text/html", homeHtml.c_str());
  });  
  
  server.on("/setting", HTTP_GET, [](AsyncWebServerRequest *request){
    homeHtml = settingPage( String(cfg.ssid)
                ,String(cfg.ssidPasswd)
                ,String(cfg.lineToken)
                ,String(cfg.darkThreshold)
                ,String(cfg.lightThreshold));
    request->send(200, "text/html", homeHtml.c_str());
  });
  
  server.on("/device", HTTP_GET, [](AsyncWebServerRequest *request){
    homeHtml = devStatusPage(String(timeval_pir_on),String(adc0Value));
    request->send(200, "text/html", homeHtml.c_str());
  });
  
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request){
    cfg.powerMode = 2;
    sendMsgByLine("led auto");
    homeHtml = powerModePage(cfg.powerMode);
    request->send(200, "text/html", homeHtml.c_str());
  });
  
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    setLedState(1);
    cfg.powerMode = 1;
    sendMsgByLine("led on");
    homeHtml = powerModePage(cfg.powerMode);
    request->send(200, "text/html", homeHtml.c_str());
  });
  
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    setLedState(0);
    cfg.powerMode = 0;
    sendMsgByLine("led off");
    homeHtml = powerModePage(cfg.powerMode);
    request->send(200, "text/html", homeHtml.c_str());
  });
  
//===============
// testing API  
//===============
  server.on("/adc", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(adc0Value).c_str());
  });
  server.on("/lamp", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ledState()).c_str());
  });
/*
  server.on("/led_on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(led, 1);
    create_json("led",1, "On");
    request->send(200, "application/json", buffer);
  });
  
  server.on("/led_off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(led, 0);
    jsonDocument.clear(); // Clear json buffer
    //create_json("led",0, "Off");
    add_json_object("led-0",0, "Off");
    add_json_object("led-1",0, "Off");
    add_json_object("led-2",0, "Off");
    serializeJson(jsonDocument, buffer);
    
    request->send(200, "application/json", buffer);
  });
  
*/
  server.begin();
}

/*========================
 * Hardware Layer
========================*/
void systemInit()
{
  gpioInit(33);
  getChipID();
  
  Serial.begin(115200);
  if (!initFlash())
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  
  cfg=readCfg(); 
  if(cfg.powerMode>2)
    cfg.powerMode = 2;

  enableLineBot(cfg.enableLineBot);
  dumpCfg(cfg);
}

void smartPlugInit(){
  systemInit();
  meshInit(String(cfg.ssid), String(cfg.ssidPasswd));
  RESTinit();
}

void smartPlugLoop()
{
  if( checkNetworking() ){
    LineInit( getlocalIP().toString());
  }
  doMultiTask();
}
