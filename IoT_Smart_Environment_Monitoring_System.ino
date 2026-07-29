#define BLYNK_TEMPLATE_ID "TMPL3AzC4NIZq"
#define BLYNK_TEMPLATE_NAME "IoT Smart Environment Monitoring System"
#define BLYNK_AUTH_TOKEN "AE6BBdQ8fOs4ZqYWJVrapkohF2EzqAoK"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define DHTPIN      4
#define DHTTYPE DHT22
#define MQ2_PIN     34
#define LED_PIN     25
#define BUZZER_PIN  26

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

const int GAS_THRESHOLD = 400;

void updateSensors() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gas = analogRead(MQ2_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("DHT read failed!");
    return;
  }

  String status = (gas > GAS_THRESHOLD) ? "DANGER" : "SAFE";

  digitalWrite(LED_PIN, gas > GAS_THRESHOLD);
  digitalWrite(BUZZER_PIN, gas > GAS_THRESHOLD);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(t,1);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print((int)h);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("G:");
  lcd.print(gas);
  lcd.print(" ");
  lcd.print(status);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, gas);
  Blynk.virtualWrite(V3, status);

  Serial.println("==========================");
  Serial.print("Temperature : "); Serial.print(t); Serial.println(" C");
  Serial.print("Humidity    : "); Serial.print(h); Serial.println(" %");
  Serial.print("Gas Value   : "); Serial.println(gas);
  Serial.print("Status      : "); Serial.println(status);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();

  Wire.begin(21,22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("IoT Smart Env");
  lcd.setCursor(0,1);
  lcd.print("Monitoring");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, updateSensors);
}

void loop() {
  Blynk.run();
  timer.run();
}
