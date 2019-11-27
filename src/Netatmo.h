#ifndef NETATMO_H
#define NETATMO_H

#include "Arduino.h"

typedef struct Weather
{
    float outsideTemp;
    int outsideHumidity;
} Weather;

class Netatmo
{
private:
    char *_clientId;
    char *_clientSecret;
    char *_username;
    char *_password;

    const char *getToken();

public:
    Netatmo(const char *clientId, const char *clientSecret, const char *username, const char *password);

    Weather getWeather();
};

#endif