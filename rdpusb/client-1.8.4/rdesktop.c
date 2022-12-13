

#include <stdarg.h>		/* va_list va_start va_end */
#include <unistd.h>		/* read close getuid getgid getpid getppid gethostname */
#include <fcntl.h>		/* open */
#include <pwd.h>		/* getpwuid */
#include <termios.h>		/* tcgetattr tcsetattr */
#include <sys/stat.h>		/* stat */
#include <sys/time.h>		/* gettimeofday */
#include <sys/times.h>		/* times */
#include <ctype.h>		/* toupper */
#include <limits.h>
#include <errno.h>
#include <signal.h>
#include "rdesktop.h"

#ifdef VBOX
# include <VBox/version.h>
# include <iprt/log.h>
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef HAVE_ICONV
#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif
#endif

#ifdef EGD_SOCKET
#include <sys/types.h>
#include <sys/socket.h>		/* socket connect */
#include <sys/un.h>		/* sockaddr_un */
#endif

#include "ssl.h"
#if defined(VBOX) && defined(OPENSSL_MANGLER)
# include <iprt/initterm.h>
#endif

/* Reconnect timeout based on approxiamted cookie life-time */
#define RECONNECT_TIMEOUT (3600+600)
#define RDESKTOP_LICENSE_STORE "/.local/share/rdesktop/licenses"

uint8 g_static_rdesktop_salt_16[16] = {
	0xb8, 0x82, 0x29, 0x31, 0xc5, 0x39, 0xd9, 0x44,
	0x54, 0x15, 0x5e, 0x14, 0x71, 0x38, 0xd5, 0x4d
};

char g_title[64] = "";
char *g_username;
char g_password[64] = "";
char g_hostname[16] = "";
char g_keymapname[PATH_MAX] = "";
unsigned int g_keylayout = 0x409;	/* Defaults to US keyboard layout */
int g_keyboard_type = 0x4;	/* Defaults to US keyboard layout */
int g_keyboard_subtype = 0x0;	/* Defaults to US keyboard layout */
int g_keyboard_functionkeys = 0xc;	/* Defaults to US keyboard layout */
int g_sizeopt = 0;		/* If non-zero, a special size has been
				   requested. If 1, the geometry will be fetched
				   from _NET_WORKAREA. If negative, absolute value
				   specifies the percent of the whole screen. */
int g_width = 800;
int g_height = 600;
int g_xpos = 0;
int g_ypos = 0;
int g_pos = 0;			/* 0 position unspecified,
				   1 specified,
				   2 xpos neg,
				   4 ypos neg  */
extern int g_tcp_port_rdp;
int g_server_depth = -1;
int g_win_button_size = 0;	/* If zero, disable single app mode */
RD_BOOL g_network_error = False;
RD_BOOL g_bitmap_compression = True;
RD_BOOL g_sendmotion = True;
RD_BOOL g_bitmap_cache = True;
RD_BOOL g_bitmap_cache_persist_enable = False;
RD_BOOL g_bitmap_cache_precache = True;
RD_BOOL g_use_ctrl = True;
RD_BOOL g_encryption = True;
RD_BOOL g_encryption_initial = True;
RD_BOOL g_packet_encryption = True;
RD_BOOL g_desktop_save = True;	/* desktop save order */
RD_BOOL g_polygon_ellipse_orders = True;	/* polygon / ellipse orders */
RD_BOOL g_fullscreen = False;
RD_BOOL g_grab_keyboard = True;
RD_BOOL g_hide_decorations = False;
RDP_VERSION g_rdp_version = RDP_V5;	/* Default to version 5 */
RD_BOOL g_rdpclip = True;
RD_BOOL g_console_session = False;
#ifndef VBOX
RD_BOOL g_numlock_sync = False;
#else /* VBOX */
/* Always use numlock synchronization with VRDP. */
RD_BOOL g_numlock_sync = True;
#endif /* VBOX */
RD_BOOL g_lspci_enabled = False;
RD_BOOL g_owncolmap = False;
RD_BOOL g_ownbackstore = True;	/* We can't rely on external BackingStore */
RD_BOOL g_seamless_rdp = False;
RD_BOOL g_use_password_as_pin = False;
char g_seamless_shell[512];
char g_seamless_spawn_cmd[512];
RD_BOOL g_seamless_persistent_mode = True;
RD_BOOL g_user_quit = False;
uint32 g_embed_wnd;
uint32 g_rdp5_performanceflags =
	RDP5_NO_WALLPAPER | RDP5_NO_FULLWINDOWDRAG | RDP5_NO_MENUANIMATIONS | RDP5_NO_CURSOR_SHADOW;
