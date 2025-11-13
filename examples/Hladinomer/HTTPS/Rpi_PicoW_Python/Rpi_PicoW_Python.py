# Sketch compatible for Raspberry Pi Pico W (WiFi)
# Usage with ultrasonic distance sensors (HC-SR04 / JSN-SR04T)
# test at Wokwi Simulation: https://wokwi.com/projects/445827666971796481

import network
import urequests
from machine import Pin, deepsleep
import utime

# --- CONFIGURATION ---
WIFI_SSID = "SSID_NAME"
WIFI_PASS = "WIFI_PASSWORD"
URL = "https://hladinomer.eu/data.php"
TOKEN = "123456789"

# --- HC-SR04 PINS ---
TRIG = Pin(3, Pin.OUT)
ECHO = Pin(2, Pin.IN)

# --- WIFI CONNECTION ---
def wifi_connect():
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print("Connecting to WiFi...")
        wlan.connect(WIFI_SSID, WIFI_PASS)
        while not wlan.isconnected():
            utime.sleep_ms(500)
            print(".", end="")
    print("\nConnected:", wlan.ifconfig())
    return wlan

# --- MEASURE DISTANCE ---
def measure_distance():
    TRIG.low()
    utime.sleep_us(2)
    TRIG.high()
    utime.sleep_us(10)
    TRIG.low()

    while ECHO.value() == 0:
        signaloff = utime.ticks_us()
    while ECHO.value() == 1:
        signalon = utime.ticks_us()

    timepassed = signalon - signaloff
    distance_cm = (timepassed * 0.0343) / 2
    return round(distance_cm, 1)

# --- SEND DATA TO SERVER ---
def send_data(value):
    payload = f"hodnota={value}&token={TOKEN}"
    headers = {
        "Content-Type": "application/x-www-form-urlencoded",
        "User-Agent": "PicoW-MCU/1.0"
    }
    try:
        print("Sending:", payload)
        response = urequests.post(URL, data=payload, headers=headers)
        print("Status:", response.status_code)
        print("Response:", response.text)
        response.close()
    except Exception as e:
        print("Error sending data:", e)

# --- MAIN FUNCTION ---
def main():
    wifi_connect()
    dist = measure_distance()
    print(f"Distance: {dist} cm")
    send_data(dist)
    print("Going to deep sleep for 5 minutes...")
    deepsleep(300_000)  # 5 minutes in milliseconds

main()
