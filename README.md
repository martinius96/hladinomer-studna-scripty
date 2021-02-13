# Hladinomer - odosielacie scripty
**Podporte projekt pre pridanie nových funkcionalít a spôsobov merania**
* Podpora možná prostredníctovom PayPalu: https://www.paypal.me/chlebovec

* Tento repozitár obsahuje voľne dostupné zdrojové kódy pre mikrokontroléry k projektu Hladinomer a Hladinomer + Zrážkomer
* Mikrokontroléry v klientskom režime komunikujú so vzdialeným serverom, ktorému odosielajú dáta
** Testovacie webové rozhrania podporuje iba HTTP protokol, príklady pre HTTPS spojenei fungovať nebudú!**
* Je možné využiť Ethernet, WiFi technológiu, sieť Sigfox (IoT) pre prenos údajov do webového rozhrania
* **Hladinomer funguje na princípe ultrazvukového senzora HC-SR04 / JSN-SR04T, prípadne jeho vodotesnej verzie JSN-SR04T, ktorý vie zaznamenať vzdialenosť od veka nádrže, studne**
* Vďaka tomu je možné určiť výšku hladiny vody, vrátane objemu v litroch, či inej kubickej veličine (v závislosti od známej maximálnej hĺbky studne a priemeru studne)
* **Ultrazvukový senzor vyšle signál Trigger a meria čas, pokým sa signál vráti do prijímača - Echo. Na základe času sa určuje vzdialenosť**
* Vzdialenosť nereprezentuje skutočnú výšku hladiny vody, iba vzdialenosť medzi senzorom a hladninou, webaplikácia využíva prepočet na skutočnú výšku hladiny vody na základe známej maximálnej hĺbky studne.
* Webaplikácia je preložená do anglického, nemeckého, ruského a slovenského jazyka

# Ako získať webové rozhranie?
**Pri záujme o kúpu webového rozhrania kontaktovať na: martinius96@gmail.com**

# Spustenie systému
* Obsah priečinka src rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania
* Webové rozhranie je Hladinomer s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna_s_prekladom/**
* Webové rozhranie je Hladinomer + Zrážkomer RG-11 s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna/**

**Programové implementácie pre protokoly:**
Mikrokontróler | HTTP | HTTPS
------------ | ------------- | -------------
Arduino + Ethernet | ✓ | ×
ESP8266 | ✓ | ✓
ESP32 | ✓ | ✓

**Podporované operačné módy v zdrojových kódoch**
Operačný mód | Offline Tester| StandBy | Deep Sleep | StandBy + Over The Air (OTA)
------------ | ------------- | ------------- | ------------- | -------------
**Mikrokontróler** | - | - | - | -
Arduino + Ethernet | ✓ | ✓ | × | ×
ESP8266 | ✓ | ✓ | ✓ | ✓
ESP32 | ✓ | ✓ | ✓ | ✓

# Získanie Root CA certifikátu (Pre ESP32 - HTTPS)
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null
# Získanie fingerprintu certifikátu servera (Pre ESP8266 - HTTPS)
* openssl s_client -connect example.com:443 -showcerts < /dev/null 2>/dev/null   | openssl x509 -in /dev/stdin -sha1 -noout -fingerprint

# Podporovaný hardvér
![Hardvér pre hladinomer - monitor výšky hladiny vody v studni](https://i.imgur.com/RqUwKbw.jpg)
# Schéma zapojenia mikrokontrolérov + HC-SR04 / JSN-SR04T
![Schéma zapojenia - hladinomer - senzor HC-SR04 / JSN-SR04T](https://i.imgur.com/O7QYERr.png)
# Ukážka webaplikácie - Hladinomer
![Hladinomer - Prehľad hĺbky studne v reálnom čase](https://i.imgur.com/bUifgSE.png)
![Hladinomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/2FfyDiw.png)
![Hladinomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/Oxar3HS.png)
![Hladinomer - Budíková reprezentácia maximálnych, minimálnych dát](https://i.imgur.com/V1fQsq5.jpg)
![Hladinomer - Odhad maximálneho merania senzormi bez odrazov so známym priemerom studne](https://i.imgur.com/XL7ohx3.png)
![Hladinomer - HC-SR04 - Detekčná charakteristika](https://i.imgur.com/iNdrwh1.png)
![Hladinomer - JSN-SR04T - Detekčná charakteristika](https://i.imgur.com/LKIuhJd.png)

# Rozšírená verzia projektu Hladinomer o Zrážkomer ponúka:
* Zber údajov zo zrážkomera Hydreon RG-11 - načítavanie v prerušení s debouncingom 15ms
* Vizualizácia vo web rozhraní
* Vizualizácia rozšírená o 30 dní, 365 dní
* Grafický vývoj zrážok
* Merateľný úhrn (súčet) zrážok za 2 hodin, 24 hodín, 7 dní, 30 dní, 365 dní

# Ukážka rozšírenej webaplikácie - Hladinomer + Zrážkomer
![Hladinomer + Zrážkomer - Prehľad hĺbky studne a zrážok v reálnom čase](https://i.imgur.com/AQI6Zz2.png)
![Hladinomer + Zrážkomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/HjrBQTO.png)
![Zrážkomer + Zrážkomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/HadPs1L.png)
![Hladinomer + Zrážkomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/NAL0cNG.png)
![Hladinomer + Zrážkomer - Odhad maximálneho merania senzormi bez odrazov so známym priemerom studne](https://i.imgur.com/svIAGZb.png)