/* Session Directory redirection */
RD_BOOL g_redirect = False;
char *g_redirect_server;
uint32 g_redirect_server_len;
char *g_redirect_domain;
uint32 g_redirect_domain_len;
char *g_redirect_username;
uint32 g_redirect_username_len;
uint8 *g_redirect_lb_info;
uint32 g_redirect_lb_info_len;
uint8 *g_redirect_cookie;
uint32 g_redirect_cookie_len;
uint32 g_redirect_flags = 0;
uint32 g_redirect_session_id = 0;

uint32 g_reconnect_logonid = 0;
char g_reconnect_random[16];
time_t g_reconnect_random_ts;
RD_BOOL g_has_reconnect_random = False;
RD_BOOL g_reconnect_loop = False;
uint8 g_client_random[SEC_RANDOM_SIZE];
RD_BOOL g_pending_resize = False;




RD_BOOL g_rdpusb = False;


#ifdef WITH_BIRD_VD_HACKS
RD_BOOL g_keep_virtual_desktop_shortcuts = False;
#endif

#ifdef HAVE_ICONV
char g_codepage[16] = "";
#endif

char *g_sc_csp_name = NULL;	/* Smartcard CSP name  */
char *g_sc_reader_name = NULL;
char *g_sc_card_name = NULL;
char *g_sc_container_name = NULL;

extern RDPDR_DEVICE g_rdpdr_device[];

extern char *g_rdpdr_clientname;


