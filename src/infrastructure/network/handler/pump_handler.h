#ifndef IRRIGATION_PUMP_HANDLER_H
#define IRRIGATION_PUMP_HANDLER_H

#include "application/usecase/pump_test_usecase.h"

#include "domain/repository/pump_repository.h"

#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET /api/pump/info, POST /api/pump/start, /api/pump/stop, /api/pump/test
    class PumpHandler
    {
    public:
        PumpHandler(
            HttpServer &httpServer,
            PumpRepository &pumpRepository,
            PumpTestUseCase &pumpTestUseCase);

        // Info statis pompa (konstanta kompilasi, bukan setting yang bisa
        // di-PUT) - saat ini cuma flowRateMlPerMinute, lihat PumpConfig.
        void handleInfo();

        void handleStart();

        void handleStop();

        void handleTest();

    private:
        HttpServer &httpServer;

        PumpRepository &pumpRepository;

        PumpTestUseCase &pumpTestUseCase;

        void sendPumpState();
    };

}

#endif
