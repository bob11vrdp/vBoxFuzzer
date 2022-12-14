#test.py
from ctypes import *
import threading
import os
import time
import subprocess


def exec_radamsa():
	cmd  = "echo 'aaaa' | radamsa"
	#print("\n>>>", cmd)
	proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	try:
		outs, error = proc.communicate()			
		#print("fuzzed data :", outs.decode("utf-8"))
		return outs
	except Exception as e:
		print(e)			
		return b""
			
libc = cdll.LoadLibrary("./librdesktop.so")



while(1):				
	try:
		outs = exec_radamsa()				
		
		libc.fuzz_connect(b"127.0.0.1", outs)
		#print("live :", libc.isConnected())
		#libc.fuzz_device_list(outs)
		time.sleep(1)	
	except Exception as e:
			print("[Exception] : ", e)

