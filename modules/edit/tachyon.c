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
/*							Tachyon							*/
/* Version 0.1  --  10.9.2000 - 20.9.2000					*/
/*	  1, 2, 4, 7, 8, 24 Bit									*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define DEBUG 0
#define TIMER 0

void tachyon_pixel(uint8_t *data, uint8_t *cliptab, unsigned short width) ASM_NAME("_tachyon_pixel");

static short (*busybox)(short pos);

MOD_INFO module_info = {
	"Tachyon",
	0x0010,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-öberschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 2, 4, 7, 8, 24, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	0,
};



/* Holt sich den Helligkeitswert des Pixels, */
/* invertiert den und konvertiert zurÅck. */
/* l = r * 0,299 + g * 0,587 + b * 0,114 */
static void do_tachyon(uint8_t *data, unsigned short width, unsigned short height)
{
	uint8_t cliptab[127 + 256 + 127];
	uint8_t r, g, b;
	short r2, g2, b2;
	unsigned short x, y;
	unsigned short bh, bl;
	long l;

#if DEBUG
	(void) Cconws("\033H");
#endif

#if TIMER
/* wie schnell sind wir? */
	init_timer();
#endif

	if ((bh = height / 10) == 0)		/* busy-height */
		bh = height;
	bl = 0;								/* busy-length */

	/* w = width * 3; */

	/* Cliptable aufbauen */
	memset(cliptab, 0, 127);
	y = 0;
	do
	{
		*(cliptab + 127 + y) = y;
	} while (++y < 256);
	memset(cliptab + 383, 0xff, 127);

	y = 0;
	while (y++ < height)
	{
		if (!(y % bh))
		{
			busybox(bl);
			bl += 12;
		}

		x = width;
		while (x--)
		{
			r = *data;
			g = *(data + 1);
			b = *(data + 2);


#if DEBUG
			printf("r: %d, g: %d, b: %d\n", r, g, b);
#endif

			l = (long) r *1225L + (long) g * 2404L + (long) b * 467L;


			/* die eigentliche Funktion und Aufgabe des Moduls, */
			/* invertieren der Helligkeit, die 2048 sind 0,5 << 12 */
			/* zum Ausgleich der Nachkommastellen */
			l = 1048576L - (l << 1) + 2048L;


			r2 = (short) ((((long) r << 12L) + l) >> 12L);
			r2 = cliptab[127 + r2];
#if 0
			if (r2 > 255)
				r2 = 255;
			else if (r2 < 0)
				r2 = 0;
#endif
			g2 = (short) ((((long) g << 12L) + l) >> 12L);
			g2 = cliptab[127 + g2];
#if 0
			if (g2 > 255)
				g2 = 255;
			else if (g2 < 0)
				g2 = 0;
#endif
			b2 = (short) ((((long) b << 12L) + l) >> 12L);
			b2 = cliptab[127 + b2];
#if 0
			if (b2 < 0)
				b2 = 0;
			else if (b2 > 255)
				b2 = 255;
#endif
#if DEBUG
			printf(": %ld\n", l);
			printf("r2: %d, g2: %d, b2: %d\n", r2, g2, b2);
			printf("r: %d, g: %d, b: %d\n\n", r, g, b);
			(void)Cnecin();
#endif

			*data++ = r2;
			*data++ = g2;
			*data++ = b2;
		}

#if 0
		tachyon_pixel(data, cliptab, width);
		data += w;
#endif
	}

#if TIMER
	/* wie schnell waren wir? */
	printf("\n%lu", get_timer());
	(void)Cnecin();
#endif
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*						Tachyon						*/
/*	Invertiert die Helligkeit ohne die Farbigkeit	*/
/*	anzutasten.										*/
/*		1-8 und 24 Bit 								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t BitsPerPixel;
	unsigned short width, height;
	unsigned long length;


	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->module_mode = M_STARTED;
		break;

	case MEXEC:
		busybox = smurf_struct->services->busybox;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24 || BitsPerPixel == 1)
			{
				data = smurf_struct->smurf_pic->pic_data;

				width = smurf_struct->smurf_pic->pic_width;
				height = smurf_struct->smurf_pic->pic_height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;
			}

			if (BitsPerPixel == 1)
			{
				length = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

				while (length--)
				{
					uint8_t v = ~*data;
					*data++ = v;
				}
			} else
			{
				if (BitsPerPixel != 24)
				{
					width = 256;
					height = 1;
				}

				do_tachyon(data, width, height);
			}
		}
#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;
		break;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
