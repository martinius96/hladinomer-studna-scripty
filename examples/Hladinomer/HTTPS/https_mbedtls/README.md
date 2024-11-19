# HTTPS - ESP32 + ultrasonic sensor, IoT Node
* Sensor Node based on ESP32-WROOM-32 MCU, Devkit compatible
* Set WiFi SSID and WiFi PASSWORD using menuconfig tool
* Wiring, Trigger GPIO22, Echo GPIO23 (hard-coded)
# Functions
* Program is using FreeRTOS with 2 tasks
* Ultrasonic task is doing measurement via Trigger, Echo method once per 5 minutes
* If measurement performed, data is sent to the Queue
* HTTPS request task is waiting until any data are in the Queue
* If data present in Queue, HTTPS task will do HTTPS request with that data (data will be removed, so Queue is again empty)
* Maximum waiting time for HTTPS request task for data is portMAX_DELAY (almost 50 days)
* Program is using Root CA cert from bundle (test webserver was using ISRG Root X1)
