#include "Carduino.h"

Carduino carduino;

void setup() {
}

void loop() {
  carduino.setClock(__DATE__, __TIME__);

  // The above line is all that is necessary.  Below is just to give a confirmation
  // and to avoid resetting the clock repeatedly in a never ending loop.
  delay(1000);
  carduino.fireworks1();
  delay(5000);
}
