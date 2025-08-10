#include "init_display.h"

extern Adafruit_SSD1306 display;
extern int consigne;
extern byte mode;
extern unsigned last_pir;
extern unsigned last_menu;
extern float temperature;
extern int consigne;
extern byte mode;
extern float gauss;
extern short min_calibOuvrir;
extern short max_calibFermer;    
String message = "";
String message1 = "";
String message2 = "";
unsigned long publish_message = 0;
void update_message() { //Alerne l'affichage message1 et message2
    



    if(message2 != "")
    {
    if (publish_message + 3000 < millis()) {
        Serial.println("Updating message");
        if(message == message1)
        {
        message = message2;
    }
        else
        message = message1;

        publish_message = millis();
    }
    }
    else
    message = message1; // Publie le message toutes les 10 secondes
}

void update_display()
{
//
         gauss = analogRead(A0); // Lecture de la sonde magnétique

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    update_message();
    display.println(message);


    display.setTextSize(3);

    switch (mode) //Affichage de la valeur à editer
    {
    
case 1:
    display.println("16");
    break;
case 4:
    display.println(min_calibOuvrir);
    break;
case 5:
    display.println(max_calibFermer);
    break;
default:
    display.print(consigne);
    display.print((char)247); // °
    display.println("C");
    break;
}
display.setTextSize(2);

switch (mode) //affichage du mode
{
case 0:
    display.println("MANUEL");
    break;
case 1:
    display.println("ECO");
    break;
case 2:
    display.println("AUTO");
    break;
case 3:
    display.println("TEST");
    break;
case 4:
    display.println("MaxFermer");
    break;
case 5:
    display.println("MinOuvrir");
    break;
}
display.setTextSize(1);
display.print(gauss);
display.print("  ");
display.print(temperature);
display.print((char)247); // °
display.println("C");

int32_t rssi = WiFi.RSSI(); // Valeur RSSI (en dBm)

// Convertir RSSI en nombre de barres (0 à 4)
int bars = 0;
if (rssi > -60)
    bars = 4;
else if (rssi > -70)
    bars = 3;
else if (rssi > -80)
    bars = 2;
else if (rssi > -90)
    bars = 1;
else
    bars = 0;

// Dessiner les barres type "antenne"
int baseX = 90;
int baseY = 12; // Position de base pour les barres
int barWidth = 3;
int spacing = 1;

for (int i = 0; i < 4; i++)
{
    int barHeight = (i + 1) * 3; // Hauteur de la barre en pixels
    if (i < bars)
    {
        display.fillRect(baseX + i * (barWidth + spacing), baseY - barHeight, barWidth, barHeight, SSD1306_WHITE);
    }
    else
    {
        display.drawRect(baseX + i * (barWidth + spacing), baseY - barHeight, barWidth, barHeight, SSD1306_WHITE);
    }
}

display.display();
}

