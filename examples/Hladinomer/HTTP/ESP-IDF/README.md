# HTTP - ESP32 + ultrasonic sensor, IoT Node
* Sensor Node based on ESP32-WROOM-32 MCU, Devkit compatible
* Set WiFi SSID and WiFi PASSWORD using menuconfig tool
* Wiring, Trigger GPIO22, Echo GPIO23 (hard-coded)
# Functions
* Program is using FreeRTOS with 2 tasks
* Ultrasonic task is doing measurement via Trigger, Echo method once per 5 minutes
* If measurement performed, data is sent to the Queue
* HTTP request task is waiting until any data are in the Queue
* If data present in Queue, HTTP task will do HTTP request with that data (data will be removed, so Queue is again empty)
* Maximum waiting time for HTTP request task for data is portMAX_DELAY (almost 50 days)
