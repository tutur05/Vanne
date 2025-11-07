#include "scheduler.h"

// On inclut la bibliothèque COMPLÈTE. C'est la "cuisine".
// C'est nécessaire car nous allons créer (définir) des objets Task et Scheduler ici.
#include <TaskScheduler.h>

// On inclut les autres en-têtes nécessaires pour les fonctions de rappel (callbacks).
#include "init_lib.h"
#include "init_display.h"
#include "config.h"

Scheduler runner;

// Déclarations des fonctions de rappel qui sont définies ailleurs.
void check_connection();
void holdMQTT_Online();
void regul_therm();
void update_display();
// void ReadCapteur(); // Exemple d'une tâche supplémentaire
void checkMouvVanneO(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void checkMouvVanneF(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void calib_moteur();

// Définition des tâches.
Task t1(10000, TASK_FOREVER, &check_connection);
Task t2(1000, TASK_FOREVER, &holdMQTT_Online); // Maintiens MQTT
Task t3(2000, TASK_FOREVER, &update_display);
Task t4(DELAI_REGUL, TASK_FOREVER, &regul_therm); // Fonction qui lance un controle de temp + action de thermostat
Task t5(58000, TASK_FOREVER, &checkHorloge);      // Vérifie toutes les minutes si une commande doit être déclenchée
Task t6(1000, TASK_FOREVER, &checkMouvVanneO);    // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
Task t7(1000, TASK_FOREVER, &checkMouvVanneF);    // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
Task t8(100, TASK_FOREVER, &calib_moteur);
Task t9(DELAI_PUB_MQTT, TASK_FOREVER, &pub_mqtt); // Publication des valeurs MQTT toutes les 10min
void init_scheduler()
{
  runner.init();

  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);
  runner.addTask(t5);
  runner.addTask(t6);
  runner.addTask(t7);
  runner.addTask(t8);
  t8.disable(); // Désactivée par défaut
  runner.addTask(t9);

  t2.enableDelayed(5000);  // MQTT (délai réduit pour un traitement plus rapide)
  t3.enableDelayed(5000);  // Controle de l'heure
  t4.enableDelayed(10000); // THERMOSTAT
  t5.enableDelayed(15000); // Check heure
  t9.enableDelayed(20000); // Publication MQTT
}