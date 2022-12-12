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
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"
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

static uint16_t const bitval[9] = {
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

uint16_t const unused[] = {
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
	uint16_t bits;
	uint16_t param;
};

static struct proto const whiteEncodeTable2[] = {
	{ 0x0035, 8, 0x0000 },
	{ 0x0007, 6, 0x0001 },
	{ 0x0007, 4, 0x0002 },
	{ 0x0008, 4, 0x0003 },
	{ 0x000b, 4, 0x0004 },
	{ 0x000c, 4, 0x0005 },
	{ 0x000e, 4, 0x0006 },
	{ 0x000f, 4, 0x0007 },
	{ 0x0013, 5, 0x0008 },
	{ 0x0014, 5, 0x0009 },
	{ 0x0007, 5, 0x000a },
	{ 0x0008, 5, 0x000b },
	{ 0x0008, 6, 0x000c },
	{ 0x0003, 6, 0x000d },
	{ 0x0034, 6, 0x000e },
	{ 0x0035, 6, 0x000f },
	{ 0x002a, 6, 0x0010 },
	{ 0x002b, 6, 0x0011 },
	{ 0x0027, 7, 0x0012 },
	{ 0x000c, 7, 0x0013 },
	{ 0x0008, 7, 0x0014 },
	{ 0x0017, 7, 0x0015 },
	{ 0x0003, 7, 0x0016 },
	{ 0x0004, 7, 0x0017 },
	{ 0x0028, 7, 0x0018 },
	{ 0x002b, 7, 0x0019 },
	{ 0x0013, 7, 0x001a },
	{ 0x0024, 7, 0x001b },
	{ 0x0018, 7, 0x001c },
	{ 0x0002, 8, 0x001d },
	{ 0x0003, 8, 0x001e },
	{ 0x001a, 8, 0x001f },
	{ 0x001b, 8, 0x0020 },
	{ 0x0012, 8, 0x0021 },
	{ 0x0013, 8, 0x0022 },
	{ 0x0014, 8, 0x0023 },
	{ 0x0015, 8, 0x0024 },
	{ 0x0016, 8, 0x0025 },
	{ 0x0017, 8, 0x0026 },
	{ 0x0028, 8, 0x0027 },
	{ 0x0029, 8, 0x0028 },
	{ 0x002a, 8, 0x0029 },
	{ 0x002b, 8, 0x002a },
	{ 0x002c, 8, 0x002b },
	{ 0x002d, 8, 0x002c },
	{ 0x0004, 8, 0x002d },
	{ 0x0005, 8, 0x002e },
	{ 0x000a, 8, 0x002f },
	{ 0x000b, 8, 0x0030 },
	{ 0x0052, 8, 0x0031 },
	{ 0x0053, 8, 0x0032 },
	{ 0x0054, 8, 0x0033 },
	{ 0x0055, 8, 0x0034 },
	{ 0x0024, 8, 0x0035 },
	{ 0x0025, 8, 0x0036 },
	{ 0x0058, 8, 0x0037 },
	{ 0x0059, 8, 0x0038 },
	{ 0x005a, 8, 0x0039 },
	{ 0x005b, 8, 0x003a },
	{ 0x004a, 8, 0x003b },
	{ 0x004b, 8, 0x003c },
	{ 0x0032, 8, 0x003d },
	{ 0x0033, 8, 0x003e },
	{ 0x0034, 8, 0x003f }
};

static struct proto const whiteEncodeTable3[] = {
	{ 0x001b, 5, 0x0040 },
	{ 0x0012, 5, 0x0080 },
	{ 0x0017, 6, 0x00c0 },
	{ 0x0037, 7, 0x0100 },
	{ 0x0036, 8, 0x0140 },
	{ 0x0037, 8, 0x0180 },
	{ 0x0064, 8, 0x01c0 },
	{ 0x0065, 8, 0x0200 },
	{ 0x0068, 8, 0x0240 },
	{ 0x0067, 8, 0x0280 },
	{ 0x00cc, 9, 0x02c0 },
	{ 0x00cd, 9, 0x0300 },
	{ 0x00d2, 9, 0x0340 },
	{ 0x00d3, 9, 0x0380 },
	{ 0x00d4, 9, 0x03c0 },
	{ 0x00d5, 9, 0x0400 },
	{ 0x00d6, 9, 0x0440 },
	{ 0x00d7, 9, 0x0480 },
	{ 0x00d8, 9, 0x04c0 },
	{ 0x00d9, 9, 0x0500 },
	{ 0x00da, 9, 0x0540 },
	{ 0x00db, 9, 0x0580 },
	{ 0x0098, 9, 0x05c0 },
	{ 0x0099, 9, 0x0600 },
	{ 0x009a, 9, 0x0640 },
	{ 0x0018, 6, 0x0680 },
	{ 0x009b, 9, 0x06c0 },
	{ 0x0001, 12, -2 }
#if 0
	{ 0x0008, 11, 0x0700 },
	{ 0x000c, 11, 0x0740 },
	{ 0x000d, 11, 0x0780 },
	{ 0x0012, 12, 0x07c0 },
	{ 0x0013, 12, 0x0800 },
	{ 0x0014, 12, 0x0840 },
	{ 0x0015, 12, 0x0880 },
	{ 0x0016, 12, 0x08c0 },
	{ 0x0017, 12, 0x0900 },
	{ 0x001c, 12, 0x0940 },
	{ 0x001d, 12, 0x0980 },
	{ 0x001e, 12, 0x09c0 },
	{ 0x001f, 12, 0x0a00 },
#endif
};

static struct proto const blackEncodeTable2[] = {
	{ 0x0037, 10, 0x0000 },
	{ 0x0002, 3, 0x0001 },
	{ 0x0003, 2, 0x0002 },
	{ 0x0002, 2, 0x0003 },
	{ 0x0003, 3, 0x0004 },
	{ 0x0003, 4, 0x0005 },
	{ 0x0002, 4, 0x0006 },
	{ 0x0003, 5, 0x0007 },
	{ 0x0005, 6, 0x0008 },
	{ 0x0004, 6, 0x0009 },
	{ 0x0004, 7, 0x000a },
	{ 0x0005, 7, 0x000b },
	{ 0x0007, 7, 0x000c },
	{ 0x0004, 8, 0x000d },
	{ 0x0007, 8, 0x000e },
	{ 0x0018, 9, 0x000f },
	{ 0x0017, 10, 0x0010 },
	{ 0x0018, 10, 0x0011 },
	{ 0x0008, 10, 0x0012 },
	{ 0x0067, 11, 0x0013 },
	{ 0x0068, 11, 0x0014 },
	{ 0x006c, 11, 0x0015 },
	{ 0x0037, 11, 0x0016 },
	{ 0x0028, 11, 0x0017 },
	{ 0x0017, 11, 0x0018 },
	{ 0x0018, 11, 0x0019 },
	{ 0x00ca, 12, 0x001a },
	{ 0x00cb, 12, 0x001b },
	{ 0x00cc, 12, 0x001c },
	{ 0x00cd, 12, 0x001d },
	{ 0x0068, 12, 0x001e },
	{ 0x0069, 12, 0x001f },
	{ 0x006a, 12, 0x0020 },
	{ 0x006b, 12, 0x0021 },
	{ 0x00d2, 12, 0x0022 },
	{ 0x00d3, 12, 0x0023 },
	{ 0x00d4, 12, 0x0024 },
	{ 0x00d5, 12, 0x0025 },
	{ 0x00d6, 12, 0x0026 },
	{ 0x00d7, 12, 0x0027 },
	{ 0x006c, 12, 0x0028 },
	{ 0x006d, 12, 0x0029 },
	{ 0x00da, 12, 0x002a },
	{ 0x00db, 12, 0x002b },
	{ 0x0054, 12, 0x002c },
	{ 0x0055, 12, 0x002d },
	{ 0x0056, 12, 0x002e },
	{ 0x0057, 12, 0x002f },
	{ 0x0064, 12, 0x0030 },
	{ 0x0065, 12, 0x0031 },
	{ 0x0052, 12, 0x0032 },
	{ 0x0053, 12, 0x0033 },
	{ 0x0024, 12, 0x0034 },
	{ 0x0037, 12, 0x0035 },
	{ 0x0038, 12, 0x0036 },
	{ 0x0027, 12, 0x0037 },
	{ 0x0028, 12, 0x0038 },
	{ 0x0058, 12, 0x0039 },
	{ 0x0059, 12, 0x003a },
	{ 0x002b, 12, 0x003b },
	{ 0x002c, 12, 0x003c },
	{ 0x005a, 12, 0x003d },
	{ 0x0066, 12, 0x003e },
	{ 0x0067, 12, 0x003f },
};

static struct proto const blackEncodeTable3[] = {
	{ 0x000f, 10, 0x0040 },
	{ 0x00c8, 12, 0x0080 },
	{ 0x00c9, 12, 0x00c0 },
	{ 0x005b, 12, 0x0100 },
	{ 0x0033, 12, 0x0140 },
	{ 0x0034, 12, 0x0180 },
	{ 0x0035, 12, 0x01c0 },
	{ 0x006c, 13, 0x0200 },
	{ 0x006d, 13, 0x0240 },
	{ 0x004a, 13, 0x0280 },
	{ 0x004b, 13, 0x02c0 },
	{ 0x004c, 13, 0x0300 },
	{ 0x004d, 13, 0x0340 },
	{ 0x0072, 13, 0x0380 },
	{ 0x0073, 13, 0x03c0 },
	{ 0x0074, 13, 0x0400 },
	{ 0x0075, 13, 0x0440 },
	{ 0x0076, 13, 0x0480 },
	{ 0x0077, 13, 0x04c0 },
	{ 0x0052, 13, 0x0500 },
	{ 0x0053, 13, 0x0540 },
	{ 0x0054, 13, 0x0580 },
	{ 0x0055, 13, 0x05c0 },
	{ 0x005a, 13, 0x0600 },
	{ 0x005b, 13, 0x0640 },
	{ 0x0064, 13, 0x0680 },
	{ 0x0065, 13, 0x06c0 },
	{ 0x0001, 12, -2 },
#if 0
	{ 0x0008, 11, 0x0700 },
	{ 0x000c, 11, 0x0740 },
	{ 0x000d, 11, 0x0780 },
	{ 0x0012, 12, 0x07c0 },
	{ 0x0013, 12, 0x0800 },
	{ 0x0014, 12, 0x0840 },
	{ 0x0015, 12, 0x0880 },
	{ 0x0016, 12, 0x08c0 },
	{ 0x0017, 12, 0x0900 },
	{ 0x001c, 12, 0x0940 },
	{ 0x001d, 12, 0x0980 },
	{ 0x001e, 12, 0x09c0 },
	{ 0x001f, 12, 0x0a00 }
#endif
};

/* info structure for main module */
MOD_INFO module_info = {
	"QFax",
	0x0020,
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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

static uint8_t bitsRead = 0;

#define TIMER 0

struct tabent {
	int16_t param;
	uint8_t bits;
	uint8_t c;
};

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);
static uint8_t *srcptr;
static uint8_t eol_count;
static struct tabent *x13a26;
static struct tabent *x13a2a;
static struct tabent x13a2e[256][32];
static struct tabent x1ba2e[256][32];




static uint16_t GetBits(uint8_t n)
{
	uint16_t val;

	val = (srcptr[0] << 8) | srcptr[1];
	val >>= 16 - (bitsRead + n);
	val &= bitmask[n];
	return val;
}


static void skip_to_eol(void)
{
	uint8_t c;
	
	for (;;)
	{
		c = *srcptr & bitval[bitsRead++];
		if (bitsRead > 7)
		{
			bitsRead = 0;
			srcptr++;
		}
		if (c != 0 && eol_count >= 11)
			return;
		if (c != 0)
			eol_count = 0;
		else
			eol_count++;
	}
}


static void decode_line(uint8_t *dest_data)
{
	int16_t d4;
	uint16_t d5;
	uint16_t o4;
	int16_t o2;
	BOOLEAN o0;
	int16_t d7;
	int16_t d3;
	uint8_t d6;
	uint8_t *dst;
	int d0;
	
	o4 = 0;
	bitsRead = 0;
	d4 = 0;
	d5 = 0;
	o0 = TRUE;
	eol_count = 0;
	while (o4 == 0)
	{
		if (o0)
		{
			d5 = GetBits(8);
			d7 = x13a26[d5].param;
			if (d7 == -1)
			{
				srcptr += (bitsRead + 8) >> 3;
				bitsRead = (bitsRead + 8) & 7;
				d3 = x13a26[d5].c;
				d5 = GetBits(5);
				d7 = x13a2e[d3][d5].param;
				if (d7 == -3)
				{
					srcptr -= 2;
					skip_to_eol();
					return;
				}
				if (d7 == -2)
				{
					srcptr += (bitsRead + 4) >> 3;
					bitsRead = (bitsRead + 4) & 7;
					return;
				}
				d6 = x13a2e[d3][d5].bits;
			} else
			{
				d6 = x13a26[d5].bits;
			}
			srcptr += (bitsRead + d6) >> 3;
			bitsRead = (bitsRead + d6) & 7;
			if (d7 < 64)
				o0 = FALSE;
			d4 += d7;
		} else
		{
			d5 = GetBits(8);
			d7 = x13a2a[d5].param;
			if (d7 == -1)
			{
				srcptr += (bitsRead + 8) >> 3;
				bitsRead = (bitsRead + 8) & 7;
				d3 = x13a2a[d5].c;
				d5 = GetBits(5);
				d7 = x1ba2e[d3][d5].param;
				if (d7 == -3)
				{
					srcptr -= 2;
					skip_to_eol();
					return;
				}
				if (d7 == -2)
				{
					srcptr += (bitsRead + 4) >> 3;
					bitsRead = (bitsRead + 4) & 7;
					return;
				}
				d6 = x1ba2e[d3][d5].bits;
			} else
			{
				d6 = x13a2a[d5].bits;
			}
			srcptr += (bitsRead + d6) >> 3;
			bitsRead = (bitsRead + d6) & 7;
			if (d7 < 64)
				o0 = TRUE;
			dst = &dest_data[d4 >> 3];
			d0 = (7 - d4) & 7;
			d4 += d7;
			while (d7 != 0 && d0 >= 0)
			{
				*dst |= (1 << d0);
				d0--;
				d7--;
			}
			o2 = d7 >> 3;
			while (o2-- != 0)
			{
				*++dst = 0xff;
				d7 -= 8;
			}
			if (d7 != 0)
			{
				d0 = 7;
				dst++;
				while (d7-- != 0)
				{
					*dst |= (1 << d0);
					d0--;
				}
			}
		}
	}
}


static void fax_decode(uint8_t *data, uint8_t *dest_data, uint16_t width, uint16_t height)
{
	uint16_t y;
	
	srcptr = data;
#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif
	y = 0;
	do
	{
		decode_line(dest_data);
		dest_data += width;
	} while (++y < height);

#if TIMER
	/* wie schnell waren wir? */
	printf("Zeit: %lu\n", get_timer());
#endif
}




static BOOLEAN mktables(void)
{
	int d3;
	int d4;
	uint8_t d5;
	int d6;
	int param;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif
	if ((x13a26 = SMalloc(256 * sizeof(*x13a26))) == NULL)
		return FALSE;
	d3 = 0;
	do
	{
		d4 = 0;
		do
		{
			x13a2e[d3][d4].param = -3;
			x13a2e[d3][d4].bits = 5;
		} while (++d4 < 32);
	} while (++d3 < 256);
	
	d3 = 0;
	do
	{
		if (whiteEncodeTable2[d3].bits <= 8)
		{
			d5 = 8 - whiteEncodeTable2[d3].bits;
			d4 = 0;
			do
			{
				d6 = (whiteEncodeTable2[d3].a << d5) | d4;
				x13a26[d6].param = whiteEncodeTable2[d3].param;
				x13a26[d6].bits = whiteEncodeTable2[d3].bits;
			} while (++d4 < (1 << d5));
		} else
		{
			param = whiteEncodeTable2[d3].a;
			param >>= whiteEncodeTable2[d3].bits - 8;
			x13a26[param].param = -1;
			x13a26[param].c = param;
			x13a26[param].bits = 8;
			d5 = 5 - (whiteEncodeTable2[d3].bits - 8);
			d4 = 0;
			do
			{
				d6 = ((whiteEncodeTable2[d3].a << d5) | d4) & bitmask[5];
				x13a2e[param][d6].param = whiteEncodeTable2[d3].param;
				x13a2e[param][d6].bits = whiteEncodeTable2[d3].bits - 8;
			} while (++d4 < (1 << d5));
		}
	} while (++d3 < 64);
	
	d3 = 0;
	do
	{
		if (whiteEncodeTable3[d3].bits <= 8)
		{
			d5 = 8 - whiteEncodeTable3[d3].bits;
			d4 = 0;
			do
			{
				d6 = (whiteEncodeTable3[d3].a << d5) | d4;
				x13a26[d6].param = whiteEncodeTable3[d3].param;
				x13a26[d6].bits = whiteEncodeTable3[d3].bits;
			} while (++d4 < (1 << d5));
		} else
		{
			param = whiteEncodeTable3[d3].a;
			param >>= whiteEncodeTable3[d3].bits - 8;
			x13a26[param].param = -1;
			x13a26[param].c = param;
			x13a26[param].bits = 8;
			d5 = 5 - (whiteEncodeTable3[d3].bits - 8);
			d4 = 0;
			do
			{
				d6 = ((whiteEncodeTable3[d3].a << d5) | d4) & bitmask[5];
				x13a2e[param][d6].param = whiteEncodeTable3[d3].param;
				x13a2e[param][d6].bits = whiteEncodeTable3[d3].bits - 8;
			} while (++d4 < (1 << d5));
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
	if ((x13a2a = SMalloc(256 * sizeof(*x13a2a))) == NULL)
	{
		SMfree(x13a26);
		x13a26 = NULL;
		return FALSE;
	}

	d3 = 0;
	do
	{
		d4 = 0;
		do
		{
			x1ba2e[d3][d4].param = -3;
			x1ba2e[d3][d4].bits = 5;
		} while (++d4 < 32);
	} while (++d3 < 256);

	d3 = 0;
	do
	{
		if (blackEncodeTable2[d3].bits <= 8)
		{
			d5 = 8 - blackEncodeTable2[d3].bits;
			d4 = 0;
			do
			{
				d6 = (blackEncodeTable2[d3].a << d5) | d4;
				x13a2a[d6].param = blackEncodeTable2[d3].param;
				x13a2a[d6].bits = blackEncodeTable2[d3].bits;
			} while (++d4 < (1 << d5));
		} else
		{
			param = blackEncodeTable2[d3].a;
			param >>= blackEncodeTable2[d3].bits - 8;
			x13a2a[param].param = -1;
			x13a2a[param].c = param;
			x13a2a[param].bits = 8;
			d5 = 5 - (blackEncodeTable2[d3].bits - 8);
			d4 = 0;
			do
			{
				d6 = ((blackEncodeTable2[d3].a << d5) | d4) & bitmask[5];
				x1ba2e[param][d6].param = blackEncodeTable2[d3].param;
				x1ba2e[param][d6].bits = blackEncodeTable2[d3].bits - 8;
			} while (++d4 < (1 << d5));
		}
	} while (++d3 < 64);

	d3 = 0;
	do
	{
		if (blackEncodeTable3[d3].bits <= 8)
		{
			d5 = 8 - blackEncodeTable3[d3].bits;
			d4 = 0;
			do
			{
				d6 = (blackEncodeTable3[d3].a << d5) | d4;
				x13a2a[d6].param = blackEncodeTable3[d3].param;
				x13a2a[d6].bits = blackEncodeTable3[d3].bits;
			} while (++d4 < (1 << d5));
		} else
		{
			param = blackEncodeTable3[d3].a;
			param >>= blackEncodeTable3[d3].bits - 8;
			x13a2a[param].param = -1;
			x13a2a[param].c = param;
			x13a2a[param].bits = 8;
			d5 = 5 - (blackEncodeTable3[d3].bits - 8);
			d4 = 0;
			do
			{
				d6 = ((blackEncodeTable3[d3].a << d5) | d4) & bitmask[5];
				x1ba2e[param][d6].param = blackEncodeTable3[d3].param;
				x1ba2e[param][d6].bits = blackEncodeTable3[d3].bits - 8;
			} while (++d4 < (1 << d5));
		}
	} while (++d3 < 28);
#if TIMER
	/* wie schnell waren wir? */
	printf("Zeit: %lu\n", get_timer());
#endif

	return TRUE;
}


static const char *get_extension(const char *filename)
{
	const char *p;
	const char *p2;
	
	p = strrchr(filename, '/');
	p2 = strrchr(filename, '\\');
	if (p2 == NULL || p > p2)
		p2 = p;
	if (p2 == NULL)
		p2 = filename;
	p = strrchr(p2, '.');
	if (p != NULL)
		p++;
	else
		p = strrchr(p2, '\0');
	
	return p;
}


short imp_module_main(GARGAMEL *smurf_struct)
{
	const char *extstart;
	uint8_t *pic_data;
	uint8_t type;
	uint16_t width;
	uint16_t height;
	uint8_t *dest_data;
	uint8_t *pal;
	
	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	pic_data = smurf_struct->smurf_pic->pic_data;
	
	extstart = get_extension(smurf_struct->smurf_pic->filename);
	if ((extstart[0] != 'F' && extstart[0] != 'f') ||
		extstart[1] < '0' || extstart[1] > '9' ||
		extstart[2] < '0' || extstart[2] > '9')
		return M_INVALID;
	
	type = pic_data[24];
	if (type == 0)
		type = 1;
	if (type > 1)
	{
		/* FIXME: translate */
		/* FIXME: must not call AES functions in modules */
		form_alert(0, "[1][2-dimensionale Komprimierung wird | noch nicht unterstÅtzt][ OK ]");
		return M_UNKNOWN_TYPE;
	}
	
	height = (pic_data[26] << 8) | pic_data[25];
	width = (pic_data[28] << 8) | pic_data[27];
#if 0
	{
	uint8_t dpi;
	if (pic_data[29] == 0)
		dpi = 100;
	else
		dpi = 200;
	}
#endif
	/* FIXME: translate */
	strcpy(smurf_struct->smurf_pic->format_name, "QFax Faxdatei");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;
	smurf_struct->services->reset_busybox(128, "QFax Faxdatei 1 Bit");
	
	width = (width + 7) >> 3;
	if ((dest_data = SMalloc((long) width * height)) == NULL)
		return M_MEMORY;

	if (mktables() == FALSE)
	{
		SMfree(dest_data);
		return M_MEMORY;
	}
	fax_decode(pic_data + 256, dest_data, width, height);
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
	SMfree(x13a26);
	x13a26 = NULL;
	SMfree(x13a2a);
	x13a2a = NULL;
			
	return M_PICDONE;					/* das wars. */
}
