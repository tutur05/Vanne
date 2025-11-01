// scheduler.cpp
#include "scheduler.h"

// On inclut la bibliothèque COMPLÈTE. C'est la "cuisine".
// C'est nécessaire car nous allons créer (définir) des objets Task et Scheduler ici.
#include <TaskScheduler.h>

// On inclut les autres en-têtes nécessaires pour les fonctions de rappel (callbacks).
#include "init_lib.h"
#include "init_display.h"


Scheduler runner;

// Déclarations des fonctions de rappel qui sont définies ailleurs.
void check_connection();
void holdMQTT_Online();
void regul_therm();
void update_display();
//void ReadCapteur(); // Exemple d'une tâche supplémentaire
void checkMouvVanneO(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void checkMouvVanneF(); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
void calib_moteur();

// Définition des tâches.
Task t1(30000, TASK_FOREVER, &check_connection);
Task t2(1000, TASK_FOREVER, &holdMQTT_Online);// Maintiens MQTT
Task t3(2000, TASK_FOREVER, &update_display);
Task t4(20000, TASK_FOREVER, &regul_therm); //Fonction qui lance un controle de temp + action de thermostat
//Task t5(200, TASK_FOREVER, &ReadCapteur); 
Task t6(1000, TASK_FOREVER, &checkMouvVanneO); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
Task t7(1000, TASK_FOREVER, &checkMouvVanneF); // Vérifie si la vanne doit être arrêtée en fonction de la position du capteur magnétique
Task t8(0,1, &calib_moteur);

// Définition de la fonction d'initialisation.
void init_scheduler()
{
  runner.init();
  
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);
  //runner.addTask(t5); 
  runner.addTask(t6);
    runner.addTask(t7);
    runner.addTask(t8);

  t1.enableDelayed(8000);
  t2.enableDelayed(23000);//MQTT
  t3.enableDelayed(5000);
 t8.enable();
 
  t4.enableDelayed(10000); //THERMOSTAT

}