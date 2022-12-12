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
/*						Gammakorrektur-Modul				*/
/* Version 0.1  --  04.06.97								*/
/*	  24 Bit und Palettenbilder								*/
/* Version 0.2  --  22.09.97								*/
/*	  jetzt auch in 16 Bit									*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Gammakorrektur"
#elif COUNTRY==0
#define TEXT1 "Gammakorrektur"
#elif COUNTRY==2
#define TEXT1 "Gammakorrektur"
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
	TEXT1,
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	"Gamma R",
	"Gamma G",
	"Gamma B",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	1, 40,
	1, 40,
	1, 40,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	10, 10, 10, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	4, 7, 8, 16, 24, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Gammakorrektur					*/
/*		4-8, 16 und 24 Bit 							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t r, g, b;
	uint8_t BitsPerPixel;

	uint16_t *data16;
	unsigned short width, height, i;
	unsigned short redgammamap[256];
	unsigned short grngammamap[256];
	unsigned short blugammamap[256];
	unsigned long length;
	double gammared, gammagrn, gammablu;
	uint8_t v;

	switch (smurf_struct->module_mode)
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

		gammared = smurf_struct->slide1 / 10.0;
		gammagrn = smurf_struct->slide2 / 10.0;
		gammablu = smurf_struct->slide3 / 10.0;

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		for (i = 0; i < 256; i++)
		{
			redgammamap[i] = 0.5 + 255 * pow(i / 255.0, 1.0 / gammared);
			grngammamap[i] = 0.5 + 255 * pow(i / 255.0, 1.0 / gammagrn);
			blugammamap[i] = 0.5 + 255 * pow(i / 255.0, 1.0 / gammablu);
		}

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width * (unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;

				length = SM_PALETTE_MAX;
			}

			while (length--)
			{
				v = *data;
				*data++ = redgammamap[v];
				v = *data;
				*data++ = grngammamap[v];
				v = *data;
				*data++ = blugammamap[v];
			}
		} else
		{
			data16 = smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width * (unsigned long) height;

			do
			{
				r = redgammamap[(*data16 & 0xf800) >> 8];
				g = grngammamap[(*data16 & 0x7e0) >> 3];
				b = blugammamap[(*data16 & 0x1f) << 3];

				*data16++ = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
			} while (--length);

		}
#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_PICDONE;
		break;

	case MTERM:
	/* Mterm empfangen - Speicher freigeben und beenden */
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
