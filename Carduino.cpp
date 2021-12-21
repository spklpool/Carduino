#include "Arduino.h"
#include "Carduino.h"

Carduino::Carduino() {
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
}

void Carduino::clockpattern() {
  const uint32_t middle = dot25 & dot26 & dot27 & dot28 & dot29 & dot30;
  uint32_t clockarray[] = { dot1, dot2, dot3, dot4,
                                  dot5, dot6, dot7, dot8,
                                  dot9, dot10, dot11, dot12,
                                  dot13, dot14, dot15, dot16,
                                  dot17, dot18, dot19, dot20,
                                  dot21, dot22, dot23, dot24};

  for (int i=0; i<24; i++) {
    if (i>0) {
      clockarray[i] = clockarray[i] & clockarray[i-1] & middle;
    } 
  }
  for (int i=0 ; i<24; i++) {
    displayDots(clockarray[i] & middle);
    delay(100);
  }
}

// Shift out the values of all the pins serially to the 
// 74HCS596 shift registers.  These chips latch on rising
// edge so they don't work as easily with the built in
// Arduino shiftOut that defaults to latching on falling
// edge.  It is possible to make it work with rising edge
// by setting the clock pin to HIGH before calling, but if
// I need to do that every time, I'd rather have my own 
// function that defaults to the way my chip works.
void Carduino::displayDots(uint32_t data) {
  // prepare the latch pin to latch on rising edge once all bits 
  // are shifted in.
  digitalWrite(latchPin, LOW);
  for (byte counter=0; counter<CARDUINO_LED_COUNT; counter++) {
    digitalWrite(clockPin, LOW);
    // write the current to the data pin
    digitalWrite(dataPin, getValueAtIndex(data, counter));
    // Clock in the current bit on the data pin, but don't
    // send it to the output pin yet.  That will be done once
    // all bits have been shifted in for all 4 shift registers.
    digitalWrite(clockPin, HIGH);
  }
  // latch the whole thing on all 4 shift registers at the same time
  digitalWrite(latchPin, HIGH);
}

// Translates the bit (0 or 1) at the given index in the given
// data to HIGH (1) or LOW (0). 
uint8_t Carduino::getValueAtIndex(uint32_t data, byte index) {
  if (bitRead(data, index) == 0) {
    return(LOW);
  } else {
    return(HIGH);
  }
}
