#pragma once
#include "knx_facade.h"

#define MANUFACTURER_ID 19748
#define DEVICE_ID 1
#define REVISION 0

// number of supported channels
#define NUM_Channels 50
#define NUM_paramBlockSize 110
#define NUM_paramOffset 12
#define KO_Offset 50

// fixed KO, just one occurence
#define KO_Heartbeat 1 // fix, only once

// fixed parameters, just one occurence
#define PAR_numChannels 0    // 4 Byte
#define PAR_startupDelay 4   // 4 Byte
#define PAR_HeartbeatDelay 8 // 4 Byte

//Dynamic parameters, per cannel occurence
#define PAR_f1StartupDelay 0     // 4 Byte
#define PAR_f1Logic 4            // 3 Bit    X
#define PAR_f1Calculate 5        // 1 Bit    X
#define PAR_f1Trigger 6          // 4 Bit    X
#define PAR_f1TriggerGateClose 7 // 2 Bit    Q
#define PAR_f1TriggerGateOpen 8  // 2 Bit    Q
#define PAR_f1E1 9               // 2 Bit    P LowNibble
#define PAR_f1E1Convert 9        // 2 Bit      HighNibble
#define PAR_f1E1Dpt 10           // 4 Bit    Y
#define PAR_f1E1Low 11           // 4 Byte
#define PAR_f1E1High 15          // 4 Byte
#define PAR_f1E1Default 19       // 2 Bit    Q
#define PAR_f1E1Repeat 20        // 4 Byte
#define PAR_f1E2 24              // 2 Bit    P LowNibble
#define PAR_f1E2Convert 24       // 2 Bit      HighNibble
#define PAR_f1E2Dpt 25           // 4 Bit    Y
#define PAR_f1E2Low 26           // 4 Byte
#define PAR_f1E2High 30          // 4 Byte
#define PAR_f1E2Default 34       // 2 Bit    Q
#define PAR_f1E2Repeat 35        // 4 Byte
#define PAR_f1I1 39              // 2 Bit    P
#define PAR_f1I1Function 40      // 1 Byte   F
#define PAR_f1I2 44              // 2 Bit    P
#define PAR_f1I2Function 45      // 1 Byte   F
#define PAR_f1OStair 49          // 1 Bit    W
#define PAR_f1OTimeBase 50       // 4 Bit    Z
#define PAR_f1OTime 51           // 4 Byte
#define PAR_f1ORetrigger 55      // 1 Bit    W
#define PAR_f1OStairOff 56       // 1 Bit    W
#define PAR_f1OBlink 57          // 4 Byte
#define PAR_f1ODelay 61          // 1 Bit / Bit 7    W
#define PAR_f1ODelayOnRepeat 61  // 2 Bit / Bit 6-5  W
#define PAR_f1ODelayOnReset 61   // 1 Bit / Bit 4    W
#define PAR_f1ODelayOffRepeat 61 // 2 Bit / Bit 3-2  W
#define PAR_f1ODelayOffReset 61  // 1 Bit / Bit 1    W
#define PAR_f1ODelayOn 62        // 4 Byte
#define PAR_f1ODelayOff 66       // 4 Byte
#define PAR_f1ORepeat 70         // 1 Bit / Bit 7   W
#define PAR_f1OOutputFiter 70    // 2 Bit / Bit 6-5
#define PAR_f1ORepeatOn 71       // 4 Byte
#define PAR_f1ORepeatOff 75      // 4 Byte
#define PAR_f1ODpt 79            // 4 Bit    Z
#define PAR_f1OOn 80             // 3 Bit    W
#define PAR_f1OOnValue 81        // 14 Byte
#define PAR_f1OOnPAArea 81       // 4 Bit / Bit 7-4
#define PAR_f1OOnPALine 81       // 4 Bit / Bit 3-0
#define PAR_f1OOnPADevice 82     // 1 Byte
#define PAR_f1OOff 95            // 3 Bit    W
#define PAR_f1OOffValue 96       // 14 Byte
#define PAR_f1OOffPAArea 96      // 4 Bit / Bit 7-4
#define PAR_f1OOffPALine 96      // 4 Bit / Bit 3-0
#define PAR_f1OOffPADevice 97    // 1 Byte

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
