#include "Netatmo.h"

#include <ArduinoHttpClient.h>
#include <WiFiClientSecure.h>

#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>

Netatmo::Netatmo(const char *clientId, const char *clientSecret, const char *username, const char *password)
{
    _clientId = strdup(clientId);
    _clientSecret = strdup(clientSecret);
    _username = strdup(username);
    _password = strdup(password);
}

const char *Netatmo::getToken()
{
    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    String postData = String() +
                      "grant_type=password" +
                      "&client_id=" +
                      _clientId +
                      "&client_secret=" +
                      _clientSecret +
                      "&username=" +
                      _username +
                      "&password=" +
                      _password;

    client.beginRequest();
    client.post("/oauth2/token");
    client.sendHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
    client.sendHeader("Content-Length", postData.length());
    client.beginBody();
    client.print(postData);
    client.endRequest();

    int statusCode = client.responseStatusCode();

    if (statusCode == 200)
    {
        String response = client.responseBody();

        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5) + 210;
        DynamicJsonDocument doc(capacity);

        deserializeJson(doc, response);

        return strdup(doc["access_token"]);
    }
    else
    {
        return NULL;
    }
}

Weather Netatmo::getWeather()
{
    const char *token = getToken();

    if (token == NULL)
    {
        return {-1, -1};
    }

    WiFiClientSecure wifi;
    HttpClient client = HttpClient(wifi, _server, 443);

    client.get(String() + "/api/getstationsdata?access_token=" + token);

    int statusCode = client.responseStatusCode();
    String response = client.responseBody();

    if (statusCode == 200)
    {
        const size_t capacity = 3 * JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(8) + 2 * JSON_OBJECT_SIZE(12) + 2 * JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(16) + 1400;
        DynamicJsonDocument doc(capacity);

        DeserializationError err = deserializeJson(doc, response.c_str());

        if (err)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(err.c_str());

            return {-3, -3};
        }

        JsonObject outdoor = doc["body"]["devices"][0]["modules"][0]["dashboard_data"];

        return {outdoor["Temperature"].as<float>(), outdoor["Humidity"].as<int>()};
    }
    else
    {
        Serial.println(String() + "Failed to fetch netatmo " + statusCode + " " + response);
        return {-2, -2};
    }
}

