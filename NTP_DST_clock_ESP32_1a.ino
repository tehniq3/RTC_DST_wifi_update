/*
 * https://werner.rothschopf.net/microcontroller/202103_arduino_esp32_ntp_en.htm
NTP TZ DST - bare minimum
NetWork Time Protocol - Time Zone - Daylight Saving Time

Our target for this MINI sketch is:
- get the SNTP request running
- set the timezone
- (implicit) respect daylight saving time
- how to "read" time to be printed to Serial.Monitor

This example is a stripped down version of the NTP-TZ-DST (v2) from the ESP8266
and contains some #precompiler defines to make it working for
- ESP32 core 1.0.5, 1.0.6, 2.0.2
- ESP8266 core 2.7.4, 3.0.2

by noiasca
2021-03-28

v.1.0 - initial version of Nicu FLORICA (niq_ro)
v.1.a - RTC updated just wifi is available at restart
*/

#include <Wire.h>
#include "RTClib.h"
RTC_DS1307 rtc;

#ifndef STASSID
#define STASSID "bbk3"          // set your SSID
#define STAPSK "internet3"       // set your wifi password
#endif

/* Configuration of NTP */
// choose the best fitting NTP server pool for your country
#define MY_NTP_SERVER "at.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"
#define MY_TZ "EET-2EEST,M3.5.0/3,M10.5.0/4"  // Bucuresti

/* Necessary Includes */
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>                     // we need wifi to get internet access
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h> 
#endif
#include <time.h>                    // for time() ctime()

/* Globals */
time_t now;                          // this are the seconds since Epoch (1970) - UTC
tm tm;                             // the structure tm holds time information in a more convenient way *

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int ora;
int minut;
int secunda;
int an;
int luna;
int zi;
int zis;
int wora;
int wminut;
int wsecunda;
int wan;
int wluna;
int wzi;
int wzis;
byte incercare = 0;

void showTime() {
  time(&now); // read the current time
  localtime_r(&now, &tm);             // update the structure tm with the current time
  wan = tm.tm_year + 1900;
  wluna = tm.tm_mon + 1;
  wzi = tm.tm_mday;
  wora = tm.tm_hour;
  wminut = tm.tm_min;
  wsecunda = tm.tm_sec;
  wzis = tm.tm_wday;
  
  Serial.print("year:");
  Serial.print(wan);    // years since 1900
  Serial.print("\tmonth:");
  Serial.print(wluna);        // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(wzi);           // day of month
  Serial.print("\thour:");
  Serial.print(wora);           // hours since midnight 0-23
  Serial.print("\tmin:");
  Serial.print(wminut);            // minutes after the hour 0-59
  Serial.print("\tsec:");
  Serial.print(wsecunda);            // seconds after the minute 0-61*
  Serial.print("\twday:");
  Serial.print(wzis);           // days since Sunday 0-6
  if (tm.tm_isdst == 1)               // Daylight Saving Time flag
    Serial.print("\tDST (summer time)");  
  else
    Serial.print("\tstandard (winter time)");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nNTP TZ DST - bare minimum");
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  #ifdef ARDUINO_ARCH_ESP32
  // ESP32 seems to be a little more complex:
  configTime(0, 0, MY_NTP_SERVER);  // 0, 0 because we will use TZ in the next line
  setenv("TZ", MY_TZ, 1);            // Set environment variable with your time zone
  tzset();
  #else
  // ESP8266
  configTime(MY_TZ, MY_NTP_SERVER);    // --> for the ESP8266 only
  #endif

  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
 // while (WiFi.status() != WL_CONNECTED) {
  while ((WiFi.status() != WL_CONNECTED) and (incercare < 100)) { 
    delay(200);
    Serial.print ( "." );
    incercare++;
  }
  Serial.println("\nWiFi connected");
  
  while ((WiFi.status() == WL_CONNECTED) and (wan < 2020)) 
  {
  showTime();
  delay(3000);
  if (wan >=2020)
  {
     rtc.adjust(DateTime(wan, wluna, wzi, wora, wminut, wsecunda));
  Serial.println("------RTC time updated---------");
  }
  }

}

void loop() {
DateTime now = rtc.now();  
ora = now.hour();
minut = now.minute();
secunda = now.second();
an = now.year();
luna = now.month();
zi = now.day();
zis = now.dayOfTheWeek();

    Serial.print(daysOfTheWeek[zis]);
    Serial.print(", ");
    Serial.print(zi/10);
    Serial.print(zi%10);
    Serial.print('-');
    Serial.print(luna/10);
    Serial.print(luna%10);
    Serial.print('-');
    Serial.print(an); 
    Serial.print(" -> ");
    Serial.print(ora/10);
    Serial.print(ora%10);
    Serial.print(':');
    Serial.print(minut/10);
    Serial.print(minut%10);
    Serial.print(':');
    Serial.print(secunda/10);
    Serial.print(secunda%10);
    Serial.println();

  delay(1000);
}
