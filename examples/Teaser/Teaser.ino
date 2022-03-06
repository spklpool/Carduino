#include "Carduino.h"

Carduino carduino;

void setup() {
}

void loop() {
//  The teaser pattern can also be done more simply with just the following line.  
//  carduino.teaser();
//  To serve as a better example, below is the entire sequence in more explicit steps.

  carduino.accelleratingClock();
  carduino.reverseClock();
  carduino.fireworks1();
  carduino.sequence4();
  carduino.fireworks2();
  carduino.fireworks2();
  carduino.sequence2(50);
  carduino.sequence3(50);
  carduino.sequence2(50);
  carduino.full();
  carduino.fadeout();
}
