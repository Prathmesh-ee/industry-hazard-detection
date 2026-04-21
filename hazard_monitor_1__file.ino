#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define FIRE 3
#define BUZZER 4
#define LED 5
#define LDR 6
#define SMOKE 7

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(FIRE, INPUT);
  pinMode(LDR, INPUT);
  pinMode(SMOKE, INPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  lcd.init();
  lcd.backlight();
  dht.begin();
  Serial.begin(9600);

  lcd.setCursor(0, 0);
  lcd.print(" INDUSTRY HAZDARDS ");
  lcd.setCursor(5, 1);
  lcd.print("MONITORING");
  lcd.setCursor(7, 2);
  lcd.print("SYSTEM");
  delay(5000);
  lcd.clear();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  int fire  = digitalRead(FIRE);
  int ldr   = digitalRead(LDR);
  int smoke = digitalRead(SMOKE);

  String dayNight;

  // DAY / NIGHT
  if (ldr == HIGH) {
    digitalWrite(LED, HIGH);
    dayNight = "NIGHT";
  } else {
    digitalWrite(LED, LOW);
    dayNight = "DAY";
  }

  // SENSOR FUSION: buzzer only if fire AND temp > 40
  // (reduces false positives from stray IR)
  bool fireAlert  = (fire == LOW) && (t > 40);
  bool smokeAlert = (smoke == LOW);

  if (fireAlert || smokeAlert) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // -------- SERIAL OUTPUT (for Python dashboard) --------
  // Format: TEMP:xx.x,HUM:xx.x,GAS:xxxx,FIRE:x,SMOKE:x,LDR:x
  Serial.print("TEMP:");   Serial.print(t);
  Serial.print(",HUM:");   Serial.print(h);
  Serial.print(",FIRE:");  Serial.print(fire == LOW ? 1 : 0);
  Serial.print(",SMOKE:"); Serial.print(smoke == LOW ? 1 : 0);
  Serial.print(",LDR:");   Serial.println(ldr == HIGH ? 1 : 0);

  // -------- LCD DISPLAY --------
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print("C H:");
  lcd.print(h, 1);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("Status:");
  lcd.print(dayNight);
  lcd.print("     ");

  lcd.setCursor(0, 2);
  lcd.print("Fire  :");
  lcd.print(fireAlert ? "DETECTED    " : "NO DETECTED ");

  lcd.setCursor(0, 3);
  lcd.print("Smoke :");
  lcd.print(smokeAlert ? "DETECTED   " : "NO DETECTED  ");

  delay(1000);
}
