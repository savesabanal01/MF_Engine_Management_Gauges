#include "OTOPGauge.h"
#include "allocateMem.h"
#include "commandmessenger.h"

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle_ot.h"
#include "./include/needle_op.h"
#include "./include/red_led.h"
#include "./include/red_marker.h"

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

OTOPGauge::OTOPGauge(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void OTOPGauge::begin()
{
}

void OTOPGauge::attach(uint16_t Pin3, char *init)
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

    mainGaugeSprPtr = (uint16_t *)mainGaugeSpr.createSprite(240, 240);
    mainGaugeSpr.setPivot(120, 120);

    needleOTSpr.createSprite(NEEDLE_OT_WIDTH, NEEDLE_OT_HEIGHT);
    needleOTSpr.setPivot(NEEDLE_OT_WIDTH / 2, 85);
    needleOTSpr.pushImage(0, 0, NEEDLE_OT_WIDTH, NEEDLE_OT_HEIGHT, needle_ot);

    needleOPSpr.createSprite(NEEDLE_OP_WIDTH, NEEDLE_OP_HEIGHT);
    needleOPSpr.setPivot(NEEDLE_OP_WIDTH / 2, 85);
    needleOPSpr.pushImage(0, 0, NEEDLE_OP_WIDTH, NEEDLE_OP_HEIGHT, needle_op);
    
    redLEDSpr.createSprite(RED_LED_WIDTH, RED_LED_HEIGHT);
    redLEDSpr.pushImage(0, 0, RED_LED_WIDTH, RED_LED_HEIGHT, red_led);

    redMarkerSpr.createSprite(RED_MARKER_WIDTH, RED_MARKER_HEIGHT);
    redMarkerSpr.pushImage(0, 0, RED_MARKER_WIDTH, RED_MARKER_HEIGHT, red_marker);
    redMarkerSpr.setPivot(RED_MARKER_WIDTH / 2, 110);

}

void OTOPGauge::detach()
{
    if (!_initialised)
        return;
    mainGaugeSpr.deleteSprite();
    needleOTSpr.deleteSprite();
    needleOPSpr.deleteSprite();
    redLEDSpr.deleteSprite();
    redMarkerSpr.deleteSprite();
    tft.endWrite();
    _initialised = false;
}

void OTOPGauge::set(int16_t messageID, char *setPoint)
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
        setOilTemperature(atof(setPoint));
        break;
    case 1:
        setOilPressure(atof(setPoint));
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

void OTOPGauge::update()
{
    // Do something which is required regulary
    drawGauge();
}

void OTOPGauge::drawGauge()
{

    mainGaugeSpr.fillSprite(TFT_BLACK);
    mainGaugeSpr.pushImage(0, 0, 240, 240, main_gauge);

    drawOTGauge();
    drawOPGauge();

#ifdef USE_DMA_TO_TFT
    while(tft.dmaBusy()) {}
    tft.pushImageDMA(0, 0, 240, 240, mainGaugeSprPtr);
#else
    mainGaugeSpr.pushSprite(0, 0, TFT_BLACK);
#endif
    // gaugeSpr.pushSprite(0, 0, TFT_BLACK);
}

void OTOPGauge::drawOTGauge()
{

    minOTRedAngle = scaleValue(minOTRed, -50, 150, -145, -35);
    maxOTRedAngle = scaleValue(maxOTRed, -50, 150, -145, -35);

    minOTYellowAngle = scaleValue(minOTYellow, -50, 150, -145, -35);
    maxOTYellowAngle = scaleValue(maxOTYellow, -50, 150, -145, -35);

    minOTGreenAngle = scaleValue(minOTGreen, -50, 150, -145, -35);
    maxOTGreenAngle = scaleValue(maxOTGreen, -50, 150, -145, -35);
    
    needleOTRotationAngle = scaleValue(oilTemperature, -50, 150, -145, -35);

    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, minOTYellowAngle + 180, maxOTYellowAngle + 180, TFT_YELLOW, TFT_BLACK); // Draw Yellow Line

    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, minOTGreenAngle + 180, maxOTGreenAngle + 180, TFT_GREEN, TFT_BLACK); // Draw Yellow Line

    needleOTSpr.pushRotated(&mainGaugeSpr, needleOTRotationAngle, TFT_BLUE);

    redMarkerSpr.pushRotated(&mainGaugeSpr, minOTRedAngle, TFT_BLACK); // Draw the minimum red line for Oil Temperature
    redMarkerSpr.pushRotated(&mainGaugeSpr, maxOTRedAngle, TFT_BLACK); // Draw the maximum red line for Oil Temperature

    // Draw the Red LED
    if (oilTemperature <= minOTRed || oilTemperature >= maxOTRed)
        redLEDSpr.pushToSprite(&mainGaugeSpr, 110, 204, TFT_BLACK);

}


