import random
import time
import sys

def main():
	#Input radio byte array from sensors
	arrayRet = bytearray(2)

	while(True):
		arrayRet[0] = random.randint(1,101)
		arrayRet[1] = random.randint(1,101)

		#Convert byte array to list and print as string without bracess
		print(str(list(arrayRet))[1:-1])

		#Flush system output
		sys.stdout.flush()

		#Sleep for 1 second
		time.sleep(0.3)

if __name__ == '__main__':
    main()
