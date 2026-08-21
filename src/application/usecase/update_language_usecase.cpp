#include "application/usecase/update_language_usecase.h"

namespace irrigation
{

    UpdateLanguageUseCase::UpdateLanguageUseCase(
        LanguageRepository &languageRepository)
        : languageRepository(languageRepository)
    {
    }

    void UpdateLanguageUseCase::execute(Language language)
    {
        languageRepository.save(language);
    }

}
