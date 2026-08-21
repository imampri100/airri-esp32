#ifndef IRRIGATION_SENSOR_READING_H
#define IRRIGATION_SENSOR_READING_H

namespace irrigation
{

struct SensorReading
{
    float soilMoisture = 0.0f;

    float airHumidity = 0.0f;

    float airTemperature = 0.0f;

    float lightIntensity = 0.0f;
};

}

#endif