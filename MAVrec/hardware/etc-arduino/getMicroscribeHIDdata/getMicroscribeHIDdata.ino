// getMicroscribeHIDdata:
//    modified from "HIDDeviceInfo - Simple HID device example" as described below
//    to extract motion data from a Revware Microscribe M 6dof from an teensy USB host
//    and to pipe that data over serial for display
// From Paul S.'s original desription in HIDDeviceInfo
// You can use Serial Input to control how much data is displayed per each HID packet
// received by the sketch.
//
// By Default it displays both the RAW (Hex dump) of the data received, as well
// as the data as the HID interpreter walks through the data into the individual
// fields, which we then print out.
//
// There are options to turn off some of this output, also an option that you can
// toggle on or off (C) to only try to show the changed fields.
//
// This example is in the public domain

#include <USBHost_t36.h>
#include "HIDDumper.h"
//#include "USBDeviceInfo.h"

USBHost myusb;
//USBHub hub1(myusb);
//USBHub hub2(myusb);
//USBDeviceInfo dinfo(myusb); // will never claim anything...
USBHIDParser hid1(myusb);
//USBHIDParser hid2(myusb);
//USBHIDParser hid3(myusb);
//USBHIDParser hid4(myusb);
//USBHIDParser hid5(myusb);

HIDDumpController hdc1(myusb);
//HIDDumpController hdc2(myusb);
//HIDDumpController hdc3(myusb);
//HIDDumpController hdc4(myusb);
//HIDDumpController hdc5(myusb);

//USBDriver *drivers[] = {&hub1, &hub2, &hid1, &hid2, &hid3, &hid4, &hid5};
USBDriver *drivers[] = { &hid1 };
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
//const char * driver_names[CNT_DEVICES] = { "HID1-XXXXXXX" , "HID2", "HID3", "HID4", "HID5"};
const char * driver_names[CNT_DEVICES] = { "HID1-XXXXXXX"};
bool driver_active[CNT_DEVICES] = {false};

// Lets also look at HID Input devices
//USBHIDInput *hiddrivers[] = {&hdc1, &hdc2,  &hdc3, &hdc4, &hdc5};
USBHIDInput *hiddrivers[] = {&hdc1};
#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
//const char * hid_driver_names[CNT_DEVICES] = {"hdc1-xxxxxxxxx", "hdc2", "hdc3", "hdc4", "hdc5"};
const char * hid_driver_names[CNT_DEVICES] = {"hdc1-xxxxxxxxx"};
bool hid_driver_active[CNT_DEVICES] = {false};
bool show_changed_only = false;
void setup()
{
  Serial1.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  //  Serial.println("\n\nMicroscribe data dumper program");
  //  Serial.println("\n*** You can control the output by simple character input to Serial ***");
  //  Serial.println("R - Turns on or off showing the raw data");
  //  Serial.println("C - Toggles showing changed data only on or off");
  //  Serial.println("<anything else> - toggles showing the Hid formatted breakdown of the data\n");

  myusb.begin();
}


void loop()
{
  myusb.Task();

  HIDDumpController::show_raw_data = false;
  HIDDumpController::show_formated_data = true;
  HIDDumpController::changed_data_only = false;

  if (Serial.available()) {
    int ch = Serial.read(); // get the first char.
    while (Serial.read() != -1) ;
    if (ch == 'b' || (ch == 'B')) {        // BEGIN data streaming


      Serial.println("%% STATUS: Starting new recording ...");

      for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) 
      {
        if (*hiddrivers[i] != hid_driver_active[i]) {
          if (hid_driver_active[i]) {
            Serial.printf("\r\n%% STATUS: *** HID Device % s - disconnected ***\n", hid_driver_names[i]);
            hid_driver_active[i] = false;
          } else {
            Serial.printf("%% STATUS: *** HID Device % s % x: % x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
            hid_driver_active[i] = true;

            const uint8_t *psz = hiddrivers[i]->manufacturer();
            if (psz && *psz) Serial.printf("%%  manufacturer: % s\n", psz);
            psz = hiddrivers[i]->product();
            if (psz && *psz) Serial.printf("%%  product: % s\n", psz);
            psz = hiddrivers[i]->serialNumber();
            if (psz && *psz) Serial.printf("%%  Serial: % s\n", psz);
          }
        }
      }
      HIDDumpController::show_formated_data = true;

    } else if (ch == 'X' || (ch == 'x')) 
    { // STOP/KILL  data streaming
      Serial.println("%% STATUS: ending data stream");
      HIDDumpController::show_formated_data = false;
    }

  }

}
