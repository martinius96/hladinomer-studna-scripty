# Hladinomer - Arduino / ESP
<p style="align: justify;">
Repozitár obsahuje programové implementácie v jazyku Wiring (Arduino Core) pre mikrokontroléry AVR (Arduino), ESP8266 a ESP32.
Implementácie dokážu merať výšku hladiny vody v studni s kompatibilným ultrazvukovým senzorom vzdialenosti (HC-SR0X / JSN-SR04T). 
Podporované technológie pre prenos údajov sú: Ethernet / WiFi / IoT sieť Sigfox.
Pre ESP32 je dostupná experimentálna implementácia aj v prostredí frameworku ESP-IDF s využitím FreeRTOS.
Firmvér je rozdelený na základe doplnkových funkcií, ktoré mikrokontroléry podporujú.
Štandardný režim StandBy umožňuje prenos dát na webové rozhranie a stará sa o udržanie stálej konektivity v LAN sieti.
Pre ESP8266 a ESP32 sú dostupné implementácie s podporou vzdialenej aktualizácie firmvéru Over-The-Air (OTA) cez LAN sieť so sieťovým OTA portom.
K dispozícii je aj Ultra-Low-Power (ULP) firmvér pre tieto mikrokontroléry, ktorý minimalizuje spotrebu elektrickej energie v dôsledku prepnutia mikrokontroléru do režimu hlbokého spánku (Deep Sleep). 
Prebudenie mikrokontroléru sa realizuje podporovanou metódou (RTC Timer pre ESP32, External Wake pre ESP8266).
Dostupný firmvér umožňuje prenos dát na testovacie webové rozhranie, kde je možné zaznamenávať a vizualizovať dáta o výške hladiny vody v studni.
**Testovacie webové rozhranie projektu podporuje iba HTTP protokol, príklady pre HTTPS spojenie fungovať nebudú!**
Webaplikácia je dostupná v anglickom, nemeckom, ruskom a slovenskom jazyku.
Projekt Hladinomer v staršej verzii existuje aj s rozšírením o hladinomer, ktorý umožňuje zaznamenávať početnosť zrážok.
Rozšírený opis projektu: **https://martinius96.github.io/hladinomer-studna-scripty/
Webové rozhranie je Hladinomer s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna_s_prekladom/
Webové rozhranie je Hladinomer + Zrážkomer RG-11 s možnosťou vyskúšania: **http://arduino.clanweb.eu/studna/
</p>

# Ako získať webové rozhranie?
**Pri záujme o kúpu webového rozhrania kontaktovať na: martinius96@gmail.com**

# Spustenie systému Hladinomer
* **Obsah priečinka /src/ rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries**
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania
* V prípade zakúpenia projektu s webovým rozhraním --> importovať .sql tabuľku do MySQL databázy
* Vo webaplikácii nastaviť prístupové údaje k MySQL databáze, HTTP Auth údaje a API key pre mikrokontróler, ktorým bude autentizovaný
* Nahrať program, ktorý sa po tomto kroku vygeneruje (s API kľúčom) do mikrokontroléru

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
