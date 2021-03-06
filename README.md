# microscribe-ATInano-video-recorder:  "MAVrec"

This project creates a processing.org sketch to record and syncronize data from the following sources simultaneously:
* 0, 1 or 2 usb cameras or similar video sources; dumping to separate mp4 files (no audio) 
* a usb-serial source such as Teensy 3.6, 4.0, or 4.1 which extracts data from the devices below and dumps it to serial which the sketch dumps to a _data.txt file.
* MicroScribe M or MX 6DoF (Microscribe plugs directly into  PC USB, processing sketch calls microscribe windows dll calls via java).
* ATI Nano17 or similar running on an ATI 9105-NETCANOEM board which forwards the extracted Fx, Fy,Fz, Tx, Ty, Tz forces as compensated by factory settings. 

Video walkthroughs for hardware setup, assembly, connectsion, and use cases available here:
https://www.youtube.com/playlist?list=PLA7mdmemHTEwr_EhA78e7k7b2WX9mksp5


Pre-reqs (All library/binaries are included in this repo): 
* ffmpeg.exe  (full build, version used: ffmpeg-4.3.2-2021-02-20-full_build_; don't need to do a separate install bc the specific executabel ffmpeg.exe lives within the project.) 
* Teensyduino (Teensyduino 1.53; only if you want to edit Arduino code: https://www.pjrc.com/teensy/td_download.html)
* Arduino 1.8.13 running on the teensy (only if you want to edit Arduino code: https://www.arduino.cc/en/Main/OldSoftwareReleases#previous) 
* The Teensy 4.1 board connected and wired to the NETCANOEM board as expected in the software (pinouts, etc)
* Microscribe USB powered on, and HOME'ed correctly (green light on), plugged into Teensy USB Host port and Teensy USB plugged into PC Host computer USB Host port
* USB cameras plugged into PC Host computer directly (ideally on a separate USB host/hub  in the PC to minimize latency or dropped video frames)
* Processing 3.5.4 (processing-3.5.4-windows64 was used https://github.com/labmrd/microscribe-ATInano-video-recorder/blob/main/toInstall/processing-3.5.4-windows64.zip) with the following libraries enabled/installed:
  * Processing - g4p_controls (G4P Graphical user interface/GUI editor for processing); import g4p_controls.*;
  * Processing - Video Capture library: import com.hamoid.*; (for recording compressed mp4s via ffmpeg)
  * Processing - Video and Serial libraries (native): import processing.serial.*;   import processing.video.*;



SOFTWARE SETUP:
 1. Create the folder in C:\tissueCapture\    Make sure it has full read-write priviliges.  All recordings are dumped directly to C:\tissueCapture\recordedData\.  This is to avoid issues with Windows 10 synchronized folders, google drive, dropbox or similar auto-syncing which may interfere with robust, low-latency data capture.  Feel free to copy the recorded files over to any cloud-based utility *after* they are done recording (after closing the processing program).  
 2. When tissueCapture/ folder is created, download or checkout the entire repository from  https://github.com/labmrd/microscribe-ATInano-video-recorder.  You can either click the "Code" button and download a zip file (~500 MB) or use github to do a local checkout.  This will keep the most up-to-date code available.  e.g., use github desktop from https://desktop.github.com/, log in with a git account, and do a checkout of this repo:  https://github.com/labmrd/microscribe-ATInano-video-recorder/.  If you downloaded a zip file, unzip it somewhere.  Either way, move the folder "./MAVrec/" to c:\tissueCapture\ so it's in "c:\tissueCapture\MAVrec\"

 3. Install [Processing 3.5.4](https://download.processing.org/processing-3.5.4-windows64.zip) for windows  (backup copy also available in ./toInstall/ folder or here:  https://github.com/labmrd/microscribe-ATInano-video-recorder/blob/main/toInstall/processing-3.5.4-windows64.zip).  
 4. Run processing.  Install the required libraries below ( G4P, Hamoid, video capture, etc. ).  The sketch won't run until this is set up. To add each library, run processing, click "Sketch --> Import Library --> Add Library"   Click libraries tab.   
    * type 'G4P', click and install  G4P | Procides a set of 2D GUI controls... (Peter Lager)
    * 'TimedEvents' | A couple of classes for firing off timed events at (Jason Gessner)
type 'video' in search and select the following and install (each):
    *  Video | GStreamer-based video library for Processing. (The processing foundation)
    *  Video Export | Simple video file exporter (Abe Pazos)
    *  OPTIONAL:  'grafica'  (Javier Gracia Carpio) for exented plotting capabilities
    *  OPTIONAL: you can add the GUI editor if you want to change/add buttons, etc.  Go to  "Sketch --> Import Library --> Add library" and click 'Tools' tab OR in processing window "Tools --> Add Tool";   Install G4P GUI Builder | Graphic design tool for creating user interfac... (Peter Lager)
    * OPTIONAL: Confirm ffmpeg.exe works by openning a command line, navigating to where it lives (e.g. C:\tissueCapture\mavREC\ , type ffmpeg and hit ENTER)  If you get an error, you have issues.  Make sure that ffmpeg is in this execution folder where the processing sketches live.  
 5. Open mavREC.pde in Processing and Run it.  Camera initialization may take a while on the first start.  Same with the first video recording (you might need to point to the ffmpeg.exe file when you hit record the first time).  Once the first recording or two is complete, close the sketch and Processing.  If the Arduino/Teensy is Com port has changed, just edit that at the top of the pde.  
 6. Right click on MAVrec.pde and make a shortcut to it on the Desktop.  
 7. Now you can run the mavREC.pde processing sketch by clicking the desktop icon whenever you need to record any sessions.  
 8. Edit the filename you want for your study, set up/select the video sources.  When you hit RECORD, the following happens:
    a) A date/timestamp is added to your filename (this makes sure you never overwrite data)
    b) Filename_DATASTAMP_notes.txt, \_data.txt files are created.  These are human readable ascii where all measurements are stored.  Any notes you make in the app during this experiment will get dumped there too.  Including critical status updates.
    c) Also \_vid1.mp4 and \_vid2.mp4 files will be created if you chose to record video sources.  NOTE: ALL FILES SHOULD ONLY BE READ/OPENED BY USERS **AFTER** THE RECORDING IS DONE.  Ideally, after you close the processing sketch completely (to make sure JAVA isn't taking forever to close and clean up files in the background).  
  
