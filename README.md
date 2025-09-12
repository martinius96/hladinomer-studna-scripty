# 💧 Water Level Monitor - Arduino / ESP8266 / ESP32

This repository contains software implementations for water level monitoring using microcontrollers such as **Arduino** (ATmega328P/ATmega2560), **ESP8266**, and **ESP32**. It supports a variety of ultrasonic, ToF laser sensors to measure water levels in wells, tanks, and other water sources. The project includes data transmission via **Ethernet**, **WiFi**, and **IoT technologies** such as **Sigfox** and **LoRa / LoRaWAN**.

The firmware is designed to be highly flexible, supporting different microcontroller functions, including **Ultra-Low-Power (ULP)** modes, **Remote Over-The-Air (OTA) updates**, and real-time operating systems like **FreeRTOS** for ESP32. This makes it suitable for both small-scale DIY projects and industrial IoT water monitoring solutions.

## 🚀 Key Features
- 💧 **Real-Time Monitoring**: Ultrasonic sensors measure water levels continuously.
- 🖥 **Web Interface**: Real-time graphs, history, and alerts.
- 🔌 **Multiple Connectivity**: Ethernet, WiFi, Sigfox, LoRa.
- 🔋 **Ultra-Low Power**: Deep sleep for ESP32/ESP8266.
- 🔄 **OTA Updates**: Easy remote firmware upgrade.
- 🕹 **FreeRTOS Support**: Advanced ESP32 projects.

## Supported Sensors:
- **Ultrasonic Sensors**: e.g., HC-SR04, JSN-SR04T, IOE-SR0X, and others.
- **Laser**: e.g., VL53L1X (TOF400C), VL53L0X (TOF200C).
- **Other**: Required to do your own firmware, or set callback in case of industrial sensors

## Supported Data Transmission Protocols:
- **Ethernet**, **WiFi**, **Sigfox**, **LoRa**.
- **HTTP** and **HTTPS** support for secure web connections.
- Integration with **MySQL databases** for storing water level data.

## Web Interface Features:
- **Real-Time Data Visualization**: View water levels, trends, and volume calculations in an easy-to-understand format.
- **Historical Data**: Access past records, minimum/maximum values, and trends over time.
- **Graphical Data Representation**: Visualize water levels over the past 24 hours, 7 days, and 30 days.
- **Multilingual Support**: Available in **English**, **German**, **Russian**, and **Slovak**.

**Important Note:**  
The test web interface currently supports **HTTP protocol only**. HTTPS configurations are available in example code but will not work with the current web interface.

## Getting Started:
To start using the Water Level Monitor project, follow these steps:
1. **Clone this repository** or download the code.
2. **Install required libraries** in Arduino IDE (`/src/` folder).
3. **Upload the sketch** to your microcontroller (choose between **Offline Tester** or **Online Sketch**).
4. If using a **web interface**:  
   - Import the `.sql` table into your **MySQL** database.
   - Update `connect.php` with your MySQL access credentials and API key for authentication.
5. Set the **depth** and **diameter** of your well in the web interface settings.
6. Upload the generated sketch to your microcontroller, ensuring the API key is included.

For **OTA updates**, ensure your microcontroller is configured with **WiFi** or **Ethernet** connectivity.

## Supported Microcontroller Operation Modes:
| Microcontroller       | Offline Tester | StandBy | Deep Sleep | StandBy + OTA | FreeRTOS |
|-----------------------|:-------------:|:-------:|:----------:|:-------------:|:--------:|
| **Arduino + Ethernet** | ✅            | ✅      | ❌         | ❌            | ❌       |
| **ESP8266**            | ✅            | ✅      | ✅         | ✅            | ❌       |
| **ESP32**              | ✅            | ✅      | ✅         | ✅            | ✅       |
| **ESP32 + PHY**        | ✅            | ✅      | ✅         | ✅            | ✅       |

# Obtaining Root CA certificate (For ESP32 / ESP8266 - HTTPS) - OpenSSL tool
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null

# Supported Hardware for Water Level Monitoring

The **Water Level Monitor** project is compatible with a wide range of hardware setups, including popular microcontrollers and sensors for measuring water levels in wells, tanks, and other water bodies. Below are the supported hardware configurations and wiring diagrams to help you get started.

### 1. **Water Level Monitor - Open-Source Hardware Setup**
<img width="758" height="437" alt="ESP32-C6 + VL53L1X/TOF400C laser sensor" src="https://github.com/user-attachments/assets/1d818a1b-2686-49ae-952c-8d9454c1e6df" />
<img width="1085" height="536" alt="ESP32-C6 + HC-SR04 ultrasonic sensor" src="https://github.com/user-attachments/assets/0379c117-04b7-4799-b8b0-fe7dd1c2ae32" />

## Water Level Monitor - Web Application Screenshots

Check out the key features of the **Water Level Monitor** web app that allows you to visualize and analyze the water level data in real time.

### 1. **Real-Time Water Level Overview**
Monitor water levels and volume in real time with easy-to-read visualizations.
<img width="1125" height="683" alt="Watmonitor - Realtime dashboard" src="https://github.com/user-attachments/assets/554dce8c-fcc1-4dbd-b736-bd57957f3353" />


### 2. **Historical Datas + Gauge graphs**
View the difference in water levels over time with a comprehensive table of recorded data and minimal / maximal values recorded
<img width="1178" height="918" alt="Watmonitor - Historical data" src="https://github.com/user-attachments/assets/6aece833-b434-4002-91b8-c43cbef81f3a" />


### 3. **Graphs**
Advanced graph visualisations with possiblity to export graph in .csv, .svg, .png
<img width="1861" height="739" alt="Watmonitor - ApexCharts" src="https://github.com/user-attachments/assets/9d5c73ff-3af3-4b54-95dc-ae11ba08aa65" />


### 4. **Source code generator**
In-built source code generator for ESP32 (automatically calculated traces based on dir depth)
<img width="1163" height="865" alt="Watmonitor - source code generator" src="https://github.com/user-attachments/assets/37ecf8b2-a80d-435c-ba8d-a58ce4503586" />


### 5. **QR scanner**
QR scanner that can visualise the latest data by scanning QR code
<img width="1920" height="1445" alt="Watmonitor - QR scanner" src="https://github.com/user-attachments/assets/5983237e-c770-4fb3-b687-8e6527955a39" />

### 5. **Augmented reality**
AR visualisation of the latest data as dashboard when QR code scanned
<img width="756" height="1680" alt="Watmonitor - Augmented reality" src="https://github.com/user-attachments/assets/397c1019-1527-4200-9a24-3d66e34a9860" />
<img width="571" height="498" alt="image" src="https://github.com/user-attachments/assets/9c3ce96e-1142-49d3-a3be-dfd694e69f0a" />


