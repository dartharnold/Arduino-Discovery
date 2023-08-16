#include <TFT_eSPI.h> 
#include "AurebeshAF.h" // include generated font

TFT_eSPI tft = TFT_eSPI();       
TFT_eSprite sprite = TFT_eSprite(&tft); 

// set font
tft.setFreeFont(&AurebeshAF);  // for TFT
sprite.setFreeFont(&AurebeshAF); // for specific sprite

// print static UTF8 string
tft.print("Test"); 
sprite.print("Test"); // for specific sprite

//print from string variable
String text="Test";
tft.print(text.c_str());
sprite.print(text.c_str());