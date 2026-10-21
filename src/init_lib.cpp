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
extern unsigned long last_interaction;
extern unsigned last_menu;
extern Task t1;
extern Task t3;
extern Task t4;
extern Task t5;
extern Task t6;
extern Task t7;
extern Task t9;
extern Adafruit_SSD1306 display;
extern String message1;
extern String message2;
extern float gauss;
int consigne_effective;

#ifndef DELAI_MANUEL
#define DELAI_MANUEL 14400000UL // 4h : retour automatique en AUTO après un réglage manuel à distance
#endif
static bool manuel_distant = false;
static unsigned long manuel_depuis = 0;
static bool ota_demarre = false;
bool ntp = false;    // heure recuperé ?
Adafruit_BME280 bme; // définition de la variable globale

// --- Variables pour la gestion Wi-Fi asynchrone ---
enum WifiState
{
  WIFI_IDLE,
  WIFI_CONNECTING_SSID1,
  WIFI_CONNECTING_SSID2,
  WIFI_CONNECTED
};
WifiState wifi_state = WIFI_IDLE;
unsigned long wifi_connect_start_time = 0;
const unsigned long WIFI_CONNECT_TIMEOUT = 10000; // 10 secondes de timeout par SSID
// ----------------------------------------------------

void callback(char *topic, byte *payload, unsigned int length)
{
  // Crée un buffer pour le payload et le termine par un caractère nul
  char buffer[length + 1];
  memcpy(buffer, payload, length);
  buffer[length] = '\0';

  // Vérifie si le message vient du bon topic
  char *fin;
  long val = strtol(buffer, &fin, 10);
  bool valide = (fin != buffer && *fin == '\0'); // Refuse le texte et les valeurs vides

  if (strcmp(topic, NOM_VANNE "/SetConsigne") == 0) // MAJ consigne
  {
    if (!valide || val < 5 || val > 30)
    {
      client.publish(NOM_VANNE "/ack", "ERR");
      return;
    }
    consigne = val;                          // Met à jour la consigne
    manuel_distant = true;                   // Retour auto en AUTO après DELAI_MANUEL
    manuel_depuis = millis();
    mode = 0;                                // Passe en mode manuel lors de la mise à jour de la consigne
    client.publish(NOM_VANNE "/ack", "OK2"); // Envoie un accusé de réception
    t4.forceNextIteration();                 // Force une régulation immédiate
    update_display();                        // Rafraîchit l'affichage pour montrer la nouvelle valeur
    t9.forceNextIteration();                 // Force une publication MQTT immédiate
  }

  else if (strcmp(topic, NOM_VANNE "/SetMode") == 0) // MAJ mode
  {
    if (!valide || val < 0 || val > 2) // Les modes 3 à 6 (tests/calibration) restent réservés aux boutons
    {
      client.publish(NOM_VANNE "/ack", "ERR");
      return;
    }
    mode = val;
    manuel_distant = (mode == 0);
    manuel_depuis = millis();
    client.publish(NOM_VANNE "/ack", "OK3"); // Envoie un accusé de réception
    t4.forceNextIteration();                 // Force une régulation immédiate

    update_display(); // Rafraîchit l'affichage pour montrer la nouvelle valeur
  }
}

// Repasse en AUTO si un réglage manuel distant dure trop longtemps
void check_manuel_distant()
{
  if (manuel_distant && mode == 0 && millis() - manuel_depuis > DELAI_MANUEL)
  {
    mode = 2;
    manuel_distant = false;
    t4.forceNextIteration();
    update_display();
  }
}

bool init_bme280()
{
  return bme.begin(0x76); // ou 0x77 selon ton câblage
}

void init_OTA()
{
  ArduinoOTA.setHostname(NOM_VANNE); // Même nom que le mDNS (upload_port de platformio.ini)
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
  // ArduinoOTA.begin() est appelé dans manage_wifi() une fois l'IP obtenue
}

