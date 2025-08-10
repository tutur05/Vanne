- Serial Deporte via MQTT
Ecran 
Test fonctionnement perte wifi 
PIR A définir
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


CALIB OUVRIR FERMER SALLE POLY : O => 519 F =>
Quand je ferma la vanne, la valeur monte 
555 Fermer (à 556 c'est trop haut, je m'arrête)
519 Ouvrir ( à518 c'est trop bas je m'arrête)


Améliorer detection butée (actuellement ouvre et seulement après vérifie si il peut ouvrir)
Controle MQTT externe
Améliorer lecture sonde via tache déporté