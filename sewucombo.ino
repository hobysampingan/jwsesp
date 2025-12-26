/*
 * SEWU AUDIO COMBO v1.0
 * JAM + RUNNING TEXT
 * 
 * Fitur:
 * - Tampilan Jam + Tanggal + Hari (mode 0)
 * - Running Text 5 pesan (mode 1)
 * - Auto cycle atau manual switch
 * - WiFi AP stabil
 * - Web config dengan dirty tracking
 * 
 * Hardware:
 * - NodeMCU ESP8266
 * - P10 LED 2 Panel (64x16)
 * - RTC DS3231
 */

#include <SPI.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RtcDS3231.h>
#include <HJS589.h>
#include <fonts/ElektronMart5x6.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>


#include "sewucombo_page.h"

// ═══════════════════════════════════════════════════════════════════
// DISPLAY
// ═══════════════════════════════════════════════════════════════════

#define PANEL_COUNT 2
HJS589 Disp(PANEL_COUNT, 1);

void ICACHE_RAM_ATTR displayRefresh() {
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);
}

// ═══════════════════════════════════════════════════════════════════
// RTC
// ═══════════════════════════════════════════════════════════════════

RtcDS3231<TwoWire> Rtc(Wire);
uint8_t jam = 0, menit = 0, detik = 0, tgl = 1, bln = 1, dow = 0;
int suhu = 25;
uint16_t tahun = 2024;

