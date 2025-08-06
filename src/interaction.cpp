#include "interaction.h"
#include "config.h"
#include "init_display.h"
#include "moteur.h"
#include <Arduino.h>

void check_pin_button()
{
    if (digitalRead(PIN_PIR))
        last_pir = millis();

    static bool lastUp = HIGH, lastSel = HIGH, lastDown = HIGH;

    // Lecture boutons
    bool up = digitalRead(PIN_BTNUP);
    bool sel = digitalRead(PIN_BTNMID);
    bool down = digitalRead(PIN_BTNDOWN);

    // 0 MANUEL // 1 ECO // 2 AUTO (PIR) // 3 Moteur
    // Détection front descendant
    if (up == LOW && lastUp == HIGH) // BTN UO PRESSE
    {
        Serial.println("up");
        getMqttClient().publish("VanneV1/button", "Up");

        if (mode == 0)
        {
            consigne = consigne + 1;
        }
        else if (mode == 3)
        {

            vanneO();
            Serial.println("Vanne1 ON");
              
        }
        else if (mode == 4)
        {
            calib1 = calib1 + 1;
        }
        else if (mode == 5)
        {
            calib2 = calib2 + 1;
        }

        update_display();
    }

    if (sel == LOW && lastSel == HIGH) // Bacule du mode via btn milieu
    {
        Serial.println("sel");

        getMqttClient().publish("VanneV1/button", "sel");

        if (mode >= 5)
        {
            mode = 0;
        }
        else
            mode = mode + 1;

        update_display();
    }

    if (down == LOW && lastDown == HIGH) // BTN DOWN PRESSE
    {
        Serial.println("down");

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
            calib1 = calib1 - 1;
        }
        else if (mode == 5)
        {
            calib2 = calib2 - 1;
        }

        update_display();
    }

    if (mode == 3) // Extinction des vannes lors du relachement du button
    {
        gauss = analogRead(A0); // Lecture de la sonde magnétique

        if (up == HIGH && lastUp == LOW) // Si on relache le bouton 
        {
            Serial.println("up relache");
            vanneOff();
        }

        if (down == HIGH && lastDown == LOW ) // Si on relache le bouton 
        {
            Serial.println("down relache");
            vanneOff();
        }
        if(digitalRead(PIN_VANNE1) && gauss < calib1)
        {
            Serial.println("Vanne1 OFF car trop proche");
            vanneOff();
        }
        if(digitalRead(PIN_VANNE0) && gauss > calib2)
        {
            Serial.println("Vanne0 OFF car trop loin");
            vanneOff(); 
        }
    }

    lastSel = sel;
    lastUp = up;
    lastDown = down;
}