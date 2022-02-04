
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//      USER SETTINGS        (edit these for program defaults)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boolean useMicroScribe = true;
boolean useATInano17   = true;
boolean useVideo1      = true;    // must be true to have any video recording.  If false, no video recording whatsoever.
boolean useVideo2      = false;   // only set true if useVideo1 is *ALSO* true.  

// Camera Defaults         // The O.S. provides a list of connected cameras that are available ( 0, 1, 2,  etc.  )
                           // if you have cam 0 and 1 plugged in, then unplug cam 0; cam 1 becomes cam 0 in the OS.
                           // or if you have a built-in webcam (defaults to 0) and want to add one or two other cameras (1 and 2)
                           // these settings below allow you to assign an arbitrary O.S.-enumerated cam to Video1 and Video2
int vid1Selection = 0;     // 0 means 1st OS-listed camera attached, 1 is 2nd OS-listed camera attached, etc. to video1 recoring
int vid2Selection = 1;     // these selections only activate if useVideo1 (and useVideo2) are true

// RECORDING STORAGE LOCATION AND FILENAME DEFAULTS ...
// Where all recorded data are stored.  NOTE: this should be a location free of interference from Antivirus scanning, cloud syncing 
//                                      (google or dropbox), or Windows defender monitoring to ensure highest-quality recording.  
// Once recording is complete, then copy files over to a central repository or a cloud-sync'ed folder. 
String fileStub = "testFile";  // initial filename that appears
String fileTime = "YY.MM.DD_hh.mm.ss"; // getTimestamp();
String fileFolder = "C:/tissueCapture/recordedData/" ;
String fileName = fileStub + "_" + fileTime;

// Enter COM Port that ATI to USB box is using (contains Teensy 4.0 running Arduino).  
String comPort ="";    // Use this to hard-code the COM port that USB Teensy 4.0 Arduino is on. 
                       //    ""    -- will attatch to last serial port (something MUST be plugged in)
                       //   "COM9" -- will attach to COM9, expecting arduino to be there. 
int comBaudrate = 384000;
int serialTimeout = 3000; // ms to wait for serial response



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  END USER SETTINGS
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


import g4p_controls.*;
import processing.video.*;
import com.hamoid.*;
import processing.serial.*;
import java.util.Arrays;
import java.util.List;
import org.multiply.processing.TimedEventGenerator;
import java.lang.*;



private TimedEventGenerator microscribeTimedEventGenerator;
private int lastMicroscribeMillis = 0;

//String myPath = myPath;
//NativeLibrary.addSearchPath("armdll64.dll", "test");
int Wvid = 210;        // vid display width
int Hvid = Wvid*3/4;   // vid display height

Capture vid1;
Capture vid2;
VideoExport videoExport1, videoExport2;

int numCameras = 0 ;       // number of cams users chooses to record from (updated based on (useVideo1 and useVideo2) 
int numCamsAvailable = -1; // total number of cameras available (gets updated)
String [] cameras;
String [] videoOptions;
int samplePeriod  = 100;   // ms.  On samle every samplePeriod seconds
Serial sPort;  
String sData; // serial data (timestamps plus ATI )
String atiInfo; // holds info extracted from ATI (or nothing if not connected)
String mData; // microscribe data string
String tempString;
int numSerialPorts;

boolean ATIinitialized; 
boolean amRecording;
boolean amReady;

PrintWriter motionLog, forcesLog, notesLog;
String motionFilenameEnd = "_motion.txt";
String forcesFilenameEnd = "_forces.txt";



void captureEvent(Capture video) {
   video.read();
}

int FPS = 30;







