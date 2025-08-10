#include "moteur.h"
#include "config.h"
extern float gauss;
extern short min_calibOuvrir;
extern short max_calibFermer;
void init_moteur()
{
  pinMode(PIN_VANNE_FERMER, OUTPUT);
  pinMode(PIN_VANNE_OUVRIR, OUTPUT);
  digitalWrite(PIN_VANNE_FERMER, LOW);
  digitalWrite(PIN_VANNE_OUVRIR, LOW);
}

void vanneF()
{


  digitalWrite(PIN_VANNE_FERMER, HIGH);
  digitalWrite(PIN_VANNE_OUVRIR, LOW);
  
}

void vanneO()
{

  digitalWrite(PIN_VANNE_FERMER, LOW);
  digitalWrite(PIN_VANNE_OUVRIR, HIGH);

  }

void vanneOff()
{
  digitalWrite(PIN_VANNE_FERMER, LOW);
  digitalWrite(PIN_VANNE_OUVRIR, LOW);
}