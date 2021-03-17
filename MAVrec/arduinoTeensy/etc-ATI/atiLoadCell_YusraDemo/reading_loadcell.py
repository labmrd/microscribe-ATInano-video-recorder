import serial
import time

loadcell = serial.Serial(
    port = 'COM15',
    baudrate = 1250000,
    parity = 'E',
    bytesize = 8,
    stopbits = 1,
    timeout = 1
    )

command='0a4655a39d'.decode('hex')
#jam='0affefff570144007fef1f'.decode('hex')

loadcell.write(command)
#loadcell.write(jam)

t0=time.time()

#for i in range(1,14,2):
while True:
    loadcell.flushInput()
    check=loadcell.read(1)
    check_hex=check.encode('hex')
    #print(check_hex)
    if check_hex=='00':
        check2 = loadcell.read(1)
        check2 = loadcell.read(1) #read twice for one byte each
        check2_hex = check2.encode('hex')
        if check2_hex=='00':
            data = loadcell.read(10)
            t1=time.time()
            print(t1-t0)
            z=list(check,check2,data)
            print(z)
            print(' ')
            print(time.time()-t1)
            print('\n')
        else:
            continue



##while checkbin==1:
##    loadcell.flushInput()
##    data = loadcell.read(13)
##    t1=time.time()
##    print(t1-t0)
####    for i in range (1,14 ,2):
####        print (i)
####        print(',')
####        print(data[i])
####        print(',')
##    z=list(data)
##    print(z)
##    #print(data.encode('hex'))
##    print(' ')
##    print(time.time()-t1)
##    print('\n')
##    
loadcell.close()
