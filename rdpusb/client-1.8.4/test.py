from ctypes import *
import threading
import os
import time
import subprocess
import sys

libc = cdll.LoadLibrary("./librdesktop.so")


class Fuzzer:
	def start_vBox(self):		
		libc.wrap_main("abcd")

	def exec_radamsa(self):
		cmd  = "echo 'aaaa' | radamsa"	
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate()			
			#print("fuzzed data :", outs.decode("utf-8"))
			return outs
		except Exception as e:
			print(e)			
			return b""
	
	def start(self): 
		vBox_thread = threading.Thread(target=self.start_vBox)
		vBox_thread.start()

		while(1):				
			try:				
				outs = self.exec_radamsa()				
				libc.fuzz_device_list(outs)		
				time.sleep(0.5)	
			except Exception as e:
					print("[Exception] : ", e)

if __name__=="__main__":
	fuzzer=Fuzzer()
	fuzzer.start()
	
