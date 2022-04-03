#include "Arduino.h"
#include "Carduino.h"

// functions copied from https://github.com/NorthernWidget/DS3231
// just took the minimal fuctionality we need for Carduino, which
// does not care about actual dates and times.  We just want a high
// precision number of seconds since a given start time to calculate
// epochs and fractions thereof.
static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
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
    return days + DAYS_IN_YEAR * y + (y + 3) / 4 - 1;
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
  pinMode(oePin, OUTPUT);
  Wire.begin();
}

void Carduino::init() {
  if (isRV3028Clock()) {
    if (!isRV3028Configured()) {
      RV3028Configure();
      configuringConfirmationDisplay();
    }
  }
}

void Carduino::setBrightness(byte brightness) // 0 to 255
{
  analogWrite(oePin, 255-brightness);
}

void Carduino::displayHoursAndMinutes() {
  uint16_t y;
  uint8_t m, d, hh, mm, ss;
  
  getNowFromClock(y,  m, d, hh, mm, ss);

  uint32_t hoursArray[] = { dot2, dot4, dot6, dot8, 
                            dot10, dot12, dot14, dot16,
                            dot18, dot20, dot22, dot24 };

  uint32_t minutesArray2[] = { dot1, dot3, dot5, dot7, 
                               dot9, dot11, dot13, dot15,
                               dot17, dot19, dot21, dot23 };

  uint32_t minutesArray1[] = { dot25, dot26, dot28, dot30, dot29, dot27 };
  
  displayDots(hoursArray[hh%12] & minutesArray1[mm/10] & minutesArray2[mm%10]);

// uncomment to debug on a device that supports it
//  char dateTimeString[60];
//  sprintf(dateTimeString, "hh mod 12: %i mm/10: %i mm mod 10: %i", hh%12, mm/10, mm%10);
//  Serial.println(dateTimeString);

  delay(1000);
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
  
  unsigned long beginingOfTimeSeconds = getSecondsAtBeginingOfTime();
  unsigned long nowSeconds = getNowFromClock();
  unsigned long shelley_seconds = nowSeconds - beginingOfTimeSeconds;
  unsigned long seconds_in_epoch = 432000; //60*60*5*24
  unsigned long seconds_in_this_epoch = shelley_seconds % seconds_in_epoch;
  float portion_of_epoch_completed = seconds_in_this_epoch / (float)seconds_in_epoch;
  int dot = portion_of_epoch_completed * 24;
  int epochIndex = seconds_in_this_epoch % 6;

  // was overflowing dynamic memory space with an array of middle dots
  // below implementation is uglier than using an array , but it trades
  // program space (which I have) with dynamic memory space (which I don't have)
  if (epochIndex == 0) {
    displayDots(clockarray[dot] & dot25);
  } else if (epochIndex == 1) {
    displayDots(clockarray[dot] & dot25 & dot26);
  } else if (epochIndex == 2) {
    displayDots(clockarray[dot] & dot25 & dot26 & dot28);
  } else if (epochIndex == 3) {
    displayDots(clockarray[dot] & dot25 & dot26 & dot28 & dot30);
  } else if (epochIndex == 4) {
    displayDots(clockarray[dot] & dot25 & dot26 & dot28 & dot30 & dot29);
  } else if (epochIndex == 5) {
    displayDots(clockarray[dot] & dot25 & dot26 & dot28 & dot30 & dot29 & dot27);
  }
  delay(100);
}

void Carduino::teaser() {
  delay(3000);
  accelleratingClock();
  reverseClock();
  fireworks1();
  sequence4();
  fireworks2();
  fireworks2();
  sequence2(50);
  sequence3(50);
  sequence2(50);
  full();
  fadeout();
}

