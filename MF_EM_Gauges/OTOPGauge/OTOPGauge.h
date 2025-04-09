#pragma once

#include "Arduino.h"
#include <TFT_eSPI.h>

class OTOPGauge
{
public:
    OTOPGauge(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();
// Function declarations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void  setInstrumentBrightnessRatio(float ratio);
    void  setOilTemperature(float value);     // set oil temperature from the sim
    void  setOilPressure(float value);    // set oil pressure from the sim
    void  setPowerSave(bool enabled);
    void drawOTGauge();
    void drawOPGauge();
    // void  setScreenRotation(int rotation);
    void  drawGauge();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    TFT_eSPI    tft = TFT_eSPI();
    TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);
    TFT_eSprite needleOTSpr = TFT_eSprite(&tft);
    TFT_eSprite needleOPSpr = TFT_eSprite(&tft);
    TFT_eSprite redLEDSpr = TFT_eSprite(&tft);
    TFT_eSprite redMarkerSpr = TFT_eSprite(&tft);

    // Pointers to start of Sprites in RAM (these are then "image" pointers)
    uint16_t *mainGaugeSprPtr;

    // Variables
    float    oilTemperature             = 0;  // oil temperature from the sim
    float    oilPressure                 = 0;  // oil Pressure from the sim
    float    instrumentBrightness      = 255;  // Instrument Brightness Ratio from sim
    float    instrumentBrightnessRatio = 0;
    float    needleOTRotationAngle         = 0; // angle of rotation of OT needle based on the oil temperature
    float    needleOPRotationAngle         = 0; // angle of rotation of OP needle based on the oil pressure

    float minOTYellow = -40; // Oil Temperature Yellow Line min value
    float maxOTYellow = 30;  // Oil Temperature Yellow Line max value
    float minOTGreen = 31;   // Oil Temperature Green Line min value
    float maxOTGreen = 100;  // Oil Temperature Green Line max value

    float maxOTRed = 100; // Oil Temperature Max Red Line
    float minOTRed = -40; // Oil Temperature Min Red Line

    float minOPYellow = 40; // Oil Temperature Yellow Line min value
    float maxOPYellow = 85; // Oil Temperature Yellow Line max value
    float minOPGreen = 85;  // Oil Temperature Green Line min value
    float maxOPGreen = 105; // Oil Temperature Green Line max value
    float maxOPRed = 200;   // Oil Temperature Max Red Line
    float minOPRed = 40;    // Oil Temperature Min Red Line

    float minOTGreenAngle = 0;
    float maxOTGreenAngle = 0;
    float minOTYellowAngle = 0;
    float maxOTYellowAngle = 0;
    float minOTRedAngle = 0;
    float maxOTRedAngle = 0;

    float minOPGreenAngle = 0;
    float maxOPGreenAngle = 0;
    float minOPYellowAngle = 0;
    float maxOPYellowAngle = 0;
    float minOPRedAngle = 0;
    float maxOPRedAngle = 0;

    bool     powerSaveFlag             = false;

    uint16_t    backlight_pin = 16;
    // bool     showLogo                  = true;

};