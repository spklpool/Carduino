#include "Arduino.h"
#include "Carduino.h"

// functions copied from https://github.com/NorthernWidget/DS3231
// just took the minimal fuctionality we need for Carduino, which
// does not care about actual dates and times.  We just want a high
// precision number of seconds since a given start time to calculate
// epochs and fractions thereof.
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }
static const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };
bool isleapYear(const uint8_t y) {
  if(y&3)//check if divisible by 4
    return false;
  //only check other, when first failed
  return (y % 100 || y % 400 == 0);
}
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && isleapYear(y))
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}
static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}
byte Carduino::decToBcd(byte val) {
  return ( (val/10*16) + (val%10) );
}
byte Carduino::bcdToDec(byte val) {
  return ( (val/16*10) + (val%16) );
}
// end of copied functions

Carduino::Carduino() {
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Wire.begin();
#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void Carduino::runClock() {
  runClock(1, false);
}

void Carduino::runClock(bool middleEpochCounter) {
  runClock(1, middleEpochCounter);
}

void Carduino::runClock(uint32_t speedMultiplier) {
  runClock(speedMultiplier, false);
}

void Carduino::runClock(uint32_t speedMultiplier, bool middleEpochCounter) {
  uint32_t middle = dot25 & dot26 & dot28 & dot30 & dot29 & dot27;
  const int sizeofEpocharray = 7;
  uint32_t epocharray[sizeofEpocharray] = { dot0, dot25, dot26, dot28, dot30, dot29, dot27 };
  for (int i=0; i<sizeofEpocharray; i++) {
    if (i>0) {
      epocharray[i] = epocharray[i] & epocharray[i-1];
    }
  }
  
  const int sizeofClockarray = 30;
  uint32_t clockarray[sizeofClockarray] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                            dot25, dot26, dot27, dot28, dot29, dot30 };
  for (int i=0; i<sizeofClockarray; i++) {
    if (i>0) {
      clockarray[i] = clockarray[i] & clockarray[i-1];
    } 
  }
  
  long currentTime = getNowFromClock();
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    if (resetCycleCount > RESET_CYCLE_THRESHOLD) {
      fireworks2();
      resetClockToZero();
    } else if (advanceCycleCount > ADVANCE_CYCLE_THRESHOLD) {
      advanceClockByOneDot();
      advanceCycleCount = 0;
      resetCycleCount ++;
    } else {
      resetCycleCount ++;
      advanceCycleCount ++;
    }
  } else {
    resetCycleCount = 0;
    advanceCycleCount = 0;
  }

  long secondsElapsed = currentTime * speedMultiplier;
  dotsElapsed = secondsElapsed/SECONDS_IN_DOT;
  epochsElapsed = dotsElapsed/DOTS_IN_EPOCH;
  uint32_t currentEpochDots = dotsElapsed%DOTS_IN_EPOCH;

  if (middleEpochCounter) {
    displayDots(clockarray[currentEpochDots] & epocharray[epochsElapsed%sizeofEpocharray]);
  } else {
    displayDots(clockarray[currentEpochDots] & middle);
  }
  delay(100);
}

void Carduino::sequence1() {
  const int sizeofClockarray = 30;
  uint32_t clockarray[sizeofClockarray] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                            dot25, dot26, dot27, dot28, dot29, dot30 };
  for (int i=0; i<sizeofClockarray; i++) {
    displayDots(clockarray[i]);
    delay(300);
  }
}

void Carduino::fireworks1() {
  const int sizeofClockarray = 3;
  uint32_t clockarray[sizeofClockarray] = { dot25 & dot26 & dot27 & dot28 & dot29 & dot30,
                                            dot1 & dot3 & dot5 & dot7 & dot9 & dot11 & dot13 & dot15 & dot17 & dot19 & dot21 & dot23,
                                            dot2 & dot4 & dot6 & dot8 & dot10 & dot12 & dot14 & dot16 & dot18 & dot20 & dot22 & dot24
                                            };
  for (int i=0; i<sizeofClockarray; i++) {
    displayDots(clockarray[i]);
    delay(300);
  }
}

