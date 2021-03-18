// ATI info for reading forces from the ATI NetCANOEM board

// Global structure for a single streamed sample from ATI NetCANoem board
struct streamingSample {
  unsigned long t_us;  // sample time in micros (as received by teensy)
  signed short G0;     // order in which ';gauge vector' is received 0,2,4,1,3,5;
  signed short G1;     // each should be 16 bit signed integer (short)   
  signed short G2;     // confirmed on teensy 4.1 with sizeof( signed short ) -> 2
  signed short G3;
  signed short G4;
  signed short G5;
  unsigned char Check;// uint8 checksum+status; originally called 'check' in ATI Digital FT Modbus doc
  bool checksumError;    // 0: all is ok; true or 1 means bad
  bool statusError;      // error status reported during streaming (last bit of 'check' field) 0: all ok
};

// GLOBAL variables:
streamingSample gLatestSample;
unsigned long int gATIstatus;  // global status word, 0 means all good (16 bit, 2 bytes) (last bit, #15, is overall status)
bool gATIsensorIsConnected;    // global flag; usually updated with every read.
bool gATIisStreaming;          // global flag, Only true when values stream (i.e. you should wait until 13 byte chunks show up)
String ATIcalibrationBanner = "% ATI Sensor not initialized; Calibration & units unknown. ";



// Internal book-keeping
unsigned char incomingByte;      // book-keeping
unsigned char inBuffer[ 13 ];    // ATI streaming message length is 13 bytes
unsigned long t, t0;  // time keeping variables [us]

//////////////////////////////////////////////////////////////////////////////
// Helper Functions:
// Returns full 16 bit status bit;  note that any error sets the last bit (#15).
// note also that while streaming, that bit is appended into the 'check' field
unsigned char ATIgetStatus( void );

// sends the start streaming command to NetCANoem board; it should start streaming data at 7kHz
// dumping the latest information to the global variable "gLatestSample"
bool ATIstartStreaming( void );

// sends the jamming sequence (any 14-byte or longer command) to stop streaming
// assumes streaming is already occuring.
void ATIstopStreaming( void );

// Reads the calibration information in the calibration table labeled calibrationNum; 
//  default is Calibration 1 --> calibrationNum = 1 
//  assumes no tool transformations in the "active calibration matrix"
//  sends "Unlock Storage command" to unlock active gain and offset hardware settings
//  writes active (volitile) gain and offset hardware settings using "GageGain" and "GageOffset" from calibration data
//  sends "unlock storage command" againg to lock the gain and offset against accidental writing
//  runs ATI getStatus( );
//  returns gATIsensorIsConnected which is only true if everything seems ok.  
//  str will contain the printable calibration information (Serial# #, matrix used, units, etc.)
bool ATIinitialize( );