static int handle_disconnect_reason(RD_BOOL deactivated, uint16 reason)
{
	char *text;
	int retval;

	switch (reason)
	{
		case exDiscReasonNoInfo:
			text = "No information available";
			if (deactivated)
				retval = EX_OK;
			else
				retval = EXRD_UNKNOWN;
			break;

		case exDiscReasonAPIInitiatedDisconnect:
			text = "Server initiated disconnect";
			retval = EXRD_API_DISCONNECT;
			break;

		case exDiscReasonAPIInitiatedLogoff:
			text = "Server initiated logoff";
			retval = EXRD_API_LOGOFF;
			break;

		case exDiscReasonServerIdleTimeout:
			text = "Server idle timeout reached";
			retval = EXRD_IDLE_TIMEOUT;
			break;

		case exDiscReasonServerLogonTimeout:
			text = "Server logon timeout reached";
			retval = EXRD_LOGON_TIMEOUT;
			break;

		case exDiscReasonReplacedByOtherConnection:
			text = "The session was replaced";
			retval = EXRD_REPLACED;
			break;

		case exDiscReasonOutOfMemory:
			text = "The server is out of memory";
			retval = EXRD_OUT_OF_MEM;
			break;

		case exDiscReasonServerDeniedConnection:
			text = "The server denied the connection";
			retval = EXRD_DENIED;
			break;

		case exDiscReasonServerDeniedConnectionFips:
			text = "The server denied the connection for security reason";
			retval = EXRD_DENIED_FIPS;
			break;

		case exDiscReasonServerInsufficientPrivileges:
			text = "The user cannot connect to the server due to insufficient access privileges.";
			retval = EXRD_INSUFFICIENT_PRIVILEGES;
			break;

		case exDiscReasonServerFreshCredentialsRequired:
			text = "The server does not accept saved user credentials and requires that the user enter their credentials for each connection.";
			retval = EXRD_FRESH_CREDENTIALS_REQUIRED;
			break;

		case exDiscReasonRPCInitiatedDisconnectByUser:
			text = "Disconnect initiated by administration tool";
			retval = EXRD_RPC_DISCONNECT_BY_USER;
			break;

		case exDiscReasonByUser:
			text = "Disconnect initiated by user";
			retval = EXRD_DISCONNECT_BY_USER;
			break;

		case exDiscReasonLicenseInternal:
			text = "Internal licensing error";
			retval = EXRD_LIC_INTERNAL;
			break;

		case exDiscReasonLicenseNoLicenseServer:
			text = "No license server available";
			retval = EXRD_LIC_NOSERVER;
			break;

		case exDiscReasonLicenseNoLicense:
			text = "No valid license available";
			retval = EXRD_LIC_NOLICENSE;
			break;

		case exDiscReasonLicenseErrClientMsg:
			text = "Invalid licensing message";
			retval = EXRD_LIC_MSG;
			break;

		case exDiscReasonLicenseHwidDoesntMatchLicense:
			text = "Hardware id doesn't match software license";
			retval = EXRD_LIC_HWID;
			break;

		case exDiscReasonLicenseErrClientLicense:
			text = "Client license error";
			retval = EXRD_LIC_CLIENT;
			break;

		case exDiscReasonLicenseCantFinishProtocol:
			text = "Network error during licensing protocol";
			retval = EXRD_LIC_NET;
			break;

		case exDiscReasonLicenseClientEndedProtocol:
			text = "Licensing protocol was not completed";
			retval = EXRD_LIC_PROTO;
			break;

		case exDiscReasonLicenseErrClientEncryption:
			text = "Incorrect client license encryption";
			retval = EXRD_LIC_ENC;
			break;

		case exDiscReasonLicenseCantUpgradeLicense:
			text = "Can't upgrade license";
			retval = EXRD_LIC_UPGRADE;
			break;

		case exDiscReasonLicenseNoRemoteConnections:
			text = "The server is not licensed to accept remote connections";
			retval = EXRD_LIC_NOREMOTE;
			break;

		default:
			if (reason > 0x1000 && reason < 0x7fff)
			{
				text = "Internal protocol error";
			}
			else
			{
				text = "Unknown reason";
			}
			retval = EXRD_UNKNOWN;
	}
	if (reason != exDiscReasonNoInfo)
		fprintf(stderr, "disconnect: %s.\n", text);

	return retval;
}

static void
rdesktop_reset_state(void)
{
	rdp_reset_state();

}


static void parse_server_and_port(char *server)
{
	char *p;
	p = strchr(server, ':');
	if (p != NULL)
	{
		g_tcp_port_rdp = strtol(p + 1, NULL, 10);
		*p = 0;
	}


}

#ifdef VBOX
fprintf(stdout, "[!!!!!!!!!!!!! VBOX \n]\n");
/* This disables iprt logging */
DECLEXPORT(PRTLOGGER) RTCALL RTLogDefaultInit(void)
{
    return NULL;
}
#endif



extern void fuzz_init(void);
extern void fuzz_device_list(char *buf);



