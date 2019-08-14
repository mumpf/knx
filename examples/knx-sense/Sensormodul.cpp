#include "Sensormodul.h"
#include <ClosedCube_HDC1080.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>

// ################################################
// ### DEBUG CONFIGURATION
// ################################################
// I2C-MODE
#define I2COFF // comment this line to disable I2C mode \
               // DEBUG-MODE
#define KDEBUG // comment this line to disable DEBUG mode

#ifdef KDEBUG
#include <DebugUtil.h>

// Get correct serial port for debugging
#ifdef __AVR_ATmega32U4__
// Leonardo/Micro/ProMicro use the USB serial port
#define DEBUGSERIAL Serial
// #elif __SAMD21G18A__
#elif ARDUINO_ARCH_SAMD
// Zero use
#define DEBUGSERIAL SerialUSB
#elif ESP8266
// ESP8266 use the 2nd serial port with TX only
#define DEBUGSERIAL Serial1
#else
// All other, (ATmega328P f.i.) use software serial
#include <SoftwareSerial.h>
SoftwareSerial softserial(11, 10); // RX, TX
#define DEBUGSERIAL softserial
#endif
// end of debugging defs
#endif

#define KNX_SERIAL Serial1 // Leonardo/Micro/Zero etc. use Serial1

// ################################################
// ### IO Configuration
// ################################################
#define PROG_LED_PIN 13
#define PROG_BUTTON_PIN 11
#define LED_YELLOW 38

extern KnxFacade *KNX;
extern Platform *platform;

// ################################################
// ### define Parameter
// ################################################
boolean isDelay = false;
boolean isMeasure = false;
uint8_t abfrage_Interval = 0;
boolean CO2_active = false;
boolean OneWire_active = false;

// ################################################
// ### define Variables
// ################################################
float temp = 0;
float temp_filt = 0;
int temp_filt_int = 0;
float hum = 0;
float temp_OneWire = 0;
float temp_OneWire_filt = 0;
int temp_OneWire_filt_int = 0;
float temp_c;
float humidity;

float temp_cal = 0;

// Anzahl der Mittelungen definieren und globale Variablen festlegen
#define anzahlMittelWerte 60
float werte[anzahlMittelWerte];
int zaehlerMittelWerte = 0;

// ################################################
// ### define Temp / Hum Sensor
// ################################################
ClosedCube_HDC1080 hdc1080;

// ################################################
// ### define CO2 Sensor
// ################################################
#define ADDR 0x5A
#define WAKE_PIN 3
// CCS811 sensor;

// ################################################
// ### define OneWire Sensor
// ################################################
OneWire oneWire(2); //I2C Adresse = 0x1A -> 0x18|0x2 = 0x1A  (0x18 ist Standard Adresse)
DallasTemperature sensors(&oneWire);

// ################################################
// ### Funktion
// ################################################

//****************** Filter Paramter **************************************************
void Filtern(float &FiltVal, int NewVal, int FF) {
    FiltVal = ((FiltVal * FF) + NewVal) / (FF + 1.0);
}

// #################################################################################
// ### SETUP
// #################################################################################

