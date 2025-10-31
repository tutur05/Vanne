
#ifndef MOTEUR_H
#define MOTEUR_H
#include <Arduino.h>
void vanneF();
void vanneO();

void init_moteur();
void vanneOff();

void  checkMouvVanneO(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void  checkMouvVanneF(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void calib_moteur();

#endif