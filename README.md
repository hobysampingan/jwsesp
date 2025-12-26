# SEWU COMBO v1.0

**Professional P10 LED Clock & Running Text Display**

A versatile dual-mode display system for P10 LED panels powered by ESP8266, featuring real-time clock, multi-message running text, alarm functionality, and web-based configuration.

---

## 🌟 Features

### Display Modes

**Mode 0: Clock Display**
- Real-time clock with date (DD/MM/YYYY)
- Day of week display (Indonesian)
- Temperature reading from RTC sensor
- 12-hour or 24-hour format
- Configurable display duration (1-60 seconds)
- Auto or manual mode switching

**Mode 1: Running Text**
- Up to 5 independent text messages
- Individual enable/disable per message
- Smooth scrolling animation
- Custom device name display
- Messages cycle automatically
- Configurable scroll speed

### Clock Features
- **DS3231 RTC Module**: High-precision timekeeping
- **Temperature Display**: Built-in sensor reading
- **Time Format**: Switchable 12h/24h
- **Date Display**: Indonesian day names
- **Battery Backup**: Maintains time during power loss

### Alarm Features
- **Configurable Alarm**: Set hour and minute
- **Multiple Tunes**: 4 different alarm melodies
- **Duration Control**: 1-5 minutes alarm time
- **Buzzer Output**: Non-blocking tone generation
- **Enable/Disable**: Web-based alarm control

### Network Features
- **WiFi Access Point**: Creates own network
- **Static IP**: 192.168.4.1 (easy to remember)
- **Watchdog Recovery**: Auto-restart on connection loss
- **No Internet Required**: Fully offline operation
- **Web Configuration**: Modern, responsive UI

### Configuration Features
- **Persistent Storage**: All settings saved to SPIFFS
- **5 Text Messages**: Independently configurable
- **Panel Support**: 1-4 panels (dynamic width)
- **Brightness Control**: 0-100% adjustment
- **Scroll Speed**: Customizable animation speed
- **Mode Selection**: Clock, Running Text, or Auto-cycle

---

## 🛠️ Hardware Requirements

### Essential Components
- **ESP8266** (Generic ESP8266 Module, 1MB Flash)
- **P10 LED Panel** (1-4 panels supported)
- **DS3231 RTC Module** (I2C with battery)
- **Buzzer** (Passive, for alarm)
- **Power Supply** (5V, 2-4A depending on panel count)

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

#### Buzzer Connection
```
ESP8266     →   Buzzer
GPIO3 (RX)  →   Positive (+)
GND         →   Negative (-)
```

**Note:** Alarm uses RX pin (GPIO3), Serial Monitor disabled after setup.

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
1. Connect P10 panel(s) according to pin diagram
2. Connect DS3231 RTC module (ensure battery installed)
3. Connect buzzer to GPIO3 (optional)
4. Connect power supply (5V to panels)

### 2. Software Upload
1. Open `sewucombo.ino` in Arduino IDE
2. Install required libraries via Library Manager
3. Configure WiFi SSID/Password in code (lines 29-30):
   ```cpp
   const char* ssid = "SEWU-AUDIO";
   const char* password = "12345678";
   ```
4. Select correct board settings
5. Upload sketch to ESP8266

### 3. First Boot
1. ESP8266 creates WiFi AP
2. SSID: As configured (default: `SEWU-AUDIO`)
3. Password: As configured (default: `12345678`)
4. Connect to WiFi AP
5. Open browser: `http://192.168.4.1`

---

## ⚙️ Configuration

### Initial Setup

**1. Set Device Name**
- Default: "SEWU AUDIO"
- Displayed during running text cycle
- Max 64 characters

**2. Configure Display**
- **Panel Count**: 1-4 panels
- **Brightness**: 0-100%
- **Speed**: Scroll speed (lower = faster)
- **Mode**: 
  - 0 = Clock only
  - 1 = Running text only
  - 2 = Auto cycle (clock → text → text...)
- **Clock Duration**: How long clock shows (1-60 seconds)

**3. Set Running Text Messages**
- **5 Message Slots**: Enter text for each
- **Enable/Disable**: Checkbox per message
- **Auto-Save**: Changes apply immediately

**4. Configure Clock**
- **Format**: 12-hour or 24-hour
- **Date/Time**: Set via web interface
- Auto-saves to RTC with battery backup

**5. Set Alarm (Optional)**
- **Enable**: Turn alarm on/off
- **Time**: Hour and minute
- **Duration**: 1-5 minutes
- **Tune**: Select melody (1-4)

---

## 🌐 Web Interface

### Accessing the Interface
1. Connect to WiFi AP (e.g., `SEWU-AUDIO`)
2. Open browser: `http://192.168.4.1`
3. Web UI loads automatically

### Interface Sections

#### Header
- Device name display
- Current date/time
- Temperature reading

