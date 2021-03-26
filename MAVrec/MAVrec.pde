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

//String comPort = "COM4";
String comPort = Serial.list()[Serial.list().length-1];   
int  comBaudrate = 115200;


// Camera Defaults
int vid1Selection = 1;     // 1 means 1st camera attached, 2 is 2nd camera attached, etc. to video1 recoring
int vid2Selection = 0;     // 0 means no camera selected for video2 recording







int Wvid = 210;        // vid display width
int Hvid = Wvid*3/4;   // vid display height
String fileStub = "testFile";  // initial filename that appears
String fileTime = "YY.MM.DD_hh.mm.ss"; // getTimestamp();
String fileFolder = "C:/tissueCapture/data/" ;
String fileName = fileStub + "_" + fileTime;
Capture vid1;
Capture vid2;
VideoExport videoExport1, videoExport2;
int numCameras = 1;        // number of cams users chooses to record from 
int numCamsAvailable = -1; // total number of cameras available (gets updated)
String [] cameras;
String [] videoOptions;
int samplePeriod  = 100;   // ms.  On samle every samplePeriod seconds
Serial sPort;  
String sData; // serial data (timestamps plus ATI 
String mData; // microscribe data string
String tempString;
int numSerialPorts;

boolean amRecording;
boolean amReady;

PrintWriter motionLog, forcesLog, notesLog;
String motionFilenameEnd = "_motion.txt";
String forcesFilenameEnd = "_forces.txt";

void captureEvent(Capture video) {
   video.read();
}

int FPS = 30;





public interface myCLibrary extends Library {
   //Native.addSearchPath( "armdll64.dll" , "C:\\tissueCapture\\" );
   myCLibrary INSTANCE = (myCLibrary)  Native.load("armdll64.dll", myCLibrary.class); // should be in ./code/ dir (on class path)
  //addSearchPath("armdll64.dll", "test");
             //void printf(String format, Object... args);
 
 
   public static class HWND extends PointerType {
      public HWND(Pointer address) {
        super(address);
      }
      public HWND() {
        super();
      }
     }
  //@FieldOrder({ "x", "y", "z"})
  public static class length_3D extends Structure /*<length_3D, length_3D.ByValue, length_3D.ByReference >*/{
  /** C type : length */
  public float x;
  /** C type : length */
  public float y;
  /** C type : length */
  public float z;
  public length_3D() {
    super();
  }
  protected List getFieldOrder() {
         return Arrays.asList("x", "y", "z");
     }

  //protected List<? > getFieldOrder() {
  //  return Arrays.asList("x", "y", "z");
  //}
  /**
   * @param x C type : length<br>
   * @param y C type : length<br>
   * @param z C type : length
   */
  public length_3D(float x, float y, float z) {
    super();
    this.x = x;
    this.y = y;
    this.z = z;
  }
  //public length_3D(Pointer peer) {
  //  super(peer);
  //}
  protected ByReference newByReference() { return new ByReference(); }
  //protected ByValue newByValue() { return new ByValue(); }
  protected length_3D newInstance() { return new length_3D(); }
  //public static length_3D[] newArray(int arrayLength) {
  //  return Structure.newArray(length_3D.class, arrayLength);
  //}
  public static class ByReference extends length_3D implements Structure.ByReference {
    
  };
  //public static class ByValue extends length_3D implements Structure.ByValue {
    
