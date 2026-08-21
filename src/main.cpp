#include <Arduino.h>
#include <time.h>

#include "domain/service/decision_engine.h"

#include "application/automation/log_retention_task.h"
#include "application/automation/maintenance_task.h"
#include "application/automation/monitor_environment_task.h"
#include "application/automation/scheduler.h"
#include "application/automation/sync_time_task.h"

#include "application/usecase/clear_logs_usecase.h"
#include "application/usecase/factory_reset_usecase.h"
#include "application/usecase/get_irrigation_logs_usecase.h"
#include "application/usecase/get_sensor_logs_usecase.h"
#include "application/usecase/get_status_usecase.h"
#include "application/usecase/get_sync_usecase.h"
#include "application/usecase/pump_test_usecase.h"
#include "application/usecase/update_language_usecase.h"
#include "application/usecase/update_restriction_usecase.h"
#include "application/usecase/update_trigger_usecase.h"

#include "infrastructure/config/file_config.h"
#include "infrastructure/config/wifi_config.h"

#include "infrastructure/hardware/button_manager.h"
#include "infrastructure/hardware/display_manager.h"
#include "infrastructure/hardware/led_manager.h"
#include "infrastructure/hardware/pump_manager.h"
#include "infrastructure/hardware/sensor_manager.h"

#include "infrastructure/i18n/translator.h"

#include "infrastructure/network/http_server.h"
#include "infrastructure/network/router.h"
#include "infrastructure/network/wifi_credential.h"
#include "infrastructure/network/wifi_credential_repository.h"
#include "infrastructure/network/wifi_manager.h"

#include "infrastructure/network/handler/irrigation_log_handler.h"
#include "infrastructure/network/handler/maintenance_handler.h"
#include "infrastructure/network/handler/pump_handler.h"
#include "infrastructure/network/handler/sensor_log_handler.h"
#include "infrastructure/network/handler/setting_handler.h"
#include "infrastructure/network/handler/status_handler.h"
#include "infrastructure/network/handler/sync_handler.h"

#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/ndjson_reader.h"
#include "infrastructure/persistence/ndjson_writer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

#include "infrastructure/repository/irrigation_log_repository.h"
#include "infrastructure/repository/language_repository.h"
#include "infrastructure/repository/metadata_repository.h"
#include "infrastructure/repository/sensor_log_repository.h"
#include "infrastructure/repository/setting_repository.h"

#include "infrastructure/time/hybrid_time_provider.h"
#include "infrastructure/time/network_time_provider.h"
#include "infrastructure/time/rtc_time_provider.h"

#include "presentation/serial/serial_logger.h"

using namespace irrigation;

namespace
{
    // ---------- i18n ----------
    // Default Language::Indonesian, di-update dari SD Card (language.json)
    // pas setup() - butuh dideklarasikan di atas DisplayManager karena
    // di-inject via reference (urutan inisialisasi global C++ mengikuti
    // urutan deklarasi).
    Translator translator;

    // ---------- Hardware ----------
    SensorManager sensorManager;
    PumpManager pumpManager;
    LedManager ledManager;
    ButtonManager buttonManager;
    DisplayManager displayManager(translator);

    // ---------- Domain service ----------
    DecisionEngine decisionEngine;

    // ---------- Persistence ----------
    PersistenceStorageManager storageManager;
    JsonSerializer jsonSerializer;
    NdjsonWriter ndjsonWriter(storageManager, jsonSerializer);
    NdjsonReader ndjsonReader(storageManager);

    // ---------- Repositories (SD Card) ----------
    SdSensorLogRepository sensorLogRepository(
        storageManager, ndjsonWriter, ndjsonReader, FileConfig::SENSOR_LOG_DIRECTORY);

    SdIrrigationLogRepository irrigationLogRepository(
        storageManager, ndjsonWriter, ndjsonReader, FileConfig::IRRIGATION_LOG_DIRECTORY);