////////////////////////////////////////////////////////////
//     SETUP (runs once)                             ///////
////////////////////////////////////////////////////////////
int t0_ms, t_ms;  // time keeping variable for when recording was started (ms, for microscribe);
public void setup(){
  
  size(800, 600 );
  createGUI();
  customGUI();
  //setTitle("MAVrec");
  if (useVideo1 == false ) {useVideo2 = false;} // set useVideo2 to false (override) if useVideo1 is false.
  if (useVideo1)  {
    numCameras = numCameras + 1;
    if (useVideo2)  numCameras = numCameras + 1;
  }
  
  microscribeTimedEventGenerator = new TimedEventGenerator(this);
  microscribeTimedEventGenerator.setIntervalMs(30);
  
  if (useMicroScribe) {
    //println( System.getProperties() );
    println( System.getProperty("jna.library.path" ) );  // this is where the dll's need to be  .addSearchPath("armdll64.dll", "test");
    println( " My Local Java path:");
    println( System.getProperty("java.library.path"));//"java.class.path"));//"user.dir") );
    println("myClibrary.ArmStart: ");  
    println( myCLibrary.INSTANCE.ArmStart( null )) ;
    println( "myClibrary.ArmConnect(0,0): ");  // ARM_SUCCESS == 0
    println( myCLibrary.INSTANCE.ArmConnect(0, null) ); // ARM_SUCCESS == 0 
    println( "myClibrary.ArmSetUpdateEx( , ): ");  // ARM_SUCCESS == 0
    println( myCLibrary.INSTANCE.ArmSetUpdateEx((int)0x0010, 20) ); //   public static final int ARM_FULL = (int)0x0010;
    
    //println( "myClibrary.ArmgetTipPositions: ");  // ARM_SUCCESS == 0
    //myCLibrary.length_3D myTipPos = new myCLibrary.length_3D();  
    //println("\t before: x, y, z = " +myTipPos.x +", " +myTipPos.y + ", " +myTipPos.z );
    //println( myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  ) );
    //println("\t after: x, y, z = " +myTipPos.x +", " +myTipPos.y + ", " +myTipPos.z );
    
    
    //myCLibrary.INSTANCE.ArmDisconnect();
    //myCLibrary.INSTANCE.ArmEnd(  ) ;
    //println("myClibrary.Disconnected and .ArmEnded. ");
    
    println( System.getProperties() );
  }
  
  if (useATInano17) {
    
    // Establish connection to arduino if serial port is available
    String [] serialList = Serial.list();  
    numSerialPorts = serialList.length;
    println("Available serial ports: [" + numSerialPorts + "] " );  printArray( serialList );  
    if ( serialList.length < 1 ) {
        println("NO ATInano/ARDUINO SERIAL DEVICES PLUGGED IN!   ...ABORTING");
        useATInano17 = false;
        exit();
        return;
    }
    
    if ( comPort == "" ) {
        println("Attempting to find and connect on last serial port listed ..."  );              
        println("Last serial port:  " + Serial.list()[ serialList.length - 1 ] );  
        comPort = Serial.list()[ serialList.length - 1 ];
    } else {
      println("Attempting to connect on user-provided comPort: " + comPort );    
    }
      
    
    //print("sPort (before open): ");  println( sPort );
    //print("sPort == null: ");  println( sPort == null );
    try {
     
     sPort = new Serial(this, comPort, comBaudrate );
     //sPort = new Serial(this, comPort, comBaudrate );
     // don’t read the serial buffer until a new line char
     // should use Serial.println("") in arduino code
     //sPort.bufferUntil('\n');    // this should avoid breaking packets between \n characters (?)
    } finally {} 
    sPort.write('x');  // send jamming (stop) command in case ATI is dumping force samples
    sPort.clear();
    sPort.write('I');  // initialize ATI board (need this before collecting force data to get correct calibration)
    //print("sPort (after open): ");  println( sPort );
    //print("sPort == null: ");  println( sPort == null );
    t0_ms = millis();
    
    println("Initializing ATI nano 17...\r\n");
    while( sPort.available() < 500 && ( (millis() - t0_ms) < serialTimeout ) ) 
      delay(1);/*do nothing*/  
    //print("sPort.available(): ");  println( sPort.available() );
    sData = "";
    t0_ms = millis();
    ATIinitialized = false;
    while( ATIinitialized == false && ( (millis() - t0_ms) < serialTimeout ))
    {
      if( sPort.available() > 0)
        sData = sData + sPort.readString();      
      if( sData.indexOf("ATI CONNECTED") != -1){
        ATIinitialized = true;      
      }
    }  
    if (ATIinitialized == true) {
      atiInfo = sData;
    } else {
      atiInfo = "% no ATI or Arduino connected; check connections and restart gui. \r\n";
      tempString = atiInfo; // feeds this to the GUI
    }
  } else { // if useATInano17
    atiInfo = "% ATI forces NOT IN USE. \r\n";
    tempString = atiInfo; // feeds this to the GUI
    sData = "% ATI not in use.";
    t0_ms = millis();
    ATIinitialized = false;
  }
  //surface.setResizable(true);
  
  // Important: set the output-framerate (sketch) 
  // equal to the input-framerate (webcam).
  // By default Processing run at 60 fps. If the webcam runs at
  // 30 fps, we save each frame twice and the result plays back 
  // at half the speed.
  frameRate(FPS);  
  // Set up cameras, find out how many there are; hook up one to start with.
  println("\r\n\r\nUSB cameras listed by system:");  
  cameras = Capture.list();
  printArray(cameras);
  numCamsAvailable = cameras.length;
      
  if ( useVideo1 && (numCamsAvailable > 0) ){
    println("Attaching Video1 source.  Total found: " + cameras.length ); 
    vid1 = new Capture(this, cameras[vid1Selection]  );
    vid1.stop();
    vid1.start();
  } else { 
    println("No cameras found."); /* no cameras found */
    numCameras = 0;
  }  
  if (  useVideo2 && (numCamsAvailable > 1) ){ // attach a second camera if it exists
    println("Attaching Video2 source." );
    vid2 = new Capture(this, cameras[vid2Selection]  );  
    vid2.stop();
    vid2.start();
  }
  
  if(useATInano17){
    sPort.write('x'); sPort.clear(); // make sure it gets stopped in case it was left running
    sPort.write('b');  // Send the 'b' character to Arduino to 'begin recording'      
  }
  
  t0_ms = millis();
  amRecording = false; 
  amReady = true;
   
  
}


