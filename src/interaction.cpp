#include "interaction.h"
#include "config.h"
#include "init_display.h"

#include <Arduino.h>

void init_pin()
{
    if (digitalRead(PIN_PIR))
        last_pir = millis();

    static bool lastUp = HIGH, lastSel = HIGH, lastDown = HIGH;

    // Lecture boutons
    bool up = digitalRead(PIN_BTNUP);
    bool sel = digitalRead(PIN_BTNMID);
    bool down = digitalRead(PIN_BTNDOWN);

     // 0 MANUEL // 1 ECO // 2 AUTO (PIR)
    // Détection front descendant
    if (up == LOW && lastUp == HIGH)
    {
        Serial.println("up");
        getMqttClient().publish("VanneV1/button", "Up");

        if (mode == 0)
        {
            consigne = consigne +1;
        }
                update_display();

    }

    if (sel == LOW && lastSel == HIGH)
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
        update_display();

    }

    lastSel = sel;
    lastUp = up;
    lastDown = down;
}