    SdSettingRepository settingRepository(
        storageManager, jsonSerializer,
        FileConfig::TRIGGER_SETTING, FileConfig::RESTRICTION_SETTING);

    SdMetadataRepository metadataRepository(
        storageManager, jsonSerializer, FileConfig::SYNC_METADATA);

    SdWifiCredentialRepository wifiCredentialRepository(
        storageManager, jsonSerializer, FileConfig::WIFI_CREDENTIAL);

    SdLanguageRepository languageRepository(
        storageManager, jsonSerializer, FileConfig::LANGUAGE_SETTING);

    // ---------- Network ----------
    WifiManager wifiManager;
    RtcTimeProvider rtcTimeProvider;
    NetworkTimeProvider networkTimeProvider;
    HybridTimeProvider timeProvider(rtcTimeProvider, networkTimeProvider);
    HttpServer httpServer(WifiConfig::HTTP_PORT);

    // ---------- Use cases ----------
    GetStatusUseCase getStatusUseCase(
        sensorManager, settingRepository, pumpManager, timeProvider, decisionEngine);

    GetSensorLogsUseCase getSensorLogsUseCase(sensorLogRepository);
    GetIrrigationLogsUseCase getIrrigationLogsUseCase(irrigationLogRepository);

    UpdateTriggerUseCase updateTriggerUseCase(settingRepository);
    UpdateRestrictionUseCase updateRestrictionUseCase(settingRepository);
    UpdateLanguageUseCase updateLanguageUseCase(languageRepository);

    ClearLogsUseCase clearLogsUseCase(sensorLogRepository, irrigationLogRepository);

    GetSyncUseCase getSyncUseCase(
        metadataRepository, sensorLogRepository, irrigationLogRepository);

    PumpTestUseCase pumpTestUseCase(pumpManager);

    FactoryResetUseCase factoryResetUseCase(
        settingRepository, metadataRepository, sensorLogRepository, irrigationLogRepository);

    // ---------- HTTP handlers ----------
    StatusHandler statusHandler(httpServer, getStatusUseCase, translator);
    SensorLogHandler sensorLogHandler(httpServer, getSensorLogsUseCase);
    IrrigationLogHandler irrigationLogHandler(httpServer, getIrrigationLogsUseCase);
    SyncHandler syncHandler(httpServer, getSyncUseCase);
    SettingHandler settingHandler(
        httpServer, settingRepository, updateTriggerUseCase, updateRestrictionUseCase,
        languageRepository, updateLanguageUseCase, translator);
    PumpHandler pumpHandler(httpServer, pumpManager, pumpTestUseCase);
    MaintenanceHandler maintenanceHandler(httpServer, clearLogsUseCase, factoryResetUseCase);

    Router router(
        httpServer,
        statusHandler,
        sensorLogHandler,
        irrigationLogHandler,
        syncHandler,
        settingHandler,
        pumpHandler,
        maintenanceHandler);

    // ---------- Automation ----------
    Scheduler scheduler;

    MonitorEnvironmentTask monitorTask(
        sensorManager,
        settingRepository,
        pumpManager,
        timeProvider,
        sensorLogRepository,
        irrigationLogRepository,
        decisionEngine);

    SyncTimeTask syncTimeTask(timeProvider);
    MaintenanceTask maintenanceTask;

    LogRetentionTask logRetentionTask(
        storageManager,
        timeProvider,
        FileConfig::SENSOR_LOG_DIRECTORY,
        FileConfig::IRRIGATION_LOG_DIRECTORY);

    // ---------- Local UI refresh state ----------
    unsigned long lastUiRefreshAt = 0;
    constexpr unsigned long UI_REFRESH_INTERVAL_MS = 1000;

    bool lastButtonPressed = false;

    bool wasStaConnected = false;

