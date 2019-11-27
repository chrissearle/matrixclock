#include "OpenWeatherMap.h"

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>

#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

OpenWeatherMap::OpenWeatherMap(const char *apiKey, const char *city)
{
    _apiKey = strdup(apiKey);
    _city = strdup(city);
}

OpenWeather OpenWeatherMap::getWeather()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    client.get(String() + _apiRoot + "weather?units=metric&id=" + _city + "&APPID=" + _apiKey);

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

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

            return {-1, "", "", -1, -1, -1, -1, -1};
        }

        return {
            doc["weather"][0]["id"].as<int>(),
            doc["weather"][0]["main"].as<const char *>(),
            doc["weather"][0]["description"].as<const char *>(),
            doc["main"]["temp"].as<float>(),
            doc["main"]["pressure"].as<int>(),
            doc["main"]["humidity"].as<int>(),
            doc["wind"]["speed"].as<float>(),
            doc["clouds"]["all"].as<int>()};
    }
    else
    {
        Serial.println(String() + "Failed to fetch weather " + statusCode + " " + response);
    }

    return {-2, "", "", -2, -2, -2, -2, -2};
}

void OpenWeatherMap::getForecast()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    client.get(String() + _apiRoot + "forecast?units=metric&cnt=40&id=" + _city + "&APPID=" + _apiKey);

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

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

            return;
        }

        JsonArray list = doc["list"];

        for (int i = 0; i < doc["cnt"]; i++)
        {
            JsonObject reading = list[i];

            Serial.println(String() + "Temp " + reading["main"]["temp"].as<float>());
            Serial.println(String() + "Pressure " + reading["main"]["pressure"].as<int>());
            Serial.println(String() + "Humidity " + reading["main"]["humidity"].as<int>());
            Serial.println(String() + "ID " + reading["weather"][0]["id"].as<int>());
            Serial.println(String() + "Weather " + reading["weather"][0]["main"].as<const char *>());
            Serial.println(String() + "Description " + reading["weather"][0]["description"].as<const char *>());
            Serial.println(String() + "Clouds " + reading["clouds"]["all"].as<int>());
            Serial.println(String() + "Wind " + reading["wind"]["speed"].as<float>());
            Serial.println(String() + "DT " + reading["dt"].as<long>());
            Serial.println(String() + "DT " + reading["dt_txt"].as<const char *>());
        }
    }
    else
    {
        Serial.println(String() + "Failed to fetch forecast " + statusCode + " " + response);
    }
}
