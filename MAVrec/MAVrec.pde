import g4p_controls.*;
import processing.video.*;
import com.hamoid.*;
import processing.serial.*;




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
String sData;


boolean amRecording;
boolean amReady;

PrintWriter motionLog, notesLog;

void captureEvent(Capture video) {
   video.read();
}

int FPS = 30;

////////////////////////////////////////////////////////////
//     SETUP (runs once)                             ///////
////////////////////////////////////////////////////////////
public void setup(){
  size(800, 600 );
  createGUI();
  customGUI();
  
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
  sPort.write('R');  // Send the 'R' character to Arduino to start 'recording'  

  amRecording = false; 
  amReady = true;
   
  
}



////////////////////////////////////////////////////////////
//     MAIN LOOP (runs until closed)                 ///////
////////////////////////////////////////////////////////////
public void draw(){
  
  // get arduino data if it is available,
  if (sPort.available() > 0) {  
    sData =  sPort.readStringUntil('\n');    
    //sData =  sPort.readString(); 
    //logData(file1,getDateTime() + sData,true);
    //delay(logDelay);
    //print("serial data: " + sData);
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
  
  text( "Latest motion/forces data:" , 15, 250);
  text( " " + sData , 20, 265);
  
  
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
