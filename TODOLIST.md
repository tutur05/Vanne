- Serial Deporte via MQTT
Ecran 
Cloturer OTA au bout de 5min + MDP
Test fonctionnement perte wifi 
PIR A définir
Afficher recherche wifi en cours
Améliorer MQTT reconnexio n(actuellement trop frequent et bloquant si serveur MQTT down) => Faire une connexion a chaque envoie plutot que de garder la liaison ? => PB dans ce cas pour recevoir les ordres ? Ou alors envoyer des msg avec une persistance (possible via "messageretained") 
Voir sinon pour inclure un compteur d'echec =>2 echec == > Reconnexion qu'une fois par 6h

GPIO 5 D1 SDA
GPIO4 D2 SCL 
UP GPIO12 D6
DOWN GPIO2 D4
MID GPIO14 D5
Vanne GPIO16 D0
Vanne2 D8
1Wire GPIO0 D3
PIR GPIO13 D7