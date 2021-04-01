////////////////////////////////////////////////////////////////////////////////////
// Serial Input Handler;  locally creates incomingByte (not global one) 
void handleMySerialInput ( unsigned char incomingByte ) {
  
    switch ( incomingByte ){
    
    // get status
    case 's':
    case 'S':
      Serial.println("% getting ATI status ... "); 
      Serial.print("%  status = ");
      if( ATIgetStatus() == 0 )
        Serial.println(" All OK (0) ");
      else {
        Serial.print(" ERROR ( status >= 1) : "); 
        Serial.println( gATIstatus, BIN );
      }
      if ( !gATIsensorIsConnected )
        Serial.println("%  ATI NOT CONNECTED or Powered Up");
      //Serial.println("% ... DONE. "); 
    break;

    
    // Begin streaming
    case 'b':
    case 'B':
      //Serial.println("% Begin Streaming ... "); 
      if  ( !ATIstartStreaming() )  Serial.println("% FAILED to start streaming");
      
      t0_ms = millis();
      tPrev_ms = t0_ms;
      sampleTimer.begin( timerISR, samplePeriod );

      if ( !gATIsensorIsConnected )
        Serial.println("% ATI is **NOT** Connecteded or Powered Up/");
      //Serial.println("                ... DONE. "); 
    break;
    
    // stop streaming (jam)
    case 'x':
    case 'X':
      Serial.println("% STOPPING Streaming / JAMMING ... "); 
      sampleTimer.end(); 
      ATIstopStreaming(); //delayMicroseconds( TIMEOUT_US );
      
      //Serial.println( ATIgetStatus() );
      //if ( !gATIsensorIsConnected )
      //  Serial.println("ATI NOT CONNECTED or Powered Up");
      
      Serial.println("%                             ... DONE. "); 
    break;

    // query (e.g. how many bytes are available on HWSERIAL?
    case '?':
    case 'q':
      Serial.print("% HWSERIAL.available("); Serial.print(HWSERIAL.available());
      Serial.print(") : ");Serial.println( HWSERIAL.read(), HEX );
    break;

    //  clear serial
    case 'c':
    case 'C':
      HWSERIAL.clear();
      Serial.print("% HWSERIAL.clear(); HWSERIAL.available() : ");
      Serial.println( HWSERIAL.available() );
    break;

    // Initialize netcanoem board (calibration, etc.)
    case 'i':
    case 'I':
    {
      Serial.println("%% initializing ATI netCANoem board ... "); 
      bool st = ATIinitialize();
      //Serial.print("%%  return status = "); Serial.println( st );
      Serial.print("% ATI CONNECTED.  Internal status =  "); Serial.println( st );
      /*
      Serial.print("  status = ");
      if( ATIgetStatus() == 0 )
        Serial.print(" All OK (0) ");
      else {
        Serial.print(" ERROR ( status >= 1) : "); 
        Serial.print( gATIstatus, BIN );
      }
      if ( !gATIsensorIsConnected )
        Serial.println("ATI NOT CONNECTED or Powered Up");
      Serial.println(" ... DONE. "); 
      */
      break;
    }
      
    default:  
      Serial.println("Unknown command, ignoring. ");  
    break;
    
    } // of switch ( incomingByte )
}
