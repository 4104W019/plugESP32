#include "smartPlug.h"

/*========================
 * Arduino Framework:
 * void setup()
 * void loop()
========================*/

void setup() {
  smartPlugInit();
}

void loop() {
  smartPlugLoop();
  delay(50);
}
