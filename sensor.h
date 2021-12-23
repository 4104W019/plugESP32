#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "Arduino.h"

#include <FunctionalInterrupt.h>
#include <ESP32AnalogRead.h>

int ledState();
void setLedState(int state);
float getADC0();
void gpioInit(int pin);

class Button
{
public:
  Button(uint8_t reqPin) : PIN(reqPin){
    pinMode(PIN, INPUT_PULLUP);
    attachInterrupt(PIN, std::bind(&Button::isr,this), FALLING);
    isTrigged = false;
  };
  ~Button() {
    detachInterrupt(PIN);
  }

  void IRAM_ATTR isr() {
    numberKeyPresses += 1;
    pressed = true;
  }

  void checkPressed() {
    
    if(restoreInterruptCnt>0){
      if(--restoreInterruptCnt <= 0){
        attachInterrupt(PIN, std::bind(&Button::isr,this), FALLING);
      }
      numberKeyPresses=0;
      pressed=false;
    }
    
    else{
      if (pressed) {
        //Serial.printf("Button on pin %u has been pressed %u times\n", PIN, numberKeyPresses);
        if(numberKeyPresses>3){
          isTrigged = true;
          detachInterrupt(PIN);
          restoreInterruptCnt = 12; //27+3sec
        }
        numberKeyPresses=0;
        pressed = false;
      }
    }
  }
  
  volatile bool isTrigged;
private:
  const uint8_t PIN;
  
  volatile int32_t restoreInterruptCnt;
    volatile uint32_t numberKeyPresses;
    volatile bool pressed;
};

#endif
