# Smart Weather & Hazard Monitoring Station (ESP8266 + Blynk IoT)

A Wi-Fi connected environmental monitoring and gas safety system built on an ESP8266 NodeMCU. It continuously measures temperature, humidity, gas level, and rain, shows live readings on an LCD and a Blynk IoT dashboard, sounds a local buzzer alert, and sends email notifications when a hazard is detected.

This is **Version 2** of the Mini Weather Station project, which was a standalone Arduino Uno prototype.

---

##  What's New in Version 2

- **ESP8266 NodeMCU Integration:** Replaced the Arduino Uno to add built-in Wi-Fi connectivity.
- **Rain Hazard Monitoring:** Added a rain sensor module; rain detection triggers a hazard alert alongside high temperature and high gas levels.
- **Blynk IoT Cloud Integration:** Real-time mobile/web dashboard with live readings, status monitoring, historical charts, and logs.
- **Remote Email Alerts:** Automated email notifications sent instantly upon hazard detection.
- **Non-blocking Architecture:** Utilizes timer-based code execution (no `delay()`) to keep cloud connections stable and responsive.
- **Hardware Fixes:** Resolved LCD display backlight power issues by bridging the I2C backpack jumper.

---

##  How It Works

**Sense ➔ Think ➔ Alert ➔ Notify**

1. **Sense:** 
   - **DHT11:** Reads ambient temperature ($^\circ\text{C}$) and relative humidity ($\%$).
   - **MQ-135:** Provides analog air quality / gas level readings.
   - **Rain Sensor:** Delivers digital wet/dry signals.
2. **Think:** The ESP8266 evaluates sensor readings against predefined threshold limits every 5 seconds.
3. **Alert:** If any parameter exceeds safety limits, the local buzzer triggers, the status LED changes, and the 16x2 LCD shows a specific hazard alert.
4. **Notify:** Real-time data streams to Blynk IoT, the status updates to **HAZARD**, and an automated email alert is dispatched.

---

### Hazard Conditions

| Condition | Threshold Trigger |
| :--- | :--- |
| **High Temperature** | Temp $> 35^\circ\text{C}$ |
| **High Gas Level** | Raw Analog Value $> 400$ *(tunable)* |
| **Rain Detected** | Digital output reads `LOW` (Wet) |

- **SAFE State:** All parameters within normal limits.
- **HAZARD State:** At least one threshold is breached.

---

### System Behavior

- **SAFE:** Green LED ON | Buzzer OFF | LCD shows live values marked `SAFE` | Blynk status reads `SAFE`
- **HAZARD:** Green LED OFF | Buzzer ON | LCD alternates between hazard messages (`OVER TEMP`, `GAS LEAK DETECT`, `RAIN DETECTED`, `MULTIPLE HAZARDS`) and live risk values | Blynk status updates to `HAZARD` | Email notification sent

---

##  Hardware Components & Circuit Design

### Components List

| Component | Purpose |
| :--- | :--- |
| ESP8266 NodeMCU (ESP-12E) | Primary Wi-Fi Microcontroller |
| DHT11 Sensor | Temperature & Humidity Sensing |
| MQ-135 Sensor | Gas Level & Air Quality Indicator |
| Rain Sensor Module | Rain / Water Drops Detection |
| 16x2 I2C LCD | Local Display |
| Active Buzzer | Audible Alarm |
| Green LED | Safe Status Indicator |
| 220 $\Omega$ Resistors (x3) | Voltage divider (x2) & LED current limiting (x1) |
| Breadboard & Jumper Wires | Circuit Connections |

---

### Pin Connections Table

| Component | Component Pin | NodeMCU Pin |
| :--- | :--- | :--- |
| **DHT11** | DATA | `D5` |
| **DHT11** | VCC / GND | `3V3` / `GND` |
| **MQ-135 Gas Sensor** | AO *(via voltage divider)* | `A0` |
| **MQ-135 Gas Sensor** | VCC / GND | `VIN` or `VU` (5V) / `GND` |
| **Rain Module** | DO | `D0` |
| **Rain Module** | VCC / GND | `3V3` / `GND` |
| **Buzzer** | + / - | `D6` / `GND` |
| **Green LED** | Anode *(via 220 $\Omega$ resistor)* / Cathode | `D7` / `GND` |
| **16x2 I2C LCD** | SDA / SCL | `D2` / `D1` |
| **16x2 I2C LCD** | VCC / GND | `VIN` or `VU` (5V) / `GND` |

