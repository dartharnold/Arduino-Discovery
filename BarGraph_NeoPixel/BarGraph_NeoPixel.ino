#include <Adafruit_NeoPixel.h>

// NeoPixel declarations
// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIN 2

#define PIXELCOUNT 32  // How many NeoPixels are attached to the Arduino?
#define NEOBRIGHT 15
#define PIXELFORMAT NEO_GRB + NEO_KHZ800  // NeoPixel color format & data rate. See the strandtest example for information on possible values.

uint32_t RED, GREEN, BLUE, CYAN, PURPLE, YELLOW, ORANGE, WHITE, BLACK;

// Rather than declaring the whole NeoPixel object here, we just create
// a pointer for one, which we'll then allocate later...
Adafruit_NeoPixel* pixels;

long barHeight[] = {0,0,0,0,0,0,0,0};
long barColor[] = {0,0,0,0,0,0,0,0}; 
uint32_t colors[] = {pixels->Color(0, 255, 255),pixels->Color(255, 255, 0),pixels->Color(255, 130, 0),pixels->Color(255, 69, 0),pixels->Color(255, 0, 0),pixels->Color(0, 0, 255),pixels->Color(255, 255, 255),pixels->Color(0, 255, 0)};

void setup() {
   // Create a new NeoPixel object dynamically with these values:
  pixels = new Adafruit_NeoPixel(PIXELCOUNT, NEOPIN, PIXELFORMAT);

  // Setup some Basic Colors to work with
  RED = pixels->Color(255, 0, 0);
  GREEN = pixels->Color(0, 255, 0);
  BLUE = pixels->Color(0, 0, 255);
  CYAN = pixels->Color(0, 255, 255);
  PURPLE = pixels->Color(255, 0, 255);
  YELLOW = pixels->Color(255, 191, 0);
  ORANGE = pixels->Color(255, 69, 0);
  WHITE = pixels->Color(255, 255, 255);
  BLACK = pixels->Color(0, 0, 0);

  pixels->begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  randomSeed(analogRead(9));
}

void loop() {
  uint8_t pixel;

  pixels->clear();  // Set all pixel colors to 'off'
  pixels->show();

  for (int i = 0; i < 8; i++) {
    barHeight[i] = random(0,5);
    barColor[i] = random(0,7);
  }
  
  for (pixel = 0; pixel <= PIXELCOUNT; pixel++){
    if (pixel < 7) {
       if (barHeight[pixel] == 4) {
        pixels->setPixelColor(pixel, colors[barHeight[pixel]]);
      } else {
        pixels->setPixelColor(pixel, BLACK);
      }
    } else if ((pixel > 7) && (pixel < 16)) {
      if (barHeight[pixel-8] >= 3) {
        pixels->setPixelColor(pixel, colors[barHeight[pixel-8]]);
      } else {
        pixels->setPixelColor(pixel, BLACK);
      }
    } else if ((pixel > 15) && (pixel < 24)) {
      if (barHeight[pixel-16] >= 2) {
        pixels->setPixelColor(pixel, colors[barHeight[pixel-16]]);
      } else {
        pixels->setPixelColor(pixel, BLACK);
      }
    } else if ((pixel > 23) && (pixel < 32)) {
     if (barHeight[pixel-24] >= 1) {
        pixels->setPixelColor(pixel, colors[barHeight[pixel-24]]);
      } else {
        pixels->setPixelColor(pixel, BLACK);
      }
    }
  }

  pixels->setBrightness(NEOBRIGHT);
  pixels->show();  // Send the updated pixel colors to the hardware.

  delay(200);
}
