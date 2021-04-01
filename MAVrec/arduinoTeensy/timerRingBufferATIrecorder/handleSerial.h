////////////////////////////////////////////////////////////////////////////////////
// Serial Input Handler;  locally creates incomingByte (not global one)
void handleSerialInput ( unsigned char incomingByte ) {

  switch ( incomingByte ) {

    // get status
    case 's':
    case 'S':
      Serial.println("% getting ATI status ... ");
      Serial.print("%  status = ");
      if ( ATIgetStatus() == 0 )
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
      if (!amRecording)
      {
        if  ( !ATIstartStreaming() )  Serial.println("% FAILED to start streaming");


        lastRead = 0;
        lastWritten = 0;
        amRecording = true;
        sampleTimer.begin( timerISR, TIMER_PERIOD );
        //Serial.println("%% STATUS: Starting new recording ...");
        Serial.print("% Tare/bias values in use [G0 G1 G2 G3 G4 G5]_bias = [");        
        for ( int g = 0; g <6; g++){
          Serial.print( Gbias[g] ); Serial.print(" "); 
        } Serial.println(" ]");
        Serial.println("% BasicMatrix values in use [6x6] = [");        
        for ( int i = 0; i<6 ; i++){
          Serial.print("% \t");
          for ( int j = 0; j <6; j++){
            Serial.print( BasicMatrix[i][j] , 7); Serial.print(" "); 
          }
          Serial.println("");
        } 
        Serial.println("% ]");
        
        
        t0_ms = millis();
        tPrev_ms = t0_ms;

      }






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
      Serial.flush();
      delay( 100 );
      lastRead = 0;
      lastWritten = 0;
      amRecording = false;
      digitalWrite( 13, HIGH);
      //Serial.println( ATIgetStatus() );
      //if ( !gATIsensorIsConnected )
      //  Serial.println("ATI NOT CONNECTED or Powered Up");

      Serial.println("%                             ... DONE. ");
      break;

    // query (e.g. how many bytes are available on HWSERIAL?
    case '?':
    case 'q':
      Serial.print("% HWSERIAL.available("); Serial.print(HWSERIAL.available());
      Serial.print(") : "); Serial.println( HWSERIAL.read(), HEX );
      break;


    // Tare forces/toorques
    case 't':
    case 'T':
      Serial.print("% EVENT: TARE Command Issued, Now "); //Serial.print(HWSERIAL.available());
      Serial.print(" [G0 G1 G2 G3 G4 G5 ]_bias = ");
      for (int g = 0; g < 6; g++)
        Gbias[g] = gLatestSample.G[g];
      for (int g = 0; g < 6; g++) {
        Serial.print( Gbias[g]); Serial.print(", ");
      }
      Serial.println();
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
