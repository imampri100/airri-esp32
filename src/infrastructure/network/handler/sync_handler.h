#ifndef IRRIGATION_SYNC_HANDLER_H
#define IRRIGATION_SYNC_HANDLER_H

#include "application/usecase/get_sync_usecase.h"

#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET /api/sync
    class SyncHandler
    {
    public:
        SyncHandler(
            HttpServer &httpServer,
            GetSyncUseCase &getSyncUseCase);

        void handleGetSync();

    private:
        HttpServer &httpServer;

        GetSyncUseCase &getSyncUseCase;
    };

}

#endif
