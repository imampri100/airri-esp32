#ifndef IRRIGATION_SENSOR_REPOSITORY_H
#define IRRIGATION_SENSOR_REPOSITORY_H

#include "domain/entity/sensor_reading.h"

namespace irrigation
{

class SensorRepository
{
public:

    virtual ~SensorRepository() = default;

    virtual SensorReading getCurrentReading() = 0;
};

}

#endif