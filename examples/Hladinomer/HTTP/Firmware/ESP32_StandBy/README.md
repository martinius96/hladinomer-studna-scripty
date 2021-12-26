# Instructions - StandBy firmware ESP32
* You can find a description of the functions of firmware at: https://martinius96.github.io/hladinomer-studna-scripty/en/
* Each firmware folder contains the esptool.exe tool and a .bat file for automated firmware upload to the ESP8266 / ESP32 microcontroller
* **It is necessary to change the COM port in the .bat file (standardly set for COM17 for ESP32 firmware)**
* The current COM port where the microcontroller is plugged in can be found in the Device Manager
* The device is most often marked as CH340 device or CP210X depending on the used USB-UART converter
* **For upload simply double-click on .bat file, it will automatically upload firmware via ESPTOOL**
* After uploading the firmware, the command line window (CLI) will close automatically and the firmware is ready for use
* When uploading to the ESP32 microcontroller, it is often necessary to switch the microcontroller to Upload mode by holding down the BOOT button or pulldown GPIO0 if board does not have BOOT button

<p align="center">
  <img src="https://i.imgur.com/M0U6HkC.png" />
</p>

# Sensor node configuration - WiFi Manager
* After successful loading firmware to ESP board, it is almost ready to run
* If the microcontroller does not have data about the WiFi network stored in the flash memory, it will start transmitting its own SSID - Hladinomer_AP.
* WiFi network Hladinomer_AP is open without entering a password
* After connecting to a WiFi network with a smartphone / computer, the WiFi Manager web interface is available at 192.168.4.1
* At Android device, you will receive automatic notifiaction about opening captive portal, or if you put any HTTP url in your browser, it will automatically redirect you to configuration page
* Web interface provides the ability to configure a WiFi Sensor node for your home WiFi network.
* In the interface it is possible to enter the name and password of the existing WiFi network to which the WiFi thermostat will connect.
<p align="center">
  <img src="https://i.imgur.com/cJb6DR9.png" />
</p>

# Sensor Node function
* Sensor Node will run main program (measurement and sending datas) after its successful configuration to your LAN network!
* After connecting to the home WiFi network, ESP stops broadcasting SSID, switches to STA (Station) mode and it will send datas to interface of main Water Level Monitor webapp
* WiFi network data is stored in the ESP flash memory and it is no longer necessary to enter it again when restarting the thermostat, power failure, device restart.
* If the network is not available, ESP will start again broadcasting its own SSID: Hladinomer_AP and ssid and password to new WiFi network can be entered
<p align="center">
  <img src="https://i.imgur.com/A3mGI9i.png" />
</p>
