#ifndef IRRIGATION_DISPLAY_MANAGER_H
#define IRRIGATION_DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#include "domain/entity/sensor_reading.h"

#include "infrastructure/i18n/translator.h"

namespace irrigation
{

    class DisplayManager
    {
    public:
        explicit DisplayManager(Translator &translator);

        ~DisplayManager();

        bool begin();

        void clear();

        void print(const String &text);

        void showStatus(
            const SensorReading &reading,
            bool pumpRunning,
            const String &clockText,
            const String &apIpText,
            const String &staIpText,
            const String &statusText);

    private:
        Translator &translator;

        Arduino_DataBus *bus = nullptr;
        Arduino_GFX *gfx = nullptr;

        // Cetak satu baris teks, di-center horizontal berdasarkan lebar
        // teks sebenarnya (getTextBounds) - dipakai supaya tiap baris
        // otomatis pas di tengah layar bundar terlepas dari panjang
        // teksnya (mis. IP address beda panjang tiap device/jaringan).
        void printCentered(const String &text, int16_t y, uint8_t textSize, uint16_t color);
    };

}

#endif
