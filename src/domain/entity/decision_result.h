#ifndef IRRIGATION_DECISION_RESULT_H
#define IRRIGATION_DECISION_RESULT_H

#include "domain/entity/reason_code.h"

namespace irrigation
{

struct DecisionResult
{
    bool shouldRunPump = false;

    ReasonCode reason = ReasonCode::TriggerNotMet;
};

}

#endif