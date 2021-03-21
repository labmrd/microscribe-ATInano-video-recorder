// Use a hardware timer interrupt and ring buffer to reliable read data from ATI NETCAN OEM board 
// and stream to serial.   A timer interrupt (timer1 16-bit timer) is used  dumps data to serial port.




#define LED_PIN      13
                            // Sample period of timer interupt service routine (ISR) in microseconds
#define   TIMER_PERIOD 10000 // e.g. set a timer of length 10000 microseconds (or 0.1 sec - or 10Hz) 
//#define TIMER_PERIOD 1000   // 1000us = 1ms =>  1 KHz update rate.

// Create an IntervalTimer object 
IntervalTimer myTimer;

// A ring buffer.  Allows you to write data VERY quickly inside an interrupt.  Can read it later in chunks
// to write to ring buffer, write myDataSample to: iWrite = (currentIndex+1) % BUF_LENGTH location along BUF_LENGTH; 
//                          THEN, WHEN DONE WRITING: set lastWritten = iWrite
// to read from ring bufffer, read myDataSample from: iRead = (currentIndex+1) % BUF_LENGTH location; 
//                          THEN, WHEN DONE READING: set lastread = iRead
#define BUF_LENGTH     1000  // number of entries in ring buffer before overflow
#define BUF_DIMENSION  1     // number of items per buffer location  (e.g.  dataSamples per buffer location)
#define BANNER         "% t[ms]    Fx[N]    Fy[N]   Fz[N]   Tx[Nm]  Ty[Nm]  Tz[Nm]"
struct myDataSample {
  unsigned long int t;  // time of sample (milli sec)  
  float Fx;    // 
  float Fy;    //
  float Fz;    // 
  float Tx;    //
  float Ty;    // 
  float Tz;    //    
  
} ;
myDataSample ringBuffer[BUF_LENGTH][BUF_DIMENSION];


bool ringBufferOverflow;
volatile unsigned long int lastWritten;
volatile unsigned long int lastRead;
volatile unsigned long int iWrite, iRead; // index values for bookeeping

volatile static unsigned long int t0 ;//= micros(); // only gets initialized the first time function is called

void setup() {
  pinMode(13, OUTPUT);

  
  lastRead = 0;
  lastWritten = 0;
  
  Serial.begin(115200);
  //Serial.println(BANNER);
  Serial.flush();

  //Timer1.initialize( TIMER_PERIOD ); //  
  //Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  //myTimer.begin( timerIsr, TIMER_PERIOD );
  

  
}



void loop() {
  
   // if there are any serial commands, process them
   if (Serial.available() > 0) {
    int ch = Serial.read(); // get the first char.
    
    if (ch == 'b' || (ch == 'B')) {        // BEGIN data streaming
      
      t0 = millis();
      myTimer.begin( timerIsr, TIMER_PERIOD );
      Serial.println("%% STATUS: Starting new recording ...");
      
    } else if (ch == 'X' || (ch == 'x')) 
    { 
      // STOP/KILL  data streaming
      myTimer.end();
      delay( 100 );
      Serial.println("%% STATUS: ending data stream");
      
    }

    
   }
    
    // Empty entire ringBuffer to Serial...
    // to read from ring bufffer, read myDataSample from: iRead = (currentIndex+1) % BUF_LENGTH location; 
    //                          THEN, WHEN DONE READING: set lastread = iRead
    int iToRead = (lastWritten - lastRead) % BUF_LENGTH;
    for ( int i = 0; i < iToRead; i++ ) {
      iRead = (lastRead+1) % BUF_LENGTH;  // start reading next entry
      //enc = ringBuffer[iRead][0].t

      // do math and unit conversion
      //ringBuffer[iRead][0].pos = ringBuffer[iRead][0].enc * ticksToDegrees;
      //ringBuffer[iRead][0].vel = ringBuffer[iRead][0].pos - ringBuffer[(iRead-1)%BUF_LENGTH][0].pos;
      //ringBuffer[iRead][0].acc = ringBuffer[iRead][0].vel - ringBuffer[(iRead-1)%BUF_LENGTH][0].vel;

      // output to serial as ASCII
      Serial.print( ringBuffer[iRead][0].t );
      Serial.print( "    " );  
      Serial.print( ringBuffer[iRead][0].Fx );
      Serial.print( "  " );
      Serial.print( ringBuffer[iRead][0].Fy );
      Serial.print( "  " ); 
      Serial.print( ringBuffer[iRead][0].Fz );
      Serial.print( "    " ); 
      Serial.print( ringBuffer[iRead][0].Tx );
      Serial.print( "  " ); 
      Serial.print( ringBuffer[iRead][0].Ty );
      Serial.print( "  " ); 
      Serial.println( ringBuffer[iRead][0].Tz );
      //Serial.println( " " );
      
      lastRead = iRead; // officially done reading the iRead entry
    }
    Serial.flush();



    
    
  
}


/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------
void timerIsr()
{
    // Toggle LED
    digitalWrite( LED_PIN, digitalRead( 13 ) ^ 1 );


    volatile static long int pos;
    volatile static long int pos_prev;

    // Get most recent data, write it to ring buffer.
    // to write to ring buffer, write myDataSample to: iWrite = (currentIndex+1) % BUF_LENGTH location along BUF_LENGTH; 
    //                          THEN, WHEN DONE WRITING: set lastWritten = iWrite
    iWrite = (lastWritten+1) % BUF_LENGTH;        // start writing to next entry
    ringBuffer[iWrite][0].t     = millis()-t0;    // get current time in millis sec
    ringBuffer[iWrite][0].Fx    = analogRead( A1 );  // get values from NetCAN OEM stream
    ringBuffer[iWrite][0].Fy    = analogRead( A2 );  // get values from NetCAN OEM stream
    ringBuffer[iWrite][0].Fz    = analogRead( A3 );  // get values from NetCAN OEM stream
    ringBuffer[iWrite][0].Tx    = analogRead( A4 );  // get values from NetCAN OEM stream
    ringBuffer[iWrite][0].Ty    = analogRead( A5 );  // get values from NetCAN OEM stream
    ringBuffer[iWrite][0].Tz    = analogRead( A6 );  // get values from NetCAN OEM stream
    lastWritten = iWrite;                       // officially done writing the "next entry"    

    

}
