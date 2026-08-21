#ifndef IRRIGATION_HTTP_SERVER_H
#define IRRIGATION_HTTP_SERVER_H

#include <Arduino.h>
#include <WebServer.h>

namespace irrigation
{

    // Wrapper tipis di atas WebServer (bawaan ESP32 Arduino core), supaya
    // Router & handler tidak coupling langsung ke library pihak ketiga.
    class HttpServer
    {
    public:
        explicit HttpServer(uint16_t port);

        void begin();

        void handleClient();

        WebServer &raw();

        // Helper CORS supaya bisa diakses dari app Flutter / browser bebas.
        void sendJson(int statusCode, const String &jsonBody);

        void sendJsonError(int statusCode, const String &message);

    private:
        WebServer server;
    };

}

#endif
