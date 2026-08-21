#include "application/automation/sync_time_task.h"

namespace irrigation
{

    namespace
    {
        // Sinkronisasi ulang tiap 6 jam kalau sudah pernah berhasil.
        constexpr uint32_t SYNC_INTERVAL_MS = 6UL * 60UL * 60UL * 1000UL;

        // Belum pernah sinkron (mis. STA masih proses connect pas boot) -
        // coba lagi tiap 30 detik biar tidak nyangkut createdAt=0 sampai
        // 6 jam gara-gara STA belum siap pas percobaan pertama.
        constexpr uint32_t RETRY_INTERVAL_MS = 30UL * 1000UL;
    }

    SyncTimeTask::SyncTimeTask(TimeProvider &timeProvider)
        : timeProvider(timeProvider)
    {
    }

    uint32_t SyncTimeTask::interval() const
    {
        return timeProvider.isSynchronized() ? SYNC_INTERVAL_MS : RETRY_INTERVAL_MS;
    }

    void SyncTimeTask::execute()
    {
        bool ok = timeProvider.synchronize();

        if (!ok)
        {
            SerialLogger::warn(
                "Fallback NTP gagal (RTC belum siap & WiFi Station belum "
                "connect ke internet) - coba lagi 30 detik lagi");
        }
    }

}
