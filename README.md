# Water Level Monitor - Arduino / ESP / Sigfox LPWAN IoT
<p align="justify">
Repository contains software implementations in Wiring language (Arduino Core) for AVR ATmega328P / ATmega2560 microcontrollers (Arduino Uno / Mega R3), ESP8266 and ESP32.
Implementations allow the use of an ultrasonic distance sensor from the RCW series, US-XXX, IOE-SR0X, SR0X, HC-SR0X, HY-SRF0X, DYP-MEXXX, Parallax PING))) ™ to record the water level (distance).
Supported technologies for data transmission are: Ethernet / WiFi / IoT LPWAN network Sigfox.
Firmware is divided based on additional functions that microcontrollers support.
Standard StandBy mode enables data transfer to the web interface and takes care of maintaining constant connectivity in the LAN network.
Implementations with Remote Over-The-Air (OTA) firmware update support are available for the ESP8266 and ESP32 over a LAN with a network OTA port.
Ultra-Low-Power (ULP) firmware for ESP microcontrollers is also available, which minimizes power consumption due to the microcontroller switching to Deep Sleep mode.
Microcontroller wake-up is performed by a supported method (RTC Timer for ESP32, External Wake for ESP8266).
Firmware is available for ESP32 using the FreeRTOS real-time operating system. The implementation is available in Arduino Core, or in the ESP-IDF framework, corresponds to StandBy mode.
Available firmware allows data transfer to a test web interface, where it is possible to record and visualize data on the water level in the well.
<b> Project's test web interface only supports the HTTP protocol, examples for HTTPS connections will not work! </b>
Web application is available in English, German, Russian and Slovak.
Water Level Monitor project in the older version also exists with the extension of a rain gauge, which allows you to record the frequency of precipitation (the rain gauge project is not maintained).
Extended project description: https://martinius96.github.io/hladinomer-studna-scripty/en/
Web interface for Water Level Monitor with the possibility of testing: http://arduino.clanweb.eu/studna_s_prekladom/?lang=en
Web interface for Water Level Monitor + RG-11 rain gauge with the possibility of testing: http://arduino.clanweb.eu/studna/?lang=en
</p>

# How to get web interface?
**If you are interested in purchasing the web interface, contact: martinius96@gmail.com**
* It is possible to try the project with your hardware for free for any period of time on the test web interface

# Starting the Water Level monitor project
* **The contents of the folder /src/ expand to C:/Users/[User]/Documents/Arduino/libraries**
* Upload the given program (off-line tester / on-line sketch) to the microcontroller for sending data to the web interface by POST method
* In case of purchasing a project with a web interface -> import .sql table into MySQL database
* In connect.php set the access data to the MySQL database, HTTP Auth data and API key for the microcontroller that will be authorized to write data
* Set the depth and diameter of the well in the Settings section, including the name of the well / measuring point
* Upload the program that will be generated after this step (with API key) to the microcontroller
* Operation of the project with the possibility of cloning

**Software implementations for transmission protocols according to microcontrollers:**
| Microcontroller | HTTP | HTTPS |
| ------------ | ------------- | ------------- |
| Arduino + Ethernet | ✓ | × |
| ESP8266 | ✓ | ✓ |
| ESP32 | ✓ | ✓ |

**Supported operating modes of microcontroller operation in source codes:**
| Operating mode | Offline Tester| StandBy | Deep Sleep | StandBy + Over The Air (OTA) |
| ------------ | ------------- | ------------- | ------------- | ------------- |
| **Microcontroller** | - | - | - | - |
| Arduino + Ethernet | ✓ | ✓ | × | × |
| ESP8266 | ✓ | ✓ | ✓ | ✓ |
| ESP32 | ✓ | ✓ | ✓ | ✓ |

# Obtaining Root CA certificate (For ESP32 / ESP8266 - HTTPS) - OpenSSL tool
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null

# Supported hardware
![Level meter hardware - well water level monitor](https://i.imgur.com/RqUwKbw.jpg)
# Wiring for microcontrollers + HC-SR04 / JSN-SR04T
![Wiring diagram - level meter - sensor HC-SR04 / JSN-SR04T](https://i.imgur.com/O7QYERr.png)
# Water level monitor - webapp screenshots
![Level meter - Overview of the water level in the well and the volume of the well in real time](https://i.imgur.com/VMLOkiW.gif)
![Level meter - Table of difference records, historical data for the entire period of the well monitor](https://i.imgur.com/YrL0DG1.png)
![Level meter - Alarm representation of maximum, minimum data](https://i.imgur.com/FqqGV8o.png)
![Level meter - Graphical representation of measured data in 24 hours, 7 days, 30 days](https://i.imgur.com/3ynXOBb.png)
![Level meter - ESP32 - ESP-IDF- FreeRTOS, HTTPS connectivity](https://i.imgur.com/xyhyH8A.png)

# Other water level sensors (requires own firmware)
* Laser (LiDAR)- Garmin LIDAR-Lite v3HP / TFMini
* Hydrostatic (submersible) - LMK307 / LMP305 / DPT200
* Electrostatic (capacitive / inductive)
* Pressure (differential / with compensating atmospheric pressure sensor)
* Optical - GP2Y0A02YK0F 
* Mechanical (float)
* Magnetic (Hall)
* Microwave (radar) - CS476 / CS475A-L / SP69 / WLR 8/15/30
* Ultrasonic - other type of output (UART, RS-232, current loop 4-20 mA, Modbus TCP / RTU, M-bus, RS-485, PROFINET, CAN, SDI, DQ ...)
* **Note:** If the water level is recalculated from the bottom on the microcontroller side, it is necessary to set the well depth to 0 cm on the web interface.

# Extended version of Water Level monitor project with Rain Gauge offers (NOT SUPPORTED):
* Water level monitor features (above)
* Data collection from Hydreon RG-11 rain gauge - reading in interruption with debouncing 15ms
* Visualization in the web interface
* Visualization extended by 30 days, 365 days
* Graphic development of precipitation
* Measurable total (sum) of precipitation in 2 hours, 24 hours, 7 days, 30 days, 365 days

# Screenshots of Water level monitor project with Rain Gauge
![Hladinomer + Zrážkomer - Prehľad hĺbky studne a zrážok v reálnom čase](https://i.imgur.com/AQI6Zz2.png)
![Hladinomer + Zrážkomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/HjrBQTO.png)
![Zrážkomer + Zrážkomer - Grafická reprezentácia nameraných údajov za 24 hodín, 7 dní, 30 dní](https://i.imgur.com/HadPs1L.png)
![Hladinomer + Zrážkomer - Tabuľka rozdielových záznamov, historické dáta za celé obdobie monitoru studne](https://i.imgur.com/NAL0cNG.png)
![Hladinomer + Zrážkomer - Odhad maximálneho merania senzormi bez odrazov so známym priemerom studne](https://i.imgur.com/svIAGZb.png)