//if (useMicroScribe){
  myCLibrary.length_3D myTipPos = new myCLibrary.length_3D();
  myCLibrary.length_3D myTipUnitDirection = new myCLibrary.length_3D();  
  myCLibrary.length_3D myTipRPY = new myCLibrary.length_3D();
//}

////////////////////////////////////////////////////////////
//     MAIN LOOP (runs until closed)                 ///////
////////////////////////////////////////////////////////////
public void draw(){
  
  
  // read cameras if needed
  if ( useVideo1 && (numCameras > 0) && vid1.available()) {
    vid1.read();
  }
  if (useVideo2 && (numCameras > 1) && vid2.available()) {
    vid2.read();
  }
  
  background(217);
  text("Saving files named ...\r\n  " + 
    fileName + "_forces.txt" + "\r\n  " + 
    fileName + "_motion.txt" + "\r\n  " +
    fileName + "_vid1.mp4"  + "\r\n  " +
    fileName + "_vid2.mp4"  + "\r\n  " +
    fileName + "_notes.txt\r\n" +  
    "In folder ...\r\n  " + fileFolder,
    15, 90);
 
  
  
  // Updates for camera changes
  cameras = Capture.list();
  numCamsAvailable = cameras.length;
  videoOptions = splice( cameras, "No Recording", 0 );
  
  if ( cameras.length < numCamsAvailable ) {
    println("Lost camera(s), new camera count ... " + cameras.length ); 
    printArray( cameras );
    numCameras = cameras.length;    
    
  } else if (cameras.length > numCamsAvailable ){
    println("Added camera(s), new camera count ... " + cameras.length ); 
    printArray( cameras );
    numCameras = cameras.length;
    
  } else { /* do nothing */}
  
  text( "Cameras available  (qty. "+ numCamsAvailable +") ... \r\n", 15, 210);
  text( join(cameras, "\r\n") + "\r\n", 20, 225);
  
  String txtVid1, txtVid2;
  if ( useVideo1 ) { 
    txtVid1 = "Video 1 - source: [" + vid1Selection + "] " + cameras[vid1Selection]; 
    labelVideo1source.setText( cameras[vid1Selection] ) ;
  } else {  txtVid1 = "Video 1 - not in use";  }
  if ( useVideo2 ) { 
    txtVid2 = "Video 2 - source: [" + vid2Selection + "] " + cameras[vid2Selection];
    labelVideo2source.setText( cameras[vid2Selection] ) ;
  }  else {   txtVid2 = "Video 2 - not in use"; }
  text( txtVid1 , 15, 270 );
  text( txtVid2 , 15, 285 );
  
  text( "Latest MicroScribe motion  [ t(ms)    x y z (cm)   r p y (deg) ] ...  \r\n   " + mData , 15, 360); 
  /*sData = sData.substring(0, sData.length()-1) + "   " + 
        nf(myTipPos.x,0,5) + " " + nf(myTipPos.y,0,5) + " " +nf(myTipPos.z,0,5) +
        nf(myTipRPY.x,0,5) + " " + nf(myTipRPY.y,0,5) + " " + nf(myTipRPY.z,0,5) + 
        "  " + (millis() - t0_ms);*/
    //println(  );
    //println("\t after: x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
    //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );
  
  if(useATInano17) {
    if (sPort == null) {    
      sData = "% ERROR: no arduino hardware connected on expected USB/Serial port. ";
      tempString = sData;
      println( sData );
    }
    if( tempString.indexOf("  %") > 0)
      tempString = tempString.substring(0, tempString.indexOf("  %"));
  } else {
    tempString = sData;
  }
  text( "Latest forces data [ t(ms) Fx Fy Fz (N) Tx Ty Tz(mNm) ] ... \r\n   " + tempString   , 15, 315);  
  
  
  

  
  //myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );  
  //myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
  //println(  );
  //println("\t after: x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
  //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );
  
  //text( "  pose  x,  y,  z  [cm]: "  + nf(myTipPos.x,0,2) + ", " + nf(myTipPos.y,0,2) + ", " +nf(myTipPos.z,0,2) +
  //  "\r\n  roll, pitch, yaw [deg]: " + nf(myTipRPY.x,0,2) + ", " + nf(myTipRPY.y,0,2) + ", " + nf(myTipRPY.z,0,2)   , 20, 280);
  
  
  // Here we don't save what we see on the display,
  // but the webcam input.
  if( useVideo1 ) {
    image(vid1, 10                , height-Hvid - 10      , Wvid, Hvid );
  } else {
    fill(255);
    noStroke();
    rect(       10                , height-Hvid - 10      , Wvid, Hvid );
  }
  if( useVideo2 ) {
    image(vid2, 10 + Wvid + 10    , height-Hvid - 10      , Wvid, Hvid );
  } else {
    fill(255);    
    noStroke();
    rect(       10 + Wvid + 10    , height-Hvid - 10      , Wvid, Hvid );
  }
  
  if (amRecording) {
    //motionLog.print( sData ); // this should be done in the time and serialEvent handlers below
    if ( useVideo1 ) videoExport1.saveFrame();
    if ( useVideo2 ) videoExport2.saveFrame();
  }
}

