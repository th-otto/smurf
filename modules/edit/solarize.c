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
/*						Solarize-Modul						*/
/* Version 0.1  --  02.10.97								*/
/*	  1-8, 16 und 24 Bit									*/
/* Version 0.2  --  18.08.98								*/
/*	  Da auch énderungen an monochromen Bildern auffallen	*/
/*	  mÅssen, dort aber eine geÑnderte Palette nichts be-	*/
/*	  wirkt, wird jetzt 2 Bit als kleinste Farbtiefe fest-	*/
/*	  gelegt.												*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Solarisierung"
#define TEXT2 "%Belichtung"
#elif COUNTRY==0
#define TEXT1 "Solarisierung"
#define TEXT2 "%Exposure"
#elif COUNTRY==2
#define TEXT1 "Solarisierung"
#define TEXT2 "%Exposure"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

MOD_INFO module_info = {
	TEXT1,
	0x0020,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	TEXT2,
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
	50, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
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
	0,
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Solarize						*/
/* Solarize soll einen "Solarisationseffekt" produ-	*/
/* zieren, den man auch sehen kann, wenn man einen	*/
/* Film wÑhrend der Entwicklung dem Licht aussetzt.	*/
/*		2-8, 16 und 24 Bit 							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL * smurf_struct)
{
	uint8_t *data;
	uint8_t r, g, b;
	uint8_t BitsPerPixel;
	uint16_t *data16;
	uint16_t val16;
	unsigned short width, height;
	unsigned short schwelle;
	unsigned long length;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* Wenn das Modul zum ersten Mal gestartet wurde, */
		/* ein Einstellformular anfordern....             */
		smurf_struct->services->f_module_prefs(&module_info, smurf_struct->module_number);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		/* Einstellformular wurde mit START verlassen - Funktion ausfÅhren */
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		schwelle = (unsigned short) ((100 - smurf_struct->slide1) * 255 / 100);

		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel != 16)
		{
			if (BitsPerPixel == 24)
			{
				data = smurf_struct->smurf_pic->pic_data;

				width = smurf_struct->smurf_pic->pic_width;
				height = smurf_struct->smurf_pic->pic_height;

				length = (unsigned long) width *(unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;

				length = 256;
			}

			while (length--)
			{
				r = *data;
				if (r > schwelle)
					*data++ = ~r;
				else
					data++;

				g = *data;
				if (g > schwelle)
					*data++ = ~g;
				else
					data++;

				b = *data;
				if (b > schwelle)
					*data++ = ~b;
				else
					data++;
			}
		} else
		{
			data16 = smurf_struct->smurf_pic->pic_data;

			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;

			length = (unsigned long) width *(unsigned long) height;

			while (length--)
			{
				val16 = *data16;

				r = (val16 & 0xf800) >> 8;
				if (r > schwelle)
					*data++ = ~r;
				else
					data++;

				g = (val16 & 0x7e0) >> 3;
				if (g > schwelle)
					*data++ = ~g;
				else
					data++;

				b = (val16 & 0x1f) << 3;
				if (b > schwelle)
					*data++ = ~b;
				else
					data++;

				*data16++ = ((r & 0x00f8) << 8) | ((g & 0x00fc) << 3) | (b >> 3);
			}
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