const char* HARI[] = {"MGU", "SEN", "SEL", "RAB", "KAM", "JUM", "SAB"};
const char* BULAN[] = {"", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES"};

const char* getHari() { return HARI[dow % 7]; }
const char* getBulan() { return BULAN[(bln > 0 && bln < 13) ? bln : 1]; }

void readRTC() {
  RtcDateTime now = Rtc.GetDateTime();
  jam = now.Hour();
  menit = now.Minute();
  detik = now.Second();
  tgl = now.Day();
  bln = now.Month();
  tahun = now.Year();
  dow = now.DayOfWeek();
  
  RtcTemperature t = Rtc.GetTemperature();
  suhu = (int)t.AsFloatDegC();
}

// ═══════════════════════════════════════════════════════════════════
// CONFIG
// ═══════════════════════════════════════════════════════════════════

struct Config {
  char nama[64];
  char info1[200];
  char info2[200];
  char info3[200];
  char info4[200];
  char info5[200];
  uint8_t enable1, enable2, enable3, enable4, enable5;
  char ssid[32];
  char password[32];
  uint8_t brightness;
  uint8_t speed;
  uint8_t displayMode;  // 0=jam, 1=running, 2=auto cycle
  uint8_t separator;
  uint8_t jamDuration;  // durasi tampilan jam (detik)
  uint8_t format24h;    // 1=24h, 0=12h
  uint8_t alarmEnable;
  uint8_t alarmHour;
  uint8_t alarmMinute;
  uint8_t alarmDuration; // detik
  uint8_t alarmTune;
};

Config cfg;
const char* CONFIG_FILE = "/config.json";

void loadConfig() {
  // Defaults - Jangan diubah agar tetap ada fallback yang aman
  strlcpy(cfg.nama, "SEWU AUDIO", sizeof(cfg.nama));
  strlcpy(cfg.info1, "SELAMAT DATANG", sizeof(cfg.info1));
  strlcpy(cfg.info2, "SEWU AUDIO SOUND SYSTEM", sizeof(cfg.info2));
  strlcpy(cfg.info3, "PROFESSIONAL LIGHTING", sizeof(cfg.info3));
  strlcpy(cfg.info4, "", sizeof(cfg.info4));
  strlcpy(cfg.info5, "", sizeof(cfg.info5));
  cfg.enable1 = cfg.enable2 = cfg.enable3 = 1;
  cfg.enable4 = cfg.enable5 = 0;
  strlcpy(cfg.ssid, "SEWU AUDIO", sizeof(cfg.ssid));
  strlcpy(cfg.password, "sewuaudio123", sizeof(cfg.password));
  cfg.brightness = 100;
  cfg.speed = 35;
  cfg.displayMode = 2;  // auto cycle
  cfg.separator = 0;
  cfg.jamDuration = 10;
  cfg.format24h = 1;    // Default 24 jam
  cfg.alarmEnable = 0;
  cfg.alarmHour = 12;
  cfg.alarmMinute = 0;
  cfg.alarmDuration = 60; // Default 1 menit
  cfg.alarmTune = 0;
  
  if (!SPIFFS.exists(CONFIG_FILE)) {
    Serial.println(F("[CFG] File not found, using defaults"));
    return;
  }
  
  File file = SPIFFS.open(CONFIG_FILE, "r");
  if (!file) {
    Serial.println(F("[CFG] Open for read fail"));
    return;
  }
  
  StaticJsonDocument<2048> doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  
  if (error) {
    Serial.print(F("[CFG] Parse err: "));
    Serial.println(error.c_str());
    return;
  }
  
  strlcpy(cfg.nama, doc["nama"] | cfg.nama, sizeof(cfg.nama));
  strlcpy(cfg.info1, doc["info1"] | cfg.info1, sizeof(cfg.info1));
  strlcpy(cfg.info2, doc["info2"] | cfg.info2, sizeof(cfg.info2));
  strlcpy(cfg.info3, doc["info3"] | cfg.info3, sizeof(cfg.info3));
  strlcpy(cfg.info4, doc["info4"] | cfg.info4, sizeof(cfg.info4));
  strlcpy(cfg.info5, doc["info5"] | cfg.info5, sizeof(cfg.info5));
  cfg.enable1 = doc["enable1"] | cfg.enable1;
  cfg.enable2 = doc["enable2"] | cfg.enable2;
  cfg.enable3 = doc["enable3"] | cfg.enable3;
  cfg.enable4 = doc["enable4"] | cfg.enable4;
  cfg.enable5 = doc["enable5"] | cfg.enable5;
  strlcpy(cfg.ssid, doc["ssid"] | cfg.ssid, sizeof(cfg.ssid));
  strlcpy(cfg.password, doc["password"] | cfg.password, sizeof(cfg.password));
  cfg.brightness = doc["brightness"] | cfg.brightness;
  cfg.speed = doc["speed"] | cfg.speed;
  cfg.displayMode = doc["displayMode"] | cfg.displayMode;
  cfg.separator = doc["separator"] | cfg.separator;
  cfg.jamDuration = doc["jamDuration"] | cfg.jamDuration;
  cfg.format24h = doc["format24h"] | cfg.format24h;
  cfg.alarmEnable = doc["alarmEnable"] | cfg.alarmEnable;
  cfg.alarmHour = doc["alarmHour"] | cfg.alarmHour;
  cfg.alarmMinute = doc["alarmMinute"] | cfg.alarmMinute;
  cfg.alarmDuration = doc["alarmDuration"] | cfg.alarmDuration;
  cfg.alarmTune = doc["alarmTune"] | cfg.alarmTune;
  
  Serial.print(F("[CFG] Loaded ok, mode="));
  Serial.println(cfg.displayMode);
}

void saveConfig() {
  Serial.println(F("[CFG] Saving..."));
  // Stop interrupts & Refresh LED during Flash Write
  timer0_detachInterrupt();
  yield();
  
  StaticJsonDocument<2048> doc;
  doc["nama"] = cfg.nama;
  doc["info1"] = cfg.info1;
  doc["info2"] = cfg.info2;
  doc["info3"] = cfg.info3;
  doc["info4"] = cfg.info4;
  doc["info5"] = cfg.info5;
  doc["enable1"] = cfg.enable1;
  doc["enable2"] = cfg.enable2;
  doc["enable3"] = cfg.enable3;
  doc["enable4"] = cfg.enable4;
  doc["enable5"] = cfg.enable5;
  doc["ssid"] = cfg.ssid;
  doc["password"] = cfg.password;
  doc["brightness"] = cfg.brightness;
  doc["speed"] = cfg.speed;
  doc["displayMode"] = cfg.displayMode;
  doc["separator"] = cfg.separator;
  doc["jamDuration"] = cfg.jamDuration;
  doc["format24h"] = cfg.format24h;
  doc["alarmEnable"] = cfg.alarmEnable;
  doc["alarmHour"] = cfg.alarmHour;
  doc["alarmMinute"] = cfg.alarmMinute;
  doc["alarmDuration"] = cfg.alarmDuration;
  doc["alarmTune"] = cfg.alarmTune;
  
  File file = SPIFFS.open(CONFIG_FILE, "w");
  if (file) {
    if (serializeJson(doc, file) == 0) {
      Serial.println(F("[CFG] Write fail"));
    } else {
      Serial.println(F("[CFG] Saved OK"));
    }
    file.close();
  } else {
    Serial.println(F("[CFG] Open for write FAIL"));
  }
  
  yield();
  // Restart Display Interrupts
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  yield();
}

// ═══════════════════════════════════════════════════════════════════
// WEB SERVER
// ═══════════════════════════════════════════════════════════════════

ESP8266WebServer server(80);
IPAddress localIP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    // Handle time/date set
    if (server.hasArg("time")) {
      String t = server.arg("time");
      RtcDateTime now = Rtc.GetDateTime();
      Rtc.SetDateTime(RtcDateTime(now.Year(), now.Month(), now.Day(),
                      t.substring(0,2).toInt(), t.substring(3,5).toInt(), 0));
    }
    if (server.hasArg("date")) {
      String d = server.arg("date");
      RtcDateTime now = Rtc.GetDateTime();
      Rtc.SetDateTime(RtcDateTime(d.substring(0,4).toInt(), d.substring(5,7).toInt(), 
                      d.substring(8,10).toInt(), now.Hour(), now.Minute(), 0));
    }
    server.send_P(200, "text/html", webpage);
  });
  
  server.on("/api/data", HTTP_GET, []() {
    String j = "{";
    j += "\"hour\":" + String(jam) + ",";
    j += "\"minute\":" + String(menit) + ",";
    j += "\"second\":" + String(detik) + ",";
    j += "\"day\":" + String(tgl) + ",";
    j += "\"month\":" + String(bln) + ",";
    j += "\"year\":" + String(tahun) + ",";
    j += "\"dayName\":\"" + String(getHari()) + "\",";
    j += "\"temp\":" + String(suhu) + ",";
    j += "\"nama\":\"" + String(cfg.nama) + "\",";
    j += "\"info1\":\"" + String(cfg.info1) + "\",";
    j += "\"info2\":\"" + String(cfg.info2) + "\",";
    j += "\"info3\":\"" + String(cfg.info3) + "\",";
    j += "\"info4\":\"" + String(cfg.info4) + "\",";
    j += "\"info5\":\"" + String(cfg.info5) + "\",";
    j += "\"enable1\":" + String(cfg.enable1) + ",";
    j += "\"enable2\":" + String(cfg.enable2) + ",";
    j += "\"enable3\":" + String(cfg.enable3) + ",";
    j += "\"enable4\":" + String(cfg.enable4) + ",";
    j += "\"enable5\":" + String(cfg.enable5) + ",";
    j += "\"ssid\":\"" + String(cfg.ssid) + "\",";
    j += "\"brightness\":" + String(cfg.brightness) + ",";
    j += "\"speed\":" + String(cfg.speed) + ",";
    j += "\"displayMode\":" + String(cfg.displayMode) + ",";
    j += "\"separator\":" + String(cfg.separator) + ",";
    j += "\"jamDuration\":" + String(cfg.jamDuration) + ",";
    j += "\"format24h\":" + String(cfg.format24h) + ",";
    j += "\"alarmEnable\":" + String(cfg.alarmEnable) + ",";
    j += "\"alarmHour\":" + String(cfg.alarmHour) + ",";
    j += "\"alarmMinute\":" + String(cfg.alarmMinute) + ",";
    j += "\"alarmDuration\":" + String(cfg.alarmDuration) + ",";
    j += "\"alarmTune\":" + String(cfg.alarmTune);
    j += "}";
    server.send(200, "application/json", j);
  });
  
  server.on("/api/info", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<512> doc;
      String payload = server.arg("plain");
      yield();  // Beri waktu untuk WiFi
      if (!deserializeJson(doc, payload)) {
        strlcpy(cfg.nama, doc["nama"] | cfg.nama, sizeof(cfg.nama));
        strlcpy(cfg.info1, doc["info1"] | cfg.info1, sizeof(cfg.info1));
        strlcpy(cfg.info2, doc["info2"] | cfg.info2, sizeof(cfg.info2));
        strlcpy(cfg.info3, doc["info3"] | cfg.info3, sizeof(cfg.info3));
        strlcpy(cfg.info4, doc["info4"] | cfg.info4, sizeof(cfg.info4));
        strlcpy(cfg.info5, doc["info5"] | cfg.info5, sizeof(cfg.info5));
        cfg.enable1 = doc["enable1"] | cfg.enable1;
        cfg.enable2 = doc["enable2"] | cfg.enable2;
        cfg.enable3 = doc["enable3"] | cfg.enable3;
        cfg.enable4 = doc["enable4"] | cfg.enable4;
        cfg.enable5 = doc["enable5"] | cfg.enable5;
        
        // Kirim response DULU sebelum save
        server.send(200, "application/json", "{\"ok\":1}");
        yield();  // Pastikan response terkirim
        delay(50);
        
        saveConfig();
        return;  // Sudah kirim response
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  server.on("/api/display", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<256> doc;
      String payload = server.arg("plain");
      yield();  // Beri waktu untuk WiFi
      if (!deserializeJson(doc, payload)) {
        uint8_t oldMode = cfg.displayMode;
        cfg.brightness = doc["brightness"] | cfg.brightness;
        cfg.speed = doc["speed"] | cfg.speed;
        cfg.displayMode = doc["displayMode"] | cfg.displayMode;
        cfg.separator = doc["separator"] | cfg.separator;
        cfg.jamDuration = doc["jamDuration"] | cfg.jamDuration;
        if (doc.containsKey("format24h")) {
          cfg.format24h = doc["format24h"];
        }
        Disp.setBrightness(cfg.brightness);
        
        // Reset display state jika mode berubah
        if (oldMode != cfg.displayMode) {
           resetDisplayState();
        }
        
        // Kirim response DULU sebelum save
        server.send(200, "application/json", "{\"ok\":1}");
        yield();  // Pastikan response terkirim
        delay(50);
        
        saveConfig();
        
        Serial.print(F("[DISP] Mode="));
        Serial.println(cfg.displayMode);
        return;  // Sudah kirim response
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  server.on("/api/wifi", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<128> doc;
      if (!deserializeJson(doc, server.arg("plain"))) {
        if (doc["ssid"]) strlcpy(cfg.ssid, doc["ssid"], sizeof(cfg.ssid));
        if (doc["password"] && strlen(doc["password"]) >= 8) {
          strlcpy(cfg.password, doc["password"], sizeof(cfg.password));
        }
        saveConfig();
        server.send(200, "application/json", "{\"ok\":1}");
        delay(1000);
        ESP.restart();
        return;
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });

  server.on("/api/alarm", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, server.arg("plain"))) {
        cfg.alarmEnable = doc["alarmEnable"] | 0;
        cfg.alarmHour = doc["alarmHour"] | 0;
        cfg.alarmMinute = doc["alarmMinute"] | 0;
        cfg.alarmDuration = doc["alarmDuration"] | 60;
        cfg.alarmTune = doc["alarmTune"] | 0;
        
        server.send(200, "application/json", "{\"ok\":1}");
        yield();
        saveConfig();
        return;
      }
    }
    server.send(200, "application/json", "{\"ok\":1}");
  });
  
  server.begin();
}

