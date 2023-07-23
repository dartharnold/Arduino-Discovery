#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// TFT Display Pins
#define TFT_CS      19
#define TFT_RST     21
#define TFT_DC      22
#define TFT_MOSI    23
#define TFT_MISO    25
#define TFT_CLK     26

#define TFT_X_OFFSET  3
#define TFT_Y_OFFSET  0

// NeoPixel Values
#define PIXEL_PIN   5
#define NUMPIXELS   5

// Buzzer
#define BUZZER_PIN  18

// BLE Values
#define BLE_SCAN_TIME 5

// MISC values
#define MAX_DROIDS 5

typedef struct droid_t {
    uint8_t chipid;
    uint8_t affid;
    char addr[20];
} Droid;

// create global objects
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
BLEScan* pBLEScan;

BLEAdvertising* pAdvertising;
BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

//uint8_t payload[] = {0x83,0x01,0x03,0x04,0x44,0x81,0x82,0x01};  // Unchipped R2
uint8_t payload[] = {0x83,0x01,0x0a,0x04,0x01,0x02,0xA6,0x01};  // Dok-Ondar's
uint8_t payload_size = (sizeof(payload)/sizeof(uint8_t));

Droid droids[MAX_DROIDS];
uint8_t droid_count = 0;

// BLE Callback
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      uint8_t *rawdata, rawdata_len;
      uint16_t mfid;

      // DROID CHECK ONE 
      //   - we haven't maxed our droid cound
      //   - object has the name "DROID"
      //   - object has manfuacturer data
      if (droid_count < MAX_DROIDS && advertisedDevice.getName() == "DROID" && advertisedDevice.haveManufacturerData()) {

        // exract manufacturer's id from data
        rawdata = (uint8_t*)advertisedDevice.getManufacturerData().data();
        rawdata_len = advertisedDevice.getManufacturerData().length();
        mfid = rawdata[0] + (rawdata[1]<<8);

        // DROID CHECK TWO
        //   - is manufacturer id 0x0183 (Disney)
        if (rawdata_len == 8 && mfid == 0x0183) {

          // store found droid's information
          droids[droid_count].chipid = rawdata[rawdata_len-1];
          droids[droid_count].affid = (rawdata[rawdata_len-2] - 0x80) / 2;
          strncpy(droids[droid_count].addr, advertisedDevice.getAddress().toString().c_str(), 20);

          // increment counter
          droid_count++;
        }
      }
    }
};

void setup() {
  // init serial monitor
  Serial.begin(115200);
  Serial.println("Scanning...");

  // init the LCD screen
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  // prep the buzzer pin
//  pinMode(BUZZER_PIN, OUTPUT);

  // BLE initialization
  BLEDevice::init("");

  // BLE Scanning
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // BLE Advertising
  oAdvertisementData.setManufacturerData(std::string(reinterpret_cast<char*>(payload), payload_size));
  oAdvertisementData.setName("KevinBeacon");
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);

  // start advertising
  //pAdvertising->start();
}

void loop() {
  uint8_t i = 0;

  // init the screen
//  tft.begin();
//  tft.setRotation(3);
//  tft.fillScreen(ILI9341_BLACK);
  
  // bluetooth scan
  screen_scanning();
  ble_scan();

  // report findings
  serial_droid_report();
  screen_droid_report();

  // delay until next scan; advertise beacon until then
  pAdvertising->start();
  delay(10000);
  pAdvertising->stop();
}

void ble_scan() {
  // get ready to count some droids
  droid_count = 0;

  // BLE scan
  BLEScanResults foundDevices = pBLEScan->start(BLE_SCAN_TIME, false);

  // Report results via Serial
  Serial.print(foundDevices.getCount());
  Serial.println(" BLE devices seen.");

  // delete results fromBLEScan buffer to release memory
  pBLEScan->clearResults();
}

