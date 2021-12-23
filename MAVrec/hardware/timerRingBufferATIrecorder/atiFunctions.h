// ATI info for reading forces from the ATI NetCANOEM board
// Global structure for a single streamed sample from ATI NetCANoem board

struct streamingSample {
  unsigned long t_us;  // sample time in micros (as received by teensy)
  unsigned long t_ms;  // sample time in millis (as received by teensy)
  signed short G[6];  
  /*signed short G0;     // order in which ';gauge vector' is received 0,2,4,1,3,5;
  signed short G1;      // each should be 16 bit signed integer (short)   
  signed short G2;      // confirmed on teensy 4.1 with sizeof( signed short ) -> 2
  signed short G3;
  signed short G4;
  signed short G5;*/
  unsigned char Check;// uint8 checksum+status; originally called 'check' in ATI Digital FT Modbus doc
  bool checksumError;    // 0: all is ok; true or 1 means bad
  bool statusError;      // error status reported during streaming (last bit of 'check' field) 0: all ok
};
signed short Gbias[6]; // used to Tare;  

// GLOBAL variables:
volatile streamingSample gLatestSample;
unsigned long int gATIstatus;  // global status word, 0 means all good (16 bit, 2 bytes) (last bit, #15, is overall status)
bool gATIsensorIsConnected;    // global flag; usually updated with every read.
bool gATIisStreaming;          // global flag, Only true when values stream (i.e. you should wait until 13 byte chunks show up)
String ATIcalibrationBanner = "% ATI Sensor not initialized; Calibration & units unknown. ";



// Internal book-keeping
unsigned char incomingByte;      // book-keeping
unsigned char inBuffer[ 13 ];    // ATI streaming message length is 13 bytes
char 		  tempBuffer[512];   // temporary space for reading in holding registers, usually converted to strings
unsigned long t, t0;  // time keeping variables [us]

String sCalibSerialNumber, sCalibPartNumber, sCalibFamilyId, sCalibTime, sForceUnits, sTorqueUnits; 
  
float BasicMatrix[6][6];


unsigned ForceUnitsCode;
unsigned TorqueUnitsCode;
float MaxRating[6];
int CountsPerForce;
int CountsPerTorque;
unsigned int GageGain[6];
unsigned int GageOffset[6];


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

// helper function to read from holding registers (e.g. calibration values) via MODBUS
void ATIreadHoldingRegister( unsigned char addrHi,  unsigned char addrLo, unsigned short numBytes);

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
unsigned char atiJammingCommand [ cmdLengthJam ] = {  //any command 14 bytes or longer will do
  B10101010, B10101010, B10101010, B10101010,  
  B10101010, B10101010, B10101010, B10101010, 
  B10101010, B10101010, B10101010, B10101010, 
  B10101010, B10101010 
};

    
unsigned char atiReadHoldingReg[ cmdLengthStatus ] = { // same length as get status command
  0x0a, // Slave Address ( DEC 10 )
  0x03, // standard modbus read register command
  0x00,	// padding, start of status word register) ATI_CALIBRATION_ADDR
  0x00, // status word register ATI_CALIBRATION_ADDR
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

  Serial.println("   ... getting status ...");  
  //unsigned int n = HWSERIAL.available();
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
    //Serial.println(HWSERIAL.available());
    //Serial.println(micros() - T);
    //if( HWSERIAL.available() > n )  {n = HWSERIAL.available(); Serial.print("HWSERIAL.available("); Serial.print( n ); Serial.println(")  ");}
    
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
    // do nothing until the expected # of bytes show up or timeout 
    if ( micros() - T > TIMEOUT_US )
    {
      gATIsensorIsConnected = false;
      return gATIsensorIsConnected; // BREAK OUT OF THIS FUNCTION; No response from  ATI Sensor
    }
  }
  
  
  /*Serial.println("Let's get everything in response for 1 s...");
  T = micros();
  while( HWSERIAL.available() || micros()-T < 1000000) 
    if( HWSERIAL.available()) 
    {Serial.print(micros()); Serial.print("   "); Serial.println( HWSERIAL.read(), HEX);}
  */

  /*//   MODBUS packet & timing de-bugging
  for ( int i = 0; i < cmdLengthStream; i++)
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
  if ( HWSERIAL.read() != 0x0A ) Serial.println( "% Internal error: unexpected address in response from start streaming");
  if ( HWSERIAL.read() != 0x46 ) Serial.println( "% Internal error: unexpected functionID in response from start streaming");
  if ( HWSERIAL.read() != 0x01 ) Serial.println( "% Internal error: unexpected response from start streaming");
  if ( HWSERIAL.read() != 0xA2 ) /*Serial.println( "% Internal error: Failed CRC check, 1st byte, start streaming")*/;
  if ( HWSERIAL.read() != 0x62 ) /*Serial.println( "% Internal error: Failed CRC check, 2nd byte, start streaming")*/;
  
  Serial.println("% Starting ATI Nano17 Streaming ...");
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
  //delayMicroseconds( TIMEOUT_US ); 
  //delayMicroseconds( TIMEOUT_US ); 
  HWSERIAL.clear();
  gATIisStreaming = false;
  return;
}