// ═══════════════════════════════════════════════════════════════════
// DISPLAY MODES
// ═══════════════════════════════════════════════════════════════════

uint8_t displayState = 0;  // 0=jam, 1=running
uint8_t runningTextIdx = 0;
uint32_t scrollX = 0;
uint32_t lastScrollTime = 0;
uint32_t modeStartTime = 0;
uint8_t textShownCount = 0;  // Dipindah dari static
uint8_t lastIdx = 255;       // Dipindah dari static

// Reset semua state display saat mode berubah
void resetDisplayState() {
  displayState = 0;
  runningTextIdx = 0;
  scrollX = 0;
  lastScrollTime = 0;
  modeStartTime = millis();
  textShownCount = 0;
  lastIdx = 255;
  Disp.clear();
  Serial.println(F("[DISP] State reset"));
}

void showClock() {
  static uint8_t lastSec = 255;
  if (detik == lastSec) return;
  lastSec = detik;
  
  char s1[3], s2[3], s3[3], s4[3];
  
  // Handle 12/24 hour format
  int jamDisplay = jam;
  if (!cfg.format24h) {
    if (jamDisplay == 0) jamDisplay = 12;
    else if (jamDisplay > 12) jamDisplay -= 12;
  }
  
  sprintf(s1, "%02d", jamDisplay);
  sprintf(s2, "%02d", menit);
  sprintf(s3, "%02d", detik);
  sprintf(s4, "%02d", tgl);
  
  Disp.clear();
  
  // Panel 1: JAM (geser 2px ke kanan, tanpa separator)
  Disp.setFont(ElektronMart6x16);
  Disp.drawText(3, 0, s1);  // Geser 2px dari 1 ke 3
  
  // Menit/Detik (posisi tetap)
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(18, 0, s2);
  Disp.drawText(18, 8, s3);
  
  // Panel 2: Tanggal (tetap sama)
  Disp.setFont(ElektronMart6x16);
  Disp.drawText(35, 0, s4);
  
  // Hari/Bulan
  Disp.setFont(ElektronMart5x6);
  Disp.drawText(50, 0, getHari());
  Disp.drawText(50, 8, getBulan());
}

