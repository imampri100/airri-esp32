#ifndef IRRIGATION_PIN_CONFIG_H
#define IRRIGATION_PIN_CONFIG_H

#include <Arduino.h>

// jgn diubah sembarangan kalau kabelnya belum diubah juga
namespace irrigation
{

    namespace PinConfig
    {
        constexpr uint8_t SOIL_SENSOR = 32; // ADC

        // I2C (AHT10 + BH1750 + RTC, bus dishare bertiga)
        constexpr uint8_t I2C_SDA = 21;
        constexpr uint8_t I2C_SCL = 22;

        // SPI (TFT + SD Card)
        constexpr uint8_t SPI_SCK = 18;
        constexpr uint8_t SPI_MOSI = 23;
        constexpr uint8_t SPI_MISO = 19;

        constexpr uint8_t TFT_CS = 33;
        constexpr uint8_t TFT_DC = 16;  // RX2
        constexpr uint8_t TFT_RES = 17; // TX2

        // beda CS sama TFT walaupun sebus SPI
        constexpr uint8_t SD_CS = 5; 

        // active LOW
        constexpr uint8_t RELAY = 14;

        constexpr uint8_t LED_GREEN = 25;
        constexpr uint8_t LED_YELLOW = 26;
        constexpr uint8_t LED_RED = 27;

        // Pakai tombol BOOT bawaan devkit dulu
        // (GPIO0, active low). ganti kalau nanti pasang tombol eksternal
        constexpr uint8_t BUTTON = 0;

    }

}

#endif
