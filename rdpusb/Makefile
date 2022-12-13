# VirtualBox-6.1.36/src/VBox/RDP/client-1.8.4/Makefile
# rdesktop: A Remote Desktop Protocol client
# Makefile.in
# Copyright (C) Matthew Chapman 1999-2007
#

prefix      = /usr/local
exec_prefix = ${prefix}
bindir      = ${exec_prefix}/bin
mandir      = ${datarootdir}/man
datarootdir = ${prefix}/share
datadir     = ${datarootdir}

VERSION     = 1.8.4
KEYMAP_PATH = $(datadir)/rdesktop/keymaps/

MY_ROOT  	= /home/son/VBX/VirtualBox-6.1.36
HEADER_PATH = -I$(MY_ROOT)/src/VBox/Devices/USB/ -I$(MY_ROOT)/include/ -I$(MY_ROOT)/src/VBox/Main/include/ -I$(MY_ROOT)/src/VBox/Runtime/include/
LIB_DEF  	= -DIN_RING3 -DRDESKTOP -DWITH_RDPUSB -DVBOX_USE_WITH_SYSFS -DVBOX_USE_WITH_INOTIFY
OBJ_PATH 	=  $(MY_ROOT)/out/linux.amd64/release/obj


CC          = gcc
INSTALL     = /usr/bin/install -c
CFLAGS      = -g -fPIC -O2 -Wall -I/usr/include     -DPACKAGE_NAME=\"rdesktop\" -DPACKAGE_TARNAME=\"rdesktop\" -DPACKAGE_VERSION=\"1.8.4\" -DPACKAGE_STRING=\"rdesktop\ 1.8.4\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DL_ENDIAN=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_LOCALE_H=1 -DHAVE_LANGINFO_H=1 -DHAVE_SYSEXITS_H=1 -Dssldir=\"/usr\" -DHAVE_XRANDR=1 -DEGD_SOCKET=\"/var/run/egd-pool\" -DWITH_RDPSND=1 -DRDPSND_OSS=1 -DRDPSND_ALSA=1 -DHAVE_DIRENT_H=1 -DHAVE_DIRFD=1 -DHAVE_DECL_DIRFD=1 -DHAVE_ICONV_H=1 -DHAVE_ICONV=1 -DICONV_CONST= -DHAVE_SYS_VFS_H=1 -DHAVE_SYS_STATVFS_H=1 -DHAVE_SYS_STATFS_H=1 -DHAVE_SYS_PARAM_H=1 -DHAVE_SYS_MOUNT_H=1 -DSTAT_STATVFS=1 -DHAVE_STRUCT_STATVFS_F_NAMEMAX=1 -DHAVE_STRUCT_STATFS_F_NAMELEN=1 -DHAVE_MNTENT_H=1 -DHAVE_SETMNTENT=1 -DKEYMAP_PATH=\"$(KEYMAP_PATH)\" $(HEADER_PATH) $(LIB_DEF)
LDFLAGS     =  -L/usr/lib -L/usr/lib64 -lssl -lcrypto  -lXrandr  -lasound   
STRIP       = strip

TARGETS     = rdesktop 
VNCINC      = 
LDVNC       = 
VNCLINK     = 
SOUNDOBJ    =  rdpsnd.o rdpsnd_dsp.o rdpsnd_oss.o rdpsnd_alsa.o
SCARDOBJ    = 
CREDSSPOBJ  = 

RDPOBJ   = tcp.o asn.o iso.o mcs.o secure.o licence.o rdp.o bitmap.o channels.o     mppc.o    ssl.o utils.o #serial.o
X11OBJ   = rdesktop.o  #xwin.o cliprdr.o ctrl.o xclip.o   xkeymap.o ewmhints.o  
VNCOBJ   = vnc/rdp2vnc.o vnc/vnc.o vnc/xkeymap.o vnc/x11stubs.o
USB_LD 	 = -L./VBoxRT.so
USBOBJ   	= vrdp/rdpusb.o   
#USBGetDevices.o USBProxyDevice-linux.o  USBLib.o  $(OBJ_PATH)/VBoxRT/r3/linux/sysfs.o  
#$(OBJ_PATH)/VBoxRT/common/string/stringalloc.o  $(OBJ_PATH)/VBoxRT/r3/posix/env-posix.o $(OBJ_PATH)/VBoxRT/common/string/utf-8-case.o $(OBJ_PATH)/VBoxRT/common/string/unidata-upper.o  $(OBJ_PATH)/VBoxRT/common/string/unidata-lower.o  $(OBJ_PATH)/VBoxRT/r3/alloc.o  $(OBJ_PATH)/VBoxRT/r3/posix/dir-posix.o $(OBJ_PATH)/VBoxRT/r3/posix/path-posix.o $(OBJ_PATH)/VBoxRT/common/log/log.o #VBoxRTImp.so 

