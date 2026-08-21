#ifndef IRRIGATION_PUMP_REPOSITORY_H
#define IRRIGATION_PUMP_REPOSITORY_H

namespace irrigation
{

class PumpRepository
{
public:

    virtual ~PumpRepository() = default;

    virtual void start() = 0;

    virtual void stop() = 0;

    virtual bool isRunning() = 0;
};

}

#endif