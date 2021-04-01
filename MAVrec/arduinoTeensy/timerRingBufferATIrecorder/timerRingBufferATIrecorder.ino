// Use a hardware timer interrupt and ring buffer to reliable read data from ATI NETCAN OEM board 
// and stream to serial.   A timer interrupt (timer1 16-bit timer) is used,  dumps data to serial port.




#define LED_PIN      13


// A ring buffer.  Allows you to write data VERY quickly inside an interrupt.  Can read it later in chunks
// to write to ring buffer, write myDataSample to: iWrite = (currentIndex+1) % BUF_LENGTH location along BUF_LENGTH; 
//                          THEN, WHEN DONE WRITING: set lastWritten = iWrite
// to read from ring bufffer, read myDataSample from: iRead = (currentIndex+1) % BUF_LENGTH location; 
//                          THEN, WHEN DONE READING: set lastread = iRead
#define BUF_LENGTH     100  // number of entries in ring buffer before overflow
#define BUF_DIMENSION  1     // number of items per buffer location  (e.g.  dataSamples per buffer location)
#define BANNER         "%  t[ms]  Fx[N]    Fy[N]   Fz[N]   Tx[mNm]  Ty[mNm]  Tz[mNm]   " 
struct myDataSample {
  //unsigned long int t_ms;  // time of sample (milli sec)  
  unsigned long t_us;  // sample time in micros (as received by teensy)
  unsigned long t_ms;  // sample time in millis (as received by teensy)
  signed short G[6];
  /*signed short G0;    // order in which ';gauge vector' is received 0,2,4,1,3,5;
  signed short G1;     // each should be 16 bit signed integer (short)   
  signed short G2;     // confirmed on teensy 4.1 with sizeof( signed short ) -> 2
  signed short G3;
  signed short G4;
  signed short G5;*/
  unsigned char Check;// uint8 checksum+status; originally called 'check' in ATI Digital FT Modbus doc
  bool checksumError;    // 0: all is ok; true or 1 means bad
  bool statusError;      // error status reported during streaming (last bit of 'check' field) 0: all ok
  double Fx;    // 
  double Fy;    //
  double Fz;    // 
  double Tx;    //
  double Ty;    // 
  double Tz;    //    
  
} ;
volatile myDataSample ringBuffer[BUF_LENGTH][BUF_DIMENSION];
bool ringBufferOverflow;
volatile unsigned long int lastWritten;
volatile unsigned long int lastRead;
volatile unsigned long int iWrite, iRead; // index values for bookeeping
bool amRecording; 
volatile unsigned long int t0_vol ;    // extra volitile timer, Just in case


// set this to the hardware serial port you wish to use
#define HWSERIAL    Serial1  // wire D0, D1 to RS485 Sparkfun breakout Rx, Tx;; 3.3v  and gnd
#define HWSERIAL_TX_ENABLE_PIN 2 // wire to sparkfun RTS
#define ATI_BAUD    1250000
#define TIMEOUT_US  200000  // after ~100 ms, stop waiting to hear from ATI
#define ATI_CALIBRATION_ADDR 0x00E3 // address of calibration # to load from netcanoem's 
                   // internal list (should match sensor Part #)                                      
                                    // see pg 2 of Digital FT Modbus Interface.pdf doc
#include "atiFunctions.h"


                               
                               // Sample period of timer interupt service routine (ISR) in microseconds
//#define   TIMER_PERIOD 10000 // e.g. set a timer of length 10000 microseconds (or 0.1 sec - or 10Hz) 
//#define TIMER_PERIOD 1000    // 1000us = 1ms =>  1 KHz update rate.
#define TIMER_PERIOD 20000     // 20ms --> 50Hz update rate

