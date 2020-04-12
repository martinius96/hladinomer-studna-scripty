# Hladinomer - odosielacie scripty
* Tento repozitár obsahuje voľne dostupné zdrojové kódy, ktoré odosielajú dáta na webové rozhranie
* Webové rozhranie je dostupné na adrese: **https://arduino.php5.sk/studna/**
* Viac o projekte: https://arduino.php5.sk/studna-voda.php
* Je možné využiť Ethernet, WiFi technológiu pre prenos údajov do webového rozhrania
* Hladinomer funguje na princípe ultrazvukového senzora HC-SR04, ktorý vie zaznamenať vzdialenosť od veka nádrže, studne
* Vďaka tomu je možné určiť výšku hladiny vody, vrátane objemu v litroch, či inej kubickej veličine
* **Webové rozhranie je možné zakúpiť si: martinius96@gmail.com**

# Spustenie systému
* Obsah priečinka src rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania
* Webové rozhranie je dostupné na adrese: **https://arduino.php5.sk/studna/**
#
**Scripty sú dostupné pre:**
* Arduino + Ethernet W5100/W5500 - HTTP protokol
* ESP8266 (NodeMCU) - HTTP/HTTPS protokol
* ESP32 (DevKit) - HTTP/HTTPS protokol

**Podporte projekt - pre uvoľnenie projektu**
* Podpora možná prostredníctovom PayPalu: https://www.paypal.me/chlebovec

# Podporovaný hardvér
![Hardvér pre hladinomer - monitor výšky hladiny vody v studni](https://i.imgur.com/RqUwKbw.jpg)
# Schéma zapojenia mikrokontrolérov + HC-SR04
![Schéma zapojenia - hladinomer](https://i.imgur.com/e4AqAIG.png)
# Ukážka webaplikácie
![Hladinomer - Prehľad](https://i.imgur.com/Qe0FbZe.png)
![Hladinomer - Grafická reprezentácia nameraných údajov](https://i.imgur.com/2FfyDiw.png)
![Hladinomer - Tabuľka rozdielových záznamov](https://i.imgur.com/cRgJkdN.png)

# Výpočet maximálnej hĺbky studne
* Maximálna hĺbka studne bez odrazov (so zohľadnením charakteristík senzora HC-SR04)
* **Výpočet trigonometriou:**
* **Efektívny lúč Echo prijímača:** 15°
* **Priemer studne:** XY (cm)
* **Maximálna hĺbka** = XY / tan(rad[15°/2]) (cm)
![Trigonometria - výpočet maximálnej hĺbky studne - hladinomer - Arduino / ESP8266 / ESP32 + HC-SR04](https://i.imgur.com/nMQZQm7.jpg)
