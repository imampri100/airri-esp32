#ifndef IRRIGATION_GET_STATUS_USECASE_H
#define IRRIGATION_GET_STATUS_USECASE_H

#include "application/dto/status_dto.h"

#include "domain/repository/pump_repository.h"
#include "domain/repository/sensor_repository.h"
#include "domain/repository/setting_repository.h"
#include "domain/repository/time_repository.h"
#include "domain/service/decision_engine.h"

namespace irrigation
{
    class GetStatusUseCase
    {
    public:
        GetStatusUseCase(
            SensorRepository &sensorRepository,
            SettingRepository &settingRepository,
            PumpRepository &pumpRepository,
            TimeRepository &timeRepository,
            DecisionEngine &decisionEngine);

        StatusDto execute();

    private:
        SensorRepository &sensorRepository;

        SettingRepository &settingRepository;

        PumpRepository &pumpRepository;

        TimeRepository &timeRepository;

        DecisionEngine &decisionEngine;
    };
}

#endif
