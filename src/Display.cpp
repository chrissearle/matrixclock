#include "Display.h"

Display::Display()
{
    _newTimeData = false;
    _newNetatmoData = false;
    _newWeatherData = false;
    _newForecastData = false;
}

void Display::setTime(time_t t)
{
    _time = t;
    _newTimeData = true;
}

void Display::setWeather(float temp, int humidity)
{
    _outsideTemperature = temp;
    _outsideHumidity = humidity;
    _newNetatmoData = true;
}

void Display::setOpenWeather(int id, float temp)
{
    _openWeatherId = id;
    _openWeatherTemp = temp;
    _newWeatherData = true;
}

void Display::setOpenForecast()
{
    _newForecastData = true;
}

void Display::update()
{
    if (_newTimeData)
    {
        char timeStr[11];
        sprintf(timeStr, "%02u - %02u:%02u", day(_time), hour(_time), minute(_time));
        Serial.println(timeStr);

        _newTimeData = false;
    }

    if (_newNetatmoData)
    {
        char netatmoStr[20];
        sprintf(netatmoStr, "T: %4.2f, H: %2d", _outsideTemperature, _outsideHumidity);
        Serial.println(netatmoStr);

        _newNetatmoData = false;
    }

    if (_newWeatherData)
    {
        char weatherStr[30];
        sprintf(weatherStr, "I: %4d, T: %4.2f", _openWeatherId, _openWeatherTemp);
        Serial.println(weatherStr);

        _newWeatherData = false;
    }

    if (_newForecastData)
    {
        // Not sure what to do with forecast yet
        Serial.println("F: ");

        _newForecastData = false;
    }
}