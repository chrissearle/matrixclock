#ifndef OWM_H
#define OWM_H

#include "Arduino.h"

typedef struct OpenWeather
{
    int id;
    const char *weather;
    const char *description;
    float temp;
    int pressure;
    int humidity;
    float windSpeed;
    int cloudPerc;
} OpenWeather;

class OpenWeatherMap
{
private:
    char *_apiKey;
    char *_city;

    const char *_server = "api.openweathermap.org";
    const char *_apiRoot = "/data/2.5/";

public:
    OpenWeatherMap(const char *apiKey, const char *city);

    OpenWeather getWeather();
    void getForecast();
};

#endif