int getNextActiveText(int current) {
  int next = current + 1;
  for (int i = 0; i < 6; i++) {
    if (next > 5) next = 0;
    switch(next) {
      case 0: return 0;  // Nama selalu aktif
      case 1: if (cfg.enable1) return 1; break;
      case 2: if (cfg.enable2) return 2; break;
      case 3: if (cfg.enable3) return 3; break;
      case 4: if (cfg.enable4) return 4; break;
      case 5: if (cfg.enable5) return 5; break;
    }
    next++;
  }
  return 0;
}

void showRunningText() {
  uint32_t currentTime = millis();
  int width = Disp.width();
  
  char* currentText;
  static char namaWithPrefix[128];
  
  switch(runningTextIdx) {
    case 0: 
      snprintf(namaWithPrefix, sizeof(namaWithPrefix), "* %s *", cfg.nama);
      currentText = namaWithPrefix;
      break;
    case 1: currentText = cfg.info1; break;
    case 2: currentText = cfg.info2; break;
    case 3: currentText = cfg.info3; break;
    case 4: currentText = cfg.info4; break;
    case 5: currentText = cfg.info5; break;
    default: currentText = cfg.nama; break;
  }
  
  Disp.setFont(ElektronMart6x8);  // Kembali ke font standar
  
  int textWidth = Disp.textWidth(currentText);
  int fullScroll = textWidth + width;
  
  if (currentTime - lastScrollTime >= cfg.speed) {
    lastScrollTime = currentTime;
    
    if (scrollX < fullScroll) {
      scrollX++;
    } else {
      scrollX = 0;
      Disp.clear();
      runningTextIdx = getNextActiveText(runningTextIdx);
      return;
    }
    
    Disp.drawText(width - scrollX, 4, currentText);  // Y=4 untuk font 8px
  }
}

