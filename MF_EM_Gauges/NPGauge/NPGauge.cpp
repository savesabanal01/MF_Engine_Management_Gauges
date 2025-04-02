#include "NPGauge.h"

namespace NPGauge
{

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle.h"
#include "./include/red_led.h"
#include "./include/red_marker.h"


TFT_eSPI    *tft;
TFT_eSprite *mainGaugeSpr;
TFT_eSprite *needleSpr;
TFT_eSprite *redLEDSpr;
TFT_eSprite *redMarkerSpr;

// Pointers to start of Sprites in RAM (these are then "image" pointers)
uint16_t *mainGaugeSprPtr;

// Function declarations
float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
void  setInstrumentBrightnessRatio(float ratio);
void  setRPM(float value);
void  setPowerSave(bool enabled);
void  drawGauge();

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

bool     powerSaveFlag             = false;
uint32_t startLogoMillis           = 0;
uint8_t  backlight_pin             = 0;
uint16_t instrumentX0              = 0;
uint16_t instrumentY0              = 0;
// bool     showLogo                  = true;

void init(TFT_eSPI *_tft, TFT_eSprite *sprites, uint8_t pin_backlight)
{
    // backlight_pin = pin_backlight;
    backlight_pin = 16;
    pinMode(backlight_pin, OUTPUT);
    digitalWrite(backlight_pin, HIGH);

    tft = _tft;
    tft->setRotation(0);
    tft->setPivot(120, 120);
    tft->fillScreen(TFT_BLACK);
    tft->startWrite(); // TFT chip select held low permanently

    mainGaugeSpr    = &sprites[0];
    needleSpr       = &sprites[1];
    redLEDSpr       = &sprites[2];
    redMarkerSpr    = &sprites[3];

    mainGaugeSprPtr = (uint16_t *)mainGaugeSpr->createSprite(240, 240);
    mainGaugeSpr->setPivot(240, 240);

    needleSpr->createSprite(NEEDLE_WIDTH, NEEDLE_HEIGHT);
    needleSpr->setPivot(NEEDLE_WIDTH / 2, 80);
    needleSpr->pushImage(0, 0, NEEDLE_WIDTH, NEEDLE_HEIGHT, needle);

    redLEDSpr->createSprite(RED_LED_WIDTH, RED_LED_HEIGHT);
    redLEDSpr->pushImage(0, 0, RED_LED_WIDTH, RED_LED_HEIGHT, red_led);

    redMarkerSpr->createSprite(RED_MARKER_WIDTH, RED_MARKER_HEIGHT);
    redMarkerSpr->setPivot(RED_MARKER_WIDTH / 2, 110);

}

void stop()
{
    tft->endWrite();
    mainGaugeSpr->deleteSprite();
    needleSpr->deleteSprite();
    redLEDSpr->deleteSprite();
    redMarkerSpr->deleteSprite();
}

void set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */

    switch (messageID) {
    case -1:
        setPowerSave(true);
    case -2:
        setPowerSave((bool)atoi(setPoint));
        break;
    case 0:
        setRPM(atof(setPoint));
        break;
    case 1:
        setInstrumentBrightnessRatio(atof(setPoint));
        break;
    case 100:
        // setScreenRotation(atoi(setPoint));
        break;
    default:
        break;
    }
}

void update()
{
    drawGauge();
}

void drawGauge()
{
    mainGaugeSpr->fillSprite(TFT_BLACK);
    mainGaugeSpr->pushImage(0, 0, 240, 240, main_gauge);
    mainGaugeSpr->drawString(String((int)RPM), 168, 170);
    mainGaugeSpr->drawSmoothArc(120, 120, 205 / 2, 195 / 2, minGreenAngle + 180, maxGreenAngle + 180, TFT_GREEN, TFT_BLACK);

    redMarkerSpr->pushRotated(mainGaugeSpr, redlineRPMAngle, TFT_BLACK);
    needleSpr->pushRotated(mainGaugeSpr, needleRotationAngle, TFT_BLUE);

    if (RPM >= redlineRPM )
        redLEDSpr->pushToSprite(mainGaugeSpr, 38, 159, TFT_BLACK);
    
    tft->pushImageDMA(instrumentX0, instrumentY0, 240, 240, mainGaugeSprPtr);
    // gaugeSpr.pushSprite(0, 0, TFT_BLACK);
}

void setRPM (float value)
{
    RPM = value;
    drawGauge();
}

void setInstrumentBrightnessRatio(float ratio)
{
    instrumentBrightnessRatio = ratio;
    instrumentBrightness      = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));

    analogWrite(backlight_pin, instrumentBrightness);
}

void setPowerSave(bool enabled)
{
    if (enabled) {
        analogWrite(backlight_pin, 0);
        powerSaveFlag = true;
    } else {
        analogWrite(backlight_pin, instrumentBrightness);
        powerSaveFlag = false;
    }
}

float scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

}