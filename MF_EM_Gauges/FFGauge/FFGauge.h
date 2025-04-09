#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>

class FFGauge
{
public:
    FFGauge(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();
// Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  setInstrumentBrightnessRatio(float ratio);
    void  setFuelFlow(float value);
    void  setPowerSave(bool enabled);
    void  drawGauge();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    
    TFT_eSPI    tft;
    TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);
    TFT_eSprite needleSpr = TFT_eSprite(&tft);;

    // Variables
    float    fuelFlow = 0; 
    float    needleRotationAngle       = 0; // angle of rotation of needle based on Fuel Flow from sim  
    float    instrumentBrightness      = 255;
    float    instrumentBrightnessRatio = 0;
    bool     powerSaveFlag             = false;
    int      prevScreenRotation        = 3;
    bool     showLogo                  = true;
    uint16_t backlight_pin = 16;

    uint16_t *mainGaugeSprPtr;
};