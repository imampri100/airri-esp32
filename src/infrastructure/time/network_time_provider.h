#ifndef IRRIGATION_NETWORK_TIME_PROVIDER_H
#define IRRIGATION_NETWORK_TIME_PROVIDER_H

#include "domain/provider/time_provider.h"
#include "domain/repository/time_repository.h"

namespace irrigation
{

    class NetworkTimeProvider : public TimeProvider, public TimeRepository
    {
    public:
        NetworkTimeProvider() = default;

        bool begin() override;

        bool synchronize() override;

        bool isSynchronized() const override;

        Timestamp now() const override;

        // TimeRepository (domain) — delegasi ke implementasi TimeProvider
        // di atas. Pakai cast ke referensi (BUKAN qualified call seperti
        // TimeProvider::now()), supaya tetap virtual dispatch, karena
        // TimeProvider::now() murni abstrak (tidak punya body sendiri).
        Timestamp now() override
        {
            return static_cast<const TimeProvider &>(*this).now();
        }

        bool isSynchronized() override
        {
            return static_cast<const TimeProvider &>(*this).isSynchronized();
        }

    private:
        bool synchronized = false;
    };

}

#endif
