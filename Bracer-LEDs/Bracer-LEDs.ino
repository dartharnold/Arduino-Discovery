// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library
// This sketch shows use of the "new" operator with Adafruit_NeoPixel.
// It's helpful if you don't know NeoPixel settings at compile time or
// just want to store this settings in EEPROM or a file on an SD card.

#include <Adafruit_NeoPixel.h>

// NeoPixel declarations
// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIN       7 
#define PIXELCOUNT   2  // How many NeoPixels are attached to the Arduino?
#define PIXELFORMAT  NEO_GRB + NEO_KHZ800  // NeoPixel color format & data rate. See the strandtest example for information on possible values.

// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
Adafruit_NeoPixel *pixels;

void setup() {
 
  // Create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(PIXELCOUNT, NEOPIN, PIXELFORMAT);

  pixels->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void updatePixels() {
  uint32_t DelayOff = random(200,500);
  uint32_t DelayOn = random(200,800);
  uint8_t pixel;

  pixels->clear(); // Set all pixel colors to 'off'
  pixels->show();

  delay(DelayOff);  // Pause Pixel Off for this pass through loop
  
  // Setup to turn Pixels On
  for (pixel = 0; pixel <= PIXELCOUNT; pixel ++) {
    pixels->setPixelColor(pixel, pixels->Color(255, 0, 0));
  }
  pixels->setBrightness(75);
  pixels->show();   // Send the updated pixel colors to the hardware.
  delay(DelayOn); // Pause Pixel On before next pass through loop
}

void loop() {
  updatePixels();
  printf("loop");
}
