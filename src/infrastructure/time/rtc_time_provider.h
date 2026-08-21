#ifndef IRRIGATION_RTC_TIME_PROVIDER_H
#define IRRIGATION_RTC_TIME_PROVIDER_H

#include <RTClib.h>

#include "domain/provider/time_provider.h"
#include "domain/repository/time_repository.h"

namespace irrigation
{

    // Sumber waktu dari modul RTC DS3231 (I2C, bus sama dengan AHT10 +
    // BH1750 - lihat PinConfig::I2C_SDA/I2C_SCL). Dipakai karena device
    // tidak punya uplink internet (WiFi cuma Access Point sendiri, lihat
    // WifiManager) sehingga NTP tidak mungkin dipakai. Beda dari
    // NetworkTimeProvider: tidak ada konsep "sync ke server luar" - RTC
    // dibaca live tiap now(), dan sudah harus di-set ke waktu lokal WIB
    // sekali (lewat fallback lostPower() di begin(), atau adjust manual).
    class RtcTimeProvider : public TimeProvider, public TimeRepository
    {
    public:
        RtcTimeProvider() = default;

        bool begin() override;

        bool synchronize() override;

        bool isSynchronized() const override;

        Timestamp now() const override;

        // TimeRepository (domain) - delegasi ke implementasi TimeProvider
        // di atas, sama seperti pola di NetworkTimeProvider.
        Timestamp now() override
        {
            return static_cast<const TimeProvider &>(*this).now();
        }

        bool isSynchronized() override
        {
            return static_cast<const TimeProvider &>(*this).isSynchronized();
        }

    private:
        // mutable - RTC_DS3231::now() dari RTClib bukan method const,
        // padahal now() di sini wajib const (override TimeProvider).
        // Membaca RTC tidak mengubah state logis provider ini, jadi
        // mutable adalah cara aman untuk itu.
        mutable RTC_DS3231 rtc;

        bool ready = false;
    };

}

#endif