  //};
  }
  int ArmStart(myCLibrary.HWND hwndParent);
  void ArmEnd();
  // Original signature : <code>int ArmConnect(int, long)</code><br>
  // <i>native declaration : line 452</i>  
  int ArmConnect(int port, NativeLong baud);
  // Original signature : <code>void ArmDisconnect()</code><br>
  // <i>native declaration : line 454</i>   
  void ArmDisconnect();
  // Original signature : <code>int ArmSetUpdateEx(int, UINT)</code><br>
  //<i>native declaration : line 468</i>  
  int ArmSetUpdateEx(int type, int minUpdatePeriodms);
   // Original signature : <code>int ArmGetTipPosition(length_3D*)</code><br>
   // <i>native declaration : line 470</i>
  int ArmGetTipPosition(length_3D pPosition);
  // Original signature : <code>int ArmGetTipOrientation(angle_3D*)</code><br>
  //<i>native declaration : line 471</i>  
  int ArmGetTipOrientation(length_3D pAngles); // angle about x, y, z
  // Original signature : <code>int ArmGetTipOrientationUnitVector(angle_3D*)</code><br>
  // <i>native declaration : line 472</i>  
  int ArmGetTipOrientationUnitVector(length_3D pOrientationUnitVector); // tip orientation vector in x,y,z
  
  
  
  public static final int ARM_SUCCESS = (int)0;
  public static final int ARM_FULL = (int)0x0010;
}



////////////////////////////////////////////////////////////
//     SETUP (runs once)                             ///////
////////////////////////////////////////////////////////////
int t0_ms, t_ms;  // time keeping variable for when recording was started (ms, for microscribe);
public void setup(){
  
  size(800, 600 );
  createGUI();
  customGUI();
  
  microscribeTimedEventGenerator = new TimedEventGenerator(this);
  microscribeTimedEventGenerator.setIntervalMs(30);
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
  
  
  
  // Establish connection to arduino if serial port is available
  String [] serialList = Serial.list();
  numSerialPorts = serialList.length;
  println("Available serial ports: ");  printArray( serialList );  
  println("Last serial port:  " + Serial.list()[ serialList.length - 1 ] );
  
  try {
   sPort = new Serial(this, comPort, comBaudrate );
   //sPort = new Serial(this, comPort, comBaudrate );
   // don’t read the serial buffer until a new line char
   // should use Serial.println("") in arduino code
   //sPort.bufferUntil('\n');    // this should avoid breaking packets between \n characters (?)
  } finally {} 
  
  
  
  //surface.setResizable(true);
  
  // Important: set the output-framerate (sketch) 
  // equal to the input-framerate (webcam).
  // By default Processing run at 60 fps. If the webcam runs at
  // 30 fps, we save each frame twice and the result plays back 
  // at half the speed.
  frameRate(FPS);
  println("\r\n\r\nUSB cameras listed by system:");
  printArray(Capture.list());
  
  
  // Set up cameras, find out how many there are; hook up one to start with.
  cameras = Capture.list();
  numCamsAvailable = cameras.length;
      
  if ( (numCameras > 0) & (numCamsAvailable > 0) ){
    println("Attaching 1st camera.  Total found: " + cameras.length ); 
    vid1 = new Capture(this, cameras[0]  );
    vid1.stop();
    vid1.start();
  } else { 
    println("No cameras found."); /* no cameras found */
    numCameras = 0;
  }  
  if (  (numCameras > 1) & (numCamsAvailable > 1) ){ // attach a second camera if it exists
    println("Attaching 2nd camera found." );
    vid2 = new Capture(this, cameras[1]  );  
    vid2.stop();
    vid2.start();
  }

  sPort.write('x'); sPort.clear(); // make sure it gets stopped in case it was left running
  sPort.write('b');  // Send the 'b' character to Arduino to 'begin recording'  
  t0_ms = millis(); 
  
  amRecording = false; 
  amReady = true;
   
  
}


myCLibrary.length_3D myTipPos = new myCLibrary.length_3D();
myCLibrary.length_3D myTipUnitDirection = new myCLibrary.length_3D();  
myCLibrary.length_3D myTipRPY = new myCLibrary.length_3D();

