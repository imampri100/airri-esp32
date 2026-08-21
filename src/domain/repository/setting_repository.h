#ifndef IRRIGATION_SETTING_REPOSITORY_H
#define IRRIGATION_SETTING_REPOSITORY_H

#include "domain/entity/restriction_setting.h"
#include "domain/entity/trigger_setting.h"

namespace irrigation
{

class SettingRepository
{
public:

    virtual ~SettingRepository() = default;

    virtual TriggerSetting getTriggerSetting() = 0;

    virtual RestrictionSetting getRestrictionSetting() = 0;

    virtual void saveTriggerSetting(
        const TriggerSetting& setting
    ) = 0;

    virtual void saveRestrictionSetting(
        const RestrictionSetting& setting
    ) = 0;
};

}

#endif