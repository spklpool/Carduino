#include "Carduino.h"

Carduino carduino;

void setup() {
}

void loop() {
  // this array defines the sequence in which the dots will be displayed
  const uint32_t pattern1array[] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                     dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                     dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                     dot25, dot26, dot28, dot30, dot29, dot27};

  for (int i=0 ; i<30; i++) { // iterates over the 30 elements in the array and displays each one in sequence
    carduino.displayDots(pattern1array[i]); // displays the pattern at the current index in the array
    delay(100); // pauses for 100 miliseconds before displaying the next dot
  }
}
