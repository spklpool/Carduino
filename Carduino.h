#ifndef Carduino_h
#define Carduino_h

#include <DS3231.h>
#include <Wire.h>

// These binary values are the secret sauce that makes this code simple.
// Each bit of the 32 bit entries in the array represents one
// of the output pins on one of the 4 shift registers.
// Below is a represendation of the bits grouped by the shift
// registers.  Each group of 8 bits is serialized least significant
// bit first and the shift registers are daisy chained so the right 
// most bits in the array will end up being latched on shift register
// 1 after all bits are serialized.
// 44444444333333332222222211111111
#define dot0  0b11111111111111111111111111111111
#define dot1  0b11111111110111111111111111111111
#define dot2  0b11111101111111111111111111111111
#define dot3  0b11101111111111111111111111111111
#define dot4  0b11111110111111111111111111111111
#define dot5  0b01111111111111111111111111111111
#define dot6  0b11110111111111111111111111111111
#define dot7  0b10111111111111111111111111111111
#define dot8  0b11111011111111111111111111111111
#define dot9  0b11111111111111011111111111111111
#define dot10 0b11011111111111111111111111111111
#define dot11 0b11111111111110111111111111111111
#define dot12 0b11111111111111101111111111111111
#define dot13 0b11111111111111111111111011111111
#define dot14 0b11111111011111111111111111111111
#define dot15 0b11111111111111111101111111111111
#define dot16 0b11111111111111111011111111111111
#define dot17 0b11111111111111111110111111111111
#define dot18 0b11111111111111111111111111111101
#define dot19 0b11111111111111111111111111111110
#define dot20 0b11111111111111111111111111011111
#define dot21 0b11111111111111110111111111111111
#define dot22 0b11111111111111111111111111101111
#define dot23 0b11111111111111111111111111111011
#define dot24 0b11111111111111111111111111110111
#define dot25 0b11111111111111111111110111111111
#define dot26 0b11111111111011111111111111111111
#define dot27 0b11111111111111111111011111111111
#define dot28 0b11111111111101111111111111111111
#define dot29 0b11111111111111111111101111111111
#define dot30 0b11111111101111111111111111111111

#define clockPin 6
#define dataPin 4
#define latchPin 5

#define CARDUINO_LED_COUNT 32
#define HOURS_TO_SECONDS_MULTIPLIER 3600
#define SECONDS_IN_DOT 18000 // 60*60*5;
#define DOTS_IN_EPOCH 24
#define MILLIS_IN_SECOND 1000

class Carduino
{
  public:
    Carduino();
    void runClock();
    void runClock(uint32_t speedMultiplier);
    void runClock(uint32_t speedMultiplier, bool middleEpochCounter);
    void runClockWithEpochCounter(uint32_t speedMultiplier);
    void inwardSpiral();
    void displayDots(uint32_t data);
    void setTimeToNow();
    void showErrorState();
    
  private:
    DS3231 clock;
    RTClib rtc;
    uint32_t dotsElapsed = 0;
    uint32_t epochsElapsed = 0;
    uint32_t lastDotsElapsed = 0;
    uint32_t lastEpochsElapsed = 0;
    uint32_t remainingMillis = 0;
    uint8_t getValueAtIndex(uint32_t data, byte index);
};


#endif
