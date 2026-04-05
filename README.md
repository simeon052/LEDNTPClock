# LED NTP Clock

A WiFi-connected digital clock displaying time on LED matrices, synchronized with NTP (Network Time Protocol) servers.

WiFi接続されたデジタルクロックで、LEDマトリクスに時刻を表示します。時刻はNTP (Network Time Protocol) サーバーと同期されます。

## Overview

This project builds a networked clock using an ESP8266 microcontroller and 12 chained MAX7219 8x8 LED matrix displays. The clock automatically synchronizes time via NTP and displays it with custom fonts on the LED matrices.

このプロジェクトは、ESP8266マイコンと12個直列接続したMAX7219 8x8 LEDマトリクスを使ったネットワーク時計です。NTPで時刻を自動同期し、LEDマトリクスにカスタムフォントで表示します。

## Hardware Requirements

- **Wemos D1 mini** (ESP8266-based development board)
- **MAX7219 8x8 LED Matrix** (quantity: 12, chained together)
- **SPI connection pins** on the Wemos D1:
  - CLK: D5 (SCK)
  - DATA: D7 (MOSI)
  - CS: D8 (SS)
- **Button** (optional, for brightness control) - connected to D3
- Power supply (5V recommended for LED matrices)

### 日本語

- **Wemos D1 mini** (ESP8266ベースの開発ボード)
- **MAX7219 8x8 LEDマトリクス** (12個を直列接続)
- Wemos D1 の **SPI接続ピン**:
  - CLK: D5 (SCK)
  - DATA: D7 (MOSI)
  - CS: D8 (SS)
- **ボタン** (任意、輝度調整用) - D3に接続
- 電源 (LEDマトリクス用に5V推奨)

## Software Requirements

### Arduino Libraries

- `ESP8266WiFi` (built-in with Arduino IDE for ESP8266)
- `NTPClient` by Fabrice Weinberg - For NTP time synchronization
- `MD_MAX72xx` by Marco Colli - For MAX7219 LED matrix control
- `SPI` (built-in)

Install libraries through Arduino IDE:
1. Sketch -> Include Library -> Manage Libraries
2. Search for and install:
   - `NTPClient` by Fabrice Weinberg
   - `MD_MAX72XX` by Marco Colli

### 日本語

- `ESP8266WiFi` (ESP8266用Arduino IDEに同梱)
- `NTPClient` by Fabrice Weinberg - NTP時刻同期用
- `MD_MAX72xx` by Marco Colli - MAX7219 LEDマトリクス制御用
- `SPI` (同梱)

Arduino IDEからライブラリをインストールします:
1. Sketch -> Include Library -> Manage Libraries を開く
2. 以下を検索してインストール:
   - `NTPClient` by Fabrice Weinberg
   - `MD_MAX72XX` by Marco Colli

## Installation & Setup

### 1. Configure WiFi Credentials

Create a file named `mywifi.h` in the project directory:

プロジェクトディレクトリに `mywifi.h` を作成します:

```cpp
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
```

### 2. Custom Font Support

This project uses a local custom font file (`custom_font.h`) and applies it in `setup()` with:

このプロジェクトではローカルのカスタムフォントファイル (`custom_font.h`) を使用し、`setup()` 内で以下を呼び出して適用します:

```cpp
mx.setFont(custom_font);
```

No replacement of files inside the Arduino library folder is required.

Arduinoライブラリフォルダ内のファイル置き換えは不要です。

### 3. Upload to Device

1. Open `LEDNTPClock.ino` in Arduino IDE
2. Select board: "WeMos D1 R2 & mini"
3. Select appropriate COM port
4. Click Upload

### 日本語

1. Arduino IDEで `LEDNTPClock.ino` を開く
2. ボードに "WeMos D1 R2 & mini" を選択
3. 使用するCOMポートを選択
4. Upload をクリック

## Configuration

### Time Zone

Edit the UTC offset in the sketch:

スケッチ内のUTCオフセットを編集します:

```cpp
const int UTC_offset = 9; // Japanese Standard Time (hours)
```

Adjust this value for your timezone (e.g., `0` for UTC, `-5` for EST).

タイムゾーンに合わせて値を変更してください (例: UTCなら `0`、ESTなら `-5`)。

