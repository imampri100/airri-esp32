#include "infrastructure/time/rtc_time_provider.h"

#include <Wire.h>

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    bool RtcTimeProvider::begin()
    {
        // Idempotent - aman dipanggil lagi walau SensorManager sudah
        // Wire.begin() duluan dengan pin yang sama.
        Wire.begin();

        ready = rtc.begin(&Wire);

        if (!ready)
        {
            SerialLogger::error(
                "RTC DS3231 tidak terdeteksi di bus I2C - timestamp log "
                "akan bernilai 0.");
            return false;
        }

        if (rtc.lostPower())
        {
            // Chip baru / baterai coin cell habis - waktu di dalam RTC
            // tidak valid. Fallback ke jam mesin saat firmware
            // dikompilasi supaya log tidak macet di epoch/garbage time -
            // TIDAK akurat, cuma estimasi kasar. Koreksi manual (adjust
            // ke waktu WIB yang benar) tetap diperlukan.
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

            SerialLogger::warn(
                "RTC DS3231 kehilangan daya (baterai coin cell habis/chip "
                "baru) - waktu di-set sementara dari jam saat kompilasi "
                "firmware. Koreksi manual ke waktu WIB yang benar kalau "
                "belum akurat.");
        }

        return true;
    }

    bool RtcTimeProvider::synchronize()
    {
        // RTC tidak ada konsep "sync ke sumber eksternal" seperti NTP -
        // dia sumber waktu itu sendiri, dibaca live tiap now(). Method
        // ini cuma ada karena wajib dipenuhi TimeProvider.
        return ready;
    }

    bool RtcTimeProvider::isSynchronized() const
    {
        return ready;
    }

    Timestamp RtcTimeProvider::now() const
    {
        if (!ready)
        {
            return Timestamp();
        }

        return Timestamp(
            static_cast<uint64_t>(rtc.now().unixtime()));
    }

}
