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


boolean amRecording;
boolean amReady;

PrintWriter motionLog, notesLog;

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
  println("Available serial ports: ");  printArray( Serial.list() );
  println("Last serial port:  " + Serial.list()[Serial.list().length-1] );
  
  try {
   sPort = new Serial(this, comPort, comBaudrate );
   //sPort = new Serial(this, comPort, comBaudrate );
   // don’t read the serial buffer until a new line char
   // should use Serial.println("") in arduino code
   sPort.bufferUntil('\n');   
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

  sPort.clear();
  sPort.write('b');  // Send the 'b' character to Arduino to 'begin recording'  

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
  
  // get arduino data if it is available,
  if (sPort.available() > 0) {  
    // the moment it arrives get the microscribe data and a timestamp
    myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );    
    myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
    // microscribeTime = ;    
    sData =  sPort.readStringUntil('\n');    
    //sData =  sPort.readString(); 
    //logData(file1,getDateTime() + sData,true);
    //delay(logDelay);
    //print("serial data: " + sData);
    /*sData = sData.substring(0, sData.length()-1) + "   " + 
        nf(myTipPos.x,0,5) + " " + nf(myTipPos.y,0,5) + " " +nf(myTipPos.z,0,5) +
        nf(myTipRPY.x,0,5) + " " + nf(myTipRPY.y,0,5) + " " + nf(myTipRPY.z,0,5) + 
        "  " + (millis() - t0_ms);*/
    //println(  );
    //println("\t after: x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
    //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );  
  }



  if ((numCameras > 0) && vid1.available()) {
    vid1.read();
  }
  if ((numCameras > 1) && vid2.available()) {
    vid2.read();
  }
  
  background(217);
  text("Saving files named ...\r\n " + 
    fileName + "_data.txt" + "\r\n " + 
    fileName + "_vid1.mp4"  + "\r\n " +
    fileName + "_vid2.mp4"  + "\r\n " +
    fileName + "_notes.txt" ,    
    15, 90);
  text("in folder ...\r\n " + fileFolder, 15, 90+80);
  
  
  
  // Updates for camera changes
  cameras = Capture.list();
  numCamsAvailable = cameras.length;
  videoOptions = splice( cameras, "No Recording", 0 );
  
  if ( cameras.length < numCamsAvailable ) {
    println("Lost camera(s), new camera count: " + cameras.length ); 
    printArray( cameras );
    numCameras = cameras.length;    
    
  } else if (cameras.length > numCamsAvailable ){
    println("Added camera(s), new camera count: " + cameras.length ); 
    printArray( cameras );
    numCameras = cameras.length;
    
  } else { /* do nothing */}
    
  dropListCam1.setItems( videoOptions, vid1Selection );
  dropListCam2.setItems( videoOptions, vid2Selection ); 
  text( "Cameras available  [qty. "+ numCamsAvailable +"]:\r\n", 15, 210);
  text( join(cameras, "\r\n") + "\r\n", 20, 225);
  
  text( "Latest Microscribe Motion  [t(ms) x y z(cm) r p y(deg)]: \r\n   " + mData , 15, 290); 
    
  text( "Latest forces data [t(ms) Fx Fy Fz (N) Tx Ty Tz(Nm)]: \r\n   " + sData  , 15, 255);  
  
  
  

  
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
    motionLog.print( sData );
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
  int millisDiff = millis() - lastMicroscribeMillis;
  lastMicroscribeMillis = millisDiff + lastMicroscribeMillis;  
  myCLibrary.INSTANCE.ArmGetTipPosition( myTipPos  );    
  myCLibrary.INSTANCE.ArmGetTipOrientation( myTipRPY );
  //System.out.println("Got a timer event at " + millis() + "ms (" + millisDiff + ")!");
  //setRandomFillAndStroke();
  //ellipse(random(width), random(height), random(100), random(100));
  //println("\t x, y, z | r p y = " + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
  //  " | " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z );
  mData = "% " + millis() + myTipPos.x + ", " + myTipPos.y + ", " +myTipPos.z +
           " , " + myTipRPY.x + ", " + myTipRPY.y + ", " + myTipRPY.z ;
  
}
