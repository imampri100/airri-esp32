#include "infrastructure/repository/irrigation_log_repository.h"

#include "infrastructure/persistence/log_period.h"
#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    namespace
    {
        // Sama seperti SdSensorLogRepository - lihat komentar di
        // sensor_log_repository.cpp untuk penjelasan lengkap.
        bool tryExtractLineId(const String &line, uint32_t &outId)
        {
            int keyIndex = line.indexOf("\"id\":");

            if (keyIndex < 0)
            {
                return false;
            }

            int digitStart = keyIndex + 5;
            int digitEnd = digitStart;
            int length = line.length();

            uint32_t value = 0;

            while (digitEnd < length && isDigit(line[digitEnd]))
            {
                value = value * 10 + (line[digitEnd] - '0');
                digitEnd++;
            }

            if (digitEnd == digitStart)
            {
                return false;
            }

            outId = value;

            return true;
        }
    }

    SdIrrigationLogRepository::SdIrrigationLogRepository(
        PersistenceStorageManager &storage,
        NdjsonWriter &writer,
        NdjsonReader &reader,
        const String &directory)
        : storage(storage),
          writer(writer),
          reader(reader),
          directory(directory)
    {
    }

    std::vector<String> SdIrrigationLogRepository::periodFilesAscending() const
    {
        return storage.listFiles(directory);
    }

    void SdIrrigationLogRepository::add(const IrrigationLog &log)
    {
        IrrigationLog toSave = log;
        toSave.id = getLastId() + 1;

        String period = LogPeriod::keyFor(toSave.createdAt.value());
        String path = directory + "/" + period + ".ndjson";

        StaticJsonDocument<384> document;

        IrrigationLogMapper::toDocument(toSave, document);

        if (!writer.append(path, document))
        {
            SerialLogger::error(
                "SdIrrigationLogRepository: gagal simpan irrigation log id=" +
                String(toSave.id));

            return;
        }

        cachedLastId = toSave.id;

        if (cachedCountValid)
        {
            cachedCount++;
        }
    }

    bool SdIrrigationLogRepository::findCachedFileMaxId(
        const String &fileName,
        uint32_t &outMaxId) const
    {
        for (const std::pair<String, uint32_t> &entry : cachedFileMaxId)
        {
            if (entry.first == fileName)
            {
                outMaxId = entry.second;

                return true;
            }
        }

        return false;
    }

    void SdIrrigationLogRepository::setCachedFileMaxId(
        const String &fileName,
        uint32_t maxId)
    {
        for (std::pair<String, uint32_t> &entry : cachedFileMaxId)
        {
            if (entry.first == fileName)
            {
                entry.second = maxId;

                return;
            }
        }

        cachedFileMaxId.push_back(std::make_pair(fileName, maxId));
    }

    std::vector<IrrigationLog> SdIrrigationLogRepository::getAfterId(
        uint32_t lastId,
        uint32_t limit)
    {
        // Instrumentasi sementara - lihat komentar di
        // SdSensorLogRepository::getAfterId untuk penjelasan lengkap.
        unsigned long startMs = millis();

        std::vector<IrrigationLog> result;

        std::vector<String> files = periodFilesAscending();

        uint32_t filesOpened = 0;
        uint32_t filesSkippedViaCache = 0;
        uint32_t linesScanned = 0;

        for (size_t i = 0; i < files.size(); i++)
        {
            const String &fileName = files[i];

            // File terakhir (bulan berjalan) masih bisa nambah baris
            // lewat add(), jadi maxId-nya tidak boleh di-cache/dipercaya.
            bool isClosedFile = (i + 1 < files.size());

            uint32_t cachedMaxId = 0;

            if (isClosedFile &&
                findCachedFileMaxId(fileName, cachedMaxId) &&
                cachedMaxId <= lastId)
            {
                // Seluruh isi file ini sudah pasti <= lastId - tidak perlu
                // buka file ini sama sekali.
                filesSkippedViaCache++;

                continue;
            }

            filesOpened++;

            String path = directory + "/" + fileName;

            uint32_t maxIdSeen = 0;
            bool scannedFully = true;

            reader.readEach(
                path,
                [&](const String &line) -> bool
                {
                    if (line.length() == 0)
                    {
                        return true;
                    }

                    linesScanned++;

                    uint32_t quickId = 0;

                    if (tryExtractLineId(line, quickId))
                    {
                        if (quickId > maxIdSeen)
                        {
                            maxIdSeen = quickId;
                        }

                        if (quickId <= lastId)
                        {
                            // Baris ini pasti tidak lolos filter - skip
                            // tanpa deserializeJson penuh.
                            return true;
                        }
                    }

                    bool ok = false;
                    IrrigationLog log = IrrigationLogMapper::fromJsonLine(line, ok);

                    if (!ok)
                    {
                        return true;
                    }

                    if (log.id > maxIdSeen)
                    {
                        maxIdSeen = log.id;
                    }

                    if (log.id > lastId)
                    {
                        result.push_back(log);
                    }

                    if (result.size() >= limit)
                    {
                        scannedFully = false;

                        return false;
                    }

                    return true;
                });

            // Cache maxId hanya kalau file benar-benar sudah closed dan
            // berhasil dibaca sampai baris terakhir.
            if (isClosedFile && scannedFully && maxIdSeen > 0)
            {
                setCachedFileMaxId(fileName, maxIdSeen);
            }

            if (result.size() >= limit)
            {
                break;
            }
        }

        SerialLogger::info(
            "SdIrrigationLogRepository::getAfterId lastId=" + String(lastId) +
            " limit=" + String(limit) +
            " filesTotal=" + String(files.size()) +
            " filesOpened=" + String(filesOpened) +
            " filesSkippedViaCache=" + String(filesSkippedViaCache) +
            " linesScanned=" + String(linesScanned) +
            " result=" + String(result.size()) +
            " durationMs=" + String(millis() - startMs));

        return result;
    }

    uint32_t SdIrrigationLogRepository::getLastId()
    {
        if (cachedLastIdLoaded)
        {
            return cachedLastId;
        }

        uint32_t lastId = 0;

        std::vector<String> files = periodFilesAscending();

        if (!files.empty())
        {
            String path = directory + "/" + files.back();

            reader.readEach(
                path,
                [&](const String &line) -> bool
                {
                    if (line.length() == 0)
                    {
                        return true;
                    }

                    bool ok = false;
                    IrrigationLog log = IrrigationLogMapper::fromJsonLine(line, ok);

                    if (ok && log.id > lastId)
                    {
                        lastId = log.id;
                    }

                    return true;
                });
        }

        cachedLastId = lastId;
        cachedLastIdLoaded = true;

        return lastId;
    }

    uint32_t SdIrrigationLogRepository::getFirstId()
    {
        for (const String &fileName : periodFilesAscending())
        {
            String path = directory + "/" + fileName;

            uint32_t firstId = 0;
            bool found = false;

            reader.readEach(
                path,
                [&](const String &line) -> bool
                {
                    if (line.length() == 0)
                    {
                        return true;
                    }

                    bool ok = false;
                    IrrigationLog log = IrrigationLogMapper::fromJsonLine(line, ok);

                    if (!ok)
                    {
                        return true;
                    }

                    firstId = log.id;
                    found = true;

                    return false;
                });

            if (found)
            {
                return firstId;
            }
        }

        return 0;
    }

    uint32_t SdIrrigationLogRepository::count()
    {
        std::vector<String> files = periodFilesAscending();

        if (cachedCountValid && files == cachedCountFileList)
        {
            return cachedCount;
        }

        uint32_t total = 0;

        for (const String &fileName : files)
        {
            String path = directory + "/" + fileName;

            reader.readEach(
                path,
                [&](const String &line) -> bool
                {
                    if (line.length() > 0)
                    {
                        total++;
                    }

                    return true;
                });
        }

        cachedCount = total;
        cachedCountValid = true;
        cachedCountFileList = files;

        return total;
    }

    void SdIrrigationLogRepository::clear()
    {
        for (const String &fileName : periodFilesAscending())
        {
            storage.remove(directory + "/" + fileName);
        }

        cachedLastId = 0;
        cachedLastIdLoaded = true;

        cachedCount = 0;
        cachedCountValid = true;
        cachedCountFileList.clear();

        cachedFileMaxId.clear();
    }

}
