#ifndef IRRIGATION_WIFI_CREDENTIAL_REPOSITORY_H
#define IRRIGATION_WIFI_CREDENTIAL_REPOSITORY_H

#include "infrastructure/network/wifi_credential.h"
#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Baca/tulis kredensial WiFi dari file JSON di SD Card
    // (/settings/wifi.json), supaya bisa diganti tanpa re-flash firmware -
    // tinggal cabut SD Card, edit filenya di laptop, pasang lagi.
    //
    // Format file:
    //   {"apSsid": "...", "apPassword": "...", "staSsid": "", "staPassword": ""}
    //
    // apSsid/apPassword = SSID/password yang di-broadcast ESP32 (Access
    // Point, selalu aktif). staSsid/staPassword = WiFi lain yang OPSIONAL
    // coba di-join (boleh dikosongkan kalau tidak dipakai).
    class SdWifiCredentialRepository
    {
    public:
        SdWifiCredentialRepository(
            PersistenceStorageManager &storage,
            JsonSerializer &serializer,
            const String &path);

        // Mengembalikan WifiCredential kosong (apSsid="") kalau file belum
        // diisi / gagal dibaca - caller yang menentukan fallback-nya.
        WifiCredential load();

        void save(const WifiCredential &credential);

    private:
        PersistenceStorageManager &storage;

        JsonSerializer &serializer;

        String path;
    };

}

#endif
