#include "application/automation/scheduler.h"

namespace irrigation
{

    void Scheduler::addTask(
        AutomationTask &task)
    {
        tasks.push_back(
            {&task,
             millis() - task.interval()});
    }

    void Scheduler::tick()
    {
        uint32_t now = millis();

        for (auto &task : tasks)
        {
            if (
                now - task.lastExecution >= task.task->interval())
            {
                task.task->execute();

                task.lastExecution = now;
            }
        }
    }

}