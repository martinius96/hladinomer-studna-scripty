# Water Level Monitor - Arduino / ESP32 / IoT / Sigfox / LoRa

This repository contains software implementations for water level monitoring using microcontrollers such as **Arduino** (ATmega328P/ATmega2560), **ESP8266**, and **ESP32**. It supports a variety of ultrasonic sensors (e.g., RCW, HC-SR04, JSN-SR04T) to measure water levels in wells, tanks, and other water sources. The project includes data transmission via **Ethernet**, **WiFi**, and **IoT technologies** such as **Sigfox** and **LoRa**.

The firmware is designed to be highly flexible, supporting different microcontroller functions, including **Ultra-Low-Power (ULP)** modes, **Remote Over-The-Air (OTA) updates**, and real-time operating systems like **FreeRTOS** for ESP32. This makes it suitable for both small-scale DIY projects and industrial IoT water monitoring solutions.

## Key Features:
- **Real-Time Water Level Monitoring**: Collect and visualize water level data using supported ultrasonic sensors.
- **Multiple Microcontroller Support**: Compatible with **Arduino**, **ESP8266**, **ESP32**, and more.
- **Flexible Data Transmission**: Supports **Ethernet**, **WiFi**, **Sigfox**, and **LoRa** for data transfer.
- **Ultra-Low Power Mode**: ESP32 and ESP8266 firmware includes deep sleep functionality for energy efficiency.
- **OTA Firmware Updates**: Remote updates for ESP8266 and ESP32 devices, allowing easy upgrades without physical access.
- **FreeRTOS for ESP32**: Full support for the FreeRTOS real-time operating system for advanced projects.

## Supported Sensors:
- **Ultrasonic Sensors**: e.g., HC-SR04, JSN-SR04T, IOE-SR0X, and others.
- **Laser (LiDAR)**: e.g., Garmin LIDAR-Lite v3HP.
- **Hydrostatic (Submersible)**: e.g., LMK307.
- **Pressure Sensors**: Differential or compensating atmospheric sensors.
- **Capacitive, Optical, and Mechanical Sensors**: For versatile monitoring options.

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
|-----------------------|----------------|---------|------------|---------------|----------|
| **Arduino + Ethernet** | ✓              | ✓       | ✖          | ✖             | ✖        |
| **ESP8266**            | ✓              | ✓       | ✓          | ✓             | ✖        |
| **ESP32**              | ✓              | ✓       | ✓          | ✓             | ✓        |
| **ESP32 + PHY**        | ✓              | ✓       | ✓          | ✓             | ✓        |

# Obtaining Root CA certificate (For ESP32 / ESP8266 - HTTPS) - OpenSSL tool
* openssl s_client -showcerts -verify 5 -connect example.com:443 < /dev/null

# Supported Hardware for Water Level Monitoring

The **Water Level Monitor** project is compatible with a wide range of hardware setups, including popular microcontrollers and sensors for measuring water levels in wells, tanks, and other water bodies. Below are the supported hardware configurations and wiring diagrams to help you get started.

### 1. **Water Level Monitor - Open-Source Hardware Setup**
![Water level monitor open-source hardware - well water level monitor](https://i.imgur.com/RqUwKbw.jpg)

### 2. **ESP32 + PHY Ethernet LAN8720 Module with Waterproof Ultrasonic Sensor (JSN-SR04T)**
![Water level monitor - ESP32 + PHY Ethernet LAN8720 module, waterproof ultrasonic sensor JSN-SR04T](https://i.imgur.com/xLaYlmK.jpg)

### 3. **Wiring Diagrams for Microcontrollers + Ultrasonic Sensors (HC-SR04 / JSN-SR04T)**
Learn how to wire the **HC-SR04** and **JSN-SR04T** ultrasonic sensors to your microcontroller for accurate water level measurements.
![Wiring diagram - level meter - sensor HC-SR04 / JSN-SR04T](https://i.imgur.com/8OJ9TQC.png)

## Water Level Monitor - Web Application Screenshots

Check out the key features of the **Water Level Monitor** web app that allows you to visualize and analyze the water level data in real time.

### 1. **Real-Time Water Level Overview**
Monitor water levels and volume in real time with easy-to-read visualizations.
![Level meter - Overview of the water level in the well and the volume of the well in real time](https://i.imgur.com/VMLOkiW.gif)

### 2. **Historical Data Table**
View the difference in water levels over time with a comprehensive table of recorded data.
![Level meter - Table of difference records, historical data for the entire period of the well monitor](https://i.imgur.com/YrL0DG1.png)

### 3. **Max/Min Data Alerts**
Receive visual representations of max/min data to monitor water levels that exceed thresholds.
![Level meter - Alarm representation of maximum, minimum data](https://i.imgur.com/FqqGV8o.png)

### 4. **Graphical Data Representation**
View water level data over different time frames: 24 hours, 7 days, 30 days.
![Level meter - Graphical representation of measured data in 24 hours, 7 days, 30 days](https://i.imgur.com/3ynXOBb.png)

### 5. **ESP32 - FreeRTOS & HTTPS Connectivity**
Integration of **ESP32** with **FreeRTOS** and secure **HTTPS** connectivity for reliable and secure data transmission.
![Level meter - ESP32 - ESP-IDF- FreeRTOS, HTTPS connectivity](https://i.imgur.com/xyhyH8A.png)

---

# Supported Water Level Sensors

The **Water Level Monitor** project supports various sensor types, allowing for flexible and accurate water monitoring solutions. Below are the supported sensor types:

### 1. **Laser (LiDAR) Sensors**
- **Garmin LIDAR-Lite v3HP**
- **TFMini**

### 2. **Hydrostatic (Submersible) Sensors**
- **LMK307**
- **LMP305**
- **DPT200**

### 3. **Electrostatic Sensors**
- Capacitive and inductive sensors for non-contact water level measurements.

### 4. **Pressure Sensors**
- **Differential Pressure Sensors**
- **Atmospheric Pressure Compensating Sensors**

### 5. **Optical Sensors**
- **GP2Y0A02YK0F** for accurate detection of water levels using light-based sensing.

### 6. **Mechanical (Float) Sensors**
- Float-based sensors for traditional water level measurement methods.

### 7. **Magnetic (Hall Effect) Sensors**
- Hall effect sensors for precise water level detection.

### 8. **Microwave (Radar) Sensors**
- **CS476**
- **CS475A-L**
- **SP69**
- **WLR 8/15/30**

### 9. **Ultrasonic Sensors with Custom Outputs**
- Supported sensor outputs include **UART**, **RS-232**, **current loop 4-20 mA**, **Modbus TCP/RTU**, **M-bus**, **RS-485**, **PROFINET**, **CAN**, **SDI**, **DQ**, and more.

**Note:** If you are using a sensor that calculates water level from the bottom (e.g., submersible or pressure sensors), ensure to set the well depth to **0 cm** in the web interface for accurate readings.

---

By utilizing a variety of sensors, the **Water Level Monitor** project can be adapted to suit different environments, from residential water tanks to industrial-scale water management systems. Whether you're using ultrasonic, radar, or pressure sensors, you can tailor the system to your specific needs.
