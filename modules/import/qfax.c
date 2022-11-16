/*
 * ***** BEGIN LICENSE BLOCK *****
 *
 * The contents of this file are subject to the GNU General Public License
 * Version 2 (the "License"); you may not use this file except in compliance
 * with the GPL. You may obtain a copy of the License at
 * http://www.gnu.org/copyleft/gpl.html or the file GPL.TXT from the program
 * or source code package.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for
 * the specific language governing rights and limitations under the GPL.
 *
 * The Original Code is Therapy Seriouz Software code.
 *
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../import.h"
#include "../../src/smurfine.h"
#include "demolib.h"

static unsigned short const bitmask[9] = {
	0x0000,
	0x0001,
	0x0003,
	0x0007,
	0x000f,
	0x001f,
	0x003f,
	0x007f,
	0x00ff
};

static uint16_t const x13250[9] = {
	0x0080,
	0x0040,
	0x0020,
	0x0010,
	0x0008,
	0x0004,
	0x0002,
	0x0001,
	0x0000
};

uint16_t const x13262[] = {
	0x8000,
	0xc000,
	0xe000,
	0xf000,
	0xf800,
	0xfc00,
	0xfe00,
	0xff00
};

struct proto {
	uint16_t a;
	int16_t b;
	uint16_t c;
};

static struct proto const x13272[] = {
	{ 0x0035, 0x0008, 0x0000 },
	{ 0x0007, 0x0006, 0x0001 },
	{ 0x0007, 0x0004, 0x0002 },
	{ 0x0008, 0x0004, 0x0003 },
	{ 0x000b, 0x0004, 0x0004 },
	{ 0x000c, 0x0004, 0x0005 },
	{ 0x000e, 0x0004, 0x0006 },
	{ 0x000f, 0x0004, 0x0007 },
	{ 0x0013, 0x0005, 0x0008 },
	{ 0x0014, 0x0005, 0x0009 },
	{ 0x0007, 0x0005, 0x000a },
	{ 0x0008, 0x0005, 0x000b },
	{ 0x0008, 0x0006, 0x000c },
	{ 0x0003, 0x0006, 0x000d },
	{ 0x0034, 0x0006, 0x000e },
	{ 0x0035, 0x0006, 0x000f },
	{ 0x002a, 0x0006, 0x0010 },
	{ 0x002b, 0x0006, 0x0011 },
	{ 0x0027, 0x0007, 0x0012 },
	{ 0x000c, 0x0007, 0x0013 },
	{ 0x0008, 0x0007, 0x0014 },
	{ 0x0017, 0x0007, 0x0015 },
	{ 0x0003, 0x0007, 0x0016 },
	{ 0x0004, 0x0007, 0x0017 },
	{ 0x0028, 0x0007, 0x0018 },
	{ 0x002b, 0x0007, 0x0019 },
	{ 0x0013, 0x0007, 0x001a },
	{ 0x0024, 0x0007, 0x001b },
	{ 0x0018, 0x0007, 0x001c },
	{ 0x0002, 0x0008, 0x001d },
	{ 0x0003, 0x0008, 0x001e },
	{ 0x001a, 0x0008, 0x001f },
	{ 0x001b, 0x0008, 0x0020 },
	{ 0x0012, 0x0008, 0x0021 },
	{ 0x0013, 0x0008, 0x0022 },
	{ 0x0014, 0x0008, 0x0023 },
	{ 0x0015, 0x0008, 0x0024 },
	{ 0x0016, 0x0008, 0x0025 },
	{ 0x0017, 0x0008, 0x0026 },
	{ 0x0028, 0x0008, 0x0027 },
	{ 0x0029, 0x0008, 0x0028 },
	{ 0x002a, 0x0008, 0x0029 },
	{ 0x002b, 0x0008, 0x002a },
	{ 0x002c, 0x0008, 0x002b },
	{ 0x002d, 0x0008, 0x002c },
	{ 0x0004, 0x0008, 0x002d },
	{ 0x0005, 0x0008, 0x002e },
	{ 0x000a, 0x0008, 0x002f },
	{ 0x000b, 0x0008, 0x0030 },
	{ 0x0052, 0x0008, 0x0031 },
	{ 0x0053, 0x0008, 0x0032 },
	{ 0x0054, 0x0008, 0x0033 },
	{ 0x0055, 0x0008, 0x0034 },
	{ 0x0024, 0x0008, 0x0035 },
	{ 0x0025, 0x0008, 0x0036 },
	{ 0x0058, 0x0008, 0x0037 },
	{ 0x0059, 0x0008, 0x0038 },
	{ 0x005a, 0x0008, 0x0039 },
	{ 0x005b, 0x0008, 0x003a },
	{ 0x004a, 0x0008, 0x003b },
	{ 0x004b, 0x0008, 0x003c },
	{ 0x0032, 0x0008, 0x003d },
	{ 0x0033, 0x0008, 0x003e },
	{ 0x0034, 0x0008, 0x003f }
};

static struct proto const x133f2[] = {
	{ 0x001b, 0x0005, 0x0040 },
	{ 0x0012, 0x0005, 0x0080 },
	{ 0x0017, 0x0006, 0x00c0 },
	{ 0x0037, 0x0007, 0x0100 },
	{ 0x0036, 0x0008, 0x0140 },
	{ 0x0037, 0x0008, 0x0180 },
	{ 0x0064, 0x0008, 0x01c0 },
	{ 0x0065, 0x0008, 0x0200 },
	{ 0x0068, 0x0008, 0x0240 },
	{ 0x0067, 0x0008, 0x0280 },
	{ 0x00cc, 0x0009, 0x02c0 },
	{ 0x00cd, 0x0009, 0x0300 },
	{ 0x00d2, 0x0009, 0x0340 },
	{ 0x00d3, 0x0009, 0x0380 },
	{ 0x00d4, 0x0009, 0x03c0 },
	{ 0x00d5, 0x0009, 0x0400 },
	{ 0x00d6, 0x0009, 0x0440 },
	{ 0x00d7, 0x0009, 0x0480 },
	{ 0x00d8, 0x0009, 0x04c0 },
	{ 0x00d9, 0x0009, 0x0500 },
	{ 0x00da, 0x0009, 0x0540 },
	{ 0x00db, 0x0009, 0x0580 },
	{ 0x0098, 0x0009, 0x05c0 },
	{ 0x0099, 0x0009, 0x0600 },
	{ 0x009a, 0x0009, 0x0640 },
	{ 0x0018, 0x0006, 0x0680 },
	{ 0x009b, 0x0009, 0x06c0 },
	{ 0x0001, 0x000c, 0xfffe }
};

static struct proto const x1349a[] = {
	{ 0x0037, 0x000a, 0x0000 },
	{ 0x0002, 0x0003, 0x0001 },
	{ 0x0003, 0x0002, 0x0002 },
	{ 0x0002, 0x0002, 0x0003 },
	{ 0x0003, 0x0003, 0x0004 },
	{ 0x0003, 0x0004, 0x0005 },
	{ 0x0002, 0x0004, 0x0006 },
	{ 0x0003, 0x0005, 0x0007 },
	{ 0x0005, 0x0006, 0x0008 },
	{ 0x0004, 0x0006, 0x0009 },
	{ 0x0004, 0x0007, 0x000a },
	{ 0x0005, 0x0007, 0x000b },
	{ 0x0007, 0x0007, 0x000c },
	{ 0x0004, 0x0008, 0x000d },
	{ 0x0007, 0x0008, 0x000e },
	{ 0x0018, 0x0009, 0x000f },
	{ 0x0017, 0x000a, 0x0010 },
	{ 0x0018, 0x000a, 0x0011 },
	{ 0x0008, 0x000a, 0x0012 },
	{ 0x0067, 0x000b, 0x0013 },
	{ 0x0068, 0x000b, 0x0014 },
	{ 0x006c, 0x000b, 0x0015 },
	{ 0x0037, 0x000b, 0x0016 },
	{ 0x0028, 0x000b, 0x0017 },
	{ 0x0017, 0x000b, 0x0018 },
	{ 0x0018, 0x000b, 0x0019 },
	{ 0x00ca, 0x000c, 0x001a },
	{ 0x00cb, 0x000c, 0x001b },
	{ 0x00cc, 0x000c, 0x001c },
	{ 0x00cd, 0x000c, 0x001d },
	{ 0x0068, 0x000c, 0x001e },
	{ 0x0069, 0x000c, 0x001f },
	{ 0x006a, 0x000c, 0x0020 },
	{ 0x006b, 0x000c, 0x0021 },
	{ 0x00d2, 0x000c, 0x0022 },
	{ 0x00d3, 0x000c, 0x0023 },
	{ 0x00d4, 0x000c, 0x0024 },
	{ 0x00d5, 0x000c, 0x0025 },
	{ 0x00d6, 0x000c, 0x0026 },
	{ 0x00d7, 0x000c, 0x0027 },
	{ 0x006c, 0x000c, 0x0028 },
	{ 0x006d, 0x000c, 0x0029 },
	{ 0x00da, 0x000c, 0x002a },
	{ 0x00db, 0x000c, 0x002b },
	{ 0x0054, 0x000c, 0x002c },
	{ 0x0055, 0x000c, 0x002d },
	{ 0x0056, 0x000c, 0x002e },
	{ 0x0057, 0x000c, 0x002f },
	{ 0x0064, 0x000c, 0x0030 },
	{ 0x0065, 0x000c, 0x0031 },
	{ 0x0052, 0x000c, 0x0032 },
	{ 0x0053, 0x000c, 0x0033 },
	{ 0x0024, 0x000c, 0x0034 },
	{ 0x0037, 0x000c, 0x0035 },
	{ 0x0038, 0x000c, 0x0036 },
	{ 0x0027, 0x000c, 0x0037 },
	{ 0x0028, 0x000c, 0x0038 },
	{ 0x0058, 0x000c, 0x0039 },
	{ 0x0059, 0x000c, 0x003a },
	{ 0x002b, 0x000c, 0x003b },
	{ 0x002c, 0x000c, 0x003c },
	{ 0x005a, 0x000c, 0x003d },
	{ 0x0066, 0x000c, 0x003e },
	{ 0x0067, 0x000c, 0x003f },
};

static struct proto const x1361a[] = {
	{ 0x000f, 0x000a, 0x0040 },
	{ 0x00c8, 0x000c, 0x0080 },
	{ 0x00c9, 0x000c, 0x00c0 },
	{ 0x005b, 0x000c, 0x0100 },
	{ 0x0033, 0x000c, 0x0140 },
	{ 0x0034, 0x000c, 0x0180 },
	{ 0x0035, 0x000c, 0x01c0 },
	{ 0x006c, 0x000d, 0x0200 },
	{ 0x006d, 0x000d, 0x0240 },
	{ 0x004a, 0x000d, 0x0280 },
	{ 0x004b, 0x000d, 0x02c0 },
	{ 0x004c, 0x000d, 0x0300 },
	{ 0x004d, 0x000d, 0x0340 },
	{ 0x0072, 0x000d, 0x0380 },
	{ 0x0073, 0x000d, 0x03c0 },
	{ 0x0074, 0x000d, 0x0400 },
	{ 0x0075, 0x000d, 0x0440 },
	{ 0x0076, 0x000d, 0x0480 },
	{ 0x0077, 0x000d, 0x04c0 },
	{ 0x0052, 0x000d, 0x0500 },
	{ 0x0053, 0x000d, 0x0540 },
	{ 0x0054, 0x000d, 0x0580 },
	{ 0x0055, 0x000d, 0x05c0 },
	{ 0x005a, 0x000d, 0x0600 },
	{ 0x005b, 0x000d, 0x0640 },
	{ 0x0064, 0x000d, 0x0680 },
	{ 0x0065, 0x000d, 0x06c0 },
	{ 0x0001, 0x000c, 0xfffe },
	{ 0x0008, 0x000b, 0x0700 },
	{ 0x000c, 0x000b, 0x0740 },
	{ 0x000d, 0x000b, 0x0780 },
	{ 0x0012, 0x000c, 0x07c0 },
	{ 0x0013, 0x000c, 0x0800 },
	{ 0x0014, 0x000c, 0x0840 },
	{ 0x0015, 0x000c, 0x0880 },
	{ 0x0016, 0x000c, 0x08c0 },
	{ 0x0017, 0x000c, 0x0900 },
	{ 0x001c, 0x000c, 0x0940 },
	{ 0x001d, 0x000c, 0x0980 },
	{ 0x001e, 0x000c, 0x09c0 },
	{ 0x001f, 0x000c, 0x0a00 }
};

/* info structure for main module */
MOD_INFO module_info = {
	"QFax",
	0x0010,
	"Christian Eyrich",
	/* Extensionen */
	{ "F01", "F02", "F03", "F04", "F05", "F06", "F07", "F08", "F09", "F10" },

	/* Slider */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* Checkboxen */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* Editfelder */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",

	/* Minima + Maxima */
	/* Slider */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	/* Edits */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};

