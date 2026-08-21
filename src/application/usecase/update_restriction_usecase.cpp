#include "application/usecase/update_restriction_usecase.h"

namespace irrigation
{

    UpdateRestrictionUseCase::UpdateRestrictionUseCase(
        SettingRepository &settingRepository)
        : settingRepository(settingRepository)
    {
    }

    void UpdateRestrictionUseCase::execute(const RestrictionSetting &setting)
    {
        settingRepository.saveRestrictionSetting(setting);
    }

}