void mqtt_setup()
{
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

bool mqtt_reconnect()
{
  // Tente de se connecter au broker MQTT
  // Serial.print("Attempting MQTT connection...");
  if (client.connect(NOM_VANNE))
  {
    // Serial.println("connected");
    // Une fois connecté, s'abonne au topic.
    client.subscribe(NOM_VANNE "/SetConsigne");
    client.subscribe(NOM_VANNE "/SetMode");

    client.loop(); // Traite immédiatement les messages en attente
  }

  return client.connected();
}

PubSubClient &getMqttClient()
{
  return client;
}

void check_connection()
{
  if (millis() - last_interaction > DELAI_INTERACTION)
  {                // SI PIR PAS RECENT
    manage_wifi(); // On utilise le nouveau gestionnaire non-bloquant

    if (WiFi.status() != WL_CONNECTED)
    {
      return; // Sortir si le WiFi n'est pas connecté
    }

    if (!client.connected())
    {
      mqtt_reconnect(); // Tente de se reconnecter si la connexion MQTT est perdue
    }

    // Ne publie que si la connexion est active
  }
}

void pub_mqtt()
{
  if (millis() - last_interaction > DELAI_INTERACTION)
  { // SI PIR PAS RECENT
    if (client.connected() && WiFi.status() == WL_CONNECTED)
    {
      client.publish(NOM_VANNE "/T", String(temperature, 2).c_str());
      client.publish(NOM_VANNE "/H", String(humidity, 2).c_str());
      
      client.publish(NOM_VANNE "/Mode", String(mode).c_str());
      client.publish(NOM_VANNE "/LastPIR", String(last_pir/1000).c_str());

      if(mode==2)
      client.publish(NOM_VANNE "/Consigne", String(consigne_effective).c_str());
      else
      client.publish(NOM_VANNE "/Consigne", String(consigne).c_str());
    }
  }
}

void holdMQTT_Online() // maintient de liaison MQTT
{
  if (WiFi.status() == WL_CONNECTED)
    getMqttClient().loop(); // maintient le mqtt en ligne (1s max entre 2 loop)
}

void regul_therm() // régulation de la vanne + lecture sonde
{
  if (calib_en_cours())
    return; // La calibration pilote le moteur, on ne régule pas en même temps

  if (millis() - last_interaction > DELAI_INTERACTION)
  {

    temperature = bme.readTemperature();
    humidity = bme.readHumidity();

    // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
    if (mode == 0)
    {
      if (consigne > temperature)
      {

        // VANNE ON
        t6.enable();
      }
      else // Il faut fermer la vanne
      {

        // VANNE OFF
        // vanneF();
        t7.enable();
      }
    }
    if (mode == 1)
    {

      if (16 > temperature)
      {
        t6.enable(); // VANNE ON
      }
      else
      {
        t7.enable(); // VANNE OFF
      }
    }
    if (mode == 2)
    {
      consigne_effective = consigne; // On utilise une variable ne pas toucher à consigne 

      if (millis() - last_pir < DELAI_PIR) // SI PIR RECENT
      {
        consigne_effective = CONSIGNE; // On utilise la consigne confort
      }
      else
        consigne_effective = 16; // CONSIGNE ECO

      if (consigne_effective > temperature)
      {
        t6.enable();
      }
      else // Il faut fermer la vanne
      {
        t7.enable();
      }
    }
  }
}

void manage_wifi()
{
  // Si on est déjà connecté, on met à jour l'IP et on sort.
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!ntp)
    {
      configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org"); // Syncro internet poru heure
      ntp = true;
    }
    if (wifi_state != WIFI_CONNECTED)
    {
      // On vient de se connecter




      IPAddress ip = WiFi.localIP();
      message1 = ip.toString().c_str();
      wifi_state = WIFI_CONNECTED;

      if (!ota_demarre) // OTA + mDNS démarrent une seule fois, quand le réseau est prêt
      {
        ArduinoOTA.begin();
        ota_demarre = true;
      }


    }
    return;
  }

  // Si on n'est pas connecté, on passe à l'état IDLE pour relancer le processus.
  if (wifi_state == WIFI_CONNECTED)
  {
    message1 = "WiFi Perdu";
    wifi_state = WIFI_IDLE;
  }

  // Machine à états pour la connexion
  switch (wifi_state)
  {
  case WIFI_IDLE:
    // On lance la première tentative de connexion
    message1 = "Connexion...";
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.hostname(NOM_VANNE);
    WiFi.begin(ssid, password);
    wifi_state = WIFI_CONNECTING_SSID1;
    wifi_connect_start_time = millis();
    break;

  case WIFI_CONNECTING_SSID1:
    // On attend la connexion au SSID 1, mais sans bloquer
    if (millis() - wifi_connect_start_time > WIFI_CONNECT_TIMEOUT)
    {
      // Le timeout est dépassé, on essaie le SSID 2
      message1 = "SSID2...";
      WiFi.disconnect();
      WiFi.begin(ssid2, password2);
      wifi_state = WIFI_CONNECTING_SSID2;
      wifi_connect_start_time = millis();
    }
    break;

  case WIFI_CONNECTING_SSID2:
    // On attend la connexion au SSID 2
    if (millis() - wifi_connect_start_time > WIFI_CONNECT_TIMEOUT)
    {
      // Le timeout est dépassé, on retourne à l'état IDLE pour une nouvelle tentative plus tard.
      message1 = "WiFi Echec";
      wifi_state = WIFI_IDLE;
    }
    break;

  case WIFI_CONNECTED:
    // Ne devrait jamais arriver ici car géré au début de la fonction.
    // Mais on le traite pour éviter un warning et par sécurité.
    break;
  }
}

void checkHorloge()
{
  // Vérifie si le WiFi est connecté avant de synchroniser l'heure
  if (WiFi.status() == WL_CONNECTED)
  {

    struct tm timeinfo;

    // Tente de récupérer l'heure actuelle locale
    if (!getLocalTime(&timeinfo))
    {
      // Si la synchro échoue (ex: coupure internet), on affiche un message
      return;
    }

    // Formatage et affichage de l'heure
    char timeStringBuff[50];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%A %d %B %Y - %H:%M:%S", &timeinfo);

    // Condition l'heure que vous voulez déclencher
    if (timeinfo.tm_hour == 01 && timeinfo.tm_min == 00)
    {
      message2 = "Calib Moteur...";
      start_calib_moteur();           // Lance la calibration non bloquante
      t5.disable();                   // Désactive cette tâche après exécution unique
      t5.enableDelayed(23 * 3600000); // Réactive la tâche après 23 heures pour éviter les répétitions
    }
  }

}