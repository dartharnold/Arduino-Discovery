// A basic everyday NeoPixel strip test program.

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define STRIP0_PIN 3
//#define STRIP1_PIN 9

// How many NeoPixels are attached to the Arduino?
#define STRIP0_LED_COUNT 2
//#define STRIP1_LED_COUNT 3

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip0(STRIP0_LED_COUNT, STRIP0_PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel strip1(STRIP1_LED_COUNT, STRIP1_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// Define Colors to use
const uint32_t off     = strip0.Color(0,0,0);
const uint32_t blue    = strip0.Color(0,0,255);
const uint32_t green   = strip0.Color(0,255,0);
const uint32_t aqua    = strip0.Color(0,255,255);
const uint32_t red     = strip0.Color(255,0,0);
const uint32_t fuchsia = strip0.Color(255,0,255);
const uint32_t yellow  = strip0.Color(255,255,0);
const uint32_t white   = strip0.Color(255,255,255);


// setup() function -- runs once at startup --------------------------------

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip0.begin();           // INITIALIZE NeoPixel strip0 object (REQUIRED)
  //strip1.begin();           // INITIALIZE NeoPixel strip1 object (REQUIRED)
  strip0.show();            // Turn OFF all pixels ASAP
  //strip1.show();            // Turn OFF all pixels ASAP
  strip0.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)
  //strip1.setBrightness(25); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip0.setPixelColor(0, white);   // fuchsia
  strip0.setPixelColor(1, white);       // red
  strip0.setPixelColor(2, white);       // red
  //strip1.setPixelColor(0, white);      // blue
  //strip1.setPixelColor(1, white);    // yellow
  //strip1.setPixelColor(2, white);     // green
  strip0.show();
  //strip1.show();
}


// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  // Pulse all Pixels 
  colorPulse(25);
}

void colorPulse(int rate) {
    for(int u=25; u<151; u++) {
        strip0.setBrightness(u);
        //strip1.setBrightness(u);
        strip0.show();
        //strip1.show();
        delay(rate);
    }
    delay(150);
    for(int d=150; d>25; d--) {
        strip0.setBrightness(d);
       //strip1.setBrightness(d);
        strip0.show();
        //strip1.show();
        delay(rate);
    }
    delay(200);
}
