#include "infrastructure/repository/language_repository.h"

#include <ArduinoJson.h>

namespace irrigation
{

    namespace
    {
        constexpr char CODE_INDONESIAN[] = "id";
        constexpr char CODE_ENGLISH[] = "en";
    }

    SdLanguageRepository::SdLanguageRepository(
        PersistenceStorageManager &storage,
        JsonSerializer &serializer,
        const String &path)
        : storage(storage),
          serializer(serializer),
          path(path)
    {
    }

    Language SdLanguageRepository::load()
    {
        File file = storage.openRead(path);
        if (!file)
        {
            return Language::Indonesian;
        }

        StaticJsonDocument<64> document;
        DeserializationError error = deserializeJson(document, file);
        file.close();

        if (error)
        {
            return Language::Indonesian;
        }

        String code = document["language"] | CODE_INDONESIAN;

        return code == CODE_ENGLISH ? Language::English : Language::Indonesian;
    }

    void SdLanguageRepository::save(Language language)
    {
        StaticJsonDocument<64> document;

        document["language"] =
            language == Language::English ? CODE_ENGLISH : CODE_INDONESIAN;

        storage.remove(path);
        File file = storage.openWrite(path);
        if (!file)
        {
            return;
        }

        serializer.serialize(document, file);
        file.close();
    }

}
