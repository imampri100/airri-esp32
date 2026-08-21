#ifndef IRRIGATION_FILE_CONFIG_H
#define IRRIGATION_FILE_CONFIG_H

#include <Arduino.h>

namespace irrigation
{

    class FileConfig
    {
    public:
        static constexpr const char *ROOT = "/";

        static constexpr const char *LOG_DIRECTORY = "/logs";

        static constexpr const char *SETTING_DIRECTORY = "/settings";

        static constexpr const char *METADATA_DIRECTORY = "/metadata";

        // Log dirotasi 1 file per bulan di dalam directory ini (mis.
        // /logs/sensor/2026-07.ndjson), bukan 1 file tunggal yang terus
        // tumbuh - supaya tidak pernah kena limit ukuran file FAT32 (~4GB).
        // Lihat LogRetentionTask untuk kebijakan retensi/purge-nya.
        static constexpr const char *SENSOR_LOG_DIRECTORY =
            "/logs/sensor";

        static constexpr const char *IRRIGATION_LOG_DIRECTORY =
            "/logs/irrigation";

        static constexpr const char *TRIGGER_SETTING =
            "/settings/trigger.json";

        static constexpr const char *RESTRICTION_SETTING =
            "/settings/restriction.json";

        static constexpr const char *SYNC_METADATA =
            "/metadata/sync.json";

        static constexpr const char *WIFI_CREDENTIAL =
            "/settings/wifi.json";

        static constexpr const char *LANGUAGE_SETTING =
            "/settings/language.json";
    };

}

#endif