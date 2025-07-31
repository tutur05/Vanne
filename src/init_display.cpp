#include "init_display.h"

extern Adafruit_SSD1306 display;
extern int consigne;
extern byte mode;
extern unsigned last_pir;
extern unsigned last_menu;
extern float temperature;
extern int consigne;
extern byte mode;

void update_display()
{

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(WiFi.localIP());
 


    display.setTextSize(3);
    if (mode == 1)
        display.print("16");
    else
        display.print(consigne);
    display.print((char)247); // °
    display.println("C");
    display.setTextSize(2);

    if (mode == 0)
        display.println("MANUEL");
    else if (mode == 1)
        display.println("ECO");
    else
        display.println("AUTO");

    display.setTextSize(1);
    display.print("           ");
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
