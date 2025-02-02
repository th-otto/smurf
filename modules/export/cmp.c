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
/*				Atari Public Painter Monochrom				*/
/* Version 0.1  --  16.02.97								*/
/*	  1 Bit, Steuerbyte noch fest							*/
/* Version 0.2  --  17.11.98								*/
/*	  Der Encoder hat viel zu fr�h aufgeh�rt zu schreiben.	*/
/*	  Au�erdem ist die Kompressionsrate jetzt geringf�gig	*/
/*	  besser.												*/
/* Version 0.3  --  18.11.98								*/
/*	  Kommt jetzt auch mit Sourcebildern anderer Gr��en als	*/
/*	  640*400 zurecht.										*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Atari Public Painter",
	0x0030,
	"Christian Eyrich",
	{ "CMP", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	1, 0, 0, 0, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};


static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Atari Public Painter Monochrom (CMP)		*/
/*		1 Bit, RLE									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t SByte;
	uint8_t counter;
	uint8_t pixel;

	unsigned short x;
	unsigned short xx;
	unsigned short y;
	unsigned short dw;
	unsigned short dheight;
	unsigned short sw;
	unsigned short sheight;
	unsigned short minwidth;
	unsigned short minheight;

	unsigned long headsize;
	unsigned long f_len;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_WAITING;
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		smurf_struct->event_par[0] = RGB;

		smurf_struct->module_mode = M_COLSYS;

		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

		buffer = smurf_struct->smurf_pic->pic_data;

		dw = 80;
		dheight = 400;
		sw = (smurf_struct->smurf_pic->pic_width + 7) / 8;
		sheight = smurf_struct->smurf_pic->pic_height;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));

		headsize = 2;

		if ((ziel = (uint8_t *) SMalloc(headsize + 32000L)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			return (exp_pic);
		} else
		{
			oziel = ziel;

			memset(ziel, 0x0, headsize + 32000L);

			f_len = 0;

			SByte = 0x81;
			*ziel++ = SByte;			/* Steuerbyte */
			*ziel++ = 0x00;				/* reserviert */

			strncpy(smurf_struct->smurf_pic->format_name, "Public Painter .CMP", 21);

			if (sw < dw)
				minwidth = sw;
			else
				minwidth = dw;

			if (sheight < dheight)
				minheight = sheight;
			else
				minheight = dheight;

			y = 0;
			do
			{
				x = 0;
				do
				{
					counter = 0;
					pixel = *buffer++;
					x++;

					xx = x;
					while (*buffer == pixel && counter < 0xff && xx < minwidth)
					{
						buffer++;
						xx++;
						counter++;
					}

					if (counter > 1 || pixel == SByte)	/* es lohnt sich, oder das */
					{					/* Steuerbyte mu� einmal */
						*ziel++ = SByte;	/* abgespeichert werden */
						*ziel++ = counter;
						*ziel++ = pixel;
						f_len += 3;
						x = xx;
					} else				/* lohnt sich leider nicht! */
					{
						do
						{
							*ziel++ = pixel;
							f_len++;
						} while (counter--);

						while ((*buffer != *(buffer + 1) || *buffer != *(buffer + 2)) && *buffer != SByte
							   && xx < minwidth)
						{
							*ziel++ = *buffer++;
							xx++;
							f_len++;
						}

						x = xx;
					}
				} while (x < minwidth);

				if (sw < dw)			/* Quellbreite < Zielbreite -> Ziel mit Wei� auff�llen */
				{
					*ziel++ = SByte;
					*ziel++ = dw - sw - 1;
					*ziel++ = 0x0;
					f_len += 3;
				} else					/* Quellbreite >= Zielbreite -> �berz�hlige Quellpixel �bergehen */
				{
					buffer += sw - dw;
				}
			} while (++y < minheight);

			if (sheight < dheight)		/* Quellh�he < Zielh�he -> Ziel mit Wei� auff�llen */
			{
				do
				{
					*ziel++ = SByte;
					*ziel++ = 79;
					*ziel++ = 0x0;
					f_len += 3;
				} while (++y < dheight);
			}

			ziel = oziel;

			smurf_struct->smurf_pic->pic_width = 640;
			smurf_struct->smurf_pic->pic_height = 400;

			f_len += headsize;
			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}								/* Malloc */

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		/* exp_pic wird hier mit Absicht nicht freigegeben */
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}

	return NULL;
}
