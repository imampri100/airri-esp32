#ifndef IRRIGATION_AUTOMATION_TASK_H
#define IRRIGATION_AUTOMATION_TASK_H

#include <Arduino.h>

namespace irrigation
{

    class AutomationTask
    {
    public:
        virtual ~AutomationTask() = default;

        virtual uint32_t interval() const = 0;

        virtual void execute() = 0;
    };

}

#endif