/* Client program */
extern int wrap_main(char * buf)
{
	int argc = 4;
	char *argv[] = {"./rdesktop-vrdp","127.0.0.1","-r","usb"};

	char server[256];
	char fullhostname[64];
	char domain[256];
	char shell[256];
	char directory[256];
	RD_BOOL prompt_password, deactivated;
	struct passwd *pw;
	uint32 flags, ext_disc_reason = 0;
	char *p;
	int c;
	char *locale = NULL;
	int username_option = 0;
	RD_BOOL geometry_option = False;

#ifdef HAVE_LOCALE_H
	fprintf(stdout, "[!!!!!!!!!!!!! HAVE_LOCALE_H \n]\n");
	/* Set locale according to environment */
	locale = setlocale(LC_ALL, "");
	if (locale)
	{
		locale = xstrdup(locale);
	}

#endif

	/* setup default flags for TS_INFO_PACKET */
	flags = RDP_INFO_MOUSE | RDP_INFO_DISABLECTRLALTDEL
		| RDP_INFO_UNICODE | RDP_INFO_MAXIMIZESHELL | RDP_INFO_ENABLEWINDOWSKEY;

	prompt_password = False;
	g_seamless_spawn_cmd[0] = domain[0] = g_password[0] = shell[0] = directory[0] = 0;
	g_embed_wnd = 0;	
	g_rdpusb = True;

	//STRNCPY(server, "192.168.226.151", sizeof(server));
	STRNCPY(server, "127.0.0.1", sizeof(server));
	parse_server_and_port(server);

	if (!username_option)
	{
		pw = getpwuid(getuid());
		if ((pw == NULL) || (pw->pw_name == NULL))
		{
			error("could not determine username, use -u\n");
			return EX_OSERR;
		}
		/* +1 for trailing \0 */
		int pwlen = strlen(pw->pw_name) + 1;
		g_username = (char *) xmalloc(pwlen);
		STRNCPY(g_username, pw->pw_name, pwlen);
	}
	
#ifdef HAVE_ICONV

	fprintf(stdout, "HAVE_ICONV\n");
	if (g_codepage[0] == 0)
	{
		if (setlocale(LC_CTYPE, ""))
		{
			STRNCPY(g_codepage, nl_langinfo(CODESET), sizeof(g_codepage));
		}
		else
		{
			STRNCPY(g_codepage, DEFAULT_CODEPAGE, sizeof(g_codepage));
		}
	}
#endif

	if (g_hostname[0] == 0)
	{
		if (gethostname(fullhostname, sizeof(fullhostname)) == -1)
		{
			error("could not determine local hostname, use -n\n");
			return EX_OSERR;
		}

		p = strchr(fullhostname, '.');
		if (p != NULL)
			*p = 0;

		STRNCPY(g_hostname, fullhostname, sizeof(g_hostname));
	}
	
	fprintf(stdout, "222222\n");
	/*if (g_keymapname[0] == 0)
	{
		//if (locale && xkeymap_from_locale(locale))
		{
			//fprintf(stderr, "Autoselected keyboard map %s\n", g_keymapname);
		}
		//else
		{
			STRNCPY(g_keymapname, "en-us", sizeof(g_keymapname));
		}
	}
	if (locale)
		xfree(locale);
		*/
	
	//if (!ui_init())
		//return EX_OSERR;

	fuzz_init();
	fprintf(stdout, "44444\n");

	g_reconnect_loop = False;
	while (1)
	{
		rdesktop_reset_state();
		fprintf(stdout, "555\n");
		if (g_redirect)
		{
			STRNCPY(domain, g_redirect_domain, sizeof(domain));
			xfree(g_username);
			g_username = (char *) xmalloc(strlen(g_redirect_username) + 1);
			strcpy(g_username, g_redirect_username);
			STRNCPY(server, g_redirect_server, sizeof(server));
			flags |= RDP_INFO_AUTOLOGON;

			fprintf(stderr, "Redirected to %s@%s session %d.\n",
				g_redirect_username, g_redirect_server, g_redirect_session_id);

		
			g_network_error = False;
			g_redirect = False;
		}
		
		//ui_init_connection();
		
		int rs =  rdp_connect(server, flags, domain, g_password, shell, directory, g_reconnect_loop);
		fprintf(stdout, "rs : %d\n", rs);
		if (!rs)
		{

			g_network_error = False;

			if (g_reconnect_loop == False)
				return EX_PROTOCOL;

			/* check if auto reconnect cookie has timed out */
			if (time(NULL) - g_reconnect_random_ts > RECONNECT_TIMEOUT)
			{
				fprintf(stderr, "Tried to reconnect for %d minutes, giving up.\n",
					RECONNECT_TIMEOUT / 60);
				return EX_PROTOCOL;
			}

			sleep(4);
			continue;
		}

		if (g_redirect)
		{
			rdp_disconnect();
			continue;
		}		

		DEBUG(("Connection successful.\n"));
		fprintf(stdout, "777\n");

		rd_create_ui();
		tcp_run_ui(True);

		deactivated = False;
		g_reconnect_loop = False;
		rdp_main_loop(&deactivated, &ext_disc_reason);
		fprintf(stdout, "888\n");
		tcp_run_ui(False);

		
		DEBUG(("Disconnecting...\n"));
		//rdp_disconnect();

		fprintf(stdout, "888\n");
		if (g_redirect)
			continue;

		/* handle network error and start autoreconnect */
		if (g_network_error && !deactivated)
		{
			fprintf(stderr,
				"Disconnected due to network error, retrying to reconnect for %d minutes.\n",
				RECONNECT_TIMEOUT / 60);
			g_network_error = False;
			g_reconnect_loop = True;
			continue;
		}

		//ui_seamless_end();
		//ui_destroy_window();

		/* Enter a reconnect loop if we have a pending resize request */
		if (g_pending_resize)
		{
			g_pending_resize = False;
			g_reconnect_loop = True;
			continue;
		}
		break;
	}

	//cache_save_state();
	//ui_deinit();


    //rdpusb_close();

	
	if (g_user_quit)
	{
		fprintf(stdout, "999\n");
		return EXRD_WINDOW_CLOSED;
	}
	return handle_disconnect_reason(deactivated, ext_disc_reason);
}


