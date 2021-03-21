////////////////////////////////////////////////////////////
//     BEGIN/START Recording                         ///////
////////////////////////////////////////////////////////////
String bannerUnits = 
"% X[mm] Y[mm] Z[mm] \t roll[deg] pitch[deg] yaw[deg] \t\t"+
  "Fx[N] Fy[N] Fz[N] \tTx[Nm] Ty[Nm] Tz[Nm] % status info";
public boolean beginRecording( ){
  
  sPort.write('x'); sPort.clear();
  sPort.write('b'); // this should reset arduino time counter to 0 (b for begin new recording)
  
  t0_ms = millis();
  
  fileTime = getTimestamp();
  fileName = fileStub + "_" + fileTime;
  String banner= "% Motion and Forces Recording of Tissue Capture App\r\n" + 
                 "% Timestamp YYYY.MM.DD_hh.mm.ss: " + fileTime + "\r\n" +
                 "% Filename: \t\t " + fileName + "\r\n" +
                 "% FileFolder: \t\t " + fileFolder +  "\r\n" +
                 bannerUnits ;
                 
  // Create a new file in the sketch directory; Bail if that fails
  println("Opening file for writing ..." + fileFolder + fileName);
  try{
    motionLog = createWriter( fileFolder + fileName + "_data.txt");
    motionLog.print( banner );
    
  } finally {  
  
    //println(motionLog);
    if (motionLog == null) {
      println("ERROR: Cannot open file for writing: " +fileFolder + fileName + "_data.txt");
      println("\t\tTry to create the folder with the correct permissions?");
      amRecording = false;
      return false; // STOP TRYING TO RECORD YO!
    }
  }
  
  // Create notes file (should be OK if the above worked)
  notesLog  = createWriter( fileFolder + fileName + "_notes.txt"); 
  notesLog.println( "% Notes taken during Tissue Capture of file: "+fileName );
  notesLog.println( "% Number of Cameras selected to record: " + numCameras );
  //notesLog.println( "% Number of Cameras available at end: " + numCamsAvailable );
  
  
    
  // Start Recording Video
  if ( numCameras > 0) {
    videoExport1 = new VideoExport(this, fileFolder + fileName + "_vid1.mp4", vid1);
    videoExport1.startMovie();
  } 
  if ( numCameras > 1) {
    videoExport2 = new VideoExport(this, fileFolder + fileName + "_vid2.mp4", vid2);
    videoExport2.startMovie();
  }
  
  
  
  
  amRecording = true;  
  
  return true; // all worked
  
  
}


////////////////////////////////////////////////////////////
//     STOP Recording                                ///////
////////////////////////////////////////////////////////////
public void stopRecording( ){
  amRecording = false;
  
  // stop writing text data to files;  close them out
  motionLog.flush();  // Writes the remaining data to the file
  motionLog.close();  // Finishes the file
 
  
  if( textareaNotes.getText().length() == 1){
    notesLog.println("% [No notes were made during recording, nothing to save.]");
    println("% [No notes were made during recording, nothing to save.]\r\n\r\n");
    
  }
  notesLog.print( textareaNotes.getText()  );// write everything no matter what
  println( textareaNotes.getText()  ); 
  notesLog.flush();
  notesLog.close();
  textareaNotes.setText("");
  //textareaNotes.setPromptText("Enter notes here (any text in this box will be saved to the same filename set)");
  
  
  // finalize all videos and save
  if ( vid1Selection > 0 ) videoExport1.endMovie();
  if ( vid2Selection > 0 ) videoExport2.endMovie();
}

// get a timestamp of the format YY.MM.DD_hh.mm.ss
public String getTimestamp( ) {
  return nf(year(),2) +"."+ nf(month(), 2) +"."+ nf(day(),   2) + "_"+
         nf(hour(),2) +"."+ nf(minute(),2) +"."+ nf(second(),2);
}
