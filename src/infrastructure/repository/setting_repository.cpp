#include "infrastructure/repository/setting_repository.h"

#include <ArduinoJson.h>

namespace irrigation
{

    namespace
    {
        void fillTriggerDocument(const TriggerSetting &setting, JsonDocument &document)
        {
            document["soilMoistureOperator"] = setting.soilMoistureOperator;
            document["soilMoistureValue"] = setting.soilMoistureValue;
        }

        void fillRestrictionDocument(const RestrictionSetting &setting, JsonDocument &document)
        {
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
        }
    }

    SdSettingRepository::SdSettingRepository(
        PersistenceStorageManager &storage,
        JsonSerializer &serializer,
        const String &triggerPath,
        const String &restrictionPath)
        : storage(storage),
          serializer(serializer),
          triggerPath(triggerPath),
          restrictionPath(restrictionPath)
    {
    }

    TriggerSetting SdSettingRepository::getTriggerSetting()
    {
        TriggerSetting setting;

        File file = storage.openRead(triggerPath);
        if (!file)
        {
            return setting;
        }

        StaticJsonDocument<256> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        if (error)
        {
            return setting;
        }

        setting.soilMoistureOperator =
            document["soilMoistureOperator"] | setting.soilMoistureOperator;

        setting.soilMoistureValue =
            document["soilMoistureValue"] | setting.soilMoistureValue;

        return setting;
    }

    RestrictionSetting SdSettingRepository::getRestrictionSetting()
    {
        RestrictionSetting setting;

        File file = storage.openRead(restrictionPath);
        if (!file)
        {
            return setting;
        }

        StaticJsonDocument<448> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        if (error)
        {
            return setting;
        }

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

        return setting;
    }

    void SdSettingRepository::saveTriggerSetting(const TriggerSetting &setting)
    {
        StaticJsonDocument<256> document;
        fillTriggerDocument(setting, document);

        storage.remove(triggerPath);
        File file = storage.openWrite(triggerPath);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

    void SdSettingRepository::saveRestrictionSetting(const RestrictionSetting &setting)
    {
        StaticJsonDocument<448> document;
        fillRestrictionDocument(setting, document);

        storage.remove(restrictionPath);
        File file = storage.openWrite(restrictionPath);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

    void SdSettingRepository::ensureCompleteFiles()
    {
        StaticJsonDocument<256> triggerComplete;
        fillTriggerDocument(TriggerSetting(), triggerComplete);

        if (needsCompleting(triggerPath, triggerComplete))
        {
            saveTriggerSetting(getTriggerSetting());
        }

        StaticJsonDocument<448> restrictionComplete;
        fillRestrictionDocument(RestrictionSetting(), restrictionComplete);

        if (needsCompleting(restrictionPath, restrictionComplete))
        {
            saveRestrictionSetting(getRestrictionSetting());
        }
    }

    bool SdSettingRepository::needsCompleting(const String &path, const JsonDocument &complete)
    {
        if (!storage.exists(path))
        {
            return true;
        }

        File file = storage.openRead(path);
        if (!file)
        {
            return false;
        }

        StaticJsonDocument<448> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        // jangan ditimpa default, bisa jadi cuma error baca sesaat
        if (error)
        {
            return false;
        }

        for (JsonPairConst pair : complete.as<JsonObjectConst>())
        {
            if (!document.containsKey(pair.key().c_str()))
            {
                return true;
            }
        }

        return false;
    }

}