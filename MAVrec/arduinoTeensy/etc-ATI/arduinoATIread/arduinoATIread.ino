/* UART Example, any character received on either the real
   serial port, or USB serial (or emulated serial to the
   Arduino Serial Monitor when using non-serial USB types)
   is printed as a message to both ports.

   This example code is in the public domain.
*/

// set this to the hardware serial port you wish to use
#define HWSERIAL    Serial1
#define ATI_BAUD    1250000
#define TIMEOUT_US  200000  // after ~100 ms, stop waiting to hear from ATI
#define ATI_CALIBRATION_ADDR_HI 0x00  // address of calibration # to load from netcanoem's 
#define ATI_CALIBRATION_ADDR_LO 0xe3  // internal list (should match sensor Part #)                                      
                                      // see pg 2 of Digital FT Modbus Interface.pdf doc
#include "atiFunctions.h"


// this function will handle all PC-issued serial commands like:
// 's' - get status
// 'b' - begin streaming
// 'x' - stop streaming
void handleSerialInput ( unsigned char incomingByte );


// Create an IntervalTimer object 
IntervalTimer sampleTimer;
unsigned int samplePeriod = 10000;  // in us, e.g. 10ms sampling period is 10000us
volatile  unsigned long t_ms, t0_ms;    // bookeeping in ms
unsigned long tPrev_ms;
volatile streamingSample currentSample; // updated more slowly than latestSample
void timerISR() {
  t_ms = millis() - t0_ms;
  currentSample.t_us = gLatestSample.t_us;
  currentSample.G0 = gLatestSample.G0;
  currentSample.G1 = gLatestSample.G1;
  currentSample.G2 = gLatestSample.G2;
  currentSample.G3 = gLatestSample.G3;
  currentSample.G4 = gLatestSample.G4;
  currentSample.G5 = gLatestSample.G5;
  currentSample.checksumError = gLatestSample.checksumError;
  currentSample.statusError = gLatestSample.statusError;
}

//////////////////////////////////////////////////////////////////////////////////
///////////////////////    SETUP    //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin( 115200 );
  HWSERIAL.begin( ATI_BAUD, SERIAL_8E1 ); // default Baud reate of netcanoem is 1.25Mbps
  t0_ms = millis();
  sampleTimer.begin( timerISR, samplePeriod );
  
  gATIstatus = gATIstatus*0 - 1;  // Initialize bitmap to all values high (all errors) via rollover
  gATIsensorIsConnected = false;
  gATIisStreaming = false; 
  // Make jamming command obvious and uniqe, (binary square wave)
  for ( int i = 0; i <cmdLengthJam; i++)
    atiJammingCommand[i] =  B10101010;

  
   /*// Check for big or little endian-ness on teensy (teensy is little endian, ati is big endian)
   union {
      short s;
      char c[sizeof(short)];
   }un;  
   un.s = 0x0102;   
   if (sizeof(short) == 2) {
      if (un.c[0] == 1 && un.c[1] == 2)
         Serial.print("big-endian\n");      
      else if (un.c[0] == 2 && un.c[1] == 1)
         Serial.print("little-endian\n");      
      else
         Serial.print("unknown\n");
   }*/

   Serial.print("size of: "); Serial.println( sizeof(short) );
   //ATIstartStreaming() ;
   t0_ms = millis();
   tPrev_ms = t_ms;

}






