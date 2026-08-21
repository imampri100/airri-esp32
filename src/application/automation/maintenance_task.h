#ifndef IRRIGATION_MAINTENANCE_TASK_H
#define IRRIGATION_MAINTENANCE_TASK_H

#include "application/automation/automation_task.h"

namespace irrigation
{

    // Tugas housekeeping ringan yang jalan berkala: memantau sisa heap
    // memory ESP32 dan melaporkan peringatan kalau mulai kritis (indikasi
    // memory leak/fragmentasi pada firmware yang jalan lama tanpa reboot).
    class MaintenanceTask : public AutomationTask
    {
    public:
        MaintenanceTask() = default;

        uint32_t interval() const override;

        void execute() override;

    private:
        static constexpr uint32_t LOW_HEAP_WARNING_BYTES = 20000;
    };

}

#endif
