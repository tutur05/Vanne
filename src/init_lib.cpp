#include "init_lib.h"
#include "config.h"

const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = 1883;
const char *mqtt_user = "";
const char *mqtt_password = "";
int mq = 0;
WiFiClient espClient;
PubSubClient client(espClient);
extern float temperature;
extern float humidity;

extern const char *ssid;
extern const char *password;
extern const char *ssid2;
extern const char *password2;
extern int consigne;
extern byte mode;
extern unsigned last_pir;
extern unsigned last_menu;

Adafruit_BME280 bme; // définition de la variable globale

bool init_bme280()
{
  return bme.begin(0x76); // ou 0x77 selon ton câblage
}

void init_OTA()
{
  ArduinoOTA.setHostname("vanneV2");
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
}

void mqtt_setup()
{
  client.setServer(mqtt_server, mqtt_port);
}

bool mqtt_reconnect()
{
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      client.publish("VanneV1/topic", "Hello from ESP8266");
      return true;
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      return false;
    }
  }
  return true;
}

PubSubClient &getMqttClient()
{
  return client;
}

void t1Callback()
{

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();

  if (!getMqttClient().connected())
  {
    mqtt_reconnect();
    mq++;
  }

  client.publish("VanneV1/T", String(temperature, 2).c_str());
  client.publish("VanneV1/H", String(humidity, 2).c_str());
  client.publish("VanneV1/timeout", String(mq).c_str());
}

void t2Callback() // maintient de liaison MQTT
{
  getMqttClient().loop(); // maintient le mqtt en ligne (1s max entre 2 loop)
}

bool checkWifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected, reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
  }
  if (WiFi.status() == WL_CONNECTED)
    return true;
  else
    return false;
}

void regul_therm()
{
  // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
  if (mode == 0)
  {
    if (consigne > temperature)
    {
      // VANNE ON
    }
    else
    {
      // VANNE OFF
    }
  }
  if (mode == 1)
  {

    if (16 > temperature)
    {
      // VANNE ON
    }
    else
    {
      // VANNE OFF
    }
  }
  if (mode == 2)
  {
    if ((last_pir + 60000) > millis()) // SI PIR RECENT
    {
      if (consigne > temperature)
      {
        // VANNE ON
      }
      else
      {
        // VANNE OFF
      }
    }
    else
    {
      mode = 1;
    }
  }
}

void check_wifi()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      // SSID2
      WiFi.disconnect();
      WiFi.begin(ssid2, password2);
      if (WiFi.waitForConnectResult() != WL_CONNECTED)
      {
        Serial.println("WiFi backup connection failed");
        return;
      }
      else
      {
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED, HIGH);
      }
    }
    else
    {
      Serial.println("WiFi1 connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      digitalWrite(LED, HIGH);
    }
  }
}