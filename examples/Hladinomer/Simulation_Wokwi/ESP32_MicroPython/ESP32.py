# Project https://wokwi.com/projects/459963642603486209

import machine
import time
import network
import urequests
import sys
import esp
import gc
import re


SSID = "Wokwi-GUEST"
PASSWORD = ""
URL = "https://hladinomer.eu/data.php"

PIN_TRIGGER = 22
PIN_ECHO = 23
TIME_TO_SLEEP = 300  # seconds


trigger = machine.Pin(PIN_TRIGGER, machine.Pin.OUT)
echo = machine.Pin(PIN_ECHO, machine.Pin.IN)


print("=== ESP32 System Info ===")
gc.collect()
print(f"MicroPython: {sys.version}")
print(f"Platform:    {sys.platform}")
sdk_v = "N/A"
try:
    sdk_v = esp.idf_version()
    if not sdk_v:
        raise AttributeError
except (AttributeError, NotImplementedError):
    match = re.search(r'v\d+\.\d+(?:\.\d+)?', sys.version)
    if match:
        sdk_v = match.group(0)

print(f"SDK Version: {sdk_v}")
print(f"CPU Freq:    {machine.freq() // 1000000} MHz")
print(f"Free RAM:    {gc.mem_free() / 1024:.2f} KB")
print(f"Allocated:   {gc.mem_alloc() / 1024:.2f} KB")
print("------------------------")

print("Set distance via HC-SR04 sensor, you have 5 seconds")
time.sleep(5) 

rtc = machine.RTC()
boot_bytes = rtc.memory()
if len(boot_bytes) == 4:
    boot_count = int.from_bytes(boot_bytes, 'big') + 1
else:
    boot_count = 1

rtc.memory(boot_count.to_bytes(4, 'big'))
print(f"Boot count: {boot_count}")

def get_distance():
    vzdialenost_sum = 0
    valid_samples = 0
    
    for _ in range(10):
        trigger.value(0)
        time.sleep_us(5)
        trigger.value(1)
        time.sleep_us(10)
        trigger.value(0)
        
        try:
            duration = machine.time_pulse_us(echo, 1, 30000)
            if duration > 0:
                cm = (duration * 0.0343) / 2
                vzdialenost_sum += cm
                valid_samples += 1
        except OSError:
            pass
        
        time.sleep_ms(50)
        
    if valid_samples > 0:
        return int(vzdialenost_sum / valid_samples)
    return None

vzdialenost = get_distance()

if vzdialenost is not None:
    print(f"Average Distance: {vzdialenost} cm")
    
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(SSID, PASSWORD)
    
    print("Connecting to WiFi", end="")
    timeout = 20
    while not wlan.isconnected() and timeout > 0:
        print(".", end="")
        time.sleep(1)
        timeout -= 1
        
    if wlan.isconnected():
        print("\nWiFi Connected")
        
        headers = {
            'Content-Type': 'application/x-www-form-urlencoded',
            'User-Agent': 'ESP32-MicroPython'
        }
        data = f"hodnota={vzdialenost}&token=123456789"
        
        print("[HTTPS] Connecting...")
        try:
            response = urequests.post(URL, data=data, headers=headers)
            print(f"[HTTPS] Response code: {response.status_code}")
            response.close()
        except Exception as e:
            print(f"[HTTPS] POST failed, error: {e}")
            
        wlan.disconnect()
        wlan.active(False)
    else:
        print("\nWiFi connection failed!")
else:
    print("Error: Sensor out of range or no reading.")

print("Entering Deep Sleep...")
machine.deepsleep(TIME_TO_SLEEP * 1000)
