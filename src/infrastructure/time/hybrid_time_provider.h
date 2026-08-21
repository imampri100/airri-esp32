#ifndef IRRIGATION_HYBRID_TIME_PROVIDER_H
#define IRRIGATION_HYBRID_TIME_PROVIDER_H

#include "domain/provider/time_provider.h"
#include "domain/repository/time_repository.h"

#include "infrastructure/time/network_time_provider.h"
#include "infrastructure/time/rtc_time_provider.h"

namespace irrigation
{

    // RTC (DS3231) sebagai sumber waktu utama - tidak butuh WiFi/internet
    // sama sekali. NTP dipakai HANYA sebagai fallback kalau RTC gagal
    // diinisialisasi (chip tidak kedeteksi/rusak) DAN WiFi Station
    // (opsional, lihat WifiManager) kebetulan berhasil connect ke
    // internet. Kalau RTC normal (kasus umum), NTP tidak pernah disentuh.
    class HybridTimeProvider : public TimeProvider, public TimeRepository
    {
    public:
        HybridTimeProvider(
            RtcTimeProvider &rtc,
            NetworkTimeProvider &network);

        bool begin() override;

        bool synchronize() override;

        bool isSynchronized() const override;

        Timestamp now() const override;

        // TimeRepository (domain) - delegasi ke implementasi TimeProvider
        // di atas, sama seperti pola di RtcTimeProvider/NetworkTimeProvider.
        Timestamp now() override
        {
            return static_cast<const TimeProvider &>(*this).now();
        }

        bool isSynchronized() override
        {
            return static_cast<const TimeProvider &>(*this).isSynchronized();
        }

    private:
        RtcTimeProvider &rtc;

        NetworkTimeProvider &network;
    };

}

#endif
