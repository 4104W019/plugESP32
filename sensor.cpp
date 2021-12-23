#include "sensor.h"

ESP32AnalogRead adc;

const int led=2;
int led_state=0;

float getADC0()
{
  return adc.readVoltage();
}

void gpioInit(int pin)
{
  pinMode(led, OUTPUT);
  adc.attach(pin);
}

int ledState(){
  return led_state;
}
void setLedState(int state)
{
  led_state = state;
  digitalWrite(led, state);
}
