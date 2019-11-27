#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WifiManager.h>
#include <NTPClient.h>
#include <Time.h>
#include <Timezone.h>

#include "netatmo_secrets.h"
#include "openweathermap_secrets.h"

#include "Netatmo.h"
#include "OpenWeatherMap.h"

Netatmo *netatmo = NULL;
OpenWeatherMap *openWeatherMap = NULL;

long longLastMsg = 0;
long longInterval = 1000 * 60 * 10;

long shortLastMsg = 0;
long shortInterval = 1000 * 60;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};
Timezone CE(CEST, CET);

time_t getTime()
{
  timeClient.update();
  return timeClient.getEpochTime();
}

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

void shortFetch()
{
  time_t t = CE.toLocal(now());
  Serial.println(String() + day(t) + " " + month(t) + " " + year(t) + " - " + hour(t) + ":" + minute(t));
}

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("NetatmoTest");

  Serial.println(WiFi.localIP());

  netatmo = new Netatmo(NETATMO_CLIENT_ID, NETATMO_CLIENT_SECRET, NETATMO_USERNAME, NETATMO_PASSWORD);
  openWeatherMap = new OpenWeatherMap(OWM_API_KEY, OWM_CITY);

  timeClient.begin();

  setSyncInterval(60);
  setSyncProvider(getTime);

  shortFetch();
  fetch();
}

void loop()
{
  long now = millis();

  if (now - shortLastMsg > shortInterval)
  {
    shortFetch();
    shortLastMsg = now;
  }

  if (now - longLastMsg > longInterval)
  {
    fetch();
    longLastMsg = now;
  }
}