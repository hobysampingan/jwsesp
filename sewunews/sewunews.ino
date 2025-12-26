/*
 * SEWU NEWS - COMPLETE WITH WEB CONFIG
 * Features: News display + Web interface for category/media selection
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <FS.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <RtcDS3231.h> // RTC Library

// --- LIBRARY HJS589 ---
#include "HJS589/HJS589.h"
#include "HJS589/fonts/ElektronMart6x8.h"
#include "HJS589/fonts/ElektronMart5x6.h"

// Web page
#include "sewunews_page.h"

// --- RTC ---
RtcDS3231<TwoWire> Rtc(Wire);
uint8_t jam = 0, menit = 0, detik = 0, tgl = 1, bln = 1, dow = 0;
uint16_t tahun = 2024;
const char* HARI[] = {"MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUMAT", "SABTU"};
const char* BULAN[] = {"", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES"};

// --- KONFIGURASI ---
const char* ssid = "DM 707";
const char* password = "Desmare707=";
#define PANEL_COUNT 2 
HJS589 Disp(PANEL_COUNT, 1);

// Web Server
ESP8266WebServer server(80);
String deviceIP = "";

// --- NEWS CONFIG ---
struct Config {
  char newsSource[64]; // Format: "headline", "category:TECHNOLOGY", "media:detik.com"
  char wifiSSID[32];
  char wifiPassword[64];
  int scrollSpeed; // milliseconds delay (20-100)
};
Config cfg;

// --- INTERRUPT LOGIC ---
void ICACHE_RAM_ATTR displayRefresh() {
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);
}

// --- NEWS VARS ---
struct NewsItem { 
  String title; 
  String source; 
  String link;  // URL berita
};
NewsItem berita[10];
int jumlahBerita = 0;
int currentNewsIdx = 0;
String currentTitle = "SEWU NEWS - CONNECTING...";
String currentSource = "LOADING";

// Timing
unsigned long lastUpdate = 0;
unsigned long lastScroll = 0;
unsigned long lastClockCycle = 0;
bool firstUpdateDone = false;

// Clock Display State (cycle: 0=Time, 1=Day, 2=Date, 3=IP)
int clockMode = 0;
int ipDisplayCount = 0; // Count how many times IP shown (max 5)

// Scroll State - Single line for "SOURCE - TITLE"
int scrollX = 64;
int textWidth = 0;

// IP scroll state
int ipScrollX = 64;
int ipScrollWidth = 0;

// --- PROTOTYPES ---
void updateNews();
void nextNews();
String cleanText(String t);
void loadConfig();
void saveConfig();
void setupWebServer();
void readRTC();
char* getHari();
char* getBulan();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== SEWU NEWS v2.0 ===");

  // SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("[SPIFFS] FAILED");
  } else {
    Serial.println("[SPIFFS] OK");
  }
  yield();
  
  loadConfig();
  yield();

  // WIFI (from config) with AP FALLBACK
  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg.wifiSSID, cfg.wifiPassword);
  Serial.print("WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
    yield();
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK!");
    deviceIP = WiFi.localIP().toString();
    Serial.println("\n========================================");
    Serial.println("   WEB CONFIG: http://" + deviceIP);
    Serial.println("========================================\n");
  } else {
    // FALLBACK: Jadi Access Point
    Serial.println(" FAILED!");
    Serial.println("[WiFi] Starting AP Mode...");
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP("SEWU-NEWS-SETUP", "sewunews123");
    deviceIP = WiFi.softAPIP().toString();
    
    Serial.println("\n========================================");
    Serial.println("   AP MODE ACTIVE");
    Serial.println("   SSID: SEWU-NEWS-SETUP");
    Serial.println("   Password: sewunews123");
    Serial.println("   IP: " + deviceIP);
    Serial.println("   Connect & goto: http://" + deviceIP);
    Serial.println("========================================\n");
  }
  yield();
  
  // Web Server
  setupWebServer();
  Serial.println("[WEB] OK");
  yield();

  // DISPLAY & RTC
  Wire.begin();
  yield();
  Rtc.Begin();
  yield();
  Disp.start();
  yield();
  
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  
  Disp.setBrightness(50);
  Disp.clear();
  yield();
  
  // Display IP on P10 for 10 seconds
  Disp.setFont(ElektronMart6x8);
  
  String topLine = WiFi.status() == WL_CONNECTED ? "WIFI: OK" : "AP MODE";
  int wTop = Disp.textWidth(topLine.c_str());
  int xTop = (64 - wTop) / 2;
  if(xTop < 0) xTop = 0;
  Disp.drawText(xTop, 0, topLine.c_str());
  
  // Show IP centered on bottom row
  int wIP = Disp.textWidth(deviceIP.c_str());
  int xIP = (64 - wIP) / 2;
  if(xIP < 0) xIP = 0;
  Disp.drawText(xIP, 8, deviceIP.c_str());
  
  Serial.println("[DISP] OK");
  Serial.println("[DISP] Showing IP for 10 seconds...");
  
  // Init Scroll Pos
  scrollX = 64;
  
  Serial.println("[SETUP] Complete");
  Serial.println("[INFO] News in 10s");
}

void loop() {
  server.handleClient();
  
  // Update RTC every second
  static unsigned long lastRTCRead = 0;
  if(millis() - lastRTCRead > 1000) {
    lastRTCRead = millis();
    readRTC();
  }
  
  // Cycle clock display every 3 seconds (but NOT when in IP mode - IP controls its own cycle)
  if(millis() - lastClockCycle > 3000 && clockMode != 3) {
    lastClockCycle = millis();
    
    // If IP already shown 5 times, skip IP mode (3)
    if(ipDisplayCount >= 5) {
      clockMode = (clockMode + 1) % 3; // Only cycle 0,1,2 (Time, Day, Date)
    } else {
      clockMode = (clockMode + 1) % 4; // All 4 modes including IP
      
      // Reset IP scroll when entering IP mode
      if(clockMode == 3) {
        ipScrollX = 64;
        Disp.setFont(ElektronMart6x8);
        ipScrollWidth = Disp.textWidth(deviceIP.c_str());
      }
    }
  }
  
  // First update
  if (!firstUpdateDone && millis() > 10000) {
    updateNews();
    firstUpdateDone = true;
    if (jumlahBerita > 0) nextNews();
  }
  
  // Periodic update (5 min)
  if (firstUpdateDone && millis() - lastUpdate > 300000) {
    lastUpdate = millis();
    updateNews();
    if (jumlahBerita > 0) {
      currentNewsIdx = 0;
      nextNews();
    }
  }

  // Display refresh
  if (millis() - lastScroll > cfg.scrollSpeed) {
    lastScroll = millis();
    
    Disp.clear();
    
    // === BARIS ATAS: Clock Cycle ===
    Disp.setFont(ElektronMart6x8);
    
    if(clockMode == 0) {
      // TIME: 12:34:56
      char buf[10];
      sprintf(buf, "%02d:%02d:%02d", jam, menit, detik);
      String topText = String(buf);
      
      int wTop = Disp.textWidth(topText.c_str());
      int xTop = (64 - wTop) / 2;
      if(xTop < 0) xTop = 0;
      xTop += 1; // Shift 1px RIGHT for better centering
      Disp.drawText(xTop, 0, topText.c_str());
      
    } else if(clockMode == 1) {
      // DAY: SABTU
      String topText = String(getHari());
      
      int wTop = Disp.textWidth(topText.c_str());
      int xTop = (64 - wTop) / 2;
      if(xTop < 0) xTop = 0;
      xTop += 1; // Shift 1px RIGHT for better centering
      Disp.drawText(xTop, 0, topText.c_str());
      
    } else if(clockMode == 2) {
      // DATE: 25 DEC 25
      char buf[20];
      sprintf(buf, "%02d %s %02d", tgl, getBulan(), tahun % 100);
      String topText = String(buf);
      
      int wTop = Disp.textWidth(topText.c_str());
      int xTop = (64 - wTop) / 2;
      if(xTop < 0) xTop = 0;
      xTop += 1; // Shift 1px RIGHT for better centering
      Disp.drawText(xTop, 0, topText.c_str());
      
    } else {
      // IP: SCROLL (too long for 64px)
      Disp.drawText(ipScrollX, 0, deviceIP.c_str());
      
      ipScrollX--;
      if(ipScrollX < -ipScrollWidth) {
        ipScrollX = 64; // Reset for next time
        ipDisplayCount++; // Increment counter
        
        // Advance to next mode manually
        clockMode = 0; // Go back to Time
        lastClockCycle = millis(); // Reset timer
      }
    }
    
    // === BARIS BAWAH: SOURCE - TITLE (merged scroll) ===
    Disp.setFont(ElektronMart6x8);
    Disp.drawText(scrollX, 8, currentTitle.c_str());
    
    scrollX--;
    if(scrollX < -textWidth) {
      if(jumlahBerita > 0) nextNews();
      scrollX = 64;
    }
  }
  
  yield();
}

void nextNews() {
  if (jumlahBerita == 0) return;
  
  currentNewsIdx = (currentNewsIdx + 1) % jumlahBerita;
  if (currentNewsIdx >= jumlahBerita) currentNewsIdx = 0;
  
  String title = berita[currentNewsIdx].title;
  currentSource = berita[currentNewsIdx].source;
  
  if (title.length() == 0) title = "NO TITLE";
  if (currentSource.length() == 0) currentSource = "UNKNOWN";
  
  title.toUpperCase();
  currentSource.toUpperCase();
  
  // MERGE: "SOURCE - TITLE"
  currentTitle = currentSource + " - " + title;
  
  Disp.setFont(ElektronMart6x8);
  textWidth = Disp.textWidth(currentTitle.c_str());
}

String cleanText(String text) {
  text.replace("\\\"", "'");
  text.replace("\\", "");
  text.replace(" - Google News", "");
  text.replace(" - Google Berita", "");
  // Dash removal handled in parsing logic now
  return text;
}

void updateNews() {
  Serial.println("\n[NEWS] ========== UPDATE START ==========");
  Serial.print("[NEWS] Current config: ");
  Serial.println(cfg.newsSource);
  
  // Build URL based on config
  String rssUrl = "https://news.google.com/rss?hl=id&gl=ID&ceid=ID:id";
  
  String src = String(cfg.newsSource);
  Serial.print("[NEWS] Source type: ");
  
  if (src.startsWith("category:")) {
    // Format: category:POLITICS, category:BUSINESS, etc.
    String cat = src.substring(9);
    rssUrl = "https://news.google.com/rss/headlines/section/topic/" + cat + "?hl=id&gl=ID&ceid=ID:id";
    Serial.println("CATEGORY - " + cat);
  } else if (src.startsWith("search:")) {
    // Format: search:otomotif, search:travel, etc.
    String query = src.substring(7);
    rssUrl = "https://news.google.com/rss/search?q=" + query + "&hl=id&gl=ID&ceid=ID:id";
    Serial.println("SEARCH - " + query);
  } else if (src.startsWith("media:")) {
    // Format: media:detik.com, media:kompas.com, etc.
    String med = src.substring(6);
    rssUrl = "https://news.google.com/rss/search?q=site:" + med + "&hl=id&gl=ID&ceid=ID:id";
    Serial.println("MEDIA - " + med);
  } else {
    Serial.println("HEADLINE");
  }
  
  Serial.println("[NEWS] RSS URL: " + rssUrl);
  
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);
  
  Serial.print("[NEWS] Connecting to api.rss2json.com... ");
  
  if(client.connect("api.rss2json.com", 443)) {
    Serial.println("OK");
    
    String url = "/v1/api.json?rss_url=" + rssUrl;
    Serial.println("[NEWS] Request: " + url);
    
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: api.rss2json.com\r\n" +
                  "Connection: close\r\n\r\n");
    
    yield();
    
    unsigned long timeout = millis();
    while(!client.available()) {
      if (millis() - timeout > 10000) {
        Serial.println("[NEWS] Timeout waiting response");
        client.stop();
        return;
      }
      yield();
      delay(10);
    }
    
    yield();
    
    // Skip header
    Serial.println("[NEWS] Reading response...");
    while(client.available()) {
      String line = client.readStringUntil('\n');
      if(line == "\r") break;
      yield();
    }
    
    // Parse - IMPROVED PARSING (Like JavaScript version)
    int cnt = 0;
    Serial.println("[NEWS] Parsing JSON...");
    
    while(client.available() && cnt < 10) {
      yield();
      
      if(client.find("\"title\":\"")) {
        String rawTitle = client.readStringUntil('"');
        rawTitle = cleanText(rawTitle);
        
        if(rawTitle.length() > 5 && rawTitle.indexOf("Google Berita") < 0 && rawTitle.indexOf("Artikel populer") < 0) {
          
          // EXTRACT SOURCE FROM TITLE (bagian setelah " - " terakhir)
          int lastDash = rawTitle.lastIndexOf(" - ");
          String title = rawTitle;
          String source = "GOOGLE";
          
          if (lastDash > 0) {
            source = rawTitle.substring(lastDash + 3); // After " - "
            source.trim();
            title = rawTitle.substring(0, lastDash); // Before " - "
          }
          
          berita[cnt].title = title;
          berita[cnt].source = source;
          berita[cnt].link = ""; // Default kosong
          
          // PARSE LINK - OPTIMIZED (Direct find, no buffer!)
          // Cari "link" field langsung tanpa buffer besar
          if(client.find("\"link\":\"")) {
            berita[cnt].link = client.readStringUntil('"');
            Serial.print("[NEWS]     Link: ");
            Serial.println(berita[cnt].link.substring(0, 60) + "...");
          } else {
            Serial.println("[NEWS]     Link: NONE (not found)");
          }
          
          Serial.print("[NEWS] #");
          Serial.print(cnt+1);
          Serial.print(" Source: ");
          Serial.print(source);
          Serial.print(" | Title: ");
          Serial.println(title.substring(0, 40) + "...");
          
          cnt++;
        }
      }
    }
    
    if(cnt > 0) {
      jumlahBerita = cnt;
      Serial.print("[NEWS] SUCCESS: Got ");
      Serial.print(cnt);
      Serial.println(" news items");
    } else {
      Serial.println("[NEWS] FAIL: No news parsed");
    }
    
    client.stop();
  } else {
    Serial.println("FAIL - Cannot connect");
  }
  
  Serial.println("[NEWS] ========== UPDATE END ==========\n");
  yield();
}

// === RTC FUNCTIONS ===
char* getHari() {
  return (char*)HARI[dow];
}

char* getBulan() {
  return (char*)BULAN[bln];
}

void readRTC() {
  if (!Rtc.IsDateTimeValid()) {
    return;
  }
  
  RtcDateTime now = Rtc.GetDateTime();
  jam = now.Hour();
  menit = now.Minute();
  detik = now.Second();
  tgl = now.Day();
  bln = now.Month();
  tahun = now.Year();
  dow = now.DayOfWeek();
}

// === CONFIG ===
void loadConfig() {
  strcpy(cfg.newsSource, "headline"); // Default
  strcpy(cfg.wifiSSID, "DM 707"); // Default
  strcpy(cfg.wifiPassword, "Desmare707="); // Default
  cfg.scrollSpeed = 40; // Default (ms)
  
  if (!SPIFFS.exists("/cfg.json")) return;
  
  File f = SPIFFS.open("/cfg.json", "r");
  if (!f) return;
  
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  
  if (err) return;
  
  strlcpy(cfg.newsSource, doc["newsSource"] | "headline", sizeof(cfg.newsSource));
  strlcpy(cfg.wifiSSID, doc["wifiSSID"] | "DM 707", sizeof(cfg.wifiSSID));
  strlcpy(cfg.wifiPassword, doc["wifiPassword"] | "Desmare707=", sizeof(cfg.wifiPassword));
  cfg.scrollSpeed = doc["scrollSpeed"] | 40;
}

void saveConfig() {
  Serial.println("[CFG] Saving config...");
  Serial.print("[CFG] newsSource: ");
  Serial.println(cfg.newsSource);
  
  // CRITICAL: Matikan interrupt saat SPIFFS write
  timer0_detachInterrupt();
  delay(50);
  
  StaticJsonDocument<512> doc;
  doc["newsSource"] = cfg.newsSource;
  doc["wifiSSID"] = cfg.wifiSSID;
  doc["wifiPassword"] = cfg.wifiPassword;
  doc["scrollSpeed"] = cfg.scrollSpeed;
  
  File f = SPIFFS.open("/cfg.json", "w");
  if (!f) {
    Serial.println("[CFG] FAIL - Cannot open file");
    
    // Restore interrupt
    noInterrupts();
    timer0_attachInterrupt(displayRefresh);
    timer0_write(ESP.getCycleCount() + 80000);
    interrupts();
    return;
  }
  
  serializeJson(doc, f);
  f.close();
  
  // Restore interrupt
  delay(50);
  noInterrupts();
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  
  Serial.println("[CFG] Saved successfully");
}

// === WEB SERVER ===
void setupWebServer() {
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", WEBPAGE);
  });
  
  server.on("/api/data", HTTP_GET, []() {
    String json = "{";
    json += "\"newsSource\":\"" + String(cfg.newsSource) + "\",";
    json += "\"wifiSSID\":\"" + String(cfg.wifiSSID) + "\",";
    json += "\"wifiPassword\":\"" + String(cfg.wifiPassword) + "\",";
    json += "\"scrollSpeed\":" + String(cfg.scrollSpeed) + ",";
    json += "\"ip\":\"" + deviceIP + "\"";
    json += "}";
    
    Serial.println("[API] GET /api/data");
    Serial.println("[API] Response: " + json);
    
    server.send(200, "application/json", json);
  });
  
  // NEW: Endpoint untuk preview berita
  server.on("/api/news", HTTP_GET, []() {
    String json = "{\"count\":" + String(jumlahBerita) + ",\"items\":[";
    
    for(int i = 0; i < jumlahBerita; i++) {
      if(i > 0) json += ",";
      json += "{";
      json += "\"source\":\"" + berita[i].source + "\",";
      json += "\"title\":\"" + berita[i].title + "\",";
      json += "\"link\":\"" + berita[i].link + "\"";
      json += "}";
    }
    
    json += "]}";
    
    server.send(200, "application/json", json);
  });
  
  server.on("/api/config", HTTP_POST, []() {
    Serial.println("[API] POST /api/config");
    Serial.print("[API] Body: ");
    Serial.println(server.arg("plain"));
    
    if (server.hasArg("plain")) {
      StaticJsonDocument<512> doc;
      if (!deserializeJson(doc, server.arg("plain"))) {
        const char* newSrc = doc["newsSource"];
        Serial.print("[API] New source: ");
        Serial.println(newSrc);
        
        strlcpy(cfg.newsSource, newSrc, sizeof(cfg.newsSource));
        saveConfig();
        
        // Trigger update
        Serial.println("[API] Triggering news update...");
        firstUpdateDone = false;
        lastUpdate = 0;
        
        server.send(200, "application/json", "{\"ok\":1}");
        return;
      }
    }
    Serial.println("[API] Invalid request");
    server.send(400, "application/json", "{\"ok\":0}");
  });
  
  // NEW: WiFi Config
  server.on("/api/wifi", HTTP_POST, []() {
    Serial.println("[API] POST /api/wifi");
    
    if (server.hasArg("plain")) {
      StaticJsonDocument<256> doc;
      if (!deserializeJson(doc, server.arg("plain"))) {
        strlcpy(cfg.wifiSSID, doc["wifiSSID"], sizeof(cfg.wifiSSID));
        strlcpy(cfg.wifiPassword, doc["wifiPassword"], sizeof(cfg.wifiPassword));
        saveConfig();
        
        Serial.print("[API] WiFi updated: ");
        Serial.println(cfg.wifiSSID);
        
        server.send(200, "application/json", "{\"ok\":1,\"msg\":\"WiFi saved. Rebooting...\"}");
        
        // Auto-reboot setelah 2 detik
        delay(2000);
        ESP.restart();
        return;
      }
    }
    server.send(400, "application/json", "{\"ok\":0}");
  });
  
  // NEW: Scroll Speed
  server.on("/api/speed", HTTP_POST, []() {
    if (server.hasArg("speed")) {
      int newSpeed = server.arg("speed").toInt();
      if(newSpeed >= 20 && newSpeed <= 100) {
        cfg.scrollSpeed = newSpeed;
        saveConfig();
        
        Serial.print("[API] Scroll speed updated: ");
        Serial.println(cfg.scrollSpeed);
        
        server.send(200, "application/json", "{\"ok\":1}");
        return;
      }
    }
    server.send(400, "application/json", "{\"ok\":0}");
  });
  
  // NEW: Set RTC Time
  server.on("/api/setRTC", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      StaticJsonDocument<128> doc;
      if (!deserializeJson(doc, server.arg("plain"))) {
        int year = doc["year"] | 2025;
        int month = doc["month"] | 1;
        int day = doc["day"] | 1;
        int hour = doc["hour"] | 0;
        int minute = doc["minute"] | 0;
        
        RtcDateTime newTime(year, month, day, hour, minute, 0);
        Rtc.SetDateTime(newTime);
        
        Serial.println("[RTC] Time updated:");
        Serial.printf("  %04d-%02d-%02d %02d:%02d\n", year, month, day, hour, minute);
        
        server.send(200, "application/json", "{\"ok\":1}");
        return;
      }
    }
    server.send(400, "application/json", "{\"ok\":0}");
  });
  
  server.begin();
}