    // Dipakai supaya perubahan status pompa (nyala/mati) langsung
    // ke-refresh ke LED/TFT saat itu juga, tidak nunggu tick 1 detik.
    bool lastPumpRunning = false;
    bool pumpStateInitialized = false;

    // RTC menyimpan waktu lokal (WIB) langsung sebagai "unix seconds" -
    // tidak ada offset tambahan yang perlu diterapkan di sini (beda dari
    // NTP dulu yang baku +7 jam via configTime). "WIB" di bawah cuma
    // label tetap, bukan hasil konversi timezone dinamis.
    String formatClock(const Timestamp &timestamp)
    {
        if (!timestamp.isValid())
        {
            return "--:--:--";
        }

        time_t rawTime = static_cast<time_t>(timestamp.value());
        struct tm parts;
        gmtime_r(&rawTime, &parts);

        char buffer[16];
        snprintf(
            buffer, sizeof(buffer),
            "%02d:%02d:%02d WIB",
            parts.tm_hour, parts.tm_min, parts.tm_sec);

        return String(buffer);
    }

    // STA cuma dipakai buat fallback NTP (lihat HybridTimeProvider) - log
    // perubahan status di sini murni buat diagnostik lewat Serial
    // Monitor, tidak ada logic lain yang bergantung ke ini. No-op total
    // kalau STA tidak dikonfigurasi.
    void logStaStatusChange()
    {
        if (!wifiManager.hasStaConfigured())
        {
            return;
        }

        bool nowConnected = wifiManager.isStaConnected();

        if (nowConnected && !wasStaConnected)
        {
            SerialLogger::info(
                "WiFi Station terhubung - fallback NTP tersedia kalau RTC "
                "bermasalah.");
        }
        else if (!nowConnected && wasStaConnected)
        {
            SerialLogger::warn("WiFi Station terputus, mencoba menyambung ulang...");
        }

        wasStaConnected = nowConnected;
    }

    void refreshLocalUi()
    {
        SensorReading reading = sensorManager.read();
        bool pumpRunning = pumpManager.isRunning();

        String statusText;

        if (!timeProvider.isSynchronized())
        {
            // Tanpa waktu valid (RTC gagal & fallback NTP juga belum
            // berhasil), createdAt di log bakal 0 dan rotasi/retensi log
            // (LogRetentionTask) skip total - ini kondisi error yang
            // harus tetap kelihatan walau pompa lagi jalan, makanya
            // dicek PALING duluan (prioritas di atas status pompa).
            ledManager.setState(LedState::Error);
            // Dipersingkat (bukan "ERROR: Waktu tidak tersinkron (RTC/NTP)")
            // supaya muat di layar bundar 3.2 cm tanpa wrap/kepotong -
            // detail lengkap tetap ada di Serial log & field
            // timeSynchronized lewat GET /api/status.
            statusText = translator.statusTimeNotSynced();
        }
        else if (pumpRunning)
        {
            ledManager.setState(LedState::Watering);
            statusText = translator.statusWatering();
        }
        else
        {
            // "Cara device diakses" beda tergantung AP dimatikan atau
            // tidak - kalau AP aktif, itu jalur utama (HP connect ke
            // SSID-nya); kalau AP dimatikan, satu-satunya jalur ya STA,
            // jadi "siap" berarti STA sudah connect, bukan ada HP di AP.
            bool waitingForAccess =
                wifiManager.isApEnabled()
                    ? wifiManager.stationCount() == 0
                    : !wifiManager.isStaConnected();

            if (waitingForAccess)
            {
                ledManager.setState(LedState::Syncing);
                statusText = wifiManager.isApEnabled()
                                 ? translator.statusWaitingForPhone()
                                 : translator.statusWaitingForStation();
            }
            else
            {
                ledManager.setState(LedState::Idle);
                statusText = translator.statusReady() + " - " +
                              (wifiManager.isApEnabled()
                                   ? wifiManager.ipAddress()
                                   : wifiManager.staIpAddress());
            }
        }

        String clockText = formatClock(timeProvider.now());

        String apIpText = wifiManager.isApEnabled() ? wifiManager.ipAddress() : "-";

        String staIpText;
        if (!wifiManager.hasStaConfigured())
        {
            staIpText = "-";
        }
        else if (wifiManager.isStaConnected())
        {
            staIpText = wifiManager.staIpAddress();
        }
        else
        {
            staIpText = "menyambung...";
        }

        displayManager.showStatus(
            reading, pumpRunning, clockText,
            apIpText, staIpText, statusText);
    }

