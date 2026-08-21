#ifndef IRRIGATION_LANGUAGE_REPOSITORY_H
#define IRRIGATION_LANGUAGE_REPOSITORY_H

#include "domain/entity/language.h"

namespace irrigation
{

class LanguageRepository
{
public:

    virtual ~LanguageRepository() = default;

    virtual Language load() = 0;

    virtual void save(Language language) = 0;
};

}

#endif
