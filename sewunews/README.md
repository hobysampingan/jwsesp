# SEWU NEWS v2.0

**Professional P10 LED News Ticker with Real-Time Clock**

A modern, feature-rich news display system for P10 LED panels powered by ESP8266, featuring live news feeds, RTC integration, and a sleek web configuration interface.

---

## 🌟 Features

### Display Features
- **Dual-Row Layout**
  - **Top Row**: Cycling clock display (Time → Day → Date, 3s intervals)
  - **Bottom Row**: Scrolling news with merged source and headline
- **Real-Time Clock**: DS3231 RTC module for accurate timekeeping
- **Smooth Scrolling**: Adjustable speed (20ms - 100ms)
- **Auto-Brightness**: 50% brightness (configurable)

### News Features
- **Multiple News Sources**:
  - Top Headlines (Indonesia)
  - 25 Categories (Politics, Business, Tech, Sports, etc.)
  - 24+ Media Sources (Detik, Kompas, Tempo, CNN, etc.)
  - Custom keyword search
- **Live Updates**: Auto-refresh every 5 minutes
- **Clickable Links**: Web preview with direct article links
- **Smart Parsing**: Extracts source and title from Google News RSS

### Network Features
- **WiFi Connection**: Connect to your home/office network
- **AP Fallback Mode**: Automatic AP if WiFi fails
  - SSID: `SEWU-NEWS-SETUP`
  - Password: `sewunews123`
  - IP: `192.168.4.1`
- **Web Configuration**: Modern GitHub-style dark UI
- **IP Display**: Shows IP on P10 for 10 seconds at boot

### Configuration Features
- **Persistent Storage**: All settings saved to SPIFFS
- **Web-Based Setup**: No code editing required
- **RTC Configuration**: Set date/time via web interface
- **Speed Control**: Real-time scroll speed adjustment

---

## 🛠️ Hardware Requirements

### Essential Components
- **ESP8266** (Generic ESP8266 Module, 1MB Flash)
- **P10 LED Panel** (2 panels, 64x16 total resolution)
- **DS3231 RTC Module** (I2C)
- **Power Supply** (5V, 3A minimum)

### Optional Components
- **74HC245D** (Bus transceiver for better signal quality)
- Recommended for cables >30cm or commercial products

### Pin Connections

#### P10 Panel Connections
```
ESP8266     →   P10 Panel
GPIO13      →   S (SCLK)
GPIO14      →   nOE
GPIO12      →   LAT
GPIO15      →   A
GPIO4       →   B
GPIO5       →   D (DATA/R)
GND         →   GND
```

#### DS3231 RTC Connections
```
ESP8266     →   DS3231
GPIO4 (SDA) →   SDA
GPIO5 (SCL) →   SCL
3.3V        →   VCC
GND         →   GND
```

---

## 📋 Software Requirements

### Arduino IDE Setup
1. **ESP8266 Board Package**: v3.1.2 or higher
2. **Libraries**:
   - `ESP8266WiFi` (built-in)
   - `ESP8266WebServer` (built-in)
   - `ArduinoJson` (v6.x)
   - `RtcDS3231` (by Makuna)
   - `HJS589` (included in project)

### Board Settings
```
Board: Generic ESP8266 Module
Flash Size: 1MB (FS:64KB OTA:~470KB)
CPU Frequency: 80 MHz
Upload Speed: 115200
```

---

## 🚀 Installation

### 1. Hardware Assembly
1. Connect P10 panels according to pin diagram
2. Connect DS3231 RTC module
3. Connect power supply (5V to panels, 3.3V to ESP8266)

### 2. Software Upload
1. Open `sewunews.ino` in Arduino IDE
2. Install required libraries via Library Manager
3. Select correct board settings
4. Upload sketch to ESP8266

### 3. First Boot
1. ESP8266 will create AP: `SEWU-NEWS-SETUP`
2. P10 displays:
   ```
   AP MODE
   192.168.4.1
   ```
3. Connect to AP (password: `sewunews123`)
4. Open browser: `http://192.168.4.1`

---

## ⚙️ Configuration

### WiFi Setup
1. Open web interface
2. Navigate to "WiFi Setup" section
3. Enter your WiFi SSID and Password
4. Click "Save WiFi & Reboot"
5. ESP will restart and connect to your network

### RTC Setup
1. In web interface, go to "Set Date & Time"
2. Date/Time inputs auto-fill with current browser time
3. Adjust if needed
4. Click "Set RTC"
5. Clock will now display accurate time

### News Source Selection
Choose ONE of the following:

**Option 1: Media Source**
- Select from 24+ Indonesian media outlets
- Example: Detik.com, Kompas.com, Tempo.co

**Option 2: Category**
- Select from 25 categories
- Example: Politics, Technology, Sports

**Option 3: Custom Search**
- Enter any keyword
- Example: "jokowi", "gempa", "pilkada"

Click "Save News Source" to apply changes.

### Scroll Speed
- Use slider to adjust (20ms = Fast, 100ms = Slow)
- Changes apply immediately
- Auto-saved

---

## 🌐 Web Interface

### Accessing the Interface

**Normal Mode (WiFi Connected)**
1. Check P10 display at boot for IP address
2. Open browser: `http://[IP_ADDRESS]`

**AP Mode (WiFi Failed)**
1. Connect to SSID: `SEWU-NEWS-SETUP`
2. Open browser: `http://192.168.4.1`

### Interface Sections

#### WiFi Setup
- Configure SSID and password
- Auto-reboot after save

