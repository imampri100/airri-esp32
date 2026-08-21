#ifndef IRRIGATION_SENSOR_LOG_REPOSITORY_H
#define IRRIGATION_SENSOR_LOG_REPOSITORY_H

#include <vector>

#include "domain/entity/sensor_log.h"

namespace irrigation
{

class SensorLogRepository
{
public:

    virtual ~SensorLogRepository() = default;

    virtual void add(
        const SensorLog& log
    ) = 0;

    virtual std::vector<SensorLog> getAfterId(
        uint32_t lastId,
        uint32_t limit
    ) = 0;

    virtual uint32_t getLastId() = 0;

    virtual uint32_t getFirstId() = 0;

    virtual uint32_t count() = 0;

    virtual void clear() = 0;
};

}

#endif