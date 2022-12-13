/* $Id: rdpusb.c $ */
/** @file
 * Remote Desktop Protocol client - USB Channel Process Functions
 */

/*
 * Copyright (C) 2006-2020 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

/* DEBUG is defined in ../rdesktop.h */
#ifdef DEBUG
# define VBOX_DEBUG DEBUG
#endif
#include "../rdesktop.h"
#undef DEBUG
#ifdef VBOX_DEBUG
# define DEBUG VBOX_DEBUG
#endif

#include "vrdpusb.h"
//#include "USBProxyDevice.h"
//#include "USBGetDevices.h"

#include <iprt/cdefs.h>
#include <iprt/types.h>
#include <iprt/errcore.h>
#include <iprt/log.h>

#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>


#define RDPUSB_REQ_OPEN              (0)
#define RDPUSB_REQ_CLOSE             (1)
#define RDPUSB_REQ_RESET             (2)
#define RDPUSB_REQ_SET_CONFIG        (3)
#define RDPUSB_REQ_CLAIM_INTERFACE   (4)
#define RDPUSB_REQ_RELEASE_INTERFACE (5)
#define RDPUSB_REQ_INTERFACE_SETTING (6)
#define RDPUSB_REQ_QUEUE_URB         (7)
#define RDPUSB_REQ_REAP_URB          (8)
#define RDPUSB_REQ_CLEAR_HALTED_EP   (9)
#define RDPUSB_REQ_CANCEL_URB        (10)
#define RDPUSB_REQ_DEVICE_LIST       (11)
#define RDPUSB_REQ_NEGOTIATE         (12)

static VCHANNEL *rdpusb_channel;


static const struct
{
    
    const char *pcszRoot;
    
    const char *pcszDevices;
} g_usbfsPaths[] =
{
    { "/proc/bus/usb", "/proc/bus/usb/devices" },
    { "/dev/bus/usb",  "/dev/bus/usb/devices" }
};


static const char *g_pcszDevicesRoot = NULL;
static bool g_fUseSysfs = false;

//static PUSBDEVICE g_pUsbDevices = NULL;

/* A device list entry */
#pragma pack (1)
typedef struct _DevListEntry
{
	uint16_t oNext;                /* Offset of the next structure. 0 if last. */
	uint32_t id;                   /* Identifier of the device assigned by the client. */
	uint16_t bcdUSB;               /* USB verion number. */
	uint8_t bDeviceClass;          /* Device class. */
	uint8_t bDeviceSubClass;       /* Device subclass. */
	uint8_t bDeviceProtocol;       /* Device protocol. */
	uint16_t idVendor;             /* Vendor id. */
	uint16_t idProduct;            /* Product id. */
	uint16_t bcdRev;               /* Revision. */
	uint16_t oManufacturer;        /* Offset of manufacturer string. */
	uint16_t oProduct;             /* Offset of product string. */
	uint16_t oSerialNumber;        /* Offset of serial string. */
	uint16_t idPort;               /* Physical USB port the device is connected to. */
} DevListEntry;
#pragma pack ()






enum {
    /** The space we set aside for the USB strings.  Should always be enough,
     * as a USB device contains up to 256 characters of UTF-16 string data. */
    MAX_STRINGS_LEN = 1024,
    /** The space we reserve for each wire format device entry */
    DEV_ENTRY_SIZE = sizeof(DevListEntry) + MAX_STRINGS_LEN
};



static void addStringToEntry(char *pBuf, uint16_t iBuf, const char *pcsz,
                             uint16_t *piString, uint16_t *piNext)
{
    size_t cch;

    *piString = 0;
    *piNext = iBuf;
    if (!pcsz)
        return;
    cch = strlen(pcsz) + 1;
    if (cch > DEV_ENTRY_SIZE - iBuf)
        return;
    strcpy(pBuf + iBuf, pcsz);
    *piString = iBuf;
    *piNext = iBuf + cch;
}







static STREAM rdpusb_init_packet(uint32 len, uint8 code)
{
	STREAM s;

	s = channel_init(rdpusb_channel, len + 5);
	out_uint32_le (s, len + sizeof (code)); /* The length of data after the 'len' field. */
	out_uint8(s, code);
	return s;
}

static void rdpusb_send(STREAM s)
{

	fprintf(stdout, ("\n ### RDPUSB send:\n"));
	hexdump(s->channel_hdr + 8, s->end - s->channel_hdr - 8);


	channel_send(s, rdpusb_channel);
}
/*
static void rdpusb_send_reply (uint8_t code, uint8_t status, uint32_t devid)
{
	STREAM s = rdpusb_init_packet(5, code);
	out_uint8(s, status);
	out_uint32_le(s, devid);
	s_mark_end(s);
	rdpusb_send(s);
}


*/

//static PUSBPROXYDEV g_proxies = NULL;



