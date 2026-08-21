#ifndef IRRIGATION_TIMESTAMP_H
#define IRRIGATION_TIMESTAMP_H

#include <stdint.h>

namespace irrigation
{

    class Timestamp
    {
    public:
        Timestamp() = default;

        explicit Timestamp(
            uint64_t unixSeconds)
            : unixSeconds(unixSeconds)
        {
        }

        uint64_t value() const
        {
            return unixSeconds;
        }

        bool isValid() const
        {
            return unixSeconds != 0;
        }

        bool operator==(
            const Timestamp &other) const
        {
            return unixSeconds == other.unixSeconds;
        }

        bool operator!=(
            const Timestamp &other) const
        {
            return !(*this == other);
        }

        bool operator<(
            const Timestamp &other) const
        {
            return unixSeconds < other.unixSeconds;
        }

        bool operator>(
            const Timestamp &other) const
        {
            return unixSeconds > other.unixSeconds;
        }

    private:
        uint64_t unixSeconds = 0;
    };

}

#endif