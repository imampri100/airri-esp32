#ifndef IRRIGATION_SYNC_TIME_TASK_H
#define IRRIGATION_SYNC_TIME_TASK_H

#include "application/automation/automation_task.h"

#include "domain/provider/time_provider.h"

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    // Coba sinkronisasi waktu secara berkala. Kalau RTC (DS3231) sudah
    // siap, ini praktis no-op (RTC bukan sumber yang butuh "resync" -
    // lihat HybridTimeProvider). Baru benar-benar coba NTP kalau RTC
    // belum/tidak siap saat boot, sebagai fallback berkala (butuh WiFi
    // Station terkonfigurasi & internet).
    class SyncTimeTask : public AutomationTask
    {
    public:
        explicit SyncTimeTask(TimeProvider &timeProvider);

        uint32_t interval() const override;

        void execute() override;

    private:
        TimeProvider &timeProvider;
    };

}

#endif
