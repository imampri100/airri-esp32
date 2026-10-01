#ifndef IRRIGATION_TIME_CONFIG_H
#define IRRIGATION_TIME_CONFIG_H

#include <Arduino.h>

namespace irrigation
{

    namespace TimeConfig
    {
        // WIB (UTC+7). waktu di RTC, log & API tetap epoch UTC, offset ini
        // cuma buat jam di TFT & konversi jam kompilasi laptop.
        // ganti kalau dipakai di zona waktu lain
        constexpr int32_t UTC_OFFSET_SECOND = 7 * 3600;
    }

}

#endif
