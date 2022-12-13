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
/*							RAW Exporter					*/
/* Version 0.1  --  2.10.98									*/
/*	  1, 8 und 24 Bit										*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"RAW",
	0x0010,
	"Christian Eyrich",
	{ "RAW", "", "", "", "", "", "", "", "", "" },
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
	1, 8, 24, 0, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					RAW Exporter (RAW)				*/
/*	  1, 8 und 24 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t BitsPerPixel;
	unsigned short x, y;
	unsigned short width, height;
	unsigned short i;
	unsigned short pallen;
	unsigned long w, f_len;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
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
		buffer = smurf_struct->smurf_pic->pic_data;
		obuffer = buffer;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel == 1)
			w = (unsigned long) (width + 7) / 8;
		else if (BitsPerPixel == 8)
			w = (unsigned long) width;
		else
			w = (unsigned long) width * 3;

		f_len = w * (unsigned long) height;
		if (BitsPerPixel == 8)
			pallen = 768;
		else
			pallen = 0;

		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(pallen + f_len)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			if (pallen)
			{
				pal = smurf_struct->smurf_pic->palette;

				for (i = 0; i < SM_PALETTE_MAX; i++)
				{
					*ziel++ = *pal++;
					*ziel++ = *pal++;
					*ziel++ = *pal++;
				}
			}

			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel++ = *buffer++;
				} while (++x < width);
			} while (++y < height);

			buffer = obuffer;
			ziel = oziel;

			exp_pic->pic_data = ziel;
			exp_pic->f_len = pallen + f_len;
		}

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		/* exp_pic wird von smurf freigegeben */
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}

	return NULL;
}
