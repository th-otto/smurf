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
/* Version 0.1  --  30.12.95								*/
/*	  TC- und HC-Bilder										*/
/* Version 0.2  --  13.03.96								*/
/*	  Palettenbilder und TC-Bilder von BGR nach RGB			*/
/* Version 0.3  --  31.03.96								*/
/*	  Das war doch alles Scheiûe. Erstens war der Ansatz	*/
/*	  falsch und zweitens langsam.							*/
/*															*/
/*						Kontrast-Modul						*/
/* Version 0.1  --  31.03.96								*/
/*	  Nur Kontrast und dann auch noch betrogener Spezialfall*/
/* Version 0.2  --  02.04.96								*/
/*	  Auf die allgemeine Geradengleichung umgestellt.		*/
/*															*/
/*				Helligkeits-Kontrast-Modul					*/
/* Version 0.1  --  02.04.96								*/
/*	  Weil es sich so ergeben hat, habe ich die beiden		*/
/*	  Module zusammengelegt.								*/
/* Version 0.2  --  03.05.96								*/
/*	  Die drei FarbkanÑle kînnen nun auch getrennt bearbei-	*/
/*	  tet werden.											*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Helligkeit-Kontrast"
#define TEXT2 "Helligkeit"
#define TEXT3 "Kontrast"
#define TEXT4 "rot"
#define TEXT5 "grÅn"
#define TEXT6 "blau"
#elif COUNTRY==0
#define TEXT1 "Brightness-contrast"
#define TEXT2 "Brightness"
#define TEXT3 "Contrast"
#define TEXT4 "Red"
#define TEXT5 "Green"
#define TEXT6 "Blue"
#elif COUNTRY==2
#define TEXT1 "Brightness-contrast"
#define TEXT2 "Brightness"
#define TEXT3 "Contrast"
#define TEXT4 "Rouge"
#define TEXT5 "Vert"
#define TEXT6 "Bleu"
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
	-255, 255,
	-128, 127,
	0, 64,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 0,
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
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
	0
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Helligkeit-Kontrast				*/
/*		1-8, 16 und 24 Bit							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

void edit_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *data;
	uint8_t BitsPerPixel;
	uint8_t rc, gc, bc;

	short slidvalb;
	short slidvalk;
	unsigned short i;
	short abs;
	short m1;
	short m2;
	short transtab[256];

	uint16_t *data16;
	unsigned short width, height;
	unsigned short t_pos;
	unsigned short val;

	unsigned long length;


	switch (smurf_struct->module_mode)
	{
/* Wenn das Modul zum ersten Mal gestartet wurde, */
/* ein Einstellformular anfordern....             */
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

		slidvalb = (short) smurf_struct->slide1;
		slidvalk = (short) smurf_struct->slide2;
		rc = smurf_struct->check1;
		gc = smurf_struct->check2;
		bc = smurf_struct->check3;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (rc != 1 && gc != 1 && bc != 1)
		{
			smurf_struct->module_mode = M_WAITING;
			return;
		}

		if (slidvalb != 0 || slidvalk != 0)
		{
			i = 0;
			t_pos = 0;
			if (slidvalk >= 0)
			{
				m2 = 256 - (2 * slidvalk);
				abs = slidvalk * 256 / m2;
				do
				{
					transtab[t_pos] = i * 256 / m2 - abs + slidvalb;

					if (transtab[t_pos] < 0)
						transtab[t_pos] = 0;
					else if (transtab[t_pos] > 255)
						transtab[t_pos] = 255;
					t_pos++;
				} while (++i < 256);
			} else if (slidvalk < 0)
			{
				m1 = 128 + slidvalk;
				do
				{
					transtab[t_pos] = i * m1 / 128 - slidvalk + slidvalb;

					if (transtab[t_pos] < 0)
						transtab[t_pos] = 0;
					else if (transtab[t_pos] > 255)
						transtab[t_pos] = 255;
					t_pos++;
				} while (++i < 256);
			}
		} else
		{
			smurf_struct->module_mode = M_WAITING;
			return;
		}

		if (BitsPerPixel != 16)
		{
			uint8_t v;

			if (BitsPerPixel == 24)
			{
				width = smurf_struct->smurf_pic->pic_width;
				height = smurf_struct->smurf_pic->pic_height;
				data = smurf_struct->smurf_pic->pic_data;

				length = (unsigned long) width * (unsigned long) height;
			} else
			{
				data = smurf_struct->smurf_pic->palette;
				length = SM_PALETTE_MAX;
			}

			if (rc == 1 && gc == 1 && bc == 1)
			{
				do
				{
					v = transtab[*data];
					*data++ = v;
					v = transtab[*data];
					*data++ = v;
					v = transtab[*data];
					*data++ = v;
				} while (--length);
			} else
			{
				do
				{
					if (rc == 1)
					{
						v = transtab[*data];
						*data++ = v;
					} else
					{
						data++;
					}
					if (gc == 1)
					{
						v = transtab[*data];
						*data++ = v;
					} else
					{
						data++;
					}
					if (bc == 1)
					{
						v = transtab[*data];
						*data++ = v;
					} else
					{
						data++;
					}
				} while (--length);
			}
		} else
		{
			width = smurf_struct->smurf_pic->pic_width;
			height = smurf_struct->smurf_pic->pic_height;
			data16 = smurf_struct->smurf_pic->pic_data;

			length = (unsigned long) width * (unsigned long) height;

			if (rc == 1 && gc == 1 && bc == 1)
			{
				do
				{
					val = *data16;
					*data16 = ((transtab[(val & 0xf800) >> 8] & 0x00f8) << 8);
					*data16 |= ((transtab[(val & 0x7e0) >> 3] & 0x00fc) << 3);
					*data16++ |= (transtab[(val & 0x1f) << 3] >> 3);
				} while (--length);
			} else
			{
				do
				{
					if (rc == 1)
						*data16 &= ((transtab[(*data16 & 0xf800) >> 8] & 0x00f8) << 8);
					if (gc == 1)
						*data16 &= ((transtab[(*data16 & 0x7e0) >> 3] & 0x00fc) << 3);
					if (bc == 1)
						*data16 &= (transtab[(*data16 & 0x1f) << 3] >> 3);
					data16++;
				} while (--length);
			}
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
