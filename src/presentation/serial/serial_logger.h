#ifndef IRRIGATION_SERIAL_LOGGER_H
#define IRRIGATION_SERIAL_LOGGER_H

#include <Arduino.h>

namespace irrigation
{

    namespace SerialLogger
    {

        void begin(unsigned long baudRate = 115200);

        void info(const String &message);

        void warn(const String &message);

        void error(const String &message);

    }

}

#endif
