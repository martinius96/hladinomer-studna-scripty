# Nutné úpravy programu StandBy
* Program je pre implementáciu Ethernet (ENC28J60 / W5100 / W5500) / ESP8266 / ESP32
* StandBy režim, stále aktívny modem, udržiavanie spojenia na WiFi sieť

```
const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis

#include <Ethernet.h> //Ethernet shield Wiznet W5100 - zakomentovat ak sa nepouziva
//#include <Ethernet2.h> //Ethernet modul Wiznet W5500 - zakomentovat ak sa nepouziva
//#include <UIPEthernet.h> //Ethernet modul ENC28J60 - zakomentovat ak sa nepouziva

const char * ssid = "WIFI_MENO"; //meno wifi siete
const char * password = "WIFI_HESLO"; //heslo na wifi siet
```

# Nutné úpravy programu OTA_WiFi
* Program je pre implementáciu ESP8266 / ESP32
* StandBy režim, stále aktívny modem, udržiavanie spojenia na WiFi sieť
* Umožňuje aktualizovať firmvér mikrokontroléru cez LAN sieť 

```
const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis

const char * ssid = "WIFI_MENO"; //meno wifi siete
const char * password = "WIFI_HESLO"; //heslo na wifi siet
```

# Nutné úpravy programu ULP_WiFi
* Program je pre implementáciu ESP8266 / ESP32
* Režim hlbokého spánku - WiFi modem OFF, Xtensa CPU off
* RTC Timer wakeup (pre ESP8266 nutné prepojiť GPIO16 - D0 s RST - označene v scheme zapojenia)

```
const char* host = "arduino.clanweb.eu"; //adresa webservera (doména) na ktorú sa odosielajú dáta
String url = "/studna_s_prekladom/data.php"; //URL adresa - cesta pod domenou k cieľovemu .php súboru, ktorý realizuje zápis

const char * ssid = "WIFI_MENO"; //meno wifi siete
const char * password = "WIFI_HESLO"; //heslo na wifi siet
```
