#include "interaction.h"
#include "config.h"
#include "init_display.h"
#include "moteur.h"
#include <Arduino.h>
#include "scheduler.h"
#include <Preferences.h>
extern byte mode;
extern Task t4;

extern Task t5;
extern Task t6;
extern Preferences backup;
extern String message2;
extern bool powerLCD;
extern unsigned long last_pir;

void check_pin_button()
{
    if (mode <= 2) // Désactivation du thermostat le temps des tests
    {
        if (t4.isEnabled())
        {
            t4.disable();
            vanneOff();
        }
    }
    else
        t4.enable();

    if (mode == 3) // Si mode 3, On controle la sonde magnétique
    {
        if (!t5.isEnabled())
        {
            t5.enable();
        }
    }
    else
    {
        if (t5.isEnabled())
        {
            t5.disable();
        }
    }

    //if (digitalRead(PIN_PIR))
    //    last_pir = millis();

    static bool lastUp = HIGH, lastSel = HIGH, lastDown = HIGH;

    // Lecture boutons
    bool up = digitalRead(PIN_BTNUP);
    bool sel = digitalRead(PIN_BTNMID);
    bool down = digitalRead(PIN_BTNDOWN);

    if (up == LOW || sel == LOW || down == LOW)
    {
        last_pir = millis();
    }

    if(powerLCD) // Si écran allumé
    {


    // 0 MANUEL // 1 ECO // 2 AUTO (PIR) // 3 Moteur
    // Détection front descendant
    if (up == LOW && lastUp == HIGH) // BTN UP PRESSE
    {
        // Serial.println("up");
        getMqttClient().publish("VanneV1/button", "Up");

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
            backup.begin("mon-app", false); // false = mode lecture/écriture
            min_calibOuvrir = min_calibOuvrir + 1;
            backup.putShort("min_calibOuvrir", min_calibOuvrir);
            backup.end(); // Ferme l'accès aux préférences
        }
        else if (mode == 5)
        {
            backup.begin("mon-app", false); // false = mode lecture/écriture
            max_calibFermer = max_calibFermer + 1;
            backup.putShort("max_calibFermer", max_calibFermer);
            backup.end(); // Ferme l'accès aux préférences
        }

        update_display();
    }

    if (sel == LOW && lastSel == HIGH) // Bacule du mode via btn milieu
    {

        getMqttClient().publish("VanneV1/button", "sel");

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

        getMqttClient().publish("VanneV1/button", "down");
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

            min_calibOuvrir = min_calibOuvrir - 1;
            backup.putShort("min_calibOuvrir", min_calibOuvrir);
            backup.end(); // Ferme l'accès aux préférences
        }
        else if (mode == 5)
        {
            backup.begin("mon-app", false); // false = mode lecture/écriture

            max_calibFermer = max_calibFermer - 1;
            backup.putShort("max_calibFermer", max_calibFermer);
            backup.end(); // Ferme l'accès aux préférences
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

        if (digitalRead(PIN_VANNE_FERMER) && gauss >= max_calibFermer)
        {
            // Serial.println("VanneFermer OFF car trop proche");
            vanneOff();
            message2 = "MaxFermer";
        }
        if (digitalRead(PIN_VANNE_OUVRIR) && gauss <= min_calibOuvrir)
        {
            // Serial.println("VanneOuvrir OFF car trop loin");
            vanneOff();
            message2 = "MinOuvert";
        }
    }

    lastSel = sel;
    lastUp = up;
    lastDown = down;
}





}