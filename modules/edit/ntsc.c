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
/*						Helligkeits-Modul					*/
/* Version 0.1  --  14.03.97								*/
/*	  16 und 24 Bit											*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "% abdunkeln"
#elif COUNTRY==0
#define TEXT1 "% darken"
#elif COUNTRY==2
#define TEXT1 "% darken"
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

MOD_INFO module_info = {
	"NTSC",
	0x0010,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	TEXT1,
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	0, 100,
	0, 64,
	0, 64,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	30, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	16, 24, 0, 0, 0, 0, 0, 0,
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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*						NTSC						*/
/* Courtesy Frank Neumanns NTSC-Filter fr PPM:		*/
/* This creates an effect similar to what I've once	*/
/* seen in the video clip 'You could be mine' by	*/
/* Guns'n'Roses. In the scene I'm talking about you	*/
/* can see John Connor on his motorbike, looking up	*/
/* from the water trench (?) he's standing in.		*/
/* While the camera pulls back, the image gets		*/
/* 'normal' by brightening up the alternate rows of	*/
/* it. I thought this would be an interesting effect*/
/* to try in MPEG. I did not yet check this out,	*/
/* however. Try for yourself.						*/
/*		16 und 24 Bit (prinzipbedingt)				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t BitsPerPixel;
	uint8_t darken;

	short transtab[256];
	uint16_t *data16;
	unsigned short i;
	unsigned short x, y;
	unsigned short width, height;
	unsigned short val;


	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	case MSTART:
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	/* Einstellformular wurde mit START verlassen - Funktion ausfhren */
	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		darken = (uint8_t) smurf_struct->slide1;
		darken = (uint8_t) (((long) darken * 255) / 100);

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		i = 0;
		do
		{
			transtab[i] = i - darken;
			if (transtab[i] < 0)
				transtab[i] = 0;
			else if (transtab[i] > 255)
				transtab[i] = 255;
		} while (++i < 256);

		if (BitsPerPixel == 24)
		{
			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;
			data = smurf_struct->smurf_pic->pic_data;

			y = 0;
			do
			{
				if ((y % 2) != 0)
				{
					x = 0;
					do
					{
						uint8_t v;
						v = transtab[*data];
						*data++ = v;
						v = transtab[*data];
						*data++ = v;
						v = transtab[*data];
						*data++ = v;
					} while (++x < width);
				} else
				{
					data += width * 3;
				}
			} while (++y < height);
		} else if (BitsPerPixel == 16)
		{
			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;
			data16 = (uint16_t *) smurf_struct->smurf_pic->pic_data;

			y = 0;
			do
			{
				if ((y % 2) != 0)
				{
					x = 0;
					do
					{
						val = *data16;
						*data16 = ((transtab[(val & 0xf800) >> 8] & 0x00f8) << 8);
						*data16 |= ((transtab[(val & 0x7e0) >> 3] & 0x00fc) << 3);
						*data16++ |= (transtab[(val & 0x1f) << 3] >> 3);
					} while (++x < width);
				} else
				{
					data16 += width;
				}
			} while (++y < height);
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
