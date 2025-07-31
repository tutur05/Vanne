
#ifndef MY_FUNCTIONS_I
#define MY_FUNCTIONS_I
#include <Arduino.h>
#include <PubSubClient.h>

PubSubClient& getMqttClient();
extern unsigned last_pir ;
extern int consigne;
extern byte mode;

void init_pin();


#endif