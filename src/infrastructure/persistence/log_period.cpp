#include "infrastructure/persistence/log_period.h"

#include <time.h>

namespace irrigation
{

    namespace
    {
        String formatYearMonth(int32_t year, int32_t month)
        {
            char buffer[8];
            snprintf(buffer, sizeof(buffer), "%04d-%02d", year, month);

            return String(buffer);
        }
    }

    String LogPeriod::keyFor(uint64_t unixSeconds)
    {
        time_t rawTime = static_cast<time_t>(unixSeconds);

        struct tm parts;
        gmtime_r(&rawTime, &parts);

        return formatYearMonth(parts.tm_year + 1900, parts.tm_mon + 1);
    }

    String LogPeriod::cutoffKey(uint64_t nowUnixSeconds, uint32_t monthsBack)
    {
        time_t rawTime = static_cast<time_t>(nowUnixSeconds);

        struct tm parts;
        gmtime_r(&rawTime, &parts);

        int32_t totalMonths =
            (parts.tm_year * 12 + parts.tm_mon) - static_cast<int32_t>(monthsBack);

        int32_t year = totalMonths / 12;
        int32_t month = totalMonths % 12;

        if (month < 0)
        {
            month += 12;
            year -= 1;
        }

        return formatYearMonth(year + 1900, month + 1);
    }

}
