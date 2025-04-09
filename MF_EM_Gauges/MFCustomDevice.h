#pragma once

#include <Arduino.h>
#include "FFGauge/FFGauge.h"
#include "FuelGauge/FuelGauge.h"
#include "NPGauge/NPGauge.h"
#include "ITTGauge/ITTGauge.h"
#include "OTOPGauge/OTOPGauge.h"
#include "TRQGauge/TRQGauge.h"

// only one entry required if you have only one custom device
enum {
    FF_GAUGE = 1,
    FUEL_GAUGE,
    ITT_GAUGE,
    NP_GAUGE,
    OTOP_GAUGE,
    TRQ_GAUGE
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
    bool           getStringFromMem(uint16_t addreeprom, char *buffer, bool configFromFlash);
    bool           _initialized = false;
    FFGauge        *_myFFGauge;
    FuelGauge      *_myFuelGauge;
    ITTGauge       *_myITTGauge;
    NPGauge        *_myNPGauge;
    OTOPGauge      *_myOTOPGauge;
    TRQGauge       *_myTRQGauge;
    uint8_t        _pin1, _pin2, _pin3;
    uint8_t        _customType = 0;
};
