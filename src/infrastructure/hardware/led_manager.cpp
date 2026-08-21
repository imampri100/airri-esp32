#include "infrastructure/hardware/led_manager.h"

#include <Arduino.h>

#include "infrastructure/config/pin_config.h"

namespace irrigation
{

    namespace
    {
        constexpr unsigned long BLINK_INTERVAL_MS = 400;
    }

    bool LedManager::begin()
    {
        pinMode(PinConfig::LED_GREEN, OUTPUT);
        pinMode(PinConfig::LED_YELLOW, OUTPUT);
        pinMode(PinConfig::LED_RED, OUTPUT);

        setState(LedState::Idle);

        return true;
    }

    void LedManager::setState(LedState state)
    {
        currentState = state;
        blinkOn = true;
        lastBlinkAt = millis();

        applyState(true);
    }

    void LedManager::tick()
    {
        bool needsBlink =
            currentState == LedState::Syncing ||
            currentState == LedState::Error;

        if (!needsBlink)
        {
            return;
        }

        unsigned long now = millis();
        if (now - lastBlinkAt >= BLINK_INTERVAL_MS)
        {
            lastBlinkAt = now;
            blinkOn = !blinkOn;
            applyState(blinkOn);
        }
    }

    void LedManager::applyState(bool ledOn)
    {
        digitalWrite(PinConfig::LED_GREEN, LOW);
        digitalWrite(PinConfig::LED_YELLOW, LOW);
        digitalWrite(PinConfig::LED_RED, LOW);

        switch (currentState)
        {
        case LedState::Idle:
            digitalWrite(PinConfig::LED_GREEN, HIGH);
            break;

        case LedState::Watering:
            digitalWrite(PinConfig::LED_YELLOW, HIGH);
            break;

        case LedState::Syncing:
            digitalWrite(PinConfig::LED_YELLOW, ledOn ? HIGH : LOW);
            break;

        case LedState::Error:
            digitalWrite(PinConfig::LED_RED, ledOn ? HIGH : LOW);
            break;
        }
    }

}
