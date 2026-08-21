#include "infrastructure/hardware/button_manager.h"

#include <Arduino.h>

#include "infrastructure/config/pin_config.h"

namespace irrigation
{

    bool ButtonManager::begin()
    {
        pinMode(
            PinConfig::BUTTON,
            INPUT_PULLUP);

        return true;
    }

    bool ButtonManager::isPressed()
    {
        return digitalRead(
                   PinConfig::BUTTON) == LOW;
    }

}