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
/*					Highlight/Shadow-Modul					*/
/* Version 0.1  --  21.10.97								*/
/*	  1-8, 16 und 24 Bit									*/
/* Version 0.2  --  13.11.97								*/
/*	  Sliderwertebereich berichtigt und Shadow-Berechnung	*/
/*	  vollkommen umgestellt. šberl„ufe sollten nun der Ver-	*/
/*	  gangenheit angeh”ren.									*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

MOD_INFO module_info = {
	"Highlight/Shadow",
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ "Highlight", "Shadow", "", "" },
	{ "", "", "", "" },
	{ "", "", "", "" },
	{ { 0, 100 }, { 0, 100 }, { 0, 64 }, { 0, 64 } },
	{ { 0, 10 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 100, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	2, 4, 7, 8, 16, 24, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	0
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Highlight/Shadow-Modul				*/
/*		1-8, 16 und 24 Bit							*/
/*	Die Ver„nderung des Highlight setzt die Hellig-	*/
/*	keit der Pixel ber dem Schwellwert auf 255 und	*/
/*	erh”ht die aller anderen leicht.				*/
/*	Beim Shadow das Gleiche andersherum.			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t highlighttab[256];
	uint8_t shadowtab[256];
	uint8_t BitsPerPixel;
	uint8_t slidvalh;
	uint8_t slidvals;
	uint8_t r, g, b;
	uint8_t bereich;
	short i;
	short val;
	uint16_t *data16;
	uint16_t width, height;
	long sval;
	unsigned long length;
	unsigned long hval;

	switch (smurf_struct->module_mode )
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
	/* Einstellformular wurde mit START verlassen - Funktion ausfhren */
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		slidvalh = smurf_struct->slide[0] * 255 / 100;
		slidvals = smurf_struct->slide[1] * 255 / 100;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		bereich = 255 - slidvals;

		hval = (long) (255 - slidvalh) * 100 / slidvalh + 1;
		i = 0;
		do
		{
			val = (i + i * hval / 100);
			if (val > 255)
				val = 255;
			highlighttab[i] = val;

			sval = (bereich - (i - slidvals)) * 100 / (bereich + 10);
			val = (i - i * sval / 100);
			if (val < 0)
				val = 0;
			shadowtab[i] = val;
		} while (++i < 255);

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				width = smurf_struct->smurf_pic->pic_width;
				height = smurf_struct->smurf_pic->pic_height;
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width *(unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;
				length = SM_PALETTE_MAX;
			}

			if (slidvalh < 255 && slidvals > 0)
			{
				do
				{
					/* Rot */
					if (*data >= slidvalh)
					{
						*data++ = 255;
					} else
					{
						*data = highlighttab[*data];
						if (*data <= slidvals)
							*data++ = 0;
						else
							*data = shadowtab[*data], data++;
					}

					/* Grn */
					if (*data >= slidvalh)
					{
						*data++ = 255;
					} else
					{
						*data = highlighttab[*data];
						if (*data <= slidvals)
							*data++ = 0;
						else
							*data = shadowtab[*data], data++;
					}

					/* Blau */
					if (*data >= slidvalh)
					{
						*data++ = 255;
					} else
					{
						*data = highlighttab[*data];
						if (*data <= slidvals)
							*data++ = 0;
						else
							*data = shadowtab[*data], data++;
					}
				} while (--length);
			} else
			{
				do
				{
					if (slidvalh < 255)
					{
						/* Rot */
						r = *data;
						if (r >= slidvalh)
							*data++ = 255;
						else
							*data++ = highlighttab[r];

						/* Grn */
						g = *data;
						if (g >= slidvalh)
							*data++ = 255;
						else
							*data++ = highlighttab[g];

						/* Blau */
						b = *data;
						if (b >= slidvalh)
							*data++ = 255;
						else
							*data++ = highlighttab[b];
					}

					if (slidvals > 0)
					{
						/* Rot */
						r = *data;
						if (r <= slidvals)
							*data++ = 0;
						else
							*data++ = shadowtab[r];

						/* Grn */
						g = *data;
						if (g <= slidvals)
							*data++ = 0;
						else
							*data++ = shadowtab[g];

						/* Blau */
						b = *data;
						if (b <= slidvals)
							*data++ = 0;
						else
							*data++ = shadowtab[b];
					}
				} while (--length);
			}
		} else
		{
			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;
			data16 = (uint16_t *) smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width * (unsigned long) height;
			r = g = b = 0;

			do
			{
				if (slidvalh < 255)
				{
					r = ((*data16 & 0xf800) >> 8);
					if (r >= slidvalh)
						r = 255;
					else
						r = highlighttab[r];

					g = ((*data16 & 0x7e0) >> 3);
					if (g >= slidvalh)
						g = 255;
					else
						g = highlighttab[g];

					b = ((*data16 & 0x1f) << 3);
					if (b >= slidvalh)
						b = 255;
					else
						b = highlighttab[b];
				}

				if (slidvals > 0)
				{
					r = ((*data16 & 0xf800) >> 8);
					if (r <= slidvals)
						r = 0;
					else
						r = shadowtab[r];

					g = ((*data16 & 0x7e0) >> 3);
					if (g <= slidvals)
						g = 0;
					else
						g = shadowtab[g];

					b = ((*data16 & 0x1f) << 3);
					if (b <= slidvals)
						b = 0;
					else
						b = shadowtab[b];
				}

				*data16++ = ((r & 0x00f8) << 8) | ((g & 0x00fc) << 3) | (b >> 3);
			} while (--length);
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu", get_timer());
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
