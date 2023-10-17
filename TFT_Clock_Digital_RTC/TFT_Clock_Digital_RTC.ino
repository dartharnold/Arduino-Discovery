// Based on clock sketch by Gilchrist 6/2/2014 1.0

/*
A few colour codes:
Default color definitions
TFT_BLACK       0x0000      [   0,   0,   0 ]
TFT_NAVY        0x000F      [   0,   0, 128 ]
TFT_DARKGREEN   0x03E0      [   0, 128,   0 ]
TFT_DARKCYAN    0x03EF      [   0, 128, 128 ]
TFT_MAROON      0x7800      [ 128,   0,   0 ]
TFT_PURPLE      0x780F      [ 128,   0, 128 ]
TFT_OLIVE       0x7BE0      [ 128, 128,   0 ]
TFT_LIGHTGREY   0xD69A      [ 211, 211, 211 ]
TFT_DARKGREY    0x7BEF      [ 128, 128, 128 ]
TFT_BLUE        0x001F      [   0,   0, 255 ]
TFT_GREEN       0x07E0      [   0, 255,   0 ]
TFT_CYAN        0x07FF      [   0, 255, 255 ]
TFT_RED         0xF800      [ 255,   0,   0 ]
TFT_MAGENTA     0xF81F      [ 255,   0, 255 ]
TFT_YELLOW      0xFFE0      [ 255, 255,   0 ]
TFT_WHITE       0xFFFF      [ 255, 255, 255 ]
TFT_ORANGE      0xFDA0      [ 255, 180,   0 ]
TFT_GREENYELLOW 0xB7E0      [ 180, 255,   0 ]
TFT_PINK        0xFE19      [ 255, 192, 203 ] Lighter pink, was 0xFC9F
TFT_BROWN       0x9A60      [ 150,  75,   0 ]
TFT_GOLD        0xFEA0      [ 255, 215,   0 ]
TFT_SILVER      0xC618      [ 192, 192, 192 ]
TFT_SKYBLUE     0x867D      [ 135, 206, 235 ]
TFT_VIOLET      0x915C      [ 180,  46, 226 ]
*/

#include <TFT_eSPI.h>
#include <SPI.h>
#include "Wire.h"   // for I2C communication
#include <WiFi.h>
#include "time.h"

// Adafruit Temprature & Humidity Sensor
#include "Adafruit_Si7021.h"

// Real Time Clock
#include "RTClib.h"

// Load GFS Fonts being used
#include "AurebeshAF_Canon10pt7b.h"
#include "AurebeshAF_Canon20pt7b.h"
#include "AurebeshAF_Canon30pt7b.h"

// OpenFontRender Items
// #include "AurebeshAF-Canon.h"
// #include "OpenFontRender.h"

// OpenFontRender fRender;

// Which RTC is Connected  Uncomment the RTC Board connected to IC2
#define PCF8523
// #define DS1307

// Define Custome colors
#define TFT_GREY 0x39C4

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

byte omm = 99, oss = 99, ohh = 99, pss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

#ifdef PCF8523
  RTC_PCF8523 rtc;
#endif

#ifdef DS1307
  RTC_DS1307 rtc;
#endif

#define TEMPF_OFFSET  -1.3
#define HUMID_OFFSET  -2

const char* ssid     = "R2-EK0";
const char* password = "onetruedarth";

// const char* ssid     = "mustafar";
// const char* password = "V@d3rWasFram3d";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -6*60*60;
const int   daylightOffset_sec = 3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
char spcdaysOfTheWeek[7][12] = {"Sunday  ", "Monday  ", "Tuesday ", "Wednesday", "Thursday ", "Friday ", "Saturday "};   // padding added so all days are same size

Adafruit_Si7021 sensor = Adafruit_Si7021();

// Declare variables to hold Temprature and Humidity
float humidity, temp_c, temp_f;

