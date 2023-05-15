#include <LiquidCrystal_I2C.h>
#include "EEPROM.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define sensor A0
#define manual_switch 2
#define auto_switch 3
#define set_m 4
#define set_m_inc 5
#define set_m_dec 6
#define W_sense 7
#define Out 8

void setup() {
  pinMode(sensor, INPUT);
  pinMode(manual_switch, INPUT_PULLUP);
  pinMode(auto_switch, INPUT_PULLUP);
  pinMode(set_m, INPUT_PULLUP);
  pinMode(set_m_dec, INPUT_PULLUP);
  pinMode(set_m_inc, INPUT_PULLUP);
  pinMode(W_sense, INPUT);
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

int value = 0;
void readmoisture() {
  value = analogRead(sensor);
  value = map(value, 0, 1024, 100, 0);
  lcd.setCursor(0, 0);
  lcd.print("Moisture: ");
  lcd.print(value);
  lcd.print("%");
  lcd.print("   ");
}

int state3 = 0;
int inc_val = EEPROM.read(1);
void set_moisture() {
  int value = digitalRead(set_m);
  if (value == 0) {
    state3 = !state3;
    delay(150);
  }
  if (state3 == 1) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(1, 0);
    lcd.print("Set Moisture:- ");
    if (digitalRead(set_m_inc) == 0) {
      if (inc_val < 80) {
        inc_val = inc_val + 1;
        delay(150);
      }
    } else if (digitalRead(set_m_dec) == 0) {
      if (inc_val > 10) {
        inc_val = inc_val - 1;
        delay(200);
      }
    }
    lcd.setCursor(0, 1);
    lcd.print("        ");
    lcd.setCursor(7, 1);
    lcd.print(inc_val);
    lcd.print("%");
    lcd.print("               ");
    EEPROM.update(1, inc_val);
  } else if (state3 == 0) {
    Home();
  }
}

int state = 0;
void maunalmode() {
  int in = digitalRead(manual_switch);
  if (in == 0) {
    state = !state;
    delay(200);
  }
  if (state == 1) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP ON");
    lcd.print(" ");
    digitalWrite(Out, state);

  } else if (state == 0) {
    lcd.setCursor(8, 1);
    lcd.print("PUMP OFF");
    lcd.print(" ");
    digitalWrite(Out, state);
  }
}

void automode() {
  if (value < EEPROM.read(1)) {
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print("PUMP ON");
    lcd.print(" ");
    digitalWrite(Out, 1);
    state = 1;
  } else if (value > EEPROM.read(1)) {
    lcd.setCursor(7, 1);
    lcd.print(" ");
    lcd.setCursor(8, 1);
    lcd.print("PUMP OFF");
    lcd.print(" ");
    digitalWrite(Out, 0);
    state = 0;
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

void Home() {
  mode_switch();
  readmoisture();
}

void display() {
  set_moisture();
}

void loop() {
  display();
}