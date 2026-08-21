#ifndef IRRIGATION_PUMP_TEST_USECASE_H
#define IRRIGATION_PUMP_TEST_USECASE_H

#include <Arduino.h>

#include "domain/repository/pump_repository.h"

namespace irrigation
{
    class PumpTestUseCase
    {
    public:
        explicit PumpTestUseCase(
            PumpRepository &pumpRepository);

        // Menyalakan pompa selama `durationSecond` detik lalu mematikannya
        // lagi (blocking, dibatasi MAX_TEST_DURATION_SECOND demi keamanan).
        // Cocok untuk tombol "Test Pump" di app - dipanggil sesekali saja,
        // bukan bagian dari loop utama.
        void execute(uint32_t durationSecond);

        static constexpr uint32_t MAX_TEST_DURATION_SECOND = 10;

    private:
        PumpRepository &pumpRepository;
    };
}

#endif
