#include <ESP8266WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NtpClientLib.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "mywifi.h" // Include your WiFi credentials header file

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

const int UTC_offset = 9; // Japanese Standard Time
void printText(uint8_t modStart, uint8_t modEnd, const char *pMsg)
// Print the text string to the LED matrix modules specified.
// Message area is padded with blank columns after printing.
{
  uint8_t state = 0;
  uint8_t curLen;
  uint16_t showLen;
  uint8_t cBuf[8];
  int16_t col = ((modEnd + 1) * COL_SIZE) - 1;

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  do // finite state machine to print the characters in the space available
  {
    switch (state)
    {
    case 0: // Load the next character from the font table
      // if we reached end of message, reset the message pointer
      if (*pMsg == '\0')
      {
        showLen = col - (modEnd * COL_SIZE); // padding characters
        state = 2;
        break;
      }

      // retrieve the next character form the font file
      showLen = mx.getChar(*pMsg++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;
      // !! deliberately fall through to next state to start displaying

    case 1: // display the next part of the character
      mx.setColumn(col--, cBuf[curLen++]);

      // done with font character, now display the space between chars
      if (curLen == showLen)
      {
        showLen = CHAR_SPACING;
        state = 2;
      }
      break;

    case 2: // initialize state for displaying empty columns
      curLen = 0;
      state++;
      // fall through

    case 3: // display inter-character spacing or end of message padding (blank columns)
      mx.setColumn(col--, 0);
      curLen++;
      if (curLen == showLen)
        state = 0;
      break;

    default:
      col = -1; // this definitely ends the do loop
    }
  } while (col >= (modStart * COL_SIZE));

  mx.control(modStart, modEnd, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setup()
{

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 0);
  printText(0, MAX_DEVICES - 1, message);

  pinMode(BUILTIN_LED, OUTPUT); // Pin mode for Bultin LED

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

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(5000);
    WiFi.printDiag(Serial);
    Serial.print(WiFi.status());
  }

  Serial.println("");
  Serial.print("Connected as ");
  Serial.println(WiFi.localIP());

  NTP.begin("ntp.nict.jp", UTC_offset, false);
  NTP.setInterval(63);

  digitalWrite(BUILTIN_LED, HIGH);

  printText(0, MAX_DEVICES - 1, "Wifi setup is done. ");
  delay(2000);
  Serial.println(digitalTimeString(true));
  Serial.println(digitalTimeString(false));

  String ipAddress = "IP : " + WiFi.localIP().toString();

  printText(0, MAX_DEVICES - 1, ipAddress.c_str());
  delay(2000);
  digitalWrite(BUILTIN_LED, LOW);
}

String twoDigits(int digits)
{
  if (digits < 10)
  {
    String i = '0' + String(digits);
    return i;
  }
  else
  {
    return String(digits);
  }
}

char *dayOfWeek[] = {"", "(\x9A)", "(\x9B)", "(\x9C)", "(\x9D)", "(\x9E)", "(\x9F)", "(\xA0)"};
String digitalTimeString(bool _12hours)
{
  int hour_now = hour();

  String weekdaynow = String(dayOfWeek[weekday()]);
  String datenow = twoDigits(month()) + "/" + twoDigits(day());

  bool IsAM = true;
  if (_12hours)
  {
    if (hour_now > 11)
    {
      hour_now = hour_now - 12;
      IsAM = false;
    }
    String timenow = twoDigits(hour_now) + ":" + twoDigits(minute()) + ":" + twoDigits(second());

    if (IsAM)
      return datenow + weekdaynow + "\x98\x95" + timenow;
    else
      return datenow + weekdaynow + "\x98\x96" + timenow;
  }
  else
  {
    String timenow = twoDigits(hour_now) + ":" + twoDigits(minute()) + ":" + twoDigits(second());
    return datenow + weekdaynow + "  " + timenow;
  }
}

void loop()
{
  delay(1000);
  printText(0, MAX_DEVICES - 1, digitalTimeString(true).c_str());
  if (minute() == 0 && second() < 2)
  {
    if (hour() < 6)
      mx.control(MD_MAX72XX::INTENSITY, 0);
    else
      mx.control(MD_MAX72XX::INTENSITY, 6);
  }
}
