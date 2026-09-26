
//  SMART WEATHER & HAZARD MONITORING STATION
// Sensors : DHT11 (temp + humidity, digital) on Pin 7
//           Gas sensor (analog) on A1
// Outputs : 16x2 I2C LCD, Buzzer on Pin 8, Safe-zone LED on Pin 9

#include <Wire.h>               // Needed for I2C communication (LCD talks over I2C)
#include <LiquidCrystal_I2C.h>  // Controls the 16x2 I2C LCD  (print text, set cursor, etc.)
#include <DHT.h>                // Reads the DHT11 temp/humidity sensor


// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  
// LCD at I2C address 0x27, 16 cols, 2 rows

// DHT11 Setup
#define DHTPIN 7        // DHT11 data pin -> Digital Pin 7
#define DHTTYPE DHT11   // Sensor model (DHT11)
DHT dht(DHTPIN, DHTTYPE); // Creates a "dht" object all readings will be pulled through this object.

// Other Pins 
const int gasPin    = A0;  // Gas sensor analog output, the more voltage output the more gas detected
const int buzzerPin = 8;   // Buzzer for hazard alerts
const int ledPin    = 9;   // Safe-zone LED (7)

// Hazard Thresholds 
const float TEMP_THRESHOLD = 35.0;   // °C, any reading above 35 is hazard
const int   GAS_THRESHOLD  = 1000;    // raw analog gas reading, above 400 hazard

void setup() {
  dht.begin();               // Start DHT11

  lcd.init();                // Correct init call for LCD library 
  lcd.backlight();           // Turns backlight on for text visibility
  pinMode(buzzerPin, OUTPUT); 
  digitalWrite(buzzerPin, LOW);   // Buzzer off initially, sends output signal

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);      // LED off initially, sends output signal
// change to high if led shows reverse working 

  lcd.setCursor(0, 0);  // Move cursor to column 0, row 0 (top-left)
  lcd.print(" Weather Station ");
  lcd.setCursor(0, 1);   // Move cursor to column 0, row 1 (bottom row)
  lcd.print(" INITIALIZING...");
  delay(2000);   // Give DHT11 time to stabilize + show splash screen
  lcd.clear();   //clears out lcd
}

void loop() {
  // READ SENSORS 
  float tempC = dht.readTemperature();  // °C from DHT11
  float humidity = dht.readHumidity();     // % humidity from DHT11
  int   gasLevel = analogRead(gasPin);     // raw 0–1023 from gas sensor

  // CHECK FOR HAZARDS 
  bool isTempHazard = (tempC > TEMP_THRESHOLD); //true if temp is too high
  bool isGasHazard  = (gasLevel > GAS_THRESHOLD); //true if gas is too high

  if (isTempHazard || isGasHazard) {
    // HAZARD STATE
    noTone(buzzerPin);         // Clear any previous buzzer state
    tone(buzzerPin, 1000);     // Sound buzzer at 1000 Hz
    digitalWrite(ledPin, LOW); // Safe LED off during hazard

    // Screen 1: Alert banner
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("! HAZARD ALERT !");
    lcd.setCursor(0, 1);
    digitalWrite(ledPin, LOW);
    if (isTempHazard && isGasHazard) { //figure out which hazard(s) triggered
      lcd.print("HIGH TEMP & GAS"); //both at once
    } else if (isTempHazard) { // temp only
      lcd.print("OVER TEMP: ");
      lcd.print((int)tempC);
      lcd.print("C");
    } else {
      lcd.print("GAS LEAK DETECT"); //gas only
    }
    delay(2500);  // Hold alert screen

    // Screen 2: Live readings + risk label
    lcd.clear();
    lcd.setCursor(0, 0);
    digitalWrite(ledPin, LOW);
    lcd.print("T:");
    lcd.print((int)tempC); //temp reading
    lcd.print("C  AQI:");
    lcd.print(gasLevel); //aqui reading 
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print((int)humidity); //humidity reading
    lcd.print("% RISK");
    delay(3000);  // Hold readings screen for 3 sec

  } else {
    // ----- SAFE STATE -----
    noTone(buzzerPin);           // Make sure buzzer is silent
    digitalWrite(ledPin, HIGH);  // Safe LED on, do low if reversed 

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print((int)tempC);
    lcd.print("C  AQI:");
    lcd.print(gasLevel);
    lcd.print("   ");            // Overwrite any leftover longer text from hazard screen
    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print((int)humidity);
    lcd.print("%  SAFE    ");
    delay(3500); //wait 3.5 sec before the next reading 
  }
}