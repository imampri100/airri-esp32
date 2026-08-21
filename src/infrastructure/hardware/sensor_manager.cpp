#include "infrastructure/hardware/sensor_manager.h"

#include <Wire.h>

#include "infrastructure/config/pin_config.h"
#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    bool SensorManager::begin()
    {
        Wire.begin(PinConfig::I2C_SDA, PinConfig::I2C_SCL);

        ahtReady = aht.begin(&Wire);
        if (!ahtReady)
        {
            SerialLogger::error("AHT10 tidak terdeteksi di bus I2C");
        }

        bh1750Ready = lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire);
        if (!bh1750Ready)
        {
            SerialLogger::error("BH1750 tidak terdeteksi di bus I2C");
        }

        analogReadResolution(12);
        analogSetPinAttenuation(PinConfig::SOIL_SENSOR, ADC_11db);
        pinMode(PinConfig::SOIL_SENSOR, INPUT);

        return ahtReady && bh1750Ready;
    }

    SensorReading SensorManager::read()
    {
        SensorReading reading;

        reading.airTemperature = readAirTemperature();
        reading.airHumidity = readAirHumidity();
        reading.lightIntensity = readLightIntensity();
        reading.soilMoisture = readSoilMoisture();

        return reading;
    }

    float SensorManager::readAirTemperature()
    {
        if (!ahtReady)
        {
            return 0.0f;
        }

        sensors_event_t humidity;
        sensors_event_t temperature;
        aht.getEvent(&humidity, &temperature);

        return temperature.temperature;
    }

    float SensorManager::readAirHumidity()
    {
        if (!ahtReady)
        {
            return 0.0f;
        }

        sensors_event_t humidity;
        sensors_event_t temperature;
        aht.getEvent(&humidity, &temperature);

        return humidity.relative_humidity;
    }

    float SensorManager::readLightIntensity()
    {
        if (!bh1750Ready)
        {
            return 0.0f;
        }

        float lux = lightMeter.readLightLevel();

        if (lux < 0)
        {
            return 0.0f;
        }

        return lux;
    }

    float SensorManager::readSoilMoisture()
    {
        int raw = analogRead(PinConfig::SOIL_SENSOR);

        long percent = map(
            raw,
            SOIL_ADC_DRY,
            SOIL_ADC_WET,
            0,
            100);

        if (percent < 0)
        {
            percent = 0;
        }
        if (percent > 100)
        {
            percent = 100;
        }

        return static_cast<float>(percent);
    }

}
