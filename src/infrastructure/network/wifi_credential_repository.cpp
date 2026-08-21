#include "infrastructure/network/wifi_credential_repository.h"

#include <ArduinoJson.h>

namespace irrigation
{

    SdWifiCredentialRepository::SdWifiCredentialRepository(
        PersistenceStorageManager &storage,
        JsonSerializer &serializer,
        const String &path)
        : storage(storage),
          serializer(serializer),
          path(path)
    {
    }

    WifiCredential SdWifiCredentialRepository::load()
    {
        File file = storage.openRead(path);
        if (!file)
        {
            return WifiCredential();
        }

        StaticJsonDocument<384> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        if (error)
        {
            return WifiCredential();
        }

        String apSsid = document["apSsid"] | "";
        String apPassword = document["apPassword"] | "";
        String staSsid = document["staSsid"] | "";
        String staPassword = document["staPassword"] | "";

        // apEnabled default true (AP selalu aktif kalau belum diatur).
        // staEnabled default FALSE (beda dari AP) - kalau field ini belum
        // ada di file (mis. wifi.json lama dari sebelum toggle ini
        // ditambahkan, meski staSsid-nya sudah terisi), STA tidak akan
        // dicoba sampai staEnabled eksplisit di-set true. Lihat komentar
        // di WifiCredential.h buat alasannya.
        bool apEnabled = document["apEnabled"] | true;
        bool staEnabled = document["staEnabled"] | false;

        return WifiCredential(apSsid, apPassword, staSsid, staPassword, apEnabled, staEnabled);
    }

    void SdWifiCredentialRepository::save(const WifiCredential &credential)
    {
        StaticJsonDocument<384> document;

        document["apSsid"] = credential.apSsid();
        document["apPassword"] = credential.apPassword();
        document["staSsid"] = credential.staSsid();
        document["staPassword"] = credential.staPassword();
        document["apEnabled"] = credential.apEnabled();
        document["staEnabled"] = credential.staEnabled();

        storage.remove(path);
        File file = storage.openWrite(path);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

}
