/*Conversie VFD → Afișaj LED cu funcție duală: ceas & frecvențmetru
  Autor: Traian Angfheluță YO3HFP
  e-mail: yo3hfp@yahoo.com
  Data: 09.05.2025

  Acest proiect înlocuiește complet modulul original de afișaj VFD
  al receptorului de unde scurte Kenwood R-1000,
  păstrând însă funcționalitatea esențială și autenticul aspect estetic al panoului frontal.
  Inițial, receptorul era echipat cu un afișaj VFD controlat de circuitul integrat OKI MSM5524,
  greu (sau de negăsit) și imposibil de reparat în cazul defectării.
*/

#include <SevSeg.h>
#include <Wire.h>
#include "RTClib.h"
#include <FreqCount.h>

SevSeg sevseg;
RTC_DS3231 rtc;

const byte switchPin = A1;       // Comutator mod: ceas/frecventa
const byte setHourPin = 2;       // Buton setare ora
const byte setMinutePin = 8;     // Buton setare minut

unsigned long lastMillis = 0;
bool showTime = false;
bool isDim = false;
unsigned long showBrightnessUntil = 0;

void setup() {
  // Setări afișaj
  byte numDigits = 6;
  byte digitPins[] = {A2, 3, 4, A3, 0, 1};
  byte segmentPins[] = {6, 7, A0, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);  // valoare inițială: luminozitate normală

  // RTC
  Wire.begin();
  rtc.begin();

  // Pini butoane
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(setHourPin, INPUT_PULLUP);
  pinMode(setMinutePin, INPUT_PULLUP);

  // FreqCount
  FreqCount.begin(500);  // 0.5 secunde
}

void loop() {
  showTime = (digitalRead(switchPin) == LOW);

  if (showTime) {
    handleTimeSetting();     // verifică butoanele H și M pentru setare oră
    // Afișează ora doar dacă nu e activ mesajul de luminozitate
    extern unsigned long showBrightnessUntil; // declarare externă
    if (millis() >= showBrightnessUntil) {
      displayTime();
    }
  } else {
    if (FreqCount.available()) {
      unsigned long count = FreqCount.read();
      count = count * 2.0;  // compensare pentru 0.5s

      long freq = (long)count * 10 - 455000;
      if (freq < 0) freq = 0;

      freq = freq / 100;  // sute de Hz
      sevseg.setNumber(freq, 4);  // format X.XXX.X
    }
  }

  handleBrightnessToggle();  // verifică apăsarea simultană D2 + D8
  sevseg.refreshDisplay();   // actualizează display-ul
}

// Afișare HH-MM cu separator clipitor
void displayTime() {
  static unsigned long lastToggle = 0;
  static bool dashVisible = true;

  unsigned long currentMillis = millis();
  if (currentMillis - lastToggle >= 500) {
    dashVisible = !dashVisible;
    lastToggle = currentMillis;
  }

  DateTime now = rtc.now();
  int hour = now.hour();
  int minute = now.minute();

  char timeStr[7];
  if (dashVisible) {
    snprintf(timeStr, sizeof(timeStr), "%02d-%02d", hour, minute);
  } else {
    snprintf(timeStr, sizeof(timeStr), "%02d %02d", hour, minute);
  }

  sevseg.setChars(timeStr);
}

// Setare ora și minut cu debounce
void handleTimeSetting() {
  static unsigned long lastHourPress = 0;
  static unsigned long lastMinutePress = 0;
  static unsigned long lastBrightnessToggle = 0;



  bool hourPressed = digitalRead(setHourPin) == LOW;
  bool minutePressed = digitalRead(setMinutePin) == LOW;

  // Dacă ambele butoane sunt apăsate simultan
  if (hourPressed && minutePressed) {
    if (millis() - lastBrightnessToggle > 500) {
      isDim = !isDim;
      sevseg.setBrightness(isDim ? -10 : 200); // Valori recomandate in cazul afisajului OPD-S3010LR-BW (la nevoie, modificati)
      sevseg.setChars("brt");  // mesaj vizual
      showBrightnessUntil = millis() + 1000; // Afișează "brt" 1 sec.
      lastBrightnessToggle = millis();
    }
    return;
  }

  // Doar unul dintre butoane e apăsat:
  if (hourPressed && millis() - lastHourPress > 300) {
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + 1) % 24, now.minute(), 0));
    lastHourPress = millis();
  }

  if (minutePressed && millis() - lastMinutePress > 300) {
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), (now.minute() + 1) % 60, 0));
    lastMinutePress = millis();
  }

  // Dacă încă trebuie să afișeze "brt"
  if (millis() < showBrightnessUntil) {
    return; // evităm să suprascriem cu ora/frecvența
  }
}



// Schimbă luminozitatea când D2 + D8 sunt apăsate simultan
void handleBrightnessToggle() {
  static bool lastBothPressed = false;
  static bool isDim = false;

  bool hourPressed = digitalRead(setHourPin) == LOW;
  bool minutePressed = digitalRead(setMinutePin) == LOW;
  bool bothPressed = hourPressed && minutePressed;

  // Detectare tranziție LOW -> HIGH (apasare simultana)
  if (bothPressed && !lastBothPressed) {
    isDim = !isDim;
    sevseg.setBrightness(isDim ? -10 : 200); //la fel ca la linia 121
  }

  lastBothPressed = bothPressed;
}