//////////////////////////////////////////////////////////////////////////
// Modbus commands for ATI NetCAN OEM device
const int cmdLengthStatus =   8;
const int cmdLengthStream =   5;
const int cmdLengthJam   =   14;
unsigned char atiReadStatusWord[ cmdLengthStatus ] = {
  0x0a, // Slave Address ( DEC 10 )
  0x03, // standard modbus read register command
  0x00, // padding, start of status word register)
  0x1d, // status word register
  0x00, // padding, # of holding registers to read
  0x01, //   # of holding registers to read
  0x15, // 1st CRC byte
  0x77, // 2nd CRC byte
};
unsigned char atiStreamDataCommand[ cmdLengthStream ] = {
  0x0a, // Slave Address ( DEC 10 )
  0x46, // Start streaming command (DEC 70)
  0x55, // Just a data byte you always send w/ this command.  Should receive 0x01 in response
  0xa3, // 1st CRC byte
  0x9d, // 2nd CRC byte
};
unsigned char atiJammingCommand [ cmdLengthJam ] = {
  0xFF  //any command 14 bytes or longer will do
};
unsigned char atiReadCalibration[ cmdLengthStatus ] = { // same length as get status command
  0x0a, // Slave Address ( DEC 10 )
  0x03, // standard modbus read register command
  ATI_CALIBRATION_ADDR_HI, // padding, start of status word register)
  ATI_CALIBRATION_ADDR_LO, // status word register
  0x00, // padding, # of holding registers to read; TBD
  0x00, //   # of holding registers to read; TBD
  0x00, // 1st CRC byte; TBD
  0x00, // 2nd CRC byte; TBD
};
#define CRC_LO 7   // index of 1st and 2nd CRC byte that's TBD
#define CRC_HI 8   //
//////////////////////////////////////////////////////////////////////////
#include "crc16.h"

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////   ATI helper function defs  /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
// Returns full 16 bit status bit;  note that any error sets the last bit (#15).
// note also that while streaming, that bit is appended into the 'check' field.
// Blocking call, won't return until done (gets response from ATI) or TIMEOUT_US
unsigned char ATIgetStatus( void ) {
  HWSERIAL.clear();
  HWSERIAL.write( atiReadStatusWord, cmdLengthStatus ); HWSERIAL.flush();
  //HWSERIAL.clear();
  unsigned long T;
  unsigned short myStatus = 0;
  unsigned char bytesToRead = 0;
  unsigned short registerValHi, registerValLow;

  /*delay(20); Serial.println("Let's get everything in response for 10 s...");
  T = micros();
  while( HWSERIAL.available() || micros()-T < 10000000) 
    if( HWSERIAL.available()) 
    {Serial.print(micros()); Serial.print("   "); Serial.println( HWSERIAL.read(), HEX);}
  */


  T = micros();
  // consume the expected response bytes from ATI NetCANOEM board (response ACK 5+2 bytes )
  while ( HWSERIAL.available() < 5+2 ) {
    /* do nothing */
    if ( micros() - T > TIMEOUT_US )
    {
      gATIsensorIsConnected = false;
      myStatus = myStatus*0-1;
      gATIstatus = myStatus;
      Serial.println(" Internal Error: Failed to read status");
      return myStatus; // BREAK OUT OF THIS FUNCTION; No response from  ATI Sensor
    }
  }

  // while( HWSERIAL.available()) Serial.println( HWSERIAL.read(), HEX);
  // delay(20); Serial.println("Let's try again...");  
  // T = micros();
  // while( HWSERIAL.available() || micros()-T < 1000000) Serial.println( HWSERIAL.read(), HEX);
  
  // Should read the following (5 bytes + 2 crc bytes = 7 bytes):
  //  0x0a (slave address)
  //  0x03 (Function code == read holding register, same as command)
  //  0x(2*N) =2 total # of bytes to read (=2*N where N is quantity of registers, 1 register = 2bytes)
  //  0xZZ  MSB status
  //  0xZZ  LSB status 
  //  0xXX, 0xYY 1st and last CRC bytes
  //  0x0   Error;  Bad if 0x83; exceptions of x01,02, ... 0x04.  0x00 is ALL OK.
  if ( HWSERIAL.read() != 0x0A ) Serial.println( "Internal error: unexpected address in response from get status");
  if ( HWSERIAL.read() != 0x03 ) Serial.println( "Internal error: unexpected functionID in response from get status");
  bytesToRead = HWSERIAL.read();
  if ( bytesToRead > 2 ) Serial.println( "Internal error: unexpected # of bytes in response from get status");
  //for ( int i = 0; i < bytesToRead; i = i+2 )
  //{    
    registerValHi  = HWSERIAL.read(); // read MSB first
    registerValLow = HWSERIAL.read(); // read LSB last
  //}

  // combine both registers into two-byte word
  //Serial.print("registerValHi : ");Serial.println( registerValHi , BIN );
  //Serial.print("registerValLow: ");Serial.println( registerValLow, BIN );

  //Serial.print("Before: "); Serial.println( myStatus, BIN );
  myStatus = (static_cast<unsigned short>(registerValHi) << 8 ) +  static_cast<unsigned short>(registerValLow);
  //Serial.print("After: ");Serial.println( myStatus, BIN );

  // TODO:  run CRC code for extra confirmation 
  // (note if anything was corrupted the status returned will be nonzero, so CRC not crucial here
  HWSERIAL.read();  // first CRC byte
  HWSERIAL.read();  // second CRC byte
  
  gATIsensorIsConnected = true;
  gATIisStreaming = false;
  gATIstatus = myStatus;
  HWSERIAL.clear();
  return myStatus;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sends the start streaming command to NetCANoem board; it should start streaming data at 7kHz
// dumping the latest information to the global variable "gLatestSample"
// Blocking call, won't return until done (gets response from ATI or TIMEOUT_US).
bool ATIstartStreaming( void ) {
  HWSERIAL.clear();
  HWSERIAL.write( atiStreamDataCommand, cmdLengthStream );HWSERIAL.flush();
  unsigned long T;
  T = micros();

  // consume the first cmdLengthStream bytes from ATI NetCANOEM board (response ACK 5 bytes of dataStreamDataCommand)
  while ( HWSERIAL.available() < cmdLengthStream )
  {
    /* do nothing until the expected # of bytes show up or timeout */
    if ( micros() - T > TIMEOUT_US )
    {
      gATIsensorIsConnected = false;
      return gATIsensorIsConnected; // BREAK OUT OF THIS FUNCTION; No response from  ATI Sensor
    }
  }

  //   MODBUS packet & timing de-bugging
  /*   Serial.println("Let's get everything in response for 1 s...");
  T = micros();
  while( HWSERIAL.available() || micros()-T < 1000000) 
    if( HWSERIAL.available()) 
    {Serial.print(micros()); Serial.print("   "); Serial.println( HWSERIAL.read(), HEX);}
  */

  //   MODBUS packet & timing de-bugging
  /*   for ( int i = 0; i < cmdLengthStream; i++)
  { 
    incomingByte =  HWSERIAL.read();
     Serial.println( incomingByte, HEX );
  }*/

  
  
  // if you get here, there should be at least 5 bytes ready for reading;  This should be 
  // the MODBUS response to the start streaming command.  + 2 crc bytes
  // 0x0a, // Slave Address ( DEC 10 )
  // 0x46, // Start streaming command (DEC 70)
  // 0x01 instead of 0x55, // Just a data byte you always send w/ this command.  Should receive 0x01 in response
  // 0xA2, // 1st CRC byte
  // 0x62, // 2nd CRC byte
  if ( HWSERIAL.read() != 0x0A ) Serial.println( "Internal error: unexpected address in response from start streaming");
  if ( HWSERIAL.read() != 0x46 ) Serial.println( "Internal error: unexpected functionID in response from start streaming");
  if ( HWSERIAL.read() != 0x01 ) Serial.println( "Internal error: unexpected response from start streaming");
  if ( HWSERIAL.read() != 0xA2 ) Serial.println( "Internal error: Failed CRC check, 1st byte, start streaming");
  if ( HWSERIAL.read() != 0x62 ) Serial.println( "Internal error: Failed CRC check, 2nd byte, start streaming");
  
  //Serial.println("% Starting ATI Nano17 Streaming ...");
  //delayMicroseconds( TIMEOUT_US ); 
  t0 = micros();
  

  gATIisStreaming = true;
  gATIsensorIsConnected = true;  
  return (gATIsensorIsConnected);
}

/////////////////////////////////////////////////////////////////////////////////
// sends the jamming sequence (any 14-byte or longer command) to stop streaming
// assumes streaming is already occuring.
void ATIstopStreaming( void ) {
  HWSERIAL.write( atiJammingCommand,  cmdLengthJam  ); HWSERIAL.flush();
  delayMicroseconds( TIMEOUT_US ); 
  delayMicroseconds( TIMEOUT_US ); 
  HWSERIAL.clear();
  gATIisStreaming = false;
  return;
}


/////////////////////////////////////////////////////////////////////////////////////////
//  Reads the calibration information in the calibration table at address ATI_CALIBRATION_ADDR; 
//    default is Calibration 1 --> calibrationNum = 1 at address ATI_CALIBRATION_ADDR
//    assumes no tool transformations in the "active calibration matrix"
//  sends "Unlock Storage command" to unlock active gain and offset hardware settings
//  writes active (volitile) gain and offset hardware settings using "GageGain" and "GageOffset" from calibration data
//  sends "unlock storage command" againg to lock the gain and offset against accidental writing
//  runs ATI getStatus( );
//  returns gATIsensorIsConnected which is only true if everything seems ok.  
//  updates the ATIcalibrationBanner string with printable calibration information 
//    like Serial#, calibration matrix, offset, units, etc.
bool ATIinitialize( void ){

  ATIcalibrationBanner = "%% ATI Sensor not initialized; Calibration & units unknown. ";
  
  //  Read the calibration information in the calibration table labeled calibrationNum;   
  //  assumes no tool transformations in the "active calibration matrix"
  //  from the documentation, the calibration data structure is:  
  //  uint8 CalibSerialNumber[8];
  //  uint8 CalibPartNumber[32];
  //  uint8 CalibFamilyId[4];
  //  uint8 CalibTime[20];
  //  float BasicMatrix[6][6];
  //  uint8 ForceUnits;
  //  uint8 TorqueUnits;
  //  float MaxRating[6];
  //  int32 CountsPerForce;
  //  int32 CountsPerTorque;
  //  uint16 GageGain[6];
  //  uint16 GageOffset[6];
  //  uint8 Resolution[6];
  //  uint8 Range[6];
  //  uint16 ScaleFactor16[6];
  //  uint8 UserField1[16];
  //  uint8 UserField2[16];
  //  uint8 SpareData[16];
  // The code below will attempt to read the desired fields above.
  unsigned long T;
  unsigned short myStatus = 0;
  unsigned char bytesToRead;
  // First figure out how many bytes to read the first time, update atiReadCalibration command and crc fields
  bytesToRead = 8*8 ; // //  uint8 CalibSerialNumber[8];
  //  uint8 CalibSerialNumber[8];

  //unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen)
  ////unsigned char *puchMsg ;    /* message to calculate CRC upon */
  ////unsigned short usDataLen ;  /* quantity of bytes in message */
  
  HWSERIAL.write( atiReadCalibration, cmdLengthStatus ); HWSERIAL.flush();
  //HWSERIAL.clear();
  
  unsigned short registerValHi, registerValLow;

  /*delay(20); Serial.println("Let's get everything in response for 10 s...");
  T = micros();
  while( HWSERIAL.available() || micros()-T < 10000000) 
    if( HWSERIAL.available()) 
    {Serial.print(micros()); Serial.print("   "); Serial.println( HWSERIAL.read(), HEX);}
  */

  /*
  T = micros();
  // consume the expected response bytes from ATI NetCANOEM board (response ACK 5+2 bytes )
  while ( HWSERIAL.available() < 5+2 ) {
    // do nothing 
    if ( micros() - T > TIMEOUT_US )
    {
      gATIsensorIsConnected = false;
      myStatus = myStatus*0-1;
      gATIstatus = myStatus;
      Serial.println(" Internal Error: Failed to read status");
      return myStatus; // BREAK OUT OF THIS FUNCTION; No response from  ATI Sensor
    }
  }
  */
  // while( HWSERIAL.available()) Serial.println( HWSERIAL.read(), HEX);
  // delay(20); Serial.println("Let's try again...");  
  // T = micros();
  // while( HWSERIAL.available() || micros()-T < 1000000) Serial.println( HWSERIAL.read(), HEX);
  
  // Should read the following (5 bytes + 2 crc bytes = 7 bytes):
  //  0x0a (slave address)
  //  0x03 (Function code == read holding register, same as command)
  //  0x(2*N) =2 total # of bytes to read (=2*N where N is quantity of registers, 1 register = 2bytes)
  //  0xZZ  MSB status
  //  0xZZ  LSB status 
  //  0xXX, 0xYY 1st and last CRC bytes
  //  0x0   Error;  Bad if 0x83; exceptions of x01,02, ... 0x04.  0x00 is ALL OK.
  //if ( HWSERIAL.read() != 0x0A ) Serial.println( "Internal error: unexpected address in response from get status");
  //if ( HWSERIAL.read() != 0x03 ) Serial.println( "Internal error: unexpected functionID in response from get status");
  //bytesToRead = HWSERIAL.read();
  //if ( bytesToRead > 2 ) Serial.println( "Internal error: unexpected # of bytes in response from get status");
  ////for ( int i = 0; i < bytesToRead; i = i+2 )
  ////{    
  //  registerValHi  = HWSERIAL.read(); // read MSB first
  //  registerValLow = HWSERIAL.read(); // read LSB last
  ////}

  //// combine both registers into two-byte word
  ////Serial.print("registerValHi : ");Serial.println( registerValHi , BIN );
  ////Serial.print("registerValLow: ");Serial.println( registerValLow, BIN );

  //Serial.print("Before: "); Serial.println( myStatus, BIN );
  //myStatus = (static_cast<unsigned short>(registerValHi) << 8 ) +  static_cast<unsigned short>(registerValLow);
  //Serial.print("After: ");Serial.println( myStatus, BIN );

  // TODO:  run CRC code for extra confirmation 
  // (note if anything was corrupted the status returned will be nonzero, so CRC not crucial here
  //HWSERIAL.read();  // first CRC byte
  //HWSERIAL.read();  // second CRC byte
  
  //gATIsensorIsConnected = true;
  //gATIisStreaming = false;
  //gATIstatus = myStatus;
  //HWSERIAL.clear();
  //return myStatus;

  
  ///////////////////////////////////////////////////////////////////////////////////////
  //  send "Unlock Storage command" to unlock active gain and offset hardware settings
  ///////////////////////////////////////////////////////////////////////////////////////
  //  writes active (volitile) gain and offset hardware settings using "GageGain" and "GageOffset" from calibration data
  ///////////////////////////////////////////////////////////////////////////////////////
  //  sends "unlock storage command" againg to lock the gain and offset against accidental writing
  ////////////////////////////
  //  runs ATI getStatus( );

  ///////////////////////////////
  //  updates the ATIcalibrationBanner string with printable calibration information 

  ///////////////////////////////////////////
  //  returns gATIsensorIsConnected which is only true if everything seems ok.  
  

  
  
  
  return false;

}
//////////////////////////////////////////////////////////////////////////////////
//////////////////////   END helper fxns   ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
