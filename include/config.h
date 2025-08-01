
#ifndef MY_FUNCTIONS_C
#define MY_FUNCTIONS_C

#define LED 2 // GPIO2 pour la led constructeur
#define PIN_VANNE1 16
#define PIN_VANNE0 15
#define PIN_PIR 13 // D7
#define PIN_BTNUP 12
#define PIN_BTNMID 14
#define PIN_BTNDOWN 2 //D4

#define SEALEVELPRESSURE_HPA (1013.25)

#define SSID "Livebox-E300"
#define PASSWIFI "FdJuA4RYCYy5VtMYug"
#define SSID2 "Betamachine_Tec"
#define PASSWIFI2 "betatecissogood"
#ifdef LUISANT
#define MQTT_SERVER "192.168.0.184"

#endif

#ifdef BETA

#define MQTT_SERVER "10.11.12.10"
#endif




#endif