#### Scroll Speed
- Real-time adjustment (20-100ms)
- Visual feedback with speed indicator

#### Set Date & Time
- Browser-time auto-fill
- One-click RTC update

#### News Source
- Three dropdown/input options
- Auto-clear between selections
- Immediate update after save

#### Live News Preview
- Shows current 10 news items
- Clickable links to full articles
- 🔗 icon indicates available link
- Auto-refresh every 30 seconds

---

## 🔌 API Endpoints

### GET /api/data
Returns current configuration and IP address.

**Response:**
```json
{
  "newsSource": "headline",
  "wifiSSID": "My-WiFi",
  "wifiPassword": "password",
  "scrollSpeed": 40,
  "ip": "192.168.0.166"
}
```

### GET /api/news
Returns current news items displayed on P10.

**Response:**
```json
{
  "count": 10,
  "items": [
    {
      "source": "DETIK.COM",
      "title": "Breaking news headline...",
      "link": "https://news.google.com/..."
    }
  ]
}
```

### POST /api/config
Update news source configuration.

**Request:**
```json
{
  "newsSource": "media:detik.com"
}
```

### POST /api/wifi
Update WiFi credentials (triggers auto-reboot).

**Request:**
```json
{
  "wifiSSID": "New-WiFi",
  "wifiPassword": "newpassword"
}
```

### POST /api/speed
Update scroll speed.

**Request:**
```
POST /api/speed?speed=50
```

### POST /api/setRTC
Set RTC date and time.

**Request:**
```json
{
  "year": 2025,
  "month": 12,
  "day": 25,
  "hour": 23,
  "minute": 0
}
```

---

## 🎨 Display Format

### Boot Screen (10 seconds)
```
┌──────────────────┐
│   WIFI: OK       │  ← WiFi status
│  192.168.0.166   │  ← IP Address
└──────────────────┘
```

### Normal Operation

**Top Row (cycles every 3 seconds):**
- `23:02:46` → Time
- `KAMIS` → Day
- `25 DEC 25` → Date

**Bottom Row (continuous scroll):**
- `DETIK.COM - JOKOWI RESMIKAN JALAN TOL BARU...`
- Source and title merged with hyphen

---

## 🐛 Troubleshooting

### Display Issues

**Problem: Blank display**
- Check power supply (5V, sufficient amperage)
- Verify P10 panel connections
- Check ESP8266 is powered (3.3V)

**Problem: Flickering or unstable**
- Add 74HC245D level shifter
- Reduce ESP8266 CPU frequency to 80MHz
- Ensure proper grounding

### Network Issues

**Problem: WiFi won't connect**
- Wait 20 seconds for timeout
- ESP will auto-switch to AP mode
- Connect to `SEWU-NEWS-SETUP` and reconfigure

**Problem: Can't access web interface**
- Check P10 display for IP address at boot
- Ensure device and phone/computer on same network
- Try AP mode if needed

### News Issues

**Problem: "CONNECTING..." stuck**
- Check internet connection
- Verify DNS is working
- Wait up to 30 seconds for first fetch

**Problem: News not updating**
- Check WiFi connection
- Verify Google News is accessible
- News auto-updates every 5 minutes

### Clock Issues

**Problem: Time is wrong**
- Use web interface to set RTC
- Ensure DS3231 battery is installed
- Check I2C connections (SDA/SCL)

---

## 📝 Configuration File

Settings are stored in `/cfg.json` on SPIFFS:

```json
{
  "newsSource": "headline",
  "wifiSSID": "My-WiFi",
  "wifiPassword": "password",
  "scrollSpeed": 40
}
```

To reset to defaults, reflash ESP8266 with "Erase Flash: All".

---

## 🔧 Technical Details

### Memory Usage
- Flash: ~400KB (sketch)
- SPIFFS: 64KB (config storage)
- RAM: ~30KB free during operation

### News Update Flow
1. Every 5 minutes, fetch from `api.rss2json.com`
2. Parse Google News RSS feed
3. Extract 10 news items
4. Parse source from title suffix
5. Update display buffer
6. Start scrolling

### Display Refresh
- Interrupt-driven at 25 FPS
- Uses `timer0_attachInterrupt()`
- Never detached during operation (anti-crash)

### Network Stack
- BearSSL for HTTPS
- WiFiClientSecure (insecure mode for speed)
- Strategic `yield()` to prevent WDT resets

---

## 🎯 Project Structure

```
sewunews/
├── sewunews.ino          # Main sketch
├── sewunews_page.h       # Web UI (GitHub dark theme)
├── HJS589/               # Display library
│   ├── HJS589.h
│   ├── HJS589.cpp
│   ├── Bitmap.h
│   ├── Bitmap.cpp
│   └── fonts/
│       ├── ElektronMart5x6.h
│       └── ElektronMart6x8.h
└── README.md             # This file
```

---

## 📜 License

This project is open-source and free to use for personal or commercial purposes.

---

## 🙏 Credits

**Developed by:** SEWU Audio Team  
**Hardware:** ESP8266 + P10 LED Matrix + DS3231 RTC  
**Libraries:** HJS589 (P10 driver), RtcDS3231, ArduinoJson  
**News API:** Google News RSS via rss2json.com  

---

## 📞 Support

For issues, questions, or improvements:
1. Check Troubleshooting section
2. Review Serial Monitor output (115200 baud)
3. Verify hardware connections
4. Test in AP mode to isolate network issues

---

**Built with ❤️ for real-time news display**  
**Version 2.0 - December 2025**