#ifdef EGD_SOCKET
/* Read 32 random bytes from PRNGD or EGD socket (based on OpenSSL RAND_egd) */
static RD_BOOL
generate_random_egd(uint8 * buf)
{
	struct sockaddr_un addr;
	RD_BOOL ret = False;
	int fd;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1)
		return False;

	addr.sun_family = AF_UNIX;
	memcpy(addr.sun_path, EGD_SOCKET, sizeof(EGD_SOCKET));
	if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		goto err;

	/* PRNGD and EGD use a simple communications protocol */
	buf[0] = 1;		/* Non-blocking (similar to /dev/urandom) */
	buf[1] = 32;		/* Number of requested random bytes */
	if (write(fd, buf, 2) != 2)
		goto err;

	if ((read(fd, buf, 1) != 1) || (buf[0] == 0))	/* Available? */
		goto err;

	if (read(fd, buf, 32) != 32)
		goto err;

	ret = True;

      err:
	close(fd);
	return ret;
}
#endif

/* Generate a 32-byte random for the secure transport code. */
void generate_random(uint8 * random)
{
	struct stat st;
	struct tms tmsbuf;
	RDSSL_MD5 md5;
	uint32 *r;
	int fd, n;

	/* If we have a kernel random device, try that first */
	if (((fd = open("/dev/urandom", O_RDONLY)) != -1)
	    || ((fd = open("/dev/random", O_RDONLY)) != -1))
	{
		n = read(fd, random, 32);
		close(fd);
		if (n == 32)
			return;
	}

#ifdef EGD_SOCKET
	/* As a second preference use an EGD */
	if (generate_random_egd(random))
		return;
#endif

	/* Otherwise use whatever entropy we can gather - ideas welcome. */
	r = (uint32 *) random;
	r[0] = (getpid()) | (getppid() << 16);
	r[1] = (getuid()) | (getgid() << 16);
	r[2] = times(&tmsbuf);	/* system uptime (clocks) */
	gettimeofday((struct timeval *) &r[3], NULL);	/* sec and usec */
	stat("/tmp", &st);
	r[5] = st.st_atime;
	r[6] = st.st_mtime;
	r[7] = st.st_ctime;

	/* Hash both halves with MD5 to obscure possible patterns */
	rdssl_md5_init(&md5);
	rdssl_md5_update(&md5, random, 16);
	rdssl_md5_final(&md5, random);
	rdssl_md5_update(&md5, random + 16, 16);
	rdssl_md5_final(&md5, random + 16);
}

