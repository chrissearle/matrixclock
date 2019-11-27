#ifndef NETATMO_H
#define NETATMO_H

#include "Arduino.h"

class Netatmo
{
private:
    char *_clientId;
    char *_clientSecret;
    char *_username;
    char *_password;

    float _outsideTemperature;
    int _outsideHumidity;

    const char *_server = "api.netatmo.com";

    const char *getToken();

public:
    Netatmo(const char *clientId, const char *clientSecret, const char *username, const char *password);

    bool update();

    float getTemperature();
    int getHumidity();
};

#endif