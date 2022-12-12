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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*						Sinuswellen-Modul					*/
/* Version 0.1  --  11.07.98 - 12.07.98						*/
/*	  8, 24 Bit												*/
/* Version 0.2  --  22.08.98								*/
/*	  Die Bildgr”že kann jetzt beibehalten werden.			*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Sinuswellen"
#define TEXT2 "St„rke"
#define TEXT3 "Weite"
#define TEXT4 "Horizontal"
#define TEXT5 "vertikal"
#define TEXT6 "Gr”že gleich"
#elif COUNTRY==0
#define TEXT1 "Sinuswellen"
#define TEXT2 "St„rke"
#define TEXT3 "Weite"
#define TEXT4 "Horizontal"
#define TEXT5 "Vertical"
#define TEXT6 "Size equal"
#elif COUNTRY==2
#define TEXT1 "Sinuswellen"
#define TEXT2 "St„rke"
#define TEXT3 "Weite"
#define TEXT4 "Horizontal"
#define TEXT5 "Vertical"
#define TEXT6 "Size equal"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

#define EXTEND	0
#define HOLD	1
#define HORIZONTAL	1
#define VERTIKAL	2

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

short (*busybox)(short pos);

MOD_INFO module_info = {
	TEXT1,
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	TEXT2,
	TEXT3,
	"",
	"",
	TEXT4,
	TEXT5,
	TEXT6,
	"",
	"",
	"",
	"",
	"",
	0, 255,
	0, 255,
	0, 100,
	0, 100,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	16, 40, 20, 20,
	3, 4, 0, 0,
	0, 0, 0, 0,
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	8, 24, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};





/* Generiert die Sinuswelle in Y-Richtung */
static uint8_t *sinus_horizontal(uint8_t *buffer, long *sinustab, unsigned short width, unsigned short height, unsigned short sinheight, uint8_t BitsPerPixel)
{
	uint8_t *ziel;
	uint8_t *oziel;
	unsigned short x, y;
	long newrealwidth;

	newrealwidth = ((long) (width + 2 * sinheight) * BitsPerPixel) >> 3;

	if ((ziel = (uint8_t *) SMalloc(newrealwidth * (long) height)) == 0)
		return NULL;
	oziel = ziel;

	if (BitsPerPixel == 8)
	{
		y = 0;
		do
		{
			ziel = oziel + (newrealwidth * y) + sinheight + *sinustab++;

			x = 0;
			do
			{
				*ziel++ = *buffer++;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		y = 0;
		do
		{
			ziel = oziel + (newrealwidth * y) + (sinheight + *sinustab++) * 3L;

			x = 0;
			do
			{
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
			} while (++x < width);
		} while (++y < height);
	}

	return oziel;
}


/* Generiert die Sinuswelle in Y-Richtung */
static uint8_t *sinus_horizontal3(uint8_t *buffer, long *sinustab, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *addr;
	uint8_t *begin;
	uint8_t *end;
	unsigned short x, y;
	long realwidth;

	realwidth = ((long) width * BitsPerPixel) >> 3;

	if ((ziel = (uint8_t *) SMalloc(realwidth * (long) height)) == 0)
		return NULL;
	oziel = ziel;

	if (BitsPerPixel == 8)
	{
		y = 0;
		do
		{
			ziel = oziel + realwidth * y;

			begin = ziel;
			end = ziel + realwidth - 1;

			ziel += *sinustab++;

			x = 0;
			do
			{
				if (ziel < begin)
					addr = ziel++ + realwidth;
				else if (ziel > end)
					addr = ziel++ - realwidth;
				else
					addr = ziel++;

				*addr = *buffer++;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		y = 0;
		do
		{
			ziel = oziel + realwidth * y;

			begin = ziel;
			end = ziel + realwidth - 3;

			ziel += *sinustab++ * 3L;

			x = 0;
			do
			{
				if (ziel < begin)
					addr = ziel + realwidth;
				else if (ziel > end)
					addr = ziel - realwidth;
				else
					addr = ziel;

				*addr++ = *buffer++;
				*addr++ = *buffer++;
				*addr = *buffer++;
				ziel += 3;
			} while (++x < width);
		} while (++y < height);
	}

	return oziel;
}


/* Generiert die Sinuswelle in X-Richtung */
static uint8_t *sinus_vertikal(uint8_t *buffer, long *sinustab, unsigned short width, unsigned short height, unsigned short sinheight, uint8_t BitsPerPixel)
{
	uint8_t *ziel;
	uint8_t *oziel;
	unsigned short x, y;
	long *osinustab = sinustab;
	long realwidth;

	realwidth = ((long) width * BitsPerPixel) >> 3;

	if ((ziel = (uint8_t *) SMalloc(realwidth * ((long) height + 2 * sinheight))) == 0)
		return NULL;
	oziel = ziel;

	ziel += realwidth * sinheight;

	if (BitsPerPixel == 8)
	{
		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				*(ziel++ + *sinustab++) = *buffer++;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				*(ziel++ + *sinustab) = *buffer++;
				*(ziel++ + *sinustab) = *buffer++;
				*(ziel++ + *sinustab++) = *buffer++;
			} while (++x < width);
		} while (++y < height);
	}

	return oziel;
}


#if 0 /* unused */
/* Generiert die Sinuswelle in X-Richtung unter Beibehaltung der Bildgr”že */
static uint8_t *sinus_vertikal2(uint8_t *buffer, long *sinustab, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *addr;
	uint8_t *begin;
	uint8_t *end;
	unsigned short x, y;
	long *osinustab = sinustab;
	long picheight;

	picheight = (((long) width * BitsPerPixel) >> 3) * (long) height;

	if ((ziel = (uint8_t *) SMalloc(picheight)) == 0)
		return NULL;
	oziel = ziel;

	begin = ziel;

	if (BitsPerPixel == 8)
	{
		end = ziel + picheight - 1;

		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				if ((addr = ziel + *sinustab++) >= begin && addr <= end)
					*addr = *buffer;

				ziel++;
				buffer++;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		end = ziel + picheight - 3;

		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				if ((addr = ziel + *sinustab++) >= begin && addr <= end)
				{
					*addr = *buffer;
					*(addr + 1) = *(buffer + 1);
					*(addr + 2) = *(buffer + 2);
				}

				ziel += 3;
				buffer += 3;
			} while (++x < width);
		} while (++y < height);
	}

	return oziel;
}
#endif


