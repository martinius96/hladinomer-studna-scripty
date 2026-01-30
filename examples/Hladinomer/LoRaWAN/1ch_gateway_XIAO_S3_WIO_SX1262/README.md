* Valid for ESP32-S3 XIAO (Sense) with Wio LoRaWAN shield (SX1262)
* Upload all 3 files from this Github directory using ESPTOOL.js: https://espressif.github.io/esptool-js/
* 0x0 ofset for Bootloader, 0x8000 for Partition Table, 0x10000 for .bin app
* Download ESP BLE Provisioning app (Android / iOS)
* Open Bluetooth and pair ESP32 device (e.g. LRHB_A70A58)
* Open Serial monitor and scan the QR code shown in Serial monitor from the ESP BLE Provisioning app
* Set WiFi credentials (SSID, password)
* Search in Serial monitor optput for IP address assigned from DHCP and Gateway-ID
* Open ipadress:8000 to configure your gateway (standardly EU1 region of TTN, 868.100 MHz for RX channel with 125 kHz BW and SF7)
* Use Gateway-ID at TTN for ESP32-S3 registration under: Gateway EUI
* After registration restart ESP32-S3, will become visible on TTN, able to receive LoRaWAN packets

![3](https://github.com/user-attachments/assets/f6f5ac08-d37e-4cf3-99b2-4340b16f5486)
![4](https://github.com/user-attachments/assets/ce106dfa-b342-4a15-a592-693d7daf5ddc)
<img width="982" height="277" alt="image" src="https://github.com/user-attachments/assets/b13a35c4-bf7b-4653-9478-61baa5563e2c" />
![5](https://github.com/user-attachments/assets/033f6418-4e2e-4a63-938c-923eaa77303c)
![2](https://github.com/user-attachments/assets/baaa7388-ce90-46a0-9891-7af3cf10b868)
![1](https://github.com/user-attachments/assets/f44229c8-d038-4965-a07c-3c8307a05aea)


