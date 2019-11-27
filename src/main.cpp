#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WifiManager.h>

#include "netatmo_secrets.h"
#include "openweathermap_secrets.h"

#include "Netatmo.h"
#include "OpenWeatherMap.h"

Netatmo *netatmo = NULL;
OpenWeatherMap *openWeatherMap = NULL;

long lastMsg = 0;
long interval = 1000 * 60 * 10;

void fetch()
{
  Serial.println("Fetching netatmo");
  Weather weather = netatmo->getWeather();

  Serial.println(weather.outsideTemp);
  Serial.println(weather.outsideHumidity);

  Serial.println("Fetching weather");
  OpenWeather openWeather = openWeatherMap->getWeather();

  Serial.println(openWeather.id);
  Serial.println(openWeather.weather);
  Serial.println(openWeather.description);
  Serial.println(openWeather.temp);
  Serial.println(openWeather.pressure);
  Serial.println(openWeather.humidity);
  Serial.println(openWeather.windSpeed);
  Serial.println(openWeather.cloudPerc);

  Serial.println("Fetching forecast");
  openWeatherMap->getForecast();
}

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("NetatmoTest");

  Serial.println(WiFi.localIP());

  netatmo = new Netatmo(NETATMO_CLIENT_ID, NETATMO_CLIENT_SECRET, NETATMO_USERNAME, NETATMO_PASSWORD);
  openWeatherMap = new OpenWeatherMap(OWM_API_KEY, OWM_CITY);

  fetch();
}

void loop()
{
  long now = millis();

  if (now - lastMsg > interval)
  {
    fetch();
    lastMsg = now;
  }
}