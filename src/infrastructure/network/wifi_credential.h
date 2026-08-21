#ifndef IRRIGATION_WIFI_CREDENTIAL_H
#define IRRIGATION_WIFI_CREDENTIAL_H

#include <Arduino.h>

namespace irrigation
{

    // Dua pasang kredensial berbeda, masing-masing punya toggle on/off
    // sendiri (apEnabled/staEnabled) - independen dari isi SSID-nya:
    // - AP (apSsid/apPassword): SSID/password yang di-broadcast ESP32
    //   sendiri - HP connect langsung ke sini. Default apEnabled=true
    //   (perilaku lama - AP selalu aktif), tapi bisa dimatikan lewat
    //   apEnabled=false kalau device memang selalu dipakai lewat STA.
    // - STA (staSsid/staPassword): WiFi lain yang OPSIONAL coba di-join
    //   (mis. buat internet/NTP fallback kalau RTC gagal). Default
    //   staEnabled=false (beda dari AP) - out-of-the-box device tidak
    //   pernah nyoba STA sama sekali sampai user eksplisit set true,
    //   biar gak ada retry loop diam-diam yang berpotensi ganggu AP
    //   (lihat STA_RETRY_INTERVAL di WifiManager) padahal user belum
    //   tentu butuh fallback NTP-nya. staEnabled dipisah dari isi
    //   staSsid supaya kredensial bisa disimpan tanpa langsung dicoba -
    //   tinggal set staEnabled=true kapan perlu tanpa ngetik ulang
    //   SSID/password.
    class WifiCredential
    {
    public:
        WifiCredential() = default;

        WifiCredential(
            const String &apSsid,
            const String &apPassword,
            const String &staSsid = "",
            const String &staPassword = "",
            bool apEnabled = true,
            bool staEnabled = false);

        const String &apSsid() const;

        const String &apPassword() const;

        const String &staSsid() const;

        const String &staPassword() const;

        bool apEnabled() const;

        // Toggle mentah dari SD Card, terlepas dari isi staSsid - dipakai
        // buat persist balik ke file (save()) supaya nilainya tidak
        // ketuker sama hasSta() yang sudah fusi dengan status SSID.
        bool staEnabled() const;

        // true kalau STA memang mau dicoba: staEnabled=true DAN staSsid
        // terisi (staEnabled=true dengan SSID kosong tidak ada gunanya).
        bool hasSta() const;

    private:
        String m_apSsid;

        String m_apPassword;

        String m_staSsid;

        String m_staPassword;

        bool m_apEnabled = true;

        bool m_staEnabled = false;
    };

}

#endif
