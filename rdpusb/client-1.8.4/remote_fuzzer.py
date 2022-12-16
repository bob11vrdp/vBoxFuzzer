from ctypes import *
import threading
import os
import time
import subprocess
import sys

libc = cdll.LoadLibrary("./librdesktop.so")

ip_addr = b"127.0.0.1"

if len(sys.argv) >= 2:
	ip_addr = sys.argv[1].encode("ascii")

class Fuzzer:
	def start_xWin(self):		
		libc.wrap_main(ip_addr)

	def exec_radamsa(self):
		cmd  = "echo '豌亮?굒엜똁챐쒎눻긯둭펤뺵' | radamsa"	
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate()			
			#print("fuzzed data :", outs.decode("utf-8"))
			return outs
		except Exception as e:
			print(e)			
			return b""
	
	def start(self): 
		vBox_thread = threading.Thread(target=self.start_xWin)
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
	