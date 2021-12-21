#include "Carduino.h"

Carduino carduino;

void setup() {

}

void loop() {
  carduino.simulateClock();
//  pattern1();
//  pattern2();
}

// Simply by providing other arrays, we can easily make the dots
// on the Cardano logo light up in different patterns.  This one
// lights up each dot by itself in sequence.  This also serves as
// a model for which bit controls which dot.
void pattern1() {
  const uint32_t pattern1array[] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                     dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                     dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                     dot25, dot26, dot28, dot30, dot29, dot27};

  for (int i=0 ; i<30; i++) {
    carduino.displayDots(pattern1array[i]);
    delay(100);
  }
}

// Concentric circles spiraling inwards
void pattern2() {
  const uint32_t pattern2array[] = { dot2, dot4, dot6, dot8, dot10, dot12, dot14, dot16, 
                                     dot18, dot20, dot22, dot24, dot1, dot3, dot5, dot7, 
                                     dot9, dot11, dot13, dot15, dot17, dot19, dot21, dot23,
                                     dot25, dot26, dot28, dot30, dot29, dot27};

  for (int i=0 ; i<30; i++) {
    carduino.displayDots(pattern2array[i]);
    delay(100);
  }
}
