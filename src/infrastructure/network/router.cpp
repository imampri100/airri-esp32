#include "infrastructure/network/router.h"

namespace irrigation
{

    Router::Router(
        HttpServer &httpServer,
        StatusHandler &statusHandler,
        SensorLogHandler &sensorLogHandler,
        IrrigationLogHandler &irrigationLogHandler,
        SyncHandler &syncHandler,
        SettingHandler &settingHandler,
        PumpHandler &pumpHandler,
        MaintenanceHandler &maintenanceHandler)
        : httpServer(httpServer),
          statusHandler(statusHandler),
          sensorLogHandler(sensorLogHandler),
          irrigationLogHandler(irrigationLogHandler),
          syncHandler(syncHandler),
          settingHandler(settingHandler),
          pumpHandler(pumpHandler),
          maintenanceHandler(maintenanceHandler)
    {
    }

    void Router::registerRoutes()
    {
        WebServer &server = httpServer.raw();

        // Ping ringan buat cek konektivitas - sengaja tidak lewat handler
        // manapun (tidak ada dependency/use case), tidak nyentuh sensor
        // atau SD Card sama sekali, jadi bisa di-poll sesering apapun
        // tanpa beban seperti GET /api/status.
        server.on("/api/ping", HTTP_GET, [this]()
                  { httpServer.sendJson(200, "{\"pong\":true}"); });

        server.on("/api/status", HTTP_GET, [this]()
                  { statusHandler.handleGetStatus(); });

        server.on("/api/logs/sensor", HTTP_GET, [this]()
                  { sensorLogHandler.handleGetSensorLogs(); });

        server.on("/api/logs/irrigation", HTTP_GET, [this]()
                  { irrigationLogHandler.handleGetIrrigationLogs(); });

        server.on("/api/logs", HTTP_DELETE, [this]()
                  { maintenanceHandler.handleClearLogs(); });

        server.on("/api/sync", HTTP_GET, [this]()
                  { syncHandler.handleGetSync(); });

        server.on("/api/settings/trigger", HTTP_GET, [this]()
                  { settingHandler.handleGetTrigger(); });

        server.on("/api/settings/trigger", HTTP_PUT, [this]()
                  { settingHandler.handlePutTrigger(); });

        server.on("/api/settings/restriction", HTTP_GET, [this]()
                  { settingHandler.handleGetRestriction(); });

        server.on("/api/settings/restriction", HTTP_PUT, [this]()
                  { settingHandler.handlePutRestriction(); });

        server.on("/api/settings/language", HTTP_GET, [this]()
                  { settingHandler.handleGetLanguage(); });

        server.on("/api/settings/language", HTTP_PUT, [this]()
                  { settingHandler.handlePutLanguage(); });

        server.on("/api/pump/info", HTTP_GET, [this]()
                  { pumpHandler.handleInfo(); });

        server.on("/api/pump/start", HTTP_POST, [this]()
                  { pumpHandler.handleStart(); });

        server.on("/api/pump/stop", HTTP_POST, [this]()
                  { pumpHandler.handleStop(); });

        server.on("/api/pump/test", HTTP_POST, [this]()
                  { pumpHandler.handleTest(); });

        server.on("/api/maintenance/factory-reset", HTTP_POST, [this]()
                  { maintenanceHandler.handleFactoryReset(); });

        // CORS preflight untuk semua path (dibutuhkan browser/Flutter web).
        server.onNotFound([this]()
                           {
            WebServer &s = httpServer.raw();

            if (s.method() == HTTP_OPTIONS)
            {
                s.sendHeader("Access-Control-Allow-Origin", "*");
                s.sendHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
                s.sendHeader("Access-Control-Allow-Headers", "Content-Type");
                s.send(204);
                return;
            }

            httpServer.sendJsonError(404, "Endpoint tidak ditemukan"); });
    }

}
