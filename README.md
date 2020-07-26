# Hladinomer - odosielacie scripty
* Tento repozitár obsahuje voľne dostupné zdrojové kódy pre mikrokontroléry
* Mikrokontroléry odosielajú dáta na webové rozhranie, kde sa archivujú a používateľovi webové rozhranie vypisuje dynamicky dáta
* Webové rozhranie je dostupné na adrese: **https://arduino.php5.sk/studna/**
* Viac o projekte: https://arduino.php5.sk/studna-voda.php
* Je možné využiť Ethernet, WiFi technológiu pre prenos údajov do webového rozhrania
* **Hladinomer funguje na princípe ultrazvukového senzora HC-SR04, prípadne jeho ultrazvukovej verzie JSN-SR04T, ktorý vie zaznamenať vzdialenosť od veka nádrže, studne**
* Vďaka tomu je možné určiť výšku hladiny vody, vrátane objemu v litroch, či inej kubickej veličine (v závislosti od známej maximálnej hĺbky studne a priemeru studne)
* Ultrazvukový signál vyšle signál Trigger a meria čas, pokým sa signál vráti do prijímača - Echo. Na základe času sa určuje vzdialenosť

**Webové rozhranie je možné zakúpiť si: martinius96@gmail.com**

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
# Schéma zapojenia mikrokontrolérov + HC-SR04 / JSN-SR04T
![Schéma zapojenia - hladinomer - senzor HC-SR04 / JSN-SR04T](https://i.imgur.com/e4AqAIG.png)
# Ukážka webaplikácie
![Hladinomer - Prehľad hĺbky studne v reálnom čase](https://i.imgur.com/bUifgSE.png)
![Hladinomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/2FfyDiw.png)
![Hladinomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/Oxar3HS.png)
![Hladinomer - Budíková reprezentácia maximálnych, minimálnych dát](https://i.imgur.com/V1fQsq5.jpg)