static void rdpusb_process(STREAM s)
{
	int rc;

	uint32 len;
	uint8 code;
	uint32 devid;

	//PUSBPROXYDEV proxy = NULL;

#ifdef RDPUSB_DEBUG
	Log(("RDPUSB recv:\n"));
	hexdump(s->p, s->end - s->p);
#endif

	in_uint32_le (s, len);
	if (len > s->end - s->p)
	{
		error("RDPUSB: not enough data len = %d, bytes left %d\n", len, s->end - s->p);
		return;
	}

	in_uint8(s, code);

	Log(("RDPUSB recv: len = %d, code = %d\n", len, code));

	switch (code)
	{
		case RDPUSB_REQ_OPEN:
		{
			
		} break;

		

		case RDPUSB_REQ_RESET:
		{
	       
		} break;

		case RDPUSB_REQ_SET_CONFIG:
		{
			
		} break;

		case RDPUSB_REQ_CLAIM_INTERFACE:
		{
			
		} break;

		case RDPUSB_REQ_RELEASE_INTERFACE:
		{
			
		} break;

		case RDPUSB_REQ_INTERFACE_SETTING:
		{
		
		} break;

		case RDPUSB_REQ_QUEUE_URB:
		{
			
		} break;

		case RDPUSB_REQ_REAP_URB:
		{
			
		} break;

		case RDPUSB_REQ_CLEAR_HALTED_EP:
		{
			
		} break;

		case RDPUSB_REQ_CANCEL_URB:
		{
			
		} break;

		case RDPUSB_REQ_DEVICE_LIST:
		{
			/*void *buf = NULL;
			int len = 0;

			buf = build_device_list (&len);

			s = rdpusb_init_packet(len? len: 2, code);
			if (len)
			{
				out_uint8p (s, buf, len);
			}
			else
			{
				out_uint16_le(s, 0);
			}
			s_mark_end(s);
			rdpusb_send(s);

			if (buf)
			{
				free (buf);
			}*/
		} break;

		case RDPUSB_REQ_NEGOTIATE:
		{
		/*	s = rdpusb_init_packet(1, code);
			out_uint8(s, VRDP_USB_CAPS_FLAG_ASYNC);
			s_mark_end(s);
			rdpusb_send(s);
			*/
		} break;

		default:
			unimpl("RDPUSB code %d\n", code);
			break;
	}
}


void fuzz_init(void)
{


    bool fUseUsbfs;	
	//RD_BOOL rs = USBProxyLinuxChooseMethod(&fUseUsbfs, &g_pcszDevicesRoot);
	

	g_pcszDevicesRoot = "/dev/vboxusb";
    //if (RT_SUCCESS(rs))
	{
	    g_fUseSysfs = !fUseUsbfs;
	    rdpusb_channel =  channel_register("vrdpusb", CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,    rdpusb_process);
		fprintf(stdout, "[rdpusb.c] - test_rdpusb_init() rdpusb_channel : %p\n", rdpusb_channel);
	    //return rdpusb_channel;
	}

	//return NULL;
}

 void fuzz_device_list(char *buf)
{
	//char* buf = "a羲羲羲羲羲羲羲羲羲羲";

	int len = strlen(buf);
	uint8 code = RDPUSB_REQ_DEVICE_LIST;
	
	STREAM s = channel_init(rdpusb_channel,  len + 5);		
	out_uint32_le(s, len + sizeof(code));
	out_uint8    (s, code);

	if (len)
	{
		out_uint8p(s, buf, len);		
	}
		
	s_mark_end(s);
	fprintf(stdout,"\n !!!!!!!!!! rdpusb_channel :  %p", rdpusb_channel );
	rdpusb_send(s);	
	
}



RD_BOOL rdpusb_init(void)
{
    bool fUseUsbfs;

	//int rs = USBProxyLinuxChooseMethod(&fUseUsbfs, &g_pcszDevicesRoot);
	g_pcszDevicesRoot = "/dev/vboxusb";
	fprintf(stdout, "########### g_pcszDevicesRoot : %s \n", g_pcszDevicesRoot);
    //if (RT_SUCCESS(rs))
	{
	    g_fUseSysfs = !fUseUsbfs;
	    rdpusb_channel =
		    channel_register("vrdpusb", CHANNEL_OPTION_INITIALIZED | CHANNEL_OPTION_ENCRYPT_RDP,
				     rdpusb_process);
	    return (rdpusb_channel != NULL);
	}
	return false;
}

void rdpusb_close (void)
{
	/*PUSBPROXYDEV proxy = g_proxies;

	while (proxy)
	{
		PUSBPROXYDEV pNext = proxy->pNext;

		Log(("RDPUSB: closing proxy %p\n", proxy));

		op_usbproxy_back_close(proxy);
		xfree (proxy);

		proxy = pNext;
	}
*/
	return;
}
