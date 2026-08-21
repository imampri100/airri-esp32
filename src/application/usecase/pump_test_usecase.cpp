#include "application/usecase/pump_test_usecase.h"

namespace irrigation
{

    PumpTestUseCase::PumpTestUseCase(
        PumpRepository &pumpRepository)
        : pumpRepository(pumpRepository)
    {
    }

    void PumpTestUseCase::execute(uint32_t durationSecond)
    {
        uint32_t cappedSecond = durationSecond;

        if (cappedSecond > MAX_TEST_DURATION_SECOND)
        {
            cappedSecond = MAX_TEST_DURATION_SECOND;
        }

        if (cappedSecond == 0)
        {
            return;
        }

        pumpRepository.start();
        delay(cappedSecond * 1000UL);
        pumpRepository.stop();
    }

}
