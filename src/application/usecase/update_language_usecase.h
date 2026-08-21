#ifndef IRRIGATION_UPDATE_LANGUAGE_USECASE_H
#define IRRIGATION_UPDATE_LANGUAGE_USECASE_H

#include "domain/entity/language.h"
#include "domain/repository/language_repository.h"

namespace irrigation
{
    class UpdateLanguageUseCase
    {
    public:
        explicit UpdateLanguageUseCase(
            LanguageRepository &languageRepository);

        void execute(Language language);

    private:
        LanguageRepository &languageRepository;
    };
}

#endif
