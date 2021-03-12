/* USB EHCI Host for Teensy 3.6
   Copyright 2017 Paul Stoffregen (paul@pjrc.com)

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, shiublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "HidDumper.h"
bool HIDDumpController::show_raw_data = true;
bool HIDDumpController::show_formated_data = true;
bool HIDDumpController::changed_data_only = false;

void HIDDumpController::init()
{
  USBHost::contribute_Transfers(mytransfers, sizeof(mytransfers) / sizeof(Transfer_t));
  USBHIDParser::driver_ready_for_hid_collection(this);
}

hidclaim_t HIDDumpController::claim_collection(USBHIDParser *driver, Device_t *dev, uint32_t topusage)
{
  // only claim RAWHID devices currently: 16c0:0486
  //TMK Serial.printf("%% STATUS: HIDDumpController Claim: %x:%x usage: %x", dev->idVendor, dev->idProduct, topusage); //tmk
  if (mydevice != NULL && dev != mydevice) {
    //TMK Serial.println("- NO (Device)"); TMK
    return CLAIM_NO;
  }
  if (usage_ && (usage_ != topusage)) {
    //TMK Serial.printf(" - NO (Usage: %x)\n");
    return CLAIM_NO;      // Only claim one
  }
  mydevice = dev;
  collections_claimed++;
  usage_ = topusage;
  driver_ = driver; // remember the driver.
  //TMK Serial.println(" - Yes");
  return CLAIM_INTERFACE;  // We want
}

void HIDDumpController::disconnect_collection(Device_t *dev)
{
  if (--collections_claimed == 0) {
    mydevice = NULL;
    usage_ = 0;
  }
}

void dump_hexbytes(const void *ptr, uint32_t len)
{
  if (ptr == NULL || len == 0) return;
  uint32_t count = 0;
//  if (len > 64) len = 64; // don't go off deep end...
  const uint8_t *p = (const uint8_t *)ptr;
  while (len--) {
    if (*p < 16) Serial.print('0');
    Serial.print(*p++, HEX);
    count++;
    if (((count & 0x1f) == 0) && len) Serial.print("\n");
    else Serial.print(' ');
  } 
  Serial.println();
}

bool HIDDumpController::hid_process_in_data(const Transfer_t *transfer)
{
  // return true if we are not showing formated data...
  hid_input_begin_level_ = 0; // always make sure we reset to 0
  count_usages_ = index_usages_;  // remember how many we output for this one
  index_usages_ = 0;        // reset the index back to zero

  //Serial.printf("HID(%x)", usage_); //tmk
  if (show_raw_data) {
    Serial.print(": ");
    dump_hexbytes(transfer->buffer, transfer->length);
  }
  else Serial.println();

  return !show_formated_data;
}

bool HIDDumpController::hid_process_out_data(const Transfer_t *transfer)
{
  Serial.printf("HIDDumpController::hid_process_out_data: %x\n", usage_);
  return true;
}

void indent_level(int level) {
  //TMK if ((level > 5) || (level < 0)) return; // bail if something is off...
  //TMK while (level--) Serial.print(" ");
}

void HIDDumpController::hid_input_begin(uint32_t topusage, uint32_t type, int lgmin, int lgmax)
{
  // Lets do simplified data for changed only
  if (changed_data_only) return;

  //TMK indent_level(hid_input_begin_level_);
  //Serial.printf("Begin topusage:%x type:%x min:%d max:%d\n", topusage, type, lgmin, lgmax); //tmk
  if (hid_input_begin_level_ < 2)
    hid_input_begin_level_++;
}

void HIDDumpController::hid_input_data(uint32_t usage, int32_t value)
{

  bool output_data = !changed_data_only;

  // See if something changed.
  if (index_usages_ < count_usages_) {
    if ((usage != usages_[index_usages_]) || (value != values_[index_usages_])) {
      output_data = true;
    }
  } else {
    output_data = true;
  }
  if (index_usages_ < MAX_CHANGE_TRACKED) {
    usages_[index_usages_] = usage;
    values_[index_usages_] = value;
    index_usages_++;
  }

  if (output_data) {
    //TMK indent_level(hid_input_begin_level_);
    //TMK Serial.printf("usage=%X, value=%d", usage, value); //tmk
    //TMK Serial.printf("\t%d" ,  value);
    //TMK if ((value >= ' ') && (value <= '~')) Serial.printf(":%c", value);

    // maybe print out some information about some of the Usage numbers that we know about
    // The information comes from the USB document, HID Usage Tables 
    // https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    switch( usage ) {
     
      case 0xA0001: // Theta 0
      case 0xA0002: // Theta 1
      case 0xA0003: // Theta 2
      case 0xA0004: // Theta 3
      case 0xA0005: // Theta 4
      case 0xA0000: // Theta 5 and Theta 6(if 7dof present)
      
          Serial.printf("%d,  " ,  value);
          break;
       
      case 0xFF0702: // timestamp in ms, 2^16 rollover
          Serial.printf("%d,  " ,  value);      
          break;
      default:
          break;
    
    }
    
   
    //Serial.println(); //timk tmk
  }
}

void HIDDumpController::hid_input_end()
{
  // Lets do simplified data for changed only
  if (changed_data_only) return;
  hid_input_begin_level_--;
  //TMK indent_level(hid_input_begin_level_);
  //Serial.println("END:");  //tmk
}
