import serial
import time
import binascii #used to hexlify and unhexlify
import pdb #used for debugging, insert pdb.trace() anywhere to start debugging at that line

loadcell = serial.Serial(
    port = 'COM15',
    baudrate = 1250000,
    parity = 'E',
    bytesize = 8,
    stopbits = 1,
    timeout = 1
    )

command=binascii.hexlify('0a4655a39d') #command to start streaming data
#jam=binascii.hexlify('0affefff570144007fef1f') #command to jam streaming data

loadcell.write(command) #write this command to serial 
#loadcell.write(jam)

t0=time.time()

#pdb.set_trace()

def CheckSample(sampleBytes): #function to check each sample for CRC bytes
    checkSum=0
    for i in range (0,11):
        checkSum = checkSum + sampleBytes[i]

    if (checkSum & 127) <> (sampleBytes[12] & 127):
        return False
    else:
        return True


while True:
    loadcell.flushInput() #flush serial
    t1=time.time()

    reading=loadcell.read(13) #read 13 bytes  as per manual
    sample=bytearray(reading) #save in array because I want to read every byte separately

    if CheckSample(sample): #if checksum passes
        for i in range(0,11,2):
            print(binacii.hexlify(reading[i])) #decode each byte
            print(' ')
        print(reading[12]) #decode status word
        print(binascii.unhexlify(reading)) #decode the full array
        print(' ')
        print(time.time()-t1)
        print('\n')
    else:
        continue


loadcell.close() #close serial
