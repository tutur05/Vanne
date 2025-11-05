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
extern Adafruit_SSD1306 display;
extern String message1;
extern String message2;
extern float gauss;
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
  String payloadStr = String(buffer);

  // Vérifie si le message vient du bon topic
  if (strcmp(topic, NOM_VANNE "/SetConsigne") == 0) // MAJ consigne
  {
    consigne = payloadStr.toInt();           // Met à jour la consigne
    mode = 0;                                // Passe en mode manuel lors de la mise à jour de la consigne
    client.publish(NOM_VANNE "/ack", "OK2"); // Envoie un accusé de réception
    t4.forceNextIteration();                 // Force une régulation immédiate
    update_display();                        // Rafraîchit l'affichage pour montrer la nouvelle valeur
  }

  else if (strcmp(topic, NOM_VANNE "/SetMode") == 0) // MAJ mode
  {
    mode = payloadStr.toInt();               // Met à jour la consigne
    client.publish(NOM_VANNE "/ack", "OK3"); // Envoie un accusé de réception
    t4.forceNextIteration();                 // Force une régulation immédiate

    update_display(); // Rafraîchit l'affichage pour montrer la nouvelle valeur
  }
}

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
  if ((last_interaction + DELAI_PIR2) < millis())
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
  if ((last_interaction + DELAI_PIR2) < millis())
  { // SI PIR PAS RECENT
    if (client.connected() && WiFi.status() == WL_CONNECTED)
    {
      client.publish(NOM_VANNE "/T", String(temperature, 2).c_str());
      client.publish(NOM_VANNE "/H", String(humidity, 2).c_str());
      client.publish(NOM_VANNE "/Consigne", String(consigne).c_str());
      client.publish(NOM_VANNE "/Mode", String(mode).c_str());
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

    if ((last_pir + DELAI_PIR) > millis()) // SI PIR RECENT
      consigne = CONSIGNE;
    else
      consigne = 16; // CONSIGNE ECO

    if (consigne > temperature)
    {
      t6.enable();
    }
    else // Il faut fermer la vanne
    {
      t7.enable();
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
      message1 = "Calib Moteur en cours";
      calib_moteur();                 // Appelle la fonction de calibration du moteur
      t5.disable();                   // Désactive cette tâche après exécution unique
      t5.enableDelayed(23 * 3600000); // Réactive la tâche après 23 heures pour éviter les répétitions
    }
  }
  else
  {
    message1 = "WiFi non connecté";
  }
}