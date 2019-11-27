#ifndef Display_H
#define Display_H

#include "Arduino.h"

class Display
{
private:
    boolean _newTimeData;
    boolean _newNetatmoData;
    boolean _newWeatherData;
    boolean _newForecastData;
    time_t _time;
    float _outsideTemperature;
    int _outsideHumidity;
    int _openWeatherId;
    float _openWeatherTemp;

public:
    Display();

    void setTime(time_t t);
    void setWeather(float temp, int humidity);
    void setOpenWeather(int id, float temp);
    void setOpenForecast();

    void update();
};

#endif