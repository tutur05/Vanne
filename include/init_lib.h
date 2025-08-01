
#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

#define LUISANT //localisation BETA ou LUISANT


#include <PubSubClient.h>
#include <WiFiClient.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


extern Adafruit_BME280 bme;
 
// Déclaration des fonctions
bool init_bme280(); 
void init_OTA();

void mqtt_setup();
bool mqtt_reconnect();
PubSubClient& getMqttClient();

void t1Callback();
void t2Callback();
void update_display();
void check_wifi();
void regul_therm();

void init_pin();

#ifdef LUISANT
#define MQTT_SERVER "192.168.0.184"

#endif

#ifdef BETA
#define MQTT_SERVER "10.11.12.10" 
#endif







#endif