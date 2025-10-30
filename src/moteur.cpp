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
extern bool etat_vanne;
unsigned long timer1 = 0;
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

void checkMouvVanneF() // Fonction qui vérifie quand arrêter la vanne (lancer via t7)
{
  if (etat_vanne)
  {
    if (t7.isFirstIteration())
    {
      vanneF();
      message2 = "Fermeture vanne";
      timer1 = millis();
    }

    if ((millis() - timer1) >= (max_calibFermer * 10))
    {
      vanneOff();
      vanne_mouvF = false;
      etat_vanne = false; // La vanne a fini de se fermer, on apsse à false

      message2 = "checkVF";

      t7.disable(); // On désactive la vérification de la vanne
    }
    else
      vanne_mouvF = true;
  }
  else
  t7.disable(); // On désactive la vérification de la vanne
}
void checkMouvVanneO() // Fonction qui vérifie quand arrêter la vanne
{
  if (etat_vanne == false)
  {
    if (t6.isFirstIteration())
    {
      message2 = "Ouverture vanne";
      timer1 = millis();
      vanneO();
    }

    if ((millis() - timer1) >= (min_calibOuvrir * 10))
    {
      timer1 = 0;
      vanneOff();
      etat_vanne = true; // La vanne a fini de s'ovurir, on apsse à true
      vanne_mouvO = false;
      // message2 = String(timer1);

      t6.disable(); // On désactive la vérification de la vanne
    }
    else
      vanne_mouvO = true;
  }
  else
        t6.disable(); // On désactive la vérification de la vanne


}
/*
void init_moteur()
{
  gauss = analogRead(A0);
  if (gauss) // Si vanne en butée
  {
    //FERMETURE VANNE VIA  LE TEMPS DU CALIB
    //ETAT VANNE =  
  }
  else
  {
    //OUVERTURE VANNE JUSQUA BUTEE
    //FERMETURE VANNE TEMPS CALIB
  
  }
  
}
*/