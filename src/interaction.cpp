#include "interaction.h"
#include "config.h"
#include "init_display.h"
#include "moteur.h"
#include <Arduino.h>
#include "scheduler.h"
#include <Preferences.h>
extern byte mode;
extern Task t1;
extern Task t4;
// extern Task t5;
extern Task t6;
extern Task t7;
extern Preferences backup;
extern String message1;
extern String message2;
extern bool powerLCD;
extern unsigned long last_pir;
extern unsigned long last_interaction;
void check_pin_button()
{
    if (mode > 2) // Désactivation du thermostat le temps des tests
    {

        if (t4.isEnabled())
        {

            t4.disable();
            vanneOff();
        }
    }
    else
    {

        if (!t4.isEnabled())
        {
            t4.enableDelayed(2000); // DELAI POUR EVITER CONFLIT AVEC BTN
        }
    }
    if (mode >= 3)
    {

        // Desactivatop, des fonctions auto
        if (t6.isEnabled())
        {
            t6.disable();
            vanneOff();
        }
        if (t7.isEnabled())
        {
            t7.disable();
            vanneOff();
        }
    }

    if (digitalRead(PIN_PIR))
    {
        last_pir = millis();
        if (mode == 2 && !t4.isEnabled()) // Activation du thermostat si mouvement détecté
            t4.enable();
    }
        static bool lastUp = HIGH, lastSel = HIGH, lastDown = HIGH;

        // Lecture boutons
        bool up = digitalRead(PIN_BTNUP);
        bool sel = digitalRead(PIN_BTNMID);
        bool down = digitalRead(PIN_BTNDOWN);

        if (up == LOW || sel == LOW || down == LOW)
        {
            last_interaction = millis();
        }

        if (powerLCD) // Si écran allumé
        {

            // 0 MANUEL // 1 ECO // 2 AUTO (PIR) // 3 Moteur
            // Détection front descendant
            if (up == LOW && lastUp == HIGH) // BTN UP PRESSE
            {
                // Serial.println("up");

                if (mode == 0)
                {
                    consigne = consigne + 1;
                }
                else if (mode == 3)
                {

                    vanneO();
                    // Serial.println("Vanne1 ON");
                }
                else if (mode == 4)
                {
                    if (TcalibOuvrir > 50) // On augmente que si il y a une marge de 10u
                    {
                        backup.begin("mon-app", false); // false = mode lecture/écriture
                        TcalibOuvrir = TcalibOuvrir + 10;
                        backup.putShort("TcalibOuvrir", TcalibOuvrir);
                        backup.end(); // Ferme l'accès aux préférences
                    }
                }
                else if (mode == 5)
                {

                    backup.begin("mon-app", false); // false = mode lecture/écriture
                    TcalibFermer = TcalibFermer + 10;
                    backup.putShort("TcalibFermer", TcalibFermer);
                    backup.end(); // Ferme l'accès aux préférences
                }
                else if (mode == 6)
                {

                    backup.begin("mon-app", false); // false = mode lecture/écriture
                    Tcalib = Tcalib + 10;
                    backup.putShort("Tcalib", Tcalib);
                    backup.end(); // Ferme l'accès aux préférences
                }

                update_display();
            }

            if (sel == LOW && lastSel == HIGH) // Bacule du mode via btn milieu
            {

                if (mode >= mode_max)
                {
                    mode = 0;
                }
                else
                    mode = mode + 1;

                update_display();
            }

            if (down == LOW && lastDown == HIGH) // BTN DOWN PRESSE
            {
                // Serial.println("down");

                if (mode == 0)
                {
                    consigne = consigne - 1;
                }
                else if (mode == 3)
                {
                    vanneF();
                }
                else if (mode == 4)
                {

                    backup.begin("mon-app", false); // false = mode lecture/écriture

                    TcalibOuvrir = TcalibOuvrir - 10;
                    backup.putShort("TcalibOuvrir", TcalibOuvrir);
                    backup.end(); // Ferme l'accès aux préférences
                }
                else if (mode == 5)
                {
                    if (TcalibFermer > 50)
                    {
                        backup.begin("mon-app", false); // false = mode lecture/écriture

                        TcalibFermer = TcalibFermer - 10;
                        backup.putShort("TcalibFermer", TcalibFermer);
                        backup.end(); // Ferme l'accès aux préférences
                    }
                }
                else if (mode == 6)
                {
                    if (Tcalib > 50)
                    {
                        backup.begin("mon-app", false); // false = mode lecture/écriture

                        Tcalib = Tcalib - 10;
                        backup.putShort("Tcalib", Tcalib);
                        backup.end(); // Ferme l'accès aux préférences
                    }
                }

                update_display();
            }

            if (mode == 3) // Extinction des vannes lors du relachement du button OU trop proche
            {

                if (up == HIGH && lastUp == LOW) // Si on relache le bouton
                {
                    // Serial.println("up relache");
                    vanneOff();
                }
                if (down == HIGH && lastDown == LOW) // Si on relache le bouton BAS
                {
                    // Serial.println("down relache");
                    vanneOff();
                }
            }

            lastSel = sel;
            lastUp = up;
            lastDown = down;
        }
    }
