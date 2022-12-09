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
/*						Equalizer-Modul						*/
/* Version 0.1  --  21.04.97								*/
/*	  1-8, 24 Bit											*/
/* Version 0.2  --  xx.xx.97								*/
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
	"Equalizer",
	0x0010,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 64,
	0, 64,
	0, 64,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
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
/*					Der Equalizer					*/
/*		1-8, 24 Bit 								*/
/* Setzt die Helligkeit im ganzen Bild auf den		*/
/* gleichen Wert.									*/
/* Daž die Ergebnisse bei WERKZE24.TGA und anderen	*/
/* so bescheiden sind, ist normal und tritt bei		*/
/* anderen Programmen auch auf.						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t *odata;
	uint8_t *equalize_map;
	uint8_t BitsPerPixel;
	uint8_t greyval;

	unsigned short x, y;
	unsigned short width, height;
	unsigned short bh, bl;

	unsigned long *histogram;
	unsigned long *map;
	unsigned long length;
	unsigned long i, j;
	unsigned long scale_factor;
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

		histogram = (unsigned long *) Malloc(256 * sizeof(*histogram) + 256 * sizeof(*map) + 256 * sizeof(*equalize_map));
		if (histogram == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return;
		}
		map = histogram + 256;
		equalize_map = (uint8_t *)(map + 256);

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;
				odata = data;
			} else
			{
				data = smurf_struct->smurf_pic->palette;
				odata = data;

				length = SM_PALETTE_MAX;
			}

			memset(histogram, 0, 256 * sizeof(*histogram));

			/* Histogramm (Graustufen) erstellen */
			if (BitsPerPixel == 24)
			{
				if ((bh = height / 5) == 0)	/* busy-height */
					bh = height;
				bl = 0;					/* busy-length */

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
				i = 0;
				while (i++ < length)
				{
					greyval = ((((long) data[0] * 871L) + ((long) data[1] * 2929L) + ((long) data[2] * 295L)) >> 12);
					data += 3;

					histogram[greyval]++;
				}
			}

			data = odata;

			/* Integrate the histogram to get the equalization map. */
			j = 0;
			i = 0;
			while (i < 256)
			{
				j += histogram[i];
				map[i] = j;

				i++;
			}


			if (map[255] == 0)
			{
				Mfree(histogram);

				smurf_struct->module_mode = M_DONEEXIT;
				return;
			}


			/* Equalize */
			scale_factor = (255L << 16) / map[255];
			for (i = 0; i < 256; i++)
				equalize_map[i] = (((map[i] * scale_factor) + (1L << 15)) >> 16);

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
						*data++ = equalize_map[v];
						v = *data;
						*data++ = equalize_map[v];
						v = *data;
						*data++ = equalize_map[v];
					} while (++x < width);
				} while (++y < height);
			} else
			{
				while (length--)
				{
					v = *data;
					*data++ = equalize_map[v];
					v = *data;
					*data++ = equalize_map[v];
					v = *data;
					*data++ = equalize_map[v];
				}
			}
		}

		Mfree(histogram);

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void) Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;
		break;

		/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
