#ifndef IRRIGATION_REASON_CODE_H
#define IRRIGATION_REASON_CODE_H

namespace irrigation
{

    // Alasan keputusan DecisionEngine, sebagai kode (bukan String literal)
    // supaya domain tidak perlu tahu bahasa tampilan - terjemahan ke teks
    // yang dilihat end-user (API decision.reason & TFT) ada di
    // infrastructure/i18n/Translator, bukan di sini.
    enum class ReasonCode
    {
        TriggerNotMet,
        PumpCanRun,
        RestrictedByHumidity,
        RestrictedByTemperature,
        RestrictedByLight
    };

    // Label kode non-lokalisasi (selalu Inggris teknis), dipakai log Serial
    // & field decision.reasonCode di GET /api/status (mobile app
    // menerjemahkannya sendiri - jangan diubah tanpa menyesuaikan app).
    // Sengaja terpisah dari Translator supaya application layer (mis.
    // MonitorEnvironmentTask) tidak perlu depend ke infrastructure/i18n.
    inline const char *reasonCodeLabel(ReasonCode code)
    {
        switch (code)
        {
        case ReasonCode::TriggerNotMet:
            return "TriggerNotMet";
        case ReasonCode::PumpCanRun:
            return "PumpCanRun";
        case ReasonCode::RestrictedByHumidity:
            return "RestrictedByHumidity";
        case ReasonCode::RestrictedByTemperature:
            return "RestrictedByTemperature";
        case ReasonCode::RestrictedByLight:
            return "RestrictedByLight";
        }

        return "Unknown";
    }

}

#endif