void Carduino::fireworks2() {
  const int sizeofClockarray = 3;
  uint32_t clockarray[sizeofClockarray] = { dot25 & dot26 & dot27 & dot28 & dot29 & dot30,
                                            dot1 & dot3 & dot5 & dot7 & dot9 & dot11 & dot13 & dot15 & dot17 & dot19 & dot21 & dot23,
                                            dot2 & dot4 & dot6 & dot8 & dot10 & dot12 & dot14 & dot16 & dot18 & dot20 & dot22 & dot24
                                            };
  for (int i = 0; i<3; i++) {
    for (int j=0; j<sizeofClockarray; j++) {
      displayDots(clockarray[j]);
      delay(100);
    }
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
  // prepare the latch pin to latch on rising edge
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

// Translates the bit (1 or 0) at the given index in the given
// data to HIGH (1) or LOW (0). 
uint8_t Carduino::getValueAtIndex(uint32_t data, byte index) {
  if (bitRead(data, index) == 0) {
    return(LOW);
  } else {
    return(HIGH);
  }
}

bool Carduino::isButtonPressed() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    return true;
  } else {
    return false;
  }
}

// read current date and time from DS3231 clock
// reference: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
long Carduino::getNowFromClock() {
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(CLOCK_ADDRESS, 7);
  uint8_t ss = bcd2bin(Wire.read() & 0x7F);
  uint8_t mm = bcd2bin(Wire.read());
  uint8_t hh = bcd2bin(Wire.read());
  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read());
  uint16_t days = date2days(y, m, d);
  long secondsElapsed = time2long(days, hh, mm, ss);
  
#ifdef DEBUG
  char dateTimeString[60];
  dotsElapsed = secondsElapsed/SECONDS_IN_DOT;
  epochsElapsed = dotsElapsed/DOTS_IN_EPOCH;
  uint32_t currentEpochDots = dotsElapsed%DOTS_IN_EPOCH;
  sprintf(dateTimeString,"%i %i %i: %i %i %i:epochs: %i dots: %i seconds: %i", d, m, y, hh, mm, ss, epochsElapsed, dotsElapsed, secondsElapsed);
  Serial.println(dateTimeString);
#endif 
  
  return secondsElapsed;
}

// reset DS3231 clock to 00:00:00 Jan 1st, 2000
// reference: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
void Carduino::resetClockToZero() {
  setClockToYMDHMS(0, 1, 1, 0, 0, 0);
}

void Carduino::advanceClockByOneDot() {
  setClockToSeconds(getNowFromClock() + SECONDS_IN_DOT);
}

// adapted from DateTime constructor from 
// https://github.com/NorthernWidget/DS3231
void Carduino::setClockToSeconds(long timeInSeconds) {
  uint8_t m = 0;
  uint8_t ss = timeInSeconds % 60;
  timeInSeconds /= 60;
  uint8_t mm = timeInSeconds % 60;
  timeInSeconds /= 60;
  uint8_t hh = timeInSeconds % 24;
  uint16_t days = timeInSeconds / 24;
  uint8_t leap;
  for (uint8_t yOff = 0; ; ++yOff) {
    leap = isleapYear(yOff);
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (m = 1; ; ++m) {
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
    if (leap && m == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  uint8_t d = days + 1;
  uint8_t y = days / 365;
  d = d % 365;
  setClockToYMDHMS(y, m, d, hh, mm, ss);
}

void Carduino::setClockToYMDHMS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x06);            // year
  Wire.write(decToBcd(y));
  Wire.endTransmission();
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x05);            // month
  Wire.write(decToBcd(m));
  Wire.endTransmission();
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x04);            // day
  Wire.write(decToBcd(d));
  Wire.endTransmission();
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x02);            // hour
  Wire.write(decToBcd(hh) & 0b10111111);
  Wire.endTransmission();
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x01);            // minute
  Wire.write(decToBcd(mm));
  Wire.endTransmission();
  Wire.beginTransmission(CLOCK_ADDRESS);
  Wire.write(0x00);            // second
  Wire.write(decToBcd(ss));
  Wire.endTransmission();
}
