/* UART Example, any character received on either the real
   serial port, or USB serial (or emulated serial to the
   Arduino Serial Monitor when using non-serial USB types)
   is printed as a message to both ports.

   This example code is in the public domain.
*/

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial1
#include "atiFunctions.h"

// Returns full 16 bit status bit;  note that any error sets the last bit (#15).
// note also that while streaming, that bit is appended into the 'check' field
unsigned char ATIgetStatus( void ) {
  HWSERIAL.write( atiStreamDataCommand, cmdLengthStream );
  HWSERIAL.clear();

  // consume the first cmdLengthStream bytes from ATI NetCANOEM board (response ACK 5 bytes of dataStreamDataCommand)
  while ( HWSERIAL.available() < cmdLengthStream ) {
    /* do nothing */
  }
  for ( int i = 0; i < cmdLengthStream; i++) {
    incomingByte =  HWSERIAL.read();
    // Serial.print( incomingByte, HEX );
  }
  //Serial.println("% Starting ATI Nano17 Streaming ...");
  t0 = micros();

  return;
}

// sends the start streaming command to NetCANoem board; it should start streaming data at 7kHz
// dumping the latest information to the global variable "latestSample"
void ATIstartStreaming( void ){

}

// sends the jamming sequence (any 14-byte or longer command) to stop streaming
// assumes streaming is already occuring.  
void ATIstopStreaming( void ){
}








void setup() {
  Serial.begin( 115200 );
  HWSERIAL.begin( 1250000, SERIAL_8E1 ); // default Baud reate of netcanoem is 1.25Mbps
}







unsigned char inBuffer[13];
unsigned long int t0, t;
void loop() {
  int incomingByte;

  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("Writing to HW serial: ");
    //0x0a 46 55 a3 9d
    //Serial.print(atiStreamDataCommand, HEX);
    //Serial.print( atiStreamDataCommand[0], HEX );
    //Serial.write( atiReadStatusWord, cmdLength );
    //Serial.write( atiStreamDataCommand, 5 );

    Serial.println(" ");
    //HWSERIAL.print("USB received:");
    //HWSERIAL.println(incomingByte, DEC);
    //HWSERIAL.write( atiReadStatusWord, cmdLength );
    HWSERIAL.write( atiStreamDataCommand, 5 );
    //HWSERIAL.clear();

    // consume the first 5 bytes from ATI NetCANOEM board (response ACK 5 bytes of ataStreamDataCommand)
    while ( HWSERIAL.available() < 5 ) {
      /* do nothing */
    }
    for ( int i = 0; i < 5; i++) {
      incomingByte =  HWSERIAL.read();
      // Serial.print( incomingByte, HEX );
    }
    Serial.println("% Starting ATI Nano17 Streaming ...");
    t0 = micros();
  }

  //incomingByte = HWSERIAL.available();
  if ( HWSERIAL.available() > 0) {
  
    // get a time stamp
    t = micros()-t0; 

    // wait for the remaining bytes to come in for this one packet
    while ( HWSERIAL.available() < 13 ) 
      {/* do nothing */}
    
    for ( int i = 0; i < 13; i++) {
      inBuffer[i] = HWSERIAL.read();
    }
    //Serial.print( HWSERIAL.available() );
    //Serial.print( " bytesAvailable to read (after reading 13).  sampleTime [us]: " );
    Serial.println( t );
    Serial.flush();
    

    /*  t = micros();
      //while( HWSERIAL.available() ){
      Serial.print( t, 10 );
      Serial.print( " % us\t bytesAvailable: " );
      Serial.print( incomingByte );
      Serial.print( " byte: ");

      while( HWSERIAL.available() ){
        incomingByte = HWSERIAL.read();
        if (incomingByte < 16) Serial.print('0');
        Serial.print(incomingByte , HEX  );
      }
      Serial.println("");
      t0 = t;*/

  }

  /*
    if (HWSERIAL.available() >= 13) { // streaming data is 13 bytes long)

    for( int i = 0; i < 13; i++) {
      inBuffer[i] = HWSERIAL.read();

    for( int i = 0; i < 13; i++)
      Serial.print(  inBuffer[i], HEX );

    incomingByte = HWSERIAL.available();
    Serial.print(" UART read 13 bytes; UART has [");
    Serial.print( incomingByte );
    Serial.print("] bytes available after reading 13 bytes already. " );
    /*while( HWSERIAL.available() ) {
      incomingByte = HWSERIAL.read();
      Serial.print(incomingByte, HEX);
    }
    Serial.println( "   DONE.");
    //HWSERIAL.print("UART received:");
    //HWSERIAL.println(incomingByte, HEX);
    }*/

}
