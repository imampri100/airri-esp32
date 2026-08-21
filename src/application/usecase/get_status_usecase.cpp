#include "application/usecase/get_status_usecase.h"

namespace irrigation
{

    GetStatusUseCase::GetStatusUseCase(
        SensorRepository &sensorRepository,
        SettingRepository &settingRepository,
        PumpRepository &pumpRepository,
        TimeRepository &timeRepository,
        DecisionEngine &decisionEngine)
        : sensorRepository(sensorRepository),
          settingRepository(settingRepository),
          pumpRepository(pumpRepository),
          timeRepository(timeRepository),
          decisionEngine(decisionEngine)
    {
    }

    StatusDto GetStatusUseCase::execute()
    {
        StatusDto dto;

        dto.sensorReading = sensorRepository.getCurrentReading();
        dto.triggerSetting = settingRepository.getTriggerSetting();
        dto.restrictionSetting = settingRepository.getRestrictionSetting();

        dto.decisionResult = decisionEngine.evaluate(
            dto.sensorReading,
            dto.triggerSetting,
            dto.restrictionSetting);

        dto.pumpRunning = pumpRepository.isRunning();

        dto.timeSynchronized = timeRepository.isSynchronized();

        return dto;
    }

}