static uint8_t BitsAvail = 0;

#define TIMER 1

struct t13a26 {
	int16_t a;
	uint8_t b;
	uint8_t c;
};

static void *(*SMalloc)(long amount);
static void *(*SMfree)(void *ptr);
static uint8_t *x13a1e;
static uint8_t x13a22;
static uint16_t global_y;
static struct t13a26 *x13a26;
static struct t13a26 *x13a2a;
static struct t13a26 x13a2e[256][32];
static struct t13a26 x1ba2e[256][32];

static char *get_extension(const char *filename);
static void x10178(uint8_t *data, uint8_t *dest_data, uint16_t width, uint16_t height);
static void x101e0(uint8_t *dest_data);
static uint16_t GetBits(uint8_t);
static void x10456(void);
static void mktables(void);



/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
#ifdef __PUREC__
/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif


short imp_module_main(GARGAMEL *smurf_struct)
{
	char *extstart;
	uint8_t *pic_data;
	uint8_t type;
	uint16_t width;
	uint16_t height;
	uint8_t dpi;
	uint8_t *dest_data;
	uint8_t *pal;
	
	pic_data = smurf_struct->smurf_pic->pic_data;
	
	extstart = get_extension(smurf_struct->smurf_pic->filename);
	if (strnicmp(extstart, "F", 1) != 0 ||
		extstart[1] < '0' || extstart[1] > '9' ||
		extstart[2] < '0' || extstart[2] > '9')
		return M_INVALID;
	
	type = pic_data[24];
	if (type == 0)
		type = 1;
	if (type > 1)
	{
		/* FIXME: translate */
		/* FIMXE: must not call AES functions in modules */
		form_alert(0, "[1][2-dimensionale Komprimierung wird | noch nicht unterstzt][ OK ]");
		return M_UNKNOWN_TYPE;
	}
	
	height = swap_word(*((uint16_t *)(pic_data + 25))); /* BUG: misaligned address */
	width = swap_word(*((uint16_t *)(pic_data + 27))); /* BUG: misaligned address */
	if (pic_data[29] == 0)
		dpi = 100;
	else
		dpi = 200;
	dpi = dpi;
	/* FIXME: translate */
	strncpy(smurf_struct->smurf_pic->format_name, "QFax Faxdatei", 21);
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;
	smurf_struct->services->reset_busybox(128, "QFax Faxdatei 1 Bit");
	
	width = (width + 7) >> 3;
	if ((dest_data = SMalloc((long) width * height)) == NULL)
		return M_MEMORY;
	memset(dest_data, 0, (long) width * height);
	x10178(pic_data + 256, dest_data, width, height);
	smurf_struct->smurf_pic->pic_data = dest_data;
	SMfree(pic_data);
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	pal = smurf_struct->smurf_pic->palette;
	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;
	
	return M_PICDONE;					/* das wars. */
}


