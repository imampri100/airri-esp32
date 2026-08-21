# Sistem Penyimpanan Log (SD Card)

Dokumen ini menjelaskan bagaimana firmware ESP32 menyimpan data log sensor
dan log irigasi ke SD Card: format file, skema ID, rotasi, retensi, dan
model sinkronisasi ke mobile app.

## Format file: NDJSON

Setiap log (sensor & irigasi) disimpan sebagai **NDJSON** (newline-delimited
JSON) - satu baris = satu record JSON, append-only. Dipilih karena:

- Menulis record baru = append 1 baris ke akhir file, tidak perlu
  parse-ulang/rewrite seluruh file (beda dengan JSON array tunggal yang
  butuh baca-modifikasi-tulis ulang setiap kali ada data baru).
- Tahan crash/reset di tengah penulisan - baris yang belum selesai ditulis
  gagal di-parse dan dilewati (`ok` bernilai `false`), bukan merusak
  keseluruhan file.

Implementasi baca/tulis: [`NdjsonWriter`](../src/infrastructure/persistence/ndjson_writer.h),
[`NdjsonReader`](../src/infrastructure/persistence/ndjson_reader.h).

Contoh 1 baris log sensor:

```json
{"id":128,"createdAt":1737350700,"soilMoisture":29,"airHumidity":62.8,"airTemperature":28.6,"lightIntensity":300,"isIrrigationRun":true,"irrigationLogId":0,"irrigationRunAt":0,"irrigationStopAt":0,"irrigationDurationSecond":0,"irrigationMillilitre":0,"isSynced":false,"syncedAt":0}
```

## Sesi irigasi yang belum selesai sengaja tidak dicatat

`IrrigationLog` cuma ditulis **satu kali**, sesudah pompa berhenti -
[`MonitorEnvironmentTask::recordIrrigationLog`](../src/application/automation/monitor_environment_task.cpp)
baru manggil `add()` ketika `irrigationRunAt` **dan** `irrigationStopAt`
sama-sama sudah diketahui. `pumpStartedAt` (waktu mulai) cuma disimpan di
RAM, tidak pernah ditulis ke SD Card duluan sebagai entry "pending".

Konsekuensinya: kalau device mati/reset di tengah siklus penyiraman
(mis. listrik putus), sesi itu **hilang total** dari log - tidak ada
entry dengan `irrigationStopAt` kosong, tidak ada apa-apa sama sekali.

Ini keputusan desain yang disengaja, bukan bug:

