# Mini Weather Station
An Arduino-based environmental monitoring system that continuously tracks
temperature, humidity, and gas concentration, displays live readings on an
LCD, and triggers immediate visual and audio alerts when a safe threshold
is crossed.
Working Project Video: https://drive.google.com/file/d/1H6hiaLtaL_GvHXFjcUX0hNfIz7xoqz9-/view?usp=drive_link
Tinkercad Stimulation: https://www.tinkercad.com/things/hLbhyswumdF-final-mse-1?sharecode=1MRBScqvQEcPyrvyUtW-kIsaw3x5rbwTijaiz8Shk8c

## Problem Statement
Gas leaks and excessive heat in enclosed spaces often go unnoticed until
they become dangerous, since manual monitoring isn't always feasible and
most alarms track only one parameter with no readable data. 
This project uses an Arduino-based system with DHT11 and gas sensors to continuously
monitor temperature, humidity, and gas levels, displaying live readings
on an LCD and triggering an LED/buzzer alert the moment a safe threshold
is crossed — enabling early detection before a hazard escalates.

## Working Principle
The DHT11 sensor measures temperature and humidity, while the gas sensor
outputs a voltage proportional to gas concentration in the air. 
The Arduino continuously reads both values and compares them against fixed
thresholds (35°C for temperature, 400 for gas). 
If either value exceeds its limit, the buzzer sounds, the LED turns off, and the LCD displays
which hazard was detected along with live readings. 
If both values stay within safe limits, the buzzer stays silent, the LED stays on, and the
LCD shows the readings as "SAFE." 
This sense-compare-alert cycle repeats continuously, giving real-time hazard detection.

## Components Used
- Arduino Uno R3
- DHT11 Temperature & Humidity Sensor
- MQ-135 Gas Sensor
- 16x2 I2C LCD Display
- Active Peizzo Buzzer
- LED
- 220Ω–330Ω resistor (for LED)

## Libraries Required
- **DHT sensor library** by Adafruit
- **Adafruit Unified Sensor** (dependency of the DHT library)
- **LiquidCrystal I2C** by Frank de Brabander

## Pin Connections

| Component        | Pin           | Arduino Pin |
|-------------------|---------------|-------------|
| DHT11             | VCC           | 5V          |
| DHT11             | DATA          | D7          |
| DHT11             | GND           | GND         |
| Gas Sensor        | VCC           | 5V          |
| Gas Sensor        | AO            | A1          |
| Gas Sensor        | GND           | GND         |
| Buzzer            | +             | D8          |
| Buzzer            | –             | GND         |
| LED               | Anode (+)     | D9 (via resistor) |
| LED               | Cathode (–)   | GND         |
| LCD               | VCC           | 5V          |
| LCD               | GND           | GND         |
| LCD               | SDA           | A4          |
| LCD               | SCL           | A5          |

## Hazard Thresholds
- Temperature: > 35°C triggers hazard state
- Gas Level: > 150 (raw analog reading) triggers hazard state

These values are defined as constants at the top of the code and can be
adjusted for different environments or sensor sensitivities.

## System Behavior

**Hazard State** (triggered if either threshold is crossed):
- Buzzer sounds continuously
- LED turns off
- LCD displays which hazard was detected (temperature, gas, or both),
  followed by a screen of live readings labeled "RISK"

**Safe State** (both readings within limits):
- Buzzer stays silent
- LED stays on
- LCD displays live readings labeled "SAFE"

## Applications

- Kitchen safety (gas leak + overheating detection)
- Home safety systems
- Laboratory and industrial environment monitoring
- Server rooms / storage areas sensitive to heat or air quality
- Warehouses and factories
- Elderly care or unattended space monitoring
- Base for future IoT extension (e.g., remote alerts via WiFi/GSM module)

4. Power the board — the LCD will show an initialization splash screen,
   then begin displaying live readings and switching between safe/hazard
   states automatically
