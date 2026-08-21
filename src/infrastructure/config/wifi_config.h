#ifndef IRRIGATION_WIFI_CONFIG_H
#define IRRIGATION_WIFI_CONFIG_H

#include <Arduino.h>

namespace irrigation
{

    namespace WifiConfig
    {
        // ESP32 SELALU jadi Access Point sendiri - HP connect langsung
        // ke SSID ini di mana pun alat dibawa, tidak butuh router/WiFi
        // eksisting di lokasi. Fallback default - HANYA dipakai kalau
        // /settings/wifi.json di SD Card masih kosong/belum diisi. Cara
        // utama ganti SSID/password AP sekarang adalah edit
        // /settings/wifi.json langsung di SD Card (gak perlu re-flash).
        // Lihat SdWifiCredentialRepository.
        constexpr char DEFAULT_AP_SSID[] = "ESP32-Irrigation";

        // Minimal 8 karakter (syarat WPA2).
        constexpr char DEFAULT_AP_PASSWORD[] = "12345678";

        // Fallback default STA (WiFi lain yang opsional di-join, mis.
        // buat fallback NTP) - HANYA dipakai kalau staSsid di
        // /settings/wifi.json masih kosong. Nilai di bawah ini dummy -
        // ganti langsung di sini (tanpa perlu edit SD Card) kalau mau
        // device otomatis coba join WiFi tertentu dari awal. Biarkan
        // dummy (nama jaringan tidak akan pernah ketemu) kalau memang
        // tidak mau device coba STA sama sekali.
        constexpr char DEFAULT_STA_SSID[] = "GANTI_SSID_WIFI_STA";

        constexpr char DEFAULT_STA_PASSWORD[] = "GANTI_PASSWORD_WIFI_STA";

        constexpr uint16_t HTTP_PORT = 80;

    }

}

#endif