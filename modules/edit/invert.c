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
/*						Invertier-Modul						*/
/* Version 0.1  --  30.12.95								*/
/*	  24 Bit												*/
/* Version 0.2  --  07.03.96								*/
/*	  Palettenbilder										*/
/* Version 0.3  --  16.03.96								*/
/*	  16 Bit												*/
/* Version 0.4  --  18.05.96								*/
/*	  Nun auch Invertierung von sw-Bildern (die haben		*/
/*	  n�mlich keine Palette mu�t Du wissen)					*/
/* Version 0.5  --  18.05.96								*/
/*	  Tja, da hatte ich wohl beim Einbau des tollen			*/
/*	  Features in die 0.4 einen bl�den Fehler gemacht. Es	*/
/*	  wurden keine Palettenbilder mehr invertiert.			*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Invertieren"
#elif COUNTRY==0
#define TEXT1 "Invertion"
#elif COUNTRY==2
#define TEXT1 "Invertion"
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
	0x0050,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-�berschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	COMPILER_ID,
/* Bezeichnung fuer Bilder */
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
/*					Invertierer						*/
/*		1-8, 16 und 24 Bit 							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t BitsPerPixel;
	uint16_t *data16;
	unsigned short width, height;
	unsigned long length;


#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	switch (smurf_struct->module_mode)
	{
	/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		smurf_struct->module_mode = M_STARTED;
		break;
	
	case MEXEC:
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel != 16)
		{
			uint8_t v;
			
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
					v = ~*data;
					*data++ = v;
				}
			} else
			{
				if (BitsPerPixel == 24)
					length = (unsigned long) width * (unsigned long) height;
				else
					length = 256;

				while (length--)
				{
					v = ~*data;
					*data++ = v;
					v = ~*data;
					*data++ = v;
					v = ~*data;
					*data++ = v;
				}
			}
		} else if (BitsPerPixel == 16)
		{
			uint16_t v;
			
			data16 = smurf_struct->smurf_pic->pic_data;

			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;

			length = (unsigned long) width * (unsigned long) height;

			while (length--)
			{
				v = ~*data16;
				*data16++ = v;
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
