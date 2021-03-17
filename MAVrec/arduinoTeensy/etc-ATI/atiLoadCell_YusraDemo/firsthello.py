import serial
import time
import pdb

loadcell = serial.Serial(
    port = 'COM15',
    baudrate = 1250000,
    parity = 'E',
    bytesize = 8,
    stopbits = 1,
    timeout = 1
    )

#command='0a4655a39d'.decode('hex')
#jam='0affefff570144007fef1f'.decode('hex')
#command='0a6aaaff1d'.decode('hex') #success
#command='0a0106000000aab8ae'.decode('hex')
command='0a03001d00011577'.decode('hex') #success
#command = '0a0300e30004b484'.decode('hex') #success calibrationserialno
#read 13bytes, print 3:10,
#command = '0a0300e70004f545'.decode('hex') #success calibpartnumber
#command = '0a0300f700027482'.decode('hex') #calib id , read 9 bytes, print 3:6
#pdb.set_trace();

#loadcell.write(command)
#loadcell.write(jam)

#while ser.inWaiting()
#response = loadcell.read(10)
#print(response.encode('hex'))
#time.sleep(0.05)
t0=time.time()
#loadcell.flushInput()
#j=0
#for i in range (1,3000):
while True:
    loadcell.flushInput()
    data = loadcell.read(13)
    #print(time.time()-t0)
    print(data.encode('hex'))
    print('\n')
    
#loadcell.close()