//////////////////////////////////////////////////////////////////////////////////
///////////////////////    LOOP     //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void loop() {

 
  // Handle Serial Commands from PC
  if (Serial.available() > 0)      
    handleSerialInput ( Serial.read() ) ;


  // Update globals with streaming data (if any) as fast as possible
  if ( gATIisStreaming && HWSERIAL.available() > 0) {
  
    // get a high res time stamp in us
    gLatestSample.t_us = micros()-t0; 

    // wait for the remaining 13 bytes to come in for this one packet
    while ( HWSERIAL.available() < 13 ) 
      {/* do nothing */}
    
    for ( int i = 0; i < 13; i++) {
       inBuffer[i] = HWSERIAL.read();
       //Serial.print( (int)inBuffer[i], DEC ); Serial.print("\t");
    }
    
    // Read MSB first ...  (this should work, big endian)
    gLatestSample.G0 = (static_cast<signed short>(inBuffer[ 0]) << 8) + inBuffer[ 1]; // note these are read in a 
    gLatestSample.G2 = (static_cast<signed short>(inBuffer[ 2]) << 8) + inBuffer[ 3]; // very specific order to 
    gLatestSample.G4 = (static_cast<signed short>(inBuffer[ 4]) << 8) + inBuffer[ 5]; // minimize cross talk
    gLatestSample.G1 = (static_cast<signed short>(inBuffer[ 6]) << 8) + inBuffer[ 7]; // G0, 2, 4, 1, 3, 5;
    gLatestSample.G3 = (static_cast<signed short>(inBuffer[ 8]) << 8) + inBuffer[ 9]; // each is int16
    gLatestSample.G5 = (static_cast<signed short>(inBuffer[10]) << 8) + inBuffer[11]; // i.e., short int    
    
    
    // Read LSB first ...
    /*gLatestSample.G0 = (static_cast<short>(inBuffer[ 1]) << 8) + inBuffer[ 0]; // note these are read in a 
    gLatestSample.G2 = (static_cast<short>(inBuffer[ 3]) << 8) + inBuffer[ 2]; // very specific order to 
    gLatestSample.G4 = (static_cast<short>(inBuffer[ 5]) << 8) + inBuffer[ 4]; // minimize cross talk
    gLatestSample.G1 = (static_cast<short>(inBuffer[ 7]) << 8) + inBuffer[ 6]; // G0, 2, 4, 1, 3, 5;
    gLatestSample.G3 = (static_cast<short>(inBuffer[ 9]) << 8) + inBuffer[ 8]; // each is int16
    gLatestSample.G5 = (static_cast<short>(inBuffer[11]) << 8) + inBuffer[10]; // i.e., short int
    */
    
    gLatestSample.Check = inBuffer[12];  // byte long   
    
    // extract error bit, bad if 1; good if 0.
    gLatestSample.statusError   = (B10000000 & inBuffer[12] ) > 0 ; // 7th bit
    
    // compute checksum 
    unsigned char localChecksum = 0;
    for ( int i = 0; i < 12; i++) 
      localChecksum += inBuffer[i];
    if (   ( localChecksum & 0x7f )  !=  ( inBuffer[12] & 0x7f )   )
      gLatestSample.checksumError = true;
    else
      gLatestSample.checksumError = false;    
    
    /*Serial.print( gLatestSample.t_us ); Serial.print("\t");
    Serial.print( gLatestSample.G0 ); Serial.print("\t");
    Serial.print( gLatestSample.G1 ); Serial.print("\t");
    Serial.print( gLatestSample.G2 ); Serial.print("\t");
    Serial.print( gLatestSample.G3 ); Serial.print("\t");
    Serial.print( gLatestSample.G4 ); Serial.print("\t");
    Serial.print( gLatestSample.G5 ); Serial.print("\t");
    Serial.print( gLatestSample.Check, BIN ); Serial.print("\t");
    Serial.print( gLatestSample.checksumError, BIN ); Serial.print("\t");
    Serial.print( gLatestSample.statusError, BIN ); Serial.println("\t");*/
  }    

 
  if ( gATIisStreaming && (t_ms != tPrev_ms )) {
    //Serial.print( currentSample.t_us ); Serial.print("\t");
    tPrev_ms = t_ms;
    Serial.print( currentSample.G0 ); Serial.print("\t");
    Serial.print( currentSample.G1 ); Serial.print("\t");
    Serial.print( currentSample.G2 ); Serial.print("\t");
    Serial.print( currentSample.G3 ); Serial.print("\t");
    Serial.print( currentSample.G4 ); Serial.print("\t");
    Serial.print( currentSample.G5 ); Serial.print("\t\t");
    //Serial.print( currentSample.Check, BIN ); Serial.print("\t");
    //Serial.print( currentSample.checksumError, BIN ); Serial.print("\t");
    //Serial.print( currentSample.statusError, BIN ); Serial.print("\t\t");
    //Serial.print( currentSample.t_us ); Serial.print("\t");
    //Serial.print( t_ms ); //Serial.print("\t");
    Serial.println();
    
  }

  
}



#include "handleSerial.h"
