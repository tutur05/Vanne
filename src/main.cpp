#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TaskScheduler.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "init_lib.h"
#include "init_display.h"
#include "moteur.h"
#include "config.h"
#include "interaction.h"
// Adafruit_BME280 bme; // I2C

#define SEALEVELPRESSURE_HPA (1013.25)



#ifdef LUISANT
#define MQTT_SERVER "192.168.0.184"
#define SSID "Livebox-E300"
#define PASSWIFI "FdJuA4RYCYy5VtMYug"
#endif

#ifdef BETA
#define SSID "Betamachine_Tec"
#define PASSWIFI "betatecissogood"
#define MQTT_SERVER "10.11.12.10"
#endif

Adafruit_SSD1306 display(128, 64, &Wire, -1);

byte mode = 0; // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
int consigne = 18;

const char *ssid = SSID;
const char *password = PASSWIFI;
float temperature = 0;
float humidity = 0;
unsigned last_pir = 0;
unsigned last_menu = 0;

Task t1(10000, TASK_FOREVER, &t1Callback); // toutes les 10s MAJ capteurs
Task t2(900, TASK_FOREVER, &t2Callback);   // Tache MAJ MQTT
Task t3(5000, TASK_FOREVER, &update_display);
Scheduler runner;

// DHTesp dht;

void setup()
{
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BTNUP, INPUT_PULLUP);
  pinMode(PIN_BTNMID, INPUT_PULLUP);
  pinMode(PIN_BTNDOWN,INPUT_PULLUP);
  init_bme280();

  init_OTA();
  delay(5000); // Delay pour s'assurer qu'on ai le temps de upper en cas de bug
               // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.clearDisplay();

  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED, HIGH);

  mqtt_setup();
  mqtt_reconnect();

  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);

  t1.enable();
  t2.enable();
  t3.enable();
}

void loop()
{
  runner.execute();

  ArduinoOTA.handle();
 init_pin();
}