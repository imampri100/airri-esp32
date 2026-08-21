#ifndef IRRIGATION_UPDATE_TRIGGER_USECASE_H
#define IRRIGATION_UPDATE_TRIGGER_USECASE_H

#include "domain/entity/trigger_setting.h"
#include "domain/repository/setting_repository.h"

namespace irrigation
{
    class UpdateTriggerUseCase
    {
    public:
        explicit UpdateTriggerUseCase(
            SettingRepository &settingRepository);

        void execute(const TriggerSetting &setting);

    private:
        SettingRepository &settingRepository;
    };
}

#endif