- Alternatif yang sempat dipertimbangkan - tulis entry "pending" saat
  pompa mulai nyala, lalu update/lengkapi entry itu saat pompa berhenti -
  ditolak karena berisiko lebih besar daripada tidak mencatat sama
  sekali. Kalau SD Card ditukar atau device di-reset di antara
  penulisan "pending" dan penulisan "selesai", entry pending itu bisa
  nyangkut permanen di kartu lain/kartu yang sudah beda kondisi -
  merusak asumsi "NDJSON append-only, satu baris = satu record valid"
  yang dipakai di seluruh sistem (lihat [Format file: NDJSON](#format-file-ndjson)).
- Pola "tulis sekali, sesudah fakta, hanya kalau seluruh record sudah
  diketahui" ini konsisten dipakai untuk semua log append-only NDJSON
  di project ini (`SensorLog` maupun `IrrigationLog`) - kalau ada
  kebutuhan baru untuk log serupa, ikuti pola yang sama, jangan
  tambahkan mekanisme "pending lalu di-reconcile" untuk mengatasi data
  loss akibat crash/mati listrik.

## Estimasi volume irigasi (`irrigationMillilitre`)

Device tidak punya flow sensor - sensor air murah (mis. YF-S201/YF-B1)
rata-rata butuh pipa >= 1/2 inch dan flow minimum 1-2 L/menit untuk baca
stabil, jauh di atas ukuran selang inlet/outlet pompa ini (4mm x 6mm) dan
debitnya (~200 mL/menit). Jadi `irrigationMillilitre` di `IrrigationLog`
**bukan hasil pengukuran**, melainkan estimasi dari durasi nyala pompa
dikali debit tetap:

```
irrigationMillilitre = irrigationDurationSecond * (FLOW_RATE_ML_PER_MINUTE / 60)
```

Dihitung di
[`MonitorEnvironmentTask::recordIrrigationLog`](../src/application/automation/monitor_environment_task.cpp),
konstanta debitnya ada di
[`PumpConfig::FLOW_RATE_ML_PER_MINUTE`](../src/infrastructure/config/pump_config.h) -
ubah nilai itu saja kalau pompa diganti dengan spesifikasi debit berbeda.
Akurasinya bergantung seberapa konstan debit riil pompa (bisa berubah
karena tekanan air/umur pompa), bukan pengukuran langsung.

Field `irrigationMillilitre` di `SensorLog` (contoh JSON di atas) berbeda
konteks - field itu belum pernah diisi (selalu `0`) karena `SensorLog`
tidak melalui `recordIrrigationLog`.

## Struktur direktori & rotasi bulanan

Log **tidak** disimpan sebagai satu file tunggal yang terus tumbuh. Sejak
implementasi rotasi, tiap jenis log punya direktori sendiri, isinya
dipecah **1 file per bulan kalender (UTC)**:

```
/logs/sensor/2026-06.ndjson
/logs/sensor/2026-07.ndjson
/logs/irrigation/2026-06.ndjson
/logs/irrigation/2026-07.ndjson
```

Path directory didefinisikan di
[`FileConfig::SENSOR_LOG_DIRECTORY`/`IRRIGATION_LOG_DIRECTORY`](../src/infrastructure/config/file_config.h).
Nama file (`YYYY-MM.ndjson`) dihitung dari `createdAt` record lewat
[`LogPeriod::keyFor`](../src/infrastructure/persistence/log_period.h) -
formatnya sengaja zero-padded fixed-width supaya urutan nama file secara
leksikografis sama dengan urutan kronologisnya (dipakai untuk sorting file
maupun perbandingan cutoff retensi tanpa perlu parsing ulang tanggal).

Kenapa dirotasi:

- SD Card di ESP32 memakai filesystem **FAT16/FAT32**, yang punya batas
  ukuran per file tunggal ~4GB. Dengan sensor log ditulis tiap 3 detik
  24 jam nonstop (lihat
  [`MonitorEnvironmentTask::interval()`](../src/application/automation/monitor_environment_task.cpp)),
  volumenya ~8,2 MB/hari - kalau ditulis ke satu file terus-menerus, limit
  4GB itu akan tercapai dalam ~1,3 tahun.
- Dengan rotasi bulanan, satu file cuma menampung ~246 MB/bulan (jauh di
  bawah 4GB), sekaligus membatasi berapa banyak file yang harus di-scan
  linear tiap request paginasi.

## Skema ID

ID tetap **global dan auto-increment** lintas seluruh file rotasi (bukan
reset per bulan), supaya kontrak API `lastId`/`limit` yang dipakai mobile
app untuk paginasi/incremental sync tidak berubah sama sekali walau di
baliknya data dipecah ke banyak file.

- `add()` menentukan `id` = `getLastId() + 1`, lalu menulis record itu ke
  file periode bulan berjalan.
- `getLastId()` di-cache di RAM (`cachedLastId`) - hanya di-load sekali
  (scan file periode **terbaru** saja) saat pertama dibutuhkan, sesudah
  itu tinggal +1 tiap `add()`. Ini menghindari O(n) scan ulang tiap tulis
  yang tadinya jadi O(n²) makin lama makin lambat.
- `getFirstId()` scan baris valid **pertama** dari file periode
  **tertua yang masih ada**, lalu berhenti (tidak perlu baca seluruh
  file - penulisan NDJSON selalu maju, jadi baris pertama pasti ID
  terkecil).
- `getAfterId(lastId, limit)` iterasi file periode secara ascending,
  mengumpulkan record dengan `id > lastId` sampai `limit` terpenuhi,
  lanjut ke file berikutnya kalau belum cukup. Untuk file periode yang
  sudah **closed** (bukan bulan berjalan), `maxId`-nya di-cache di RAM
  (`cachedFileMaxId`) setelah pertama kali dibaca penuh sampai baris
  terakhir - request berikutnya yang `lastId`-nya sudah >= `maxId` file
  itu langsung skip file tersebut tanpa dibuka sama sekali. Ini cuma
  membantu kasus `lastId` tertinggal **lintas bulan** (device lama tidak
  disync); kalau `lastId` masih tertinggal di **dalam** file periode yang
  sama, request itu tetap scan linear dari awal file itu (tidak ada index
  di dalam satu file NDJSON) - tapi dua hal ini yang bikin scan itu jauh
  lebih murah dibanding baca-per-baris naif:
  - [`NdjsonReader::readEach`](../src/infrastructure/persistence/ndjson_reader.cpp)
    baca file per blok 512 byte lalu split baris di memori (bukan
    `Stream::readStringUntil` yang baca 1 byte per panggilan fungsi -
    jauh lebih lambat untuk file ratusan ribu baris).
  - Baris yang bakal dibuang (`id <= lastId`) cukup diambil field `id`-nya
    lewat pencarian substring `"id":` (`tryExtractLineId`, fungsi lokal
    di `sensor_log_repository.cpp`/`irrigation_log_repository.cpp`),
    tanpa `deserializeJson()` penuh. Full parse cuma dipanggil untuk
    baris yang benar-benar lolos filter, atau kalau ekstraksi cepat
    gagal (fallback ke parser lama, termasuk penanganan baris corrupt
    yang tidak berubah).

Implementasi: [`SdSensorLogRepository`](../src/infrastructure/repository/sensor_log_repository.cpp),
[`SdIrrigationLogRepository`](../src/infrastructure/repository/irrigation_log_repository.cpp).

## Retensi & purge otomatis

[`LogRetentionTask`](../src/application/automation/log_retention_task.h)
adalah housekeeping task yang jalan otomatis di scheduler firmware
(dicek tiap 24 jam - lihat catatan di bawah soal kenapa bukan "3 bulan"
langsung).

Dua aturan, mana pun tercapai lebih dulu:

1. **Retensi kalender - 3 bulan.** File periode yang lebih tua dari
   `now - 3 bulan` dihapus (dihitung via
   [`LogPeriod::cutoffKey`](../src/infrastructure/persistence/log_period.h)
   dari waktu wall-clock RTC DS3231 (sumber utama) atau fallback NTP -
   lihat
   [`HybridTimeProvider`](../src/infrastructure/time/hybrid_time_provider.h)).
2. **Kuota ukuran - 16GB dikurangi margin aman 200MB** (`MAX_TOTAL_LOG_BYTES`
   di `log_retention_task.h`). Kalau total ukuran log (sensor + irigasi
   digabung) masih di atas itu walau masih di dalam window 3 bulan, file
   **tertua** dihapus berulang (lintas kedua directory, dipilih berdasar
   nama periode terkecil) sampai total kembali di bawah kuota.

Task ini **skip total** kalau `TimeRepository::isSynchronized()` masih
`false` - tanpa waktu yang valid, tidak ada dasar aman untuk hapus file
berdasar cutoff kalender.

Kenapa dicek tiap 24 jam, bukan langsung "tiap 3 bulan": interval task di
`Scheduler` diukur dalam milidetik `uint32_t` (berbasis `millis()`), dan
3 bulan dalam ms (~7,78 miliar) **overflow** tipe itu (maksimum ~4,29
miliar ms ≈ 49,7 hari). Jadi task-nya dicek harian (murah, hampir selalu
tidak melakukan apa-apa), tapi keputusan hapus-atau-tidak dihitung dari
`Timestamp` (unix seconds) yang tidak kena masalah overflow itu.

### Kenapa device tidak peduli status sync

Desain ini sengaja **tidak** menunggu konfirmasi dari mobile app sebelum
purge (tidak ada mekanisme "acknowledge" dari mobile app ke device).
Sinkronisasi murni langsung IoT ↔ Flutter (tanpa cloud), dan tanggung
jawab menjaga histori lengkap ada di mobile app - device hanya perlu jadi
buffer sementara. Konsekuensinya: kalau mobile app tidak sempat
menyinkron sebelum data lewat retensi (kalender atau kuota), data itu
hilang permanen dan **tidak bisa dipulihkan**.

## Deteksi gap dari sisi mobile app

Karena purge bisa menghapus data yang belum sempat diambil mobile app,
`GET /api/sync` menyediakan info untuk mendeteksi (bukan mencegah) gap
itu:

```json
{
  "storageId": 1,
  "sensorLogLastId": 15000,
  "irrigationLogLastId": 340,
  "sensorLogCount": 9600,
  "irrigationLogCount": 210,
  "sensorLogFirstId": 5401,
  "irrigationLogFirstId": 131
}
```

- `sensorLogLastId`/`irrigationLogLastId`: ID terbaru saat ini (live dari
  repository, bukan dari file metadata statis - lihat catatan di bawah).
  Dipakai mobile app sebagai acuan `lastId` request berikutnya.
- `sensorLogFirstId`/`irrigationLogFirstId`: ID **tertua** yang masih
  tersedia di device. Naik seiring waktu begitu `LogRetentionTask`
  menghapus file lama.

Mobile app membandingkan `lastSyncedId` (ID terakhir yang sudah tersimpan
di local DB-nya) dengan `sensorLogFirstId`/`irrigationLogFirstId` dari
response ini:

- Kalau `lastSyncedId + 1 < FirstId` → ada rentang ID yang sudah hilang
  dari device sebelum sempat disinkron (mis. HP lama tidak connect ke
  WiFi device). Mobile app perlu menandai histori pada rentang itu
  sebagai tidak lengkap - datanya tidak bisa dipulihkan lagi, tapi
  setidaknya diketahui ada gap, bukan diam-diam dianggap kontinu.
- Kalau tidak, request `GET /api/logs/sensor?lastId={lastSyncedId}&limit=...`
  seperti biasa aman dilanjutkan tanpa ada data yang terlewat.

> Catatan implementasi: `sensorLogLastId`/`irrigationLogLastId` sempat
> jadi bug - field itu awalnya dibaca dari file metadata statis
> (`metadata/sync.json`) yang cuma diperbarui saat factory reset, jadi
> nilainya basi begitu ada log baru masuk. Sudah diperbaiki di
> [`GetSyncUseCase`](../src/application/usecase/get_sync_usecase.cpp) -
> kedua field itu sekarang selalu diambil live dari repository, sama
> seperti `sensorLogCount`/`irrigationLogCount`.

## Endpoint terkait

Lihat detail request/response lengkap di
[`smart-irrigation-api_postman_collection.json`](./smart-irrigation-api_postman_collection.json).

| Endpoint | Fungsi |
| --- | --- |
| `GET /api/logs/sensor?lastId=&limit=` | Ambil log sensor setelah `lastId`, maksimal `limit` (di-clamp ke 200 di firmware). |
| `GET /api/logs/irrigation?lastId=&limit=` | Sama, untuk log siklus irigasi. |
| `DELETE /api/logs` | Hapus semua file log (sensor & irigasi, semua periode). Tidak bisa di-undo. |
| `GET /api/sync` | Metadata sinkronisasi: `*LastId`, `*FirstId`, `*Count` untuk sensor & irigasi. |

## Perkiraan pertumbuhan data

Dengan interval baca sensor 3 detik (24/7):

- ~28.800 record/hari, ~298 byte/record → **~8,2 MB/hari** untuk log
  sensor.
- Log irigasi jauh lebih jarang (hanya saat pompa berhenti/timeout) -
  kontribusinya ke storage bisa diabaikan.
- Tanpa retensi, kartu 32GB akan penuh dalam ~10 tahun murni dari sisi
  kapasitas - tapi limit file tunggal FAT32 (4GB) akan tercapai jauh
  lebih dulu (~1,3 tahun) kalau tidak dirotasi. Retensi 3 bulan / kuota
  16GB di atas memberi margin jauh di bawah kedua batas itu.
