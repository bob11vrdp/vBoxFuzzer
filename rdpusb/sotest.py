#test.py
from ctypes import *
import threading
import os
import time
import subprocess


def exec_radamsa():
	cmd  = "echo 'aaaaaaaaaaaaasssssssssssddddddddddddddffffffffffffwwwwwwwwwweeeeeeeee' | radamsa"
	print("\n>>>", cmd)
	proc  = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

	try:
		outs, error = proc.communicate()			
		print("stdout :", outs.decode("utf-8"))
		return outs
	except Exception as e:
		print(e)			
		return ""
			
libc = cdll.LoadLibrary("./librdesktop.so")
print(libc)

libc.wrap_main("abcd")

while(1):				
	outs = exec_radamsa()					
	libc.fuzz_device_list(outs)
	time.sleep(0.4)	
