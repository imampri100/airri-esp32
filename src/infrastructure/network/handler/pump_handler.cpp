#include "infrastructure/network/handler/pump_handler.h"

#include <ArduinoJson.h>

#include "infrastructure/config/pump_config.h"

namespace irrigation
{

    PumpHandler::PumpHandler(
        HttpServer &httpServer,
        PumpRepository &pumpRepository,
        PumpTestUseCase &pumpTestUseCase)
        : httpServer(httpServer),
          pumpRepository(pumpRepository),
          pumpTestUseCase(pumpTestUseCase)
    {
    }

    void PumpHandler::handleInfo()
    {
        StaticJsonDocument<64> document;
        document["flowRateMlPerMinute"] = PumpConfig::FLOW_RATE_ML_PER_MINUTE;

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

    void PumpHandler::handleStart()
    {
        pumpRepository.start();
        sendPumpState();
    }

    void PumpHandler::handleStop()
    {
        pumpRepository.stop();
        sendPumpState();
    }

    void PumpHandler::handleTest()
    {
        WebServer &server = httpServer.raw();

        uint32_t durationSecond = 3;

        if (server.hasArg("plain"))
        {
            StaticJsonDocument<128> document;
            if (!deserializeJson(document, server.arg("plain")))
            {
                durationSecond = document["durationSecond"] | durationSecond;
            }
        }

        // Blocking selama durasi test (dibatasi PumpTestUseCase, maks
        // beberapa detik) - HTTP client akan menunggu respons sampai
        // pompa selesai diuji.
        pumpTestUseCase.execute(durationSecond);

        sendPumpState();
    }

    void PumpHandler::sendPumpState()
    {
        StaticJsonDocument<64> document;
        document["running"] = pumpRepository.isRunning();

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

}
