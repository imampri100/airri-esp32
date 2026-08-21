#include "infrastructure/network/handler/setting_handler.h"

#include <ArduinoJson.h>

namespace irrigation
{

    SettingHandler::SettingHandler(
        HttpServer &httpServer,
        SettingRepository &settingRepository,
        UpdateTriggerUseCase &updateTriggerUseCase,
        UpdateRestrictionUseCase &updateRestrictionUseCase,
        LanguageRepository &languageRepository,
        UpdateLanguageUseCase &updateLanguageUseCase,
        Translator &translator)
        : httpServer(httpServer),
          settingRepository(settingRepository),
          updateTriggerUseCase(updateTriggerUseCase),
          updateRestrictionUseCase(updateRestrictionUseCase),
          languageRepository(languageRepository),
          updateLanguageUseCase(updateLanguageUseCase),
          translator(translator)
    {
    }

    void SettingHandler::handleGetTrigger()
    {
        TriggerSetting setting = settingRepository.getTriggerSetting();

        StaticJsonDocument<256> document;
        document["soilMoistureOperator"] = setting.soilMoistureOperator;
        document["soilMoistureValue"] = setting.soilMoistureValue;

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

    void SettingHandler::handlePutTrigger()
    {
        WebServer &server = httpServer.raw();

        if (!server.hasArg("plain"))
        {
            httpServer.sendJsonError(400, "Body JSON tidak ditemukan");
            return;
        }

        StaticJsonDocument<256> document;
        DeserializationError error = deserializeJson(document, server.arg("plain"));

        if (error)
        {
            httpServer.sendJsonError(400, "JSON tidak valid");
            return;
        }

        TriggerSetting setting;
        setting.soilMoistureOperator =
            document["soilMoistureOperator"] | setting.soilMoistureOperator;
        setting.soilMoistureValue =
            document["soilMoistureValue"] | setting.soilMoistureValue;

        updateTriggerUseCase.execute(setting);

        handleGetTrigger();
    }

    void SettingHandler::handleGetRestriction()
    {
        RestrictionSetting setting = settingRepository.getRestrictionSetting();

        StaticJsonDocument<448> document;
        document["airHumidityEnabled"] = setting.airHumidityEnabled;
        document["airHumidityOperator"] = setting.airHumidityOperator;
        document["airHumidityValue"] = setting.airHumidityValue;
        document["airTemperatureEnabled"] = setting.airTemperatureEnabled;
        document["airTemperatureOperator"] = setting.airTemperatureOperator;
        document["airTemperatureValue"] = setting.airTemperatureValue;
        document["lightIntensityEnabled"] = setting.lightIntensityEnabled;
        document["lightIntensityOperator"] = setting.lightIntensityOperator;
        document["lightIntensityValue"] = setting.lightIntensityValue;
        document["maxPumpRuntimeSecond"] = setting.maxPumpRuntimeSecond;

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

    void SettingHandler::handlePutRestriction()
    {
        WebServer &server = httpServer.raw();

        if (!server.hasArg("plain"))
        {
            httpServer.sendJsonError(400, "Body JSON tidak ditemukan");
            return;
        }

        StaticJsonDocument<448> document;
        DeserializationError error = deserializeJson(document, server.arg("plain"));

        if (error)
        {
            httpServer.sendJsonError(400, "JSON tidak valid");
            return;
        }

        RestrictionSetting setting;
        setting.airHumidityEnabled =
            document["airHumidityEnabled"] | setting.airHumidityEnabled;
        setting.airHumidityOperator =
            document["airHumidityOperator"] | setting.airHumidityOperator;
        setting.airHumidityValue =
            document["airHumidityValue"] | setting.airHumidityValue;
        setting.airTemperatureEnabled =
            document["airTemperatureEnabled"] | setting.airTemperatureEnabled;
        setting.airTemperatureOperator =
            document["airTemperatureOperator"] | setting.airTemperatureOperator;
        setting.airTemperatureValue =
            document["airTemperatureValue"] | setting.airTemperatureValue;
        setting.lightIntensityEnabled =
            document["lightIntensityEnabled"] | setting.lightIntensityEnabled;
        setting.lightIntensityOperator =
            document["lightIntensityOperator"] | setting.lightIntensityOperator;
        setting.lightIntensityValue =
            document["lightIntensityValue"] | setting.lightIntensityValue;
        setting.maxPumpRuntimeSecond =
            document["maxPumpRuntimeSecond"] | setting.maxPumpRuntimeSecond;

        updateRestrictionUseCase.execute(setting);

        handleGetRestriction();
    }

    void SettingHandler::handleGetLanguage()
    {
        Language language = languageRepository.load();

        StaticJsonDocument<64> document;
        document["language"] = language == Language::English ? "en" : "id";

        String body;
        serializeJson(document, body);

        httpServer.sendJson(200, body);
    }

    void SettingHandler::handlePutLanguage()
    {
        WebServer &server = httpServer.raw();

        if (!server.hasArg("plain"))
        {
            httpServer.sendJsonError(400, "Body JSON tidak ditemukan");
            return;
        }

        StaticJsonDocument<64> document;
        DeserializationError error = deserializeJson(document, server.arg("plain"));

        if (error)
        {
            httpServer.sendJsonError(400, "JSON tidak valid");
            return;
        }

        String code = document["language"] | "id";
        Language language = code == "en" ? Language::English : Language::Indonesian;

        updateLanguageUseCase.execute(language);
        translator.setLanguage(language);

        handleGetLanguage();
    }

}