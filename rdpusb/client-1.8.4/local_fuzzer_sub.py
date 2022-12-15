#sub.py
import threading
import os
import time
import subprocess
from ctypes import *

crash_count = 0
UUID = "win10"  

def crash_log(out, err, ret, filename):
    path, file_extension = os.path.splitext(filename)
    crashFilename = "crash.log"
    f = open(crashFilename, "w")
    f.write("Crash detected:  \n\n")
    f.write("STDERR :\n")
    f.write(str(err, 'utf-8') + "\n\n\n")
    f.write("STDOUT :\n")
    f.write(str(out, 'utf-8') + "\n\n\n")
    f.write("RET :\n")
    f.write(str(ret) + "\n\n\n")
    f.close() 


def start_vBox():
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

def checkRunning( cmd):
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

if __name__=="__main__":
    while(1):
        vBoxRunning = checkRunning("ps -ef | grep VBoxHeadless | grep -v grep")
        if vBoxRunning == False:
            start_vBox()
        
        time.sleep(1)
	
	