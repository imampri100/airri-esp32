#include "infrastructure/persistence/persistence_storage_manager.h"

#include <SD.h>

#include <algorithm>

#include "infrastructure/config/file_config.h"
#include "infrastructure/config/pin_config.h"
#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    bool PersistenceStorageManager::begin()
    {
        bool ok = SD.begin(PinConfig::SD_CS);

        if (!ok)
        {
            SerialLogger::error(
                "SD.begin() gagal - cek: kartu terpasang benar, format "
                "FAT32, wiring CS/SCK/MOSI/MISO, dan suplai daya modul SD.");
            return false;
        }

        uint8_t cardType = SD.cardType();
        if (cardType == CARD_NONE)
        {
            SerialLogger::error(
                "SD.begin() sukses tapi tidak ada kartu SD terdeteksi "
                "(CARD_NONE) - cek kartu terpasang dengan benar.");
            return false;
        }

        SerialLogger::info("SD Card terdeteksi, tipe: " + String(cardType));

        return true;
    }

    bool PersistenceStorageManager::initialize()
    {
        // Coba buat semua directory/file, JANGAN berhenti di kegagalan
        // pertama - supaya semua masalah kelihatan sekaligus di log,
        // bukan cuma yang pertama ditemukan.
        bool ok = true;

        if (!createDirectory(FileConfig::LOG_DIRECTORY))
        {
            SerialLogger::error("Gagal membuat directory " + String(FileConfig::LOG_DIRECTORY));
            ok = false;
        }

        if (!createDirectory(FileConfig::SENSOR_LOG_DIRECTORY))
        {
            SerialLogger::error("Gagal membuat directory " + String(FileConfig::SENSOR_LOG_DIRECTORY));
            ok = false;
        }

        if (!createDirectory(FileConfig::IRRIGATION_LOG_DIRECTORY))
        {
            SerialLogger::error("Gagal membuat directory " + String(FileConfig::IRRIGATION_LOG_DIRECTORY));
            ok = false;
        }

        if (!createDirectory(FileConfig::SETTING_DIRECTORY))
        {
            SerialLogger::error("Gagal membuat directory " + String(FileConfig::SETTING_DIRECTORY));
            ok = false;
        }

        if (!createDirectory(FileConfig::METADATA_DIRECTORY))
        {
            SerialLogger::error("Gagal membuat directory " + String(FileConfig::METADATA_DIRECTORY));
            ok = false;
        }

        if (!createDefaultFile(FileConfig::TRIGGER_SETTING, "{}"))
        {
            SerialLogger::error("Gagal membuat file " + String(FileConfig::TRIGGER_SETTING));
            ok = false;
        }

        if (!createDefaultFile(FileConfig::RESTRICTION_SETTING, "{}"))
        {
            SerialLogger::error("Gagal membuat file " + String(FileConfig::RESTRICTION_SETTING));
            ok = false;
        }

        if (!createDefaultFile(FileConfig::SYNC_METADATA, "{}"))
        {
            SerialLogger::error("Gagal membuat file " + String(FileConfig::SYNC_METADATA));
            ok = false;
        }

        if (!createDefaultFile(
                FileConfig::WIFI_CREDENTIAL,
                "{\"apSsid\":\"\",\"apPassword\":\"\",\"staSsid\":\"\",\"staPassword\":\"\","
                "\"apEnabled\":true,\"staEnabled\":false}"))
        {
            SerialLogger::error("Gagal membuat file " + String(FileConfig::WIFI_CREDENTIAL));
            ok = false;
        }

        if (!createDefaultFile(FileConfig::LANGUAGE_SETTING, "{\"language\":\"id\"}"))
        {
            SerialLogger::error("Gagal membuat file " + String(FileConfig::LANGUAGE_SETTING));
            ok = false;
        }

        if (ok)
        {
            SerialLogger::info("SD Card siap - semua directory/file default sudah ada");
        }

        return ok;
    }

    bool PersistenceStorageManager::exists(
        const String &path) const
    {
        return SD.exists(path);
    }

    File PersistenceStorageManager::openRead(
        const String &path) const
    {
        return SD.open(
            path,
            FILE_READ);
    }

    File PersistenceStorageManager::openWrite(
        const String &path)
    {
        return SD.open(
            path,
            FILE_WRITE);
    }

    File PersistenceStorageManager::openAppend(
        const String &path)
    {
        return SD.open(
            path,
            FILE_APPEND);
    }

    bool PersistenceStorageManager::remove(
        const String &path)
    {
        if (!exists(path))
        {
            return true;
        }

        return SD.remove(path);
    }

    bool PersistenceStorageManager::clear(
        const String &path)
    {
        remove(path);

        File file =
            openWrite(path);

        if (!file)
        {
            return false;
        }

        file.close();

        return true;
    }

    std::vector<String> PersistenceStorageManager::listFiles(
        const String &directory) const
    {
        std::vector<String> files;

        File dir = SD.open(directory);

        if (!dir || !dir.isDirectory())
        {
            return files;
        }

        File entry = dir.openNextFile();

        while (entry)
        {
            if (!entry.isDirectory())
            {
                String name = String(entry.name());

                int slash = name.lastIndexOf('/');
                if (slash >= 0)
                {
                    name = name.substring(slash + 1);
                }

                files.push_back(name);
            }

            entry.close();
            entry = dir.openNextFile();
        }

        dir.close();

        std::sort(files.begin(), files.end());

        return files;
    }

    uint32_t PersistenceStorageManager::fileSize(
        const String &path) const
    {
        File file = openRead(path);

        if (!file)
        {
            return 0;
        }

        uint32_t size = file.size();

        file.close();

        return size;
    }

    bool PersistenceStorageManager::createDirectory(
        const String &path)
    {
        if (SD.exists(path))
        {
            return true;
        }

        return SD.mkdir(path);
    }

    bool PersistenceStorageManager::createDefaultFile(
        const String &path,
        const String &content)
    {
        if (exists(path))
        {
            return true;
        }

        File file =
            openWrite(path);

        if (!file)
        {
            return false;
        }

        file.print(content);

        file.close();

        return true;
    }

}
