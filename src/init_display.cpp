#include "init_display.h"
#include "config.h"
#include "scheduler.h"
extern Adafruit_SSD1306 display;
extern int consigne;
extern byte mode;
extern unsigned long last_pir;
extern unsigned last_menu;
extern float temperature;
extern int consigne;
extern byte mode;
extern float gauss;
extern short TcalibOuvrir;
extern short TcalibFermer;
extern short Tcalib;
extern byte mode_max;
String message = "";
String message1 = "";
String message2 = "";
unsigned long publish_message = 0;
const unsigned long UPDATE_INTERVAL = 2000;
byte count = 0;
extern Task t1;
extern Task t2;
extern Task t4;
extern Task t6;
extern Task t7;
extern Task t8;
extern Task t9;
extern bool etat_vanne;


void update_message()
{

    // On exécute l'alternance seulement si message2 n'est pas vide
    if (message2 != "")
    {
        // Est-ce que 3 secondes se sont écoulées depuis la dernière mise à jour ?
        if (millis() - publish_message >= UPDATE_INTERVAL)
        {

            // On alterne le message
            if (message == message1)
            {
                count = count + 1;
                message = message2;
            }
            else
            {
                message = message1;
                if (count > 2)
                {
                    count = 0;
                    message2 = "";
                }
            }

            // On met à jour le moment de la dernière publication
            publish_message = millis();
        }
    }
    // Si message2 est vide, on s'assure que le message est bien message1
    // (Cette partie est optionnelle si vous initialisez bien 'message' dans setup())
    else
    {
        message = message1;
    }
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

    switch (mode) // Affichage de la valeur à editer
    {

    case 1:
        display.println("16");
        break;
    case 4:
        display.println(TcalibOuvrir);
        break;
    case 5:
        display.println(TcalibFermer);
        break;
    case 6:
        display.println(Tcalib);
        break;
    default:
        display.print(consigne);
        display.print((char)247); // °
        display.println("C");

        break;
    }
    display.setTextSize(2);

    switch (mode) // affichage du mode
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
        display.println("T_Ouvrir");
        break;
    case 5:
        display.println("T_Fermer");
        break;
    case 6:
        display.println("TCalib");
        break;
    }
    if (mode_max != 2)
    {
        display.setTextSize(1);
        display.print("+O ");
        display.print(TcalibOuvrir);
        display.print("<");
        display.print(roundf(gauss));
        display.print(">");
        display.print(TcalibFermer);
        display.println(" -F");
    }
    display.print(temperature);
    display.print((char)247); // °
    display.print("C - ");
    if (mode_max != 2)
    {
        /*if (etat_vanne)
            display.print("VOuv ");
        else
            display.print("VFer ");
        if (t6.isEnabled())
            display.print("cO");
        if (t7.isEnabled())
            display.print("cF");
        */
       if(t1.isEnabled())
        display.print("1");
       if(t2.isEnabled())
        display.print("2");
       if(t4.isEnabled())
        display.print("4");
        if(t8.isEnabled())
        display.print("8");
        if(t9.isEnabled())
        display.print("9");         
    }
    int bars = 0;
    if (WiFi.status() == WL_CONNECTED)
    {
        int32_t rssi = WiFi.RSSI(); // Valeur RSSI (en dBm)
        // Convertir RSSI en nombre de barres (0 à 4)
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
    }
    else
    {
        // Si le WiFi n'est pas connecté, afficher un '!'
        display.setTextSize(2);
        display.setCursor(100, 0);
        display.print("!");
    }

    // Dessiner les barres type "antenne"
    int baseX = 110;
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
