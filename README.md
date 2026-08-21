# Smart Irrigation — IoT Firmware (ESP32)

Firmware sistem irigasi otomatis berbasis pemantauan lingkungan (soil moisture, suhu, kelembapan udara, intensitas cahaya) menggunakan ESP32, dengan REST API untuk dikonsumsi mobile app (Flutter).

Dibangun pakai pola **Clean Architecture** (domain → application → infrastructure → presentation) supaya business logic (kapan pompa harus nyala) terpisah dari detail hardware/network.

---

## Daftar Isi

- [Hardware](#hardware)
- [Arsitektur & Struktur Folder](#arsitektur--struktur-folder)
- [Persiapan](#persiapan)
- [Konfigurasi Sebelum Flash](#konfigurasi-sebelum-flash)
- [Build & Upload](#build--upload)
- [REST API](#rest-api)
- [Testing via Postman](#testing-via-postman)
- [Known Issues & Assumptions](#known-issues--assumptions)
- [Next Improvement](#next-improvement)
- [Catatan Lain](#catatan-lain)

---

## Hardware

| Komponen | Pin ESP32 | Keterangan |
|---|---|---|
| Soil moisture sensor | GPIO 32 (ADC) | Kalibrasi wajib, lihat [Konfigurasi](#konfigurasi-sebelum-flash) |
| AHT10 (suhu + kelembapan udara) | GPIO 21 (SDA) / GPIO 22 (SCL) | I2C, share bus sama BH1750 + RTC DS3231 |
| BH1750 (cahaya/lux) | GPIO 21 (SDA) / GPIO 22 (SCL) | I2C, share bus sama AHT10 + RTC DS3231 |
| RTC DS3231 (jam/waktu) | GPIO 21 (SDA) / GPIO 22 (SCL) + VCC 3.3V + GND | I2C (alamat `0x68`), share bus sama AHT10/BH1750. Wajib ada baterai coin cell CR2032 di modul supaya jam tetap jalan walau device mati. Tidak perlu resistor pull-up tambahan (sudah ada di modul breakout). Pin **SQW** & **32K** di modul **tidak disambung/dibiarkan floating** - tidak dipakai (firmware cuma polling `rtc.now()` lewat I2C, tidak pakai alarm/interrupt maupun output clock 32kHz-nya) |
| Relay pompa | GPIO 14 | Aktif LOW. Debit ~200 mL/menit, selang inlet/outlet 4mm x 6mm - lihat [Konfigurasi](#konfigurasi-sebelum-flash) soal kalibrasi estimasi volume |
| LED status hijau | GPIO 25 | Idle |
| LED status kuning | GPIO 26 | Watering / Syncing (blink) |
| LED status merah | GPIO 27 | Error (blink) |
| TFT GC9A01 (round display) | CS=33, DC=16, RES=17 | SPI, share bus sama SD Card |
| SD Card | CS=5 | SPI (SCK=18, MOSI=23, MISO=19 — default VSPI ESP32) |
| Tombol manual | GPIO 0 | Asumsi pakai tombol BOOT bawaan DevKit, sesuaikan kalau pakai tombol eksternal |

Board target: **ESP32 DevKit V1 (30 pin)**.

> Default-nya ESP32 jadi Access Point sendiri (HP connect langsung ke SSID
> yang di-broadcast device, tidak perlu router/WiFi eksisting di lokasi).
> Opsional, device juga bisa SEKALIGUS join WiFi lain (Station/STA) kalau
> dikonfigurasi - dipakai buat fallback NTP kalau RTC bermasalah. AP dan
> STA masing-masing punya toggle on/off sendiri (`apEnabled`/`staEnabled`
> di `/settings/wifi.json`), lihat [Konfigurasi](#konfigurasi-sebelum-flash).
> Sumber waktu utama tetap **RTC DS3231** (tidak butuh WiFi/internet sama
> sekali) - NTP cuma fallback, tidak wajib.

---

## Arsitektur & Struktur Folder

```
src/
├── domain/            # Business rules murni, tidak tahu ESP32/hardware sama sekali
│   ├── entity/         # SensorReading, SensorLog, IrrigationLog, TriggerSetting, dst.
│   ├── repository/     # Interface abstrak (SensorRepository, PumpRepository, dst.)
│   ├── provider/        # TimeProvider
│   └── service/         # DecisionEngine — logika "kapan pompa harus nyala"
│
├── application/        # Orkestrasi use case, masih platform-agnostic
│   ├── usecase/         # GetStatusUseCase, PumpTestUseCase, dst.
│   ├── automation/      # Scheduler + task berkala (monitor, sync waktu, maintenance)
│   └── dto/              # StatusDto, SyncDto
│
├── infrastructure/     # Implementasi konkret, boleh depend ke library ESP32
│   ├── hardware/         # SensorManager, PumpManager, LedManager, DisplayManager, ButtonManager
│   ├── network/          # HttpServer, Router, WifiManager, handler REST API
│   ├── persistence/      # Baca/tulis SD Card (JSON & NDJSON)
│   ├── repository/       # Implementasi repository domain → SD Card ("Sd*Repository")
│   ├── mapper/           # Entity <-> JsonDocument
│   ├── i18n/               # Translator — terjemahan ID/EN buat decision.reason & label TFT
│   ├── time/              # RtcTimeProvider (utama) + NetworkTimeProvider (fallback NTP) + HybridTimeProvider (gabungan)
│   └── config/            # pin_config.h, wifi_config.h, file_config.h
│
├── presentation/
│   └── serial/            # SerialLogger (log ke Serial Monitor)
│
└── main.cpp             # Composition root — merakit semua dependency
```

Aturan arah dependency: `infrastructure` & `application` boleh depend ke `domain`, tapi **tidak sebaliknya**. `domain` tidak boleh tahu apa-apa soal ESP32/Arduino/SD Card.

---

## Persiapan

- [PlatformIO](https://platformio.org/) (extension VSCode atau CLI)
- ESP32 DevKit V1 + wiring sesuai tabel di atas
- MicroSD Card (format FAT32) terpasang di module SD Card reader

Semua dependency library sudah didaftarkan di `platformio.ini`, PlatformIO akan otomatis download saat build pertama kali (butuh koneksi internet):

```ini
lib_deps =
    bblanchon/ArduinoJson @ ^6.21.5
    adafruit/Adafruit AHTX0 @ ^2.0.5
    adafruit/Adafruit Unified Sensor @ ^1.1.14
    claws/BH1750 @ ^1.3.0
    moononournation/GFX Library for Arduino @ 1.4.9
    adafruit/RTClib @ ^2.1.4
```

> ⚠️ Versi GFX Library **sengaja dipin ke `1.4.9`** (bukan versi terbaru). Lihat [Known Issues](#known-issues--assumptions).

---

## Konfigurasi Sebelum Flash

Yang perlu disesuaikan sebelum/sesudah upload ke device:

### 1. WiFi Access Point + Station (opsional) — via SD Card (tidak perlu re-flash)

Default-nya ESP32 jadi Access Point sendiri - HP connect langsung ke SSID yang di-broadcast device ini. Opsional, device juga bisa SEKALIGUS join WiFi lain (Station/STA) kalau dikonfigurasi - berguna buat fallback NTP (lihat poin 2) kalau RTC bermasalah. AP dan STA masing-masing punya **toggle on/off sendiri** (`apEnabled`/`staEnabled`), independen dari isi SSID-nya:
- `staEnabled: false` - cara paling bersih buat matikan percobaan STA sama sekali, kredensialnya (`staSsid`/`staPassword`) tetap tersimpan di file (tidak perlu dihapus/ditulis ulang kalau nanti mau diaktifkan lagi). Ini juga solusi buat isu AP kedip-kedip/susah kelihatan pas discan HP kalau SSID STA yang dikonfigurasi tidak terjangkau di lokasi (lihat [Known Issues](#known-issues--assumptions)).
- `apEnabled: false` - matikan Access Point sepenuhnya (device cuma bisa diakses lewat STA/WiFi lain). Kalau `staEnabled` juga `false` (device jadi sama sekali tidak bisa diakses), firmware otomatis maksa AP tetap nyala sebagai fail-safe (lihat log Serial `apEnabled=false & STA tidak dikonfigurasi...` kalau ini kejadian).

Kredensial (AP wajib, STA opsional) dibaca dari file **`/settings/wifi.json`** di SD Card saat boot, bukan dari kode. File ini otomatis dibuat kosong pas pertama kali SD Card dipakai (path-nya `FileConfig::WIFI_CREDENTIAL`, `src/infrastructure/config/file_config.h`):

```json
{"apSsid": "", "apPassword": "", "staSsid": "", "staPassword": "", "apEnabled": true, "staEnabled": false}
```

File ini **cuma dibuat kalau belum ada** (`PersistenceStorageManager::createDefaultFile`) - sekali sudah diisi/diedit, isinya tidak akan ditimpa balik ke default kosong di boot-boot berikutnya, termasuk setelah re-flash firmware (SD Card tidak ikut ke-flash).

Cara ganti:
1. Cabut SD Card dari module, colok ke laptop (pakai card reader).
2. Buka `/settings/wifi.json`, edit jadi:
   ```json
   {"apSsid": "nama_ap_baru", "apPassword": "password_ap_baru", "staSsid": "wifi_rumah", "staPassword": "password_wifi_rumah", "apEnabled": true, "staEnabled": true}
   ```
   Set `staEnabled: false` kalau tidak mau device coba join WiFi lain sama sekali (kredensialnya boleh tetap diisi, tidak akan dicoba selama `staEnabled` `false`).
3. Simpan, pasang lagi SD Card ke ESP32, restart device (tombol reset atau cabut-colok power). **Tidak perlu buka VSCode / upload ulang.**

`apEnabled` default `true` dan `staEnabled` default **`false`** kalau field-nya belum ada di file (mis. `wifi.json` lama dari sebelum toggle ini ditambahkan) - AP tetap otomatis jalan seperti biasa, tapi STA **tidak** otomatis dicoba lagi meskipun `staSsid` sudah terisi dari sebelumnya. Ini beda dari perilaku lama - kalau `staSsid` sudah pernah diisi sebelum update firmware ini dan STA-nya masih mau dipakai, tambahkan `"staEnabled": true` secara eksplisit di file-nya.

**Fallback**: kalau `apSsid`/`staSsid` di file itu masih kosong (atau SD Card gagal dibaca), firmware otomatis pakai default dari `src/infrastructure/config/wifi_config.h` - ganti langsung di sana (lalu re-flash) kalau malas cabut-colok SD Card tiap mau ganti WiFi Station:
```cpp
constexpr char DEFAULT_AP_SSID[] = "ESP32-Irrigation";
constexpr char DEFAULT_AP_PASSWORD[] = "12345678"; // minimal 8 karakter (syarat WPA2)

constexpr char DEFAULT_STA_SSID[] = "GANTI_SSID_WIFI_STA";     // dummy, ganti sesuai WiFi tujuan
constexpr char DEFAULT_STA_PASSWORD[] = "GANTI_PASSWORD_WIFI_STA";
```
Kredensial di `wifi.json` SD Card, kalau diisi, selalu menang atas default ini. Kalau ada 2+ alat dites berdekatan, ganti `apSsid`-nya per alat supaya tidak sama persis. Beda dengan SSID/password, `apEnabled`/`staEnabled` tidak punya default di `wifi_config.h` - murni dari SD Card (`apEnabled` default `true`, `staEnabled` default `false` kalau belum diisi).

> Implementasi: `SdWifiCredentialRepository` (`src/infrastructure/network/wifi_credential_repository.h/.cpp`), `WifiManager` (`src/infrastructure/network/wifi_manager.h/.cpp`, mode `WIFI_AP_STA`). Kelas repository juga punya method `save()` kalau nanti mau ditambah endpoint API buat ganti kredensial dari app tanpa cabut SD Card sama sekali.

### 2. RTC DS3231 — set waktu awal (sumber waktu utama)

Sumber waktu utama device adalah modul RTC DS3231 (lihat tabel [Hardware](#hardware) untuk wiring), **tidak** butuh WiFi/internet sama sekali. NTP cuma dipakai sebagai fallback - baru benar-benar disentuh kalau RTC gagal diinisialisasi (chip tidak kedeteksi/rusak) **dan** WiFi Station (poin 1 di atas) berhasil connect ke internet.

Saat pertama kali RTC dipasang (chip baru) atau baterai coin cell-nya pernah habis, firmware otomatis fallback set waktu dari **jam laptop/PC saat kompilasi & upload** (`__DATE__`/`__TIME__`). Supaya waktunya benar:
1. Pastikan jam laptop yang dipakai `pio run --target upload` sudah di zona **WIB**, bukan UTC/zona lain.
2. Setelah upload pertama kali, cek Serial Monitor - harus muncul log `RTC siap. Unix timestamp sekarang: ...` dengan angka yang masuk akal (bukan tahun lama/epoch).
3. Selama baterai coin cell CR2032 di modul RTC masih hidup, jam ini **tetap tersimpan** walau ESP32 mati/restart - tidak perlu di-set ulang tiap boot.

Kalau RTC gagal (log `RTC DS3231 tidak siap`) dan STA sudah dikonfigurasi ke WiFi yang ada internetnya, firmware otomatis coba fallback NTP saat boot (dan dicoba ulang tiap 30 detik oleh `SyncTimeTask` selama belum berhasil sinkron; setelah berhasil, sinkronisasi ulang berikutnya baru tiap 6 jam).

> Implementasi: `RtcTimeProvider` (`src/infrastructure/time/rtc_time_provider.h/.cpp`), `NetworkTimeProvider` (`src/infrastructure/time/network_time_provider.h/.cpp`), digabung lewat `HybridTimeProvider` (`src/infrastructure/time/hybrid_time_provider.h/.cpp`).

### 3. Kalibrasi Soil Moisture — `src/infrastructure/hardware/sensor_manager.h`

```cpp
static constexpr int SOIL_ADC_DRY = 3000; // nilai ADC saat sensor benar-benar kering (di udara)
static constexpr int SOIL_ADC_WET = 1200; // nilai ADC saat sensor benar-benar basah (dicelup air)
```

Cara ukur: upload firmware dulu dengan nilai default di atas, buka Serial Monitor, catat nilai `soilMoisture` mentah pas sensor kering & basah, lalu update dua konstanta ini dan upload ulang.

### 4. Debit Pompa (estimasi volume) — `src/infrastructure/config/pump_config.h`

Device **tidak punya flow sensor** (sensor air murah butuh pipa >= 1/2 inch & flow minimum 1-2 L/menit, jauh di atas selang 4mm x 6mm & debit ~200 mL/menit pompa ini - lihat detail di [`docs/storage.md`](docs/storage.md#estimasi-volume-irigasi-irrigationmillilitre)). Volume yang tercatat di log (`irrigationMillilitre`) adalah **estimasi** dari durasi nyala pompa dikali konstanta debit tetap:

```cpp
constexpr float FLOW_RATE_ML_PER_MINUTE = 200.0f;
```

Kalau ganti pompa dengan spesifikasi debit berbeda, update angka ini dan upload ulang - tidak ada kalibrasi runtime, murni konstanta di kode. Mobile app bisa baca angka yang sedang aktif lewat `GET /api/pump/info` (`flowRateMlPerMinute`) tanpa perlu hardcode di app-nya - tapi field ini read-only, tidak ada cara ubah dari API.

### 5. Bahasa (Indonesia/Inggris)

String yang dilihat end-user - `decision.reason` di `GET /api/status` dan label/status di layar TFT (`Suhu`/`Temp`, `Menyiram...`/`Watering...`, dst.) - bisa dipilih bahasanya lewat file **`/settings/language.json`** di SD Card:

```json
{"language": "id"}
```

Ganti `"id"` jadi `"en"` buat bahasa Inggris. Default `"id"` kalau file belum ada/isinya tidak dikenali. Cara ganti manual sama seperti WiFi (poin 1) - cabut SD Card, edit file, pasang lagi, restart device.

Bisa juga diganti **tanpa cabut SD Card sama sekali** lewat `GET`/`PUT /api/settings/language` (lihat [REST API](#rest-api)) - perubahan lewat API langsung terasa seketika (TFT & response API berikutnya), tidak perlu restart device.

> Pesan error HTTP (mis. `"JSON tidak valid"`, `"Endpoint tidak ditemukan"`) dan log Serial **tidak** ikut lokalisasi ini - sengaja tetap bahasa Indonesia, sifatnya debug/development bukan yang dilihat end-user awam. Implementasi: `Translator` (`src/infrastructure/i18n/translator.h/.cpp`), `SdLanguageRepository` (`src/infrastructure/repository/language_repository.h/.cpp`).

---

## Build & Upload

Via VSCode PlatformIO extension:
1. Buka folder project ini di VSCode.
2. Klik icon PlatformIO (alien head) di sidebar → **esp32dev → General → Build**.
3. Colok ESP32 via USB → **esp32dev → General → Upload**.
4. **esp32dev → General → Monitor** untuk lihat Serial log (baud rate `115200`).

Via CLI:
```bash
pio run                  # build
pio run --target upload  # upload
pio device monitor        # serial monitor
```

Setelah boot, Access Point langsung aktif (instan, tidak perlu tunggu konek ke router manapun) - kecuali `apEnabled: false` di `/settings/wifi.json`, lihat [Konfigurasi WiFi](#1-wifi-access-point--station-opsional--via-sd-card-tidak-perlu-re-flash). Kalau STA dikonfigurasi & `staEnabled: true`, device juga coba join WiFi itu di background (tidak blocking, tidak mempengaruhi kapan AP siap dipakai). Cari IP address ESP32 dari:
- Layar TFT (`"Menunggu HP connect..."` sebelum ada yang connect, lalu `"Siap - <IP>"` setelah HP connect ke SSID-nya - defaultnya `192.168.4.1`)
- Serial Monitor (log `Access Point aktif - SSID: ..., IP: ...` saat boot)

### Tampilan Layar TFT

Modul fisik yang dipakai: GC9A01 bundar, diameter **3.2 cm** (240x240 px). Layar (`DisplayManager::showStatus`, `src/infrastructure/hardware/display_manager.cpp`) update tiap 1 detik (atau langsung begitu pompa nyala/mati), isinya:

```
      14:23:45 WIB        <- kecil (textSize 1)
     AP  192.168.4.1        <- IP Access Point, "-" kalau apEnabled: false
     STA 192.168.1.42       <- IP Station, "-" kalau STA tidak dikonfigurasi/
                                dimatikan, "menyambung..." kalau dikonfigurasi
                                tapi belum konek
    Suhu   : 28.6 C
    Lembap : 62.8 %
    Cahaya : 300 lux
    Tanah  : 45 %
    Pompa  : ON/OFF
<status: "Menyiram..." / "ERROR: Waktu blm sinkron" / "Menunggu HP connect..." /
 "Menunggu WiFi Station..." / "Siap - <IP>">
```

Semua baris di-center horizontal (lebar teks diukur pakai `getTextBounds`, bukan asumsi jumlah karakter) supaya tetap pas di tengah layar bundar walau panjang teksnya beda-beda (mis. IP address). Baris status sengaja dipakai `textSize` lebih kecil dari baris lain karena isinya bisa jauh lebih panjang (pesan error) - di layar sekecil ini, teks yang kepanjangan gampang wrap/kepotong bezel kalau dipaksa `textSize` besar.

Label (`Suhu`/`Lembap`/`Cahaya`/`Tanah`/`Pompa`) dan baris status di atas contoh dalam bahasa Indonesia (default) - kalau `/settings/language.json` di-set `"en"`, semuanya jadi Inggris (`Temp`/`Humidity`/`Light`/`Soil`/`Pump`, `Watering...`/`Waiting for phone...`/dst.), lihat [Konfigurasi Bahasa](#5-bahasa-indonesiainggris). `AP`/`STA`/`ON`/`OFF` dan satuan (`C`/`%`/`lux`) tidak ikut diterjemahkan (sudah universal di kedua bahasa).

Jam diambil dari `HybridTimeProvider` (RTC atau fallback NTP), format `HH:MM:SS WIB` - label `WIB` di sini tetap (bukan konversi timezone dinamis), lihat [Konfigurasi RTC](#2-rtc-ds3231-set-waktu-awal-sumber-waktu-utama). IP Access Point (`WiFi.softAPIP()`, default `192.168.4.1`) dan IP Station (`WiFi.localIP()`, cuma valid kalau STA berhasil connect) sama-sama ditampilkan di layar, jadi tidak perlu cek dari router/network scanner buat tahu IP Station.

Prioritas baris status (dari yang paling diutamakan): **waktu tidak tersinkron** (LED **merah berkedip** - anggap kondisi error karena `createdAt` di log bakal `0` dan rotasi/retensi log ikut skip, lihat [`docs/storage.md`](docs/storage.md); tetap tampil walau pompa lagi nyala, karena ini dianggap masalah sistem yang lebih penting) > pompa nyala (LED kuning menyala) > belum bisa diakses (LED kuning berkedip - "Menunggu HP connect..." kalau AP aktif tapi belum ada HP yang connect, atau "Menunggu WiFi Station..." kalau AP dimatikan dan STA belum berhasil connect) > siap (LED hijau, "Siap - `<IP AP>`" atau "Siap - `<IP STA>`" tergantung mana yang aktif jadi jalur akses).

---

## REST API

Base URL: `http://<IP_ESP32>` (port default `80`, bisa diubah di `wifi_config.h` → `WifiConfig::HTTP_PORT`).

CORS sudah diaktifkan (`Access-Control-Allow-Origin: *`), jadi bisa langsung diakses dari app Flutter maupun Postman.

| Method | Endpoint | Body (JSON) | Keterangan |
|---|---|---|---|
| GET | `/api/ping` | — | Cek konektivitas ringan (`{"pong": true}`), tidak nyentuh sensor/SD Card sama sekali - aman di-poll sesering apapun, beda dari `/api/status` yang tiap dipanggil baca sensor + setting beneran |
| GET | `/api/status` | — | Kondisi terkini: sensor, trigger, restriction, hasil keputusan, status pompa, status sinkronisasi waktu (`timeSynchronized`) |
| GET | `/api/logs/sensor?lastId=0&limit=50` | — | Riwayat log sensor (paginasi pakai `lastId`) |
| GET | `/api/logs/irrigation?lastId=0&limit=50` | — | Riwayat siklus penyiraman |
| DELETE | `/api/logs` | — | Hapus semua log sensor & irigasi |
| GET | `/api/sync` | — | Metadata sinkronisasi + jumlah log |
| GET | `/api/settings/trigger` | — | Ambil setting trigger (kapan pompa nyala) |
| PUT | `/api/settings/trigger` | `{"soilMoistureOperator": "<", "soilMoistureValue": 30}` | Update setting trigger |
| GET | `/api/settings/restriction` | — | Ambil setting restriction (kondisi yang mencegah pompa nyala) |
| PUT | `/api/settings/restriction` | lihat contoh di bawah | Update setting restriction |
| GET | `/api/settings/language` | — | Ambil bahasa aktif (`{"language": "id"}` atau `"en"`) |
| PUT | `/api/settings/language` | `{"language": "en"}` | Ganti bahasa - langsung terasa seketika (TFT & API berikutnya), tidak perlu restart |
| GET | `/api/pump/info` | — | Info statis pompa (`flowRateMlPerMinute`) - konstanta kompilasi, bukan setting yang bisa diubah lewat API |
| POST | `/api/pump/start` | — | Nyalakan pompa manual (terus sampai di-stop) |
| POST | `/api/pump/stop` | — | Matikan pompa manual |
| POST | `/api/pump/test` | `{"durationSecond": 3}` | Test pompa X detik lalu auto-mati (maks 10 detik, **blocking**) |
| POST | `/api/maintenance/factory-reset` | — | Reset semua setting ke default + hapus semua log |

`timeSynchronized` di `GET /api/status` (`false` kalau RTC gagal & fallback NTP juga belum berhasil, lihat [Konfigurasi RTC](#2-rtc-ds3231-set-waktu-awal-sumber-waktu-utama)) - sama seperti LED merah berkedip & pesan error di TFT, mobile app bisa poll field ini buat nampilin warning ke user (mis. "timestamp log belum akurat") tanpa perlu lihat device fisiknya langsung.

`decision.reason` di `GET /api/status` mengikuti bahasa yang di-set di `/settings/language.json` (lihat [Konfigurasi Bahasa](#5-bahasa-indonesiainggris)) - contoh: `"Pemicu belum terpenuhi"` (id) / `"Trigger not met"` (en). Field lain di response (`sensor`, `trigger`, `restriction`, dst.) tidak ikut terpengaruh bahasa, cuma `decision.reason` yang berupa kalimat manusia.

Contoh body `PUT /api/settings/restriction`:
```json
{
  "airHumidityEnabled": true,
  "airHumidityOperator": "<",
  "airHumidityValue": 40,
  "airTemperatureEnabled": false,
  "airTemperatureOperator": ">",
  "airTemperatureValue": 35,
  "lightIntensityEnabled": false,
  "lightIntensityOperator": "<",
  "lightIntensityValue": 100,
  "maxPumpRuntimeSecond": 15
}
```

`maxPumpRuntimeSecond` — batas maksimal pompa nyala terus-menerus tanpa henti (detik) sebelum dipaksa mati otomatis (safety net kalau soil sensor rusak/putus). Default **15**, lihat [Known Issues](#known-issues--assumptions).

---

## Testing via Postman

1. Connect laptop/HP yang jalanin Postman ke WiFi Access Point yang di-broadcast ESP32 (SSID default `ESP32-Irrigation`, lihat [Konfigurasi](#konfigurasi-sebelum-flash)).
2. Untuk request `PUT`/`POST` yang ada body: tab **Body → raw → JSON**, tambahkan header `Content-Type: application/json`.
3. Mulai dari `GET /api/status` buat pastiin koneksi & sensor OK sebelum coba endpoint lain.

---

## Known Issues & Assumptions

- **GFX Library versi terbaru (1.6.x) gagal compile** dengan `platform = espressif32` resmi PlatformIO — butuh header `esp32-hal-periman.h` yang cuma ada di arduino-esp32 core 3.x, sementara PlatformIO resmi masih core 2.x. Makanya versi library di-pin ke `1.4.9`. Alternatif jangka panjang: pindah ke platform [pioarduino](https://github.com/pioarduino/platform-espressif32) kalau butuh core 3.x.
- **Pin tombol manual (GPIO 0)** adalah asumsi, pakai tombol BOOT bawaan DevKit. Sesuaikan `PinConfig::BUTTON` kalau pakai tombol eksternal.
- **AP & STA masing-masing punya toggle on/off** (`apEnabled`/`staEnabled` di `/settings/wifi.json` - default `apEnabled: true`, `staEnabled: false`) - keduanya dibaca dari SD Card saat boot, dengan fallback AP-only ke `wifi_config.h` kalau file itu kosong/SD gagal dibaca. Belum ada endpoint API buat ganti kredensial/toggle langsung dari app (harus edit file di SD Card manual), tapi struktur repository-nya (`SdWifiCredentialRepository`) sudah siap kalau mau ditambah nanti. Kalau `apEnabled` dan `staEnabled` dua-duanya `false` (device jadi sama sekali tidak bisa diakses), `WifiManager::begin()` otomatis maksa AP tetap nyala sebagai fail-safe (lihat `src/infrastructure/network/wifi_manager.cpp`).
- AP bisa berkedip/susah terlihat saat scan HP kalau STA aktif tapi SSID-nya tidak ketemu — ESP32 cuma punya 1 radio buat AP+STA, jadi tiap retry `WiFi.begin()` beacon AP ikut terganggu. Interval retry sudah diperlambat ke 30 detik, tapi kalau STA memang tidak akan pernah konek lebih baik set `staEnabled: false` di `wifi.json`.
- **Sumber waktu utama RTC DS3231, NTP cuma fallback** (lihat [Konfigurasi](#konfigurasi-sebelum-flash)) - dipakai kalau RTC gagal & STA berhasil connect internet. Kalau baterai coin cell RTC habis DAN STA tidak dikonfigurasi/gagal connect, jam bisa reset/drift dan **tidak ada mekanisme otomatis** yang memperbaikinya sepenuhnya - device perlu di-flash ulang (fallback `lostPower()` pakai jam laptop) atau dikonfigurasi STA-nya supaya fallback NTP bisa jalan. Kondisi ini setidaknya **kelihatan** (bukan diam-diam) - LED merah berkedip + teks error di layar TFT selama waktu belum tersinkron (lihat [Tampilan Layar TFT](#tampilan-layar-tft)).
- **`POST /api/pump/test` bersifat blocking** — request akan "menggantung" selama durasi test (maks 10 detik) karena firmware pakai `delay()`, bukan task async. Cukup untuk keperluan testing manual dari app, tapi jangan panggil endpoint lain secara bersamaan selagi test jalan.
- **Safety timeout pompa**: pompa otomatis dipaksa mati kalau menyala terus tanpa henti melebihi `maxPumpRuntimeSecond` (default **15 detik**, mencegah pompa nyala terus kalau soil sensor rusak/putus). Bisa diatur lewat `GET`/`PUT /api/settings/restriction` (field `maxPumpRuntimeSecond`), tidak perlu re-flash.
- `getLastId()` dan `count()` di repository log (`SdSensorLogRepository`/`SdIrrigationLogRepository`) di-cache di RAM, bukan baca ulang seluruh file NDJSON tiap dipanggil (dulunya `count()` scan penuh tiap `GET /api/sync`, bisa berat kalau log sudah ratusan ribu baris - request bisa sampai beberapa detik walau via AP atau STA sekalipun, karena bottleneck-nya di baca SD Card, bukan WiFi). Cache count divalidasi dengan membandingkan daftar file saat ini vs snapshot terakhir, jadi otomatis re-scan (sekali) kalau `LogRetentionTask` menghapus file lama atau ada rotasi ke file bulan baru.
- **Layout layar TFT bundar** (`DisplayManager::showStatus`) - posisi tiap baris (termasuk jam kecil di pojok atas) diperkirakan lewat perhitungan geometri lingkaran (radius vs lebar teks hasil `getTextBounds`), **bukan diukur langsung di hardware asli**. Baris status pernah kepotong bezel di percobaan sebelumnya (sudah dikoreksi); kalau masih/kembali kepotong setelah di-flash, sesuaikan lagi posisi `y`/`lineHeight` di file itu.
- **`irrigationMillilitre` di log irigasi cuma estimasi** (durasi x debit tetap `PumpConfig::FLOW_RATE_ML_PER_MINUTE`), bukan hasil pengukuran flow sensor — device memang tidak punya flow sensor (lihat [Konfigurasi](#4-debit-pompa-estimasi-volume-srcinfrastructureconfigpump_configh)). `irrigationMillilitre` di `SensorLog` juga tidak pernah diisi (selalu `0`).
- **Lokalisasi (`/settings/language.json`, `GET`/`PUT /api/settings/language`) cuma nyakup `decision.reason` (API) & label/status layar TFT** (lihat [Konfigurasi Bahasa](#5-bahasa-indonesiainggris)) — pesan error HTTP, log Serial, dan boot message di TFT (mis. `"Menyalakan Access Point..."`) sengaja tidak ikut, tetap bahasa Indonesia.

---

## Next Improvement

- Endpoint API buat update WiFi credential langsung dari app (tanpa cabut SD Card) — `SdWifiCredentialRepository::save()` sudah siap, tinggal buat handler

---

## Catatan Lain

- **Mobile app Flutter** yang consume API di atas sudah dikembangkan, tapi di repo terpisah (`mobile/airri-mobile`), tidak masuk cakupan repo ini.
- **Tidak ada rencana sinkronisasi log ke cloud/backend.** Device didesain berdiri sendiri di jaringan lokal (AP/STA + SD Card sebagai penyimpanan utama), bukan menuju arsitektur cloud-connected.