/*
const size_t capacity = 3*JSON_ARRAY_SIZE(1) + 2*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(8) + 2*JSON_OBJECT_SIZE(12) + 2*JSON_OBJECT_SIZE(13) + JSON_OBJECT_SIZE(16) + 1400;
DynamicJsonDocument doc(capacity);

const char* json = "{\"body\":{\"devices\":[{\"_id\":\"70:ee:50:00:b2:e6\",\"station_name\":\"Havna allé\",\"date_setup\":1365591385,\"last_setup\":1365591385,\"type\":\"NAMain\",\"last_status_store\":1574840042,\"module_name\":\"Indoor\",\"firmware\":132,\"last_upgrade\":1440005952,\"wifi_status\":58,\"reachable\":true,\"co2_calibrating\":false,\"data_type\":[\"Temperature\",\"CO2\",\"Humidity\",\"Noise\",\"Pressure\"],\"place\":{\"altitude\":82,\"city\":\"Oslo\",\"country\":\"NO\",\"timezone\":\"Europe/Oslo\",\"location\":[10.713543,59.938755]},\"dashboard_data\":{\"time_utc\":1574840028,\"Temperature\":22.7,\"CO2\":896,\"Humidity\":39,\"Noise\":37,\"Pressure\":1003.4,\"AbsolutePressure\":993.7,\"min_temp\":22.1,\"max_temp\":22.9,\"date_max_temp\":1574819641,\"date_min_temp\":1574812687,\"temp_trend\":\"up\",\"pressure_trend\":\"down\"},\"modules\":[{\"_id\":\"02:00:00:00:ab:50\",\"type\":\"NAModule1\",\"module_name\":\"Outdoor\",\"data_type\":[\"Temperature\",\"Humidity\"],\"last_setup\":1365595402,\"battery_percent\":56,\"reachable\":true,\"firmware\":44,\"last_message\":1574840038,\"last_seen\":1574840000,\"rf_status\":68,\"battery_vp\":4943,\"dashboard_data\":{\"time_utc\":1574840000,\"Temperature\":2.7,\"Humidity\":91,\"min_temp\":2.3,\"max_temp\":2.7,\"date_max_temp\":1574836974,\"date_min_temp\":1574825646,\"temp_trend\":\"stable\"}},{\"_id\":\"05:00:00:00:8d:7e\",\"type\":\"NAModule3\",\"module_name\":\"Rain gauge\",\"data_type\":[\"Rain\"],\"last_setup\":1402583543,\"battery_percent\":6,\"reachable\":false,\"firmware\":8,\"last_message\":1536715402,\"last_seen\":1536715331,\"rf_status\":95,\"battery_vp\":3942},{\"_id\":\"06:00:00:01:47:76\",\"type\":\"NAModule2\",\"module_name\":\"Wind Gauge\",\"data_type\":[\"Wind\"],\"last_setup\":1472653619,\"battery_percent\":33,\"reachable\":false,\"firmware\":18,\"last_message\":1530856769,\"last_seen\":1530856769,\"rf_status\":86,\"battery_vp\":4618}]}],\"user\":{\"mail\":\"chris@chrissearle.org\",\"administrative\":{\"country\":\"NO\",\"reg_locale\":\"en-NO\",\"lang\":\"en-GB\",\"unit\":0,\"windunit\":2,\"pressureunit\":0,\"feel_like_algo\":0}}},\"status\":\"ok\",\"time_exec\":0.06195783615112305,\"time_server\":1574840237}";

deserializeJson(doc, json);

JsonObject body_devices_0 = doc["body"]["devices"][0];
const char* body_devices_0__id = body_devices_0["_id"]; // "70:ee:50:00:b2:e6"
const char* body_devices_0_station_name = body_devices_0["station_name"]; // "Havna allé"
long body_devices_0_date_setup = body_devices_0["date_setup"]; // 1365591385
long body_devices_0_last_setup = body_devices_0["last_setup"]; // 1365591385
const char* body_devices_0_type = body_devices_0["type"]; // "NAMain"
long body_devices_0_last_status_store = body_devices_0["last_status_store"]; // 1574840042
const char* body_devices_0_module_name = body_devices_0["module_name"]; // "Indoor"
int body_devices_0_firmware = body_devices_0["firmware"]; // 132
long body_devices_0_last_upgrade = body_devices_0["last_upgrade"]; // 1440005952
int body_devices_0_wifi_status = body_devices_0["wifi_status"]; // 58
bool body_devices_0_reachable = body_devices_0["reachable"]; // true
bool body_devices_0_co2_calibrating = body_devices_0["co2_calibrating"]; // false

JsonArray body_devices_0_data_type = body_devices_0["data_type"];
const char* body_devices_0_data_type_0 = body_devices_0_data_type[0]; // "Temperature"
const char* body_devices_0_data_type_1 = body_devices_0_data_type[1]; // "CO2"
const char* body_devices_0_data_type_2 = body_devices_0_data_type[2]; // "Humidity"
const char* body_devices_0_data_type_3 = body_devices_0_data_type[3]; // "Noise"
const char* body_devices_0_data_type_4 = body_devices_0_data_type[4]; // "Pressure"

JsonObject body_devices_0_place = body_devices_0["place"];
int body_devices_0_place_altitude = body_devices_0_place["altitude"]; // 82
const char* body_devices_0_place_city = body_devices_0_place["city"]; // "Oslo"
const char* body_devices_0_place_country = body_devices_0_place["country"]; // "NO"
const char* body_devices_0_place_timezone = body_devices_0_place["timezone"]; // "Europe/Oslo"

float body_devices_0_place_location_0 = body_devices_0_place["location"][0]; // 10.713543
float body_devices_0_place_location_1 = body_devices_0_place["location"][1]; // 59.938755

JsonObject body_devices_0_dashboard_data = body_devices_0["dashboard_data"];
long body_devices_0_dashboard_data_time_utc = body_devices_0_dashboard_data["time_utc"]; // 1574840028
float body_devices_0_dashboard_data_Temperature = body_devices_0_dashboard_data["Temperature"]; // 22.7
int body_devices_0_dashboard_data_CO2 = body_devices_0_dashboard_data["CO2"]; // 896
int body_devices_0_dashboard_data_Humidity = body_devices_0_dashboard_data["Humidity"]; // 39
int body_devices_0_dashboard_data_Noise = body_devices_0_dashboard_data["Noise"]; // 37
float body_devices_0_dashboard_data_Pressure = body_devices_0_dashboard_data["Pressure"]; // 1003.4
float body_devices_0_dashboard_data_AbsolutePressure = body_devices_0_dashboard_data["AbsolutePressure"]; // 993.7
float body_devices_0_dashboard_data_min_temp = body_devices_0_dashboard_data["min_temp"]; // 22.1
float body_devices_0_dashboard_data_max_temp = body_devices_0_dashboard_data["max_temp"]; // 22.9
long body_devices_0_dashboard_data_date_max_temp = body_devices_0_dashboard_data["date_max_temp"]; // 1574819641
long body_devices_0_dashboard_data_date_min_temp = body_devices_0_dashboard_data["date_min_temp"]; // 1574812687
const char* body_devices_0_dashboard_data_temp_trend = body_devices_0_dashboard_data["temp_trend"]; // "up"
const char* body_devices_0_dashboard_data_pressure_trend = body_devices_0_dashboard_data["pressure_trend"]; // "down"

JsonArray body_devices_0_modules = body_devices_0["modules"];

JsonObject body_devices_0_modules_0 = body_devices_0_modules[0];
const char* body_devices_0_modules_0__id = body_devices_0_modules_0["_id"]; // "02:00:00:00:ab:50"
const char* body_devices_0_modules_0_type = body_devices_0_modules_0["type"]; // "NAModule1"
const char* body_devices_0_modules_0_module_name = body_devices_0_modules_0["module_name"]; // "Outdoor"

const char* body_devices_0_modules_0_data_type_0 = body_devices_0_modules_0["data_type"][0]; // "Temperature"
const char* body_devices_0_modules_0_data_type_1 = body_devices_0_modules_0["data_type"][1]; // "Humidity"

long body_devices_0_modules_0_last_setup = body_devices_0_modules_0["last_setup"]; // 1365595402
int body_devices_0_modules_0_battery_percent = body_devices_0_modules_0["battery_percent"]; // 56
bool body_devices_0_modules_0_reachable = body_devices_0_modules_0["reachable"]; // true
int body_devices_0_modules_0_firmware = body_devices_0_modules_0["firmware"]; // 44
long body_devices_0_modules_0_last_message = body_devices_0_modules_0["last_message"]; // 1574840038
long body_devices_0_modules_0_last_seen = body_devices_0_modules_0["last_seen"]; // 1574840000
int body_devices_0_modules_0_rf_status = body_devices_0_modules_0["rf_status"]; // 68
int body_devices_0_modules_0_battery_vp = body_devices_0_modules_0["battery_vp"]; // 4943

JsonObject body_devices_0_modules_0_dashboard_data = body_devices_0_modules_0["dashboard_data"];
long body_devices_0_modules_0_dashboard_data_time_utc = body_devices_0_modules_0_dashboard_data["time_utc"]; // 1574840000
float body_devices_0_modules_0_dashboard_data_Temperature = body_devices_0_modules_0_dashboard_data["Temperature"]; // 2.7
int body_devices_0_modules_0_dashboard_data_Humidity = body_devices_0_modules_0_dashboard_data["Humidity"]; // 91
float body_devices_0_modules_0_dashboard_data_min_temp = body_devices_0_modules_0_dashboard_data["min_temp"]; // 2.3
float body_devices_0_modules_0_dashboard_data_max_temp = body_devices_0_modules_0_dashboard_data["max_temp"]; // 2.7
long body_devices_0_modules_0_dashboard_data_date_max_temp = body_devices_0_modules_0_dashboard_data["date_max_temp"]; // 1574836974
long body_devices_0_modules_0_dashboard_data_date_min_temp = body_devices_0_modules_0_dashboard_data["date_min_temp"]; // 1574825646
const char* body_devices_0_modules_0_dashboard_data_temp_trend = body_devices_0_modules_0_dashboard_data["temp_trend"]; // "stable"

JsonObject body_devices_0_modules_1 = body_devices_0_modules[1];
const char* body_devices_0_modules_1__id = body_devices_0_modules_1["_id"]; // "05:00:00:00:8d:7e"
const char* body_devices_0_modules_1_type = body_devices_0_modules_1["type"]; // "NAModule3"
const char* body_devices_0_modules_1_module_name = body_devices_0_modules_1["module_name"]; // "Rain gauge"

const char* body_devices_0_modules_1_data_type_0 = body_devices_0_modules_1["data_type"][0]; // "Rain"

long body_devices_0_modules_1_last_setup = body_devices_0_modules_1["last_setup"]; // 1402583543
int body_devices_0_modules_1_battery_percent = body_devices_0_modules_1["battery_percent"]; // 6
bool body_devices_0_modules_1_reachable = body_devices_0_modules_1["reachable"]; // false
int body_devices_0_modules_1_firmware = body_devices_0_modules_1["firmware"]; // 8
long body_devices_0_modules_1_last_message = body_devices_0_modules_1["last_message"]; // 1536715402
long body_devices_0_modules_1_last_seen = body_devices_0_modules_1["last_seen"]; // 1536715331
int body_devices_0_modules_1_rf_status = body_devices_0_modules_1["rf_status"]; // 95
int body_devices_0_modules_1_battery_vp = body_devices_0_modules_1["battery_vp"]; // 3942

JsonObject body_devices_0_modules_2 = body_devices_0_modules[2];
const char* body_devices_0_modules_2__id = body_devices_0_modules_2["_id"]; // "06:00:00:01:47:76"
const char* body_devices_0_modules_2_type = body_devices_0_modules_2["type"]; // "NAModule2"
const char* body_devices_0_modules_2_module_name = body_devices_0_modules_2["module_name"]; // "Wind Gauge"

const char* body_devices_0_modules_2_data_type_0 = body_devices_0_modules_2["data_type"][0]; // "Wind"

long body_devices_0_modules_2_last_setup = body_devices_0_modules_2["last_setup"]; // 1472653619
int body_devices_0_modules_2_battery_percent = body_devices_0_modules_2["battery_percent"]; // 33
bool body_devices_0_modules_2_reachable = body_devices_0_modules_2["reachable"]; // false
int body_devices_0_modules_2_firmware = body_devices_0_modules_2["firmware"]; // 18
long body_devices_0_modules_2_last_message = body_devices_0_modules_2["last_message"]; // 1530856769
long body_devices_0_modules_2_last_seen = body_devices_0_modules_2["last_seen"]; // 1530856769
int body_devices_0_modules_2_rf_status = body_devices_0_modules_2["rf_status"]; // 86
int body_devices_0_modules_2_battery_vp = body_devices_0_modules_2["battery_vp"]; // 4618

const char* body_user_mail = doc["body"]["user"]["mail"]; // "chris@chrissearle.org"

JsonObject body_user_administrative = doc["body"]["user"]["administrative"];
const char* body_user_administrative_country = body_user_administrative["country"]; // "NO"
const char* body_user_administrative_reg_locale = body_user_administrative["reg_locale"]; // "en-NO"
const char* body_user_administrative_lang = body_user_administrative["lang"]; // "en-GB"
int body_user_administrative_unit = body_user_administrative["unit"]; // 0
int body_user_administrative_windunit = body_user_administrative["windunit"]; // 2
int body_user_administrative_pressureunit = body_user_administrative["pressureunit"]; // 0
int body_user_administrative_feel_like_algo = body_user_administrative["feel_like_algo"]; // 0

const char* status = doc["status"]; // "ok"
float time_exec = doc["time_exec"]; // 0.06195783615112305
long time_server = doc["time_server"]; // 1574840237
*/