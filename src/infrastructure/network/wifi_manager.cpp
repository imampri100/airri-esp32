#include "infrastructure/network/wifi_manager.h"

#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    void WifiManager::begin(
        const WifiCredential &credential)
    {
        this->credential = credential;
        staConfigured = credential.hasSta();
        apEnabled = credential.apEnabled();

        // Fail-safe: kalau AP & STA dua-duanya dimatikan, device jadi
        // sama sekali tidak bisa diakses (tidak ada cara buat konek ke
        // API-nya). Config kayak gini pasti bukan yang dimaksud user -
        // paksa AP tetap nyala supaya device tidak "hilang" total.
        if (!apEnabled && !staConfigured)
        {
            SerialLogger::warn(
                "apEnabled=false & STA tidak dikonfigurasi - device bakal "
                "sama sekali tidak bisa diakses. AP dipaksa tetap nyala "
                "sebagai fail-safe.");
            apEnabled = true;
        }

        if (apEnabled && staConfigured)
        {
            WiFi.mode(WIFI_AP_STA);
        }
        else if (apEnabled)
        {
            WiFi.mode(WIFI_AP);
        }
        else
        {
            WiFi.mode(WIFI_STA);
        }

        // Matikan WiFi power-save (modem-sleep). Defaultnya ESP32 sering
        // gagal nemu SSID (WL_NO_SSID_AVAIL) pas scan STA bersamaan AP
        // aktif di mode WIFI_AP_STA - power-save bikin radio miss
        // beacon/probe response pas time-sharing AP+STA.
        WiFi.setSleep(false);

        if (apEnabled)
        {
            WiFi.softAP(
                credential.apSsid().c_str(),
                credential.apPassword().c_str());
        }

        if (staConfigured)
        {
            logNearbyNetworks();
            connectSta();
        }
    }

    void WifiManager::logNearbyNetworks()
    {
        int found = WiFi.scanNetworks();

        SerialLogger::info(
            "Scan WiFi selesai - " + String(found) + " jaringan kelihatan:");

        for (int i = 0; i < found; i++)
        {
            SerialLogger::info(
                "  [" + String(i) + "] \"" + WiFi.SSID(i) + "\" ch=" +
                String(WiFi.channel(i)) + " rssi=" + String(WiFi.RSSI(i)) +
                "dBm");
        }

        WiFi.scanDelete();
    }

    void WifiManager::connectSta()
    {
        WiFi.begin(
            credential.staSsid().c_str(),
            credential.staPassword().c_str());

        staConnecting = true;
        lastStaAttempt = millis();
    }

    void WifiManager::tick()
    {
        if (!staConfigured)
        {
            return;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            staConnecting = false;
            return;
        }

        if (millis() - lastStaAttempt >= STA_RETRY_INTERVAL)
        {
            // Kode status WiFi.status() diagnostik: 1=NO_SSID_AVAIL (SSID
            // tidak ketemu/di luar jangkauan/salah nama), 4=CONNECT_FAILED
            // (biasanya password salah), 6=DISCONNECTED. Lihat enum
            // wl_status_t di WiFi.h buat daftar lengkap.
            SerialLogger::warn(
                "WiFi Station belum konek (status code: " +
                String(WiFi.status()) + ") - retry...");

            connectSta();
        }
    }

    bool WifiManager::isStaConnected() const
    {
        return WiFi.status() == WL_CONNECTED;
    }

    bool WifiManager::hasStaConfigured() const
    {
        return staConfigured;
    }

    bool WifiManager::isApEnabled() const
    {
        return apEnabled;
    }

    String WifiManager::ipAddress() const
    {
        return WiFi.softAPIP().toString();
    }

    String WifiManager::staIpAddress() const
    {
        return WiFi.localIP().toString();
    }

    String WifiManager::macAddress() const
    {
        return WiFi.softAPmacAddress();
    }

    uint8_t WifiManager::stationCount() const
    {
        return WiFi.softAPgetStationNum();
    }

}
