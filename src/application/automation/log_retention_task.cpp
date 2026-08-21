#include "application/automation/log_retention_task.h"

#include "infrastructure/persistence/log_period.h"
#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    LogRetentionTask::LogRetentionTask(
        PersistenceStorageManager &storage,
        TimeRepository &timeRepository,
        const String &sensorLogDirectory,
        const String &irrigationLogDirectory)
        : storage(storage),
          timeRepository(timeRepository),
          sensorLogDirectory(sensorLogDirectory),
          irrigationLogDirectory(irrigationLogDirectory)
    {
    }

    uint32_t LogRetentionTask::interval() const
    {
        // Dicek sekali sehari - kerja beratnya (hapus file) cuma jalan
        // kalau memang ada file yang lewat retensi/kuota, jadi aman
        // dicek sesering ini. "3 bulan" sendiri sengaja TIDAK dijadikan
        // interval task ini - dalam milidetik itu overflow uint32_t
        // (maks ~49 hari) - jadi cutoff kalendernya dihitung dari
        // wall-clock (Timestamp) di dalam execute(), bukan dari interval
        // scheduler ini.
        return 24UL * 60 * 60 * 1000;
    }

    void LogRetentionTask::execute()
    {
        if (!timeRepository.isSynchronized())
        {
            // Tanpa waktu yang valid, jangan ambil resiko hapus file
            // berdasar cutoff kalender yang salah.
            return;
        }

        Timestamp now = timeRepository.now();

        String cutoff = LogPeriod::cutoffKey(now.value(), MONTHS_TO_KEEP);

        purgeOlderThanCutoff(sensorLogDirectory, cutoff);
        purgeOlderThanCutoff(irrigationLogDirectory, cutoff);

        while (totalLogBytes() > MAX_TOTAL_LOG_BYTES)
        {
            if (!purgeOldestFile())
            {
                break;
            }
        }
    }

    void LogRetentionTask::purgeOlderThanCutoff(
        const String &directory,
        const String &cutoffPeriodKey)
    {
        for (const String &fileName : storage.listFiles(directory))
        {
            String periodKey = fileName;

            int dotIndex = periodKey.indexOf('.');
            if (dotIndex >= 0)
            {
                periodKey = periodKey.substring(0, dotIndex);
            }

            if (periodKey < cutoffPeriodKey)
            {
                String path = directory + "/" + fileName;

                SerialLogger::info(
                    "LogRetentionTask: hapus log lewat retensi " +
                    String(MONTHS_TO_KEEP) + " bulan - " + path);

                storage.remove(path);
            }
        }
    }

    uint64_t LogRetentionTask::totalLogBytes()
    {
        uint64_t total = 0;

        for (const String &directory : {sensorLogDirectory, irrigationLogDirectory})
        {
            for (const String &fileName : storage.listFiles(directory))
            {
                total += storage.fileSize(directory + "/" + fileName);
            }
        }

        return total;
    }

    bool LogRetentionTask::purgeOldestFile()
    {
        String oldestDirectory;
        String oldestFileName;

        for (const String &directory : {sensorLogDirectory, irrigationLogDirectory})
        {
            std::vector<String> files = storage.listFiles(directory);

            if (files.empty())
            {
                continue;
            }

            // listFiles() sudah terurut ascending, jadi elemen pertama
            // adalah periode tertua di directory ini.
            const String &candidate = files.front();

            if (oldestFileName.length() == 0 || candidate < oldestFileName)
            {
                oldestDirectory = directory;
                oldestFileName = candidate;
            }
        }

        if (oldestFileName.length() == 0)
        {
            return false;
        }

        String path = oldestDirectory + "/" + oldestFileName;

        SerialLogger::warn(
            "LogRetentionTask: total log melebihi kuota, hapus file "
            "tertua - " +
            path);

        storage.remove(path);

        return true;
    }

}
