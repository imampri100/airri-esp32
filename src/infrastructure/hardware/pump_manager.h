#ifndef IRRIGATION_PUMP_MANAGER_H
#define IRRIGATION_PUMP_MANAGER_H

#include "domain/repository/pump_repository.h"

namespace irrigation
{

    class PumpManager : public PumpRepository
    {
    public:
        bool begin();

        void start() override;

        void stop() override;

        bool isRunning() override;

    private:
        bool running = false;
    };

}

#endif
