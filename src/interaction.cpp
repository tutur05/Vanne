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
    if (up == LOW && lastUp == HIGH)
    {
        Serial.println("up");
        getMqttClient().publish("VanneV1/button", "Up");

        if (mode == 0)
        {
            consigne = consigne +1;
        }
                else if (mode == 3)
        {
            vanneO();
        }
        
                update_display();

    }

    if (sel == LOW && lastSel == HIGH) //Bacule du mode via btn milieu 
    {
        Serial.println("sel");

        getMqttClient().publish("VanneV1/button", "sel");
        if (mode == 0)
        {
            mode = 1;
                    Serial.println("Passage en M1");

        }
        else if (mode == 1)
        {
            mode = 2;
            Serial.println("Passage en M2");
            
        }
        else if (mode == 2)
        {
            mode = 3;Serial.println("Passage en M3");
        }
                else if (mode == 3)
        {
            mode = 0;Serial.println("Passage en M0");
        }


        update_display();
    }

    if (down == LOW && lastDown == HIGH)
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
        update_display();

    }

    if(mode == 3) //Extinction des vannes lors du relachement du button
    {
   gauss = analogRead(A0); // Lecture de la sonde magnétique

    if (up == HIGH && lastUp == LOW)
    {
        Serial.println("up relache");
        vanneOff();
    }
        if (down == HIGH && lastDown == LOW)
    {
        Serial.println("down relache");
        vanneOff();
    }

    if(digitalRead(PIN_VANNE0) == HIGH)
    {
        if(gauss >545){
        Serial.println("Vanne0 OFF");
        vanneOff();
        }
    }

        if(digitalRead(PIN_VANNE1) == HIGH) //vanneF = btn Bas
    {
        if(gauss <500)
        vanneOff();
    }



    }



    lastSel = sel;
    lastUp = up;
    lastDown = down;
}