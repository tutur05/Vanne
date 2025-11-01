#include "moteur.h"
#include "config.h"
#include "scheduler.h"

extern short TcalibOuvrir;
extern short TcalibFermer;
extern bool vanne_mouvO;
extern bool vanne_mouvF;
extern Task t6;
extern Task t7;
extern String message1;
extern String message2;
extern bool etat_vanne;
unsigned long timer1 = 0;
extern short Tcalib;

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

    if ((millis() - timer1) >= (unsigned long)(TcalibFermer * 10))
    {
      vanneOff();
      vanne_mouvF = false;
      etat_vanne = false; // La vanne a fini de se fermer, on apsse à false

      message2 = "Vanne fermée";

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

    if ((millis() - timer1) >= (unsigned long)(TcalibOuvrir * 10))
    {
      timer1 = 0;
      vanneOff();
      etat_vanne = true; // La vanne a fini de s'ovurir, on apsse à true
      vanne_mouvO = false;
      message2 = "Vanne ouverte";

      t6.disable(); // On désactive la vérification de la vanne
    }
    else
      vanne_mouvO = true;
  }
  else
    t6.disable(); // On désactive la vérification de la vanne
}

void calib_moteur()
{
delay(1000);
  if (analogRead(A0) > 500) // Si vanne en butée
  {
    vanneF();
    delay(Tcalib*10);
    vanneOff();
    etat_vanne = true;
  }
  else //SINON ON TAPE LA BUTEE PUIS ON CALIB 
  {
    // OUVERTURE VANNE JUSQUA BUTEE
    while (analogRead(A0) < 100)
    {
      vanneO();
      delay(150);
    vanneOff(); 
    delay(25);

    }
    vanneF();
    delay(Tcalib*10);
    vanneOff();
    etat_vanne = true;
  }
  message1 = "Calib end";
}
