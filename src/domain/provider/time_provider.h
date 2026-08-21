#ifndef IRRIGATION_TIME_PROVIDER_H
#define IRRIGATION_TIME_PROVIDER_H

#include "domain/value_object/timestamp.h"

namespace irrigation
{

class TimeProvider
{
public:

    virtual ~TimeProvider() = default;

    virtual bool begin() = 0;

    virtual bool synchronize() = 0;

    virtual bool isSynchronized() const = 0;

    virtual Timestamp now() const = 0;
};

}

#endif