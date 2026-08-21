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
        uint32_t nowMs = millis();

        bool wasRunning = pumpRepository.isRunning();

        // Pompa yang dinyalakan/dimatikan dari luar task ini (tombol BOOT,
        // POST /api/pump/start|stop) baru terdeteksi di siklus ini, jadi
        // waktu mulai/berhenti yang tercatat bisa telat maks. 1 siklus.
        if (wasRunning && !pumpTracked)
        {
            if (decision.shouldRunPump)
            {
                beginPumpTracking(now, nowMs);
            }
            else
            {
                // Tidak didukung pemicu = uji coba, dimatikan tanpa dicatat
                // (sama seperti POST /api/pump/test).
                pumpRepository.stop();
                wasRunning = false;

                SerialLogger::info(
                    "Pompa OFF (nyala manual, tidak dicatat) - " +
                    String(reasonCodeLabel(decision.reason)));
            }
        }
        else if (!wasRunning && pumpTracked)
        {
            recordIrrigationLog(now);
        }

        if (decision.shouldRunPump && !wasRunning)
        {
            pumpRepository.start();
            beginPumpTracking(now, nowMs);

            // Label kode non-lokalisasi (bukan Translator) - log Serial
            // sengaja tidak ikut i18n, lihat reason_code.h.
            SerialLogger::info(
                "Pompa ON - " + String(reasonCodeLabel(decision.reason)));
        }
        else if (!decision.shouldRunPump && wasRunning)
        {
            pumpRepository.stop();

            SerialLogger::info(
                "Pompa OFF - " + String(reasonCodeLabel(decision.reason)));

            recordIrrigationLog(now);
        }
        else if (wasRunning)
        {
            // Safety net: paksa berhenti kalau pompa menyala terus melebihi
            // restriction.maxPumpRuntimeSecond (mis. soil sensor rusak/putus).
            // Pakai millis() supaya tetap jalan walau waktu RTC/NTP belum
            // sinkron.
            uint32_t elapsedSecond = (nowMs - pumpStartedAtMs) / 1000UL;

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

    void MonitorEnvironmentTask::beginPumpTracking(const Timestamp &now, uint32_t nowMs)
    {
        pumpTracked = true;
        pumpStartedAt = now;
        pumpStartedAtMs = nowMs;
    }

    void MonitorEnvironmentTask::recordIrrigationLog(const Timestamp &stopAt)
    {
        pumpTracked = false;

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