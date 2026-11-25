#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD I2C alamat 0x27 ukuran 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Definisi Pin ---
const int PIN_TOMBOL_DARURAT = 2;   // Input Digital (Active LOW)
const int PIN_SENSOR_AIR     = A0;  // Input ADC (Slide Potentiometer)
const int PIN_POMPA_PWM      = 9;   // Output PWM (LED Biru Simulasi Pompa)
const int PIN_LED_BAHAYA     = 7;   // Output Digital (LED Merah)
const int PIN_BUZZER         = 8;   // Output Digital (Sirine Buzzer)

void setup() {
  pinMode(PIN_TOMBOL_DARURAT, INPUT_PULLUP);
  pinMode(PIN_POMPA_PWM, OUTPUT);
  pinMode(PIN_LED_BAHAYA, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("POMPA BANJIR");
  lcd.setCursor(0, 1);
  lcd.print("SISTEM SIAP...");
  delay(1200);
  lcd.clear();
}

void loop() {
  // 1. Baca Input Digital & Input ADC
  int statusTombol = digitalRead(PIN_TOMBOL_DARURAT); // 1 = lepas, 0 = tekan
  int nilaiADC     = analogRead(PIN_SENSOR_AIR);      // 0 - 1023
  int tinggiAir    = map(nilaiADC, 0, 1023, 0, 100);  // 0 - 100 cm

  int pwmPompa     = 0;
  int logikaAlarm  = LOW;
  String kondisi   = "AMAN    ";

  // 2. Evaluasi Logika Kontrol
  if (statusTombol == LOW) {
    kondisi     = "DARURAT!";
    pwmPompa    = 255;
    logikaAlarm = HIGH;
  } 
  else if (tinggiAir < 30) {
    kondisi     = "AMAN    ";
    pwmPompa    = 0;
    logikaAlarm = LOW;
  } 
  else if (tinggiAir <= 75) {
    kondisi     = "SIAGA   ";
    pwmPompa    = map(tinggiAir, 30, 75, 70, 200);
    logikaAlarm = LOW;
  } 
  else {
    kondisi     = "BAHAYA! ";
    pwmPompa    = 255;
    logikaAlarm = HIGH;
  }

  // 3. Eksekusi Output PWM & Output Digital
  analogWrite(PIN_POMPA_PWM, pwmPompa);
  digitalWrite(PIN_LED_BAHAYA, logikaAlarm);
  digitalWrite(PIN_BUZZER, logikaAlarm);

  // 4. Tampilkan Luaran ke Layar LCD 16x2
  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.print(tinggiAir);
  lcd.print("cm ");
  lcd.setCursor(10, 0);
  lcd.print("P:");
  lcd.print(pwmPompa);
  lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.print("B:");
  lcd.print(statusTombol); // Level logika Input Digital (1/0)
  lcd.print(" A:");
  lcd.print(logikaAlarm);  // Level logika Output Digital (1/0)
  lcd.print(" ");
  lcd.print(kondisi);

  delay(200);
}