void serial_droid_report() {
  uint8_t i;

  if (droid_count > 0) {
    Serial.print("--- ");
    Serial.print(droid_count);
    Serial.print(" droid");
    if (droid_count > 1) {
      Serial.print("s");
    }
    Serial.println(" found! ---");
  }

  for(i=0;i<droid_count;i++) {
    Serial.print(droids[i].addr);
    Serial.print(" ");
    switch(droids[i].chipid) {
      case 1:
        Serial.print("Unchipped R2");
        break;
      case 2:
        Serial.print("Unchipped BB");
        break;
      case 3:
        Serial.print("Blue");
        break;
      case 4:
        Serial.print("Gray");
        break;
      case 5:
        Serial.print("Red");
        break;
      case 6:
        Serial.print("Orange");
        break;
      case 7:
        Serial.print("Purple");
        break;
      case 8:
        Serial.print("Black");
        break;
      default: 
        Serial.print("unknown personality chip (");
        Serial.print(droids[i].chipid);
        Serial.print(")");
        break;
    }
    Serial.print(" ");
    switch(droids[i].affid) {
      case 1:
        Serial.print("Scoundrel");
        break;
      case 3:
        Serial.print("Ruthsarian");
        break;
      case 5:
        Serial.print("Resistance");
        break;
      case 9:
        Serial.print("First Order");
        break;
      default: 
        Serial.print("unknown affiliation (");
        Serial.print(droids[i].affid);
        Serial.print(")");
        break;    
    }
    Serial.println();
    Serial.println();
  }
}

void screen_scanning() {

  String msg = "SCANNING";
  uint8_t font_size = 6;

  // char width = textsize * 6
  // char height = textsize * 8

  // blank the screen
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setCursor((tft.width()/2)-((msg.length()*(font_size * 6))/2), (tft.height()/2)-((font_size*8)/2));
  tft.setTextSize(font_size);
  tft.setTextColor(ILI9341_GREEN);
  tft.println(msg);
}

void screen_droid_report() {
  uint8_t i;

  // blank screen
  tft.fillScreen(ILI9341_BLACK);

  // write title
  tft.setCursor(TFT_X_OFFSET, TFT_Y_OFFSET);
  tft.setTextSize(4);
  tft.setTextColor(ILI9341_RED);
  tft.println("DROID REPORT");

  // set font size and position
  tft.setCursor(TFT_X_OFFSET, tft.getCursorY()+5);
  tft.setTextSize(2);

  if (droid_count > 0) {
    for(i=0; i<droid_count; i++) {

      // print droid BLE address
      tft.setTextColor(ILI9341_BLUE);
      tft.println(droids[i].addr);

      // set font for outputing properties of chip
      tft.setCursor(TFT_X_OFFSET, tft.getCursorY());
      tft.setTextColor(ILI9341_GREEN);
      tft.print("  ");

      switch(droids[i].affid) {
        case 1:
          tft.print("Scoundrel");
          break;
        case 3:
          tft.print("Ruthsarian");
          break;
        case 5:
          tft.print("Resistance");
          break;
        case 9:
          tft.print("First Order");
          break;
        default: 
          tft.print("Unknown (");
          tft.print(droids[i].affid);
          tft.print(")");
          break;    
      }
      tft.print("  ");
      switch(droids[i].chipid) {
        case 1:
          tft.print("R Unit");
          break;
        case 2:
          tft.print("BB Unit");
          break;
        case 3:
          tft.print("Blue");
          break;
        case 4:
          tft.print("Gray");
          break;
        case 5:
          tft.print("Red");
          break;
        case 6:
          tft.print("Orange");
          break;
        case 7:
          tft.print("Purple");
          break;
        case 8:
          tft.print("Black");
          break;
        default: 
          tft.print("Unknown (");
          tft.print(droids[i].chipid);
          tft.print(")");
          break;
      }
      tft.println();
      tft.setCursor(TFT_X_OFFSET, tft.getCursorY()+5);

    }
  } else {
    tft.setTextColor(ILI9341_YELLOW);
    tft.println("No Droids In Area");
  }
}

void screen_hello() {

  // blank the screen
  tft.fillScreen(ILI9341_BLACK);

  // position the cursor
  tft.setCursor(10, 10);

  // set text size and color
  tft.setTextSize(4);
  tft.setTextColor(ILI9341_WHITE);

  // write text to the screen
  tft.println("Hello World!");
}








/***** NOTES ***********
 
 
 
  Advertised Device: Name: DROID, Address: c2:b1:05:2b:1f:91, manufacturer data: 8301030444818202  

  company id = 0x0183
  data = 0x030444818202
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
*/
