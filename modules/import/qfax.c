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

#define TIMER 1

static uint8_t x137f0 = 0;

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
struct t13a26 x13a2e[256][32];
struct t13a26 x1ba2e[256][32];

static char *get_extension(const char *filename);
static void x10178(uint8_t *data, uint8_t *dest_data, uint16_t width, uint16_t height);
static void x101e0(uint8_t *dest_data);
static uint16_t x10428(int);
static void x10456(void);
static void x104a4(void);



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
	x104a4();
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
	x137f0 = 0;
	d4 = 0;
	d5 = 0;
	o0 = TRUE;
	x13a22 = 0;
	while (o4 == 0)
	{
		if (o0 == TRUE)
		{
			d5 = x10428(8);
			d7 = x13a26[d5].a;
			if (d7 == -1)
			{
				x13a1e += (int)(x137f0 + 8) >> 3;
				x137f0 = (x137f0 + 8) & 7;
				d3 = x13a26[d5].c;
				d5 = x10428(5);
				d7 = x13a2e[d3][d5].a;
				if (d7 == -3)
				{
					x13a1e -= 2;
					x10456();
					return;
				}
				if (d7 == -2)
				{
					x13a1e += (int)(x137f0 + 4) >> 3;
					x137f0 = (x137f0 + 4) & 7;
					return;
				}
				d6 = x13a2e[d3][d5].b;
			} else
			{
				d6 = x13a26[d5].b;
			}
			x13a1e += (int)(x137f0 + d6) >> 3;
			x137f0 = (x137f0 + d6) & 7;
			if (d7 < 64)
				o0 = FALSE;
			d4 += d7;
		} else
		{
			d5 = x10428(8);
			d7 = x13a2a[d5].a;
			if (d7 == -1)
			{
				x13a1e += (int)(x137f0 + 8) >> 3;
				x137f0 = (x137f0 + 8) & 7;
				d3 = x13a2a[d5].c;
				d5 = x10428(5);
				d7 = x1ba2e[d3][d5].a;
				if (d7 == -3)
				{
					x13a1e -= 2;
					x10456();
					return;
				}
				if (d7 == -2)
				{
					x13a1e += (int)(x137f0 + 4) >> 3;
					x137f0 = (x137f0 + 4) & 7;
					return;
				}
				d6 = x1ba2e[d3][d5].b;
			} else
			{
				d6 = x13a2a[d5].b;
			}
			x13a1e += (int)(x137f0 + d6) >> 3;
			x137f0 = (x137f0 + d6) & 7;
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


static uint16_t x10428(int x)
{
	return x;
}


static void x10456(void)
{
}


static void x104a4(void)
{
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

/*
struct t13a26 x13a2e[256][32];
struct t13a26 x1ba2e[256][32];
*/