.PHONY: all
all: $(TARGETS)

rdesktop:  $(RDPOBJ) $(USBOBJ)
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF) -o rdesktop $(USBOBJ)  $(RDPOBJ)   $(LDFLAGS) -lX11

USBGetDevices.o : 
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF) -o USBGetDevices.o -c $(MY_ROOT)/src/VBox/Main/src-server/linux/USBGetDevices.cpp 

# $(USB_LD) -L/home/son//VBX/VirtualBox-6.1.36/out/linux.amd64/release/bin/VBoxRT.so 

USBProxyDevice-linux.o: 
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF) -o USBProxyDevice-linux.o -c $(MY_ROOT)/src/VBox/Devices/USB/linux/USBProxyDevice-linux.cpp 

#-L/home/son//VBX/VirtualBox-6.1.36/out/linux.amd64/release/bin/VBoxRT.so 

librdesktop: $(RDPOBJ)  $(USBOBJ) $(X11OBJ)
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF)-fPIC -shared -o librdesktop.so $(X11OBJ) $(USBOBJ) $(RDPOBJ) $(LDFLAGS)$(USB_LD)   -lX11 
#-L/home/son//VBX/VirtualBox-6.1.36/out/linux.amd64/release/bin/VBoxRT.so 

rdp2vnc: $(VNCOBJ) $(SOUNDOBJ) $(RDPOBJ) $(SCARDOBJ) $(CREDSSPOBJ)
	$(VNCLINK) $(CFLAGS) -o rdp2vnc $(VNCOBJ) $(SOUNDOBJ) $(RDPOBJ) $(SCARDOBJ) $(CREDSSPOBJ) $(LDFLAGS) $(LDVNC)

vnc/rdp2vnc.o: rdesktop.c
	$(CC) $(CFLAGS) $(VNCINC) -DRDP2VNC -o vnc/rdp2vnc.o -c rdesktop.c

vnc/vnc.o: vnc/vnc.c
	$(CC) $(CFLAGS) $(VNCINC) -DRDP2VNC -o vnc/vnc.o -c vnc/vnc.c

vnc/xkeymap.o: xkeymap.c
	$(CC) $(CFLAGS) $(VNCINC) -DRDP2VNC -o vnc/xkeymap.o -c xkeymap.c

vnc/x11stubs.o: vnc/x11stubs.c
	$(CC) $(CFLAGS) $(VNCINC) -o vnc/x11stubs.o -c vnc/x11stubs.c



VBoxRTImp.so :
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF) -fPIC -shared -o VBoxRTImp.so -c $(MY_ROOT)/out/linux.amd64/release/obj/VBoxRTImp/VBoxRTImpImp.c



USBLib.o : 
	$(CC) $(CFLAGS) $(HEADER_PATH) $(LIB_DEF) -o USBLib.o -c $(MY_ROOT)/src/VBox/HostDrivers/VBoxUSB/USBLib.cpp

.PHONY: install
install: installbin installkeymaps installman

.PHONY: installbin
installbin: rdesktop
	mkdir -p $(DESTDIR)$(bindir)
	$(INSTALL) rdesktop $(DESTDIR)$(bindir)
	$(STRIP) $(DESTDIR)$(bindir)/rdesktop
	chmod 755 $(DESTDIR)$(bindir)/rdesktop

.PHONY: installman
installman: doc/rdesktop.1
	mkdir -p $(DESTDIR)$(mandir)/man1
	cp doc/rdesktop.1 $(DESTDIR)$(mandir)/man1
	chmod 644 $(DESTDIR)$(mandir)/man1/rdesktop.1

.PHONY: installkeymaps
installkeymaps:
	mkdir -p $(DESTDIR)$(KEYMAP_PATH)
