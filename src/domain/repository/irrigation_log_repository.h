#ifndef IRRIGATION_IRRIGATION_LOG_REPOSITORY_H
#define IRRIGATION_IRRIGATION_LOG_REPOSITORY_H

#include <vector>

#include "domain/entity/irrigation_log.h"

namespace irrigation
{

class IrrigationLogRepository
{
public:

    virtual ~IrrigationLogRepository() = default;

    virtual void add(
        const IrrigationLog& log
    ) = 0;

    virtual std::vector<IrrigationLog> getAfterId(
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