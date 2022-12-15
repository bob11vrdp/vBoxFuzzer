
#local_fuzzer.py
import threading
import os
import time
import subprocess
from ctypes import *

try:
	libc = cdll.LoadLibrary("./librdesktop.so")
except Exception as e:
	print(e)

class Fuzzer:
	def __init__(self):
		self.iteration = 0
	
	def exec_radamsa(self):
		cmd  = "echo 'aaa' | radamsa"
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		
		try:
			outs, error = proc.communicate()
			return outs
		except Exception as e:
			print(e)
			return b""
	
	def checkRunning(self, cmd):
		print("\n>>>",cmd)
		proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		
		try:
			outs, error = proc.communicate()
			
			if outs.decode("utf-8") != "" :
				print("running.... \n")
				return True
			else:
				print("not running... \n")
				return False
		except Exception as e:
			return False
	
	def start_rdesktop(self):
		libc.wrap_main(b"127.0.0.1")
	
	def start_vBox(self):
		os.system("gnome-terminal -e 'bash -c \"python3 local_fuzzer_sub.py;\"'")

	def start(self):
		while True:
			vBoxRunning = self.checkRunning("ps -ef | grep VBoxHeadless | grep -v grep")

			if vBoxRunning == False:
				#self.start_vBox()
				vBox_thread = threading.Thread(target=self.start_vBox)
				vBox_thread.start()
				time.sleep(1)
				
			else:
				rs = libc.isConnected()
				print( "is connected : ", rs )
				
				if rs == False:
					libc.wrap_main(b"127.0.0.1")
				else:
					time.sleep(0.1)

			_counter =0	
			while _counter < 4:
				time.sleep(0.2)
				print("*")
				_counter += 1

if __name__=="__main__":
	fuzzer=Fuzzer()
	fuzzer.start()