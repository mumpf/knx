#include "Logikmodul.h"

#define LOGIKTEST 1

union KoData {
    uint32_t data[3];
    struct Ko {
        uint32_t Ko1 = 1;
        uint32_t Ko2 = 1;
        uint32_t Ko3 = 0;
    };
};

union ParamData {
    uint8_t data[110];
    struct Params {
        uint32_t numChannels = 1;       // 0
        uint32_t startupDelay = 10;     // 4
        uint32_t f1StartupDelay = 0;    // 8
        uint8_t f1Logic = 1;            // 12 - AND
        uint8_t f1Calculate = 0;        // 13 - eval on invalid inputs
        uint8_t f1Trigger = 15;         // 14 - trigger any value
        uint8_t f1TriggerGateClose = 0; // 15 - gate not valid
        uint8_t f1TriggerGateOpen = 0;  // 16 - gate not valid
        uint8_t f1E1 = 1;               // 17 - normal input 1
        //uint8_t f1E1Convert 17
        uint8_t f1E1Dpt = 0;            // 18 - DPT1
        uint32_t f1E1Low = 0;           // 19 - not valid
        uint32_t f1E1High = 0;          // 23 - not valid
        uint8_t f1E1Default = 0;        // 27 - default unset
        uint32_t f1E1Repeat = 0;        // 28 - no read from bus
        uint8_t f1E2 = 1;               // 32 - normal input 2
        //uint8_t f1E2Convert 32     
        uint8_t f1E2Dpt = 0;            // 33 - DPT1
        uint32_t f1E2Low = 0;           // 34 - not valid
        uint32_t f1E2High = 0;          // 38 - not valid
        uint8_t f1E2Default = 0;        // 42 - default unset
        uint32_t f1E2Repeat = 0;        // 43 - no read from bus
        uint8_t f1I1 = 0;               // 47 - int 1 off
        uint32_t f1I1Function = 0;      // 48 - not valid
        uint8_t f1I2 = 0;               // 52 - int 2 off
        uint32_t f1I2Function = 0;      // 53 - not valid
        uint8_t f1OStair = 0;           // 57 - no stair
        uint8_t f1OTimeBase = 0;        // 58 - not valid
        uint32_t f1OTime = 0;           // 59 - not valid
        uint8_t f1ORetrigger = 0;       // 63 - not valid
        uint8_t f1OStairOff = 0;        // 64 - not valid
        uint32_t f1OBlink = 0;          // 65 - no blink
        uint8_t f1ODelay = 0;           // 69 - no delay
        uint32_t f1ODelayOn = 0;        // 70 - not valid
        uint32_t f1ODelayOff = 0;       // 74 - not valid
        uint8_t f1ORepeat = 0;          // 78 - no repeat
        uint32_t f1ORepeatOn = 0;       // 79 - not vlid
        uint32_t f1ORepeatOff = 0;      // 83 - not valid
        uint8_t f1ODpt = 0;             // 87 - DPT1
        uint8_t f1OOn = 1;              // 88 - send on
        uint8_t f1OOnValue = 1;         // 89 - send 1 for on
        char filler1[13];     
        uint8_t f1OOff = 1;             // 103 - send off
        uint8_t f1OOffValue = 0;        // 104 - send 0 for off
        char filler2[13];
    };
};

ParamData sParamData;
KoData sKoData;
