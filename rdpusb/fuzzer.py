#fuzzer.py
import threading
import os
#import shutil
import time
import subprocess
from ctypes import *

class Fuzzer:
	def __init__(self):		
		self.iteration = 0
		self.crash_count = 0		
		self.UUID = "win10"

	def crash_log(self, out, err, ret, filename):
		path, file_extension = os.path.splitext(filename)
		#crashFilename = path.replace("/", "-")		
		#f = open(f"./crash/vbox-{crashFilename}.log", "w")
		crashFilename = "crash.log"
		f = open(crashFilename, "w")
		f.write("Crash detected:  \n\n")
		f.write("STDERR :\n")
		f.write(str(err, 'utf-8') + "\n\n\n")
		f.write("STDOUT :\n")
		f.write(str(out, 'utf-8') + "\n\n\n")
		f.write("RET :\n")
		f.write(str(ret) + "\n\n\n")
		#shutil.copyfile(filename, "crash/input/crash.log")
		f.close()

	def start_vBox(self):		
		cmd  = " ./VBoxHeadless --startvm win10 --vrde on"
		print("\n>>>", cmd)
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate()
			print("returnCode :",  proc.returncode)
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))
			
			if error .decode("utf-8") != "" : 
					self.crash_count += 1
					self.crash_log(outs, error, proc.returncode, "crash.log")
				
		except Exception as e:
			print(e)			
			return


	def start_rdesktop(self, buf):
		libc = cdll.LoadLibrary("./librdesktop.so")
		libc.wrap_main(buf)
		
	def __start_rdesktop(self, buf):		
		cmd  = "./rdesktop-vrdp 127.0.0.1 -r usb"
		print("\n>>>", cmd)
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate(timeout=10)
			print("returnCode :",  proc.returncode)
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))
		except Exception as e:
			print(e)			
			return

	def exec_radamsa(self):		
		cmd  = "echo 'aaa' | radamsa"
		print("\n>>>", cmd)
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate()			
			print("stdout :", outs.decode("utf-8"))
			return outs.decode("utf-8")
		except Exception as e:
			print(e)			
			return ""
					

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


	def start(self):
		while True:
			
			vBoxRunning = self.checkRunning("ps -ef | grep VBoxHeadless | grep -v grep")

			if vBoxRunning == False:
				vBox_thread = threading.Thread(target=self.start_vBox)
				vBox_thread.start()

			else:
				clientRunning = self.checkRunning("ps -ef | grep rdesktop | grep -v grep")
				
				if clientRunning == False:
					buf = self.exec_radamsa()
					self.start_rdesktop(buf)					

			#############################
			_counter = 1
			while _counter < 4:				
				time.sleep(0.5)				
				print("*")
				_counter += 1
				

if __name__=="__main__":
	fuzzer=Fuzzer()
	fuzzer.start()