*Note: On some NodeMCU development boards, the 5V pin is labeled as `VU` instead of `VIN`.*

---

### Voltage Divider Circuit (MQ-135 to ESP8266)

The ESP8266 analog pin (`A0`) supports an input voltage range of **0V – 3.3V**, whereas the MQ-135 sensor outputs up to **5V**. A voltage divider is required to step down the signal.

---

##  Software Setup & Installation

### Required Software & Libraries

1. Install **Arduino IDE** (v1.8.x or v2.x).
2. Add ESP8266 Board Support via Board Manager (`NodeMCU 1.0 (ESP-12E Module)`).
3. Install the following libraries via the **Library Manager**:
   - `Blynk` by Volodymyr Shymanskyy
   - `DHT sensor library` by Adafruit
   - `Adafruit Unified Sensor`
   - `LiquidCrystal I2C` by Frank de Brabander

---

### Blynk IoT Configuration

1. Log in to [Blynk.cloud](https://blynk.cloud/) and create a new **Template**:
   - **Hardware:** ESP8266
   - **Connection Type:** WiFi
2. Configure **Datastreams**:

| Datastream | Pin | Type | Range | History / Default |
| :--- | :--- | :--- | :--- | :--- |
| Temperature | `V0` | Double | 0 – 100 | 1 Min AVG |
| Humidity | `V1` | Double | 0 – 100 | 1 Min AVG |
| Gas Level | `V2` | Integer | 0 – 1023 | 1 Min AVG |
| System Status| `V3` | String | N/A | Last Value |
| Rain Indicator| `V4` | Integer | 0 – 1 | 1 Min AVG |

3. Create an **Event Log**:
   - Event Code: `hazard_alert`
   - Type: `Critical`
   - Enable **Email Notifications**.
4. Set up the Web / Mobile Dashboard with gauges (`V0`, `V1`, `V2`), labels (`V3`), an LED widget (`V4`), and charts.

---

### Firmware Upload

1. Open the project `.ino` file in the Arduino IDE.
2. Update your network credentials and Blynk tokens:

```cpp
#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "WEATHER STAT"
#define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"

char ssid[] = "YOUR_WIFI_NAME";      // 2.4 GHz networks only
char pass[] = "YOUR_WIFI_PASSWORD";
```

3. Connect your NodeMCU board, select the correct COM Port, and press **Upload**.
4. Open the Serial Monitor set to `115200` baud rate to check connection logs.

> ⚠️ **Security Warning:** Never push your actual Wi-Fi passwords or Blynk Auth Tokens to a public GitHub repository.

---

## 🔍 Troubleshooting Guide

| Issue | Potential Solution |
| :--- | :--- |
| **LCD stuck on "CONNECTING..."** | Check Wi-Fi credentials. Ensure network is **2.4 GHz** (ESP8266 does not support 5 GHz). |
| **LCD display backlight is OFF** | Ensure the two backlight jumper pins on the back of the I2C module are bridged. |
| **LCD power light is ON, but no text** | Adjust the blue potentiometer screw on the I2C backpack for contrast; verify I2C address (`0x27` or `0x3F`). |
| **Rain module continuously triggers** | Turn the sensitivity potentiometer screw counter-clockwise until the indicator LED turns off in dry conditions. |
| **Rain detection inverted** | Check digital output logic. Switch `== LOW` to `== HIGH` inside your firmware if necessary. |
| **Gas readings inaccurate** | Re-calibrate `GAS_THRESHOLD` based on real-time Serial readings after installing the voltage divider. |
| **Board reconnecting frequently** | Ensure power source is adequate (use a high-quality micro-USB cable connected to standard USB power). |

---

## 💡 Potential Applications

- Smart home & kitchen gas/heat monitoring systems
- Automated monitoring for server rooms, warehouses, and storage hubs
- Laboratory environmental safety compliance
- Unattended remote location status tracking

---

alisinghal70)
