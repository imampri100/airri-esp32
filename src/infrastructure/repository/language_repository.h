#ifndef IRRIGATION_INFRA_LANGUAGE_REPOSITORY_H
#define IRRIGATION_INFRA_LANGUAGE_REPOSITORY_H

#include "domain/repository/language_repository.h"

#include "infrastructure/persistence/json_serializer.h"
#include "infrastructure/persistence/persistence_storage_manager.h"

namespace irrigation
{

    // Implementasi LanguageRepository (domain) -> file JSON tunggal di SD
    // Card (/settings/language.json, {"language": "id"} atau {"language":
    // "en"}) - bisa diganti tanpa re-flash, sama seperti wifi.json.
    class SdLanguageRepository : public LanguageRepository
    {
    public:
        SdLanguageRepository(
            PersistenceStorageManager &storage,
            JsonSerializer &serializer,
            const String &path);

        // Default Language::Indonesian kalau file kosong/belum ada/gagal
        // dibaca, atau isinya bukan "id"/"en".
        Language load() override;

        void save(Language language) override;

    private:
        PersistenceStorageManager &storage;

        JsonSerializer &serializer;

        String path;
    };

}

#endif
