#ifndef IRRIGATION_LOG_RETENTION_TASK_H
#define IRRIGATION_LOG_RETENTION_TASK_H

#include "application/automation/automation_task.h"

#include "domain/repository/time_repository.h"

#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Housekeeping berkala: hapus file log NDJSON per-bulan (lihat
    // LogPeriod/SdSensorLogRepository) yang sudah lebih tua dari retensi
    // kalender, dan kalau total ukuran log masih melebihi kuota walau
    // masih di dalam retensi, hapus file tertua lagi sampai di bawah
    // kuota. Murni berbasis waktu/ukuran - device TIDAK peduli status
    // sync mobile app (mobile app yang tanggung jawab narik data sebelum
    // lewat retensi; lihat sensorLogFirstId/irrigationLogFirstId di
    // /api/sync untuk deteksi gap di sisi mobile).
    class LogRetentionTask : public AutomationTask
    {
    public:
        LogRetentionTask(
            PersistenceStorageManager &storage,
            TimeRepository &timeRepository,
            const String &sensorLogDirectory,
            const String &irrigationLogDirectory);

        uint32_t interval() const override;

        void execute() override;

    private:
        PersistenceStorageManager &storage;

        TimeRepository &timeRepository;

        String sensorLogDirectory;

        String irrigationLogDirectory;

        static constexpr uint32_t MONTHS_TO_KEEP = 3;

        // 16GB dikurangi margin aman ~200MB, supaya tidak pernah mepet
        // pas ke limit kapasitas kartu (resiko hang/gagal tulis).
        static constexpr uint64_t MAX_TOTAL_LOG_BYTES =
            16ULL * 1024 * 1024 * 1024 - 200ULL * 1024 * 1024;

        void purgeOlderThanCutoff(
            const String &directory,
            const String &cutoffPeriodKey);

        uint64_t totalLogBytes();

        bool purgeOldestFile();
    };

}

#endif