void Carduino::configuringConfirmationDisplay() {
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
                                            
  for (int i=0; i<sizeofClockarray; i++) {
    displayDots(clockarray[i]);
    delay(200);
  }
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

void Carduino::sequence4() {
  const int sizeofClockarray = 30;
  uint32_t clockarray[sizeofClockarray] = { dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24,
                                            dot25, dot26, dot27, dot28, dot29, dot30 };
  for (int i=0; i<sizeofClockarray; i++) {
    displayDots(clockarray[i]);
    delay(50);
  }
}

void Carduino::accelleratingClock() {
  const int sizeofClockarray = 25;
  uint32_t clockarray[sizeofClockarray] = { dot0, dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24 };
  for (int i=0; i<sizeofClockarray; i++) {
    if (i>0) {
      clockarray[i] = clockarray[i] & clockarray[i-1];
    } 
  }

  displayDots(dot0);

  delay(3000);

  displayDots(dot25 & dot26 & dot27 & dot28 & dot29 & dot30);

  delay(2000);

  for (int i=0; i<3; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(2000);
  }

  for (int i=3; i<8; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(1000);
  }

  for (int i=8; i<12; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(500);
  }

  for (int i=12; i<25; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(200);
  }
}

void Carduino::accelleratingClock2() {
  const int sizeofClockarray = 25;
  uint32_t clockarray[sizeofClockarray] = { dot0, dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24 };
  for (int i=0; i<sizeofClockarray; i++) {
    if (i>0) {
      clockarray[i] = clockarray[i] & clockarray[i-1];
    } 
  }

  for (int i=0; i<3; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(500);
  }

  for (int i=3; i<8; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(200);
  }

  for (int i=8; i<12; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(100);
  }

  for (int i=12; i<25; i++) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(50);
  }
}

