#include "application/usecase/update_trigger_usecase.h"

namespace irrigation
{

    UpdateTriggerUseCase::UpdateTriggerUseCase(
        SettingRepository &settingRepository)
        : settingRepository(settingRepository)
    {
    }

    void UpdateTriggerUseCase::execute(const TriggerSetting &setting)
    {
        settingRepository.saveTriggerSetting(setting);
    }

}