void appSetup() {
    //****************** Init IO *******************************************************
    pinMode(LED_YELLOW, OUTPUT);
    digitalWrite(LED_YELLOW, HIGH);

    Wire.begin(); // join i2c bus (address optional for master)

    //****************** Init Debug Interface ********************************************
#ifdef KDEBUG
        // Start debug serial with 9600 bauds
    DEBUGSERIAL.begin(115200);
#if defined(__AVR_ATmega32U4__) || defined(__SAMD21G18A__)
    // wait for serial port to connect. Needed for Leonardo/Micro/ProMicro/Zero only
    while (!DEBUGSERIAL)
#endif
        // make debug serial port known to debug class
        // Means: KONNEKTING will sue the same serial port for console debugging
        Debug.setPrintStream(&DEBUGSERIAL);
    Debug.print(F("KONNEKTING DemoSketch\n"));
#endif

    //****************** Read Parameter ***************************************************
#ifdef KDEBUG
    Debug.println("** READ Parameter ******************");
#endif
    uint32_t mHeartbeat = KNX->paramInt(PAR_Heartbeat) * 1000;
    static uint8_t abfrage_Interval_val = ((uint8_t)Konnekting.getUINT8Param(0)); // abfrage_Interval
    temp_cal = ((uint8_t)Konnekting.getUINT8Param(1)) / 10.0;
    CO2_active = (uint8_t)Konnekting.getUINT8Param(2);
    OneWire_active = (uint8_t)Konnekting.getUINT8Param(3);

    //abfrage_Interval_val = 1; //*******************Aendern***************************************************************

    switch (abfrage_Interval_val) {
        case 0:
            abfrage_Interval = 5;
            break;
        case 1:
            abfrage_Interval = 10;
            break;
        case 2:
            abfrage_Interval = 30;
            break;
        case 3:
            abfrage_Interval = 60;
            break;
        case 4:
            abfrage_Interval = 300;
            break;
        case 5:
            abfrage_Interval = 600;
            break;
        case 6:
            abfrage_Interval = 1800;
            break;
        case 7:
            abfrage_Interval = 3600;
            break;
        default:
            Debug.println("abfrage_Interval ist irgendwas\n");
            break;
    }
#ifdef KDEBUG
    Debug.println("Sende_Zyklus: %i", abfrage_Interval);
#endif

    //****************** INIT Temp Hum Sensor *********************************************
#ifdef KDEBUG
    Debug.println("*************************************");
    Debug.println("Sensor INIT START");
    Debug.println("*************************************");
#endif
#ifdef I2COFF
#ifdef KDEBUG
    Debug.print("Checking for Temp/Hum ...: ");
#endif
    hdc1080.begin(0x40);
#ifdef KDEBUG
    Debug.println("OK");
#endif
#endif

    if (CO2_active == true) {
#ifdef I2COFF
//         if (!sensor.begin(uint8_t(ADDR), uint8_t(WAKE_PIN))) {
// #ifdef KDEBUG
//             Debug.println("*************************************");
//             Debug.println("Initialization CO2 failed.");
// #endif
//         }
#endif
    }
#ifdef KDEBUG
    else {
        Debug.println("*************************************");
        Debug.println("VOC Sensor Inaktiv");
    }
#endif

    if (OneWire_active == true) {
#ifdef I2COFF
        sensors.begin();
#endif
#ifdef KDEBUG
        Debug.println("*************************************");
        Debug.print("Checking for OneWire_Master...: ");
#endif
#ifdef I2COFF
        if (oneWire.checkPresence()) {
#ifdef KDEBUG
            Debug.println("OK");
#endif
            oneWire.deviceReset();
        } else {
#ifdef KDEBUG
            Debug.println("FAIL");
#endif
        }
#endif
    }
#ifdef KDEBUG
    else {
        Debug.println("*************************************");
        Debug.println("OneWire Sensor Inaktiv");
    }
#endif

#ifdef KDEBUG
    Debug.println("*************************************");
    Debug.println("Sensor INIT FINISH");
    Debug.println("*************************************");
#endif

    //****************** Init Timer *******************************************************
    // setTimer();
    // setTimer_ms(1000);

    //  for(int i = 0; i>100; i++)
    //  {
    //   writeEeprom(i, 255);
    //   delay(10);
    //  }

#ifdef KDEBUG
    Debug.println("FINISH Setup");
#endif

    digitalWrite(LED_YELLOW, LOW);
}

// #################################################################################
// ### Loop()
// #################################################################################

void loop() {
    //digitalWrite(LED_YELLOW, LOW);
    Knx.task();

    if (Konnekting.isReadyForApplication()) {

        if (isDelay == true) {
            isDelay = false;
            digitalWrite(LED_YELLOW, HIGH);
            //******* Temp & HUM  ****************************
            temp_filt_int = temp_filt * 10;
            temp_filt = (float)temp_filt_int / 10.0;
#ifdef KDEBUG
            Debug.print("Temp_filt: %.2f", temp_filt);
            Debug.println("°C  ");
#endif
            Knx.write(0, temp_filt);
#ifdef KDEBUG
            Debug.print("Hum: %.2f", hum);
            Debug.println(" Prozent");
#endif
            Knx.write(1, hum);
            Knx.task();
            //******* OneWire ****************************
            temp_OneWire_filt_int = temp_OneWire_filt * 10;
            temp_OneWire_filt = (float)temp_OneWire_filt_int / 10.0;
#ifdef KDEBUG
            Debug.print("OneWire Temperature: %.2f", temp_OneWire);
            Debug.println("°C");
#endif
            Knx.write(4, temp_OneWire_filt);
            digitalWrite(LED_YELLOW, LOW);
        }

        if (isMeasure == true) {
            isMeasure = false;

            //******* Temp & HUM  ****************************
#ifdef I2COFF
            temp = (hdc1080.readTemperature()) - temp_cal;
            temp_filt = mittelWert(temp);
            hum = hdc1080.readHumidity();
#endif

            Knx.task();
            //******* OneWire ****************************
            if (OneWire_active == true) {
#ifdef KDEBUG
                Debug.print("Requesting temperatures...");
#endif
#ifdef I2COFF
                sensors.requestTemperatures(); // Send the command to get temperatures
#endif
#ifdef KDEBUG
                Debug.println("DONE");
#endif
#ifdef I2COFF
                temp_OneWire = sensors.getTempCByIndex(0);
                temp_OneWire_filt = mittelWert(temp_OneWire);
#endif
            }
            //******* VOC & eCO2 ****************************

        } //ENDE ISDELAY
    }     //ENDE KONNEKTING APPLIKATION RUNING
}

float mittelWert(float neuerWert) {
    // neuen int Wert eingeben und den Mittelwert als float zurück geben
    //
    // Matthias Busse 9.2.2016 Version 1.0
    float mittel, summe = 0;
    werte[zaehlerMittelWerte] = neuerWert;
    for (int k = 0; k < anzahlMittelWerte; k++)
        summe += werte[k];
    mittel = (float)summe / anzahlMittelWerte;
    zaehlerMittelWerte++;
    if (zaehlerMittelWerte >= anzahlMittelWerte)
        zaehlerMittelWerte = 0;
    return mittel;
}
