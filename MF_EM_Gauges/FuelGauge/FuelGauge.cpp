#include "FuelGauge.h"
#include "allocateMem.h"
#include "commandmessenger.h"

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle_left.h"
#include "./include/needle_right.h"



/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

FuelGauge::FuelGauge(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void FuelGauge::begin()
{
}

void FuelGauge::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
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

void FuelGauge::detach()
{
    if (!_initialised)
        return;
    mainGaugeSpr.deleteSprite();
    needleLeftSpr.deleteSprite();
    needleRightSpr.deleteSprite();
    tft.endWrite();
    _initialised = false;
}

void FuelGauge::set(int16_t messageID, char *setPoint)
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


    // do something according your messageID
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

void FuelGauge::update()
{
    // Do something which is required regulary
    drawGauge();
}

void FuelGauge::drawGauge()
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

void FuelGauge::setLeftFuel (float value)
{
    leftFuel = value;
    drawGauge();
}

void FuelGauge::setRightFuel (float value)
{
    rightFuel = value;
    drawGauge();
}


void FuelGauge::setInstrumentBrightnessRatio(float ratio)
{
    instrumentBrightnessRatio = ratio;
    instrumentBrightness      = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));

    analogWrite(backlight_pin, instrumentBrightness);
}

void FuelGauge::setPowerSave(bool enabled)
{
    if (enabled) {
        analogWrite(backlight_pin, 0);
        powerSaveFlag = true;
    } else {
        analogWrite(backlight_pin, instrumentBrightness);
        powerSaveFlag = false;
    }
}

float FuelGauge::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}