# Prevent copying the CVS directory
	cp keymaps/?? keymaps/??-?? $(DESTDIR)$(KEYMAP_PATH)
	cp keymaps/common $(DESTDIR)$(KEYMAP_PATH)
	cp keymaps/modifiers $(DESTDIR)$(KEYMAP_PATH)
	chmod 644 $(DESTDIR)$(KEYMAP_PATH)/*

.PHONY: proto
proto:
	cat proto.head > proto.h
	cproto -DMAKE_PROTO \
	bitmap.c cache.c channels.c cliprdr.c disk.c mppc.c ewmhints.c	\
	iso.c licence.c mcs.c orders.c parallel.c printer.c printercache.c \
	pstcache.c rdesktop.c rdp5.c rdp.c rdpdr.c rdpsnd.c \
	secure.c serial.c tcp.c xclip.c xkeymap.c xwin.c lspci.c seamless.c \
	scard.c >> proto.h
	cat proto.tail >> proto.h

.PHONY: clean
clean:
	rm -f *.o *~ vnc/*.o vnc/*~ rdesktop rdp2vnc

.PHONY: distclean
distclean: clean
	rm -rf autom4te.cache config.log config.status Makefile rdesktop-$(VERSION).tar.gz

.PHONY: dist
dist: rdesktop-$(VERSION).tar.gz

rdesktop-$(VERSION).tar.gz: Makefile configure
	mkdir -p /tmp/rdesktop-make-dist-dir
	ln -sf `pwd` /tmp/rdesktop-make-dist-dir/rdesktop-$(VERSION)
	(cd /tmp/rdesktop-make-dist-dir; \
	tar zcvf rdesktop-$(VERSION)/rdesktop-$(VERSION).tar.gz \
	rdesktop-$(VERSION)/COPYING \
	rdesktop-$(VERSION)/README \
	rdesktop-$(VERSION)/configure \
	rdesktop-$(VERSION)/configure.ac \
	rdesktop-$(VERSION)/config.sub \
	rdesktop-$(VERSION)/config.guess \
	rdesktop-$(VERSION)/bootstrap \
	rdesktop-$(VERSION)/install-sh \
	rdesktop-$(VERSION)/Makefile.in \
	rdesktop-$(VERSION)/rdesktop.spec \
	rdesktop-$(VERSION)/*.c \
	rdesktop-$(VERSION)/*.h \
	rdesktop-$(VERSION)/proto.head \
	rdesktop-$(VERSION)/proto.tail \
	rdesktop-$(VERSION)/keymaps/?? \
	rdesktop-$(VERSION)/keymaps/??-?? \
	rdesktop-$(VERSION)/keymaps/common \
	rdesktop-$(VERSION)/keymaps/modifiers \
	rdesktop-$(VERSION)/keymaps/convert-map \
	rdesktop-$(VERSION)/doc/HACKING \
	rdesktop-$(VERSION)/doc/AUTHORS \
	rdesktop-$(VERSION)/doc/TODO \
	rdesktop-$(VERSION)/doc/ChangeLog \
	rdesktop-$(VERSION)/doc/keymapping.txt \
	rdesktop-$(VERSION)/doc/keymap-names.txt \
	rdesktop-$(VERSION)/doc/ipv6.txt \
	rdesktop-$(VERSION)/doc/licensing.txt \
	rdesktop-$(VERSION)/doc/patches.txt \
	rdesktop-$(VERSION)/doc/redirection.txt \
	rdesktop-$(VERSION)/doc/rdesktop.1 )
	rm -rf /tmp/rdesktop-make-dist-dir

.PHONY: dist-noversion
dist-noversion: rdesktop.tar.gz

rdesktop.tar.gz: rdesktop-$(VERSION).tar.gz
	mkdir -p /tmp/rdesktop-make-dist-dir
	tar zxvf $< -C /tmp/rdesktop-make-dist-dir
	mv /tmp/rdesktop-make-dist-dir/rdesktop-$(VERSION) /tmp/rdesktop-make-dist-dir/rdesktop
	ls /tmp/rdesktop-make-dist-dir/rdesktop
	tar zcvf $@ -C /tmp/rdesktop-make-dist-dir rdesktop
	rm -rf /tmp/rdesktop-make-dist-dir

Makefile: Makefile.in configure
	./config.status

configure: configure.ac
	./bootstrap

.SUFFIXES:
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: doc/AUTHORS
doc/AUTHORS:
	./genauthors *.c