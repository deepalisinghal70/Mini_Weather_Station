// SMART WEATHER & HAZARD MONITORING STATION - ESP8266 (NodeMCU) + BLYNK IOT
// Sensors : DHT11 (temperature + humidity), MQ gas sensor, rain sensor module
// Hazard  : high temperature OR high gas OR rain
// Outputs : 16x2 I2C LCD, buzzer, safe-zone LED, Blynk dashboard + notifications

// Blynk template details (must be before the includes)
#define BLYNK_TEMPLATE_ID   "TMPL35XOIS0Oe"
#define BLYNK_TEMPLATE_NAME "WEATHER STAT"
#define BLYNK_AUTH_TOKEN    "tvybTnMnI_q_tPZGUev3Gf7fWX88HKz7"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>          // WiFi for the ESP8266
#include <BlynkSimpleEsp8266.h>   // Blynk connection
#include <Wire.h>                 // I2C for the LCD
#include <LiquidCrystal_I2C.h>    // 16x2 I2C LCD
#include <DHT.h>                  // DHT11 sensor

char ssid[] = "Airtel_rite_1564";       // Must be a 2.4 GHz network
char pass[] = "RRR@1564";

LiquidCrystal_I2C lcd(0x27, 16, 2);   // Try 0x3F if the screen stays blank

// ---------- Pins ----------
#define DHTPIN D5                     // DHT11 data pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int gasPin    = A0;             // Gas sensor AO (through voltage divider)
const int rainPin   = D0;             // Rain module DO (LOW = rain on most modules)
const int buzzerPin = D6;             // Buzzer +
const int ledPin    = D7;             // Safe-zone LED (through 220 ohm resistor)
// LCD: SDA = D2, SCL = D1 (fixed by the board)

// ---------- Hazard thresholds ----------
const float TEMP_THRESHOLD = 35.0;    // °C, above this is a hazard
const int   GAS_THRESHOLD  = 400;     // raw 0-1023, tune after testing

BlynkTimer timer;                     // Runs tasks on a schedule without delay()

// ---------- Latest values shared between the timers ----------
float tempC = 0, humidity = 0;
int gasLevel = 0;
bool isTempHazard = false, isGasHazard = false, isRaining = false;
int lastHazardMask = 0;               // Remembers which hazards were already reported
int lcdPage = 0;                      // Which screen is showing

// Runs every 5 seconds: read sensors, decide state, send to Blynk
void readAndSend() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) return;   // Skip this cycle if the DHT misses a read
  tempC = t;
  humidity = h;
  gasLevel = analogRead(gasPin);
  isRaining = (digitalRead(rainPin) == LOW);   // Change to HIGH if your module is reversed

  isTempHazard = (tempC > TEMP_THRESHOLD);
  isGasHazard  = (gasLevel > GAS_THRESHOLD);

  // Rain counts as a hazard, same as temperature and gas
  bool hazard = isTempHazard || isGasHazard || isRaining;

  // Local alerts
  if (hazard) {
    tone(buzzerPin, 1000);            // Buzzer on
    digitalWrite(ledPin, LOW);        // Safe LED off
  } else {
    noTone(buzzerPin);                // Buzzer off
    digitalWrite(ledPin, HIGH);       // Safe LED on
  }

  // Send readings to the Blynk dashboard
  Blynk.virtualWrite(V0, tempC);      // Temperature
  Blynk.virtualWrite(V1, humidity);   // Humidity
  Blynk.virtualWrite(V2, gasLevel);   // Gas level
  Blynk.virtualWrite(V3, hazard ? "HAZARD" : "SAFE");   // Status
  Blynk.virtualWrite(V4, isRaining ? 1 : 0);            // Rain (1 = yes, 0 = no)

  // Notification: sent when a NEW hazard appears (so rain starting during a
  // temperature hazard still sends an alert, but nothing repeats every 5 s)
  int hazardMask = (isTempHazard ? 1 : 0) | (isGasHazard ? 2 : 0) | (isRaining ? 4 : 0);
  if ((hazardMask & ~lastHazardMask) != 0) {
    String msg = "Hazard! ";
    if (isTempHazard) msg += "High temp " + String((int)tempC) + "C. ";
    if (isGasHazard)  msg += "Gas level " + String(gasLevel) + ". ";
    if (isRaining)    msg += "Rain detected.";
    Blynk.logEvent("hazard_alert", msg);
  }
  lastHazardMask = hazardMask;
}

// Runs every 3 seconds: update the LCD (alternates two screens during a hazard)
void updateLCD() {
  int hazardCount = isTempHazard + isGasHazard + isRaining;   // How many hazards are active
  bool hazard = (hazardCount > 0);
  lcd.clear();
  if (hazard && lcdPage == 0) {
    lcd.setCursor(0, 0);
    lcd.print("! HAZARD ALERT !");
    lcd.setCursor(0, 1);
    if (hazardCount > 1)       lcd.print("MULTIPLE HAZARDS");
    else if (isTempHazard)     lcd.print("OVER TEMP");
    else if (isGasHazard)      lcd.print("GAS LEAK DETECT");
    else                       lcd.print("RAIN DETECTED");
    lcdPage = 1;
  } else {
    lcd.setCursor(0, 0);
    lcd.print("T:");   lcd.print((int)tempC);
    lcd.print("C G:"); lcd.print(gasLevel);
    lcd.setCursor(0, 1);
    lcd.print("H:");   lcd.print((int)humidity);
    lcd.print("% R:"); lcd.print(isRaining ? "YES " : "NO ");
    lcd.print(hazard ? "RISK" : "SAFE");
    lcdPage = 0;
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(rainPin, INPUT);
  lcd.print(" Monitoring System ");
  lcd.setCursor(0, 1);
  lcd.print(" CONNECTING...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);   // Connects WiFi + Blynk
  timer.setInterval(5000L, readAndSend);       // Sensor read + upload every 5 s
  timer.setInterval(3000L, updateLCD);         // LCD refresh every 3 s
}

void loop() {
  Blynk.run();   // Keeps the cloud connection alive
  timer.run();   // Fires the scheduled tasks
}