    void handleButton()
    {
        bool pressed = buttonManager.isPressed();

        if (pressed && !lastButtonPressed)
        {
            if (pumpManager.isRunning())
            {
                pumpManager.stop();
                SerialLogger::info("Pompa dimatikan manual (tombol)");
            }
            else
            {
                pumpManager.start();
                SerialLogger::info("Pompa dinyalakan manual (tombol, uji coba)");
            }
        }

        lastButtonPressed = pressed;
    }
}

void setup()
{
    SerialLogger::begin();
    SerialLogger::info("Smart Irrigation - booting...");

    bool sensorOk = sensorManager.begin();
    bool pumpOk = pumpManager.begin();
    bool ledOk = ledManager.begin();
    bool buttonOk = buttonManager.begin();
    bool displayOk = displayManager.begin();

    if (!sensorOk)
    {
        SerialLogger::error("Sebagian sensor tidak terdeteksi (AHT10/BH1750)");
    }
    if (!displayOk)
    {
        SerialLogger::error("Inisialisasi TFT gagal");
    }

    bool storageOk = storageManager.begin() && storageManager.initialize();
    if (!storageOk)
    {
        SerialLogger::error("SD Card gagal diinisialisasi - logging/setting tidak akan tersimpan");
    }

    // Bahasa buat string yang dilihat end-user (API decision.reason &
    // layar TFT) - dibaca dari SD Card, default Indonesia kalau file
    // belum ada/gagal dibaca. Bisa diganti tanpa re-flash, sama seperti
    // wifi.json - lihat SdLanguageRepository.
    translator.setLanguage(languageRepository.load());

    ledManager.setState((sensorOk && pumpOk && ledOk && buttonOk && displayOk && storageOk)
                             ? LedState::Idle
                             : LedState::Error);

    displayManager.print("Menyalakan Access Point...");

    WifiCredential loaded = wifiCredentialRepository.load();

    // apSsid/staSsid kosong di /settings/wifi.json -> pakai default dari
    // wifi_config.h (edit langsung di sana kalau tidak mau repot cabut SD
    // Card). File di SD Card, kalau diisi, selalu menang atas default ini.
    bool apFromSd = loaded.apSsid().length() > 0;
    bool staFromSd = loaded.staSsid().length() > 0;

    // apEnabled/staEnabled murni dari SD Card (default true kalau file
    // belum ada field-nya, lihat SdWifiCredentialRepository::load()) -
    // tidak ada fallback wifi_config.h buat toggle on/off ini, beda dari
    // SSID/password yang punya default dummy.
    WifiCredential credential(
        apFromSd ? loaded.apSsid() : WifiConfig::DEFAULT_AP_SSID,
        apFromSd ? loaded.apPassword() : WifiConfig::DEFAULT_AP_PASSWORD,
        staFromSd ? loaded.staSsid() : WifiConfig::DEFAULT_STA_SSID,
        staFromSd ? loaded.staPassword() : WifiConfig::DEFAULT_STA_PASSWORD,
        loaded.apEnabled(),
        loaded.staEnabled());

    SerialLogger::info(
        apFromSd
            ? "Kredensial Access Point dimuat dari SD Card (" + credential.apSsid() + ")"
            : "/settings/wifi.json belum diisi apSsid - pakai default AP dari wifi_config.h (" +
                  credential.apSsid() + ").");

    SerialLogger::info(
        staFromSd
            ? "Kredensial WiFi Station dimuat dari SD Card (" + credential.staSsid() + ")"
            : "/settings/wifi.json belum diisi staSsid - pakai default STA dari wifi_config.h (" +
                  credential.staSsid() +
                  ", dummy kalau belum diganti/mau device otomatis coba join WiFi tertentu).");

    displayManager.print(
        "WiFi STA sumber:\n" +
        String(staFromSd ? "SD Card" : "default") + "\n" +
        "SSID: " + credential.staSsid());

    // softAP() sinkron/instan - HP tinggal connect ke SSID ini kapan
    // saja, tidak perlu nunggu apapun. STA (opsional) coba konek di
    // background lewat wifiManager.tick() di loop() - tidak blocking.
    // wifiManager.isApEnabled() dipakai buat log (bukan credential.apEnabled()
    // mentah) karena begin() bisa override ke true lewat fail-safe kalau
    // AP & STA dua-duanya dimatikan di config.
    wifiManager.begin(credential);

    if (wifiManager.isApEnabled())
    {
        SerialLogger::info(
            "Access Point aktif - SSID: " + credential.apSsid() +
            ", IP: " + wifiManager.ipAddress() +
            " -- pakai ini sebagai base_url, contoh: http://" +
            wifiManager.ipAddress());
    }
    else
    {
        SerialLogger::info(
            "Access Point dimatikan (apEnabled=false) - device cuma bisa "
            "diakses lewat WiFi Station (\"" + credential.staSsid() +
            "\"), base_url baru kelihatan setelah STA berhasil connect.");
    }

    bool rtcReady = timeProvider.begin();

    if (rtcReady)
    {
        SerialLogger::info(
            "RTC siap. Unix timestamp sekarang: " +
            String((uint32_t)timeProvider.now().value()));
    }
    else
    {
        SerialLogger::warn(
            "RTC DS3231 tidak siap - mencoba fallback NTP (butuh WiFi "
            "Station terkonfigurasi & berhasil connect ke internet)...");

        bool ntpOk = timeProvider.synchronize();

        if (ntpOk)
        {
            SerialLogger::info(
                "Fallback NTP berhasil. Unix timestamp sekarang: " +
                String((uint32_t)timeProvider.now().value()));
        }
        else
        {
            SerialLogger::error(
                "RTC & fallback NTP dua-duanya gagal - createdAt di log "
                "akan bernilai 0 sampai salah satu berhasil (SyncTimeTask "
                "otomatis coba lagi tiap 30 detik sampai berhasil, lalu "
                "tiap 6 jam setelahnya, atau device di-restart). Cek "
                "wiring I2C ke RTC dan/atau kredensial WiFi Station.");
        }
    }

    router.registerRoutes();
    httpServer.begin();
    SerialLogger::info("HTTP server aktif di port " + String(WifiConfig::HTTP_PORT));

    scheduler.addTask(monitorTask);
    scheduler.addTask(syncTimeTask);
    scheduler.addTask(maintenanceTask);
    scheduler.addTask(logRetentionTask);

    SerialLogger::info("Setup selesai");
}

void loop()
{
    wifiManager.tick();
    logStaStatusChange();
    httpServer.handleClient();
    scheduler.tick();

    unsigned long now = millis();

    bool pumpRunningNow = pumpManager.isRunning();
    bool pumpStateChanged =
        !pumpStateInitialized || pumpRunningNow != lastPumpRunning;

    if (pumpStateChanged || now - lastUiRefreshAt >= UI_REFRESH_INTERVAL_MS)
    {
        lastUiRefreshAt = now;
        lastPumpRunning = pumpRunningNow;
        pumpStateInitialized = true;
        refreshLocalUi();
    }

    handleButton();
    ledManager.tick();
}