### NTP Server

The NTP server is set to `ntp.nict.jp` (Japan Standard Time server). Change as needed:

NTPサーバーは `ntp.nict.jp` (日本標準時サーバー) に設定されています。必要に応じて変更してください:

```cpp
NTPClient timeClient(ntpUDP, "ntp.nict.jp", UTC_offset * 3600, 63000);
```

### Brightness Control

Brightness cycles through 4 levels by pressing the button on D3:

D3のボタンを押すごとに、輝度が4段階で切り替わります:

- `0, 4, 8, 15` (intensity scale 0-15, default index 2 = level 8)

0:00から6:00の間は、毎時のタイミングで自動的に輝度0 (消灯) になります。

## Features

- Automatic time synchronization via NTP (`ntp.nict.jp`)
- WiFi connectivity (station mode)
- Custom font support including day-of-week characters
- 12-hour / 24-hour time formatting
- Brightness adjustment via button (4 levels)
- Auto-dim at night (0:00-6:00)
- All-LED flash on startup completion

### 日本語

- NTP (`ntp.nict.jp`) による自動時刻同期
- WiFi接続 (ステーションモード)
- 曜日文字を含むカスタムフォント表示
- 12時間 / 24時間表示切り替え
- ボタンによる輝度調整 (4段階)
- 夜間自動減光 (0:00-6:00)
- 起動完了時に全LEDをフラッシュ表示

## Startup Sequence

1. Displays `Initializing...` while booting
2. Connects to WiFi (retries every 5 seconds)
3. Synchronizes time with NTP server
4. Displays `Wifi setup is done.`
5. Displays assigned IP address
6. Flashes all LEDs briefly to indicate ready
7. Begins normal clock display

### 日本語

1. 起動中に `Initializing...` を表示
2. WiFiに接続 (5秒ごとに再試行)
3. NTPサーバーと時刻同期
4. `Wifi setup is done.` を表示
5. 割り当てられたIPアドレスを表示
6. 準備完了表示として全LEDを短く点滅
7. 通常の時計表示を開始

## Usage

Once uploaded and powered:
1. The device connects to your WiFi network
2. Synchronizes time with NTP servers
3. Displays current time on LED matrices
4. Press button on D3 to cycle through brightness levels

### 日本語

書き込み後に電源を入れると:
1. WiFiネットワークへ接続
2. NTPサーバーと時刻同期
3. LEDマトリクスに現在時刻を表示
4. D3ボタンで輝度レベルを切り替え

## Customization

### LED Matrix Configuration

Adjust number of displays (if different from 12):

表示器の数が12と異なる場合は以下を変更します:

```cpp
#define MAX_DEVICES 12
```

### Adding Custom Characters

Edit `custom_font.h` to add or modify glyphs (for example, day-of-week glyphs and AM/PM indicators). The sketch already loads this font via `mx.setFont(custom_font)`.

`custom_font.h` を編集してグリフを追加・変更できます (例: 曜日グリフ、AM/PM表示)。スケッチ側では `mx.setFont(custom_font)` で読み込み済みです。

## Troubleshooting

- **Clock not updating**: Check WiFi credentials in `mywifi.h`
- **LED display not showing**: Verify SPI connections (CLK, DATA, CS pins)
- **Incorrect time**: Verify `UTC_offset` is correct for your timezone
- **Compilation errors**: Ensure `NTPClient` (Fabrice Weinberg) and `MD_MAX72XX` libraries are installed

### 日本語

- **時計が更新されない**: `mywifi.h` のWiFi認証情報を確認
- **LED表示されない**: SPI配線 (CLK, DATA, CS) を確認
- **時刻がずれる**: `UTC_offset` がタイムゾーンに合っているか確認
- **コンパイルエラー**: `NTPClient` と `MD_MAX72XX` がインストールされているか確認

## License

[Add your license information here]

ライセンス情報をここに記載してください。

## Credits

Uses libraries:

- [MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72xx) - LED Matrix control
- [NTPClient](https://github.com/arduino-libraries/NTPClient) - NTP time synchronization

使用ライブラリ:

- [MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72xx) - LEDマトリクス制御
- [NTPClient](https://github.com/arduino-libraries/NTPClient) - NTP時刻同期
