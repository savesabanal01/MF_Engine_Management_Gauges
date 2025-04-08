#include "FuelGauge.h"

namespace FuelGauge
{

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle_left.h"
#include "./include/needle_right.h"

// #define digits      NotoSansMonoSCB20
// #define PANEL_COLOR 0x7BEE

TFT_eSPI    tft = TFT_eSprite(&tft); 
TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft); 
TFT_eSprite needleLeftSpr = TFT_eSprite(&tft); 
TFT_eSprite needleRightSpr = TFT_eSprite(&tft); 

// Pointers to start of Sprites in RAM (these are then "image" pointers)
uint16_t *mainGaugeSprPtr;

// Function declarations
float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
void  setInstrumentBrightnessRatio(float ratio);
void  setLeftFuel(float value);     // fuel quantity for left fuel tank
void  setRightFuel(float value);    // fuel quantity for right fuel tank
void  setPowerSave(bool enabled);
// void  setScreenRotation(int rotation);
void  drawGauge();

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
// bool     showLogo                  = true;

void init(uint8_t pin_backlight)
{
    // backlight_pin = pin_backlight;
    backlight_pin = 16;
    pinMode(backlight_pin, OUTPUT);
    digitalWrite(backlight_pin, HIGH);

    // tft.begin();
    tft.init();
    tft.setRotation(0);
    tft.setPivot(120, 120);
    tft.fillScreen(TFT_BLACK);
    tft.startWrite();

#ifdef USE_DMA_TO_TFT
    tft.initDMA(); // Initialise the DMA engine (tested with STM32F446 and STM32F767)
#endif

    mainGaugeSpr.createSprite(240, 240);
    mainGaugeSpr.setPivot(120, 120);

    needleLeftSpr.createSprite(NEEDLE_LEFT_WIDTH, NEEDLE_LEFT_HEIGHT);
    needleLeftSpr.setPivot(NEEDLE_LEFT_WIDTH / 2, 85);
    needleLeftSpr.pushImage(0, 0, NEEDLE_LEFT_WIDTH, NEEDLE_LEFT_HEIGHT, needle_left);

    needleRightSpr.createSprite(NEEDLE_RIGHT_WIDTH, NEEDLE_RIGHT_HEIGHT);
    needleRightSpr.setPivot(NEEDLE_RIGHT_WIDTH / 2, 85);
    needleRightSpr.pushImage(0, 0, NEEDLE_RIGHT_WIDTH, NEEDLE_RIGHT_HEIGHT, needle_right);
}

void stop()
{
    mainGaugeSpr.deleteSprite();
    needleLeftSpr.deleteSprite();
    needleRightSpr.deleteSprite();
    tft.endWrite();
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
        setLeftFuel(atof(setPoint));
        break;
    case 1:
        setRightFuel(atof(setPoint));
        break;
    case 2:
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
    mainGaugeSpr.fillSprite(TFT_BLACK);
    mainGaugeSpr.pushImage(0, 0, 240, 240, main_gauge);

    
    needleLeftRotationAngle = scaleValue(leftFuel, 0, 25, -145, -35);
    needleLeftSpr.pushRotated(&mainGaugeSpr, needleLeftRotationAngle, TFT_BLUE);

    needleRightRotationAngle = scaleValue(rightFuel, 0, 25, 145, 35);
    needleRightSpr.pushRotated(&mainGaugeSpr, needleRightRotationAngle, TFT_BLUE);

#ifdef USE_DMA_TO_TFT
    while(tft.dmaBusy()) {}
    tft.pushImageDMA(0, 0, 240, 240, mainGaugeSprPtr);
#else
    mainGaugeSpr.pushSprite(0, 0, TFT_BLACK);
#endif

}

void setLeftFuel (float value)
{
    leftFuel = value;
    drawGauge();
}

void setRightFuel (float value)
{
    rightFuel = value;
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