// Create an IntervalTimer object 
IntervalTimer sampleTimer;
unsigned int samplePeriod = TIMER_PERIOD;  // in us, e.g. 10ms sampling period is 10000us
volatile  unsigned long t_ms, t0_ms;    // bookeeping in ms
unsigned long tPrev_ms;
volatile myDataSample currentSample; // updated more slowly than latestSample

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
int ledState = HIGH;
void timerISR()
{
    // Toggle LED
    ledState = !ledState;
    digitalWrite( LED_PIN, ledState );
    static int g = 0;  
    // Get most recent data, write it to ring buffer.
    // to write to ring buffer, write myDataSample to: iWrite = (currentIndex+1) % BUF_LENGTH location along BUF_LENGTH; 
    //                          THEN, WHEN DONE WRITING: set lastWritten = iWrite
    iWrite = (lastWritten+1) % BUF_LENGTH;        // start writing to next entry
    ringBuffer[iWrite][0].t_ms     = millis()-t0_ms;    // get current time in millis sec
    ringBuffer[iWrite][0].t_us = gLatestSample.t_us;
    //ringBuffer[iWrite][0].t_ms = gLatestSample.t_ms;
    for( g = 0; g < 6  ; g++)
      ringBuffer[iWrite][0].G[g] = gLatestSample.G[g];
    /*ringBuffer[iWrite][0].G0 = gLatestSample.G0;
    ringBuffer[iWrite][0].G1 = gLatestSample.G1;
    ringBuffer[iWrite][0].G2 = gLatestSample.G2;
    ringBuffer[iWrite][0].G3 = gLatestSample.G3;
    ringBuffer[iWrite][0].G4 = gLatestSample.G4;
    ringBuffer[iWrite][0].G5 = gLatestSample.G5;*/
    ringBuffer[iWrite][0].checksumError = gLatestSample.checksumError;
    ringBuffer[iWrite][0].statusError = gLatestSample.statusError;
//    ringBuffer[iWrite][0].Fx    = ledState;  // get values from NetCAN OEM stream
//    ringBuffer[iWrite][0].Fy    = HIGH;  // get values from NetCAN OEM stream
//    ringBuffer[iWrite][0].Fz    = LOW;  // get values from NetCAN OEM stream
//    ringBuffer[iWrite][0].Tx    = analogRead( A4 );  // get values from NetCAN OEM stream
//    ringBuffer[iWrite][0].Ty    = analogRead( A5 );  // get values from NetCAN OEM stream
//    ringBuffer[iWrite][0].Tz    = analogRead( A6 );  // get values from NetCAN OEM stream
    lastWritten = iWrite;                       // officially done writing this "next entry"        
}




// this function will handle all PC-issued serial commands like:
// 's' - get status
// 'b' - begin streaming
// 'x' - stop streaming
#include "handleSerial.h"





//////////////////////////////////////////////////////////////////////////////////
///////////////////////    SETUP    //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  
  lastRead = 0;
  lastWritten = 0;
  
  Serial.begin( 384000/* 115200*/ );
  //Serial.println(BANNER);
  Serial.flush();

  HWSERIAL.begin( ATI_BAUD, SERIAL_8E1 ); // default Baud reate of netcanoem is 1.25Mbps
  Serial1.transmitterEnable( HWSERIAL_TX_ENABLE_PIN );

  amRecording = false;
    
  //ATIstartStreaming() ;
  t0_ms = millis();
  tPrev_ms = t_ms;
  
}



