# old-bionic
This is an IoT project using a Coldplay show bracelet. This bracelet has LEDs and was used in shows with big audiences. I made an adaptation to connect a Wemos D1 Mini with some sensors in the bracelet circuit to monitoring seniors people walking. If the movement sensor detects a fall, the board sends an MQTT request to a NodeJS server warning that probably an emergency occurred

## Components:
- Wemos D1 Mini
- GY-521 MPU6050 3 axis acceleration gyroscope module
- Coldplay Bracelet Led

## Setup
1. Clone the project
2. D1 port (GPIO 5) - SCL MPU6050 port
3. D2 port (GPIO 4) - SDA MPU6050 port

## Wifi config
```
const char* SSID = "***";
const char* WIFI_PASSWORD = "***";
```

## MQTT Server config
```
const char* MQTT_SERVER = "***";
const int   MQTT_PORT = 000;
const char* MQTT_USER = "***";
const char* MQTT_PASS = "***";
const char* MQTT_TOPIC = "***";
```

![alt tag](https://i.imgur.com/oSHx3SH.jpg)
