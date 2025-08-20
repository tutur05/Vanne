#include "moteur.h"
#include "config.h"
#include "scheduler.h"
extern float gauss;
extern short min_calibOuvrir;
extern short max_calibFermer;
extern bool vanne_mouvO;
extern bool vanne_mouvF;
extern Task t6;
extern Task t7;
extern String message2;
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

void checkMouvVanneF() //Fonction qui vérifie quand arrêter la vanne
{
  if(t7.isFirstIteration())
  vanneF();

  if(gauss >= max_calibFermer)
  {
    vanneOff();
    vanne_mouvF = false;
    message2 = "checkVF";    
  
    t7.disable(); // On désactive la vérification de la vanne
  }
  else
  vanne_mouvF = true;

}
void checkMouvVanneO() //Fonction qui vérifie quand arrêter la vanne
{
  if(t6.isFirstIteration())
  vanneO();

  if(gauss <= min_calibOuvrir)
  {
    vanneOff();
    vanne_mouvO = false;
    message2 = "checkVO";    
  
    t6.disable(); // On désactive la vérification de la vanne
  }
  else
  vanne_mouvO = true;
}

