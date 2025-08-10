#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>

#include "init_lib.h"
#include "init_display.h"
#include "moteur.h"
#include "config.h"
#include "interaction.h"
#include "scheduler.h" 


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
short max_calibFermer = 555;
short min_calibOuvrir = 519;

extern String message1;
byte mode_max = 5 ; //Menu accessible



void setup()
{
message1 = "Connecting";

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


  init_moteur();
        init_scheduler();
}

void loop()
{

  runner.execute();

  if(millis() < 3000000) // 5 minutes pour faire la MAJ de l'OTA
  ArduinoOTA.handle();
  else
 mode_max = 2; //Menu accessible seulement les 5 minutes suivant le boot


 check_pin_button();
}