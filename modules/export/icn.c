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
/*						GEM Icon Format						*/
/* Version 0.1  --  02.07.98								*/
/*	  monochrome Icons										*/
/* Version 0.2  --  04.07.98								*/
/*	  2, 4 und 8 Bit										*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

static void *(*SMalloc)(long amount);

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"C-Source",						/* Name of module */
	0x0020,							/* version */
	"Christian Eyrich",				/* author */
	/* 10 Extensionen fr Importer */
	{ "ICN", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* 4 checkbox titles */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* 4 edit object titles */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",
	/* min/max values for slider */
	0,128, 0,128, 0,128, 0,128,
	/* min/max values for edit objects */
	0,10, 0,10, 0,10, 0,10,
	/* default values for slider */
	0, 0, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for edit objects */
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	1, 2, 4, 8, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				GEM Icon-Format (ICN)				*/
/*		1, 2, 4, 8 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *obuffer;
	char *ziel;
	char *oziel;
	char *fname;
	static char const hexTable[16] = "0123456789ABCDEF";
	uint8_t BitsPerPixel;
	uint8_t two;
	uint8_t p;
	uint8_t Planes;
	uint16_t x, y, width, height;
	unsigned long w, headlen, f_len;

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

		SMalloc = smurf_struct->services->SMalloc;

		buffer = smurf_struct->smurf_pic->pic_data;
		obuffer = buffer;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;
		Planes = BitsPerPixel;

#if 0
		   /* GEM Icon Definition: */
#define xxxxxxxx_W 0x0020
#define xxxxxxxx_H 0x0020
#define DATASIZE 0x0040
		UWORD xxxxxxxx[DATASIZE] = {
		};
#endif
		headlen = 144;

		w = (unsigned long) (width + 15) / 16;

		/* Breite (in 16tel Pixel) * H”he * Stringl„nge pro 16 Pixel * Planes + Planes mal CRLF */
		f_len = w * (unsigned long) height * sizeof(" 0x0000,") * Planes + Planes * 2;

		if ((ziel = (char *) SMalloc(headlen + f_len)) == 0)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		} else
		{
			oziel = ziel;

			fname = strrchr(smurf_struct->smurf_pic->filename, '\\');
			if (fname != NULL)
				fname++;
			else
				fname = smurf_struct->smurf_pic->filename;

			strtok(fname, ".");

			/* Header schreiben */
			sprintf(ziel,
					"/* GEM Icon Definition: */\r\n"
					"#define ICON_W 0x%04lx\r\n"
					"#define ICON_H 0x%04lx\r\n"
					"#define DATASIZE 0x%04lx\r\n"
					"UWORD %s[DATASIZE] =\r\n{",
					(long) width, (long) height, w * (long) height * Planes, fname);

			ziel += strlen(ziel);

			w = (unsigned long) (width + 7) / 8;

			p = 0;
			do
			{
				*ziel++ = '\r';
				*ziel++ = '\n';

				y = 0;
				do
				{
					x = 0;
					do
					{
						*ziel++ = '0';
						*ziel++ = 'x';

						two = 0;
						do
						{
							*ziel++ = hexTable[*buffer >> 4];
							*ziel++ = hexTable[(*buffer++) & 0x0f];
							x++;
						} while (++two < 2 && x < w);

						if (two == 2)
						{
							*ziel++ = ',';
							*ziel++ = ' ';
						}
					} while (x < w);
					if (two == 1)
					{
						*ziel++ = '0';
						*ziel++ = '0';
						*ziel++ = ',';
						*ziel++ = ' ';
					}
				} while (++y < height);
			} while (++p < Planes);

			ziel -= 2;
			*ziel++ = '\r';
			*ziel++ = '\n';
			*ziel++ = '}';
			*ziel++ = ';';
			*ziel++ = '\r';
			*ziel++ = '\n';

			f_len = ziel - oziel;

			buffer = obuffer;
			ziel = oziel;

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