// helper function to read from holding registers (e.g. calibration values) via MODBUS
// each addr entry pointed to (points) of the holding registers is 2 bytes wide.
void ATIreadHoldingRegister( unsigned short addr , unsigned short numPointsToRead){
	
	unsigned short myCRC;
	unsigned long T;
	unsigned char incoming[512];  // used to process incoming messages that need a CRC check. 
	unsigned char addrHi, addrLo;
	addrHi = addr >> 8;
	addrLo = addr & 0x00FF;
	
	// update the command atiReadHoldingReg[cmdLengthStatus] with new address and by
	//atiReadHoldingReg[ 0 ] = 0x0a; // Slave Address ( DEC 10 )
	//atiReadHoldingReg[ 1 ] = 0x03; // std modbus read register command
	atiReadHoldingReg[ 2 ] = addrHi; // start of status word register, High
	atiReadHoldingReg[ 3 ] = addrLo; // status word register, low
	atiReadHoldingReg[ 4 ] = numPointsToRead >> 8; //   # of holding registers to read; High
	atiReadHoldingReg[ 5 ] = numPointsToRead;      //   # of holding registers to read; Low
	myCRC = CRC16( atiReadHoldingReg, 6 );
	
	atiReadHoldingReg[ 6 ] = myCRC >> 8; // 1st CRC byte; 
	atiReadHoldingReg[ 7 ] = myCRC;  	 // 2nd CRC byte; 
	
	//Serial.println(" in function ATIreadHoldingRegister() ");
	//Serial.print(" atiReadHoldingReg command: \r\n");
	//for( int i = 0; i < cmdLengthStatus ; i++)
	//	Serial.println( atiReadHoldingReg[ i ] , HEX );	
	//Serial.print(" numPointsToRead: "); Serial.println( numPointsToRead, HEX );
	//Serial.print(" myCRC "); Serial.println( myCRC, HEX);
	
	// write the command 
	HWSERIAL.clear();
	HWSERIAL.write( atiReadHoldingReg, cmdLengthStatus ); HWSERIAL.flush();
	
	
	
	/*Serial.println("Let's get everything in response for 1 s...");	
	T = micros();
	while( micros()-T < 1000000 ) 
	{
		delayMicroseconds( 1000 );
    if( HWSERIAL.available()){
		  Serial.print("HWSERIAL.available("); Serial.print( HWSERIAL.available()); Serial.print(")  ");
    }
    
		if( HWSERIAL.available()) 
		{
			Serial.print(micros());
			Serial.print("[us]     "); 
			Serial.println( HWSERIAL.read() ,HEX);
		}
	}*/
	
	
	T = micros(); 
	// consume the expected response bytes from ATI NetCANOEM board (response ACK 5+2n bytes )
	// the 5 comes from: address, command type, numberofBytes, crc1, crc2;
  //unsigned int n = HWSERIAL.available();
  while ( HWSERIAL.available() < 5 + 2*numPointsToRead ) {
		/* do nothing */
		
		//if( HWSERIAL.available() > n )  {n = HWSERIAL.available(); Serial.print("HWSERIAL.available("); Serial.print( n ); Serial.println(")  ");}
    
		if ( micros() - T > TIMEOUT_US * 6 * 6 ) // longer timeout due to 6x6 matrix reads
		{
		  gATIsensorIsConnected = false;
		  gATIstatus = gATIstatus*0-1;		  
		  Serial.println(" Internal Error: Failed to read a holding register due to timeout");
		  Serial.print("   waiting for "); Serial.println( 5 + 2*numPointsToRead ); 
		  Serial.print(" bytes, got: "); Serial.println( HWSERIAL.available());
		  return; // BREAK OUT OF THIS FUNCTION; No response from  ATI Sensor
		}
	}
	
	
	
	// read stuff into incoming, check if it's ok, then copy over confirmed payload to tempBuffer.
	// Serial.println("Reading expected holding register bytes...");
	// read in everything	
	for ( int i = 0; i < 5+2*numPointsToRead; i++ )
	{
		incoming[ i ] = HWSERIAL.read();
		//tempBuffer[ i ] = '\0'; 
		//Serial.println( incoming[ i ] , HEX);
	}
	
	if ( incoming[ 0 ] != 0x0A ) Serial.println( "Internal error: unexpected address in reading holding registers");
	if ( incoming[ 1 ] != 0x03 ) Serial.println( "Internal error: unexpected functionID in reading holding registers");
	if ( incoming[ 2 ] != 2*numPointsToRead ) Serial.println( "Internal error: unexpected # of bytes in response from holding register");
  	myCRC = CRC16( incoming, 3 + 2*numPointsToRead ); // compute crc on everything except last 2 crc bytes
	if ( ((myCRC >> 8	) != incoming[ 3+2*numPointsToRead     ]) |
	     ((myCRC & 0x00FF) != incoming[ 3+2*numPointsToRead + 1])  )
		 Serial.println("Internal error: failed crc check when reading holding registers");	
	//Serial.print("myCRC, crcH, crcL: ");
	//Serial.println( myCRC >> 8 		, HEX  );  
	//Serial.println( myCRC & 0x00FF 	, HEX  ); 
	//Serial.println( incoming[ 3+2*numPointsToRead     ], HEX); //access 1st CRC byte
	//Serial.println( incoming[ 3+2*numPointsToRead + 1 ], HEX); //access 2nd CRC byte)
	
	// copy over only the payload	
	for ( int i = 0; i < 2*numPointsToRead; i++ )
	{
		tempBuffer[ i ] = incoming[ i + 3 ]; 
		//Serial.println( tempBuffer[ i     ] );
	}
	tempBuffer [ 2*numPointsToRead    ] = '\0';
	tempBuffer [ 2*numPointsToRead + 1] = '\0';
	
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
    
  //  uint8 CalibSerialNumber[8], 
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR, 8/2);  // divide by two bc each register point is 2 bytes (16bit)
  sCalibSerialNumber =  tempBuffer ;
  //Serial.print("sCalibSerialNumber = ");  Serial.println( sCalibSerialNumber );
  //return(-199);
  
  //  uint8 CalibPartNumber[32];
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8)/2, 32/2);  // add previous read size to offset
  sCalibPartNumber = tempBuffer ;
  
  //  uint8 CalibFamilyId[4];
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32)/2, 4/2);  // add previous read size to offset
  sCalibFamilyId = tempBuffer ;
  
  //  uint8 CalibTime[20];
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR +(8+32+4)/2, 20/2);  // add previous read size to offset
  sCalibTime = tempBuffer ;

  ATIcalibrationBanner = "% ATI_CalibSerialNumber = '" + sCalibSerialNumber + "';\r\n" +
						 "% ATI_CalibPartNumber = '" + sCalibPartNumber + "';\r\n" +
						 "% ATI_CalibFamilyId = '" + sCalibFamilyId + "';\r\n" + 
						 "% ATI_CalibTime = '" + sCalibTime + "';\r\n" ;
  //Serial.println( ATIcalibrationBanner );  
  
  //  float BasicMatrix[6][6]; // have to read in 6 floats at a time due to MODBUS length restrictions
  float f2 = 0;
  //float *f_ptr;  // pointer to float  
  char  charArry[4];  // for swaping byte order to float
  for (int i = 0; i<6; i++) 
  {
	ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+i*6*4)/2,  6*4/2);  // floats are 4 bytes, 32bits in ATI doc   
	for (int j = 0; j < 6; j++)
	{	
		/*f = static_cast<float> (tempBuffer[ i + 0 ]<<(8*3) + 
			tempBuffer[ i + 1 ]<<(8*2) +
			tempBuffer[ i + 2 ]<<(8  ) +
			tempBuffer[ i + 3 ] )         ;
				
		f = *(float*) (tempBuffer); // gives same as   memcpy(&f2, &tempBuffer[0], sizeof(float));*/
		
		charArry[3] = tempBuffer[ j*4 + 0 ];
		charArry[2] = tempBuffer[ j*4 + 1 ];
		charArry[1] = tempBuffer[ j*4 + 2 ];
		charArry[0] = tempBuffer[ j*4 + 3 ];		
		memcpy( &f2, charArry, sizeof(float));
		BasicMatrix[i][j] = f2;
		
		/*Serial.print( tempBuffer[ i + 0 ]<<(8*3) , BIN); Serial.print( " " );
		Serial.print( tempBuffer[ i + 1 ]<<(8*2) , BIN); Serial.print( " " );
		Serial.print( tempBuffer[ i + 2 ]<<(8*1) , BIN); Serial.print( " " );
		Serial.print( tempBuffer[ i + 3 ]<<(8*0) , BIN); Serial.print( " -  " );
		
		BasicMatrix[i][j] = tempBuffer[j + i*6 + 3*(j+i*6) + 0] << 8*3 +  
							tempBuffer[j + i*6 + 3*(j+i*6) + 1] << 8*2 +
							tempBuffer[j + i*6 + 3*(j+i*6) + 2] << 8*1 +
							tempBuffer[j + i*6 + 3*(j+i*6) + 3] << 8*0 ;
							*/
		//Serial.print( j + i*6 + 3*(j+i*6)  ) ; Serial.print( ", " )		;	
		
		//Serial.print( " \t ");
		//Serial.print( BasicMatrix[i][j] ); Serial.print( "\t  " )		;
		
		//Serial.print( "  f = " ); Serial.println( f );  Serial.print( "  f2 = " ); Serial.println( f2 ); 
	}
	//Serial.println();
  }
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_BasicMatrix = [\r\n";
  char f_str[32];
  for (int i = 0; i<6; i++) 
  {		
	ATIcalibrationBanner = ATIcalibrationBanner +"%\t";
	for (int j = 0; j < 6; j++)
	{
		dtostrf( BasicMatrix[i][j], 10, 4, f_str);  // min width, precision
		ATIcalibrationBanner = ATIcalibrationBanner + f_str + "\t" ;
	}
	ATIcalibrationBanner = ATIcalibrationBanner +"\r\n";
  }  
  ATIcalibrationBanner = ATIcalibrationBanner + "%                   ]; \r\n";
  
  
  //  uint8 ForceUnits; ...code is a numeric val, e.g. 1 --> Pounds, 2-->Newtons 
  //  uint8 TorqueUnits;
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+6*6*4)/2,  2/2); 
  ForceUnitsCode = (unsigned char) tempBuffer[0];
  TorqueUnitsCode = (unsigned char) tempBuffer[1];
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_ForceUnitsCode = " + String( ForceUnitsCode ) + ";\r\n";  
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_TorqueUnitsCode = " + String( TorqueUnitsCode ) + ";\r\n";
  switch ( ForceUnitsCode ) 
  {
	case 1:  sForceUnits = "Pounds"	 	 ; break;
	case 2:  sForceUnits = "Newtons" 	 ; break;
	case 3:  sForceUnits = "KiloPounds"	 ; break;
	case 4:  sForceUnits = "KiloNewtons" ; break;
	case 5:  sForceUnits = "Kg-force"	 ; break;
	case 6:  sForceUnits = "grams-force" ; break;	
	default: sForceUnits = "UNKNOWN"     ; break;
  }
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_ForceUnits = '" + sForceUnits + "';\r\n";
  switch ( TorqueUnitsCode ) 
  {
	case 1:  sTorqueUnits = "Pound-inch"	  	; break;
	case 2:  sTorqueUnits = "Pound-foot" 	 	; break;
	case 3:  sTorqueUnits = "Newton-meters"	 	; break;
	case 4:  sTorqueUnits = "Newton-millimeters"; break;
	case 5:  sTorqueUnits = "Kg-cm"	 			; break;
	case 6:  sTorqueUnits = "KN-m" 				; break;	
	default: sTorqueUnits = "UNKNOWN"     		; break;
  }
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_TorqueUnits = '" + sTorqueUnits + "';\r\n";
  
  //  float MaxRating[6];
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+6*6*4+2)/2,  4*6/2); 
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_MaxRating = [    ";
  for (int j = 0; j < 6; j++)
  {		
	charArry[3] = tempBuffer[ j*4 + 0 ];
	charArry[2] = tempBuffer[ j*4 + 1 ];
	charArry[1] = tempBuffer[ j*4 + 2 ];
	charArry[0] = tempBuffer[ j*4 + 3 ];		
	memcpy( &f2, charArry, sizeof(float));
	MaxRating[j] = f2;
	dtostrf(MaxRating[j], 4, 1, f_str);  // min width, precision
	ATIcalibrationBanner = ATIcalibrationBanner + f_str + "\t"; 
  }
  ATIcalibrationBanner = ATIcalibrationBanner +" ];\r\n";
  
  
  //  int32 CountsPerForce;
  //  int32 CountsPerTorque;
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+6*6*4+2+4*6)/2,  4*2/2);   
  charArry[3] = tempBuffer[ 0 ];
  charArry[2] = tempBuffer[ 1 ];
  charArry[1] = tempBuffer[ 2 ];
  charArry[0] = tempBuffer[ 3 ];		
  memcpy( &CountsPerForce, charArry, sizeof(CountsPerForce) );
  charArry[3] = tempBuffer[ 4 ];
  charArry[2] = tempBuffer[ 5 ];
  charArry[1] = tempBuffer[ 6 ];
  charArry[0] = tempBuffer[ 7 ];		
  memcpy( &CountsPerTorque, charArry, sizeof(CountsPerTorque) );  
  
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_CountsPerForce  = " + String( CountsPerForce) + "\r\n";
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_CountsPerTorque = " + String(CountsPerTorque) + "\r\n";
  
  // uint16 GageGain[6];
  // uint16 GageOffset[6];
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+6*6*4+2+4*6+4*2)/2,  6*2/2); 
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_GageGain = [    ";
  for (int j = 0; j < 6; j++)
  {		
	unsigned short u16;
	charArry[1] = tempBuffer[ j*2 + 0 ];
	charArry[0] = tempBuffer[ j*2 + 1 ];		
	memcpy( &u16, charArry, sizeof(u16));
	GageGain[j] = u16;	
	ATIcalibrationBanner = ATIcalibrationBanner + String( u16 ) + "\t"; 
/* 	Serial.println( tempBuffer[ j*2 + 0 ], HEX);
	Serial.println( tempBuffer[ j*2 + 1 ], HEX);
	Serial.println( u16, HEX); */
  }
  ATIcalibrationBanner = ATIcalibrationBanner +" ] \r\n";
  ATIreadHoldingRegister( ATI_CALIBRATION_ADDR + (8+32+4+20+6*6*4+2+4*6+4*2+6*2)/2,  6*2/2); 
  ATIcalibrationBanner = ATIcalibrationBanner + "% ATI_GageOffset = [    ";
  for (int j = 0; j < 6; j++)
  {		
	  unsigned short u16;
	  charArry[1] = tempBuffer[ j*2 + 0 ];
	  charArry[0] = tempBuffer[ j*2 + 1 ];		
	  memcpy( &u16, charArry, sizeof(u16));
	  GageOffset[j] = u16;	
	  ATIcalibrationBanner = ATIcalibrationBanner + u16 + "\t"; 
  }
  ATIcalibrationBanner = ATIcalibrationBanner +" ] \r\n";
  
    
  
  Serial.print( ATIcalibrationBanner );  
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
