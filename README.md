# Smart Home System with ESP32 and Arduino

This project implements a **Smart Home System** using the **ESP32** and **Arduino UNO**. The system leverages the **ESP32** for its WiFi capabilities to host a web server, allowing users to control smart home devices and view sensor data through a browser. The **Arduino UNO** handles sensors and actuators, enabling real-time interaction with the home environment.

## Features

- **Web Server**: The ESP32 hosts a web server accessible through any browser.
- **WiFi Integration**: Control your smart home devices over WiFi from a smartphone, tablet, or computer.
- **Real-Time Data**: View sensor data collected by the Arduino via the web server.
- **Communication**:
  - **UART Protocol**: ESP32 and Arduino communicate using the UART protocol.
  - Alternatively, the **I2C Protocol** can be used for structured and scalable communication.
- **Control Devices**: Control lights, fans, and other appliances through the web interface.
- **Sensor Feedback**: Monitor temperature, humidity, or other sensor data in real-time.

## Protocols Used

### **1. UART (Default)**
- **Simple and Point-to-Point**: Direct communication between the ESP32 and Arduino.
- Suitable for this project as it involves only two devices.

### **2. I2C (Optional)**
- **Multi-Slave**: Structured communication with support for multiple devices.
- Can be implemented if the system is expanded with more sensors and actuators.

## Hardware Requirements

- **ESP32 Development Board**
- **Arduino UNO**
- **Sensors** (e.g., Temperature, Humidity, Motion Detectors)
- **Actuators** (e.g., LEDs, Relays)
- **Pull-Up Resistors** (4.7kΩ for I2C lines, if I2C is used)
- **Jumper Wires**
- **Voltage Divider or Logic Level Shifter** (For UART communication to match voltage levels)

## Wiring Setup

### UART Communication (Default)
| **ESP32 Pin** | **Arduino Pin** | **Description**       |
|---------------|-----------------|-----------------------|
| TX (GPIO1)    | RX (Pin 0)      | ESP32 sends data to Arduino |
| RX (GPIO3)    | TX (Pin 1)      | Arduino sends data to ESP32 |
| GND           | GND             | Common ground         |

### I2C Communication (Optional)
| **ESP32 Pin** | **Arduino Pin** | **Description**       |
|---------------|-----------------|-----------------------|
| SDA (GPIO21)  | SDA (A4)        | Data line             |
| SCL (GPIO22)  | SCL (A5)        | Clock line            |
| GND           | GND             | Common ground         |

## Software Overview

### ESP32 (Master)
- Hosts a **web server** to control the system via WiFi.
- Sends commands (e.g., `LED_ON`, `LED_OFF`) to the Arduino over UART or I2C.
- Receives sensor data from the Arduino and displays it on the web interface.

### Arduino UNO (Slave)
- Executes commands received from the ESP32.
- Reads data from connected sensors.
- Sends sensor readings back to the ESP32 upon request.

## Getting Started

### 1. Hardware Setup
- Wire the ESP32 and Arduino according to the chosen protocol (UART or I2C).
- Connect sensors and actuators to the Arduino as required.

### 2. Software Setup
- Install the Arduino IDE and required board packages for the ESP32 and Arduino UNO.
- Upload the respective codes:
  - **ESP32**: `esp32_code.ino`
  - **Arduino UNO**: `arduino_code.ino`

### 3. Web Server Access
- Power on the ESP32 and connect it to your WiFi network.
- Note the IP address printed on the Serial Monitor.
- Access the web server by entering the ESP32's IP address in your browser.

### 4. Test Functionality
- Use the web interface to control connected devices (e.g., turn LEDs on/off).
- View real-time sensor data on the web server.

## Future Improvements

- **Scalability**: Add more sensors and actuators using the I2C protocol.
- **Security**: Implement authentication for web server access.
- **Cloud Integration**: Connect the system to a cloud service for remote control and monitoring.

## Conclusion

This project demonstrates a flexible and scalable approach to building a smart home system using the ESP32 and Arduino. By combining the ESP32's WiFi capabilities with the Arduino's sensor handling, the system provides an interactive and real-time solution for smart home automation.

---

For further information or troubleshooting, feel free to reach out!
