
#ifndef MY_FUNCTIONS_I
#define MY_FUNCTIONS_I
#include <PubSubClient.h>

PubSubClient& getMqttClient();
extern unsigned last_pir ;
extern int consigne;
extern byte mode;
extern float gauss;
extern short calib1;
extern short calib2;

void check_pin_button();


#endif