#include "infrastructure/network/handler/sync_handler.h"

#include <ArduinoJson.h>

namespace irrigation
{

    SyncHandler::SyncHandler(
        HttpServer &httpServer,
        GetSyncUseCase &getSyncUseCase)
        : httpServer(httpServer),
          getSyncUseCase(getSyncUseCase)
    {
    }

    void SyncHandler::handleGetSync()
    {
        SyncDto sync = getSyncUseCase.execute();

        StaticJsonDocument<384> document;

        document["storageId"] = sync.metadata.storageId;
        document["sensorLogLastId"] = sync.metadata.sensorLogLastId;
        document["irrigationLogLastId"] = sync.metadata.irrigationLogLastId;
        document["sensorLogCount"] = sync.sensorLogCount;
        document["irrigationLogCount"] = sync.irrigationLogCount;
        document["sensorLogFirstId"] = sync.sensorLogFirstId;
        document["irrigationLogFirstId"] = sync.irrigationLogFirstId;

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

}
