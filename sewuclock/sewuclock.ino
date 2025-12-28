/*
 * SEWU AUDIO - Clock V3.7 FINAL
 * 2 PANEL - STABLE (NO EEPROM) + ALARM
 */

#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RtcDS3231.h>
#include <HJS589.h>
#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>

#include "sewu_webpage.h"

// Display
HJS589 Disp(2, 1);

void ICACHE_RAM_ATTR displayRefresh() {
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);
}

// RTC
RtcDS3231<TwoWire> Rtc(Wire);

// Globals
int brightness = 100;
uint8_t jam = 0, menit = 0, detik = 0, tgl = 1, bln = 1, dow = 0;
int suhu = 25;
bool format24 = true; // true = 24h, false = 12h

// Buzzer
#define BUZZER_PIN 3  // RX Pin (GPIO3)

// Alarm Structure
struct Alarm {
  uint8_t hour;
  uint8_t minute;
  bool enabled;
  uint8_t tune; // 0=Nokia, 1=Casio, 2=Beep, 3=Melody, 4=Urgent
};

Alarm alarms[2] = {
  {7, 0, false, 0},
  {12, 0, false, 1}
};

// Buzzer State
unsigned long buzzerStartTime = 0;
int buzzerBeepCount = 0;
bool buzzerActive = false;
int currentAlarmIndex = -1;

// Web
ESP8266WebServer server(80);

// Safe hari/bulan
const char* HARI[] = {"MGU", "SEN", "SEL", "RAB", "KAM", "JUM", "SAB"};
const char* BULAN[] = {"", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES"};

const char* getHari() { return HARI[dow % 7]; }
const char* getBulan() { return BULAN[(bln > 0 && bln < 13) ? bln : 1]; }

// ═══════════════════════════════════════════════════════════════════

void readRTC() {
  RtcDateTime now = Rtc.GetDateTime();
  jam = now.Hour();
  menit = now.Minute();
  detik = now.Second();
  tgl = now.Day();
  bln = now.Month();
  dow = now.DayOfWeek();
  
  RtcTemperature t = Rtc.GetTemperature();
  suhu = (int)t.AsFloatDegC();
}

void showClock() {
  static uint8_t lastSec = 255;
  if (detik == lastSec) return;
  lastSec = detik;
  
  // Convert hour based on format
  uint8_t displayHour = jam;
  if (!format24) {
    // 12-hour format (no AM/PM on display, just the number)
    displayHour = jam % 12;
    if (displayHour == 0) displayHour = 12; // Midnight/Noon = 12
  }
  
  char s1[3], s2[3], s3[3], s4[3];
  sprintf(s1, "%02d", displayHour);
  sprintf(s2, "%02d", menit);
  sprintf(s3, "%02d", detik);
  sprintf(s4, "%02d", tgl);
  
  Disp.clear();
  
  // Panel 1: JAM
  Disp.setFont(ElektronMart6x16);
  Disp.drawText(1, 0, s1);
  
  // Colon
  Disp.drawRect(14, 4, 15, 5, 0, 1);
  Disp.drawRect(14, 10, 15, 11, 0, 1);
  
  // Menit/Detik
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(18, 0, s2);
  Disp.drawText(18, 8, s3);
  
  // Panel 2: Tanggal
  Disp.setFont(ElektronMart6x16);
  Disp.drawText(35, 0, s4);
  
  // Hari/Bulan
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(50, 0, getHari());
  Disp.drawText(50, 8, getBulan());
}

// ═══════════════════════════════════════════════════════════════════
// BUZZER & ALARM

void startBuzzer(int alarmIdx) {
  buzzerActive = true;
  buzzerStartTime = millis();
  buzzerBeepCount = 0;
  currentAlarmIndex = alarmIdx;
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.print(F("[ALARM] Triggered #"));
  Serial.println(alarmIdx);
}

void updateBuzzer() {
  if (!buzzerActive) return;
  
  unsigned long elapsed = millis() - buzzerStartTime;
  uint8_t tune = alarms[currentAlarmIndex].tune;
  
  // Tune patterns - Classic Ringtones
  if (tune == 0) {
    // NOKIA: Classic Nokia tune pattern (iconic!)
    // Fast triple beep pattern
    int cycle = elapsed / 600;
    if (cycle >= 4) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      currentAlarmIndex = -1;
      return;
    }
    int pos = elapsed % 600;
    if (pos < 100 || (pos >= 150 && pos < 250) || (pos >= 300 && pos < 400)) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
  else if (tune == 1) {
    // CASIO: Classic watch beep (short and sweet)
    // Two quick beeps
    int cycle = elapsed / 500;
    if (cycle >= 2) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      currentAlarmIndex = -1;
      return;
    }
    int pos = elapsed % 500;
    if (pos < 80 || (pos >= 120 && pos < 200)) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
  else if (tune == 2) {
    // BEEP: Simple continuous beeps
    // 5 medium beeps
    int cycle = elapsed / 600;
    if (cycle >= 5) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      currentAlarmIndex = -1;
      return;
    }
    digitalWrite(BUZZER_PIN, (elapsed % 600) < 300 ? HIGH : LOW);
  }
  else if (tune == 3) {
    // MELODY: Musical pattern (do-re-mi style)
    // Variable length beeps
    int cycle = elapsed / 1000;
    if (cycle >= 3) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      currentAlarmIndex = -1;
      return;
    }
    int pos = elapsed % 1000;
    if (pos < 200 || (pos >= 300 && pos < 450) || (pos >= 550 && pos < 750)) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
  else if (tune == 4) {
    // URGENT: Rapid alarm (wake up NOW!)
    // 10 fast beeps
    int cycle = elapsed / 200;
    if (cycle >= 10) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
      currentAlarmIndex = -1;
      return;
    }
    digitalWrite(BUZZER_PIN, (elapsed % 200) < 100 ? HIGH : LOW);
  }
}