void runDisplay() {
  uint32_t currentTime = millis();
  
  // Mode handling
  if (cfg.displayMode == 0) {
    // Jam only
    showClock();
  } else if (cfg.displayMode == 1) {
    // Running text only
    showRunningText();
  } else {
    // Auto cycle (mode 2)
    if (displayState == 0) {
      // Showing clock
      showClock();
      if (currentTime - modeStartTime >= cfg.jamDuration * 1000) {
        displayState = 1;
        modeStartTime = currentTime;
        scrollX = 0;
        runningTextIdx = 0;
        Disp.clear();
      }
    } else {
      // Showing running text
      showRunningText();
      // Check if all texts shown (runningTextIdx wrapped to 0)
      // textShownCount dan lastIdx sekarang global, bukan static
      
      if (lastIdx != runningTextIdx) {
        lastIdx = runningTextIdx;
        if (runningTextIdx == 0 && textShownCount > 0) {
          // Wrapped back to start - switch to clock
          displayState = 0;
          modeStartTime = currentTime;
          textShownCount = 0;
          lastIdx = 255;
          Disp.clear();
        } else {
          textShownCount++;
        }
      }
    }
  }
}

// ═══════════════════════════════════════════════════════════════════
// WIFI
// ═══════════════════════════════════════════════════════════════════

unsigned long lastWiFiCheck = 0;
unsigned long wifiDownSince = 0;
bool wifiWasDown = false;

