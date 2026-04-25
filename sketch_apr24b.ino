#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
#define TDS_PIN 34
#define TURBIDITY_PIN 35
#define BUZZER_PIN 25
#define RELAY_PIN 23   // NEW: Relay pin

// Variables
float tdsValue = 0;
float tdsVoltage = 0;

float turbidityValue = 0;
float turbidityVoltage = 0;

// Limits
#define TDS_SAFE_LIMIT 300
#define TURBIDITY_LIMIT 2.5

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);   // NEW

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH); // Pump OFF initially (important!)

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Water Monitor");
  delay(2000);
  lcd.clear();
}

void loop() {

  // ----------- TDS SENSOR -----------
  int tdsAnalog = analogRead(TDS_PIN);
  tdsVoltage = tdsAnalog * (3.3 / 4095.0);

  tdsValue = (133.42 * tdsVoltage * tdsVoltage * tdsVoltage 
             - 255.86 * tdsVoltage * tdsVoltage 
             + 857.39 * tdsVoltage) * 0.5;

  // ----------- TURBIDITY SENSOR -----------
  int turbidityAnalog = analogRead(TURBIDITY_PIN);
  turbidityVoltage = turbidityAnalog * (3.3 / 4095.0);

  turbidityValue = turbidityVoltage;

  // ----------- DISPLAY -----------
  lcd.setCursor(0,0);
  lcd.print("TDS:");
  lcd.print((int)tdsValue);
  lcd.print("ppm ");

  lcd.setCursor(0,1);
  lcd.print("Tur:");
  lcd.print(turbidityValue, 2);
  lcd.print(" ");

  // ----------- LOGIC -----------
  if (tdsValue > TDS_SAFE_LIMIT || turbidityValue > TURBIDITY_LIMIT) {
    
    // ❌ BAD WATER → Pump OFF
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);   // OFF (active LOW relay)

    lcd.setCursor(10,1);
    lcd.print("BAD ");

  } else {
    
    // ✅ SAFE WATER → Pump ON
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);    // ON (active LOW relay)

    lcd.setCursor(10,1);
    lcd.print("GOOD");
  }

  Serial.print("TDS: ");
  Serial.print(tdsValue);
  Serial.print(" | Turbidity: ");
  Serial.println(turbidityValue);

  delay(1000);
}