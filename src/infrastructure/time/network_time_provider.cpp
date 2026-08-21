#include "infrastructure/time/network_time_provider.h"

#include <Arduino.h>
#include <time.h>

namespace irrigation
{

    namespace
    {
        // WIB (UTC+7). Sesuaikan kalau device dipakai di zona waktu lain.
        constexpr long GMT_OFFSET_SEC = 7 * 3600;
        constexpr int DAYLIGHT_OFFSET_SEC = 0;
        constexpr const char *NTP_SERVER_1 = "pool.ntp.org";
        constexpr const char *NTP_SERVER_2 = "time.google.com";
        constexpr unsigned long SYNC_TIMEOUT_MS = 10000;
    }

    bool NetworkTimeProvider::begin()
    {
        configTime(
            GMT_OFFSET_SEC,
            DAYLIGHT_OFFSET_SEC,
            NTP_SERVER_1,
            NTP_SERVER_2);

        return true;
    }

    bool NetworkTimeProvider::synchronize()
    {
        struct tm timeInfo;

        unsigned long start = millis();

        while (!getLocalTime(&timeInfo, 1000))
        {
            if (millis() - start >= SYNC_TIMEOUT_MS)
            {
                synchronized = false;
                return false;
            }
        }

        synchronized = true;

        return true;
    }

    bool NetworkTimeProvider::isSynchronized() const
    {
        return synchronized;
    }

    Timestamp NetworkTimeProvider::now() const
    {
        if (!synchronized)
        {
            return Timestamp();
        }

        return Timestamp(
            static_cast<uint64_t>(
                time(nullptr)));
    }

}
