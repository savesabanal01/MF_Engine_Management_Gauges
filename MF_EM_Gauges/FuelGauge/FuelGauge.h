#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>

class FuelGauge
{
public:
    FuelGauge(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

    //Functio declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  setInstrumentBrightnessRatio(float ratio);
    void  setLeftFuel(float value);     // fuel quantity for left fuel tank
    void  setRightFuel(float value);    // fuel quantity for right fuel tank
    void  setPowerSave(bool enabled);
    // void  setScreenRotation(int rotation);
    void  drawGauge();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;

    TFT_eSPI    tft = TFT_eSprite(&tft); 
    TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft); 
    TFT_eSprite needleLeftSpr = TFT_eSprite(&tft); 
    TFT_eSprite needleRightSpr = TFT_eSprite(&tft); 

    // Variables
    float    leftFuel                  = 0;  // fuel quantity for left fuel tank
    float    rightFuel                  = 0;  // fuel quantity for right fuel tank
    float    instrumentBrightness      = 255;  // Instrument Brightness Ratio from sim
    float    instrumentBrightnessRatio = 0;
    float    needleLeftRotationAngle         = 0; // angle of rotation of needle based on the left fuel gauge
    float    needleRightRotationAngle         = 0; // angle of rotation of needle based on the right fuel gauge
    bool     powerSaveFlag             = false;
    uint32_t startLogoMillis           = 0;
    uint8_t  backlight_pin             = 0;
    uint16_t instrumentX0              = 0;
    uint16_t instrumentY0              = 0;

    // Pointers to start of Sprites in RAM (these are then "image" pointers)
    uint16_t *mainGaugeSprPtr;
};