void OTOPGauge::drawOPGauge()
{
    if (minOPRed >= 0 && minOPRed < 40)
        minOPRedAngle = scaleValue(minOPRed, 0, 40, 140, 120);
    else if (minOPRed > 120 && minOPRed <= 200)
        minOPRedAngle = scaleValue(minOPRed, 120, 200, 60, 40);
    else
        minOPRedAngle = scaleValue(minOPRed, 40, 120, 120, 60);

    if (maxOPRed >= 0 && maxOPRed < 40)
        maxOPRedAngle = scaleValue(maxOPRed, 0, 40, 140, 120);
    else if (maxOPRed > 120 && maxOPRed <= 200)
        maxOPRedAngle = scaleValue(maxOPRed, 120, 200, 60, 40);
    else
        maxOPRedAngle = scaleValue(maxOPRed, 40, 120, 120, 60);

    if (minOPYellow >= 0 && minOPYellow < 40)
        minOPYellowAngle = scaleValue(minOPYellow, 0, 40, 140, 120);
    else if (minOPYellow > 120 && minOPYellow <= 200)
        minOPYellowAngle = scaleValue(minOPYellow, 120, 200, 60, 40);
    else
        minOPYellowAngle = scaleValue(minOPYellow, 40, 120, 120, 60);

    if (maxOPYellow >= 0 && maxOPYellow < 40)
        maxOPYellowAngle = scaleValue(maxOPYellow, 0, 40, 140, 120);
    else if (maxOPYellow > 120 && maxOPYellow <= 200)
        maxOPYellowAngle = scaleValue(maxOPYellow, 120, 200, 60, 40);
    else
        maxOPYellowAngle = scaleValue(maxOPYellow, 40, 120, 120, 60);

    if (minOPGreen >= 0 && minOPGreen < 40)
        minOPGreenAngle = scaleValue(minOPGreen, 0, 40, 140, 120);
    else if (minOPGreen > 120 && minOPGreen <= 200)
        minOPGreenAngle = scaleValue(minOPGreen, 120, 200, 60, 40);
    else
        minOPGreenAngle = scaleValue(minOPGreen, 40, 120, 120, 60);

    if (maxOPGreen >= 0 && maxOPGreen < 40)
        maxOPGreenAngle = scaleValue(maxOPGreen, 0, 40, 140, 120);
    else if (maxOPGreen > 120 && maxOPGreen <= 200)
        maxOPGreenAngle = scaleValue(maxOPGreen, 120, 200, 60, 40);
    else
        maxOPGreenAngle = scaleValue(maxOPGreen, 40, 120, 120, 60);

    if (oilPressure >= 0 && oilPressure < 40)
        needleOPRotationAngle= scaleValue(oilPressure, 0, 40, 140, 120);
    else if (oilPressure > 120 && oilPressure <= 200)
        needleOPRotationAngle= scaleValue(oilPressure, 120, 200, 60, 40);
    else
        needleOPRotationAngle= scaleValue(oilPressure, 40, 120, 120, 60);

    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, maxOPYellowAngle + 180, minOPYellowAngle + 180, TFT_YELLOW, TFT_BLACK); // Draw Yellow Line

    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, maxOPGreenAngle + 180, minOPGreenAngle + 180, TFT_GREEN, TFT_BLACK); // Draw Yellow Line

    redMarkerSpr.pushRotated(&mainGaugeSpr, minOPRedAngle, TFT_BLACK); // Draw the minimum red line for Oil Temperature
    redMarkerSpr.pushRotated(&mainGaugeSpr, maxOPRedAngle, TFT_BLACK); // Draw the maximum red line for Oil Temperature

    needleOPSpr.pushRotated(&mainGaugeSpr, needleOPRotationAngle, TFT_BLUE);

    // Draw the red led
    if (oilPressure <= minOPRed || oilPressure >= maxOPRed)
        redLEDSpr.pushToSprite(&mainGaugeSpr, 110, 204, TFT_BLACK);

    // mainGaugeSpr.drawString(String(angle), 168, 170);
    // needleSpr.pushRotated(&mainGaugeSpr, angle, TFT_BLUE);

    // digitalWrite(2, HIGH);
}

void OTOPGauge::setOilTemperature (float value)
{
    oilTemperature = value;
    drawGauge();
}

void OTOPGauge::setOilPressure (float value)
{
    oilPressure = value;
    drawGauge();
}


void OTOPGauge::setInstrumentBrightnessRatio(float ratio)
{
    instrumentBrightnessRatio = ratio;
    instrumentBrightness      = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));

    analogWrite(backlight_pin, instrumentBrightness);
}

void OTOPGauge::setPowerSave(bool enabled)
{
    if (enabled) {
        analogWrite(backlight_pin, 0);
        powerSaveFlag = true;
    } else {
        analogWrite(backlight_pin, instrumentBrightness);
        powerSaveFlag = false;
    }
}

float OTOPGauge::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