/* malloc; exit if out of memory */
void *
xmalloc(int size)
{
	void *mem = malloc(size);
	if (mem == NULL)
	{
		error("xmalloc %d\n", size);
		exit(EX_UNAVAILABLE);
	}
	return mem;
}

/* Exit on NULL pointer. Use to verify result from XGetImage etc */
void
exit_if_null(void *ptr)
{
	if (ptr == NULL)
	{
		error("unexpected null pointer. Out of memory?\n");
		exit(EX_UNAVAILABLE);
	}
}

/* strdup */
char *
xstrdup(const char *s)
{
	char *mem = strdup(s);
	if (mem == NULL)
	{
		perror("strdup");
		exit(EX_UNAVAILABLE);
	}
	return mem;
}

/* realloc; exit if out of memory */
void *
xrealloc(void *oldmem, size_t size)
{
	void *mem;

	if (size == 0)
		size = 1;
	mem = realloc(oldmem, size);
	if (mem == NULL)
	{
		error("xrealloc %ld\n", size);
		exit(EX_UNAVAILABLE);
	}
	return mem;
}

/* free */
void
xfree(void *mem)
{
	free(mem);
}

/* report an error */
void
error(char *format, ...)
{
	va_list ap;

	fprintf(stderr, "ERROR: ");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

/* report a warning */
void
warning(char *format, ...)
{
	va_list ap;

	fprintf(stderr, "WARNING: ");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

/* report an unimplemented protocol feature */
void
unimpl(char *format, ...)
{
	va_list ap;

	fprintf(stderr, "NOT IMPLEMENTED: ");

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

/* produce a hex dump */
void
hexdump(unsigned char *p, unsigned int len)
{
	unsigned char *line = p;
	int i, thisline, offset = 0;

	while (offset < len)
	{
		printf("%04x ", offset);
		thisline = len - offset;
		if (thisline > 16)
			thisline = 16;

		for (i = 0; i < thisline; i++)
			printf("%02x ", line[i]);

		for (; i < 16; i++)
			printf("   ");

		for (i = 0; i < thisline; i++)
			printf("%c", (line[i] >= 0x20 && line[i] < 0x7f) ? line[i] : '.');

		printf("\n");
		offset += thisline;
		line += thisline;
	}
}


char *next_arg(char *src, char needle)
{
	char *nextval;
	char *p;
	char *mvp = 0;

	/* EOS */
	if (*src == (char) 0x00)
		return 0;

	p = src;
	/*  skip escaped needles */
	while ((nextval = strchr(p, needle)))
	{
		mvp = nextval - 1;
		/* found backslashed needle */
		if (*mvp == '\\' && (mvp > src))
		{
			/* move string one to the left */
			while (*(mvp + 1) != (char) 0x00)
			{
				*mvp = *(mvp + 1);
				mvp++;
			}
			*mvp = (char) 0x00;
			p = nextval;
		}
		else
		{
			p = nextval + 1;
			break;
		}

	}

	/* more args available */
	if (nextval)
	{
		*nextval = (char) 0x00;
		return ++nextval;
	}

	/* no more args after this, jump to EOS */
	nextval = src + strlen(src);
	return nextval;
}


void
toupper_str(char *p)
{
	while (*p)
	{
		if ((*p >= 'a') && (*p <= 'z'))
			*p = toupper((int) *p);
		p++;
	}
}


RD_BOOL
str_startswith(const char *s, const char *prefix)
{
	return (strncmp(s, prefix, strlen(prefix)) == 0);
}


/* Split input into lines, and call linehandler for each
   line. Incomplete lines are saved in the rest variable, which should
   initially point to NULL. When linehandler returns False, stop and
   return False. Otherwise, return True.  */
RD_BOOL
str_handle_lines(const char *input, char **rest, str_handle_lines_t linehandler, void *data)
{
	char *buf, *p;
	char *oldrest;
	size_t inputlen;
	size_t buflen;
	size_t restlen = 0;
	RD_BOOL ret = True;

	/* Copy data to buffer */
	inputlen = strlen(input);
	if (*rest)
		restlen = strlen(*rest);
	buflen = restlen + inputlen + 1;
	buf = (char *) xmalloc(buflen);
	buf[0] = '\0';
	if (*rest)
		STRNCPY(buf, *rest, buflen);
	strncat(buf, input, buflen);
	p = buf;

	while (1)
	{
		char *newline = strchr(p, '\n');
		if (newline)
		{
			*newline = '\0';
			if (!linehandler(p, data))
			{
				p = newline + 1;
				ret = False;
				break;
			}
			p = newline + 1;
		}
		else
		{
			break;

		}
	}

	/* Save in rest */
	oldrest = *rest;
	restlen = buf + buflen - p;
	*rest = (char *) xmalloc(restlen);
	STRNCPY((*rest), p, restlen);
	xfree(oldrest);

	xfree(buf);
	return ret;
}

/* Execute the program specified by argv. For each line in
   stdout/stderr output, call linehandler. Returns false on failure. */
RD_BOOL
subprocess(char *const argv[], str_handle_lines_t linehandler, void *data)
{
	pid_t child;
	int fd[2];
	int n = 1;
	char output[256];
	char *rest = NULL;

	if (pipe(fd) < 0)
	{
		perror("pipe");
		return False;
	}

	if ((child = fork()) < 0)
	{
		perror("fork");
		return False;
	}

	/* Child */
	if (child == 0)
	{
		/* Close read end */
		close(fd[0]);

		/* Redirect stdout and stderr to pipe */
		dup2(fd[1], 1);
		dup2(fd[1], 2);

		/* Execute */
		execvp(argv[0], argv);
		perror("Error executing child");
		_exit(128);
	}

	/* Parent. Close write end. */
	close(fd[1]);
	while (n > 0)
	{
		n = read(fd[0], output, 255);
		output[n] = '\0';
		str_handle_lines(output, &rest, linehandler, data);
	}
	xfree(rest);

	return True;
}


/* not all clibs got ltoa */
#define LTOA_BUFSIZE (sizeof(long) * 8 + 1)

char *
l_to_a(long N, int base)
{
	static char ret[LTOA_BUFSIZE];

	char *head = ret, buf[LTOA_BUFSIZE], *tail = buf + sizeof(buf);

	register int divrem;

	if (base < 36 || 2 > base)
		base = 10;

	if (N < 0)
	{
		*head++ = '-';
		N = -N;
	}

	tail = buf + sizeof(buf);
	*--tail = 0;

	do
	{
		divrem = N % base;
		*--tail = (divrem <= 9) ? divrem + '0' : divrem + 'a' - 10;
		N /= base;
	}
	while (N);

	strcpy(head, tail);
	return ret;
}

int
load_licence(unsigned char **data)
{
	uint8 ho[20], hi[16];
	char *home, path[PATH_MAX], hash[41];
	struct stat st;
	int fd, length;

	home = getenv("HOME");
	if (home == NULL)
		return -1;

	memset(hi, 0, sizeof(hi));
	snprintf((char *) hi, 16, "%s", g_hostname);
	sec_hash_sha1_16(ho, hi, g_static_rdesktop_salt_16);
	sec_hash_to_string(hash, sizeof(hash), ho, sizeof(ho));

	snprintf(path, PATH_MAX, "%s" RDESKTOP_LICENSE_STORE "/%s.cal", home, hash);
	path[sizeof(path) - 1] = '\0';

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		/* fallback to try reading old license file */
		snprintf(path, PATH_MAX, "%s/.rdesktop/license.%s", home, g_hostname);
		path[sizeof(path) - 1] = '\0';
		if ((fd = open(path, O_RDONLY)) == -1)
			return -1;
	}

	if (fstat(fd, &st))
	{
		close(fd);
		return -1;
	}

	*data = (uint8 *) xmalloc(st.st_size);
	length = read(fd, *data, st.st_size);
	close(fd);
	return length;
}

void
save_licence(unsigned char *data, int length)
{
	uint8 ho[20], hi[16];
	char *home, path[PATH_MAX + 5], tmppath[PATH_MAX + 10], hash[41];
	int fd;

	home = getenv("HOME");
	if (home == NULL)
		return;

	snprintf(path, PATH_MAX, "%s" RDESKTOP_LICENSE_STORE, home);
	path[sizeof(path) - 1] = '\0';
	if (utils_mkdir_p(path, 0700) == -1)
	{
		perror(path);
		return;
	}

	memset(hi, 0, sizeof(hi));
	snprintf((char *) hi, 16, "%s", g_hostname);
	sec_hash_sha1_16(ho, hi, g_static_rdesktop_salt_16);
	sec_hash_to_string(hash, sizeof(hash), ho, sizeof(ho));

	/* write licence to {sha1}.cal.new, then atomically
	   rename to {sha1}.cal */
	snprintf(path, sizeof(path), "%s" RDESKTOP_LICENSE_STORE "/%s.cal", home, hash);
	path[sizeof(path) - 1] = '\0';

	snprintf(tmppath, sizeof(tmppath), "%s.new", path);
	path[sizeof(path) - 1] = '\0';

	fd = open(tmppath, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		perror(tmppath);
		return;
	}

	if (write(fd, data, length) != length)
	{
		perror(tmppath);
		unlink(tmppath);
	}
	else if (rename(tmppath, path) == -1)
	{
		perror(path);
		unlink(tmppath);
	}

	close(fd);
}

/* create rdesktop ui */
void
rd_create_ui()
{
	/* only create a window if we dont have one intialized */
//	if (!ui_have_window())
	{
		//if (!ui_create_window())
		//	exit(EX_OSERR);
	}
}

/* Create the bitmap cache directory */
RD_BOOL
rd_pstcache_mkdir(void)
{
	char *home;
	char bmpcache_dir[256];

	home = getenv("HOME");

	if (home == NULL)
		return False;

#ifdef VBOX
	snprintf(bmpcache_dir, sizeof(bmpcache_dir), "%s/%s", home, ".rdesktop");
#else
	sprintf(bmpcache_dir, "%s/%s", home, ".rdesktop");
#endif

	if ((mkdir(bmpcache_dir, S_IRWXU) == -1) && errno != EEXIST)
	{
		perror(bmpcache_dir);
		return False;
	}

#ifdef VBOX
	snprintf(bmpcache_dir, sizeof(bmpcache_dir), "%s/%s", home, ".rdesktop/cache");
#else
	sprintf(bmpcache_dir, "%s/%s", home, ".rdesktop/cache");
#endif

	if ((mkdir(bmpcache_dir, S_IRWXU) == -1) && errno != EEXIST)
	{
		perror(bmpcache_dir);
		return False;
	}

	return True;
}

/* open a file in the .rdesktop directory */
int
rd_open_file(char *filename)
{
	char *home;
	char fn[256];
	int fd;

	home = getenv("HOME");
	if (home == NULL)
		return -1;
#ifdef VBOX
	snprintf(fn, sizeof(fn), "%s/.rdesktop/%s", home, filename);
#else
	sprintf(fn, "%s/.rdesktop/%s", home, filename);
#endif
	fd = open(fn, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1)
		perror(fn);
	return fd;
}

/* close file */
void
rd_close_file(int fd)
{
	close(fd);
}

/* read from file*/
int
rd_read_file(int fd, void *ptr, int len)
{
	return read(fd, ptr, len);
}

/* write to file */
int
rd_write_file(int fd, void *ptr, int len)
{
	return write(fd, ptr, len);
}

/* move file pointer */
int
rd_lseek_file(int fd, int offset)
{
	return lseek(fd, offset, SEEK_SET);
}

/* do a write lock on a file */
RD_BOOL
rd_lock_file(int fd, int start, int len)
{
	struct flock lock;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = start;
	lock.l_len = len;
	if (fcntl(fd, F_SETLK, &lock) == -1)
		return False;
	return True;
}
