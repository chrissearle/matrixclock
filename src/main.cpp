#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WifiManager.h>

#include "netatmo_secrets.h"

#include "Netatmo.h"

Netatmo *netatmo = NULL;

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("NetatmoTest");

  Serial.println(WiFi.localIP());

  netatmo = new Netatmo(NETATMO_CLIENT_ID, NETATMO_CLIENT_SECRET, NETATMO_USERNAME, NETATMO_PASSWORD);

  Weather weather = netatmo->getWeather();

  Serial.println(weather.outsideTemp);
  Serial.println(weather.outsideHumidity);
}

void loop()
{
  // put your main code here, to run repeatedly:
}