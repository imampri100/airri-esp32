#include "infrastructure/network/http_server.h"

#include <ArduinoJson.h>

namespace irrigation
{

    HttpServer::HttpServer(uint16_t port)
        : server(port)
    {
    }

    void HttpServer::begin()
    {
        server.begin();
    }

    void HttpServer::handleClient()
    {
        server.handleClient();
    }

    WebServer &HttpServer::raw()
    {
        return server;
    }

    void HttpServer::sendJson(int statusCode, const String &jsonBody)
    {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(statusCode, "application/json", jsonBody);
    }

    void HttpServer::sendJsonError(int statusCode, const String &message)
    {
        StaticJsonDocument<128> document;
        document["error"] = message;

        String body;
        serializeJson(document, body);

        sendJson(statusCode, body);
    }

}
