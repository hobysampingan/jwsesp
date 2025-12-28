# 🕐 SEWU CLOCK - Digital Alarm Clock V3.7 FINAL

A professional-grade P10 LED Matrix alarm clock powered by ESP8266, featuring dual alarms, classic ringtones, real-time clock synchronization, temperature monitoring, and a premium mobile-first web interface.

![Version](https://img.shields.io/badge/version-3.7-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP8266-green.svg)
![Status](https://img.shields.io/badge/status-production-brightgreen.svg)

---

## 🎯 Key Features

### ⏰ **Smart Alarm System**
- **2 Independent Alarms** - Perfect for daily routines
- **5 Classic Ringtones** - Nokia, Casio, Beep, Melody, Urgent
- **🔊 Tune Preview** - Test each ringtone before use
- **Buzzer Output** - GPIO3 (RX pin) with non-blocking patterns
- **Enable/Disable Toggle** - Quick on/off without deleting
- **Dirty Tracking** - Changes only save when alarm is enabled

### 🕐 **Display Options**
- **12/24 Hour Format** - Toggle between formats
- **LED Display** - Follows format (no AM/PM clutter on P10)
- **Web Clock** - Shows AM/PM in 12h mode for clarity
- **Live Updates** - Real-time synchronization

### 🌐 **Modern Web Interface**
- **Mobile-First Design** - Optimized for touch screens (480px max)
- **Dark Theme** - Premium glassmorphic UI
- **Styled Inputs** - Date/time pickers match theme
- **No Auto-Close** - Pickers stay open while selecting
- **Instant Feedback** - Visual state updates
- **Touch-Optimized** - Large tap targets (44px+)

### 📊 **Additional Features**
- 🌡️ **Temperature Display** - DS3231 built-in sensor
- 🔆 **Adjustable Brightness** - 18-255 levels
- 📅 **Date & Time Setting** - Easy web-based configuration
- 🔋 **Battery Backup** - RTC maintains time during power loss
- 💾 **No EEPROM** - Flash-friendly, settings via runtime
- 📱 **WiFi Access Point** - "SEWU CLOCK" (no router needed)

---

## 🎵 Ringtone Patterns

| Tune | Pattern | Duration | Best For |
|------|---------|----------|----------|
| **📱 Nokia** | Fast triple beeps (4 cycles) | ~2.4s | Nostalgic wake-up |
| **⌚ Casio** | Two quick beeps (2 cycles) | ~1s | Quick alerts |
| **🔊 Beep** | Steady medium beeps (5x) | ~3s | Reliable wake-up |
| **🎵 Melody** | Musical do-re-mi pattern (3x) | ~3s | Pleasant alarm |
| **⚠️ Urgent** | Rapid beeps (10x) | ~2s | Heavy sleepers! |

**Test them all with the 🔊 TEST button before choosing!**

---

## 🔧 Hardware Requirements

### Components
| Component | Specification | Quantity | Notes |
|-----------|---------------|----------|-------|
| **Microcontroller** | ESP8266 (NodeMCU/Wemos D1 Mini) | 1 | Main controller |
| **Display** | P10 LED Matrix Panel (32x16) | 2 | HJS589 compatible |
| **RTC Module** | DS3231 (I2C) | 1 | With CR2032 battery |
| **Buzzer** | Active buzzer (5V) | 1 | Connected to GPIO3 |
| **Power Supply** | 5V 3A+ | 1 | More current for higher brightness |

### Additional Items
- Jumper wires (Male-to-Female recommended)
- Breadboard (optional, for prototyping)
- Project enclosure (optional, for finished look)

---

## 📐 Wiring Diagram

### P10 Panel Connection (HJS589)
```
ESP8266          P10 Panel
--------         ---------
D0 (GPIO16) -->  OE
D1 (GPIO5)  -->  CLK
D2 (GPIO4)  -->  LAT
D3 (GPIO0)  -->  A
D4 (GPIO2)  -->  B
D5 (GPIO14) -->  C
D6 (GPIO12) -->  D
D7 (GPIO13) -->  R1
D8 (GPIO15) -->  R2 (for 2nd panel)
GND         -->  GND
5V          -->  VCC
```

### DS3231 RTC Module
```
ESP8266          DS3231
--------         ------
D1 (GPIO5)  -->  SCL
D2 (GPIO4)  -->  SDA
3.3V        -->  VCC
GND         -->  GND
```

### Buzzer Connection
```
ESP8266          Buzzer
--------         ------
D9 (GPIO3)  -->  Positive (+)
GND         -->  Negative (-)
```

> **Important:** GPIO3 is the RX pin. Disconnect buzzer when uploading code!

---

## 💻 Software Requirements

### Arduino IDE Setup
1. **Install Arduino IDE** (1.8.19 or later)
2. **Add ESP8266 Board Support**
   - Go to `File` → `Preferences`
   - Add to "Additional Board Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Install "ESP8266" from `Tools` → `Board` → `Boards Manager`

### Required Libraries
Install via `Sketch` → `Include Library` → `Manage Libraries`:

| Library | Version | Purpose |
|---------|---------|---------|
| **HJS589** | Latest | P10 LED Matrix control |
| **RtcDS3231** (Rtc by Makuna) | Latest | DS3231 RTC communication |
| **ESP8266WiFi** | Built-in | WiFi functionality |
| **ESP8266WebServer** | Built-in | Web server |
| **Wire** | Built-in | I2C communication |
| **SPI** | Built-in | SPI communication |

### Font Files Required
Ensure these fonts are in your HJS589 library folder:
- `ElektronMart5x6.h`
- `ElektronMart6x8.h`
- `ElektronMart6x16.h`

---

## 🚀 Installation

### 1. Download Project
```bash
git clone <repository-url>
cd sewuclock
```

### 2. Open in Arduino IDE
- Open `sewuclock.ino`

### 3. Configure Board Settings
```
Board: "NodeMCU 1.0 (ESP-12E Module)"
Upload Speed: 115200
CPU Frequency: 80 MHz
Flash Size: "4M (1M SPIFFS)"
Port: <Your COM Port>
```

### 4. Upload Code
- **IMPORTANT:** Disconnect buzzer from GPIO3 (RX) before upload!
- Click "Upload" button
- Wait for compilation and upload
- Reconnect buzzer after upload complete

### 5. Verify Serial Output
Open Serial Monitor (115200 baud):
```
=== SEWU AUDIO V3.7 ===
[BUZZER] OK
[RTC] OK
[WiFi] OK
[WEB] OK
[DISP] OK
=== READY ===
```

---

## 📡 WiFi Configuration

### Default Credentials
- **SSID:** `SEWU CLOCK`
- **Password:** `sewuaudio123`
- **IP Address:** `192.168.4.1`

### Access Web Interface
1. Connect to `SEWU CLOCK` WiFi network
2. Open browser: `http://192.168.4.1`
3. You'll see the premium mobile interface!

---

## 🌐 Web Interface Guide

### 📱 Main Display
```
┌─────────────────────────────────┐
│  🕐 SEWU CLOCK                  │
│  Alarm V3.7                     │
├─────────────────────────────────┤
│       02:30:45 PM  (12h mode)   │
│    or 14:30:45     (24h mode)   │
├─────────────────────────────────┤
│ HARI │  TGL  │ SUHU              │
│ KAM  │ 26/12 │ 28°C              │
└─────────────────────────────────┘
```

### 🔔 Alarm Setup
```
┌───────────────────────────────┐
│ Waktu  [07:00]                │ ← Time picker
│ Nada   [Nokia ▼] 🔊 TEST      │ ← Tune + preview
│ Aktif  [●─────] ON            │ ← Enable toggle
└───────────────────────────────┘
```

**How to Use:**
1. **Set Time:** Tap time field → Use native picker
2. **Choose Tune:** Select from dropdown → Tap TEST to preview
3. **Enable:** Toggle switch to ON → Alarm is now active!

**Pro Tip:** Changes only save when alarm is ENABLED (dirty tracking prevents accidents!)

### 🕐 Format Toggle
```
┌─────────────────────┐
│ [24 Jam ✓] [12 Jam] │ ← Tap to switch
└─────────────────────┘
```
- **24 Jam:** Military time (00:00 - 23:59)
- **12 Jam:** Standard AM/PM (12:00 AM - 11:59 PM)

### 🔆 Brightness Control
```
[────●────────] 100
```
- Range: 18 (dim) to 255 (max)
- Adjust slider → Tap "Simpan" to save

### ⏰ Set Date/Time
```
Tanggal [2025-12-26]  ← Native date picker
Jam     [14:30]       ← Native time picker
```

---

## 🎨 LED Display Layout

### Two-Panel Configuration (64x16 pixels)

#### 24-Hour Format:
```
┌─────────────────┬─────────────────┐
│   PANEL 1       │   PANEL 2       │
│   [14] : [30]   │   [26] [KAM]   │
│          [45]   │        [DES]   │
└─────────────────┴─────────────────┘
    Hour/Min/Sec      Date/Day/Month
```

#### 12-Hour Format:
```
┌─────────────────┬─────────────────┐
│   PANEL 1       │   PANEL 2       │
│   [02] : [30]   │   [26] [KAM]   │
│          [45]   │        [DES]   │
└─────────────────┴─────────────────┘
    Converted Hour     Date/Day/Month
```

**Note:** LED display shows time without AM/PM (space constraints). Web UI shows AM/PM for clarity in 12h mode.

---

## 🔌 API Endpoints

### GET `/api/data`
Get current system data.

**Response:**
```json
{
  "hour": 14,
  "minute": 30,
  "second": 45,
  "day": 26,
  "month": 12,
  "dayName": "KAM",
  "temp": 28,
  "brightness": 100,
  "format24": true
}
```

### GET `/api/alarms`
Get all alarm configurations.

**Response:**
```json
{
  "alarms": [
    {
      "id": 0,
      "hour": 7,
      "minute": 0,
      "enabled": true,
      "tune": 0
    },
    {
      "id": 1,
      "hour": 12,
      "minute": 0,
      "enabled": false,
      "tune": 1
    }
  ],
  "active": false,
  "currentAlarm": -1
}
```

### POST `/api/alarms`
Update alarm configuration.

**Request:**
```json
{
  "id": 0,
  "hour": 7,
  "minute": 30,
  "enabled": true,
  "tune": 0
}
```

### POST `/api/display`
Update display settings.

**Request:**
```json
{
  "brightness": 150,
  "format24": false
}
```

### POST `/api/test`
Preview/test a tune.

**Request:**
```json
{
  "tune": 0
}
```

### POST `/api/stop`
Manually stop active alarm.

**Response:**
```json
{
  "ok": 1
}
```

### GET `/?time=HH:MM&date=YYYY-MM-DD`
Set RTC time/date via URL parameters.

**Example:**
```
http://192.168.4.1/?time=14:30&date=2025-12-26
```

---

## ⚙️ Configuration

### Change WiFi Credentials
Edit in `setup()` function:
```cpp
WiFi.softAP("YOUR_SSID", "YOUR_PASSWORD");
```

### Default Brightness
Modify global variable:
```cpp
int brightness = 100; // Range: 18-255
```

### Default Time Format
```cpp
bool format24 = true; // true = 24h, false = 12h
```

### Display Refresh Rate
```cpp
timer0_write(ESP.getCycleCount() + 80000); // Default: 80000
```

---

## 🐛 Troubleshooting

### Display Issues

**Problem:** Display not showing
- ✅ Check all wiring connections (especially GND)
- ✅ Verify 5V power supply adequate (min 2A)
- ✅ Ensure HJS589 library installed correctly
- ✅ Check panel configuration: `HJS589 Disp(2, 1);`

**Problem:** Display flickering
- ✅ Insufficient power (upgrade to 3A+)
- ✅ Loose wiring connections
- ✅ Lower brightness level

### WiFi Issues

**Problem:** WiFi not broadcasting
- ✅ Verify ESP8266 powered properly
- ✅ Check Serial Monitor for boot messages
- ✅ Press RESET button
- ✅ No WiFi name conflicts nearby

**Problem:** Can't connect to web interface
- ✅ Connected to "SEWU CLOCK" network?
- ✅ Use exact IP: `192.168.4.1`
- ✅ Try different browser
- ✅ Clear browser cache

### RTC Issues

**Problem:** Time not accurate
- ✅ Set time via web interface
- ✅ Check DS3231 battery (CR2032)
- ✅ Verify I2C connections (SDA, SCL)
- ✅ Check Serial Monitor for RTC errors

**Problem:** Time resets on power loss
- ✅ Install CR2032 battery in DS3231
- ✅ Battery may be dead (replace)

### Alarm Issues

**Problem:** Buzzer not beeping
- ✅ Check buzzer connection to GPIO3
- ✅ Active buzzer required (not passive)
- ✅ Ensure alarm is ENABLED
- ✅ Time must match alarm setting

**Problem:** Upload fails
- ✅ **Disconnect buzzer from GPIO3 (RX) before upload!**
- ✅ Reconnect after upload complete

**Problem:** Alarm resets to default
- ✅ This is normal - settings are runtime only
- ✅ Set alarm after each power cycle
- ✅ Or modify default values in code

### Web UI Issues

**Problem:** Time/tune picker closes unexpectedly
- ✅ This is FIXED in v3.7!
- ✅ Update to latest code
- ✅ Pickers now stay open

**Problem:** Alarm time won't save
- ✅ Enable alarm FIRST
- ✅ Then change time
- ✅ Dirty tracking prevents saves when disabled

---

## 🔬 Technical Specifications

### Memory Usage
- **Flash:** ~350KB program storage
- **RAM:** ~30KB dynamic memory
- **PROGMEM:** Web page stored in flash
- **No EEPROM:** Prevents flash wear

### Performance
- **Display Refresh:** Hardware timer interrupts (~1ms)
- **Web Updates:** Clock (1s), Alarms (3s)
- **API Response:** < 50ms typical
- **Alarm Check:** Once per minute

### Power Consumption
- **Idle:** ~150mA @ 5V (0.75W)
- **Max Brightness:** ~800mA @ 5V (4W)
- **Typical:** ~400mA @ 5V (2W at 50% brightness)

### Supported Browsers
- ✅ Chrome/Edge (Desktop & Mobile)
- ✅ Safari (iOS & macOS)
- ✅ Firefox (Desktop & Mobile)
- ✅ Samsung Internet
- ⚠️ IE not supported (use modern browser)

---

## 📝 Version History

### V3.7 FINAL (Current - Production Ready!)
- ✨ 2 alarms (simplified from 5)
- ✨ 5 classic tunes (Nokia, Casio, Beep, Melody, Urgent)
- ✨ Tune preview with TEST button
- ✨ 12/24 hour format toggle
- ✨ LED display follows time format
- ✨ Proper dirty tracking (no resets!)
- ✨ Fixed auto-close on pickers
- ✨ Styled date picker to match theme
- ✨ Mobile-first responsive design
- ✨ "SEWU CLOCK" branding
- 🐛 All known bugs fixed

### V3.6
- Modern web interface
- Single alarm system
- Basic buzzer control

### V3.0
- Initial stable release
- Clock display only

---

## 💡 Pro Tips

### Alarm Setup
1. **Test Before Enable:** Use 🔊 TEST to preview each tune
2. **Enable Last:** Set time → Choose tune → Enable (prevents accidental saves)
3. **Morning Alarm:** Nokia or Melody (pleasant wake-up)
4. **Quick Alerts:** Casio (short & sweet)
5. **Heavy Sleepers:** Urgent (rapid beeps!)

### Display Optimization
1. **Brightness:** Start at 100, adjust based on ambient light
2. **Night Mode:** Use 18-50 brightness for dark rooms
3. **Daytime:** Use 150-255 for bright environments
4. **Power Saving:** Lower brightness = less power consumption

### WiFi Setup
1. **First Boot:** Connect to "SEWU CLOCK" immediately
2. **Set Time:** Use web interface right away
3. **Bookmark:** Save `192.168.4.1` in browser
4. **Mobile:** Add to home screen for quick access

### Reliability
1. **Power:** Use quality 5V power supply (min 2A)
2. **RTC Battery:** Replace CR2032 yearly
3. **Wiring:** Secure connections prevent glitches
4. **Placement:** Keep away from strong magnetic fields

---

## 🛠️ Customization Ideas

### Expand Alarm Count
Change from 2 to more alarms:
```cpp
Alarm alarms[5] = { ... }; // Change 2 to desired count
// Update all loops: for(int i = 0; i < 5; i++)
```

### Add More Tunes
Create new beep pattern in `updateBuzzer()`:
```cpp
else if (tune == 5) {
  // Your custom pattern here!
}
```

### Change Boot Message
In `setup()`:
```cpp
Disp.drawText(7, 4, "YOUR TEXT");
```

### Temperature in Fahrenheit
In `readRTC()`:
```cpp
suhu = (int)(t.AsFloatDegC() * 9/5 + 32);
```

### Longer Alarm Duration
Increase beep cycles in buzzer patterns:
```cpp
if (cycle >= 10) { // Change 10 to higher value
```

---

## 📄 License

This project is open-source. Free to use, modify, and distribute!

**Created with ❤️ by SEWU AUDIO Team**

---

## 🤝 Contributing

Contributions welcome!
- Report bugs via issues
- Suggest features
- Submit pull requests
- Share improvements

---

## 🎯 Credits

- **ESP8266 Community** - Arduino core & libraries
- **Makuna** - RtcDS3231 library
- **HJS589** - P10 LED matrix library  
- **ElektronMart** - Display fonts
- **You** - For using SEWU CLOCK! 🙏

---

## 📞 Support

**Having issues?**
1. Check troubleshooting section above
2. Verify all connections
3. Read error messages in Serial Monitor
4. Check library versions

**Still stuck?**
- Open an issue on repository
- Include Serial Monitor output
- Describe exact symptoms
- Photos of wiring help!

---

**SEWU CLOCK V3.7 - Production Ready! 🔥**

*Professional LED alarm clock with premium web interface*

**Made with 💯 by SEWU AUDIO**

---

### Quick Start Checklist

- [ ] Wire all components (P10, DS3231, Buzzer)
- [ ] Install Arduino IDE + ESP8266 support
- [ ] Install required libraries (HJS589, RtcDS3231)
- [ ] **Disconnect buzzer from RX before upload!**
- [ ] Upload sewuclock.ino
- [ ] Reconnect buzzer after upload
- [ ] Connect to "SEWU CLOCK" WiFi
- [ ] Open http://192.168.4.1
- [ ] Set current time & date
- [ ] Configure alarms
- [ ] Test tunes with 🔊 TEST button
- [ ] Enable alarms
- [ ] Enjoy your SEWU CLOCK! 🎉

**Total Setup Time:** ~30 minutes
**Difficulty:** Intermediate
**Satisfaction:** 100% 🔥