void Carduino::reverseClock() {
  const int sizeofClockarray = 25;
  uint32_t clockarray[sizeofClockarray] = { dot0, dot1, dot2, dot3, dot4, dot5, dot6, dot7, dot8,
                                            dot9, dot10, dot11, dot12, dot13, dot14, dot15, dot16,
                                            dot17, dot18, dot19, dot20, dot21, dot22, dot23, dot24 };
  for (int i=0; i<sizeofClockarray; i++) {
    if (i>0) {
      clockarray[i] = clockarray[i] & clockarray[i-1];
    } 
  }

  for (int i=sizeofClockarray-1; i>=0; i--) {
    displayDots(clockarray[i] & dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
    delay(100);
  }
}

void Carduino::full() {
  displayDots(dot1 & dot2 & dot3 & dot4 & dot5 & dot6 & dot7 & dot8 & 
              dot9 & dot10 & dot11 & dot12 & dot13 & dot14 & dot15 & dot16 & 
              dot17 & dot18 & dot19 & dot20 & dot21 & dot22 & dot23 & dot24 &
              dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
  delay(3000);
}

void Carduino::heartbeat() {
  setBrightness(0);
  displayDots(dot1 & dot2 & dot3 & dot4 & dot5 & dot6 & dot7 & dot8 & 
              dot9 & dot10 & dot11 & dot12 & dot13 & dot14 & dot15 & dot16 & 
              dot17 & dot18 & dot19 & dot20 & dot21 & dot22 & dot23 & dot24 &
              dot25 & dot26 & dot27 & dot28 & dot29 & dot30);
  for (byte b = 0; b < 255; b++)
  {
    setBrightness(b);
    delay(5);
  }
  for (byte b = 255; b > 0; b--)
  {
    setBrightness(b);
    delay(5);
  }
  setBrightness(0);
  delay(500);
}

void Carduino::fadeout() {
  const int sizeofClockarray = 3;
  uint32_t clockarray[sizeofClockarray] = { dot25 & dot26 & dot27 & dot28 & dot29 & dot30,
                                            dot1 & dot3 & dot5 & dot7 & dot9 & dot11 & dot13 & dot15 & dot17 & dot19 & dot21 & dot23 & dot25 & dot26 & dot27 & dot28 & dot29 & dot30,
                                            dot1 & dot2 & dot3 & dot4 & dot5 & dot6 & dot7 & dot8 &
                                            dot9 & dot10 & dot11 & dot12 & dot13 & dot14 & dot15 & dot16 &
                                            dot17 & dot18 & dot19 & dot20 & dot21 & dot22 & dot23 & dot24 & 
                                            dot25 & dot26 & dot27 & dot28 & dot29 & dot30 };
  for (int i=sizeofClockarray-1; i>=0; i--) {
    displayDots(clockarray[i]);
    delay(3000);
  }
  displayDots(dot0);
  delay(2000);
}

void Carduino::sequence2(int limit) {
  const int sizeOfClockArray = 12;
  const int sizeOfMiddleArray = 6;
  uint32_t middlearray[sizeOfClockArray] = { dot25, dot26, dot28, dot30, dot29, dot27 };
  uint32_t clockarray[sizeOfClockArray] = { dot1 & dot2, dot3 & dot4, dot5 & dot6, dot7 & dot8, dot9 & dot10,
                                            dot11 & dot12, dot13 & dot14, dot15 & dot16, dot17 & dot18, dot19 & dot20,
                                            dot21 & dot22, dot23 & dot24 };
  int masterCounter = 0;
  int middleCounter = 0;
  int outerCounter = 0;
  while (true) {
    displayDots(clockarray[outerCounter] & middlearray[middleCounter]);
    middleCounter ++;
    outerCounter ++;
    masterCounter ++;
    if (middleCounter >= sizeOfMiddleArray) middleCounter = 0;
    if (outerCounter >= sizeOfClockArray) outerCounter = 0;
    if (masterCounter > limit) break;
    delay(100);
  }
}

void Carduino::sequence3(int limit) {
  const int sizeOfClockArray = 12;
  uint32_t middlearray[sizeOfClockArray] = { dot25, dot26, dot28, dot30, dot29, dot27 };
  uint32_t clockarray[sizeOfClockArray] = { dot1 & dot2, dot3 & dot4, dot5 & dot6, dot7 & dot8, dot9 & dot10,
                                            dot11 & dot12, dot13 & dot14, dot15 & dot16, dot17 & dot18, dot19 & dot20,
                                            dot21 & dot22, dot23 & dot24 };
  int masterCounter = 0;
  int middleCounter = 5;
  int outerCounter = 11;
  while (true) {
    displayDots(clockarray[outerCounter] & middlearray[middleCounter]);
    middleCounter --;
    outerCounter --;
    masterCounter ++;
    if (middleCounter <= 0) middleCounter = 5;
    if (outerCounter <= 0) outerCounter = 11;
    if (masterCounter > limit) break;
    delay(100);
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
      delay(600);
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

long Carduino::getSecondsAtBeginingOfTime() {
  return getSecondsFormYMDHHMMSS(BEGINING_OF_TIME_YEAR, BEGINING_OF_TIME_MONTH, BEGINING_OF_TIME_DAY, 
                                 BEGINING_OF_TIME_HOURS, BEGINING_OF_TIME_MINUTES, BEGINING_OF_TIME_SECONDS);
}

long Carduino::getSecondsFormYMDHHMMSS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  uint16_t days = date2days(y, m, d);
  return time2long(days, hh, mm, ss);
}

bool Carduino::isDS3231Clock() {
  Wire.beginTransmission(DS3231_ADDR);
  return (Wire.endTransmission() == 0);
}

bool Carduino::isRV3028Clock() {
  Wire.beginTransmission(RV3028_ADDR);
  return (Wire.endTransmission() == 0);
}

long Carduino::getNowFromClock() {
  if (isDS3231Clock()) {
    return getNowFromDS3231();
  } else if (isRV3028Clock()) {
    return getNowFromRV3028();
  }
  return 0;
}

// read current date and time from DS3231 clock
// reference: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
long Carduino::getNowFromDS3231() {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 7);
  uint8_t ss = bcd2bin(Wire.read() & 0x7F);
  uint8_t mm = bcd2bin(Wire.read());
  uint8_t hh = bcd2bin(Wire.read());
  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read());
  uint16_t days = date2days(y, m, d);
  long secondsElapsed = time2long(days, hh, mm, ss);

  printDateTimeToSerial(ss, mm, hh, y, m, d);
  
  return secondsElapsed;
}

// read current date and time from DS3231 clock
// reference: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
long Carduino::getNowFromRV3028() {
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(RV3028_ADDR, 7);
  uint8_t ss = bcd2bin(Wire.read());
  uint8_t mm = bcd2bin(Wire.read());
  uint8_t hh = bcd2bin(Wire.read());
  Wire.read();
  uint8_t d = bcd2bin(Wire.read());
  uint8_t m = bcd2bin(Wire.read());
  uint16_t y = bcd2bin(Wire.read());
  uint16_t days = date2days(y, m, d);
  long secondsElapsed = time2long(days, hh, mm, ss);

  printDateTimeToSerial(ss, mm, hh, y, m, d);
  
  return secondsElapsed;
}

void Carduino::printDateTimeToSerial(uint8_t ss, uint8_t mm, uint8_t hh, uint8_t d, uint8_t m, uint16_t y) {
  // uncomment to debug on a device that supports it
//  char dateTimeString[60];
//  unsigned long beginingOfTimeSeconds = getSecondsAtBeginingOfTime();
//  unsigned long shelley_seconds = secondsElapsed - beginingOfTimeSeconds;
//  unsigned long seconds_in_epoch = 432000; //60*60*5*24
//  unsigned long epoch = (shelley_seconds / seconds_in_epoch) + SHELLY_EPOCH_OFFSET;
//  unsigned long seconds_in_this_epoch = shelley_seconds % seconds_in_epoch;
//  float portion_of_epoch_completed = seconds_in_this_epoch / (float)seconds_in_epoch;
//  int dot = portion_of_epoch_completed * 24;
//  int epochIndex = seconds_in_this_epoch % 6;
//  sprintf(dateTimeString,"%i %i %i: %i %i %i:epochs: %i dots: %i", d, m, y, hh, mm, ss, epoch, dot);
//  Serial.println(dateTimeString);
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
    if (days < (uint16_t)(DAYS_IN_YEAR + leap))
      break;
    days -= DAYS_IN_YEAR + leap;
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
  uint8_t y = days / DAYS_IN_YEAR;
  d = d % DAYS_IN_YEAR;
  setClockToYMDHMS(y, m, d, hh, mm, ss);
}

void Carduino::setClockToYMDHMS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  if (isDS3231Clock()) {
    setDS3231ToYMDHMS(y, m, d, hh-1, mm, ss);
  } else if (isRV3028Clock()) {
    setRV3028ToYMDHMS(y, m, d, hh-1, mm, ss);
  }
}

