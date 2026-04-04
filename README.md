# LED NTP Clock

A WiFi-connected digital clock displaying time on LED matrices, synchronized with NTP (Network Time Protocol) servers.

## Overview

This project builds a networked clock using an ESP8266 microcontroller and 12 chained MAX7219 8x8 LED matrix displays. The clock automatically synchronizes time via NTP and displays it with custom fonts on the LED matrices.

## Hardware Requirements

- **Wemos D1 mini** (ESP8266-based development board)
- **MAX7219 8x8 LED Matrix** (quantity: 12, chained together)
- **SPI connection pins** on the Wemos D1:
  - CLK: D5 (SCK)
  - DATA: D7 (MOSI)
  - CS: D8 (SS)
- **Button** (optional, for brightness control) - connected to D3
- Power supply (5V recommended for LED matrices)

## Software Requirements

### Arduino Libraries
- `ESP8266WiFi` (built-in with Arduino IDE for ESP8266)
- `NTPClient` by Fabrice Weinberg - For NTP time synchronization
- `MD_MAX72xx` by Marco Colli - For MAX7219 LED matrix control
- `SPI` (built-in)

Install libraries through Arduino IDE:
1. Sketch → Include Library → Manage Libraries
2. Search for and install:
   - `NTPClient` by Fabrice Weinberg
   - `MD_MAX72XX` by Marco Colli

## Installation & Setup

### 1. Configure WiFi Credentials
Create a file named `mywifi.h` in the project directory:

```cpp
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
```

### 2. Custom Font Support
The project includes a custom font header for better display options:

1. Copy `MD_MAX72xx_font_someFontsAdded.h` to the MD_MAX72xx library `src` folder:
   - Path: `C:\Users\<username>\AppData\Local\Arduino15\libraries\MD_MAX72XX\src\`

2. Replace (or rename over) the existing `MD_MAX72xx_font.h` with this file.

### 3. Upload to Device
1. Open `LEDNTPClock.ino` in Arduino IDE
2. Select board: "WeMos D1 R2 & mini"
3. Select appropriate COM port
4. Click Upload

## Configuration

### Time Zone
Edit the UTC offset in the sketch:
```cpp
const int UTC_offset = 9; // Japanese Standard Time (hours)
```
Adjust this value for your timezone (e.g., `0` for UTC, `-5` for EST).

### NTP Server
The NTP server is set to `ntp.nict.jp` (Japan Standard Time server). Change as needed:
```cpp
NTPClient timeClient(ntpUDP, "ntp.nict.jp", UTC_offset * 3600, 63000);
```

### Brightness Control
Brightness cycles through 4 levels by pressing the button on D3:
- `0, 4, 8, 15` (intensity scale 0–15, default index 2 = level 8)

Between 0:00 and 6:00, brightness is automatically set to 0 (off) at the top of each hour.

## Features

- Automatic time synchronization via NTP (`ntp.nict.jp`)
- WiFi connectivity (station mode)
- Custom font support including day-of-week characters
- 12-hour / 24-hour time formatting
- Brightness adjustment via button (4 levels)
- Auto-dim at night (0:00–6:00)
- All-LED flash on startup completion

## Startup Sequence

1. Displays `Initializing...` while booting
2. Connects to WiFi (retries every 5 seconds)
3. Synchronizes time with NTP server
4. Displays `Wifi setup is done.`
5. Displays assigned IP address
6. Flashes all LEDs briefly to indicate ready
7. Begins normal clock display

## Usage

Once uploaded and powered:
1. The device connects to your WiFi network
2. Synchronizes time with NTP servers
3. Displays current time on LED matrices
4. Press button on D3 to cycle through brightness levels

## Customization

### LED Matrix Configuration
Adjust number of displays (if different from 12):
```cpp
#define MAX_DEVICES 12
```

### Adding Custom Characters
The included `MD_MAX72xx_font_someFontsAdded.h` file adds characters beyond the default font set (day-of-week glyphs, AM/PM indicators, etc.).

## Troubleshooting

- **Clock not updating**: Check WiFi credentials in `mywifi.h`
- **LED display not showing**: Verify SPI connections (CLK, DATA, CS pins)
- **Incorrect time**: Verify `UTC_offset` is correct for your timezone
- **Compilation errors**: Ensure `NTPClient` (Fabrice Weinberg) and `MD_MAX72XX` libraries are installed

## License

[Add your license information here]

## Credits

Uses libraries:
- [MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72xx) - LED Matrix control
- [NTPClient](https://github.com/arduino-libraries/NTPClient) - NTP time synchronization
