// Helper functions for reading forces from the ATI NetCANOEM board

// Returns full 16 bit status bit;  note that any error sets the last bit (#15).  
// note also that while streaming, that bit is appended into the 'check' field
unsigned char ATIgetStatus( void ); 

// sends the start streaming command to NetCANoem board; it should start streaming data at 7kHz
// dumping the latest information to the global variable "latestSample"
void ATIstartStreaming( void );

// sends the jamming sequence (any 14-byte or longer command) to stop streaming
// assumes streaming is already occuring.  
void ATIstopStreaming( void );

// Global structure for a single streamed sample from ATI NetCANoem board
struct streamingSample {
  unsigned long t_us;  // sample time in micros (as received by teensy)
  short G0;            // order in which ';gauge vector' is received 0,2,4,1,3,5; 
  short G2;            // should be 16 bit signed integer
  short G4; 
  short G1; 
  short G3; 
  short G5; 
  unsigned char Check;// uint8 checksum+status; originally called 'check' in ATI Digital FT Modbus doc    
  //bool checksumOK;    // 
  //bool statusOK;      // error status reported during streaming (last bit of 'check' field)
};

// The global variable:
streamingSample latestSample; 

unsigned long t, t0;  // time keeping variables [us]

// Modbus commands for ATI NetCAN OEM device
const int cmdLengthStatus = 8;
const int cmdLengthStream = 5;
const int cmdLengthStop = 14;
unsigned char atiReadStatusWord[ cmdLengthStatus ] = {
  0x0a, // Slave Address ( DEC 10 )
  0x03, // standard modbus read register command
  0x00, // padding, start of status word register)
  0x1d, //   status word register
  0x00, // padding, 1st address of register
  0x01, //   1st address of register
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
unsigned char atiJammingCommand [ cmdLengthStop ] = {
  0xFF  //any command 14 bytes or longer will do
};
