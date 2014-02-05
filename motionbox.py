import os
import sys
import time
import serial
from serial.tools import list_ports

try:
	import ctypes
except:
	print('Warning: lock screen is only available on Windows')
	
def readArduino(ser, lock, ex):
	buffer = ''
	while 1:
		b = ser.read()
		while b:
			print('state = '),
			print(b)
			if b == '0':
				if lock:
					if os.name == 'nt':
 						ctypes.windll.user32.LockWorkStation()
 					else:
 						print('Available only on Windows')
 				if ex:
 					quit()
			b = ser.read();
	
def serial_ports():
    if os.name == 'nt':
        # windows
        for i in range(256):
            try:
                s = serial.Serial(i)
                s.close()
                yield 'COM' + str(i + 1)
            except serial.SerialException:
                pass
    else:
        # unix
        for port in list_ports.comports():
            yield port[0]


def print_usage():
	print
	print('motionbox -p <SERIAL PORT> -b <BAUD RATE> [options]')
	print('Options:')
	print('  -l       Lock the computer screen on movement detection')
	print('  -e       Exit the application on movement detection')
	print('  -w  <N>  Wait N seconds before to start')
	print
	print('Example (linux): motionbox -p ttyUSB0 -b 9600 -l -e')
	print('Example (winows): motionbox -p COM6 -b 9600 -l')	
	print
	print('Available serial ports: ')
	print(list(serial_ports()))
	quit()
	
def main():
	argc = len(sys.argv)
	if argc < 5:
		print_usage()
		
	ex = False
	wait = 0
	lock = False
	port = 'COM6'
	baud_rate = 9600
	
	for i in range(argc):
		arg = sys.argv[i]
		if arg == '-p':
			port = sys.argv[i+1]
			print('Serial port = ' + port)
			continue
			
		if arg == '-b':
			baud_rate = sys.argv[i+1]
			print('Baud rate = ' + baud_rate)
			continue
			
		if arg == '-l':
			lock = True
			print('Lock screen on motion detection activated')
			continue
			
		if arg == '-e':
			ex = True
			print('Exit on motion detection')
			continue
			
		if arg == '-w':
			wait = int(sys.argv[i+1])
			print('You have '+str(wait)+' seconds to get away of the motion detector')
			continue
	
	ser = serial.Serial(port, baud_rate, timeout=0)
	
	if wait > 0:
		for i in range(wait):
			print(wait-i),
			time.sleep(1)
		print
		print('Activated')
	
	readArduino(ser, lock, ex)
	
main()
    