// Functions
void wifi_setTime()
{
 struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  int year = timeinfo.tm_year;
  char timeYear[5];
  char timeMonth[3];
  char timeDay[3];
  char timeHour[3];
  char timeMinute[3];
  char timeSecond[3];

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 
  Serial.println("Set iTime Varable");

  strftime(timeYear,5, "%Y", &timeinfo);
  strftime(timeMonth,3, "%m", &timeinfo);
  strftime(timeDay,3, "%d", &timeinfo);
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);
  strftime(timeSecond,3, "%S", &timeinfo);

  Serial.println("Set time on RTC");

  rtc.adjust(DateTime(atoi(timeYear), atoi(timeMonth), atoi(timeDay), atoi(timeHour), atoi(timeMinute), atoi(timeSecond)));

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void draw_date()
{
  // positioning of Date Line
  int dxpos = 8;
  int dypos = 25; // Top of Date String

  DateTime now = rtc.now();

  tft.setFreeFont(&AurebeshAF_Canon10pt7b);
  dxpos += tft.drawString(spcdaysOfTheWeek[now.dayOfTheWeek()], dxpos, dypos);
  dxpos += tft.drawChar(' ', dxpos, dypos);
  if (now.month() < 10) dxpos += tft.drawNumber(0, dxpos, dypos);
  dxpos += tft.drawNumber(now.month(), dxpos, dypos);
  dxpos += tft.drawChar('/', dxpos, dypos + 16);
  if (now.day() < 10) dxpos += tft.drawNumber(0, dxpos, dypos);
  dxpos += tft.drawNumber(now.day(), dxpos, dypos);
  dxpos += tft.drawChar('/', dxpos, dypos + 16);
  dxpos += tft.drawNumber(now.year(), dxpos, dypos);
}

void get_hum_temp()
{
    // Read the humidity from the sensor.
    humidity = sensor.readHumidity() + HUMID_OFFSET;

    // Read the temperature from the sensor.
    temp_c = sensor.readTemperature();

    // Calculate temperature from C
    temp_f = (((temp_c*9)/5)+32) + TEMPF_OFFSET;
}

void draw_enviro()
{
  // Positioning of Temp & Humid
  int txpos = 8;
  int typos = 120;
  int hxpos = 8;
  int hypos = 140;

  get_hum_temp();

  tft.setFreeFont(&AurebeshAF_Canon10pt7b);
  txpos =+ tft.drawString("Temprature:     °F", txpos, typos);  // ° not displaying but left in for positioning of Temp Calc
  
  // back up the txpos based on number of Digits
  if (temp_f > 99) {
    txpos = txpos - 62;
  } else if (temp_f > 9) {
    txpos = txpos - 46;
  } else {
    txpos = txpos - 30;
  }

  txpos =+ tft.drawNumber(temp_f, txpos, typos);

  hxpos =+ tft.drawNumber(humidity, hxpos, hypos);
  hxpos =+ tft.drawString("% Humidity", hxpos + 10, hypos);
}

