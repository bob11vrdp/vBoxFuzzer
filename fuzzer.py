import random
import sys
import struct
import threading
import os
import shutil
import time
import getopt
import subprocess
import shlex

class Fuzzer:
	def __init__(self):
		self.vBoxRunning = False
		self.clientRunning = False
		self.iteration = 0
		self.pid = None
		self.UUID = "eaf256f8-9e66-4917-a8f6-14f85edacf9f" #release
#		self.UUID = "7b43c58b-d163-4923-beac-f9947cc8a7ce" #debug


	def check_env(self):		

		cmd = "LD_LIBRARY_PATH=/opt/qt56/lib ./VBoxManage list vms"
		proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)		

		try:
			outs, error = proc.communicate()
			print("returnCode :",  proc.returncode)		
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))
			
			#vmCount = len(outs.split(b'\n'))-1
			#if vmCount ==0 :
			#	print("vm count : ", vmCount)
			#	cmd = "LD_LIBRARY_PATH=/opt/qt56/lib ./VirtualBox"
			#	out2 = subprocess.run(cmd, shell=True)
			#	return False
		
		except Exception as e:
			print(e)
			return False		
		
		return True
	

	def start_xfreeRDP(self):
		print("================ xfreeRDP start===============================")		
		cmd  = "xfreerdp /u:son /p:1234 /v:127.0.0.1 /audio /sound -sec-tls -sec-nla"
		#cmd  = "xfreerdp /u:rdp /p:rdp /v:192.168.226.139 /audio /sound -sec-tls -sec-nla"  
		args = shlex.split(cmd)		
		proc  = subprocess.Popen(args, shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		self.rdp_proc = proc
		self.clientRunning = True

		try:
			outs, error = proc.communicate(timeout=10)#timeout=7
			print("returnCode :",  proc.returncode)		
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))
				
		except subprocess.TimeoutExpired as e:
			print("[TimeoutExpired ] :\n", e)
			proc.kill()
			self.clientRunning = False
		except Exception as e:
			self.clientRunning = False
			self.vBoxRunning = False

			print("[start_xfreeRDP  Fail ] :\n", e)
			proc.kill()


	def start_vBox(self):
		print("================ VBoxHeadless start ===============================")		
		cmd  = "LD_LIBRARY_PATH=/opt/qt56/lib ./VBoxHeadless --startvm " + self.UUID + " --vrde on"
		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		try:
			outs, error = proc.communicate()
			print("returnCode :",  proc.returncode)
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))			

			if proc.returncode == 1 or proc.returncode == 23:
				print("vBox is running...")
				self.vBoxRunning = True
		except Exception as e:
			print("\n[Exception ] :\n", e)
			self.vBoxRunning = False
			self.clientRunning = False
			return

		#self.vBoxRunning = True


	def exec_cmd(self, cmd):
		print("**** exec_cmd ==> ", cmd )

		proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		try:
			outs, error = proc.communicate()
			print("returnCode :",  proc.returncode)
			print("stdout :", outs.decode("utf-8"))
			print("stderr :", error.decode("utf-8"))			
		except Exception as e:
			print("\n[Exception ] :\n", e)



	def stop_vBox(self):		
	
		print("\n================ VBoxHeadless stop ===============================")
		cmd = "./VBoxManage controlvm " + self.UUID + " savestate"
		args = shlex.split(cmd)
		out = subprocess.run(args, shell=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		print("\n",out)

		try:
			rs = out.check_returncode()
		except subprocess.CalledProcessError as e: #not running
			print("\nout.returncode :" , out.returncode)
			print("[CalledProcessError ] :\n", e)		
			self.vBoxRunning = False	
			return
		#except Exception as e:
		#	print("\n[Exception ] :\n", e)
		#	return
		

		self.vBoxRunning = False #stop success


	def start(self):
		while True:
			
			if self.vBoxRunning == False:
				vBox_thread = threading.Thread(target=self.start_vBox)
				vBox_thread.setDaemon(0)
				vBox_thread.start()

			else: #vBox is running
			
				if self.clientRunning == False:
					xfreeRDP_thread = threading.Thread(target=self.start_xfreeRDP)
					xfreeRDP_thread.setDaemon(0)
					xfreeRDP_thread.start()
				else: 
					#self.exec_cmd("./testcase/tstLow")
					self.exec_cmd("./testcase/tstRTSymlink")
					self.exec_cmd("./testcase/tstPage")
					self.exec_cmd("./testcase/tstUsbMouse")
					self.exec_cmd("./testcase/tstRTMath")

					self.exec_cmd("./testcase/tstRTS3")

				
					#self.exec_cmd("./testcase/tstRTS3")

					#self.rdp_proc.kill()

				self.iteration += 1
			
				

			#############################
			_counter = 1
			while _counter < 4:
				char = "*"
				time.sleep(0.5)
				char = char * _counter
				print(char)
				_counter += 1
				

if __name__=="__main__":
	fuzzer=Fuzzer()
	rs = fuzzer.check_env()
	if rs==True:
		fuzzer.start()


