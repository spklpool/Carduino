#include "Carduino.h"

Carduino carduino;

void setup() {
}

void loop() {
  carduino.runClock(); // runs the epoch countdown timer at normal speed
//  carduino.runClock(HOURS_TO_SECONDS_MULTIPLIER*5); // runs the epoch countdown timer sped up to one dot per second
//  userDefinedPatternExample1();  // example of how you can define your own patterns
//  carduino.inwardSpiral(); // displays a spiral of single dots going inwards
//  carduino.displayDots(dot1 & dot2); delay(1000); carduino.displayDots(dot13 & dot14); delay(1000); // try it out to see what this does
//  carduino.showErrorState(); // displays a pattern that would be shown if there is an unexpected error
}

// Simply by providing other arrays, we can easily make the dots
// on the Cardano logo light up in different patterns.  This one
// lights up each dot by itself in sequence.  Make your own pattern 
// by moving the dots around in the array and trying it out.
void userDefinedPatternExample1() {
  const uint32_t pattern1array[] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                     dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                     dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                     dot25, dot26, dot28, dot30, dot29, dot27};

  for (int i=0 ; i<30; i++) { // iterates over the 30 elements in the array and displays each one in sequence
    carduino.displayDots(pattern1array[i]); // displays the pattern at the current index in the array
    delay(100); // pauses for 100 miliseconds
  }
}