void keyPressed() {
  if (key == 'q') {
    //videoExport.endMovie();
    stopRecording();
    if ( useVideo1 ) {  vid1.stop();  }
    if ( useVideo2 ) {  vid2.stop();  }
    exit();
  }
}

// Use this method to add additiconal statements
// to customise the GUI controls
public void customGUI(){
  
  textfieldFilename.setText( fileStub );
  //videoOptions = splice( Capture.list(), "No Recording", 0  );
  //if ( useVideo1 ) { labelVideo1source.setText("[" + vid1Selection + "] ") ; }
  //if ( useVideo2 ) { labelVideo2source.setText("[" + vid1Selection + "] ") ; }
  
    
  //dropListCam1.setItems( videoOptions, vid1Selection );
  //dropListCam2.setItems( videoOptions, vid2Selection );
  
  //println( height );
}

void onTimerEvent() {
  t_ms = millis() - t0_ms;  // time in ms since begin recording got started 
  lastMicroscribeMillis = t_ms; //millisDiff + lastMicroscribeMillis;  
  
  
  if(useMicroScribe){
    myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );    
    myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
    //System.out.println("Got a timer event at " + millis() + "ms (" + millisDiff + ")!");
    //setRandomFillAndStroke();
    //ellipse(random(width), random(height), random(100), random(100));
    //println("\t x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
    //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );
    //sData = lastMicroscribeMillis + ",  " + sPort.readString();
    mData =  lastMicroscribeMillis + ",   " + 
          nfs(myTipPos.x, 0, 3) + ", " + nfs(myTipPos.y, 0, 3) + ", " + nfs(myTipPos.z, 0, 3) + ",   " +
          nfs(myTipRPY.x, 0, 3) + ", " + nfs(myTipRPY.y, 0, 3) + ", " + nfs(myTipRPY.z, 0, 3) + ",    "  
          + ((millis() - t0_ms ) ) + "\r\n";   
    
  } else {
    mData = "% MicroScribe not in use %     " + lastMicroscribeMillis + ",    " + ((millis() - t0_ms ) ) +",\r\n";
  }
  
  if ( amRecording ) {    
      motionLog.print( mData );
      //forcesLog.print( sData );
  } 

  if ( useATInano17 ){
    /*if (sPort == null) {    
      sData = "% ERROR: no arduino hardware connected on expected USB/Serial port. ";
      tempString = sData;
      println( sData );
      // get arduino data if it is available (should  be),
    } else*/
    if (sPort.available() > 0) {    
      // the moment it arrives get the microscribe data and a timestamp
      //myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );    
      //myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
      // microscribeTime = ;    
      sData =  sPort.readString();
      //while (sPort.available() > 0)
      //sData =  sPort.readString();
      //sData = sData + "% lastMicroscribeTime(above samples): " + lastMicroscribeMillis; 
      //sData =  sPort.readStringUntil('\n');    
      //sData =  sPort.readString(); 
      //print("serial data: " + sData);
      int i;
      i = sData.indexOf( '\n' );
      if (i <= 0 )
        tempString = "% Arduino-ATI NetCANOEM board providing no data or not connected\r\n";
      else
        tempString = sData.substring(0, i);// sData.indexOf( '\n' ));
          
      // usually, the last thing in sData should  be a '\n'; check that.  
      if( '\n' != sData.charAt( sData.length()-1 ) ) 
      {
        print("last sData char not \\n !!  (check data file for integrity) \r\n");
      } else if ( amRecording )  
      {
        forcesLog.print( sData.substring( 0, sData.length()-2 ) ); // log everthing but last newline 
        forcesLog.print( "   % microscribeSampleTime = " + lastMicroscribeMillis + "\r\n");
      }  
    }
    
    
  }  else { //if ( useATInano17 )
    if ( amRecording )  forcesLog.println( sData ); // log everthing even if  ATI not in use   
  }
}
/*
void serialEvent(Serial p) { 
  sData = lastMicroscribeMillis + ",  " +p.readString();
   
  if ( amRecording ){     
    forcesLog.print( sData );
  }
  
} */


void exit() {
  println("exiting");
  if (useATInano17)  sPort.write('x'); 
  super.exit();
}
