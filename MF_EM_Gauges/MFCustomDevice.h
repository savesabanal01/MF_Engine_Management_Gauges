#pragma once

#include <Arduino.h>
// #include "MF_EM_Gauges.h"
#include "FFGauge/FFGauge.h"
#include "FuelGauge/FuelGauge.h"
#include "OTOPGauge/OTOPGauge.h"
#include "NPGauge/NPGauge.h"

// only one entry required if you have only one custom device
enum {
    FF_GAUGE = 1,
    FUEL_GAUGE,
    OTOP_GAUGE,
    NP_GAUGE,
    LAST_INSTRUMENT // MUST be always at last position!!
};

class MFCustomDevice
{
public:
    MFCustomDevice();
    void attach(uint16_t adrPin, uint16_t adrType, uint16_t adrConfig, bool configFromFlash = false);
    void detach();
    void update();
    void set(int16_t messageID, char *setPoint);

private:
    bool     getStringFromMem(uint16_t addreeprom, char *buffer, bool configFromFlash);
    bool     _initialized = false;
    uint8_t  _customType  = 0;
    uint32_t demoMillis   = millis();
    uint16_t loopCounter  = 0;
    uint32_t startMillis  = millis();
    uint16_t interval     = 10;
    String   fps          = "xx.xx fps";
    int16_t  roll = 0, pitch = 0; // only for Demo required, should be deleted for production
    bool     roll_up_down = true; // only for Demo required, should be deleted for production
    void     demo();              // only for Demo required, should be deleted for production

    void calcFPS();
};