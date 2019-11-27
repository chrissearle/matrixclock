#include "Netatmo.h"

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>

#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

Netatmo::Netatmo(const char *clientId, const char *clientSecret, const char *username, const char *password)
{
    _clientId = strdup(clientId);
    _clientSecret = strdup(clientSecret);
    _username = strdup(username);
    _password = strdup(password);

    _outsideTemperature = -1000.0;
    _outsideHumidity = -1000;
}

const char *Netatmo::getToken()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    char postData[65 + strlen(_clientId) + strlen(_clientSecret) + strlen(_username) + strlen(_password)];

    sprintf(postData, "grant_type=password&client_id=%s&client_secret=%s&username=%s&password=%s",
            _clientId, _clientSecret, _username, _password);

    client.beginRequest();
    client.post("/oauth2/token");
    client.sendHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
    client.sendHeader("Content-Length", strlen(postData));
    client.beginBody();
    client.print(postData);
    client.endRequest();

    int statusCode = client.responseStatusCode();

    if (statusCode == 200)
    {
        const char *response = client.responseBody().c_str();

        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5) + 210;
        DynamicJsonDocument doc(capacity);

        deserializeJson(doc, response);

        return strdup(doc["access_token"]);
    }
    else
    {
        return NULL;
    }
}

bool Netatmo::update()
{
    const char *token = getToken();

    if (token == NULL)
    {
        return false;
    }

    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    char query[35 + strlen(token)];
    sprintf(query, "/api/getstationsdata?access_token=%s", token);
    client.get(query);

    int statusCode = client.responseStatusCode();
    const char *response = client.responseBody().c_str();

    if (statusCode == 200)
    {
        const size_t capacity = 3 * JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(8) + 2 * JSON_OBJECT_SIZE(12) + 2 * JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(16) + 1400;
        DynamicJsonDocument doc(capacity);

        DeserializationError err = deserializeJson(doc, response);

        if (err)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(err.c_str());

            return false;
        }

        JsonObject outdoor = doc["body"]["devices"][0]["modules"][0]["dashboard_data"];

        _outsideTemperature = outdoor["Temperature"].as<float>();
        _outsideHumidity = outdoor["Humidity"].as<int>();

        return true;
    }
    else
    {
        Serial.print("Failed to fetch netatmo ");
        Serial.print(statusCode);
        Serial.print(" ");
        Serial.println(response);

        return false;
    }
}

float Netatmo::getTemperature()
{
    return _outsideTemperature;
}

int Netatmo::getHumidity()
{
    return _outsideHumidity;
}
