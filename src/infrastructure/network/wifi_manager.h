#ifndef IRRIGATION_WIFI_MANAGER_H
#define IRRIGATION_WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

#include "infrastructure/network/wifi_credential.h"

namespace irrigation
{

    // AP & STA punya toggle sendiri2, kombinasinya nentuin WiFi.mode().
    // kalau dua2nya mati device gabisa diakses -> begin() maksa AP nyala lagi
    class WifiManager
    {
    public:
        WifiManager() = default;

        void begin(
            const WifiCredential &credential);

        void tick(); // retry STA doang, no-op kalau STA gak dipake

        bool isStaConnected() const;

        bool hasStaConfigured() const;

        bool isApEnabled() const;

        String ipAddress() const;

        // valid kalau STA konek, kalau enggak ya "0.0.0.0"
        String staIpAddress() const;

        String macAddress() const;

        uint8_t stationCount() const;

    private:
        void connectSta();

        void logNearbyNetworks();

        WifiCredential credential;

        bool apEnabled = true;

        bool staConfigured = false;

        bool staConnecting = false;

        uint32_t lastStaAttempt = 0;

        // ini kenapa 30 detik bukan 5: ESP32 cuma 1 radio buat AP+STA,
        // jadi tiap kali coba connect STA dia scan channel lain dan itu
        // ganggu beacon AP. dulu isi 5 detik ehh AP nya jadi kedip2 pas
        // discan hp, kelamaan gasik gaenak juga jadi naikin ke 30
        static constexpr uint32_t STA_RETRY_INTERVAL = 30000;
    };

}

#endif