void stopBuzzer() {
  digitalWrite(BUZZER_PIN, LOW);
  buzzerActive = false;
  currentAlarmIndex = -1;
}

void checkAlarms() {
  static uint8_t lastMinute = 255;
  
  // Only check once per minute
  if (menit == lastMinute) return;
  lastMinute = menit;
  
  // Check all enabled alarms
  for (int i = 0; i < 2; i++) {
    if (alarms[i].enabled && alarms[i].hour == jam && alarms[i].minute == menit) {
      startBuzzer(i);
      break; // Only trigger one alarm at a time
    }
  }
}

// ═══════════════════════════════════════════════════════════════════

void setupWeb() {
  server.on("/", HTTP_GET, []() {
    if (server.hasArg("time")) {
      String t = server.arg("time");
      RtcDateTime now = Rtc.GetDateTime();
      RtcDateTime newDt(now.Year(), now.Month(), now.Day(),
                        t.substring(0,2).toInt(), t.substring(3,5).toInt(), 0);
      Rtc.SetDateTime(newDt);
    }
    if (server.hasArg("date")) {
      String d = server.arg("date");
      RtcDateTime now = Rtc.GetDateTime();
      RtcDateTime newDt(d.substring(0,4).toInt(), d.substring(5,7).toInt(), 
                        d.substring(8,10).toInt(), now.Hour(), now.Minute(), 0);
      Rtc.SetDateTime(newDt);
    }
    server.send_P(200, "text/html", webpage);
  });
  
  server.on("/api/data", HTTP_GET, []() {
    String j = "{\"hour\":";
    j += jam;
    j += ",\"minute\":";
    j += menit;
    j += ",\"second\":";
    j += detik;
    j += ",\"day\":";
    j += tgl;
    j += ",\"month\":";
    j += bln;
    j += ",\"dayName\":\"";
    j += getHari();
    j += "\",\"temp\":";
    j += suhu;
    j += ",\"brightness\":";
    j += brightness;
    j += ",\"format24\":";
    j += format24 ? "true" : "false";
    j += "}";
    server.send(200, "application/json", j);
  });
  
  server.on("/api/display", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String d = server.arg("plain");
      int i = d.indexOf("brightness\":");
      if (i > 0) {
        int v = d.substring(i + 12).toInt();
        if (v >= 18 && v <= 255) {
          brightness = v;
          Disp.setBrightness(brightness);
          Serial.print(F("[B] "));
          Serial.println(brightness);
        }
      }
      int f = d.indexOf("format24\":");
      if (f > 0) {
        format24 = (d.indexOf("true", f) > 0);
        Serial.print(F("[FMT] "));
        Serial.println(format24 ? "24h" : "12h");
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  // Alarm API - Get all alarms
  server.on("/api/alarms", HTTP_GET, []() {
    String j = "{\"alarms\":[";
    for (int i = 0; i < 2; i++) {
      if (i > 0) j += ",";
      j += "{\"id\":";
      j += i;
      j += ",\"hour\":";
      j += alarms[i].hour;
      j += ",\"minute\":";
      j += alarms[i].minute;
      j += ",\"enabled\":";
      j += alarms[i].enabled ? "true" : "false";
      j += ",\"tune\":";
      j += alarms[i].tune;
      j += "}";
    }
    j += "],\"active\":";
    j += buzzerActive ? "true" : "false";
    j += ",\"currentAlarm\":";
    j += currentAlarmIndex;
    j += "}";
    server.send(200, "application/json", j);
  });
  
  // Alarm API - Update alarm
  server.on("/api/alarms", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      
      // Parse alarm data
      int idIdx = body.indexOf("\"id\":");
      int hourIdx = body.indexOf("\"hour\":");
      int minIdx = body.indexOf("\"minute\":");
      int enIdx = body.indexOf("\"enabled\":");
      int tuneIdx = body.indexOf("\"tune\":");
      
      if (idIdx > 0) {
        int id = body.substring(idIdx + 5).toInt();
        if (id >= 0 && id < 2) {
          if (hourIdx > 0) {
            alarms[id].hour = body.substring(hourIdx + 7).toInt();
          }
          if (minIdx > 0) {
            alarms[id].minute = body.substring(minIdx + 9).toInt();
          }
          if (enIdx > 0) {
            alarms[id].enabled = (body.indexOf("true", enIdx) > 0);
          }
          if (tuneIdx > 0) {
            alarms[id].tune = body.substring(tuneIdx + 7).toInt();
          }
          Serial.print(F("[ALARM] Updated #"));
          Serial.println(id);
        }
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  // Stop buzzer manually
  server.on("/api/stop", HTTP_POST, []() {
    stopBuzzer();
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  // Test/Preview tune
  server.on("/api/test", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      int tuneIdx = body.indexOf("\"tune\":");
      if (tuneIdx > 0) {
        int tune = body.substring(tuneIdx + 7).toInt();
        if (tune >= 0 && tune <= 4) {
          // Stop any active alarm first
          stopBuzzer();
          // Set temporary test alarm
          currentAlarmIndex = 0; // Use first alarm for test
          uint8_t origTune = alarms[0].tune;
          alarms[0].tune = tune;
          // Start test buzzer
          buzzerActive = true;
          buzzerStartTime = millis();
          digitalWrite(BUZZER_PIN, HIGH);
          Serial.print(F("[TEST] Tune #"));
          Serial.println(tune);
          // Restore original tune after a brief moment
          // (will be overridden by updateBuzzer anyway)
        }
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  server.begin();
}

// ═══════════════════════════════════════════════════════════════════

void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== SEWU AUDIO V3.7 ==="));
  
  // Buzzer Pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println(F("[BUZZER] OK"));
  
  // RTC
  Wire.begin();
  Rtc.Begin();
  if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);
  Serial.println(F("[RTC] OK"));
  
  // WiFi
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SEWU CLOCK", "sewuaudio123");
  Serial.println(F("[WiFi] OK"));
  
  // Web
  setupWeb();
  Serial.println(F("[WEB] OK"));
  
  // Display
  Disp.start();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  Disp.setBrightness(brightness);
  Serial.println(F("[DISP] OK"));
  
  
  // Branding
  Disp.clear();
  Disp.setFont(ElektronMart6x8);
  Disp.drawText(7, 4, "SEWU CLOCK");
  delay(2000);
  Disp.clear();
  
  Serial.println(F("=== READY ===\n"));
}

void loop() {
  server.handleClient();
  yield();
  
  readRTC();
  showClock();
  checkAlarms();
  updateBuzzer();
  
  delay(50);
}
