# Inštrukcie - Hladinomer client firmvér
* Každá zložka firmvéru obsahuje nástroj esptool.exe a .bat súbor pre automatizované nahratie firmvéru do mikrokontroléru ESP8266 / ESP32
* **V .bat súbore je nutné zmeniť COM port (vzorovo nastavený pre COM7 v ESP8266 firmvér, resp. COM17 pre ESP32 firmvér)**
* Aktuálny port, kde je prihlásený mikrokontróler nájdete v Správcovi zariadení
* Zariadenie je najčastejšie označené ako CH340 device, alebo CP210X v závislosti od použitého USB-UART prevodníka
* Po nahratí firmvéru sa okno príkazového riadku (CLI) automaticky zatvorí a firmvér je pripravený na použitie
* Pri nahrávaní do mikrokontroléru ESP32 je často nutné prepnúť mikrokontróler do Upload režimu držaním tlačidla BOOT, respektíve privedením GND na GPIO0

<p align="center">
  <img src="https://i.imgur.com/M0U6HkC.png" />
</p>

# Konfigurácia Hladinomer clienta - WiFi Manager
* Po úspešnom nahratí binárneho programu - firmvéru je client plne pripravený na prevádzku a prenos dát na testovacie webové rozhranie.
* V prípade, že vo flash pamäti nemá uložené mikrokontróler údaje o WiFi sieti, začne vysielať vlastné SSID - Hladinomer_AP. 
* WiFi sieť Hladinomer_AP je otvorená bez zadania hesla
* Po pripojení smartfónom / počítačom do tejto WiFi siete je na adrese 192.168.4.1 dostupné webové rozhranie WiFi Manager
* Rozhranie poskytuje možnosť konfigurácie WiFi údajov pre vašu domácu WiFi sieť. 
* V rozhraní je možné zadať meno a heslo existujúcej WiFi siete, na ktorú sa ESP pripojí, prípadne je možné vybrať WiFi sieť v dosahu a dopísať k nej heslo.

# Aktivita Hladinomer clienta
* Funkcie klienta, meranie výšky hladiny vody a prenos dát sú spustené až po pripojení ESP mikrokontroléru do vašej LAN siete! 
* Po pridelení IP adresy pre konektivitu vo vašej sieti.
* Po pripojení na domácu WiFi sieť ESP prestáva vysielať SSID, prepne sa do STA (Station) módu a funguje už v režime klienta a prenáša áta 
* Údaje o WiFi sieti sú uložené do flash pamäte mikrokontroléra a už ich nie je nutné zadávať znova pri jeho opätovnom spustení
* V prípade, že daná sieť nie je dostupná, začne ESP opäť vysielať vlastné SSID: Hladinomer_AP a opätovne je možné zadať prihlasovacie informácie. 
