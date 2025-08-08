// scheduler.cpp
#include "scheduler.h"

// On inclut la bibliothèque COMPLÈTE. C'est la "cuisine".
// C'est nécessaire car nous allons créer (définir) des objets Task et Scheduler ici.
#include <TaskScheduler.h>

// On inclut les autres en-têtes nécessaires pour les fonctions de rappel (callbacks).
#include "init_lib.h"
#include "init_display.h"

// DÉFINITION de la variable 'runner'. Pas de 'extern' ici, on la crée pour de vrai.
Scheduler runner;

// Déclarations des fonctions de rappel qui sont définies ailleurs.
void t1Callback();
void t2Callback();
void regul_therm();
void update_display();

// Définition des tâches.
Task t1(30000, TASK_FOREVER, &t1Callback);
Task t2(1000, TASK_FOREVER, &t2Callback);
Task t3(2000, TASK_FOREVER, &update_display);
Task t4(5000, TASK_FOREVER, &regul_therm);

// Définition de la fonction d'initialisation.
void init_scheduler()
{
  runner.init();
  
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);

  t1.enable();
  t2.enable();
  t3.enable();
  t4.enable();
}