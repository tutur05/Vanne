#include "moteur.h"
#include "config.h"
#include "scheduler.h"
#include "init_display.h"

extern short TcalibOuvrir;
extern short TcalibFermer;
extern bool vanne_mouvO;
extern bool vanne_mouvF;
extern Task t1;
extern Task t4;
extern Task t6;
extern Task t7;
extern Task t8;
extern String message1;
extern String message2;
extern bool etat_vanne;
unsigned long timer1 = 0;
extern short Tcalib;

enum CalibState {
    CALIB_IDLE,
    CALIB_START,
    CALIB_CLOSING_FROM_OPEN_BUTEE,
    CALIB_SEARCHING_OPEN_BUTEE,
    CALIB_PAUSE_BEFORE_CLOSING,
    CALIB_CLOSING_FOR_CALIBRATION,
    CALIB_DONE,
    CALIB_ERROR
};

static CalibState calibState = CALIB_IDLE;
static unsigned long calibTimer = 0;

// Durée en ms, bornée : une valeur flash corrompue ne peut pas laisser le moteur tourner indéfiniment
static unsigned long dureeMs(short t)
{
  if (t < CALIB_MIN) t = CALIB_MIN;
  if (t > CALIB_MAX) t = CALIB_MAX;
  return (unsigned long)t * 10;
}

bool calib_en_cours()
{
  return calibState != CALIB_IDLE;
}
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

    if ((millis() - timer1) >= dureeMs(TcalibFermer))
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

    if ((millis() - timer1) >= dureeMs(TcalibOuvrir))
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

void start_calib_moteur() {
    if (calibState != CALIB_IDLE)
      return; // Déjà en cours
    t6.disable(); // Plus de mouvement de régulation pendant la calibration
    t7.disable();
    vanneOff();
    calibState = CALIB_START;
    t8.enable(); // On active la tâche de calibration
}

void calib_moteur()
{
  const unsigned long CALIBRATION_TIMEOUT = 20000; // 15 secondes de sécurité

  switch (calibState) {
    case CALIB_IDLE:
      // Ne fait rien, attend d'être démarré
      break;

    case CALIB_START:
      message1 = "Calibration...";
      update_display();
      if (analogRead(A0) > 500) { // Si vanne déjà en butée ouverte
        calibState = CALIB_CLOSING_FROM_OPEN_BUTEE;
        message1 = "Fermeture...";
        update_display();
        vanneF();
        calibTimer = millis();
      } else { // Sinon, on cherche la butée ouverte
        calibState = CALIB_SEARCHING_OPEN_BUTEE;
        message1 = "Rech. butee O";
        update_display();
        vanneO();
        calibTimer = millis();
      }
      break;

    case CALIB_CLOSING_FROM_OPEN_BUTEE:
      if (millis() - calibTimer >= dureeMs(Tcalib)) {
        vanneOff();
        etat_vanne = false; // La vanne est maintenant considérée comme fermée
        calibState = CALIB_DONE;
      }
      break;

    case CALIB_SEARCHING_OPEN_BUTEE:
      if (analogRead(A0) >= 100) { // Butée ouverte atteinte
        vanneOff();
        message1 = "Butee O. OK";
        update_display();
        calibState = CALIB_PAUSE_BEFORE_CLOSING;
        calibTimer = millis();
      } else if (millis() - calibTimer > CALIBRATION_TIMEOUT) {
        calibState = CALIB_ERROR;
      }
      break;

    case CALIB_PAUSE_BEFORE_CLOSING:
      if (millis() - calibTimer >= 100) {
        calibState = CALIB_CLOSING_FOR_CALIBRATION;
        message1 = "Fermeture...";
        update_display();
        vanneF(); // On ferme complètement la vanne
        calibTimer = millis();
      }
      break;

    case CALIB_CLOSING_FOR_CALIBRATION:
      if (millis() - calibTimer >= dureeMs(Tcalib)) {
        vanneOff();
        etat_vanne = false; // La vanne est maintenant considérée comme fermée
        calibState = CALIB_DONE;
      }
      break;

    case CALIB_DONE:
      message1 = "Calib terminee";
      update_display();
      calibState = CALIB_IDLE;
      t8.disable(); // Calibration terminée, on désactive la tâche
      t1.enableDelayed(1000); //Activation Wifi
      t4.enableDelayed(2000); // (Re)lance la régulation maintenant que la vanne est à une position connue

      break;

    case CALIB_ERROR:
      vanneOff();
      message1 = "Erreur Calib";
      update_display();
      calibState = CALIB_IDLE;
      t8.disable(); // On arrête en cas d'erreur
      t1.enableDelayed(1000); //Activation Wifi
      t4.enableDelayed(2000);
      break;
  }
}
