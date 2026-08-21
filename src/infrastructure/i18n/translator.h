#ifndef IRRIGATION_TRANSLATOR_H
#define IRRIGATION_TRANSLATOR_H

#include <Arduino.h>

#include "domain/entity/language.h"
#include "domain/entity/reason_code.h"

namespace irrigation
{

    // Terjemahan string yang dilihat end-user: API decision.reason & layar
    // TFT. Hanya Indonesia/Inggris. Pesan error HTTP & log Serial sengaja
    // TIDAK ikut (tetap bahasa Indonesia, sifatnya debug/development).
    class Translator
    {
    public:
        explicit Translator(Language language = Language::Indonesian);

        void setLanguage(Language language);

        Language language() const;

        // API: GET /api/status -> decision.reason
        String reasonText(ReasonCode code) const;

        // TFT - label kolom sensor
        String labelAirTemperature() const;

        String labelAirHumidity() const;

        String labelLightIntensity() const;

        String labelSoilMoisture() const;

        String labelPump() const;

        // TFT - baris status paling bawah (tanpa embel-embel IP/durasi,
        // itu digabung oleh caller)
        String statusWatering() const;

        String statusWaitingForPhone() const;

        String statusWaitingForStation() const;

        String statusTimeNotSynced() const;

        String statusReady() const;

    private:
        Language currentLanguage;
    };

}

#endif
