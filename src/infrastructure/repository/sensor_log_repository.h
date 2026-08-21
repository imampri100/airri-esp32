#ifndef IRRIGATION_INFRA_SENSOR_LOG_REPOSITORY_H
#define IRRIGATION_INFRA_SENSOR_LOG_REPOSITORY_H

#include <utility>

#include "domain/repository/sensor_log_repository.h"

#include "infrastructure/mapper/sensor_log_mapper.h"
#include "infrastructure/persistence/ndjson_reader.h"
#include "infrastructure/persistence/ndjson_writer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Implementasi SensorLogRepository (domain) yang menyimpan data ke
    // SD Card dalam format NDJSON (1 baris = 1 JSON record, append-only),
    // dirotasi 1 file per bulan di dalam `directory` (mis.
    // /logs/sensor/2026-07.ndjson) - lihat LogPeriod & LogRetentionTask.
    class SdSensorLogRepository : public SensorLogRepository
    {
    public:
        SdSensorLogRepository(
            PersistenceStorageManager &storage,
            NdjsonWriter &writer,
            NdjsonReader &reader,
            const String &directory);

        void add(const SensorLog &log) override;

        std::vector<SensorLog> getAfterId(
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

        // Cache lastId di RAM supaya add() tidak perlu scan ulang SELURUH
        // file NDJSON tiap kali nulis (dulu O(n) per tulis -> O(n^2) total,
        // makin lama file makin gede makin lambat/blocking). Di-load sekali
        // (lazy, saat pertama dibutuhkan), setelah itu tinggal +1 di RAM.
        uint32_t cachedLastId = 0;

        bool cachedLastIdLoaded = false;

        // Cache count() di RAM dengan alasan sama seperti cachedLastId -
        // count() dulu O(n) baca ulang SELURUH file NDJSON tiap dipanggil
        // (mahal buat GET /api/sync kalau log sudah ratusan ribu baris).
        // Validitas cache dicek dengan membandingkan daftar file saat ini
        // vs `cachedCountFileList` (snapshot saat cache terakhir dihitung
        // penuh) - kalau beda (LogRetentionTask hapus file lama, atau
        // rotasi ke file bulan baru), berarti stale, hitung ulang penuh
        // sekali lalu cache lagi. add() cukup +1 tanpa perlu tahu soal
        // retention - mismatch di atas otomatis mengoreksi diri sendiri
        // di pemanggilan count() berikutnya kalau incrementnya "salah"
        // (mis. gara-gara file baru muncul).
        uint32_t cachedCount = 0;

        bool cachedCountValid = false;

        std::vector<String> cachedCountFileList;

        // Cache maxId per file NDJSON yang sudah "closed" (bukan file
        // bulan berjalan, yang masih bisa nambah baris) - dipakai
        // getAfterId() supaya bisa skip file lama yang seluruh isinya
        // pasti sudah <= lastId, tanpa perlu buka file itu sama sekali.
        // Hanya diisi kalau file berhasil dibaca PENUH sampai baris
        // terakhir (bukan berhenti di tengah gara-gara limit tercapai),
        // supaya nilainya pasti benar - tidak pakai trik seek-dari-akhir
        // supaya tidak salah kalau baris terakhir sempat corrupt/partial
        // (mis. mati listrik saat menulis).
        std::vector<std::pair<String, uint32_t>> cachedFileMaxId;

        bool findCachedFileMaxId(const String &fileName, uint32_t &outMaxId) const;

        void setCachedFileMaxId(const String &fileName, uint32_t maxId);

        // Nama file (bukan path lengkap) di `directory`, terurut ascending
        // - urutan nama "YYYY-MM.ndjson" = urutan kronologis.
        std::vector<String> periodFilesAscending() const;
    };

}

#endif