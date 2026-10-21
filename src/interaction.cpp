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
extern PubSubClient client;
extern int consigne_effective;
// Ajuste un temps de calibration (borné) et le sauvegarde en flash seulement s'il change
static void ajuster_calib(short &val, short delta, const char *cle)
{
    short n = val + delta;
    if (n < CALIB_MIN) n = CALIB_MIN;
    if (n > CALIB_MAX) n = CALIB_MAX;
    if (n == val)
        return;
    val = n;
    backup.begin("mon-app", false);
    backup.putShort(cle, val);
    backup.end();
}

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

        if (!t4.isEnabled() && !calib_en_cours()) // t4 est relancée par la fin de calibration
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

    static bool lastPir = false;
    static unsigned long lastPirPub = 0;
    bool pir = digitalRead(PIN_PIR);
    if (pir)
    {
        last_pir = millis();
        // Publication sur front montant, puis au plus toutes les 30 s tant que le PIR reste actif
        if ((!lastPir || millis() - lastPirPub > 30000) && client.connected())
        {
            client.publish(NOM_VANNE "/PIR", "1");
            lastPirPub = millis();
        }

        if (mode == 2 && !t4.isEnabled() && !calib_en_cours()) // Activation du thermostat si mouvement détecté
            t4.enable();
    }
    lastPir = pir;
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
                    ajuster_calib(TcalibOuvrir, 10, "TcalibOuvrir");
                }
                else if (mode == 5)
                {
                    ajuster_calib(TcalibFermer, 10, "TcalibFermer");
                }
                else if (mode == 6)
                {
                    ajuster_calib(Tcalib, 10, "Tcalib");
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
                    ajuster_calib(TcalibOuvrir, -10, "TcalibOuvrir");
                }
                else if (mode == 5)
                {
                    ajuster_calib(TcalibFermer, -10, "TcalibFermer");
                }
                else if (mode == 6)
                {
                    ajuster_calib(Tcalib, -10, "Tcalib");
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
