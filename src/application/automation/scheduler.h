#ifndef IRRIGATION_SCHEDULER_H
#define IRRIGATION_SCHEDULER_H

#include <Arduino.h>

#include <vector>

#include "application/automation/automation_task.h"

namespace irrigation
{

    class Scheduler
    {
    public:
        void addTask(
            AutomationTask &task);

        void tick();

    private:
        struct TaskEntry
        {
            AutomationTask *task;

            uint32_t lastExecution = 0;
        };

        std::vector<TaskEntry> tasks;
    };

}

#endif