// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <ESP32Time.h>

RTC_PCF8523 etc;
ESP32Time rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
    Serial.begin(115200);
    Wire.begin(43, 44);

    #ifndef ESP8266
        while (!Serial); // wait for serial port to connect. Needed for native USB
    #endif

    if (! etc.begin()) {
        Serial.println("Couldn't find eTC");
        Serial.flush();
        while (1) delay(10);
    }

    // if (! etc.initialized() || etc.lostPower()) {

    //   Serial.println("RTC is NOT initialized, let's set the time!");

        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled

      etc.adjust(DateTime(F(__DATE__), F(__TIME__)));

        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:

        // etc.adjust(DateTime(2022, 3, 2, 11, 33, 30));

        // Note: allow 2 seconds after inserting battery or applying external power
        // without battery before calling adjust(). This gives the PCF8523's
        // crystal oscillator time to stabilize. If you call adjust() very quickly
        // after the RTC is powered, lostPower() may still return true.

    // }

    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // etc.adjust(DateTime(2022, 3, 7, 10, 26, 00));
    // When the RTC was stopped and stays connected to the battery, it has
    // to be restarted by clearing the STOP bit. Let's do this to ensure
    // the RTC is running.

    etc.start();

    // The PCF8523 can be calibrated for:
    // - Aging adjustment
    // - Temperature compensation
    // - Accuracy tuning
    // The offset mode to use, once every two hours or once every minute.
    // The offset Offset value from -64 to +63. See the Application Note for calculation of offset values.
    // https://www.nxp.com/docs/en/application-note/AN11247.pdf
    // The deviation in parts per million can be calculated over a period of observation. Both the drift (which can be negative)
    // and the observation period must be in seconds. For accuracy the variation should be observed over about 1 week.
    // Note: any previous calibration should cancelled prior to any new observation period.
    // Example - RTC gaining 43 seconds in 1 week

    float drift = 43; // seconds plus or minus over oservation period - set to 0 to cancel previous calibration.
    float period_sec = (7 * 86400); // total obsevation period in seconds (86400 = seconds in 1 day: 7 days = (7 * 86400) seconds )
    float deviation_ppm = (drift / period_sec * 1000000); // deviation in parts per million (μs)
    float drift_unit = 4.34; // use with offset mode PCF8523_TwoHours

    // float drift_unit = 4.069; //For corrections every min the drift_unit is 4.069 ppm (use with offset mode PCF8523_OneMinute)

    int offset = round(deviation_ppm / drift_unit);

    // rtc.calibrate(PCF8523_TwoHours, offset); // Un-comment to perform calibration once drift (seconds) and observation period (seconds) are correct
    // rtc.calibrate(PCF8523_TwoHours, 0); // Un-comment to cancel previous calibration

    // Serial.print("Offset is "); Serial.println(offset); // Print to control offset
    rtc.setTime(30, 24, 15, 17, 1, 2021); // 17th Jan 2021 15:24:30
}

void loop () {

    DateTime now = etc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    if (etc.now () != now){
        etc.now () = DateTime (); // 17th Jan 2021 15:24:30
        Serial.println(rtc.getTime("Internal time, %A, %B %d %Y %H:%M:%S")); // (String) returns time with specified format
        Serial.flush();
        esp_deep_sleep_start();
        Serial.println("This will never be printed");
    }

    // formating options http://www.cplusplus.com/reference/ctime/strftime/
    //if(bootCheck != bootCount)
    //{
    // rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), (now.year())); // 17th Jan 2021 15:24:30
    // Serial.println(rtc.getTime("Internal time, %A, %B %d %Y %H:%M:%S")); // (String) returns time with specified format
    // bootCheck++;
    //}

    delay(3000);

    struct tm timeinfo = rtc.getTimeStruct();

    //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); // (tm struct) Sunday, January 17 2021 07:24:38
}