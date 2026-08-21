#ifndef IRRIGATION_SENSOR_MANAGER_H
#define IRRIGATION_SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include <BH1750.h>

#include "domain/entity/sensor_reading.h"
#include "domain/repository/sensor_repository.h"

namespace irrigation
{

    class SensorManager : public SensorRepository
    {
    public:
        bool begin();

        SensorReading read();

        // SensorRepository (domain)
        SensorReading getCurrentReading() override { return read(); }

        // Kalibrasi soil moisture: nilai ADC mentah saat sensor benar-benar
        // KERING (di udara) dan saat benar-benar BASAH (di air). Ukur dengan
        // Serial Monitor lalu sesuaikan dua konstanta ini sesuai sensor yang dipakai.
        static constexpr int SOIL_ADC_DRY = 3000;
        static constexpr int SOIL_ADC_WET = 1200;

    private:
        Adafruit_AHTX0 aht;
        BH1750 lightMeter;

        bool ahtReady = false;
        bool bh1750Ready = false;

        float readAirTemperature();

        float readAirHumidity();

        float readLightIntensity();

        float readSoilMoisture();
    };

}

#endif
