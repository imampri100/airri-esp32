#ifndef IRRIGATION_LOG_PERIOD_H
#define IRRIGATION_LOG_PERIOD_H

#include <Arduino.h>

namespace irrigation
{

    // Kunci periode rotasi log, format "YYYY-MM" (UTC), dipakai sebagai
    // nama file NDJSON per bulan (mis. "2026-07.ndjson"). Formatnya
    // zero-padded fixed-width supaya urut leksikografis = urut kronologis,
    // jadi bisa langsung dipakai untuk sorting nama file maupun
    // perbandingan cutoff retensi tanpa parsing ulang.
    class LogPeriod
    {
    public:
        static String keyFor(uint64_t unixSeconds);

        static String cutoffKey(uint64_t nowUnixSeconds, uint32_t monthsBack);
    };

}

#endif
