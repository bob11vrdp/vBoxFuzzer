
#local_fuzzer.py
import threading
import os
import time
import subprocess
from ctypes import *


libc = cdll.LoadLibrary("./librdesktop.so")


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
	
	def start_xWin(self):
		libc.wrap_main(b"127.0.0.1")
	
	def start_vBox(self):
		#os.system("gnome-terminal -e 'bash -c \"python3 local_fuzzer_sub.py;\"'")
		cmd  = "/home/son/VBX/VirtualBox-6.1.36/out/linux.amd64/release/bin/VBoxHeadless --startvm win10 --vrde on"
		print("\n", cmd)
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		
		try:
			outs, error = proc.communicate()
			print("returnCode :",  proc.returncode)
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))
			
			#if error .decode("utf-8") != "" :             
			#    crash_log(outs, error, proc.returncode, "crash.log")
		except Exception as e:
			print(e)
			return
	

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
					xWin_thread = threading.Thread(target=self.start_xWin)
					xWin_thread.start()
					#libc.start_xWin(b"127.0.0.1")
					time.sleep(1)
				else:
					outs = self.exec_radamsa()
					if libc.isConnected() == True:
						libc.fuzz_device_list(outs)						
						time.sleep(0.1)

			_counter =0	
			while _counter < 4:
				time.sleep(0.2)
				print("*")
				_counter += 1

if __name__=="__main__":
	fuzzer=Fuzzer()
	fuzzer.start()