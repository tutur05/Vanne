
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
void start_calib_moteur();
bool calib_en_cours(); // true tant que la calibration n'est pas terminée

#define CALIB_MIN 50   // Bornes des temps de calibration (x10 ms)
#define CALIB_MAX 1000

#endif