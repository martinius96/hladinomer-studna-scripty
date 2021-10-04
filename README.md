# Hladinomer - Arduino / ESP

Tento repozitár obsahuje programovú implementáciu - zdrojové kódy pre mikrokontroléry Arduino, ESP8266, ESP32, ktoré sú využité v projekte Hladinomer / Hladinomer + Zrážkomer. Mikrokontroléry realizujú HTTP / HTTPS POST request na webserver v LAN sieti, alebo na vzdialený server na internete, pričom do tela správy obsiahnú namerané údaje.
**Testovacie webové rozhranie projektu podporuje iba HTTP protokol, príklady pre HTTPS spojenie fungovať nebudú!**
Pre prenos údajov je možné využiť tieto technológie: Ethernet, WiFi (2.4GHz), sieť Sigfox (IoT).
**Projekt Hladinomer funguje na princípe ultrazvukového senzora HC-SR04, prípadne jeho vodotesnej verzie JSN-SR04T, ktorý je schopný zaznamenať vzdialenosť od veka nádrže, studne**
Vďaka tomu je možné určiť výšku hladiny vody, vrátane objemu v litroch, či inej kubickej veličine (v závislosti od známej maximálnej hĺbky studne a priemeru studne)
**Ultrazvukový senzor vyšle signál Trigger a meria čas, pokým sa signál vráti do prijímača - Echo. Na základe rýchlosti šírenia zvuku sa vypočíta vzdialenosť**
Vzdialenosť nereprezentuje skutočnú výšku hladiny vody, iba vzdialenosť medzi senzorom a hladinou, webaplikácia využíva prepočet na skutočnú výšku hladiny vody na základe známej maximálnej hĺbky studne vo webaplikácii (prepočet realizuje webserver, mikrokontróler oznamuje vzdialenosť po hladinu).
Webaplikácia je preložená do anglického, nemeckého, ruského a slovenského jazyka.

# Ako získať webové rozhranie?
**Pri záujme o kúpu webového rozhrania kontaktovať na: martinius96@gmail.com**

# Spustenie systému Hladinomer
* **Obsah priečinka /src/ rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries**
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania
* V prípade zakúpenia projektu s webovým rozhraním --> importovať .sql tabuľku do MySQL databázy
* Vo webaplikácii nastaviť prístupové údaje k MySQL databáze, HTTP Auth údaje a API key pre mikrokontróler, ktorým bude autentizovaný
* Nahrať program, ktorý sa po tomto kroku vygeneruje (s API kľúčom) do mikrokontroléru
* Rozšírený opis projektu: **https://martinius96.github.io/hladinomer-studna-scripty/**
* Webové rozhranie je Hladinomer s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna_s_prekladom/**
* Webové rozhranie je Hladinomer + Zrážkomer RG-11 s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna/**

**Programové implementácie pre prenosové protokoly podľa mikrokontrolérov:**
| Mikrokontróler | HTTP | HTTPS |
| ------------ | ------------- | ------------- |
| Arduino + Ethernet | ✓ | × |
| ESP8266 | ✓ | ✓ |
| ESP32 | ✓ | ✓ |

**Podporované operačné módy prevádzky mikrokontrolérov v zdrojových kódoch:**
| Operačný mód | Offline Tester| StandBy | Deep Sleep | StandBy + Over The Air (OTA) |
| ------------ | ------------- | ------------- | ------------- | ------------- |
| **Mikrokontróler** | - | - | - | - |
| Arduino + Ethernet | ✓ | ✓ | × | × |
| ESP8266 | ✓ | ✓ | ✓ | ✓ |
| ESP32 | ✓ | ✓ | ✓ | ✓ |

# Získanie Root CA certifikátu (Pre ESP32 - HTTPS)
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null
# Získanie fingerprintu certifikátu servera (Pre ESP8266 - HTTPS)
* openssl s_client -connect example.com:443 -showcerts < /dev/null 2>/dev/null   | openssl x509 -in /dev/stdin -sha1 -noout -fingerprint

# Podporovaný hardvér
![Hardvér pre hladinomer - monitor výšky hladiny vody v studni](https://i.imgur.com/RqUwKbw.jpg)
# Schéma zapojenia mikrokontrolérov + HC-SR04 / JSN-SR04T
![Schéma zapojenia - hladinomer - senzor HC-SR04 / JSN-SR04T](https://i.imgur.com/O7QYERr.png)
# Ukážka webaplikácie - Hladinomer
![Hladinomer - Prehľad hĺbky studne v reálnom čase](https://i.imgur.com/7J295Ae.png)
![Hladinomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/SdDbSvn.png)
![Hladinomer - Budíková reprezentácia maximálnych, minimálnych dát](https://i.imgur.com/Rido0yn.png)
![Hladinomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/k1DgXgY.png)
![Hladinomer - Odhad maximálneho merania senzormi bez odrazov so známym priemerom studne](https://i.imgur.com/vtg6NpT.png)

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
