#pragma once
#include "knx_facade.h"

#define MANUFACTURER_ID 19748
#define DEVICE_ID 1
#define REVISION 0

#define KO_f1E1 1
#define KO_f1E2 2
#define KO_f1O 3

#define PAR_numChannels 0         // 4 Byte
#define PAR_startupDelay 4        // 4 Byte
#define PAR_f1StartupDelay 8      // 4 Byte
#define PAR_f1Logic 12            // 3 Bit    X
#define PAR_f1Calculate 13        // 1 Bit    X
#define PAR_f1Trigger 14          // 4 Bit    X
#define PAR_f1TriggerGateClose 15 // 2 Bit    Q
#define PAR_f1TriggerGateOpen 16  // 2 Bit    Q
#define PAR_f1E1 17               // 2 Bit    P LowNibble
#define PAR_f1E1Convert 17        // 2 Bit      HighNibble
#define PAR_f1E1Dpt 18            // 4 Bit    Y
#define PAR_f1E1Low 19            // 4 Byte
#define PAR_f1E1High 23           // 4 Byte
#define PAR_f1E1Default 27        // 2 Bit    Q
#define PAR_f1E1Repeat 28         // 4 Byte
#define PAR_f1E2 32               // 2 Bit    P LowNibble
#define PAR_f1E2Convert 32        // 2 Bit      HighNibble
#define PAR_f1E2Dpt 33            // 4 Bit    Y
#define PAR_f1E2Low 34            // 4 Byte
#define PAR_f1E2High 38           // 4 Byte
#define PAR_f1E2Default 42        // 2 Bit    Q
#define PAR_f1E2Repeat 43         // 4 Byte
#define PAR_f1I1 47               // 2 Bit    P
#define PAR_f1I1Function 48       // 1 Byte   F
#define PAR_f1I2 52               // 2 Bit    P
#define PAR_f1I2Function 53       // 1 Byte   F
#define PAR_f1OStair 57           // 1 Bit    W
#define PAR_f1OTimeBase 58        // 4 Bit    Z
#define PAR_f1OTime 59            // 4 Byte
#define PAR_f1ORetrigger 63       // 1 Bit    W
#define PAR_f1OStairOff 64        // 1 Bit    W
#define PAR_f1OBlink 65           // 4 Byte
#define PAR_f1ODelay 69           // 1 Bit    W
#define PAR_f1ODelayOn 70         // 4 Byte
#define PAR_f1ODelayOff 74        // 4 Byte
#define PAR_f1ORepeat 78          // 1 Bit    W
#define PAR_f1ORepeatOn 79        // 4 Byte
#define PAR_f1ORepeatOff 83       // 4 Byte
#define PAR_f1ODpt 87             // 4 Bit    Z
#define PAR_f1OOn 88              // 1 Bit    W
#define PAR_f1OOnValue 89         // 14 Byte
#define PAR_f1OOff 103            // 1 Bit    W
#define PAR_f1OOffValue 104       // 14 Byte

#define NUM_paramBlockSize 110
#define NUM_paramOffset 8

// F 2 Byte, 6 gespart
// P 1 Byte, 3 gespart
// Q 1 Byte, 3 gespart
// W 1 Byte, 6 gespart
// X 1 Byte, 2 gespart
// Y 1 Byte, 1 gespart
// Z 1 Byte, 1 gespart

//   22 Bytes gespart.
//  114 -   22 =   92
//   92 *   80 = 7360
// 7360 + 1760 = 9120

// KnxFacade *knx;
// Platform *platform;

void appSetup();
void appLoop();
