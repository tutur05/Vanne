
#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H



#include <PubSubClient.h>
#include <WiFiClient.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>





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



void check_pin_button();









#endif