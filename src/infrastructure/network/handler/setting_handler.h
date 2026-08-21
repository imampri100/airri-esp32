#ifndef IRRIGATION_SETTING_HANDLER_H
#define IRRIGATION_SETTING_HANDLER_H

#include "application/usecase/update_language_usecase.h"
#include "application/usecase/update_restriction_usecase.h"
#include "application/usecase/update_trigger_usecase.h"

#include "domain/repository/language_repository.h"
#include "domain/repository/setting_repository.h"

#include "infrastructure/i18n/translator.h"
#include "infrastructure/network/http_server.h"

namespace irrigation
{

    // GET/PUT /api/settings/trigger, GET/PUT /api/settings/restriction,
    // GET/PUT /api/settings/language
    class SettingHandler
    {
    public:
        SettingHandler(
            HttpServer &httpServer,
            SettingRepository &settingRepository,
            UpdateTriggerUseCase &updateTriggerUseCase,
            UpdateRestrictionUseCase &updateRestrictionUseCase,
            LanguageRepository &languageRepository,
            UpdateLanguageUseCase &updateLanguageUseCase,
            Translator &translator);

        void handleGetTrigger();

        void handlePutTrigger();

        void handleGetRestriction();

        void handlePutRestriction();

        void handleGetLanguage();

        // Selain persist ke SD Card, langsung update Translator yang
        // aktif juga - perubahan bahasa kelihatan seketika (TFT & API
        // berikutnya), tidak perlu restart device.
        void handlePutLanguage();

    private:
        HttpServer &httpServer;

        SettingRepository &settingRepository;

        UpdateTriggerUseCase &updateTriggerUseCase;

        UpdateRestrictionUseCase &updateRestrictionUseCase;

        LanguageRepository &languageRepository;

        UpdateLanguageUseCase &updateLanguageUseCase;

        Translator &translator;
    };

}

#endif
