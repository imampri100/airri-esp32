#ifndef IRRIGATION_INFRA_IRRIGATION_LOG_REPOSITORY_H
#define IRRIGATION_INFRA_IRRIGATION_LOG_REPOSITORY_H

#include <utility>

#include "domain/repository/irrigation_log_repository.h"

#include "infrastructure/mapper/irrigation_log_mapper.h"
#include "infrastructure/persistence/ndjson_reader.h"
#include "infrastructure/persistence/ndjson_writer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Implementasi IrrigationLogRepository (domain) -> SD Card, format
    // NDJSON, dirotasi 1 file per bulan di dalam `directory` - lihat
    // LogPeriod & LogRetentionTask.
    class SdIrrigationLogRepository : public IrrigationLogRepository
    {
    public:
        SdIrrigationLogRepository(
            PersistenceStorageManager &storage,
            NdjsonWriter &writer,
            NdjsonReader &reader,
            const String &directory);

        void add(const IrrigationLog &log) override;

        std::vector<IrrigationLog> getAfterId(
            uint32_t lastId,
            uint32_t limit) override;

        uint32_t getLastId() override;

        uint32_t getFirstId() override;

        uint32_t count() override;

        void clear() override;

    private:
        PersistenceStorageManager &storage;

        NdjsonWriter &writer;

        NdjsonReader &reader;

        String directory;

        // Sama seperti SdSensorLogRepository - cache lastId di RAM supaya
        // add() tidak scan ulang seluruh file NDJSON tiap kali nulis.
        uint32_t cachedLastId = 0;

        bool cachedLastIdLoaded = false;

        // Sama seperti SdSensorLogRepository - cache count() di RAM,
        // divalidasi dengan membandingkan daftar file saat ini vs
        // snapshot terakhir (`cachedCountFileList`). Lihat komentar di
        // sensor_log_repository.h untuk penjelasan lengkap kenapa ini
        // aman terhadap file yang dihapus LogRetentionTask di luar
        // repository ini.
        uint32_t cachedCount = 0;

        bool cachedCountValid = false;

        std::vector<String> cachedCountFileList;

        // Sama seperti SdSensorLogRepository - cache maxId per file yang
        // sudah "closed" supaya getAfterId() bisa skip file lama yang
        // seluruh isinya pasti <= lastId tanpa perlu buka file itu. Lihat
        // komentar di sensor_log_repository.h untuk penjelasan lengkap.
        std::vector<std::pair<String, uint32_t>> cachedFileMaxId;

        bool findCachedFileMaxId(const String &fileName, uint32_t &outMaxId) const;

        void setCachedFileMaxId(const String &fileName, uint32_t maxId);

        std::vector<String> periodFilesAscending() const;
    };

}

#endif