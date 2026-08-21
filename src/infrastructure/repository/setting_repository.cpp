#include "infrastructure/repository/setting_repository.h"

#include <ArduinoJson.h>

namespace irrigation
{

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

        document["soilMoistureOperator"] = setting.soilMoistureOperator;
        document["soilMoistureValue"] = setting.soilMoistureValue;

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

        storage.remove(restrictionPath);
        File file = storage.openWrite(restrictionPath);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

}