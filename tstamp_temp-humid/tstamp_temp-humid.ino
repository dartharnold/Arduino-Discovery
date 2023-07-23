#include <DHT11.h>
#include "Wire.h"
#include "RTClib.h"

RTC_PCF8523 rtc;

#define TEMPF_OFFSET   -1.3
#define HUMID_OFFSET  -2

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Create an instance of the DHT11 class and set the digital I/O pin.
DHT11 dht11(13);
// Declare variables to hold Temprature and Humidity
float humidity, temp_c, temp_f;

void get_hum_temp()
{
    // Read the humidity from the sensor.
    humidity = dht11.readHumidity() + HUMID_OFFSET;

    // Read the temperature from the sensor.
    temp_c = dht11.readTemperature();

    // Calculate temperature from C
    temp_f = (((temp_c*9)/5)+32) + TEMPF_OFFSET;
}

void setup()
{
  //Setup SDA, SCL for RTC
  Wire.begin(43, 44);
  
  //Setup for Serial Console
  Serial.begin(57600);
  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  // Is the RTC Present?
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // When the RTC was stopped and stays connected to the battery, it has 
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();

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

void loop()
{
    // Read Date and Time 
    DateTime now = rtc.now();
    // Read Temperature and Humidity
    get_hum_temp();   
    
    // If the temperature and humidity readings were successful, print them to the serial monitor.
    if (temp_c != -1 && humidity != -1)
    {
      Serial.println("-------------");
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print('/');
      Serial.print(now.year(), DEC);
      Serial.print(" (");
      Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print(") ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      if (now.minute() < 10) {
        Serial.print("0");
      }
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      if (now.second() < 10) {
        Serial.print("0");
      }
      Serial.print(now.second(), DEC);
      Serial.print(" - Temperature: ");
      Serial.print(temp_f, 1);
      Serial.print("° F with ");
      Serial.print(humidity,0);
      Serial.print("% Humidity");
      Serial.println();
    }
    else
    {
        // If the temperature or humidity reading failed, print an error message.
        Serial.println("Error reading data");
    }

    // Wait for 2 seconds before the next reading.
    delay(30000);
}