/* Generiert die Sinuswelle in X-Richtung unter Beibehaltung der Bildgr”že */
static uint8_t *sinus_vertikal3(uint8_t *buffer, long *sinustab, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *addr;
	uint8_t *begin;
	uint8_t *end;
	unsigned short x, y;
	long *osinustab = sinustab;
	long picheight;


	picheight = (((long) width * BitsPerPixel) >> 3) * (long) height;

	if ((ziel = (uint8_t *) SMalloc(picheight)) == 0)
		return NULL;
	oziel = ziel;

	begin = ziel;

	if (BitsPerPixel == 8)
	{
		end = ziel + picheight - 1;

		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				if ((addr = ziel + *sinustab++) < begin)
					addr += picheight;
				else if (addr > end)
					addr -= picheight;

				*addr = *buffer++;

				ziel++;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		end = ziel + picheight - 3;

		y = 0;
		do
		{
			sinustab = osinustab;

			x = 0;
			do
			{
				if ((addr = ziel + *sinustab++) < begin)
					addr += picheight;
				else if (addr > end)
					addr -= picheight;

				*addr = *buffer++;
				*(addr + 1) = *buffer++;
				*(addr + 2) = *buffer++;

				ziel += 3;
			} while (++x < width);
		} while (++y < height);
	}

	return oziel;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Sinuswellen						*/
/*		8, 24 Bit 									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t BitsPerPixel;
	uint8_t direction;
	uint8_t mode;
	unsigned short i;
	unsigned short width, height;
	unsigned short sinheight;
	unsigned short wide;
	long *sinustab;
	long realwidth;
	long offset;

	double sindiv = 180.0 / M_PI;
	double widemul;

	switch (smurf_struct->module_mode)
	{
		/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;
	
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

		busybox = smurf_struct->services->busybox;

		if (smurf_struct->check1 > smurf_struct->check2)
			direction = HORIZONTAL;
		else
			direction = VERTIKAL;

		mode = smurf_struct->check3;

		sinheight = (short) smurf_struct->slide1;
		wide = (short) smurf_struct->slide2;
		widemul = 180 / wide;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		buffer = smurf_struct->smurf_pic->pic_data;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		sinustab = (long *) Malloc(width * sizeof(*sinustab));
		if (sinustab == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}
		realwidth = (width * BitsPerPixel) >> 3;

		if (direction == HORIZONTAL)
			offset = 1;
		else
			offset = realwidth;

		i = 0;
		do
		{
			sinustab[i] = (long) (sin(i * widemul / sindiv) * (double) sinheight) * offset;
		} while (++i < width);

		if (direction == HORIZONTAL)
		{
			if (mode == HOLD)
				ziel = sinus_horizontal3(buffer, sinustab, width, height, BitsPerPixel);
			else
				ziel = sinus_horizontal(buffer, sinustab, width, height, sinheight, BitsPerPixel);
		} else if (mode == HOLD)
		{
			ziel = sinus_vertikal3(buffer, sinustab, width, height, BitsPerPixel);
		} else
		{
			ziel = sinus_vertikal(buffer, sinustab, width, height, sinheight, BitsPerPixel);
		}
		Mfree(sinustab);

		if (ziel == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		} else
		{
			if (mode == EXTEND)
			{
				if (direction == HORIZONTAL)
					smurf_struct->smurf_pic->pic_width = width + 2 * sinheight;
				else
					smurf_struct->smurf_pic->pic_height = height + 2 * sinheight;
			}
			smurf_struct->smurf_pic->pic_data = ziel;
			SMfree(buffer);
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
