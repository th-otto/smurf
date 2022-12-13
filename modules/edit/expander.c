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
/*						Expander-Modul						*/
/* Version 0.1  --  21.04.97								*/
/*	  2-8, 24 Bit											*/
/* Version 0.2  --  29.09.97								*/
/*	  Bescheuerten Fehler gefixt, der bei Palettenbildern	*/
/*	  zu einem ganz schwarzen Bild fÅhrte.					*/
/* Version 0.3  --  27.03.98								*/
/*	  Zeilenweise Assemblerleseroutine eingebaut.			*/
/* Version 0.4  --  xx.xx.98								*/
/*	  16 Bit												*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

static short (*busybox)(short pos);

MOD_INFO module_info = {
	"Expander",
	0x0030,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 64 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	2, 4, 7, 8, 24, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Der Expander (auch als normalize bekannt)	*/
/*		2-8, 16, 24 Bit 							*/
/* Die Expander skaliert den Helligkeitsumfang auf	*/
/* den vollen Umfang von 255.						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t *odata;
	uint8_t *palette = 0;
	uint8_t *pal;
	uint8_t *normalize_map;
	uint8_t *pixbuf;
	uint8_t *line;
	uint8_t BitsPerPixel;
	uint8_t pixval;
	uint8_t greyval;
	uint8_t high, low;

	unsigned short x, y;
	unsigned short width, height;
	unsigned short bh, bl;

	unsigned long *histogram;
	unsigned long length;
	unsigned long i;
	unsigned long intensity;
	unsigned long threshold_intensity;
	unsigned long w;
	unsigned long planelength;
	uint8_t v;

	/* Wenn das Modul zum ersten Mal gestartet wurde */
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_STARTED;
		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		busybox = smurf_struct->services->busybox;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		histogram = (unsigned long *) Malloc(256 * sizeof(*histogram) + 256 * sizeof(*normalize_map));

		if (histogram == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}
		normalize_map = (uint8_t *)(histogram + 256);
		memset(histogram, 0, 256 * sizeof(*histogram) + 256 * sizeof(*normalize_map));

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;
				odata = data;
			} else
			{
				data = smurf_struct->smurf_pic->pic_data;
				odata = data;
				palette = smurf_struct->smurf_pic->palette;
			}

			if ((bh = height / 5) == 0)	/* busy-height */
				bh = height;
			bl = 0;						/* busy-length */

			/* Histogramm (Graustufen) erstellen */
			if (BitsPerPixel == 24)
			{
				y = 0;
				do
				{
					if (!(y % bh))
					{
						busybox(bl);
						bl += 15;
					}

					x = 0;
					do
					{
						greyval = ((((long) data[0] * 871L) + ((long) data[1] * 2929L) + ((long) data[2] * 295L)) >> 12);
						data += 3;
						histogram[greyval]++;
					} while (++x < width);
				} while (++y < height);
			} else
			{
				if (smurf_struct->smurf_pic->format_type == FORM_STANDARD)
				{
					pixbuf = (uint8_t *) Malloc(width + 7);

					w = (unsigned long) ((width + 7) / 8);
					planelength = w * (unsigned long) height;	/* LÑnge einer Plane in Bytes */

					y = 0;
					do
					{
						if (!(y % bh))
						{
							busybox(bl);
							bl += 25;
						}

						memset(pixbuf, 0, width);
						getpix_std_line(data, pixbuf, BitsPerPixel, planelength, width);
						data += w;
						line = pixbuf;

						x = 0;
						do
						{
							pixval = *line++;
							pal = palette + pixval * 3;

							greyval = ((((long) pal[0] * 871L) + ((long) pal[1] * 2929L) + ((long) pal[2] * 295L)) >> 12);

							histogram[greyval]++;
						} while (++x < width);
					} while (++y < height);

					Mfree(pixbuf);
				} else
				{
					y = 0;
					do
					{
						if (!(y % bh))
						{
							busybox(bl);
							bl += 25;
						}

						x = 0;
						do
						{
							pixval = *data++;
							pal = palette + pixval + pixval + pixval;

							greyval = ((((long) *pal * 871L) + ((long) *(pal + 1) * 2929L) + ((long) *(pal + 2) * 295L)) >> 12);

							histogram[greyval]++;
						} while (++x < width);
					} while (++y < height);
				}
			}

			data = odata;

			/* Histogrammgrenzen durch Suche nach 1 Prozent Schritt finden */
			threshold_intensity = (((long) width * (long) height) / 100);

			intensity = 0;
			for (low = 0; low < 255; low++)
			{
				intensity += histogram[low];

				if (intensity > threshold_intensity)
					break;
			}

			intensity = 0;
			for (high = 255; high != 0; high--)
			{
				intensity += histogram[high];

				if (intensity > threshold_intensity)
					break;
			}

			if (low == high)
			{
				/* Fast kein Kontrast; 0-Grenze fÅr Grenzfindung benutzen. */
				threshold_intensity = 0;
				intensity = 0;
				for (low = 0; low < 255; low++)
				{
					intensity += histogram[low];

					if (intensity > threshold_intensity)
						break;
				}

				intensity = 0;
				for (high = 255; high != 0; high--)
				{
					intensity += histogram[high];

					if (intensity > threshold_intensity)
						break;
				}

				if (low == high)
					return;				/* zero span bound */
			}

			/* Das Histogramm aufziehen um ein normalisiertes Mapping zu erstellen */
			for (i = 0; i < 256; i++)
			{
				if (i < low)
					normalize_map[i] = 0;
				else if (i > high)
					normalize_map[i] = 255 - 1;
				else
					normalize_map[i] = (255 - 1) * (i - low) / (high - low);
			}

			/* Normalize */
			if (BitsPerPixel == 24)
			{
				if ((bh = height / 5) == 0)	/* busy-height */
					bh = height;

				y = 0;
				do
				{
					if (!(y % bh))
					{
						busybox(bl);
						bl += 10;
					}

					x = 0;
					do
					{
						v = *data;
						*data++ = normalize_map[v];
						v = *data;
						*data++ = normalize_map[v];
						v = *data;
						*data++ = normalize_map[v];
					} while (++x < width);
				} while (++y < height);
			} else
			{
				pal = palette;
				length = 256;
				while (length--)
				{
					v = *pal;
					*pal++ = normalize_map[v];
					v = *pal;
					*pal++ = normalize_map[v];
					v = *pal;
					*pal++ = normalize_map[v];
				}
			}
		}

		Mfree(histogram);

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
