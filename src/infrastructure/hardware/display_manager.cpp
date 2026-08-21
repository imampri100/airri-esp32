#include "infrastructure/hardware/display_manager.h"

#include "infrastructure/config/pin_config.h"

namespace irrigation
{

    namespace
    {
        constexpr int16_t SCREEN_SIZE = 240; // GC9A01 round, 240x240
        constexpr uint16_t COLOR_BG = BLACK;
        constexpr uint16_t COLOR_TEXT = WHITE;
        constexpr uint16_t COLOR_OK = GREEN;
        constexpr uint16_t COLOR_WARN = YELLOW;
    }

    DisplayManager::DisplayManager(Translator &translator)
        : translator(translator)
    {
    }

    DisplayManager::~DisplayManager()
    {
        delete gfx;
        delete bus;
    }

    bool DisplayManager::begin()
    {
        bus = new Arduino_ESP32SPI(
            PinConfig::TFT_DC,
            PinConfig::TFT_CS,
            PinConfig::SPI_SCK,
            PinConfig::SPI_MOSI,
            PinConfig::SPI_MISO);

        gfx = new Arduino_GC9A01(
            bus,
            PinConfig::TFT_RES,
            0 /* rotation */,
            true /* IPS */);

        if (!gfx->begin())
        {
            return false;
        }

        gfx->fillScreen(COLOR_BG);

        return true;
    }

    void DisplayManager::clear()
    {
        if (gfx == nullptr)
        {
            return;
        }

        gfx->fillScreen(COLOR_BG);
    }

    void DisplayManager::print(const String &text)
    {
        if (gfx == nullptr)
        {
            return;
        }

        clear();

        gfx->setTextColor(COLOR_TEXT);
        gfx->setTextSize(2);
        gfx->setCursor(20, SCREEN_SIZE / 2 - 10);
        gfx->println(text);
    }

    void DisplayManager::printCentered(
        const String &text,
        int16_t y,
        uint8_t textSize,
        uint16_t color)
    {
        gfx->setTextSize(textSize);
        gfx->setTextColor(color);

        // Ukur lebar teks sebenarnya dulu (bukan asumsi jumlah karakter)
        // supaya tetap center walau isinya beda panjang tiap kali render
        // (mis. IP address atau pesan status yang panjangnya berubah-ubah).
        int16_t boundsX, boundsY;
        uint16_t width, height;
        gfx->getTextBounds(text, 0, y, &boundsX, &boundsY, &width, &height);

        int16_t x = (SCREEN_SIZE - static_cast<int16_t>(width)) / 2;

        gfx->setCursor(x, y);
        gfx->println(text);
    }

    void DisplayManager::showStatus(
        const SensorReading &reading,
        bool pumpRunning,
        const String &clockText,
        const String &apIpText,
        const String &staIpText,
        const String &statusText)
    {
        if (gfx == nullptr)
        {
            return;
        }

        clear();

        // Modul fisik yang dipakai GC9A01 bundar diameter 3.2 cm (240x240
        // px) - area yang benar-benar aman dibaca (tidak kepotong bezel)
        // menyempit drastis makin dekat ke tepi atas/bawah lingkaran.
        // Makanya tiap baris di-center horizontal (printCentered, pakai
        // getTextBounds - bukan asumsi lebar per karakter) dan baris yang
        // isinya bisa panjang/berubah-ubah (status) sengaja dipakai
        // textSize kecil supaya tidak pernah melebar sampai kepotong.
        // Posisi y di bawah masih estimasi geometri lingkaran, belum
        // diverifikasi langsung di device fisik - sesuaikan lagi kalau
        // ada baris yang kepotong bezel pas di-flash.
        int16_t y = 16;

        // Jam - baris terkecil, paling dekat tepi atas (area paling sempit).
        printCentered(clockText, y, 1, COLOR_TEXT);
        y += 22;

        // IP Access Point & IP Station - agak lebih besar dari jam
        // (textSize 2) supaya gampang dibaca dari jarak biasa. Label
        // "AP "/"STA " sengaja tanpa titik dua & sesingkat mungkin supaya
        // tetap muat berdampingan dengan IP-nya di layar sekecil ini.
        printCentered("AP  " + apIpText, y, 2, COLOR_TEXT);
        y += 24;

        printCentered("STA " + staIpText, y, 2, COLOR_TEXT);
        y += 24;

        const int16_t lineHeight = 20;

        printCentered(
            translator.labelAirTemperature() + ": " + String(reading.airTemperature, 1) + " C",
            y, 2, COLOR_TEXT);
        y += lineHeight;

        printCentered(
            translator.labelAirHumidity() + ": " + String(reading.airHumidity, 1) + " %",
            y, 2, COLOR_TEXT);
        y += lineHeight;

        printCentered(
            translator.labelLightIntensity() + ": " + String(reading.lightIntensity, 0) + " lux",
            y, 2, COLOR_TEXT);
        y += lineHeight;

        printCentered(
            translator.labelSoilMoisture() + ": " + String(reading.soilMoisture, 0) + " %",
            y, 2, COLOR_TEXT);
        y += lineHeight + 6;

        printCentered(
            translator.labelPump() + ": " + (pumpRunning ? "ON" : "OFF"),
            y, 2, pumpRunning ? COLOR_OK : COLOR_TEXT);
        y += lineHeight + 4;

        // Status dipakai textSize 1 (bukan 2 seperti baris lain) karena
        // isinya bisa jauh lebih panjang (mis. pesan error) - di textSize
        // 2 beberapa pesan sudah pasti lebih lebar dari layar (wrap ke
        // baris baru otomatis dan gampang kepotong bezel di layar bundar
        // sekecil ini).
        printCentered(statusText, y, 1, COLOR_WARN);
    }

}
