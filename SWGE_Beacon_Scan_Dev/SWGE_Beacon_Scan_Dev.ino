#include <bluefruit.h>

// Device Info
const char* DEVNAME = "NavaComp1";

// Counters and Timers
#define CHANGEDELY  60000

// Location IDs
#define MARKETPLACE 0x01
#define BEHINDDEPOT 0x02
#define RESISTANCE  0x03
#define UNKNOWN     0x04
#define DROIDDEPOT  0x05
#define DOKONDARS   0x06
#define FIRSTORDER  0x07

// Filters
#define RSSI        -80
#define BLE_DISNEY  0x0183
const String IGNOREHOST = "DROIDTLBX";

void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("SWGE_Beacon_Scan");
  Serial.println("----------------\n");

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  /* Set the device name */
  Bluefruit.setName(DEVNAME);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Filter out packet with a min rssi
   * - Interval = 100 ms, window = 50 ms
   * - Use active scan (used to retrieve the optional scan response adv packet)
   * - Start(0) = will scan forever since no timeout is given
   */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.filterRssi(RSSI);
  Bluefruit.Scanner.filterMSD(BLE_DISNEY);
  Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
  Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds

  Serial.println("Starting SWGE Location Scanning ...");
}

void scan_callback(ble_gap_evt_adv_report_t* report)
{
  PRINT_LOCATION();
  uint8_t len = 0;
  uint8_t beacon_name_buff[32];
  String beacon_name = "";
  uint8_t md_len = 0;
  uint8_t md_buffer[32];

  memset(beacon_name_buff, 0, sizeof(beacon_name_buff));
  memset(md_buffer, 0, sizeof(md_buffer));

  uint8_t mfg_size;
  uint8_t mfg_data[32];

  // Scan Values to store
  int8_t scan_rssi;
  uint16_t area_num;

/* Check for Manufacturer Specific Data is it a Location?*/

  md_len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, md_buffer, sizeof(md_buffer));
  if ((md_len) && (md_buffer[2] == 0x0A))
  {
    /* Complete Local Name */
    Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, beacon_name_buff, sizeof(beacon_name_buff));

    // Convert beacon_name_buff to string beacon_name
    for (int i = 0; i < sizeof(beacon_name); i++)
    {
      if (beacon_name_buff[i] > 0)
      {
        beacon_name.concat(char(beacon_name_buff[i]));
      }
    }

    if (beacon_name != IGNOREHOST)
    {
      // What is the location ID?
      area_num = md_buffer[4];

      /* RSSI value */
      scan_rssi = report->rssi;

      Serial.println();
      Serial.print("Beacon Host: ");
      Serial.println(beacon_name);
      Serial.print("RSSI: ");
      Serial.print(scan_rssi);
      Serial.println(" dBm");
      Serial.print("Area #: ");
      Serial.println(area_num);
      Serial.println("----------------\n");
    }
  }

  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();  
}

void loop() 
{
  // nothing to do
}
