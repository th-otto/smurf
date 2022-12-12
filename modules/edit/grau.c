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
/*                      Graustufen-Modul                    */
/* Version 0.1  --  12.03.96                                */
/*    TC- und Palettenbilder                                */
/* Version 0.2  --  12.03.96                                */
/*	  Flieûkomma rausgeschmiûen -> Faktor 60 schneller		*/
/* Version 0.3  --  23.03.96                                */
/*	  HC-Bilder												*/
/* Version 0.4  --  24.03.96                                */
/*	  HC-Modus hatte nicht so (Ñh, gar nicht) funktioniert	*/
/*	  VerknÅpfung des Graustufenbildes in wÑhlbarer StÑrke	*/
/*	  mit Ursprungsbild. Sieht gut aus, jedoch werde ich	*/
/*	  jetzt wohl ein Preview brauchen. :)					*/
/* Version 0.5  --  03.05.96                                */
/*	  Variable Anzahl (2 bis 32 und 256) Graustufen; dabei  */
/*	  fiel nochmal eine Beschleunigung ab					*/
/* Version 0.6  --  21.05.96                                */
/*	  ein kleiner Fehler bei der Schleifenendebedingung		*/
/*	  war noch drinnen. Sah wie ein öberlauf aus.			*/
/* Version 0.7  --  01.11.96                                */
/*	  Es wurde eine Graustufe zu wenig erzeugt				*/
/* Version 0.8  --  16.02.97                                */
/*	  RGB-Faktoren von 0,299; 0,587; 0,114 auf				*/
/*	  0,212671; 0,715160; 0,072169 umgestellt				*/
/*	  (s. Colorspace-FAQ)									*/
/*	  MOD_INFO wurde falsch bestÅckt						*/
/* Version 0.9  --  21.09.97                                */
/*	  IntensitÑtsstufen von 2-64 auf benutzerfreundliche	*/
/*	  1-100 umgestellt.										*/
/* Version 1.0  --  31.01.98                                */
/*	  Bei 8 Bit Bildern wird jetzt eine lineare Palette		*/
/*	  genommen.												*/
/* Version 1.1  --  05.05.98                                */
/*	  Bei Erzeugung einer linearen Graustufenpalette muû	*/
/*	  auch das Bild verÑndert werden. Das ging auch - bei	*/
/*	  8 Bit Pixelpacked, fÅr Standardformat gab es aber		*/
/*	  keine Routine ...										*/
/* Version 1.2  --  18.08.98                                */
/*	  Stufenbegrenzung des Editfeldes waren falsch (0-10).	*/
/*	  Bei 256 Stufen im Editfeld wird jetzt auch auf Smooth	*/
/*	  geschalten.											*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Graustufen"
#define TEXT2 "IntensitÑt"
#define TEXT3 "256 Stufen"
#define TEXT4 "Stufen"
#elif COUNTRY==0
#define TEXT1 "Graustufen"
#define TEXT2 "IntensitÑt"
#define TEXT3 "256 Stufen"
#define TEXT4 "Stufen"
#elif COUNTRY==2
#define TEXT1 "Graustufen"
#define TEXT2 "IntensitÑt"
#define TEXT3 "256 Stufen"
#define TEXT4 "Stufen"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0


MOD_INFO module_info = {
	TEXT1,
	0x0120,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	{ TEXT2, "", "", "" },
	{ TEXT3, "", "", "" },
	{ TEXT4, "", "", "" },
	{ { 1, 100 }, { 0, 64 }, { 0, 64 }, { 0, 64 } },
	{ { 2, 256 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	{ 100, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 2, 0, 0, 0 },
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 2, 4, 7, 8, 16, 24, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*                  Graustufenwandler               */
/*		1-8, 16 und 24 Bit							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t *pal;
	uint8_t *palette;
	uint8_t *line;
	uint8_t *pixbuf;
	uint8_t BitsPerPixel;
	uint8_t val;
	uint8_t r, g, b;
	uint8_t grenze;
	uint8_t grenzen[256];
	uint8_t stepwidth;
	uint8_t smooth;
	unsigned short i, j, l;

	uint16_t *data16;
	unsigned short x;
	unsigned short y;
	unsigned short width;
	unsigned short height;
	unsigned short intens;
	unsigned short k;
	unsigned short stufen;

	unsigned long length;
	unsigned long planelength;


	switch (smurf_struct->module_mode)
	{
/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

/* Einstellformular wurde mit START verlassen - Funktion ausfÅhren */
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		if (smurf_struct->smurf_pic->col_format == GREY)
		{
			smurf_struct->module_mode = M_WAITING;
			return;
		}

		intens = ((short) smurf_struct->slide[0] * 64) / 100;
		stufen = (unsigned short) smurf_struct->edit[0];
		if (smurf_struct->check[0] || stufen == 256)
			smooth = 1;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		if (smooth == 1)
		{
			k = 0;
			do
			{
				grenzen[k] = k;
			} while (++k < 256);
		} else
		{
			stepwidth = 256 / (stufen - 1);
			grenze = 256 / stufen;
			stufen--;

			k = 0;
			l = 0;
			i = 0;
			do
			{
				j = 0;
				do
				{
					grenzen[k++] = l;
				} while (++j < grenze);

				l += stepwidth;
			} while (++i < stufen);
			do
			{
				grenzen[k] = 255;
			} while (++k < 256);
		}

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24 || (BitsPerPixel == 8 && intens == 64))
			{
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width *(unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;

				length = SM_PALETTE_MAX;
			}

			if (intens == 64)
			{
				if (BitsPerPixel == 8)
				{
					palette = smurf_struct->smurf_pic->palette;

					if (smurf_struct->smurf_pic->format_type == FORM_PIXELPAK)
					{
						do
						{
							val = *data;
							pal = palette + val * 3;
							*data++ = grenzen[(((long) pal[0] * 872L) + ((long) pal[1] * 2930L) + ((long) pal[2] * 296L)) >> 12];
						} while (--length);
					} else
					{
						planelength = (long) (width + 7) / 8 * (long) height;

						pixbuf = (uint8_t *) Malloc(width + 7);

						y = 0;
						do
						{
							memset(pixbuf, 0x0, width);
							getpix_std_line(data, pixbuf, BitsPerPixel, planelength, width);
							line = pixbuf;

							x = 0;
							do
							{
								val = *line;
								pal = palette + val * 3;
								*line++ = grenzen[(((long) pal[0] * 872L) + ((long) pal[1] * 2930L) + ((long) pal[2] * 296L)) >> 12];
							} while (++x < width);

							data += setpix_std_line(pixbuf, data, BitsPerPixel, planelength, width);
						} while (++y < height);

						Mfree(pixbuf);
					}

					/* lineare Palette erzeugen */
					pal = smurf_struct->smurf_pic->palette;
					for (k = 0; k < SM_PALETTE_MAX; k++)
					{
						*pal++ = k;
						*pal++ = k;
						*pal++ = k;
					}
				} else
				{
					do
					{
						val = grenzen[(((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12];

						*data++ = val;
						*data++ = val;
						*data++ = val;
					} while (--length);
				}
			} else
			{
				do
				{
					val = grenzen[(((long) *data * 872L) + ((long) *(data + 1) * 2930L) + ((long) *(data + 2) * 296L)) >> 12];

					data[0] = ((val * intens + data[0] * (64 - intens)) >> 6);
					data[1] = ((val * intens + data[1] * (64 - intens)) >> 6);
					data[2] = ((val * intens + data[2] * (64 - intens)) >> 6);
					data += 3;
				} while (--length);
			}
		} else
		{
			data16 = (uint16_t *) smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width * (unsigned long) height;

			if (intens == 64)
			{
				do
				{
					val = grenzen[(((long) ((*data16 & 0xf800) >> 8) * 872L)
								   + ((long) ((*data16 & 0x7e0) >> 3) * 2930L)
								   + ((long) ((*data16 & 0x1f) << 3) * 296L)) >> 12];

					*data16++ = ((val & 0x00f8) << 8) | ((val & 0x00fc) << 3) | (val >> 3);
				} while (--length);
			} else
			{
				do
				{
					val = grenzen[(((long) (r = (*data16 & 0xf800) >> 8) * 872L)
								   + ((long) (g = (*data16 & 0x7e0) >> 3) * 2930L)
								   + ((long) (b = (*data16 & 0x1f) << 3) * 296L)) >> 12];

					*data16 = (((val * intens + r * (64 - intens)) >> 6) & 0x00f8) << 8;
					*data16 |= ((((val * intens + g * (64 - intens)) >> 6) & 0x00fc) << 3);
					*data16++ |= ((((val * intens + b * (64 - intens)) >> 6)) >> 3);
				} while (--length);
			}
		}


		if (intens == 64)
			smurf_struct->smurf_pic->col_format = GREY;

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
