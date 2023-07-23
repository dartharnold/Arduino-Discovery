#include <TFT_eSPI.h>
#include <SPI.h>
#include <DHT11.h>

#define DEFAULT_TEXT_SIZE                   2                       // a generic size used throughout 
#define ABOUT_TEXT_SIZE                    DEFAULT_TEXT_SIZE       // title is ABOUT_TEXT_SIZE+1 size, all other text is ABOUT_TEXT_SIZE
#define ABOUT_TITLE_COLOR                  TFT_RED
#define ABOUT_SUBTITLE_COLOR               TFT_ORANGE
#define ABOUT_TEXT_COLOR                   TFT_LIGHTGREY
#define ABOUT_VERSION_COLOR                C565(64,64,64)          // TFT_VERYDARKGREY
#define C565(r,g,b)                         ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)    // macro to convert RGB values to TFT_eSPI color value

#define MSG_VERSION                         "v0.61.01"                 // the version displayed on the about screen at the lower right


// Create an instance of the DHT11 class and set the digital I/O pin.
DHT11 dht11(13);
// Declare variables to hold Temprature and Humidity
float humidity, temp_c, temp_f;

//const char msg_version[]                = MSG_VERSION;
const char msg_title[]                  = "Droid Toolbox";
const char msg_email[]                  = "ruthsarian@gmail.com";
const char msg_continue1[]              = "press any button";
const char msg_continue2[]              = "to continue...";
const char msg_version[]                = MSG_VERSION;


TFT_eSPI tft = TFT_eSPI();      // display interface
bool tft_update = true;         // flag to inidcate display needs to be updated

void display_about() {
  uint16_t y = 0;

  // surely there's an easier way to vertically position the about screen text
  tft.setTextSize(1);
  y = (tft.height() - ((tft.fontHeight() * (ABOUT_TEXT_SIZE + 1)) + (tft.fontHeight() * ABOUT_TEXT_SIZE * 4))) / 2;
  tft.setCursor(0, y);

  tft.setTextDatum(TC_DATUM);

  // title
  tft.setTextSize(ABOUT_TEXT_SIZE + 1);
  tft.setTextColor(ABOUT_TITLE_COLOR);
  tft.drawString(msg_title, tft.width()/2, y);
  y += tft.fontHeight();

  // contact
  tft.setTextSize(ABOUT_TEXT_SIZE);
  tft.setTextColor(ABOUT_SUBTITLE_COLOR);
  tft.drawString(msg_email, tft.width()/2, y);
  y += (tft.fontHeight() * 2);

  // press any button...
  tft.setTextColor(ABOUT_TEXT_COLOR);
  tft.drawString(msg_continue2, tft.width()/2, y);
  y += tft.fontHeight();
  tft.drawString(msg_continue1, tft.width()/2, y);

  // version
  tft.setTextColor(ABOUT_VERSION_COLOR);
  tft.setTextDatum(BR_DATUM);
  tft.drawString(msg_version, tft.width(), tft.height());
}

void get_hum_temp()
{
    // Read the humidity from the sensor.
    humidity = dht11.readHumidity();

    // Read the temperature from the sensor.
    temp_c = dht11.readTemperature();

    // Calculate temperature from C
    temp_f = ((temp_c*9)/5)+32;
}

void reset_screen(void) {
  tft.resetViewport();
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setCursor(0, 0);
}

void display_clock() {
  uint16_t y = 0;

  // surely there's an easier way to vertically position the about screen text
  tft.setTextSize(1);
  y = (tft.height() - ((tft.fontHeight() * (ABOUT_TEXT_SIZE + 1)) + (tft.fontHeight() * ABOUT_TEXT_SIZE * 4))) / 2;
  tft.setCursor(0, y);

  tft.setTextDatum(TC_DATUM);

  // title
  tft.setTextSize(ABOUT_TEXT_SIZE + 1);
  tft.setTextColor(ABOUT_TITLE_COLOR);
  tft.drawString("10:15 PM", tft.width()/2, y);
  y += tft.fontHeight();

  // contact
  // tft.setTextSize(ABOUT_TEXT_SIZE);
  // tft.setTextColor(ABOUT_SUBTITLE_COLOR);
  // String stemp_f = String(temp_f);
  // tft.drawString(stemp_f, tft.width()/2, y);
  // y += (tft.fontHeight() * 2);

  // press any button...
  tft.setTextColor(ABOUT_TEXT_COLOR);
  tft.drawString(msg_continue2, tft.width()/2, y);
  y += tft.fontHeight();
  tft.drawString(msg_continue1, tft.width()/2, y);

  // version
  // tft.setTextColor(ABOUT_VERSION_COLOR);
  // tft.setTextDatum(BR_DATUM);
  // tft.drawString(msg_version, tft.width(), tft.height());
}

void setup() {
  // put your setup code here, to run once:
// setup display
  tft.init();
  tft.setRotation(3);
  reset_screen();
}

void loop() {
  // put your main code here, to run repeatedly:
 
  // display_about();
  // delay(1000);
  
  reset_screen();
  
  //get_hum_temp();
  display_clock();
  delay(2000);
}
