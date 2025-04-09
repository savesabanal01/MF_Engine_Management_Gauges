#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace ITTGauge
{
    void init(uint8_t pin_backlight);
    void stop();
    void set(int16_t messageID, char *setPoint);
    void update();

}