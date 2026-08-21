#include "infrastructure/time/hybrid_time_provider.h"

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    HybridTimeProvider::HybridTimeProvider(
        RtcTimeProvider &rtc,
        NetworkTimeProvider &network)
        : rtc(rtc),
          network(network)
    {
    }

    bool HybridTimeProvider::begin()
    {
        bool rtcReady = rtc.begin();

        // configTime() (di dalamnya) murah & tidak butuh koneksi aktif -
        // aman selalu dipanggil sebagai persiapan fallback, dipakai
        // beneran (lewat synchronize()) cuma kalau RTC gagal.
        network.begin();

        // true = RTC siap (kasus umum, tidak butuh WiFi/internet sama
        // sekali). false = caller perlu coba synchronize() eksplisit
        // buat fallback NTP.
        return rtcReady;
    }

    bool HybridTimeProvider::synchronize()
    {
        if (rtc.isSynchronized())
        {
            return true;
        }

        return network.synchronize();
    }

    bool HybridTimeProvider::isSynchronized() const
    {
        return rtc.isSynchronized() || network.isSynchronized();
    }

    Timestamp HybridTimeProvider::now() const
    {
        if (rtc.isSynchronized())
        {
            return rtc.now();
        }

        return network.now();
    }

}
