#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef __PUREC__
#include <tos.h>
#if defined(_MINT_OSBIND_H)
#include <mint/falcon.h>
#else
#define VgetMonitor() mon_type()
#define VsetMode(mode) Vsetmode(mode)
#endif
#else
#include <osbind.h>
#include <mintbind.h>
#include <mint/falcon.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include "demolib.h"

/* Values returned by VgetMonitor() */
#ifndef MON_MONO
#define MON_MONO		0
#define MON_COLOR		1
#define MON_VGA			2
#define MON_TV			3
#endif

static unsigned long screen_size;
static unsigned short monitor_type;
static int modecode;
unsigned short *screens[2];
static bool doublebuffer;
static unsigned short orig_mode;
static void *orig_logbase;
static void *orig_physbase;
static unsigned long black;
static unsigned char orig_conterm;
int last_error;

#if defined(__PUREC__) && !defined(_MINT_OSBIND_H)
#define VgetSize(mode) xbios(91, mode)
#define VsetScreen(laddr, paddr, rez, mode) xbios(5, laddr, paddr, rez, mode)
#endif

#define screenpt *((unsigned short **)0x45e)
#define conterm *((unsigned char *)0x484)
#define FColor ((unsigned long *)0xffff9800L)


bool init_screens(bool doublebuffering, int modecode_rgb, int modecode_vga)
{
	monitor_type = VgetMonitor();
	if (monitor_type == MON_MONO)
	{
		last_error = No_Mono;
		return True;
	}
	if (modecode_rgb == -1 && monitor_type != MON_VGA)
	{
		last_error = No_RGBTV_support;
		return True;
	}
	if (modecode_vga == -1 && monitor_type == MON_VGA)
	{
		last_error = No_VGA_support;
		return True;
	}
	if (monitor_type == MON_VGA)
		modecode = modecode_vga;
	else
		modecode = modecode_rgb;
	doublebuffer = doublebuffering;
	screen_size = VgetSize(modecode);

	screens[0] = malloc(screen_size);
	if (screens[0] == NULL)
	{
		last_error = No_Memory;
		return True;
	}
	if (doublebuffer)
	{
		screens[1] = malloc(screen_size);
		if (screens[1] == NULL)
		{
			last_error = No_Memory;
			return True;
		}
		memset(screens[1], 0, screen_size);
	}

	orig_mode = VsetMode(-1);
	orig_logbase = Logbase();
	orig_physbase = Physbase();
	VsetScreen(screens[0], screens[0], 3, modecode);
	(void)VsetMode(modecode);
	set_screen(0);
	memset(screens[0], 0, screen_size);
	if (doublebuffer)
		memset(screens[1], 0, screen_size);

	return False;
}


void exit_screens(void)
{
	VsetScreen(orig_logbase, orig_physbase, 3, orig_mode);
	(void)VsetMode(orig_mode);
}


/* supervisor-mode only */
void set_screen(int nr)
{
	screenpt = screens[nr];
	Vsync();
	screenpt = 0;
}


void tc_line_adr(unsigned short *adr, int color, int x1, int y1, int x2, int y2)
{
	int xdiff;
	int ydiff;
	int x;
	int tmp;
	long offset;

	xdiff = abs(x2 - x1);
	ydiff = abs(y2 - y1);
	if (xdiff > ydiff)
	{
		if (xdiff == 0)
		{
			adr[(long) y1 * 320L + x1] = color;
		} else
		{
			if (x2 < x1)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
				tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			ydiff = y2 - y1;
			for (x = 0; (xdiff + 1) > x; x++)
			{
				offset = (long) x;
				offset = offset * ydiff;
				offset = offset / xdiff;
				offset += y1;
				adr[offset * 320 + (x1 + x)] = color;
			}
		}
	} else if (ydiff == 0)
	{
		adr[(long) y1 * 320L + x1] = color;
	} else
	{
		if (y2 < y1)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		xdiff = x2 - x1;
		for (x = 0; (ydiff + 1) > x; x++)
		{
			offset = (long) x;
			offset = offset * xdiff;
			offset = offset / ydiff;
			offset += x1;
			adr[(y1 + x) * 320L + offset] = color;
		}
	}
}


void tc_line(int screen, int color, int x1, int y1, int x2, int y2)
{
	int xdiff;
	int ydiff;
	int x;
	int tmp;
	long offset;

	xdiff = abs(x2 - x1);
	ydiff = abs(y2 - y1);
	if (xdiff > ydiff)
	{
		if (xdiff == 0)
		{
			screens[screen][(long) y1 * 320L + x1] = color;
		} else
		{
			if (x2 < x1)
			{
				tmp = x1;
				x1 = x2;
				x2 = tmp;
				tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			ydiff = y2 - y1;
			for (x = 0; (xdiff + 1) > x; x++)
			{
				offset = (long) x;
				offset = offset * ydiff;
				offset = offset / xdiff;
				offset += y1;
				screens[screen][offset * 320 + (x1 + x)] = color;
			}
		}
	} else if (ydiff == 0)
	{
		screens[screen][(long) y1 * 320L + x1] = color;
	} else
	{
		if (y2 < y1)
		{
			tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}
		xdiff = x2 - x1;
		for (x = 0; (ydiff + 1) > x; x++)
		{
			offset = (long) x;
			offset = offset * xdiff;
			offset = offset / ydiff;
			offset += x1;
			screens[screen][(y1 + x) * 320L + offset] = color;
		}
	}
}


coor between(coor p1, coor p2, short pos, short steps)
{
	coor p;

	p.x = p2.x - p1.x;
	p.y = p2.y - p1.y;
	p.x *= pos;
	p.y *= pos;
	p.x /= steps;
	p.y /= steps;
	p.x += p1.x;
	p.y += p1.y;
	return p;
}


/* supervisor-mode only */
void rand_black(void)
{
	black = FColor[0];
	FColor[0] = 0;
	orig_conterm = conterm;
	conterm &= 0xf8;					/* disable keyclick, repeat and bell */
}


/* supervisor-mode only */
void rand_restore(void)
{
	FColor[0] = black;
	conterm = orig_conterm;
}


void tc_screenshot(void *adr, int height)
{
	int fd;
	long magic;
	short w;
	short h;

	magic = 0x54525550L;				/* 'TRUP' */
	w = 320;
	h = height;
	fd = open("SS.TRP", O_CREAT | O_TRUNC | O_WRONLY);
	if (fd != -1)
	{
		write(fd, &magic, sizeof(magic));
		write(fd, &w, sizeof(w));
		write(fd, &h, sizeof(h));
		write(fd, adr, (long) h * 320 * 2);
		close(fd);
	}
}