void Carduino::setDS3231ToYMDHMS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x06);            // year
  Wire.write(decToBcd(y));
  Wire.endTransmission();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x05);            // month
  Wire.write(decToBcd(m));
  Wire.endTransmission();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x04);            // day
  Wire.write(decToBcd(d));
  Wire.endTransmission();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x02);            // hour
  Wire.write(decToBcd(hh) & 0b10111111);
  Wire.endTransmission();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x01);            // minute
  Wire.write(decToBcd(mm));
  Wire.endTransmission();
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0x00);            // second
  Wire.write(decToBcd(ss));
  Wire.endTransmission();
}

void Carduino::setRV3028ToYMDHMS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x06);            // year
  Wire.write(decToBcd(y));
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x05);            // month
  Wire.write(decToBcd(m));
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x04);            // day
  Wire.write(decToBcd(d));
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x02);            // hour
  Wire.write(decToBcd(hh) & 0b10111111);
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x01);            // minute
  Wire.write(decToBcd(mm));
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x00);            // second
  Wire.write(decToBcd(ss));
  Wire.endTransmission();
}

// to set the clock to the current time, add the following to the sketch
//  carduino.setClock(__DATE__, __TIME__);
void Carduino::setClock(const char* date, const char* time) {
  static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  static char buff[4] = {'0','0','0','0'};
  uint16_t y;
  uint8_t m,d,hh,mm,ss;
  sscanf(date, "%s %hhu %d", buff, &d, &y);
  m = (strstr(month_names, buff) - month_names) / 3 + 1;
  sscanf(time, "%hhu:%hhu:%hhu", &hh, &mm, &ss);
  setClockToYMDHMS(y-2000, m, d, hh, mm, ss);
}


// read current date and time from DS3231 clock
// reference: https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
void Carduino::getNowFromClock(uint16_t &y, uint8_t &m, uint8_t &d, uint8_t &hh, uint8_t &mm, uint8_t &ss) {
  if (isDS3231Clock()) {
    getNowFromDS3231Clock(y, m, d, hh, mm, ss);
  } else if (isRV3028Clock()) {
    getNowFromRV3028Clock(y, m, d, hh, mm, ss);
  }
}

void Carduino::getNowFromDS3231Clock(uint16_t &y, uint8_t &m, uint8_t &d, uint8_t &hh, uint8_t &mm, uint8_t &ss) {
  Wire.beginTransmission(DS3231_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDR, 7);
  ss = bcd2bin(Wire.read() & 0x7F);
  mm = bcd2bin(Wire.read());
  hh = bcd2bin(Wire.read());
  Wire.read();
  d = bcd2bin(Wire.read());
  m = bcd2bin(Wire.read());
  y = bcd2bin(Wire.read());
}

void Carduino::getNowFromRV3028Clock(uint16_t &y, uint8_t &m, uint8_t &d, uint8_t &hh, uint8_t &mm, uint8_t &ss) {
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(RV3028_ADDR, 7);
  ss = bcd2bin(Wire.read());
  mm = bcd2bin(Wire.read());
  hh = bcd2bin(Wire.read());
  Wire.read();
  d = bcd2bin(Wire.read());
  m = bcd2bin(Wire.read());
  y = bcd2bin(Wire.read());
}

