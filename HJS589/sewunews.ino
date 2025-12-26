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

// --- LIBRARY HJS589 ---
#include "HJS589/HJS589.h"
#include "HJS589/fonts/ElektronMart6x8.h"
#include "HJS589/fonts/ElektronMart5x6.h"

// Web page
#include "sewunews_page.h"

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
int scrollX = 64; 
int textWidth = 100;

// Timing
unsigned long lastUpdate = 0;
unsigned long lastScroll = 0;
bool firstUpdateDone = false;

// Scroll State - SEPARATE untuk source & title
int scrollXSource = 64;  // Source loop terus
int scrollXTitle = 64;   // Title 1x aja
int textWidthSource = 0;
int textWidthTitle = 0;

// --- PROTOTYPES ---
void updateNews();
void nextNews();
String cleanText(String t);
void loadConfig();
void saveConfig();
void setupWebServer();

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== SEWU NEWS START ===");

  // SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("[FS] Format...");
    SPIFFS.format();
    SPIFFS.begin();
  }
  
  loadConfig();

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" OK!");
  deviceIP = WiFi.localIP().toString();
  Serial.println("\n========================================");
  Serial.println("   WEB CONFIG: http://" + deviceIP);
  Serial.println("========================================\n");
  
  // Web Server
  setupWebServer();
  Serial.println("[WEB] OK");

  // DISPLAY
  Wire.begin();
  Disp.start();
  
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(displayRefresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  
  Disp.setBrightness(50);
  Disp.clear();
  
  Serial.println("[DISP] OK");
  
  // Init Scroll Pos
  scrollXSource = 64;
  scrollXTitle = 64;
  
  Serial.println("[SETUP] Complete");
  Serial.println("[INFO] News in 10s");
}

void loop() {
  server.handleClient();
  
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

  // Scroll animation
  if (millis() - lastScroll > 40) {
    lastScroll = millis();
    
    Disp.clear();
    
    // BARIS ATAS (SOURCE) - LOOP TERUS
    Disp.setFont(ElektronMart6x8);
    Disp.drawText(scrollXSource, 0, currentSource.c_str());
    
    // BARIS BAWAH (TITLE) - SCROLL 1X
    Disp.setFont(ElektronMart6x8);
    Disp.drawText(scrollXTitle, 8, currentTitle.c_str());
    
    // Source animation - LOOP TERUS
    scrollXSource--;
    if (scrollXSource < -textWidthSource) {
      scrollXSource = 64; // Reset source, tapi title tetep jalan
    }
    
    // Title animation - 1X AJA
    scrollXTitle--;
    if (scrollXTitle < -textWidthTitle) {
      if (jumlahBerita > 0) nextNews();
      scrollXSource = 64; // Reset source juga
      scrollXTitle = 64;  // Reset title
    }
  }
  
  yield();
}

void nextNews() {
  if (jumlahBerita == 0) return;
  
  currentNewsIdx = (currentNewsIdx + 1) % jumlahBerita;
  if (currentNewsIdx >= jumlahBerita) currentNewsIdx = 0;
  
  currentTitle = berita[currentNewsIdx].title;
  currentSource = berita[currentNewsIdx].source;
  
  if (currentTitle.length() == 0) currentTitle = "NO TITLE";
  if (currentSource.length() == 0) currentSource = "UNKNOWN";
  
  currentTitle.toUpperCase();
  currentSource.toUpperCase();
  
  // Hitung width masing-masing
  Disp.setFont(ElektronMart6x8);
  textWidthSource = Disp.textWidth(currentSource.c_str());
  textWidthTitle = Disp.textWidth(currentTitle.c_str());
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

// === CONFIG ===
void loadConfig() {
  strcpy(cfg.newsSource, "headline"); // Default
  
  if (!SPIFFS.exists("/cfg.json")) return;
  
  File f = SPIFFS.open("/cfg.json", "r");
  if (!f) return;
  
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  
  if (err) return;
  
  strlcpy(cfg.newsSource, doc["newsSource"] | "headline", sizeof(cfg.newsSource));
}

void saveConfig() {
  Serial.println("[CFG] Saving config...");
  Serial.print("[CFG] newsSource: ");
  Serial.println(cfg.newsSource);
  
  // CRITICAL: Matikan interrupt saat SPIFFS write
  timer0_detachInterrupt();
  delay(50);
  
  StaticJsonDocument<256> doc;
  doc["newsSource"] = cfg.newsSource;
  
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
      StaticJsonDocument<256> doc;
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
  
  server.begin();
}
