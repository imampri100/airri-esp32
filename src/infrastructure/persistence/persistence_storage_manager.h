#ifndef IRRIGATION_PERSISTENCE_STORAGE_MANAGER_H
#define IRRIGATION_PERSISTENCE_STORAGE_MANAGER_H

#include <Arduino.h>
#include <FS.h>

#include <vector>

namespace irrigation
{

    class PersistenceStorageManager
    {
    public:
        bool begin();

        bool initialize();

        bool exists(
            const String &path) const;

        File openRead(
            const String &path) const;

        File openWrite(
            const String &path);

        File openAppend(
            const String &path);

        bool remove(
            const String &path);

        bool clear(
            const String &path);

        // Nama file (bukan path lengkap) di dalam `directory`, terurut
        // ascending secara leksikografis. Dipakai repository log yang
        // dirotasi per periode ("YYYY-MM.ndjson") supaya urutan nama file
        // = urutan kronologis.
        std::vector<String> listFiles(
            const String &directory) const;

        uint32_t fileSize(
            const String &path) const;

        bool createDirectory(
            const String &path);

    private:

        bool createDefaultFile(
            const String &path,
            const String &content);
    };

}

#endif