void setupWiFi() {
  WiFi.persistent(false);  // PENTING: false agar tidak conflict dengan SPIFFS
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  yield();
  WiFi.disconnect(true);
  yield();
  WiFi.softAPdisconnect(true);
  yield();
  delay(100);
  
  WiFi.mode(WIFI_OFF);
  yield();
  delay(100);
  WiFi.mode(WIFI_AP);
  yield();
  delay(100);
  
  WiFi.softAPConfig(localIP, gateway, subnet);
  yield();
  delay(100);
  
  bool apStarted = WiFi.softAP(cfg.ssid, cfg.password, 1, false, 4);
  yield();
  
  if (apStarted) {
    Serial.print(F("[WiFi] "));
    Serial.println(cfg.ssid);
  } else {
    Serial.println(F("[WiFi] FAIL!"));
  }
  
  WiFi.setOutputPower(20.5);
}

void checkWiFi() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastWiFiCheck < 10000) return;  // Check setiap 10 detik saja
  lastWiFiCheck = currentMillis;
  
  yield();
  
  WiFiMode_t mode = WiFi.getMode();
  IPAddress ip = WiFi.softAPIP();
  
  bool isDown = (mode != WIFI_AP && mode != WIFI_AP_STA) || 
                (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0);
  
  if (isDown) {
    if (!wifiWasDown) {
      wifiDownSince = currentMillis;
      wifiWasDown = true;
      Serial.println(F("[WiFi] Down detected"));
    } else if (currentMillis - wifiDownSince > 5000) {  // Tunggu 5 detik sebelum restart
      // Restart WiFi dengan yield di setiap langkah
      Serial.println(F("[WiFi] Restarting..."));
      yield();
      WiFi.softAPdisconnect(true);
      yield();
      WiFi.disconnect(true);
      yield();
      delay(200);
      yield();
      WiFi.mode(WIFI_OFF);
      yield();
      delay(200);
      yield();
      WiFi.mode(WIFI_AP);
      yield();
      delay(100);
      yield();
      WiFi.softAPConfig(localIP, gateway, subnet);
      yield();
      delay(100);
      yield();
      WiFi.softAP(cfg.ssid, cfg.password, 1, false, 4);
      yield();
      wifiWasDown = false;
      Serial.println(F("[WiFi] Restarted"));
    }
  } else {
    if (wifiWasDown) {
      wifiWasDown = false;
      Serial.println(F("[WiFi] Recovered"));
    }
  }
}

// ═══════════════════════════════════════════════════════════════════
// BUZZER & ALARM
// ═══════════════════════════════════════════════════════════════════

#define BUZZER_PIN 3  // RX Pin

bool alarmActive = false;
uint32_t alarmStart = 0;
uint32_t lastTone = 0;
int toneState = 0;

