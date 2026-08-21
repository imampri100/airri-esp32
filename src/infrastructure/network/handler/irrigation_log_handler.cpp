#include "infrastructure/network/handler/irrigation_log_handler.h"

#include <ArduinoJson.h>

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    namespace
    {
        constexpr uint32_t DEFAULT_LIMIT = 50;
        constexpr uint32_t MAX_LIMIT = 200;
    }

    IrrigationLogHandler::IrrigationLogHandler(
        HttpServer &httpServer,
        GetIrrigationLogsUseCase &getIrrigationLogsUseCase)
        : httpServer(httpServer),
          getIrrigationLogsUseCase(getIrrigationLogsUseCase)
    {
    }

    void IrrigationLogHandler::handleGetIrrigationLogs()
    {
        WebServer &server = httpServer.raw();

        uint32_t lastId = 0;
        uint32_t limit = DEFAULT_LIMIT;

        if (server.hasArg("lastId"))
        {
            lastId = server.arg("lastId").toInt();
        }

        if (server.hasArg("limit"))
        {
            limit = server.arg("limit").toInt();
        }

        if (limit == 0 || limit > MAX_LIMIT)
        {
            limit = MAX_LIMIT;
        }

        // Instrumentasi sementara untuk cari tahu fase mana yang paling
        // makan waktu (baca SD Card vs build JSON vs kirim response).
        unsigned long startMs = millis();

        std::vector<IrrigationLog> logs =
            getIrrigationLogsUseCase.execute(lastId, limit);

        unsigned long fetchDoneMs = millis();

        // Kapasitas dihitung dari jumlah record aktual (6 field/objek),
        // bukan angka tetap, supaya tidak diam-diam terpotong saat
        // hasil query mendekati/melebihi MAX_LIMIT.
        size_t capacity =
            JSON_ARRAY_SIZE(logs.size()) +
            logs.size() * JSON_OBJECT_SIZE(6);

        if (capacity < 128)
        {
            capacity = 128;
        }

        DynamicJsonDocument document(capacity);
        JsonArray array = document.to<JsonArray>();

        for (const IrrigationLog &log : logs)
        {
            JsonObject item = array.createNestedObject();
            item["id"] = log.id;
            item["createdAt"] = log.createdAt.value();
            item["irrigationRunAt"] = log.irrigationRunAt.value();
            item["irrigationStopAt"] = log.irrigationStopAt.value();
            item["irrigationDurationSecond"] = log.irrigationDurationSecond;
            item["irrigationMillilitre"] = log.irrigationMillilitre;
        }

        String body;
        serializeJson(document, body);

        unsigned long jsonDoneMs = millis();

        httpServer.sendJson(200, body);

        unsigned long sendDoneMs = millis();

        SerialLogger::info(
            "GET /api/logs/irrigation lastId=" + String(lastId) +
            " limit=" + String(limit) +
            " result=" + String(logs.size()) +
            " fetchMs=" + String(fetchDoneMs - startMs) +
            " jsonMs=" + String(jsonDoneMs - fetchDoneMs) +
            " sendMs=" + String(sendDoneMs - jsonDoneMs) +
            " totalMs=" + String(sendDoneMs - startMs));
    }

}
