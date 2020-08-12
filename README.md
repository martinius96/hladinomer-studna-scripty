# Hladinomer - odosielacie scripty
**Podporte projekt - pre uvoľnenie projektu**
* Podpora možná prostredníctovom PayPalu: https://www.paypal.me/chlebovec

* Tento repozitár obsahuje voľne dostupné zdrojové kódy pre mikrokontroléry k projektu Hladinomer
* Mikrokontroléry v klientskom režime komunikujú so vzdialeným serverom, ktorému odosielajú dáta
* Testovacie webové rozhranie je dostupné na adrese: **https://arduino.php5.sk/studna/**
* Viac informácii o projekte: https://arduino.php5.sk/studna-voda.php
* Je možné využiť Ethernet, WiFi technológiu pre prenos údajov do webového rozhrania
* **Hladinomer funguje na princípe ultrazvukového senzora HC-SR04, prípadne jeho vodotesnej verzie JSN-SR04T, ktorý vie zaznamenať vzdialenosť od veka nádrže, studne**
* Vďaka tomu je možné určiť výšku hladiny vody, vrátane objemu v litroch, či inej kubickej veličine (v závislosti od známej maximálnej hĺbky studne a priemeru studne)
* **Ultrazvukový senzor vyšle signál Trigger a meria čas, pokým sa signál vráti do prijímača - Echo. Na základe času sa určuje vzdialenosť**
* Vzdialenosť nereprezentuje skutočnú výšku hladiny vody, iba vzdialenosť medzi senzorom a hladninou!

# Ako získať webové rozhranie?
**Pri záujme o kúpu webového rozhrania kontaktovať na: martinius96@gmail.com**

# Spustenie systému
* Obsah priečinka src rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania
* Webové rozhranie je dostupné na adrese: **https://arduino.php5.sk/studna/**

**Scripty sú dostupné pre:**
* Arduino + Ethernet W5100/W5500 - HTTP protokol
* ESP8266 (NodeMCU) - HTTP/HTTPS protokol
* ESP32 (DevKit) - HTTP/HTTPS protokol

Operačný mód | StandBy | Deep Sleep | StandBy + Over The Air (OTA)
------------ | ------------- | ------------- | -------------
Mikrokontróler | - | - | -
Arduino + Ethernet | ✓ | × | ×
ESP8266 | ✓ | ✓ | ✓
ESP32 | ✓ | ✓ | ✓

# Podporovaný hardvér
![Hardvér pre hladinomer - monitor výšky hladiny vody v studni](https://i.imgur.com/RqUwKbw.jpg)
# Schéma zapojenia mikrokontrolérov + HC-SR04 / JSN-SR04T
![Schéma zapojenia - hladinomer - senzor HC-SR04 / JSN-SR04T](https://i.imgur.com/O7QYERr.png)
# Ukážka webaplikácie
![Hladinomer - Prehľad hĺbky studne v reálnom čase](https://i.imgur.com/bUifgSE.png)
![Hladinomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/2FfyDiw.png)
![Hladinomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/Oxar3HS.png)
![Hladinomer - Budíková reprezentácia maximálnych, minimálnych dát](https://i.imgur.com/V1fQsq5.jpg)
