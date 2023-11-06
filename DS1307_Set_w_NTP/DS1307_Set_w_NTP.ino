/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-date-time-ntp-client-server-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include "RTClib.h"
#include <WiFi.h>
#include "time.h"
#include "Wire.h"

// Which RTC is Connected  Uncomment the RTC Board connected to IC2
#define PCF8523
//#define DS1307

#ifdef PCF8523
  RTC_PCF8523 rtc;
#endif

#ifdef DS1307
  RTC_DS1307 rtc;
#endif

//const char* ssid     = "R2-EK0";
//const char* password = "onetruedarth";

const char* ssid     = "mustafar";
const char* password = "V@d3rWasFram3d";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -6*60*60;
const int   daylightOffset_sec = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup(){
  Serial.begin(115200);

  // Setup I2C on Pins 42 & 44
  Wire.begin(43,44);

  #ifdef PCF8523
    // if (! rtc.initialized() || rtc.lostPower()) {
    //   // When time needs to be set on a new device, or after a power loss, the
    //   // following line sets the RTC to the date & time this sketch was compiled
    //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // }
    rtc.start();
  #endif

  #ifdef DS1307
    rtc.begin();
  #endif

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setTimeonRTC();
  // printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop(){
  delay(1000);
  printLocalTime();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void setTimeonRTC(){
  struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
 
  Serial.println("Set iTime Varable");

  int year = timeinfo.tm_year;
  char timeYear[5];
  char timeMonth[3];
  char timeDay[3];
  char timeHour[3];
  char timeMinute[3];
  char timeSecond[3];
  
  strftime(timeYear,5, "%Y", &timeinfo);
  strftime(timeMonth,3, "%m", &timeinfo);
  strftime(timeDay,3, "%d", &timeinfo);
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);
  strftime(timeSecond,3, "%S", &timeinfo);

  Serial.println("Set time on RTC");

  rtc.adjust(DateTime(atoi(timeYear), atoi(timeMonth), atoi(timeDay), atoi(timeHour), atoi(timeMinute), atoi(timeSecond)));

  DateTime now = rtc.now();

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    // Serial.print(atoi(timeYear));
    // Serial.print('/');
    // Serial.print(timeMonth);
    // Serial.print('/');
    // Serial.print(timeDay);
    // Serial.print(" ");
    // Serial.print(timeHour);
    // Serial.print(':');
    // Serial.print(timeMinute);
    // Serial.print(':');
    // Serial.print(timeSecond);
    // Serial.println();
}