#### Info Settings
- Device name input
- 5 running text message inputs
- Enable/disable checkboxes

#### Display Settings
- Brightness slider (0-100%)
- Scroll speed control
- Display mode selector (Clock/Text/Auto)
- Panel count dropdown (1-4)
- Clock duration (seconds)
- Time format (12h/24h)

#### Time Settings
- Date picker
- Time picker (24h format)
- One-click update to RTC

#### Alarm Settings
- Enable/disable switch
- Hour and minute selection
- Duration selector (1-5 min)
- Tune picker (1-4 melodies)

---

## 📺 Display Examples

### Clock Mode (2 Panels, 64px)
```
┌──────────────────┐
│ 23:08  SAB  25°C │  ← Time, Day, Temp
│ 25/12/2025       │  ← Date
└──────────────────┘
```

### Running Text Mode
```
┌──────────────────┐
│ SEWU AUDIO       │  ← Device name
│ [MESSAGE] ─────→ │  ← Scrolling text
└──────────────────┘
```

### Auto Cycle Mode
- Shows clock for configured duration (e.g., 10 seconds)
- Cycles through enabled running text messages
- Returns to clock, repeats

---

## 🔔 Alarm Operation

### How It Works
1. Alarm enabled and time matches current time
2. Buzzer starts playing selected tune
3. Plays for configured duration (default: 1 minute)
4. Stops automatically
5. Resets for next day

### Tune Patterns
- **Tune 1**: Simple beep pattern
- **Tune 2**: Rising tone pattern
- **Tune 3**: Complex melody
- **Tune 4**: Rapid beep sequence

**Note:** Buzzer uses non-blocking tone generation, display continues updating normally.

---

## 🔌 API Endpoints

### GET /
Returns main HTML page with embedded configuration interface.

### GET /api/data
Returns current configuration and RTC data.

**Response:**
```json
{
  "hour": 23,
  "minute": 8,
  "second": 39,
  "day": 25,
  "month": 12,
  "year": 2025,
  "dayName": "SAB",
  "temp": 25,
  "nama": "SEWU AUDIO",
  "info1": "Message 1...",
  "info2": "Message 2...",
  "info3": "Message 3...",
  "info4": "Message 4...",
  "info5": "Message 5...",
  "enable1": 1,
  "enable2": 1,
  "enable3": 0,
  "enable4": 0,
  "enable5": 0,
  "brightness": 50,
  "speed": 50,
  "displayMode": 2,
  "separator": 1,
  "jamDuration": 10,
  "format24h": 1,
  "alarmEnable": 0,
  "alarmHour": 6,
  "alarmMinute": 0,
  "alarmDuration": 1,
  "alarmTune": 1
}
```

### POST /api/info
Update device name and running text messages.

**Request:**
```json
{
  "nama": "SEWU AUDIO",
  "info1": "Welcome to our store!",
  "info2": "Special offer today!",
  "info3": "Open Mon-Sat 9AM-9PM",
  "info4": "Contact: 081234567890",
  "info5": "Thank you for visiting",
  "enable1": 1,
  "enable2": 1,
  "enable3": 1,
  "enable4": 0,
  "enable5": 0
}
```

### POST /api/display
Update display settings.

**Request:**
```json
{
  "brightness": 75,
  "speed": 50,
  "displayMode": 2,
  "separator": 1,
  "jamDuration": 15,
  "format24h": 1
}
```

### POST /api/alarm
Update alarm configuration.

**Request:**
```json
{
  "alarmEnable": 1,
  "alarmHour": 6,
  "alarmMinute": 30,
  "alarmDuration": 2,
  "alarmTune": 2
}
```

### GET / (with query params)
Set RTC time and date.

**Examples:**
- Set time: `http://192.168.4.1/?time=14:30`
- Set date: `http://192.168.4.1/?date=2025-12-25`

---

## 🐛 Troubleshooting

### Display Issues

**Blank Display**
- Check power supply connections
- Verify P10 panel wiring
- Check brightness setting (not 0%)
- Ensure panels match configured count

**Flickering Display**
- Reduce CPU frequency to 80MHz
- Check ground connections
- Verify power supply amperage is sufficient

**Wrong Width**
- Set correct panel count in settings
- Each panel = 32px width
- 2 panels = 64px (most common)

### Network Issues

**Can't Connect to WiFi**
- Check SSID/Password in code
- Ensure device is in AP mode (check Serial Monitor)
- WiFi channel may conflict, try changing in code
- Reboot ESP8266

**Web Page Won't Load**
- Verify IP: `192.168.4.1`
- Check you're connected to ESP8266's WiFi
- Try different browser
- Clear browser cache

**Settings Not Saving**
- Check SPIFFS is mounted (Serial Monitor)
- May need to format SPIFFS (first boot)
- Verify flash size includes FS

### Clock Issues

**Wrong Time**
- Use web interface to set RTC
- Check RTC battery is installed and charged
- Verify I2C connections (SDA/SCL)
- Check RTC module is genuine DS3231

