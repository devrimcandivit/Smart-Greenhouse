# Smart Greenhouse Automation System

This is an Arduino based embedded system designed to automate greenhouse environment control. This project uses Object Oriented Programming concepts via C++ struct to support different plant profiles like Amasya Apple and Mersin Banana. It also implements sensor fusion by averaging multiple moisture sensors for industrial grade reliability.

## Hardware Components

* Microcontroller: Arduino Uno or Nano
* Sensors:
  * DHT11 Temperature and Humidity Sensor
  * 3x Analog Soil Moisture Sensors for spatial averaging
  * Analog Water Level Sensor
* Actuators:
  * 3x 5V Relay Modules for Pump, Heater, and Cooler
* UI and Alerts:
  * 16x2 I2C LCD Display
  * Active Buzzer and Warning LED

## Pinout Table

| Component | Arduino Pin | Type | Note |
| --- | --- | --- | --- |
| DHT11 | D2 | Digital I/O | Data signal |
| Soil Moisture 1 | A0 | Analog Input | Array Index 0 |
| Soil Moisture 2 | A2 | Analog Input | Array Index 1 |
| Soil Moisture 3 | A3 | Analog Input | Array Index 2 |
| Water Level | A1 | Analog Input | Safety check |
| Heater Relay | D4 | Digital Output | Active HIGH trigger |
| Cooler Relay | D5 | Digital Output | Active HIGH trigger |
| Pump Relay | D6 | Digital Output | Active HIGH trigger |
| Warning LED | D7 | Digital Output | With 220 ohm resistor |
| Warning Buzzer| D8 | Digital Output | Active buzzer |
| 16x2 LCD | A4 SDA, A5 SCL | I2C Bus | Hardware I2C pins |

## Control Logic

The system operates on a continuous feedback loop with advanced protocols.

1. Plant Profiles: Configured using C++ struct. Modifying a single variable reconfigures the entire greenhouse temperature and moisture thresholds.
2. Sensor Averaging: Reads 3 different soil moisture sensors via a for loop, eliminating single point hardware failures and false readings.
3. Dynamic Temperature Control: Activates the heater or cooler based on the minimum and maximum temperature values defined in the active plant profile.
4. Safety Override: If the water level sensor detects an empty tank, it initiates an emergency stop. The pump relay is hardware locked to LOW, preventing motor burnout.
