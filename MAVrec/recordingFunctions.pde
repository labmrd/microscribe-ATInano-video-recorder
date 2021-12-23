////////////////////////////////////////////////////////////
//     BEGIN/START Recording                         ///////
////////////////////////////////////////////////////////////
String bannerUnitsMotion = 
"% writingEndTime should match sampleTime.  If they differ, this is how much possible time elapsed during writing of data (jitter); \r\n"+
"% microScribeTime[ms]  X[mm] Y[mm] Z[mm] \t roll[deg] pitch[deg] yaw[deg]  writingEndTime[ms]\t\t" +  "% status info\r\n";
String bannerUnitsForces = 
"% ATIbasicMatrix applied to G0...G5 to get Forces, Torques:  [F; T] = ATIbasicMtx * [ G ]\r\n\r\n"+
"% ATInanoHardwareTime[ms]\t Fx[N] Fy[N] Fz[N]   Tx[mNm] Ty[mNm] Tz[mNm]\t % hardwareTime[us]  checksumError[0=OK] statusError[1=bad] %  microscribeSampleTime  \r\n"+ 
"% NOTE: if hardwareTime[us] does NOT match ATInanoHardwareTime[ms] closely, those samples are stale, often happens on startup, takes about 100ms for forces to start streaming \r\n\r\n\r\n";
public void  tareATIreadings() {
  if ( useATInano17 ){
    sPort.write('t');
  }
}

public boolean beginRecording( ){
  
  
  
  fileTime = getTimestamp();
  fileName = fileStub + "_" + fileTime;
  String banner= "% Motion and Forces Recording of Tissue Capture App\r\n" + 
                 "% Timestamp YYYY.MM.DD_hh.mm.ss: " + fileTime + "\r\n" +                 
                 "% FileFolder: \t\t " + fileFolder +  "\r\n" ;
                 //"% Filename: \t\t " + fileName + "\r\n" +
                 //bannerUnits ; motionFilenameEnd
                 
  // Create a new file in the sketch directory; Bail if that fails
  println("Opening file for writing ..." + fileFolder + fileName);
  try{
    motionLog = createWriter( fileFolder + fileName + "_motion.txt");
    motionLog.print( banner + "% Filename: \t\t " + 
                                fileName + motionFilenameEnd + "\r\n" + 
                                bannerUnitsMotion );
    forcesLog = createWriter( fileFolder + fileName + "_forces.txt");
    forcesLog.print( banner + "% Filename: \t\t " + 
                                fileName + forcesFilenameEnd + "\r\n" + atiInfo +
                                bannerUnitsForces );
    
  } finally {  
  
    //println(motionLog);
    if (motionLog == null) {
      println("ERROR: Cannot open file for writing: " +fileFolder + fileName + motionFilenameEnd);
      println("\t\tTry to create the folder with the correct permissions?");
      amRecording = false;
      return false; // STOP TRYING TO RECORD YO!
    }
    if (forcesLog == null) {
      println("ERROR: Cannot open file for writing: " +fileFolder + fileName + forcesFilenameEnd);
      println("\t\tTry to create the folder with the correct permissions?");
      amRecording = false;
      return false; // STOP TRYING TO RECORD YO!
    }
  }
  
  // Create notes file (should be OK if the above worked)
  notesLog  = createWriter( fileFolder + fileName + "_notes.txt"); 
  notesLog.println( "% Notes taken during Tissue Capture of file: "+fileName );
  if ( useMicroScribe ) { notesLog.println("% MicroScribe (motion recording): in use"); }  
  else {                  notesLog.println("% MicroScribe (motion recording): not in use"); };   
  if ( useATInano17 ) {   notesLog.println("% ATI Nano 17 (force recording): in use"); }  
  else {                  notesLog.println("% MicroScribe (motion recording): not in use"); };  
  if ( useVideo1 ) {      notesLog.println("% Video1 Recording: in use, source: [" + vid1Selection + "] " + cameras[vid1Selection]); }
  else {                  notesLog.println("% Video1 Recording: not in use"); }
  if ( useVideo2 ) {      notesLog.println("% Video2 Recording: in use, source: [" + vid2Selection + "] " + cameras[vid2Selection]); }
  else {                  notesLog.println("% Video2 Recording: not in use"); }
  //notesLog.println( "% Number of Cameras selected to record: " + numCameras );
  //notesLog.println( "% Number of Cameras available at end: " + numCamsAvailable );
  
  
    
  // Start Recording Video
  if ( useVideo1 ) {
    videoExport1 = new VideoExport(this, fileFolder + fileName + "_vid1.mp4", vid1);
    videoExport1.startMovie();
  } 
  if ( useVideo2 ) {
    videoExport2 = new VideoExport(this, fileFolder + fileName + "_vid2.mp4", vid2);
    videoExport2.startMovie();
  }
  
  if ( useATInano17 ){
    // reset and start streaming ATI data...
    sPort.write('x'); sPort.clear();
    //sPort.write('i'); // get information from ATI (configuration file)
    sPort.write('b'); // this should reset arduino time counter to 0 (b for begin new recording)
  }
  
  t0_ms = millis();
  //lastMicroscribeMillis = millis();
  
  
  
  amRecording = true;  
  
  return true; // all worked
  
  
}


////////////////////////////////////////////////////////////
//     STOP Recording                                ///////
////////////////////////////////////////////////////////////
public void stopRecording( ){
  amRecording = false;
  //sPort.write('x'); 
  
  // stop writing text data to files;  close them out
  motionLog.flush();  // Writes the remaining data to the file
  motionLog.close();  // Finishes the file
  // stop writing text data to files;  close them out
  forcesLog.flush();  // Writes the remaining data to the file
  forcesLog.close();  // Finishes the file
 
  
  if( textareaNotes.getText().length() == 1){
    notesLog.println("% [No user notes were entered during recording, nothing to save.]");
    println("% [No user notes were entered during recording, nothing to save.]\r\n\r\n");
    
  }
  notesLog.print( textareaNotes.getText()  );// write everything no matter what
  println( textareaNotes.getText()  ); 
  notesLog.flush();
  notesLog.close();
  textareaNotes.setText("");
  //textareaNotes.setPromptText("Enter notes here (any text in this box will be saved to the same filename set)");
  
  
  
  
  // finalize all videos and save
  if ( useVideo1 ) videoExport1.endMovie();
  if ( useVideo2 ) videoExport2.endMovie();
  
  //sPort.clear();
  //sPort.write('b');
  
}

// get a timestamp of the format YY.MM.DD_hh.mm.ss
public String getTimestamp( ) {
  return nf(year(),2) +"."+ nf(month(), 2) +"."+ nf(day(),   2) + "_"+
         nf(hour(),2) +"."+ nf(minute(),2) +"."+ nf(second(),2);
}
