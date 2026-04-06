#include <ESP8266WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "mywifi.h" // Include your WiFi credentials header file
#include "custom_font.h"

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;
#define CLK_PIN D5  // or SCK
#define DATA_PIN D7 // or MOSI
#define CS_PIN D8   // or SS
int numberOfHorizontalDisplays = 8;
int numberOfVerticalDisplays = 1;
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 12
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Text parameters
#define CHAR_SPACING 1 // pixels between characters
#define BUF_SIZE 75
char message[BUF_SIZE] = "Initializing...";

const int UTC_offset = 9; // Japanese Standard Time (hours)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.nict.jp", UTC_offset * 3600, 63000);

// 指定されたLEDマトリクスモジュールに文字列を表示する。
// 表示領域の残りは空白列でパディングする。
// 引数:
//   modStart: 表示開始モジュール番号
//   modEnd  : 表示終了モジュール番号
//   pMsg    : 表示する文字列へのポインタ
void printText(uint8_t modStart, uint8_t modEnd, const char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t state = 0;                           // ステートマシンの現在状態
  uint8_t curLen;                              // 現在処理中の文字のカラム数カウンター
  uint16_t showLen;                            // 表示すべきカラム数
  uint8_t cBuf[8];                             // 1文字分のフォントデータバッファ
  int16_t col = ((modEnd + 1) * COL_SIZE) - 1; // 現在書き込むカラム位置（右端から開始）

  // 指定モジュール範囲の自動更新を一時停止（まとめて描画するため）
  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do // finite state machine to print the characters in the space available
  {
    switch (state)
    {
    case 0: // Load the next character from the font table
      // if we reached end of message, reset the message pointer
      // メッセージ末尾('\0')に達した場合、残りを空白でパディング
      if (*pMsg == '\0')
      {
        showLen = col - (modEnd * COL_SIZE); // padding characters / パディングのカラム数を計算
        state = 2;
        break;
      }

      // retrieve the next character form the font file
      // フォントから次の文字のビットマップデータを取得する
      showLen = mx.getChar(*pMsg++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;
      // !! deliberately fall through to next state to start displaying
      // !! 意図的にcase 1へフォールスルーして即座に描画を開始する

    case 1: // display the next part of the character / 文字の各カラムを1列ずつ描画する
      mx.setColumn(col--, cBuf[curLen++]);

      // done with font character, now display the space between chars
      // 1文字分すべてのカラムを描画し終えたら、文字間スペースへ移行
      if (curLen == showLen)
      {
        showLen = CHAR_SPACING;
        state = 2;
      }
      break;

    case 2: // initialize state for displaying empty columns / 空白カラム表示の初期化
      curLen = 0;
      state++;
      // fall through / フォールスルーでcase 3へ

    case 3: // display inter-character spacing or end of message padding (blank columns)
             // 文字間スペースまたは末尾パディングの空白カラムを描画する
      mx.setColumn(col--, 0);
      curLen++;
      if (curLen == showLen)
        state = 0; // 次の文字の処理へ戻る
      break;

    default:
      col = -1; // this definitely ends the do loop / このdoループを終了させる
    }
  } while (col >= (modStart * COL_SIZE));

  // 描画完了後に自動更新を再開してLEDに反映する
  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

#define BUTTON_PIN D3 // 例: D3ピンをボタン入力に使用

int brightnessLevels[] = {0, 4, 8, 15}; // 輝度レベル例（0〜15）
int brightnessIndex = 2;                // 初期輝度（8）

void setup()
{
  // LEDマトリクスを初期化する
  mx.begin();
  // カスタムフォント (custom_font.h) を使用するよう設定する
  mx.setFont(custom_font);
  // 起動時の輝度を最低値(0)に設定する
  mx.control(MD_MAX72XX::INTENSITY, 0);
  // 起動メッセージをLEDに表示する
  printText(0, MAX_DEVICES - 1, message);
  // 自動更新を無効化する（後続の描画と衝突しないようにする）
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // 内蔵LEDを出力モードに設定する（WiFi接続完了の確認用）
  pinMode(BUILTIN_LED, OUTPUT);      // Pin mode for Bultin LED
  pinMode(BUTTON_PIN, INPUT_PULLUP); // ボタンはGNDに落とす

  // シリアル通信を115200bpsで開始する
  Serial.begin(115200); // Set baud 115200
  Serial.setDebugOutput(true);
  delay(10);
  Serial.println("");
  Serial.print("ESP8266 Wifi - Connecting to [");
  Serial.print(ssid);
  Serial.println("]");
  Serial.println("]");
  // WiFiクライアントモード設定
  WiFi.mode(WIFI_STA);

  // WiFiを繋ぐ前に、WiFi状態をシリアルに出力
  WiFi.printDiag(Serial);

  // 指定したSSIDとパスワードでWiFi接続を開始する
  WiFi.begin(ssid, pass);

  // WiFi接続が確立するまで5秒ごとに状態を確認する
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    WiFi.printDiag(Serial);
    Serial.print(WiFi.status());
  }

  Serial.println("");
  Serial.print("Connected as ");
  Serial.println(WiFi.localIP());

  // NTPクライアントを開始し、すぐに時刻を同期する
  timeClient.begin();
  timeClient.update();

  // WiFi接続完了を内蔵LEDの点灯で通知する
  digitalWrite(BUILTIN_LED, HIGH);

  // 接続完了メッセージをLEDに表示する
  printText(0, MAX_DEVICES - 1, "Wifi setup is done.");
  delay(2000);
  Serial.println(digitalTimeString(true));
  Serial.println(digitalTimeString(false));

  // 取得したIPアドレスをLEDに表示する
  String ipAddress = "IP : " + WiFi.localIP().toString();

  printText(0, MAX_DEVICES - 1, ipAddress.c_str());
  delay(2000);
  // 表示完了後に内蔵LEDを消灯する
  digitalWrite(BUILTIN_LED, LOW);
}

// 整数値を2桁の文字列に変換して返す（1桁の場合は先頭に'0'を付ける）
// 引数:
//   digits: 変換する整数値（時・分・秒・月・日など）
String twoDigits(int digits)
{
  if (digits < 10)
  {
    // 1桁の場合は先頭に'0'を補って2桁にする（例: 9 → "09"）
    String i = '0' + String(digits);
    return i;
  }
  else
  {
    // 2桁以上はそのまま文字列に変換して返す
    return String(digits);
  }
}

// 曜日グリフの配列（インデックス1=日曜〜7=土曜、0は未使用）
char *dayOfWeek[] = {"", "(\x9A)", "(\x9B)", "(\x9C)", "(\x9D)", "(\x9E)", "(\x9F)", "(\xA0)"};

// 現在時刻を表示用の文字列に組み立てて返す
// 引数:
//   _12hours: true のとき12時間表示（AM/PM付き）、false のとき24時間表示
String digitalTimeString(bool _12hours)
{
  // NTPクライアントからエポック秒を取得してtm構造体に変換する
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime(&epochTime);

  // 時・分・秒をNTPクライアントから取得する
  int hour_now = timeClient.getHours();
  int min_now  = timeClient.getMinutes();
  int sec_now  = timeClient.getSeconds();
  // 月・日・曜日をtm構造体から取得する（月は0始まりなので+1、曜日は1始まりに合わせる）
  int month_now = ptm->tm_mon + 1;
  int day_now   = ptm->tm_mday;
  int wday_now  = ptm->tm_wday + 1; // 1=Sunday

  bool IsAM = true; // AM/PMフラグ（12時間表示のときのみ使用）
  // カスタムフォントの曜日グリフ文字列を取得する
  String weekdaynow = String(dayOfWeek[wday_now]);
  // 月/日の文字列を組み立てる（例: "04/06"）
  String datenow = twoDigits(month_now) + "/" + twoDigits(day_now);

  if (_12hours)
  {
    // 12以上は午後のため、AM/PMフラグを設定して12時間制に変換する
    if (hour_now > 11)
    {
      hour_now = hour_now - 12;
      IsAM = false;
    }
    // 時:分:秒 の文字列を組み立てる
    String timenow = twoDigits(hour_now) + ":" + twoDigits(min_now) + ":" + twoDigits(sec_now);

    // AM記号(\x95)またはPM記号(\x96)を付けて返す
    if (IsAM)
      return datenow + " " + weekdaynow + "  \x95" + timenow;
    else
      return datenow + " " + weekdaynow + "  \x96" + timenow;
  }
  else
  {
    // 24時間表示: AM/PM記号なしで日付・曜日・時刻を結合して返す
    String timenow = twoDigits(hour_now) + ":" + twoDigits(min_now) + ":" + twoDigits(sec_now);
    return datenow + weekdaynow + timenow;
  }
}

void loop()
{
  // NTPクライアントを更新して最新時刻を取得する
  timeClient.update();

  static bool prevButtonState = HIGH; // 前回のボタン状態を保持する（チャタリング防止の立ち下がり検出用）
  bool buttonState = digitalRead(BUTTON_PIN);

  // ボタンが押されたとき（立ち下がり検出）
  if (prevButtonState == HIGH && buttonState == LOW)
  {
    // 輝度インデックスを循環させて次のレベルへ切り替える
    brightnessIndex = (brightnessIndex + 1) % (sizeof(brightnessLevels) / sizeof(brightnessLevels[0]));
    mx.control(MD_MAX72XX::INTENSITY, brightnessLevels[brightnessIndex]);
    delay(200); // チャタリング防止
  }
  prevButtonState = buttonState; // 今回のボタン状態を次回比較用に保存する

  // 1秒待機してから現在時刻を12時間表示でLEDに描画する
  delay(1000);
  printText(0, MAX_DEVICES - 1, digitalTimeString(true).c_str());
  // 毎時0分の先頭数秒間に夜間自動減光を適用する
  if (timeClient.getMinutes() == 0 && timeClient.getSeconds() < 2)
  {
    // 0:00〜5:59は輝度を0（消灯）にする
    if (timeClient.getHours() < 6)
      mx.control(MD_MAX72XX::INTENSITY, 0);
    else
      // それ以外の時間帯は現在の輝度レベルに戻す
      mx.control(MD_MAX72XX::INTENSITY, brightnessLevels[brightnessIndex]);
  }
}
