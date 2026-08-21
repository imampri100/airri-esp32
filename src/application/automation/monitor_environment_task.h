#ifndef IRRIGATION_MONITOR_ENVIRONMENT_TASK_H
#define IRRIGATION_MONITOR_ENVIRONMENT_TASK_H

#include "application/automation/automation_task.h"

#include "domain/entity/irrigation_log.h"
#include "domain/entity/sensor_log.h"

#include "domain/repository/sensor_repository.h"
#include "domain/repository/setting_repository.h"
#include "domain/repository/pump_repository.h"
#include "domain/repository/time_repository.h"
#include "domain/repository/sensor_log_repository.h"
#include "domain/repository/irrigation_log_repository.h"

#include "domain/service/decision_engine.h"

namespace irrigation
{

    class MonitorEnvironmentTask
        : public AutomationTask
    {
    public:
        MonitorEnvironmentTask(
            SensorRepository &sensorRepository,
            SettingRepository &settingRepository,
            PumpRepository &pumpRepository,
            TimeRepository &timeRepository,
            SensorLogRepository &sensorLogRepository,
            IrrigationLogRepository &irrigationLogRepository,
            DecisionEngine &decisionEngine);

        uint32_t interval() const override;

        void execute() override;

    private:
        void recordIrrigationLog(const Timestamp &stopAt);

        Timestamp pumpStartedAt;

        SensorRepository &sensorRepository;

        SettingRepository &settingRepository;

        PumpRepository &pumpRepository;

        TimeRepository &timeRepository;

        SensorLogRepository &sensorLogRepository;

        IrrigationLogRepository &irrigationLogRepository;

        DecisionEngine &decisionEngine;
    };

}

#endif