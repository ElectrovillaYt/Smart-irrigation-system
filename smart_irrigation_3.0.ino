
#include <LiquidCrystal_I2C.h>
#include "EEPROM.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define sensor A0
#define manual_switch 2
#define auto_switch 3
#define dry_sense 4
#define Out 5

void setup() {
  pinMode(sensor, INPUT);
  pinMode(manual_switch, INPUT_PULLUP);
  pinMode(auto_switch, INPUT_PULLUP);
  pinMode(dry_sense, INPUT);
  pinMode(Out, OUTPUT);

  EEPROM.begin();
  lcd.begin();
  lcd.backlight();

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(150);
  }
  lcd.clear();
}

int M_value;
void readmoisture() {
  M_value = analogRead(sensor);
  M_value = map(M_value, 0, 1024, 100, -1);
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(M_value);
  lcd.print("%");
  lcd.print(" ");
}

int state = 0;
void maunalmode() {
  int in = digitalRead(manual_switch);
  if (in == 0) {
    state = !state;
    digitalWrite(Out, state);
    delay(200);
  }
  if (state == 1) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP ON");
    lcd.print(" ");
  } else if (state == 0) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP OFF");
    lcd.print(" ");
  }
}


void automode() {
  if (M_value <= 20) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP ON");
    lcd.print(" ");
    digitalWrite(Out, 1);
  } else if (M_value > 20) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP OFF");
    lcd.print(" ");
    digitalWrite(Out, 0);
  }
}

int state2 = 0;
void mode_switch() {
  int value = digitalRead(auto_switch);
  if (value == 0) {
    state2 = !state2;
    EEPROM.update(0, state2);
    delay(200);
  }
  if (EEPROM.read(0) == 1) {
    lcd.setCursor(0, 1);
    lcd.print("Auto:");
    lcd.print("  ");
    automode();
  } else if (EEPROM.read(0) == 0) {
    lcd.setCursor(0, 1);
    lcd.print("Manual:");
    lcd.print("  ");
    maunalmode();
  }
}

void display() {
  readmoisture();
  mode_switch();
}


void loop() {
  display();
}
