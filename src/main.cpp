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
// Adafruit_BME280 bme; // I2C/



Adafruit_SSD1306 display(128, 64, &Wire, -1);

byte mode = 0; // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
int consigne = 18;

const char *ssid2 = SSID2;
const char *password2 = PASSWIFI2;
const char *ssid = SSID1;
const char *password = PASSWIFI1;
float temperature = 0;
float humidity = 0;
unsigned last_pir = 0;
unsigned last_menu = 0;
float gauss = 0;
short calib1 = 540;
short calib2 = 600;

Task t1(60000, TASK_FOREVER, &t1Callback); // toutes les 10s MAJ capteurs
Task t2(900, TASK_FOREVER, &t2Callback);   // Tache MAJ MQTT
Task t3(500, TASK_FOREVER, &update_display);
Task t4(30000, TASK_FOREVER, &check_wifi); // toutes les 30s on check le wifi;
Scheduler runner;

// DHTesp dht;

void setup()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));

  }

  update_display();
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BTNUP, INPUT_PULLUP);
  pinMode(PIN_BTNMID, INPUT_PULLUP);
  pinMode(PIN_BTNDOWN,INPUT_PULLUP);
  pinMode(A0, INPUT); // Pin A0 pour la sonde magnetique
  init_bme280();

  init_OTA();


  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  check_wifi();

  mqtt_setup();
  mqtt_reconnect();

  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);
  t1.enable();
  t2.enable();
  t3.enable();
  t4.enable();
  init_moteur();
}

void loop()
{

  runner.execute();

  if(millis() < 3000000) // 5 minutes pour faire la MAJ de l'OTA
  ArduinoOTA.handle();

 check_pin_button();
}