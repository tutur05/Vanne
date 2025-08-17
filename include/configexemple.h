
#ifndef MY_FUNCTIONS_C
#define MY_FUNCTIONS_C

#define LED 2 // GPIO2 pour la led constructeur
#define PIN_VANNE_FERMER 3  // Pin RX (on utilise pas sérial)
#define PIN_VANNE_OUVRIR 16 //D0
#define PIN_PIR 13 // D7
#define PIN_BTNUP 12 //D6
#define PIN_BTNMID 0 // D3
#define PIN_BTNDOWN 2 //D4
#define PIN_1WIRE 14 //Pas encore teste
#define PIN_PWM 15 //Pas encore teste

#define SEALEVELPRESSURE_HPA (1013.25)

#define SSID ""
#define PASSWIFI ""
#define SSID2 ""
#define PASSWIFI2 ""
#ifdef LUISANT
#define MQTT_SERVER "192.168.0.184"

#endif

#ifdef BETA

#define MQTT_SERVER "10.11.12.10"
#endif




#endif