static void x10178(uint8_t *data, uint8_t *dest_data, uint16_t width, uint16_t height)
{
	uint16_t y;
	
	x13a1e = data;
	mktables();
#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif
	y = 0;
	do
	{
		global_y = y;
		x101e0(dest_data);
		dest_data += width;
	} while (++y < height);

#if TIMER
	/* wie schnell waren wir? */
	printf("Zeit: %lu\n", get_timer());
#endif
	SMfree(x13a26);
	SMfree(x13a2a);
}


static void x101e0(uint8_t *dest_data)
{
	int16_t d4;
	uint16_t d5;
	uint16_t o4;
	int16_t o2;
	BOOLEAN o0;
	int16_t d7;
	int16_t d3;
	uint8_t d6;
	uint8_t *a3;
	int d0;
	
	o4 = 0;
	BitsAvail = 0;
	d4 = 0;
	d5 = 0;
	o0 = TRUE;
	x13a22 = 0;
	while (o4 == 0)
	{
		if (o0 == TRUE)
		{
			d5 = GetBits(8);
			d7 = x13a26[d5].a;
			if (d7 == -1)
			{
				x13a1e += (BitsAvail + 8) >> 3;
				BitsAvail = (BitsAvail + 8) & 7;
				d3 = x13a26[d5].c;
				d5 = GetBits(5);
				d7 = x13a2e[d3][d5].a;
				if (d7 == -3)
				{
					x13a1e -= 2;
					x10456();
					return;
				}
				if (d7 == -2)
				{
					x13a1e += (BitsAvail + 4) >> 3;
					BitsAvail = (BitsAvail + 4) & 7;
					return;
				}
				d6 = x13a2e[d3][d5].b;
			} else
			{
				d6 = x13a26[d5].b;
			}
			x13a1e += (BitsAvail + d6) >> 3;
			BitsAvail = (BitsAvail + d6) & 7;
			if (d7 < 64)
				o0 = FALSE;
			d4 += d7;
		} else
		{
			d5 = GetBits(8);
			d7 = x13a2a[d5].a;
			if (d7 == -1)
			{
				x13a1e += (BitsAvail + 8) >> 3;
				BitsAvail = (BitsAvail + 8) & 7;
				d3 = x13a2a[d5].c;
				d5 = GetBits(5);
				d7 = x1ba2e[d3][d5].a;
				if (d7 == -3)
				{
					x13a1e -= 2;
					x10456();
					return;
				}
				if (d7 == -2)
				{
					x13a1e += (BitsAvail + 4) >> 3;
					BitsAvail = (BitsAvail + 4) & 7;
					return;
				}
				d6 = x1ba2e[d3][d5].b;
			} else
			{
				d6 = x13a2a[d5].b;
			}
			x13a1e += (BitsAvail + d6) >> 3;
			BitsAvail = (BitsAvail + d6) & 7;
			if (d7 < 64)
				o0 = TRUE;
			a3 = &dest_data[d4 >> 3];
			d0 = (7 - d4) & 7;
			d4 += d7;
			while (d7 != 0 && d0 >= 0)
			{
				*a3 = *a3 | (1 << d0);
				d0--;
				d7--;
			}
			o2 = d7 >> 3;
			while (o2-- != 0)
			{
				*++a3 = 0xff;
				d7 -= 8;
			}
			if (d7 != 0)
			{
				d0 = 7;
				a3++;
				while (d7-- != 0)
				{
					*a3 = *a3 | (1 << d0);
					d0--;
				}
			}
		}
	}
}


