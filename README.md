# microscribe-ATInano-video-recorder  MAVrec

This project creates a processing.org sketch to  to record  and syncronize data from the following sources simultaneously:
* 0, 1 or 2 usb cameras or similar video sources; dumping to separate mp4 files (no audio) 
* a usb-serial source such as Teensy 3.6, 4.0, or 4.1 which extracts data from the devices below and dumps it to serial to the program
* MicroScribe M or MX 6DoF (Microscribe plugs directly into Teensy USB host port, extracts usb packets, does timing sync, and forwards them to serial. Note this is only raw encoder readings and the arm kinematic link DH parameters needed to map them to Cartesian pose
* ATI Nano17 or similar running on an ATI 9105-NETCANOEM board which forwards the extracted Fx, Fy,Fz, Tx, Ty, Tz forces as compensated by factory settings. 



Pre-reqs (All library/binaries are included in this repo): 
* ffmpeg.exe  (full build, version used: ffmpeg-4.3.2-2021-02-20-full_build_) 
* Teensyduino and USBHost_t36 (Teensyduino 1.53)
* Arduino 1.8.13 running on the teensy 
* The Teensy 4.1 board connected and wired to the NETCANOEM board as expected in the software (pinouts, etc)
* Microscribe USB powered on, and HOME'ed correctly (green light on), plugged into Teensy USB Host port and Teensy USB plugged into PC Host computer USB Host port
* USB cameras plugged into PC Host computer directly (ideally on a separate USB host/hub  in the PC to minimize latency or dropped video frames)
* Processing 3.5.4 (processing-3.5.4-windows64 was used) with the following libraries enabled/installed:
* Processing - g4p_controls (G4P Graphical user interface/GUI editor for processing); import g4p_controls.*;
* Processing - Video Capture library: import com.hamoid.*; (for recording compressed mp4s via ffmpeg)
* Processing - Video and Serial libraries (native): import processing.serial.*;   import processing.video.*;



SETUP:
 1. Create the folder in C:\tissueCapture\    Make sure it has full read-write priviliges.  All recordings are dumped directly to C:\tissueCapture\recordedData\.  This is to avoid issues with Windows 10 synchronized folders, google drive, dropbox or similar auto-syncing which may interfere with robust, low-latency data capture.  Feel free to copy the recorded files over to any cloud-based utility *after* they are done recording (after closing the processing program). 
 2. Install Processing (from the provided installer) and make sure the required libraries are added ( G4P, Hamoid video capture ).  The sketch won't run until this is set up. 
 3. OPTIONAL: Confirm ffmpeg.exe works by openning a command line, navigating to where it lives (e.g. C:\tissueCapture\mavREC\ , type ffmpeg and hit ENTER)
 4. Open mavREC.pde in Processing and Run it.  Camera initialization may take a while on the first start.  Same with the first video recording (you might need to point to the ffmpeg.exe file when you hit record the first time).  Once the first recording or two is complete, close the sketch and Processing.  If the Arduino/Teensy is Com port has changed, just edit that at the top of the pde.  
 5. Right click on MAVrec.pde and make a shortcut to it on the Desktop.  
 6. Now you can run the mavREC.pde processing sketch by clicking the desktop icon whenever you need to record any sessions.  
 7. Edit the filename you want for your study, set up/select the video sources.  When you hit RECORD, the following happens:
    a) A date/timestamp is added to your filename (this makes sure you never overwrite data)
    b) Filename_DATASTAMP_notes.txt, _data.txt files are created.  These are human readable ascii where all measurements are stored.  Any notes you make in the app during this experiment will get dumped there too.  Including critical status updates.
    c) Also _vid1.mp4 and _vid2.mp4 files will be created if you chose to record video sources.  NOTE: ALL FILES SHOULD ONLY BE READ/OPENED BY USERS **AFTER** THE RECORDING IS DONE.  Ideally, after you close the processing sketch completely (to make sure JAVA isn't taking forever to close and clean up files in the background).  
  
