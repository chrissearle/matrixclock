#include "OpenWeatherMap.h"

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>

#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

OpenWeatherMap::OpenWeatherMap(const char *apiKey, const char *city)
{
    _apiKey = strdup(apiKey);
    _city = strdup(city);
    _weather = NULL;
}

bool OpenWeatherMap::updateWeather()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    char query[32 + strlen(_apiRoot) + strlen(_city) + strlen(_apiKey)];
    sprintf(query, "%sweather?units=metric&id=%s&APPID=%s", _apiRoot, _city, _apiKey);
    client.get(query);

    int statusCode = client.responseStatusCode();
    const char *response = client.responseBody().c_str();

    if (statusCode == 200)
    {
        const size_t capacity = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(4) + 2 * JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(13) + 310;
        DynamicJsonDocument doc(capacity);

        DeserializationError err = deserializeJson(doc, response);

        if (err)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(err.c_str());
            Serial.println(response);

            return false;
        }

        // Not sure what fields I want yet
        OpenWeather weather = {
            doc["weather"][0]["id"].as<int>(),
            doc["weather"][0]["main"].as<const char *>(),
            doc["weather"][0]["description"].as<const char *>(),
            doc["main"]["temp"].as<float>(),
            doc["main"]["pressure"].as<int>(),
            doc["main"]["humidity"].as<int>(),
            doc["wind"]["speed"].as<float>(),
            doc["clouds"]["all"].as<int>()};

        _weather = &weather;

        return true;
    }
    else
    {
        Serial.print("Failed to fetch weather ");
        Serial.print(statusCode);
        Serial.print(" ");
        Serial.println(response);
    }

    return false;
}

int OpenWeatherMap::getWeatherId()
{
    return _weather->id;
}

float OpenWeatherMap::getTemperature()
{
    return _weather->temp;
}

bool OpenWeatherMap::updateForecast()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    char query[39 + strlen(_apiRoot) + strlen(_city) + strlen(_apiKey)];
    sprintf(query, "%sforecast?units=metric&cnt=40&id=%s&APPID=%s", _apiRoot, _city, _apiKey);
    client.get(query);

    int statusCode = client.responseStatusCode();
    const char *response = client.responseBody().c_str();

    if (statusCode == 200)
    {
        const size_t capacity = 40 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(40) + 92 * JSON_OBJECT_SIZE(1) + 41 * JSON_OBJECT_SIZE(2) + 40 * JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 29 * JSON_OBJECT_SIZE(7) + 52 * JSON_OBJECT_SIZE(8) + 8850;
        DynamicJsonDocument doc(capacity);

        DeserializationError err = deserializeJson(doc, response);

        if (err)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(err.c_str());
            Serial.println(response);

            return false;
        }

        JsonArray list = doc["list"];

        for (int i = 0; i < doc["cnt"]; i++)
        {
            JsonObject reading = list[i];

            // Not sure what to do with forecast yet
            /*
            Serial.println(String() + "Temp " + reading["main"]["temp"].as<float>());
            Serial.println(String() + "Pressure " + reading["main"]["pressure"].as<int>());
            Serial.println(String() + "Humidity " + reading["main"]["humidity"].as<int>());
            Serial.println(String() + "ID " + reading["weather"][0]["id"].as<int>());
            Serial.println(String() + "Weather " + reading["weather"][0]["main"].as<const char *>());
            Serial.println(String() + "Description " + reading["weather"][0]["description"].as<const char *>());
            Serial.println(String() + "Clouds " + reading["clouds"]["all"].as<int>());
            Serial.println(String() + "Wind " + reading["wind"]["speed"].as<float>());
            Serial.println(String() + "DT " + reading["dt"].as<long>());
            */
            Serial.println(String() + "DT " + reading["dt_txt"].as<const char *>());
        }

        return true;
    }
    else
    {
        Serial.print("Failed to fetch forecast ");
        Serial.print(statusCode);
        Serial.print(" ");
        Serial.println(response);
    }

    return false;
}