static uint16_t GetBits(uint8_t x)
{
	int16_t val;
	
	val = *((int16_t *)x13a1e);
	val >>= 16 - (BitsAvail + x);
	val &= bitmask[x];
	return val;
}


static void x10456(void)
{
	uint8_t c;
	
	for (;;)
	{
		c = *x13a1e & x13250[BitsAvail++];
		if (BitsAvail > 7)
		{
			BitsAvail = 0;
			x13a1e++;
		}
		if (c != 0 && x13a22 >= 11)
			return;
		if (c != 0)
			x13a22 = 0;
		else
			x13a22++;
	}
}


static void mktables(void)
{
	int d3;
	int d4;
	uint8_t d5;
	int d6;
	int d7;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif
	x13a26 = SMalloc(256 * sizeof(*x13a26));
	memset(x13a26, 0, 256 * sizeof(*x13a26));
	d3 = 0;
	do
	{
		d4 = 0;
		do
		{
			x13a2e[d3][d4].a = -3;
			x13a2e[d3][d4].b = 5;
		} while (++d4 < 32);
	} while (++d3 < 256);
	
	d3 = 0;
	do
	{
		if (x13272[d3].b <= 8)
		{
			d5 = 8 - x13272[d3].b;
			d4 = 0;
			do
			{
				d6 = (x13272[d3].a << d5) | d4;
				x13a26[d6].a = x13272[d3].c;
				x13a26[d6].b = x13272[d3].b;
			} while ((1 << d5) > ++d4);
		} else
		{
			d7 = x13272[d3].a;
			d7 >>= x13272[d3].b - 8;
			x13a26[d7].a = -1;
			x13a26[d7].c = d7;
			x13a26[d7].b = 8;
			d5 = 5 - (x13272[d3].b - 8);
			d4 = 0;
			do
			{
				d6 = ((x13272[d3].a << d5) | d4) & bitmask[5];
				x13a2e[d7][d6].a = x13272[d3].c;
				x13a2e[d7][d6].b = x13272[d3].b - 8;
			} while ((1 << d5) > ++d4);
		}
	} while (++d3 < 64);
	
	d3 = 0;
	do
	{
		if (x133f2[d3].b <= 8)
		{
			d5 = 8 - x133f2[d3].b;
			d4 = 0;
			do
			{
				d6 = (x133f2[d3].a << d5) | d4;
				x13a26[d6].a = x133f2[d3].c;
				x13a26[d6].b = x133f2[d3].b;
			} while ((1 << d5) > ++d4);
		} else
		{
			d7 = x133f2[d3].a;
			d7 >>= x133f2[d3].b - 8;
			x13a26[d7].a = -1;
			x13a26[d7].c = d7;
			x13a26[d7].b = 8;
			d5 = 5 - (x133f2[d3].b - 8);
			d4 = 0;
			do
			{
				d6 = ((x133f2[d3].a << d5) | d4) & bitmask[5];
				x13a2e[d7][d6].a = x133f2[d3].c;
				x13a2e[d7][d6].b = x133f2[d3].b - 8;
			} while ((1 << d5) > ++d4);
		}
	} while (++d3 < 28);
#if TIMER
	/* wie schnell waren wir? */
	printf("Zeit: %lu\n", get_timer());
#endif

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif
	x13a2a = SMalloc(256 * sizeof(*x13a2a));
	memset(x13a2a, 0, 256 * sizeof(*x13a2a));

	d3 = 0;
	do
	{
		d4 = 0;
		do
		{
			x1ba2e[d3][d4].a = -3;
			x1ba2e[d3][d4].b = 5;
		} while (++d4 < 32);
	} while (++d3 < 256);

	d3 = 0;
	do
	{
		if (x1349a[d3].b <= 8)
		{
			d5 = 8 - x1349a[d3].b;
			d4 = 0;
			do
			{
				d6 = (x1349a[d3].a << d5) | d4;
				x13a2a[d6].a = x1349a[d3].c;
				x13a2a[d6].b = x1349a[d3].b;
			} while ((1 << d5) > ++d4);
		} else
		{
			d7 = x1349a[d3].a;
			d7 >>= x1349a[d3].b - 8;
			x13a2a[d7].a = -1;
			x13a2a[d7].c = d7;
			x13a2a[d7].b = 8;
			d5 = 5 - (x1349a[d3].b - 8);
			d4 = 0;
			do
			{
				d6 = ((x1349a[d3].a << d5) | d4) & bitmask[5];
				x13a2e[d7][d6].a = x1349a[d3].c;
				x13a2e[d7][d6].b = x1349a[d3].b - 8;
			} while ((1 << d5) > ++d4);
		}
	} while (++d3 < 64);

	d3 = 0;
	do
	{
		if (x1361a[d3].b <= 8)
		{
			d5 = 8 - x1361a[d3].b;
			d4 = 0;
			do
			{
				d6 = (x1361a[d3].a << d5) | d4;
				x13a2a[d6].a = x1361a[d3].c;
				x13a2a[d6].b = x1361a[d3].b;
			} while ((1 << d5) > ++d4);
		} else
		{
			d7 = x1361a[d3].a;
			d7 >>= x1361a[d3].b - 8;
			x13a2a[d7].a = -1;
			x13a2a[d7].c = d7;
			x13a2a[d7].b = 8;
			d5 = 5 - (x1361a[d3].b - 8);
			d4 = 0;
			do
			{
				d6 = ((x1361a[d3].a << d5) | d4) & bitmask[5];
				x13a2e[d7][d6].a = x1361a[d3].c;
				x13a2e[d7][d6].b = x1361a[d3].b - 8;
			} while ((1 << d5) > ++d4);
		}
	} while (++d3 < 28);
#if TIMER
	/* wie schnell waren wir? */
	printf("Zeit: %lu\n", get_timer());
#endif
}


static char *get_extension(const char *filename)
{
	char *p;
	
	p = strrchr(filename, '.');
	if (p != NULL)
		p++;
	else
		p = strrchr(filename, '\0');
	
	return p;
}
