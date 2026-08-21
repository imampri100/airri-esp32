#include "infrastructure/hardware/pump_manager.h"

#include <Arduino.h>

#include "infrastructure/config/pin_config.h"

namespace irrigation
{

    bool PumpManager::begin()
    {
        pinMode(
            PinConfig::RELAY,
            OUTPUT);

        stop();

        return true;
    }

    void PumpManager::start()
    {
        digitalWrite(
            PinConfig::RELAY,
            HIGH);

        running = true;
    }

    void PumpManager::stop()
    {
        digitalWrite(
            PinConfig::RELAY,
            LOW);

        running = false;
    }

    bool PumpManager::isRunning()
    {
        return running;
    }

}