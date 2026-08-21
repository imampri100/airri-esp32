#ifndef IRRIGATION_STATUS_HANDLER_H
#define IRRIGATION_STATUS_HANDLER_H

#include "application/usecase/get_status_usecase.h"

#include "infrastructure/i18n/translator.h"
#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET /api/status
    class StatusHandler
    {
    public:
        StatusHandler(
            HttpServer &httpServer,
            GetStatusUseCase &getStatusUseCase,
            Translator &translator);

        void handleGetStatus();

    private:
        HttpServer &httpServer;

        GetStatusUseCase &getStatusUseCase;

        Translator &translator;
    };

}

#endif
