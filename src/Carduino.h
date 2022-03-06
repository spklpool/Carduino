#ifndef Carduino_h
#define Carduino_h

#include <Wire.h>

#define CLOCK_ADDRESS 0x68

// These binary values are the secret sauce that makes this code simple.
// Each bit of the 32 bit entries in the array represents one
// of the output pins on one of the 4 shift registers.
// Below is a represendation of the bits grouped by the shift
// registers.  Each group of 8 bits is serialized least significant
// bit first and the shift registers are daisy chained so the right 
// most bits in the array will end up being latched on shift register
// 1 after all bits are serialized.
// 44444444333333332222222211111111

// 20 23     21 24 19 22 15 18 17 16 30 14 9  12 11 10 7 2 5 8 3 6 4

//    1      13  

#define dot0  0b11111111111111111111111111111111
#define dot1  0b11111111111111111111110111111111
#define dot2  0b11111111111111111111111111011111
#define dot3  0b11111111111111111111111111111011
#define dot4  0b11111111111111111111111110111111
#define dot5  0b11111111111111110111111111111111
#define dot6  0b11111111111111111111111111110111
#define dot7  0b11111111111111111111111111111110
#define dot8  0b11111111111111111111111111101111
#define dot9  0b11111111111111111101111111111111
#define dot10 0b11111111111111111111111111111101
#define dot11 0b11111111111111111110111111111111
#define dot12 0b11111111111111111011111111111111
#define dot13 0b11111111101111111111111111111111
#define dot14 0b11111111011111111111111111111111
#define dot15 0b11111111111111011111111111111111
#define dot16 0b11111111111111101111111111111111
#define dot17 0b11111111111110111111111111111111
#define dot18 0b11011111111111111111111111111111
#define dot19 0b10111111111111111111111111111111
#define dot20 0b11111101111111111111111111111111
#define dot21 0b01111111111111111111111111111111
#define dot22 0b11111011111111111111111111111111
#define dot23 0b11101111111111111111111111111111
#define dot24 0b11110111111111111111111111111111
#define dot25 0b11111111110111111111111111111111
#define dot26 0b11111111111111111111101111111111
#define dot27 0b11111111111101111111111111111111
#define dot28 0b11111111111111111111011111111111
#define dot29 0b11111111111011111111111111111111
#define dot30 0b11111111111111111111111011111111

#define ALLOFF 0b01111111111111111111111111111111
#define ALLON  0b00000000000000000000000000000000

#define oePin 0
#define clockPin 1
#define buttonPin 3
#define dataPin 4
#define latchPin 5

#define CARDUINO_LED_COUNT 32
#define HOURS_TO_SECONDS_MULTIPLIER 3600
#define SECONDS_IN_DOT 18000 // 60*60*5;
#define DOTS_IN_EPOCH 24
#define MILLIS_IN_SECOND 1000
#define RESET_CYCLE_THRESHOLD 50
#define ADVANCE_CYCLE_THRESHOLD 10

#define BEGINING_OF_TIME_HOURS 16
#define BEGINING_OF_TIME_MINUTES 45
#define BEGINING_OF_TIME_SECONDS 00
#define BEGINING_OF_TIME_DAY 3
#define BEGINING_OF_TIME_MONTH 8
#define BEGINING_OF_TIME_YEAR 20
#define SHELLY_EPOCH_OFFSET 209;

class Carduino
{
  public:
    Carduino();
    void displayHoursAndMinutes();
    void runClock();
    void runClock(bool middleEpochCounter);
    void runClock(uint32_t speedMultiplier);
    void runClock(uint32_t speedMultiplier, bool middleEpochCounter);
    void teaser();
    void inwardSpiral();
    void displayDots(uint32_t data);
    void setTimeToNow();
    void checkDateTime();
    void printNow();
    long getSecondsAtBeginingOfTime();
    long getSecondsFormYMDHHMMSS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);
    long getNowFromClock();
    void getNowFromClock(uint16_t &y, uint8_t &m, uint8_t &d, uint8_t &hh, uint8_t &mm, uint8_t &ss);
    void resetClockToZero();
    void setClock(const char* date, const char* time);
    void setClockToSeconds(long timeInSeconds);
    void setClockToYMDHMS(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);
    void advanceClockByOneDot();
    bool isButtonPressed();
    void full();
    void fadeout();
    void fireworks1();
    void fireworks2();
    void sequence1();
    void sequence2(int limit);
    void sequence3(int limit);
    void sequence4();
    void accelleratingClock();
    void accelleratingClock2();
    void reverseClock();
    void test1();
        
  private:
    byte decToBcd(byte val);
    byte bcdToDec(byte val);
    uint32_t dotsElapsed = 0;
    uint32_t epochsElapsed = 0;
    uint8_t getValueAtIndex(uint32_t data, byte index);
    uint8_t resetCycleCount = 0;
    uint8_t advanceCycleCount = 0;
    bool buttonPressed = false;
};


#endif
