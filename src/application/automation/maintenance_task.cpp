#include "application/automation/maintenance_task.h"

#include <Arduino.h>

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    namespace
    {
        // Cek tiap 5 menit - cukup jarang, tidak perlu bebani loop.
        constexpr uint32_t MAINTENANCE_INTERVAL_MS = 5UL * 60UL * 1000UL;
    }

    uint32_t MaintenanceTask::interval() const
    {
        return MAINTENANCE_INTERVAL_MS;
    }

    void MaintenanceTask::execute()
    {
        uint32_t freeHeap = ESP.getFreeHeap();

        SerialLogger::info("Maintenance check - free heap: " + String(freeHeap) + " bytes");

        if (freeHeap < LOW_HEAP_WARNING_BYTES)
        {
            SerialLogger::warn(
                "Free heap rendah (" + String(freeHeap) +
                " bytes) - pertimbangkan restart device");
        }
    }

}
