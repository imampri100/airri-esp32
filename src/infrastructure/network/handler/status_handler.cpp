#include "infrastructure/network/handler/status_handler.h"

#include <ArduinoJson.h>

namespace irrigation
{

    StatusHandler::StatusHandler(
        HttpServer &httpServer,
        GetStatusUseCase &getStatusUseCase,
        Translator &translator)
        : httpServer(httpServer),
          getStatusUseCase(getStatusUseCase),
          translator(translator)
    {
    }

    void StatusHandler::handleGetStatus()
    {
        StatusDto status = getStatusUseCase.execute();

        StaticJsonDocument<768> document;

        JsonObject sensor = document.createNestedObject("sensor");
        sensor["soilMoisture"] = status.sensorReading.soilMoisture;
        sensor["airHumidity"] = status.sensorReading.airHumidity;
        sensor["airTemperature"] = status.sensorReading.airTemperature;
        sensor["lightIntensity"] = status.sensorReading.lightIntensity;

        JsonObject trigger = document.createNestedObject("trigger");
        trigger["soilMoistureOperator"] = status.triggerSetting.soilMoistureOperator;
        trigger["soilMoistureValue"] = status.triggerSetting.soilMoistureValue;

        JsonObject restriction = document.createNestedObject("restriction");
        restriction["airHumidityEnabled"] = status.restrictionSetting.airHumidityEnabled;
        restriction["airHumidityOperator"] = status.restrictionSetting.airHumidityOperator;
        restriction["airHumidityValue"] = status.restrictionSetting.airHumidityValue;
        restriction["airTemperatureEnabled"] = status.restrictionSetting.airTemperatureEnabled;
        restriction["airTemperatureOperator"] = status.restrictionSetting.airTemperatureOperator;
        restriction["airTemperatureValue"] = status.restrictionSetting.airTemperatureValue;
        restriction["lightIntensityEnabled"] = status.restrictionSetting.lightIntensityEnabled;
        restriction["lightIntensityOperator"] = status.restrictionSetting.lightIntensityOperator;
        restriction["lightIntensityValue"] = status.restrictionSetting.lightIntensityValue;
        restriction["maxPumpRuntimeSecond"] = status.restrictionSetting.maxPumpRuntimeSecond;

        JsonObject decision = document.createNestedObject("decision");
        decision["shouldRunPump"] = status.decisionResult.shouldRunPump;
        decision["reason"] = translator.reasonText(status.decisionResult.reason);

        document["pumpRunning"] = status.pumpRunning;
        document["timeSynchronized"] = status.timeSynchronized;

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

}
