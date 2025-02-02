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
/*					Inshape-Format Encoder					*/
/* Version 0.1  --  25.01.96								*/
/*	  Der 2 Minuten Encoder									*/
/* Version 0.2  --  14.05.96								*/
/*	  Graukonvertierung f�r 8 Bit eingebaut					*/
/* Version 0.3  --  08.05.97								*/
/*	  Graukonvertierung wird nun vom Smurf erledigt.		*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

static void *(*SMalloc)(long amount);

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Inshape Image",
	0x0030,
	"Christian Eyrich",
	{ "IIM", "", "", "", "", "", "", "", "", "" },
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

typedef struct
{
	char magic[8];
	uint16_t unknown;
	uint16_t BitsPerPixel;
	uint16_t width;
	uint16_t height;
} HEAD;


static void write_header(HEAD *iim_header, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	strncpy(iim_header->magic, "IS_IMAGE", 8);

	iim_header->BitsPerPixel = BitsPerPixel;

	switch (BitsPerPixel)
	{
	case 1:
		iim_header->unknown = 0;
		break;
	case 8:
		iim_header->unknown = 1;
		break;
	case 24:
		iim_header->unknown = 4;
		break;
	}

	iim_header->width = width;
	iim_header->height = height;
}



/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Inshape-Format (IIM)				*/
/*		1, 8 und 24 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t BitsPerPixel;
	unsigned short y, width, height;
	unsigned long w;
	unsigned long headsize;
	unsigned long f_len;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		if (smurf_struct->smurf_pic->depth == 8)
			smurf_struct->event_par[0] = GREY;
		else
			smurf_struct->event_par[0] = RGB;

		smurf_struct->module_mode = M_COLSYS;

		break;

	case MEXEC:
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		SMalloc = smurf_struct->services->SMalloc;

		headsize = sizeof(HEAD);

		buffer = smurf_struct->smurf_pic->pic_data;
		obuffer = buffer;

		exp_pic = (EXPORT_PIC *) smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel == 1)
			w = (unsigned long) (width + 7) / 8;
		else if (BitsPerPixel == 8)
			w = (unsigned long) width;
		else
			w = (unsigned long) width * 3L;

		f_len = w * (unsigned long) height;

		if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(headsize + f_len)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			write_header((HEAD *) ziel, width, height, BitsPerPixel);

			ziel += headsize;

			y = 0;
			do
			{
				memcpy(ziel, buffer, w);
				ziel += w;
				buffer += w;
			} while (++y < height);

			buffer = obuffer;
			ziel = oziel;

			f_len += headsize;
			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
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


#if 0
	if (BitsPerPixel == 8)
	{
		length = f_len;
		oziel = ziel;
		ziel += headsize;
		pal = smurf_struct->smurf_pic->palette;
		do
		{
			pixval = (*buffer++ * 3);
			*ziel++ = (((long)pal[pixval] * 871L) + ((long)pal[pixval + 1] * 2929L) + ((long)pal[pixval + 2] * 295L)) >> 12;
		} while (--length);
		ziel = oziel;
	} else
	{
		memcpy(ziel + headsize, buffer, f_len);
	}
#endif