//////////////////////////////////////////////////////////////////////////////////
///////////////////////    LOOP     //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void loop() {
  
   // if there are any serial commands, process them
   // Handle Serial Commands from PC
   if (Serial.available() > 0)      
      handleSerialInput ( Serial.read() ) ;    
   
    
    // Empty entire ringBuffer to Serial...
    // to read from ring bufffer, read myDataSample from: iRead = (currentIndex+1) % BUF_LENGTH location; 
    //                          THEN, WHEN DONE READING: set lastread = iRead
    unsigned int iToRead = (lastWritten - lastRead + BUF_LENGTH) % BUF_LENGTH;    
    for ( int i = 0; i < iToRead; i++ ) {
      
      iRead = (lastRead+1) % BUF_LENGTH;  // start reading next entry
      //enc = ringBuffer[iRead][0].t

      // do math for unit conversion (see ATI documentation;  [F; T] = BasicMatrix * [G0; G1; ...G5]
      // remove bias (tare value)
      for ( int g = 0; g < 6; g++){
        ringBuffer[iRead][0].G[g] = ringBuffer[iRead][0].G[g] - Gbias[g];
      }
      ringBuffer[iRead][0].Fx = 0;
      ringBuffer[iRead][0].Fy = 0;
      ringBuffer[iRead][0].Fz = 0;
      ringBuffer[iRead][0].Tx = 0;
      ringBuffer[iRead][0].Ty = 0;
      ringBuffer[iRead][0].Tz = 0;
      
      for( int g = 0; g < 6; g++){
        ringBuffer[iRead][0].Fx += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[0][g]; 
        ringBuffer[iRead][0].Fy += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[1][g]; 
        ringBuffer[iRead][0].Fz += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[2][g]; 
        ringBuffer[iRead][0].Tx += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[3][g]; 
        ringBuffer[iRead][0].Ty += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[4][g]; 
        ringBuffer[iRead][0].Tz += ringBuffer[iRead][0].G[g] * (float)BasicMatrix[5][g];       
      }


      // output to serial as ASCII 
      Serial.print( ringBuffer[iRead][0].t_ms );      
      Serial.print( ",    " );
      Serial.print( ringBuffer[iRead][0].Fx / (float)CountsPerForce );
      Serial.print( ",  " );
      Serial.print( ringBuffer[iRead][0].Fy / (float)CountsPerForce );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].Fz / (float)CountsPerForce );
      Serial.print( ",    " ); 
      Serial.print( ringBuffer[iRead][0].Tx / (float)(CountsPerTorque/1000) ); // convert to mNm
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].Ty / (float)(CountsPerTorque/1000) );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].Tz / (float)(CountsPerTorque/1000) );   
      Serial.print( "     % " );
      Serial.print( ringBuffer[iRead][0].t_us );      
      Serial.print( ", iToRead:" );
      Serial.print( iToRead );
      Serial.print( ", iRead:" );
      Serial.print( iRead );
      Serial.print( ", lastWritten:" ); 
      Serial.print( lastWritten );
      Serial.print( ", lastRead:" ); 
      Serial.print( lastRead );
      Serial.print( ",  i:" ); 
      Serial.print( i       );
      /*Serial.print( ringBuffer[iRead][0].G[0] );
      Serial.print( ",  " );
      Serial.print( ringBuffer[iRead][0].G[1] );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].G[2] );
      Serial.print( ",    " ); 
      Serial.print( ringBuffer[iRead][0].G[3] );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].G[4] );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].G[5] );   */
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].checksumError );
      Serial.print( ",  " ); 
      Serial.print( ringBuffer[iRead][0].statusError );   
      Serial.println(  );
      
      lastRead = iRead; // officially done reading the iRead entry
    }
    Serial.flush();


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
    gLatestSample.G[0] = (static_cast<signed short>(inBuffer[ 0]) << 8) + inBuffer[ 1]; // note these are read in a 
    gLatestSample.G[2] = (static_cast<signed short>(inBuffer[ 2]) << 8) + inBuffer[ 3]; // very specific order to 
    gLatestSample.G[4] = (static_cast<signed short>(inBuffer[ 4]) << 8) + inBuffer[ 5]; // minimize cross talk
    gLatestSample.G[1] = (static_cast<signed short>(inBuffer[ 6]) << 8) + inBuffer[ 7]; // G0, 2, 4, 1, 3, 5;
    gLatestSample.G[3] = (static_cast<signed short>(inBuffer[ 8]) << 8) + inBuffer[ 9]; // each is int16
    gLatestSample.G[5] = (static_cast<signed short>(inBuffer[10]) << 8) + inBuffer[11]; // i.e., short int    


    
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

 
  /*if ( gATIisStreaming && (t_ms != tPrev_ms )) {
    Serial.print( currentSample.t_us ); Serial.print("\t");
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
    
  }*/




    
    
  
}