**Temperature Shows Wrong Value**
- DS3231 sensor is ±3°C accurate
- Allow 30 seconds for stabilization
- Value is in Celsius

**Date/Time Resets**
- Replace RTC battery (CR2032)
- Check battery holder connections

### Alarm Issues

**Alarm Doesn't Sound**
- Check buzzer connections (GPIO3/RX, GND)
- Verify buzzer is passive type (not active)
- Ensure alarm is enabled in settings
- Check alarm time is set correctly
- Serial Monitor disabled (normal, RX pin used)

**Alarm Won't Stop**
- Wait for configured duration
- Power cycle if stuck
- Check alarm duration setting

---

## 🔧 Advanced Configuration

### Changing WiFi Credentials
Edit `sewucombo.ino` lines 29-30:
```cpp
const char* ssid = "YOUR-NETWORK-NAME";
const char* password = "YOUR-PASSWORD";
```
Re-upload sketch.

### Panel Count Configuration
No code changes needed! Set via web interface:
- 1 Panel = 32px wide
- 2 Panels = 64px wide
- 3 Panels = 96px wide
- 4 Panels = 128px wide

### Custom Alarm Tunes
Edit `buzzerWorker()` function in code to create custom melody patterns using different frequencies and durations.

### WiFi Watchdog Tuning
Adjust watchdog timeout (default: 2 minutes):
```cpp
const unsigned long WATCHDOG_TIMEOUT = 120000; // milliseconds
```

---

## 📝 Configuration File

Settings stored in `/config.json` on SPIFFS:

```json
{
  "nama": "SEWU AUDIO",
  "info1": "Welcome...",
  "info2": "...",
  "enable1": 1,
  "brightness": 50,
  "speed": 50,
  "displayMode": 2,
  "jamDuration": 10,
  "format24h": 1,
  "alarmEnable": 0,
  "alarmHour": 6,
  "alarmMinute": 0,
  "alarmDuration": 1,
  "alarmTune": 1
}
```

To reset to defaults: Flash with "Erase Flash: All Flash Contents".

---

## 🎯 Use Cases

### Digital Storefront Sign
- Display store name (device name)
- Show promotions (running text messages)
- Display current time/date
- Set opening hours reminder (alarm)

### Office Reception Display
- Company name and welcome message
- Important announcements
- Meeting room schedule
- Time and temperature display

### Event Signage
- Event name and details
- Scrolling program schedule
- Real-time clock for timing
- Panel-scalable for different venues

### Home Automation
- Personalized wake-up alarm
- Daily reminders via text messages
- Temperature monitoring
- Custom notification display

---

## 🔬 Technical Details

### Memory Usage
- Flash: ~350KB (sketch)
- SPIFFS: 64KB (configuration)
- RAM: ~35KB free during operation

### Display Refresh
- Timer interrupt-driven (25 FPS)
- Uses `timer0_attachInterrupt()`
- Detached during config save (anti-crash)
- Restored immediately after

### WiFi Management
- AP-only mode (no STA)
- Static IP configuration
- Watchdog monitors connection
- Auto-restart on prolonged disconnect
- Status LED feedback

### RTC Integration
- I2C communication (Wire library)
- Read every second in loop
- Temperature updated with time
- Battery-backed timekeeping

---

## 🎨 Project Structure

```
sewucombo/
├── sewucombo.ino          # Main sketch
├── sewucombo_page.h       # Web UI (embedded HTML)
├── HJS589/                # Display library
│   ├── HJS589.h
│   ├── HJS589.cpp
│   ├── Bitmap.h
│   ├── Bitmap.cpp
│   └── fonts/
│       ├── ElektronMart5x6.h
│       ├── ElektronMart6x8.h
│       ├── ElektronMart6x16.h
│       └── SewuAudio6x16.h
└── README.md              # This file
```

---

## 📜 License

This project is open-source and free to use for personal or commercial purposes.

---

## 🙏 Credits

**Developed by:** SEWU Audio Team  
**Hardware:** ESP8266 + P10 LED Matrix + DS3231 RTC  
**Libraries:** HJS589 (P10 driver), RtcDS3231, ArduinoJson  
**Custom Fonts:** SewuAudio font family  

---

## 📞 Support

For issues or questions:
1. Check Troubleshooting section above
2. Review Serial Monitor output (115200 baud)
3. Verify hardware connections match pin diagrams
4. Check configuration file in SPIFFS

### Common Quick Fixes
- **Reset Config**: Reflash with "Erase All"
- **Reset WiFi**: Change credentials in code, re-upload
- **Reset RTC**: Set time via web interface
- **Restart Device**: Power cycle ESP8266

---

**Built with ❤️ for versatile LED display applications**  
**Version 1.0 - SEWU Audio**  
**Perfect for stores, offices, events, and home automation!**
