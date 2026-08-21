#include "presentation/serial/serial_logger.h"

namespace irrigation
{

    namespace SerialLogger
    {

        void begin(unsigned long baudRate)
        {
            Serial.begin(baudRate);
            delay(200);
        }

        void info(const String &message)
        {
            Serial.print("[INFO] ");
            Serial.println(message);
        }

        void warn(const String &message)
        {
            Serial.print("[WARN] ");
            Serial.println(message);
        }

        void error(const String &message)
        {
            Serial.print("[ERROR] ");
            Serial.println(message);
        }

    }

}
