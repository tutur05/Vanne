#include "init_lib.h"
#include "config.h"
#include "scheduler.h"
#include "init_display.h"
#include "moteur.h"
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
extern unsigned long last_pir;
extern unsigned last_menu;
extern Task t3;
extern Task t6;
extern Task t7;
extern Adafruit_SSD1306 display;
extern String message1;
extern float gauss;
extern bool etat_vanne; // false = VANNE FERMEE, true = VANNE OUVERTE
Adafruit_BME280 bme;    // définition de la variable globale

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
  ArduinoOTA.setPassword((const char *)"tototititata28");
 /*/ ArduinoOTA.onStart([]()
   //                  { // Serial.println("Start"); });
  //ArduinoOTA.onEnd([]()
     //              { // Serial.println("\nEnd"); });
  //ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
       //                 { // Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  //ArduinoOTA.onError([](ota_error_t error)
         //            {
    // Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) // Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) // Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) // Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) // Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) // Serial.println("End Failed"); });*/
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
    // Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client"))
    {
      // Serial.println("connected");
      client.publish("VanneV1/topic", "Hello from ESP8266");
      return true;
    }
    else
    {
      // Serial.print("failed, rc=");
      // Serial.print(client.state());
      // Serial.println(" try again in 5 seconds");
      return false;
    }
  }
  return true;
}

PubSubClient &getMqttClient()
{
  return client;
}

void check_connection()
{
  check_wifi();

  if (!getMqttClient().connected())
  {
    mqtt_reconnect();
    mq++;
  }

  client.publish("VanneV1/T", String(temperature, 2).c_str());
  client.publish("VanneV1/H", String(humidity, 2).c_str());
  client.publish("VanneV1/timeout", String(mq).c_str());
}

void holdMQTT_Online() // maintient de liaison MQTT
{
  getMqttClient().loop(); // maintient le mqtt en ligne (1s max entre 2 loop)
}

void regul_therm() // régulation de la vanne + lecture sonde
{
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();

  // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
  if (mode == 0)
  {
    if (consigne > temperature)
    {

      // VANNE ON
      t6.enable(); // On active la desaction auto du moteur de la vanne
      //vanneO();
    }
    else // Il faut fermer la vanne
    {

      // VANNE OFF
      //vanneF();
      t7.enable(); // On active  la desaction auto du moteur de la vanne
    }
  }
  if (mode == 1)
  {

    if (16 > temperature)
    {
      // VANNE ON
      t6.enable(); // On active la desaction auto du moteur de la vanne
      //vanneO();
    }
    else
    {
      // VANNE OFF
      //vanneF();
      t7.enable(); // On active  la desaction auto du moteur de la vanne
    }
  }
  if (mode == 2)
  {
    if ((last_pir + 60000) > millis()) // SI PIR RECENT
    {
      if (consigne > temperature)
      {
        if (!etat_vanne)
        {
          // VANNE ON
        }
      }
      else
      {
        if (etat_vanne)
        {
          // VANNE OFF
        }
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
        // Serial.println("WiFi backup connection failed");
        return;
      }

    }

  }
  else
  {
    IPAddress ip = WiFi.localIP();
    message1 = ip.toString().c_str();
  }
}

void ReadCapteur()
{
  gauss = analogRead(A0); // Lecture de la sonde magnétique
}