void disableSerialForBuzzer() {
  // Matikan fungsi Serial pin agar bisa dipakai Buzzer
  // Serial.end(); // Tidak perlu flush/end kalau mau override pinMode saja
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void checkAlarm() {
  if (!cfg.alarmEnable) return;
  if (alarmActive) return; // Sedang bunyi
  
  // Cek jam (hanya trigger di detik 0)
  if (jam == cfg.alarmHour && menit == cfg.alarmMinute && detik == 0) {
    if (millis() - alarmStart > 60000) { // Debounce simple, minimal 1 min baru boleh trigger lagi
      alarmActive = true;
      alarmStart = millis();
      toneState = 0;
      Serial.println(F("[ALARM] Triggered!"));
    }
  }
}

// Non-blocking Tone Generator
void buzzerWorker() {
  if (!alarmActive) {
    digitalWrite(BUZZER_PIN, LOW); // Pastikan mati
    return;
  }
  
  // Auto stop setelah durasi yg ditentukan
  if (millis() - alarmStart > (cfg.alarmDuration * 1000)) {
    alarmActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }
  
  uint32_t now = millis();
  uint32_t elapsed = now - lastTone;
  
  // Tune Logic
  switch(cfg.alarmTune) {
    case 0: // Tunggal (Beep .... Beep)
      if (toneState == 0) { tone(BUZZER_PIN, 2000); if (elapsed > 500) { noTone(BUZZER_PIN); lastTone = now; toneState = 1; } }
      else { if (elapsed > 500) { lastTone = now; toneState = 0; } }
      break;
      
    case 1: // Ganda (Beep Beep ....)
      if (toneState == 0) { tone(BUZZER_PIN, 2500); if (elapsed > 100) { noTone(BUZZER_PIN); lastTone = now; toneState = 1; } }
      else if (toneState == 1) { if (elapsed > 100) { lastTone = now; toneState = 2; } } // Silent gap
      else if (toneState == 2) { tone(BUZZER_PIN, 2500); if (elapsed > 100) { noTone(BUZZER_PIN); lastTone = now; toneState = 3; } }
      else { if (elapsed > 1000) { lastTone = now; toneState = 0; } } // Long pause
      break;
      
    case 2: // Panjang (BEEEEEEP)
      if (toneState == 0) { tone(BUZZER_PIN, 1500); if (elapsed > 2000) { noTone(BUZZER_PIN); lastTone = now; toneState = 1; } }
      else { if (elapsed > 1000) { lastTone = now; toneState = 0; } }
      break;

    case 3: // Sirine (Ni Nu Ni Nu)
      if (toneState == 0) { tone(BUZZER_PIN, 2000); if (elapsed > 300) { lastTone = now; toneState = 1; } }
      else if (toneState == 1) { tone(BUZZER_PIN, 1500); if (elapsed > 300) { lastTone = now; toneState = 0; } }
      break;

    case 4: // Nokia (Tu Tuli Tu... Tu Tuli Tu...)
      // Simplified: High High Low High ... 
      if (toneState == 0) { tone(BUZZER_PIN, 2000); if (elapsed > 150) { lastTone = now; toneState = 1; } }
      else if (toneState == 1) { noTone(BUZZER_PIN); if (elapsed > 50) { lastTone = now; toneState = 2; } }
      else if (toneState == 2) { tone(BUZZER_PIN, 2000); if (elapsed > 150) { lastTone = now; toneState = 3; } }
      else if (toneState == 3) { noTone(BUZZER_PIN); if (elapsed > 200) { lastTone = now; toneState = 4; } }
      else if (toneState == 4) { tone(BUZZER_PIN, 1000); if (elapsed > 150) { lastTone = now; toneState = 5; } }
      else if (toneState == 5) { noTone(BUZZER_PIN); if (elapsed > 50) { lastTone = now; toneState = 6; } }
      else if (toneState == 6) { tone(BUZZER_PIN, 1500); if (elapsed > 150) { noTone(BUZZER_PIN); lastTone = now; toneState = 7; } }
      else { if (elapsed > 1500) { lastTone = now; toneState = 0; } }
      break;
  }
}

// ═══════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════

void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== SEWU COMBO v1.0 ==="));
  
  // SPIFFS dengan auto-format jika gagal mount
  if (!SPIFFS.begin()) {
    Serial.println(F("[FS] SPIFFS Fail, formatting..."));
    SPIFFS.format();
    SPIFFS.begin();
  }
  
  loadConfig();
  
  // RTC
  Wire.begin();
  Rtc.Begin();
  if (!Rtc.GetIsRunning()) Rtc.SetIsRunning(true);
  Serial.println(F("[RTC] OK"));
  
  // WiFi
  setupWiFi();
  
  // Web
  setupWebServer();
  Serial.println(F("[WEB] OK"));
  
  // Display
  Disp.start();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  Disp.setBrightness(cfg.brightness);
  Disp.setBrightness(cfg.brightness);
  Serial.println(F("[DISP] OK"));

  // BUZZER INIT - Ambil alih pin RX
  // Berikan delay sedikit agar serial flush
  Serial.println(F("[SYS] Disabling Serial for Buzzer..."));
  Serial.flush();
  delay(100);
  disableSerialForBuzzer();
  
  // Test Buzzer Pendek Power On
  tone(BUZZER_PIN, 2000, 100); 
  
  // Branding - dua baris dengan font medium (6x8)
  Disp.clear();
  Disp.setFont(ElektronMart6x8);  // Font medium, tidak kurus
  // "SEWU" = 4 huruf x 6px = 24px, centered di panel 64px = (64-24)/2 = 20
  Disp.drawText(20, 0, "SEWU");
  // "AUDIO" = 5 huruf x 6px = 30px, centered = (64-30)/2 = 17
  Disp.drawText(17, 8, "AUDIO");
  delay(2000);
  Disp.clear();
  
  modeStartTime = millis();
  
  Serial.println(F("=== READY ===\n"));
}

// ═══════════════════════════════════════════════════════════════════
// LOOP
// ═══════════════════════════════════════════════════════════════════

void loop() {
  yield();
  server.handleClient();
  yield();
  checkWiFi();
  yield();
  
  readRTC();
  checkAlarm();
  buzzerWorker();
  runDisplay();
  
  delay(10);
}
