#include "infrastructure/i18n/translator.h"

namespace irrigation
{

    Translator::Translator(Language language)
        : currentLanguage(language)
    {
    }

    void Translator::setLanguage(Language language)
    {
        currentLanguage = language;
    }

    Language Translator::language() const
    {
        return currentLanguage;
    }

    String Translator::reasonText(ReasonCode code) const
    {
        bool en = currentLanguage == Language::English;

        switch (code)
        {
        case ReasonCode::TriggerNotMet:
            return en ? "Trigger not met" : "Pemicu belum terpenuhi";
        case ReasonCode::PumpCanRun:
            return en ? "Pump can run" : "Pompa dapat menyala";
        case ReasonCode::RestrictedByHumidity:
            return en ? "Restricted by air humidity" : "Dibatasi oleh kelembapan udara";
        case ReasonCode::RestrictedByTemperature:
            return en ? "Restricted by air temperature" : "Dibatasi oleh suhu udara";
        case ReasonCode::RestrictedByLight:
            return en ? "Restricted by light intensity" : "Dibatasi oleh intensitas cahaya";
        }

        return "";
    }

    String Translator::labelAirTemperature() const
    {
        return currentLanguage == Language::English ? "Temp" : "Suhu";
    }

    String Translator::labelAirHumidity() const
    {
        return currentLanguage == Language::English ? "Humidity" : "Lembap";
    }

    String Translator::labelLightIntensity() const
    {
        return currentLanguage == Language::English ? "Light" : "Cahaya";
    }

    String Translator::labelSoilMoisture() const
    {
        return currentLanguage == Language::English ? "Soil" : "Tanah";
    }

    String Translator::labelPump() const
    {
        return currentLanguage == Language::English ? "Pump" : "Pompa";
    }

    String Translator::statusWatering() const
    {
        return currentLanguage == Language::English ? "Watering..." : "Menyiram...";
    }

    String Translator::statusWaitingForPhone() const
    {
        return currentLanguage == Language::English
                   ? "Waiting for phone..."
                   : "Menunggu HP connect...";
    }

    String Translator::statusWaitingForStation() const
    {
        return currentLanguage == Language::English
                   ? "Waiting for WiFi Station..."
                   : "Menunggu WiFi Station...";
    }

    String Translator::statusTimeNotSynced() const
    {
        return currentLanguage == Language::English
                   ? "ERROR: Time not synced"
                   : "ERROR: Waktu blm sinkron";
    }

    String Translator::statusReady() const
    {
        return currentLanguage == Language::English ? "Ready" : "Siap";
    }

}
