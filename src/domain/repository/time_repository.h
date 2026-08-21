#ifndef IRRIGATION_TIME_REPOSITORY_H
#define IRRIGATION_TIME_REPOSITORY_H

#include "domain/value_object/timestamp.h"

namespace irrigation
{

class TimeRepository
{
public:

    virtual ~TimeRepository() = default;

    virtual Timestamp now() = 0;

    virtual bool isSynchronized() = 0;
};

}

#endif
