#ifndef IRRIGATION_INFRA_SETTING_REPOSITORY_H
#define IRRIGATION_INFRA_SETTING_REPOSITORY_H

#include "domain/repository/setting_repository.h"

#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Implementasi SettingRepository (domain) -> file JSON tunggal di SD
    // Card untuk trigger.json dan restriction.json.
    class SdSettingRepository : public SettingRepository
    {
    public:
        SdSettingRepository(
            PersistenceStorageManager &storage,
            JsonSerializer &serializer,
            const String &triggerPath,
            const String &restrictionPath);

        TriggerSetting getTriggerSetting() override;

        RestrictionSetting getRestrictionSetting() override;

        void saveTriggerSetting(const TriggerSetting &setting) override;

        void saveRestrictionSetting(const RestrictionSetting &setting) override;

    private:
        PersistenceStorageManager &storage;

        JsonSerializer &serializer;

        String triggerPath;

        String restrictionPath;
    };

}

#endif
