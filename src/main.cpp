#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <Preferences.h>

#include "init_lib.h"
#include "init_display.h"
#include "moteur.h"
#include "config.h"
#include "interaction.h"
#include "scheduler.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);

byte mode = 0; // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
int consigne = 20; // Valeur par défaut au démarrage
const char *ssid2 = SSID2;
const char *password2 = PASSWIFI2;
const char *ssid = SSID1;
const char *password = PASSWIFI1;
float temperature = 0;
float humidity = 0;
unsigned last_menu = 0;
float gauss = 0;
short TcalibFermer;
short TcalibOuvrir;
short Tcalib;
bool etat_vanne = false; // false = VANNE FERMEE, true = VANNE OUVERTE
bool vanne_mouvO = false;
bool vanne_mouvF = false;

unsigned long last_pir = 0;

const unsigned long DELAI_EXTINCTION = 60000; // 10 secondes en millisecondes

// Variable d'état pour savoir si l'écran est allumé ou non
bool powerLCD = true;
extern String message1;
byte mode_max = 6; // Menu accessible
Preferences backup;
extern Task t4; // Tâche de régulation du thermostat

void setup()
{
  init_moteur();

  pinMode(15, OUTPUT);

  analogWrite(15, 125);

  message1 = "Init";

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    // Serial.println(F("SSD1306 allocation failed"));
  }

  backup.begin("mon-app", false); // false = mode lecture/écriture
  TcalibOuvrir = backup.getShort("TcalibOuvrir", 200);//Valeur pour salle poly
  TcalibFermer = backup.getShort("TcalibFermer", 200);//Valeur pour salle poly
  Tcalib = backup.getShort("Tcalib", 420); //Valeur pour salle poly
  backup.end(); // Ferme l'accès aux préférences

  update_display();

  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_BTNUP, INPUT_PULLUP);
  pinMode(PIN_BTNMID, INPUT_PULLUP);
  pinMode(PIN_BTNDOWN, INPUT_PULLUP);
  pinMode(A0, INPUT); // Pin A0 pour la sonde magnetique
  init_bme280();

  init_OTA();

  // Serial.begin(115200);
  // pinMode(LED, OUTPUT);


  mqtt_setup();
  mqtt_reconnect();

  init_scheduler();


}

void loop()
{
  //            vanneOff();

  runner.execute();

  if (millis() < 3000000) // 5 minutes pour faire la MAJ de l'OTA
    ArduinoOTA.handle();
  else
    mode_max = 2; // Menu accessible seulement les 5 minutes suivant le boot

  if (millis() - last_pir > DELAI_EXTINCTION)
  {
    if (powerLCD)
    {
      display.ssd1306_command(SSD1306_DISPLAYOFF); // ...on l'éteint
      powerLCD = false;                            // ...et on mémorise son nouvel état
    }
  }
  else
  {
    if (!powerLCD)
    {
      display.ssd1306_command(SSD1306_DISPLAYON); // ...on le rallume
      powerLCD = true;                            // ...et on mémorise son nouvel état
    }
  }
  check_pin_button();
}