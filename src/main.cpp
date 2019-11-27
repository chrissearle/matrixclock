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
#include "Display.h"

Netatmo *netatmo = NULL;
OpenWeatherMap *openWeatherMap = NULL;
Display *display = NULL;

TaskHandle_t DisplayTask;

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
  if (netatmo->update())
  {
    display->setWeather(netatmo->getTemperature(), netatmo->getHumidity());
  }

  if (openWeatherMap->updateWeather())
  {
    display->setOpenWeather(openWeatherMap->getWeatherId(),
                            openWeatherMap->getTemperature());
  }

  if (openWeatherMap->updateForecast())
  {
    display->setOpenForecast();
  }
}

void shortFetch()
{
  display->setTime(CE.toLocal(now()));
}

void UpdateDisplay(void *parameter)
{
  for (;;)
  {
    display->update();
    delay(20); // Watchdog
  }
}

void setup()
{
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect("NetatmoTest");

  Serial.println(WiFi.localIP());

  netatmo = new Netatmo(NETATMO_CLIENT_ID, NETATMO_CLIENT_SECRET, NETATMO_USERNAME, NETATMO_PASSWORD);
  openWeatherMap = new OpenWeatherMap(OWM_API_KEY, OWM_CITY);
  display = new Display();

  timeClient.begin();

  setSyncInterval(60);
  setSyncProvider(getTime);

  xTaskCreatePinnedToCore(
      UpdateDisplay,
      "DisplayTask",
      10000,
      NULL,
      0,
      &DisplayTask,
      0);

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