bool Carduino::isRV3028Configured() {
  uint8_t eeprom_backup_value = RV3028readConfigEEPROM(0x37);
  if (bitRead(eeprom_backup_value, 0) != 0) return false; //TCR  bits 1:0
  if (bitRead(eeprom_backup_value, 1) != 0) return false; //TCR  00 TCR 3 kΩ – Default value on delivery
  if (bitRead(eeprom_backup_value, 2) != 1) return false; //BSM  bits 3:2
  if (bitRead(eeprom_backup_value, 3) != 0) return false; //BSM  01 Enables the Direct Switching Mode (DSM) - Switchover when VDD < VBACKUP
  if (bitRead(eeprom_backup_value, 4) != 1) return false; //FEDE 1 Default value on delivery
  if (bitRead(eeprom_backup_value, 5) != 0) return false; //TCE  0 Disabled – Default value on delivery
  if (bitRead(eeprom_backup_value, 6) != 0) return false; //BSIE 0 Default value on delivery
  return true;
}

void Carduino::RV3028Configure() {
  // The RV3028 RTC defaults to backup power disabled.  We want it enabled to avoid losing
  // the correct time.  Below switches backup power mode to direct switching mode
  // https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3028-C7_App-Manual.pdf
  // section 3.15.6
  // Do not change bit 7 as it is part of the offset register that is factory calibrated.
  uint8_t eeprom_backup_value = RV3028readConfigEEPROM(0x37);
  bitClear(eeprom_backup_value, 0);
  bitClear(eeprom_backup_value, 1);
  bitSet(eeprom_backup_value, 2);
  bitClear(eeprom_backup_value, 3);
  bitSet(eeprom_backup_value, 4);
  bitClear(eeprom_backup_value, 5);
  bitClear(eeprom_backup_value, 6);
  RV3028writeConfigEEPROM(0x37, eeprom_backup_value);
}

void Carduino::RV3028waitForNotBusy() {
  while (bitRead(RV3028readRegister(0x0E), 7) != 0);
}

uint8_t Carduino::RV3028readConfigEEPROM(uint8_t eepromaddr)
{
  RV3028waitForNotBusy();

  RV3028disableAutoRefresh();
  
  //Read EEPROM Register
  //https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-3028-C7_App-Manual.pdf
  //3.13.EEPROM MEMORY CONTROL REGISTERS
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x25);
  Wire.write(eepromaddr);
  Wire.endTransmission();
  
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x27);  
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x27);
  Wire.write(0x22);
  Wire.endTransmission();
  
  RV3028waitForNotBusy();
  
  uint8_t eepromdata = RV3028readRegister(0x26);
  
  RV3028waitForNotBusy();
  
  RV3028enableAutoRefresh();

  return eepromdata;
}

void Carduino::RV3028writeConfigEEPROM(uint8_t eepromaddr, uint8_t val)
{
  RV3028waitForNotBusy();
  RV3028disableAutoRefresh();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(eepromaddr);
  Wire.write(val);
  Wire.endTransmission();
  RV3028writeToEEPROM();
  RV3028waitForNotBusy();
  RV3028disableAutoRefresh();
  RV3028waitForNotBusy();
}

void Carduino::RV3028enableAutoRefresh() {
  uint8_t ctrl1_read_value = RV3028readRegister(0x0F);
  bitSet(ctrl1_read_value, 3);
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x0F);
  Wire.write(ctrl1_read_value);
  Wire.endTransmission();
}

void Carduino::RV3028disableAutoRefresh() {
  uint8_t ctrl1_read_value = RV3028readRegister(0x0F);
  bitClear(ctrl1_read_value, 3);
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x0F);
  Wire.write(ctrl1_read_value);
  Wire.endTransmission();
}

uint8_t Carduino::RV3028readRegister(uint8_t addr)
{
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(addr);
  Wire.endTransmission();

  Wire.requestFrom(RV3028_ADDR, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  else {
    return (0xFF);
  }
}

void Carduino::RV3028writeToEEPROM() {
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x27);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.beginTransmission(RV3028_ADDR);
  Wire.write(0x27);
  Wire.write(0x11);
  Wire.endTransmission();
}
