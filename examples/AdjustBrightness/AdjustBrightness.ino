#include "Carduino.h"

Carduino carduino;

void setup() {
}

void loop() {
  // gradually increase brightness over 20*255 ms (5.1s)
  for (int i=0 ; i<=255; i++) { // brightness goes from 0 (lowest) to 255 (brightest)
    carduino.setBrightness(i);
    carduino.displayDots(dot25 & dot26 & dot27 & dot28 & dot29 & dot30);  // the 6 big middle dots
    delay(20); // pauses for 10 miliseconds before displaying the next dot
  }
  // gradually reduce brightness over 20*255 ms (5.1s)
  for (int i=255 ; i>=0; i--) { // brightness goes from 0 (lowest) to 255 (brightest)
    carduino.setBrightness(i);
    carduino.displayDots(dot25 & dot26 & dot27 & dot28 & dot29 & dot30);  // the 6 big middle dots
    delay(20); // pauses for 10 miliseconds before displaying the next dot
  }
}
