#include "infrastructure/persistence/ndjson_reader.h"

#include <cstring>

namespace irrigation
{

    namespace
    {
        // Arduino Stream::readStringUntil() baca 1 byte per panggilan
        // fungsi (lihat cores/esp32/Stream.cpp) - sangat lambat untuk
        // file ratusan ribu baris. Sebagai gantinya baca blok besar
        // sekaligus ke buffer ini, baru split baris di memori (memchr).
        // Ukuran dipilih supaya muat beberapa baris NDJSON sekaligus
        // (~200-300 byte/baris) tanpa buffer stack yang berlebihan.
        constexpr size_t READ_BUFFER_SIZE = 512;
    }

    NdjsonReader::NdjsonReader(
        PersistenceStorageManager &storage)
        : storage(storage)
    {
    }

    bool NdjsonReader::readEach(
        const String &path,
        const std::function<bool(const String &)> &callback)
    {
        File file =
            storage.openRead(path);

        if (!file)
        {
            return false;
        }

        uint8_t buffer[READ_BUFFER_SIZE];

        // Baris yang kepotong di batas blok buffer (belum ketemu '\n'
        // dalam blok yang sama) - disambung ke blok berikutnya sebelum
        // diserahkan ke callback.
        String pending;

        bool keepGoing = true;

        while (keepGoing)
        {
            size_t bytesRead = file.read(buffer, READ_BUFFER_SIZE);

            if (bytesRead == 0)
            {
                break;
            }

            size_t start = 0;

            while (start < bytesRead && keepGoing)
            {
                void *newlinePtr =
                    memchr(buffer + start, '\n', bytesRead - start);

                if (newlinePtr == nullptr)
                {
                    pending.concat(
                        reinterpret_cast<const char *>(buffer + start),
                        bytesRead - start);

                    start = bytesRead;

                    continue;
                }

                size_t newlineIndex =
                    reinterpret_cast<const uint8_t *>(newlinePtr) - buffer;

                size_t segmentLength = newlineIndex - start;

                if (pending.length() > 0)
                {
                    pending.concat(
                        reinterpret_cast<const char *>(buffer + start),
                        segmentLength);

                    keepGoing = callback(pending);

                    pending = "";
                }
                else
                {
                    String line(
                        reinterpret_cast<const char *>(buffer + start),
                        segmentLength);

                    keepGoing = callback(line);
                }

                start = newlineIndex + 1;
            }
        }

        // Baris terakhir tanpa newline penutup (mis. file terpotong saat
        // penulisan gara-gara mati listrik) - tetap diserahkan ke
        // callback, sama seperti perilaku readStringUntil sebelumnya
        // (baris sisa di EOF tanpa '\n' tetap ikut diproses).
        if (keepGoing && pending.length() > 0)
        {
            callback(pending);
        }

        file.close();

        return true;
    }

}