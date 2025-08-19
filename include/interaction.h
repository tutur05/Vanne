
#ifndef MY_FUNCTIONS_I
#define MY_FUNCTIONS_I
#include <PubSubClient.h>

PubSubClient& getMqttClient();
extern unsigned long last_pir ;
extern int consigne;
extern byte mode;
extern float gauss;
extern short max_calibFermer;
extern short min_calibOuvrir;
extern byte mode_max; //Menu prog  accessible
void check_pin_button();


#endif