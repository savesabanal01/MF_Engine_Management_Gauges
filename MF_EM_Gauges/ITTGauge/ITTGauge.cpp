#include "ITTGauge.h"

namespace ITTGauge
{

#include "./include/DotMatrix_Regular-30.h"
#include "./include/main_gauge.h"
#include "./include/needle.h"
#include "./include/red_led.h"
#include "./include/red_marker.h"

TFT_eSPI    tft;
TFT_eSprite mainGaugeSpr = TFT_eSprite(&tft);
TFT_eSprite needleSpr = TFT_eSprite(&tft);
TFT_eSprite redLEDSpr = TFT_eSprite(&tft);
TFT_eSprite redMarkerSpr = TFT_eSprite(&tft);

// TFT_eSPI    *tft;
// TFT_eSprite *mainGaugeSpr;
// TFT_eSprite *needleSpr;
// TFT_eSprite *redLEDSpr;
// TFT_eSprite *redMarkerSpr;

// Pointers to start of Sprites in RAM (these are then "image" pointers)
uint16_t *mainGaugeSprPtr;

// Function declarations
float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
void  setInstrumentBrightnessRatio(float ratio);
void  setITT(float value);
void  setPowerSave(bool enabled);
void  drawGauge();

// Variables
float    ITT                  = 0;  // ITT Value from sim
float    instrumentBrightness      = 255;  // Instrument Brightness Ratio from sim
float    instrumentBrightnessRatio = 0;
float    needleRotationAngle         = 0; // angle of rotation of needle based on the ITT

float minGreenAngle = 0;
float maxGreenAngle = 0;
float redlineITTAngle = 0;

float minGreenITT = 1600;
float maxGreenITT = 1900;
float redlineITT = 1900;

int numOfDataPoints = 3;
int counter = 0;
float sumOfDataPoints = 0;
float averageOfDataPoints = 0;

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

void stop()
{
    mainGaugeSpr.deleteSprite();
    needleSpr.deleteSprite();
    redLEDSpr.deleteSprite();
    redMarkerSpr.deleteSprite();
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
        setITT(atof(setPoint));
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

    
    minGreenAngle = scaleValue(minGreenITT, 0, 2400, -110, 110);
    maxGreenAngle = scaleValue(maxGreenITT, 0, 2400, -110, 110);
    redlineITTAngle = scaleValue(redlineITT, 0, 2400, -110, 110);

    
    mainGaugeSpr.fillSprite(TFT_BLACK);
    mainGaugeSpr.pushImage(0, 0, 240, 240, main_gauge);
    mainGaugeSpr.drawString(String((int)ITT), 168, 170);
    mainGaugeSpr.drawSmoothArc(120, 120, 205 / 2, 195 / 2, minGreenAngle + 180, maxGreenAngle + 180, TFT_GREEN, TFT_BLACK);

    needleRotationAngle = scaleValue(ITT, 200, 1200, -110, 110);
    redMarkerSpr.pushRotated(&mainGaugeSpr, redlineITTAngle, TFT_BLACK);
    needleSpr.pushRotated(&mainGaugeSpr, needleRotationAngle, TFT_BLUE);

    if (ITT >= redlineITT )
        redLEDSpr.pushToSprite(&mainGaugeSpr, 38, 159, TFT_BLACK);

#ifdef USE_DMA_TO_TFT
    while(tft.dmaBusy()) {}
    tft.pushImageDMA(0, 0, 240, 240, mainGaugeSprPtr);
#else
    mainGaugeSpr.pushSprite(0, 0, TFT_BLACK);
#endif

}

void setITT (float value)
{
    ITT = value;
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