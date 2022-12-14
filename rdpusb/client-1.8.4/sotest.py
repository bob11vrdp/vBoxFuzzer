#test.py
from ctypes import *
import threading
import os
import time
import subprocess
import sys


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

ip_addr = ""
if len(sys.argv) < 2:
	ip_addr = b"127.0.0.1"
else:
	ip_addr = sys.argv[1].encode('ascii')
	
print("ip addr : ", ip_addr)

libc = cdll.LoadLibrary("./librdesktop.so")
libc.fuzz_connect(ip_addr)
while(1):				
	try:
		print(libc)
		outs = exec_radamsa()	
		libc.fuzz_device_list(outs)					
		
		#print("live :", libc.isConnected())
		#libc.fuzz_device_list(outs)
		time.sleep(1)	
	except Exception as e:
			print("[Exception] : ", e)