void draw_time()
{
  // Update digital time
  int xpos = 8;
  int ypos = 57; // Top left corner ot clock text, about half way down
  int ysecs = ypos + 14;

  DateTime now = rtc.now();

  if (omm != now.minute()) { // Redraw hours and minutes time every minute
    omm = now.minute();

    tft.setFreeFont(&AurebeshAF_Canon30pt7b);
    // Draw hours and minutes
    if (now.hour() < 10) xpos += tft.drawNumber(0, xpos, ypos); // Add hours leading zero for 24 hr clock
    xpos += tft.drawNumber(now.hour(), xpos, ypos);             // Draw hours
    xcolon = xpos; // Save colon coord for later to flash on/off later
    xpos += tft.drawChar(':', xpos, ypos + 45);
    if (now.minute() < 10) xpos += tft.drawNumber(0, xpos, ypos); // Add minutes leading zero
    xpos += tft.drawNumber(now.minute(), xpos, ypos);             // Draw minutes

    xsecs = xpos; // Save seconds 'x' position for later display updates
  }

  if (oss != now.second()) { // Redraw seconds time every second
  oss = now.second();
  xpos = xsecs;

  if (now.second() % 2) { // Flash the colons on/off
    tft.setFreeFont(&AurebeshAF_Canon30pt7b);
    tft.setTextColor(TFT_GREY, TFT_MAROON);         // Set colour to grey to dim colon
    tft.drawChar(':', xcolon, ypos + 45);         // Hour:minute colon
    tft.setFreeFont(&AurebeshAF_Canon20pt7b);
    xpos += tft.drawChar(':', xsecs, ysecs + 30); // Seconds colon
    tft.setTextColor(TFT_WHITE, TFT_MAROON);      // Set colour back to yellow
  }
  else {
    tft.setFreeFont(&AurebeshAF_Canon30pt7b);
    tft.drawChar(':', xcolon, ypos + 45);         // Hour:minute colon
    tft.setFreeFont(&AurebeshAF_Canon20pt7b);
    xpos += tft.drawChar(':', xsecs, ysecs + 30); // Seconds colon
  }

  tft.setFreeFont(&AurebeshAF_Canon20pt7b);
  //Draw seconds
  if (now.second() < 10) xpos += tft.drawNumber(0, xpos, ysecs); // Add leading zero
  tft.drawNumber(now.second(), xpos, ysecs);                     // Draw seconds
  }
}

void setup_tft(void) {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_MAROON);

  tft.setTextSize(1);
  // tft.setTextColor(TFT_YELLOW, TFT_MAROON);
  tft.setTextColor(TFT_WHITE, TFT_MAROON);

  tft.setFreeFont(&AurebeshAF_Canon30pt7b);
}

void setup_sensors(void){
  //Setup SDA, SCL for RTC
  Wire.begin(43, 44);
  
  //Setup for Serial Console
  Serial.begin(57600);

  // Is the RTC Present?
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  #ifdef PCF8523
    if (! rtc.initialized() || rtc.lostPower()) {
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      wifi_setTime();

      // Note: allow 2 seconds after inserting battery or applying external power
      // without battery before calling adjust(). This gives the PCF8523's
      // crystal oscillator time to stabilize. If you call adjust() very quickly
      // after the RTC is powered, lostPower() may still return true.
    }

    // When the RTC was stopped and stays connected to the battery, it has 
    // to be restarted by clearing the STOP bit. Let's do this to ensure
    // the RTC is running.
    rtc.start();
  #endif

  #ifdef DS1307
    rtc.begin();
  #endif

  // The PCF8523 can be calibrated for:
  //        - Aging adjustment
  //        - Temperature compensation
  //        - Accuracy tuning
  // The offset mode to use, once every two hours or once every minute.
  // The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
  // https://www.nxp.com/docs/en/application-note/AN11247.pdf
  // The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
  // and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
  // Note: any previous calibration should cancelled prior to any new observation period.
  // Example - RTC gaining 43 seconds in 1 week

  float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
  float period_sec = (7 * 86400);  // total obsevation period in seconds (86400 = seconds in 1 day:  7 days = (7 * 86400) seconds )
  float deviation_ppm = (drift / period_sec * 1000000); //  deviation in parts per million (μs)
  float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours

  // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)

  int offset = round(deviation_ppm / drift_unit);

  // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
  // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

  // Serial.print("Offset is "); Serial.println(offset); // Print to control offset
}

void setup(void) {
  setup_tft();
  setup_sensors();
}

void loop() {
  
  DateTime now = rtc.now();

  if (pss != now.second()) 
  {
    pss = now.second();

    if (ohh != now.hour()) 
    {
      ohh = now.hour();
      draw_date();
    }

    if (omm != now.minute())
    {
      draw_enviro();
    }

    draw_time();
  }
}

