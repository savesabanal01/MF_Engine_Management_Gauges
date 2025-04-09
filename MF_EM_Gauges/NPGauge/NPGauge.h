#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>

class NPGauge
{
public:
    NPGauge(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

// Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  setInstrumentBrightnessRatio(float ratio);
    void  setRPM(float value);
    void  setPowerSave(bool enabled);
    void  drawGauge();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;

    TFT_eSPI    tft;
    TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);
    TFT_eSprite needleSpr = TFT_eSprite(&tft);
    TFT_eSprite redLEDSpr = TFT_eSprite(&tft);
    TFT_eSprite redMarkerSpr = TFT_eSprite(&tft);

    // Pointers to start of Sprites in RAM (these are then "image" pointers)
    uint16_t *mainGaugeSprPtr;

    // Variables
    float    RPM                  = 0;  // RPM Value from sim
    float    instrumentBrightness      = 255;  // Instrument Brightness Ratio from sim
    float    instrumentBrightnessRatio = 0;
    float    needleRotationAngle         = 0; // angle of rotation of needle based on the RPM

    float minGreenAngle = 0;
    float maxGreenAngle = 0;
    float redlineRPMAngle = 0;

    float minGreenRPM = 1600;
    float maxGreenRPM = 1900;
    float redlineRPM = 1900;

    int numOfDataPoints = 3;
    int counter = 0;
    float sumOfDataPoints = 0;
    float averageOfDataPoints = 0;

    bool     powerSaveFlag             = false;
    uint16_t backlight_pin = 16;

};