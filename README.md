# Hladinomer - Arduino / ESP / Sigfox LPWAN IoT
<p align="justify">
Repozitár obsahuje programové implementácie v jazyku Wiring (Arduino Core) pre mikrokontroléry AVR ATmega328P / ATmega2560 (Arduino Uno / Mega R3), ESP8266 a ESP32.
Implementácie počítajú s využitím ultrazvukového senzora vzdialenosti (HC-SR0X / JSN-SR04T / HY-SRF0X / DYP-ME007) pre záznam výšky hladiny vody (vzdialenosti). 
Podporované technológie pre prenos údajov sú: Ethernet / WiFi / IoT LPWAN sieť Sigfox.
Pre ESP32 je dostupná experimentálna implementácia aj v prostredí frameworku ESP-IDF s využitím operačného systému FreeRTOS.
Firmvér je rozdelený na základe doplnkových funkcií, ktoré mikrokontroléry podporujú.
Štandardný režim StandBy umožňuje prenos dát na webové rozhranie a stará sa o udržanie stálej konektivity v LAN sieti.
Pre ESP8266 a ESP32 sú dostupné implementácie s podporou vzdialenej aktualizácie firmvéru Over-The-Air (OTA) cez LAN sieť so sieťovým OTA portom.
K dispozícii je aj Ultra-Low-Power (ULP) firmvér pre ESP mikrokontroléry, ktorý minimalizuje spotrebu elektrickej energie v dôsledku prepnutia mikrokontroléru do režimu hlbokého spánku (Deep Sleep). 
Prebudenie mikrokontroléru sa realizuje podporovanou metódou (RTC Timer pre ESP32, External Wake pre ESP8266).
Dostupný firmvér umožňuje prenos dát na testovacie webové rozhranie, kde je možné zaznamenávať a vizualizovať dáta o výške hladiny vody v studni.
<b>Testovacie webové rozhranie projektu podporuje iba HTTP protokol, príklady pre HTTPS spojenie fungovať nebudú!</b>
Webaplikácia je dostupná v anglickom, nemeckom, ruskom a slovenskom jazyku.
Projekt Hladinomer v staršej verzii existuje aj s rozšírením o zrážkomer, ktorý umožňuje zaznamenávať početnosť zrážok (projekt zrážkomera je neudržiavaný).
Rozšírený opis projektu: https://martinius96.github.io/hladinomer-studna-scripty/
Webové rozhranie je Hladinomer s možnosťou vyskúšania: http://arduino.clanweb.eu/studna_s_prekladom/
Webové rozhranie je Hladinomer + Zrážkomer RG-11 s možnosťou vyskúšania: http://arduino.clanweb.eu/studna/
</p>

# Ako získať webové rozhranie?
**Pri záujme o kúpu webového rozhrania kontaktovať na: martinius96@gmail.com**
* Na testovacom webovom rozhraní je možné zdarma po ľubovoľnú dobu vyskúšať projekt s vašim hardvérom

# Spustenie systému Hladinomer
* **Obsah priečinka /src/ rozbaliť do C:/Users/[User]/Dokumenty/Arduino/libraries**
* Nahrať do mikrokontroléra daný program (off-line tester / on-line sketch) pre odosielanie dát do webového rozhrania POST metódou
* V prípade zakúpenia projektu s webovým rozhraním --> importovať .sql tabuľku do MySQL databázy
* V connect.php nastaviť prístupové údaje k MySQL databáze, HTTP Auth údaje a API key pre mikrokontróler, ktorým bude autorizovaný pre zápis dát
* Nastaviť hĺbku a priemer studne v časti Nastavení vrátane názvu studne / miesta merania
* Nahrať program, ktorý sa po tomto kroku vygeneruje (s API kľúčom) do mikrokontroléra
* Prevádzkovanie projektu s možnosťou klonovania

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

# Získanie Root CA certifikátu (Pre ESP32 / ESP8266 - HTTPS) - OpenSSL
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null

# Podporovaný hardvér
![Hardvér pre hladinomer - monitor výšky hladiny vody v studni](https://i.imgur.com/RqUwKbw.jpg)
# Schéma zapojenia mikrokontrolérov + HC-SR04 / JSN-SR04T
![Schéma zapojenia - hladinomer - senzor HC-SR04 / JSN-SR04T](https://i.imgur.com/O7QYERr.png)
# Ukážka webaplikácie - Hladinomer
![Hladinomer - Prehľad hladiny vody v studni a objemu studne v reálnom čase](https://i.imgur.com/gHbErg5.gif)
![Hladinomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/mRWHkEU.png)
![Hladinomer - Budíková reprezentácia maximálnych, minimálnych dát](https://i.imgur.com/VjgTmxd.png)
![Hladinomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/Bh98Yx0.png)
![Hladinomer - Odhad maximálneho merania senzormi bez odrazov so známym priemerom studne](https://i.imgur.com/CLO2A7I.png)
![Hladinomer - ESP32 - ESP-IDF- FreeRTOS, HTTPS connectivity](https://i.imgur.com/xyhyH8A.png)

# Rozšírená verzia projektu Hladinomer o Zrážkomer ponúka (NEPODPOROVANÁ):
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
