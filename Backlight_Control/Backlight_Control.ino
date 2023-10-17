#include"TFT_eSPI.h"
TFT_eSPI tft;
#define LCD_BACKLIGHT (38) // Control Pin of LCD

void setup() {
  // put your setup code here, to run once:
  
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_RED);

    delay(2000);
    // Turning off the LCD backlight
    digitalWrite(LCD_BACKLIGHT, LOW);
    delay(2000);
    // Turning on the LCD backlight
    digitalWrite(LCD_BACKLIGHT, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

}