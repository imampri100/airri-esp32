#include "application/automation/monitor_environment_task.h"

#include "infrastructure/config/pump_config.h"
#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    MonitorEnvironmentTask::MonitorEnvironmentTask(
        SensorRepository &sensorRepository,
        SettingRepository &settingRepository,
        PumpRepository &pumpRepository,
        TimeRepository &timeRepository,
        SensorLogRepository &sensorLogRepository,
        IrrigationLogRepository &irrigationLogRepository,
        DecisionEngine &decisionEngine)
        : sensorRepository(sensorRepository),
          settingRepository(settingRepository),
          pumpRepository(pumpRepository),
          timeRepository(timeRepository),
          sensorLogRepository(sensorLogRepository),
          irrigationLogRepository(irrigationLogRepository),
          decisionEngine(decisionEngine)
    {
    }

    uint32_t MonitorEnvironmentTask::interval() const
    {
        // Baca sensor & evaluasi keputusan tiap 3 detik.
        return 3000;
    }

    void MonitorEnvironmentTask::execute()
    {
        auto reading =
            sensorRepository.getCurrentReading();

        auto trigger =
            settingRepository.getTriggerSetting();

        auto restriction =
            settingRepository.getRestrictionSetting();

        auto decision =
            decisionEngine.evaluate(
                reading,
                trigger,
                restriction);

        Timestamp now = timeRepository.now();

        bool wasRunning = pumpRepository.isRunning();

        if (decision.shouldRunPump && !wasRunning)
        {
            pumpRepository.start();
            pumpStartedAt = now;

            // Label debug non-lokalisasi (bukan Translator) - log Serial
            // sengaja tidak ikut i18n, lihat reason_code.h.
            SerialLogger::info(
                "Pompa ON - " + String(reasonCodeDebugLabel(decision.reason)));
        }
        else if (!decision.shouldRunPump && wasRunning)
        {
            pumpRepository.stop();

            SerialLogger::info(
                "Pompa OFF - " + String(reasonCodeDebugLabel(decision.reason)));

            recordIrrigationLog(now);
        }
        else if (wasRunning)
        {
            // Safety net: paksa berhenti kalau sudah kelamaan menyala
            // terus-menerus (mis. soil sensor rusak/putus). Batasnya
            // sekarang diambil dari setting (bisa diatur lewat
            // PUT /api/settings/restriction, field maxPumpRuntimeSecond),
            // bukan hardcoded lagi.
            uint32_t elapsedSecond =
                pumpStartedAt.isValid() && now.isValid()
                    ? static_cast<uint32_t>(now.value() - pumpStartedAt.value())
                    : 0;

            if (elapsedSecond >= restriction.maxPumpRuntimeSecond)
            {
                pumpRepository.stop();

                SerialLogger::warn(
                    "Pompa dimatikan paksa - melebihi batas runtime aman");

                recordIrrigationLog(now);
            }
        }

        // Simpan log sensor setiap siklus, tandai apakah bertepatan
        // dengan siklus penyiraman.
        SensorLog log;
        log.createdAt = now;
        log.soilMoisture = reading.soilMoisture;
        log.airHumidity = reading.airHumidity;
        log.airTemperature = reading.airTemperature;
        log.lightIntensity = reading.lightIntensity;
        log.isIrrigationRun = pumpRepository.isRunning();

        sensorLogRepository.add(log);
    }

    void MonitorEnvironmentTask::recordIrrigationLog(const Timestamp &stopAt)
    {
        if (!pumpStartedAt.isValid())
        {
            return;
        }

        IrrigationLog log;
        log.createdAt = stopAt;
        log.irrigationRunAt = pumpStartedAt;
        log.irrigationStopAt = stopAt;

        if (stopAt.isValid() && pumpStartedAt.isValid())
        {
            log.irrigationDurationSecond =
                static_cast<uint32_t>(stopAt.value() - pumpStartedAt.value());

            // Tidak ada flow sensor (lihat PumpConfig) - volume diestimasi
            // dari durasi nyala pompa x debit tetap.
            log.irrigationMillilitre =
                static_cast<float>(log.irrigationDurationSecond) *
                (PumpConfig::FLOW_RATE_ML_PER_MINUTE / 60.0f);
        }

        irrigationLogRepository.add(log);

        pumpStartedAt = Timestamp();
    }

}