////////////////////////////////////////////////////////////
//     MAIN LOOP (runs until closed)                 ///////
////////////////////////////////////////////////////////////
public void draw(){
  
  
  // read cameras if needed
  if ((numCameras > 0) && vid1.available()) {
    vid1.read();
  }
  if ((numCameras > 1) && vid2.available()) {
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
    
  dropListCam1.setItems( videoOptions, vid1Selection );
  dropListCam2.setItems( videoOptions, vid2Selection ); 
  text( "Cameras available  (qty. "+ numCamsAvailable +") ... \r\n", 15, 210);
  text( join(cameras, "\r\n") + "\r\n", 20, 225);
  
  text( "Latest Microscribe motion  [ t(ms)    x y z (cm)   r p y(deg) ] ...  \r\n   " + mData , 15, 290); 
  /*sData = sData.substring(0, sData.length()-1) + "   " + 
        nf(myTipPos.x,0,5) + " " + nf(myTipPos.y,0,5) + " " +nf(myTipPos.z,0,5) +
        nf(myTipRPY.x,0,5) + " " + nf(myTipRPY.y,0,5) + " " + nf(myTipRPY.z,0,5) + 
        "  " + (millis() - t0_ms);*/
    //println(  );
    //println("\t after: x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
    //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );

  //if ( tempString 
  text( "Latest forces data [ t(ms) Fx Fy Fz (N) Tx Ty Tz(Nm) ] ... \r\n   " + tempString  , 15, 255);  
  
  
  

  
  //myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );  
  //myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
  //println(  );
  //println("\t after: x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
  //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );
  
  //text( "  pose  x,  y,  z  [cm]: "  + nf(myTipPos.x,0,2) + ", " + nf(myTipPos.y,0,2) + ", " +nf(myTipPos.z,0,2) +
  //  "\r\n  roll, pitch, yaw [deg]: " + nf(myTipRPY.x,0,2) + ", " + nf(myTipRPY.y,0,2) + ", " + nf(myTipRPY.z,0,2)   , 20, 280);
  
  
  // Here we don't save what we see on the display,
  // but the webcam input.
  if(numCameras > 0) {
    image(vid1, 10                , height-Hvid - 10      , Wvid, Hvid );
  } else {
    fill(255);
    noStroke();
    rect(       10                , height-Hvid - 10      , Wvid, Hvid );
  }
  if(numCameras > 1) {
    image(vid2, 10 + Wvid + 10    , height-Hvid - 10      , Wvid, Hvid );
  } else {
    fill(255);    
    noStroke();
    rect(       10 + Wvid + 10    , height-Hvid - 10      , Wvid, Hvid );
  }
  
  if (amRecording) {
    //motionLog.print( sData ); // this should be done in the time and serialEvent handlers below
    if ( vid1Selection > 0 ) videoExport1.saveFrame();
    if ( vid2Selection > 0 ) videoExport2.saveFrame();
  }
}

void keyPressed() {
  if (key == 'q') {
    //videoExport.endMovie();
    stopRecording();
    vid1.stop(); vid2.stop();    
    exit();
  }
}

// Use this method to add additional statements
// to customise the GUI controls
public void customGUI(){
  
  textfieldFilename.setText( fileStub );
  videoOptions = splice( Capture.list(), "No Recording", 0  );
  dropListCam1.setItems( videoOptions, vid1Selection );
  dropListCam2.setItems( videoOptions, vid2Selection );
  
  //println( height );
}

void onTimerEvent() {
  t_ms = millis() - t0_ms;  // time in ms since begin recording got started 
  lastMicroscribeMillis = t_ms; //millisDiff + lastMicroscribeMillis;  
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
  if ( amRecording ) {    
    motionLog.print( mData );
    //forcesLog.print( sData );
  }  
  
  
  // get arduino data if it is available (should  be),
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
      //print("last sData char not '\n' !!  (check data file for integrity) \r\n");
    } else if ( amRecording )  
    {
      forcesLog.print( sData.substring( 0, sData.length()-3 ) ); // log everthing but last newline 
      forcesLog.print( "   % sampleTime = " + lastMicroscribeMillis + "\r\n");
    }
     
    
      
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
  sPort.write('x'); 
  super.exit();
}
