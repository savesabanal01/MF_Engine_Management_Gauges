#include "NPGauge.h"
#include "allocateMem.h"
#include "commandmessenger.h"

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle.h"
#include "./include/red_led.h"
#include "./include/red_marker.h"

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

NPGauge::NPGauge(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void NPGauge::begin()
{
}

void NPGauge::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
    // tft.begin();
    tft.init();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setPivot(120, 120);
    tft.startWrite();
    
#ifdef USE_DMA_TO_TFT
    tft.initDMA(); // Initialise the DMA engine (tested with STM32F446 and STM32F767)
#endif

    mainGaugeSprPtr = (uint16_t *)mainGaugeSpr.createSprite(240, 240);
    mainGaugeSpr.setPivot(120, 120);
    mainGaugeSpr.loadFont(DotMatrix_Regular_30);
    mainGaugeSpr.setTextColor(TFT_GREEN);
    mainGaugeSpr.setTextDatum(TR_DATUM);

    needleSpr.createSprite(NEEDLE_WIDTH, NEEDLE_HEIGHT);
    needleSpr.setPivot(NEEDLE_WIDTH / 2, 80);
    needleSpr.pushImage(0, 0, NEEDLE_WIDTH, NEEDLE_HEIGHT, needle);

    redLEDSpr.createSprite(RED_LED_WIDTH, RED_LED_HEIGHT);
    redLEDSpr.pushImage(0, 0, RED_LED_WIDTH, RED_LED_HEIGHT, red_led);

    redMarkerSpr.createSprite(RED_MARKER_WIDTH, RED_MARKER_HEIGHT);
    redMarkerSpr.pushImage(0, 0, RED_MARKER_WIDTH, RED_LED_HEIGHT, red_marker);
    redMarkerSpr.setPivot(RED_MARKER_WIDTH / 2, 110);

}

void NPGauge::detach()
{
    if (!_initialised)
        return;
    mainGaugeSpr.deleteSprite();
    needleSpr.deleteSprite();
    redLEDSpr.deleteSprite();
    redMarkerSpr.deleteSprite();
    tft.endWrite();
    _initialised = false;
}

void NPGauge::set(int16_t messageID, char *setPoint)
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

void NPGauge::update()
{
    // Do something which is required regulary
    drawGauge();
}

void NPGauge::drawGauge()
{

    
    minGreenAngle = scaleValue(minGreenRPM, 0, 2400, -110, 110);
    maxGreenAngle = scaleValue(maxGreenRPM, 0, 2400, -110, 110);
    redlineRPMAngle = scaleValue(redlineRPM, 0, 2400, -110, 110);

    
    mainGaugeSpr.fillSprite(TFT_BLACK);
    mainGaugeSpr.pushImage(0, 0, 240, 240, main_gauge);
    mainGaugeSpr.drawString(String((int)RPM), 168, 170);
    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, minGreenAngle + 180, maxGreenAngle + 180, TFT_GREEN, TFT_BLACK);

    needleRotationAngle = scaleValue(RPM, 0, 2400, -110, 110);
    redMarkerSpr.pushRotated(&mainGaugeSpr, redlineRPMAngle, TFT_BLACK);
    needleSpr.pushRotated(&mainGaugeSpr, needleRotationAngle, TFT_BLUE);

    if (RPM >= redlineRPM )
        redLEDSpr.pushToSprite(&mainGaugeSpr, 38, 159, TFT_BLACK);

#ifdef USE_DMA_TO_TFT
    while(tft.dmaBusy()) {}
    tft.pushImageDMA(0, 0, 240, 240, mainGaugeSprPtr);
#else
    mainGaugeSpr.pushSprite(0, 0, TFT_BLACK);
#endif

}

void NPGauge::setRPM (float value)
{
    RPM = value;
    drawGauge();
}

void NPGauge::setInstrumentBrightnessRatio(float ratio)
{
    instrumentBrightnessRatio = ratio;
    instrumentBrightness      = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));

    analogWrite(backlight_pin, instrumentBrightness);
}

void NPGauge::setPowerSave(bool enabled)
{
    if (enabled) {
        analogWrite(backlight_pin, 0);
        powerSaveFlag = true;
    } else {
        analogWrite(backlight_pin, instrumentBrightness);
        powerSaveFlag = false;
    }
}

float NPGauge::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}