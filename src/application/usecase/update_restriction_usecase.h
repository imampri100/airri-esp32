#ifndef IRRIGATION_UPDATE_RESTRICTION_USECASE_H
#define IRRIGATION_UPDATE_RESTRICTION_USECASE_H

#include "domain/entity/restriction_setting.h"
#include "domain/repository/setting_repository.h"

namespace irrigation
{
    class UpdateRestrictionUseCase
    {
    public:
        explicit UpdateRestrictionUseCase(
            SettingRepository &settingRepository);

        void execute(const RestrictionSetting &setting);

    private:
        SettingRepository &settingRepository;
    };
}

#endif
