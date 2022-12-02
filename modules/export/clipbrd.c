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
/*						GEM-Image Encoder					*/
/*				Spezialversion fÅr Clipboard				*/
/* Version 0.1  --  07.02.97								*/
/*	  1-8 und 24 Bit										*/
/*	  Nur BitString zur schnellen Abarbeitung.				*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define IMG		0
#define XIMG	1

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Clipboard",
	0x0010,
	"Christian Eyrich",
	{ "IMG", "", "", "", "", "", "", "", "", "" },
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	1, 2, 3, 4, 6, 7, 8, 24,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	0
};

static void *(*SMalloc)(long amount);


static unsigned long write_RGB(uint8_t *buffer, uint8_t *ziel, unsigned long w, uint8_t pad, unsigned short height)
{
	unsigned short k, x, y;
	unsigned long run, complen;

	complen = 0;

	y = 0;
	do
	{
		x = 0;
		do
		{
			/* BitString-Head schreiben */
			*ziel++ = 0x80;
			if ((run = w - x) > 0xff)
				run = 0xff;

			*ziel++ = run;
			complen += run + 2;

			k = 0;
			do
			{
				*ziel++ = *buffer++;
			} while (++k < run);

			x += k;
		} while (x < w);

		/* noch pad Bytes als SOLID0 dazu */
		if (pad)
		{
			*ziel++ = pad;
			complen++;
		}
	} while (++y < height);

	return complen;
}


static unsigned long write_Plane(uint8_t *buffer, uint8_t *ziel, unsigned long w, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *obuffer;
	uint8_t Planes;
	uint8_t p;
	unsigned short k, x, y;
	unsigned long pla;
	unsigned long plh;
	unsigned long plo;
	unsigned long run;
	unsigned long complen;

	Planes = BitsPerPixel;

	plh = (unsigned long) height * w;	/* Hîhe einer Plane in Byte */

	complen = 0;

	obuffer = buffer;

	y = 0;
	do
	{
		plo = y * w;					/* Offset vom Planeanfang in Bytes */

		p = 0;
		do
		{
			pla = plh * p;				/* Abstand dieser Plane vom Bildanfang */
			buffer = obuffer + pla + plo;	/* Quelladresse der zu kodierenden Scanline */

			x = 0;
			do
			{
				/* BitString-Head schreiben */
				*ziel++ = 0x80;
				if ((run = w - x) > 0xff)
					run = 0xff;

				*ziel++ = run;
				complen += run + 2;

				k = 0;
				do
				{
					*ziel++ = *buffer++;
				} while (++k < run);

				x += k;
			} while (x < w);
		} while (++p < Planes);
	} while (++y < height);

	return complen;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			GEM-(X)IMG Komprimierer (IMG)			*/
/*			Spezialversion fÅr Clipboard			*/
/*		1-8 , 24 Bit, eigene Komprimierungen		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t pad;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	uint8_t PattLength;

	uint16_t *ppal;
	unsigned short k;
	unsigned short width, height;
	unsigned short strings;
	unsigned short Header;
	unsigned short img_headsize;
	unsigned short ximg_headsize;
	unsigned short Palette;
	unsigned short cols;
	unsigned long w, memwidth;
	unsigned long f_len;

	typedef struct
	{
		uint16_t version;
		uint16_t headsize;
		uint16_t BitsPerPixel;
		uint16_t patlen;
		uint16_t pixwidth;
		uint16_t pixheight;
		uint16_t width;
		uint16_t height;
	} IMG_HEAD;

	typedef enum
	{
		RGB_,
		CMY_,
		HLS_,
		PANTONE_
	} XIMG_COLMODEL;

	typedef struct
	{
		unsigned long imgtype;
		XIMG_COLMODEL colmodel;
	} XIMG_HEAD;

	IMG_HEAD *img_header;
	XIMG_HEAD *ximg_header;


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
		SMalloc = smurf_struct->services->SMalloc;

		img_headsize = (unsigned short) sizeof(IMG_HEAD);

		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));

		/* allgemeine Variablen fÅllen */
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		BitsPerPixel = smurf_struct->smurf_pic->depth;
		if (BitsPerPixel == 24)
		{
			Planes = 1;
			PattLength = 3;
		} else
		{
			Planes = BitsPerPixel;
			PattLength = 1;
		}

		Header = img_headsize;
		if (BitsPerPixel != 1)
		{
			if (BitsPerPixel == 24)
				Palette = 0;
			else
				Palette = (1 << BitsPerPixel) * 6;
			ximg_headsize = (unsigned short) sizeof(XIMG_HEAD);
			Header += (unsigned short) (ximg_headsize + Palette);
		} else
		{
			ximg_headsize = 0;
			Palette = 0;
		}

		if (BitsPerPixel == 24)
		{
			w = (unsigned long) width * 3;

			memwidth = (unsigned long) width;
			pad = (((width + 15) / 16) * 16 - width) * 3;
		} else
		{
			w = (width + 7) / 8;		/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
			memwidth = w * 8;
			pad = 0;
		}

		strings = (unsigned short) (w / 255 + 1);

		/* Daten + Bit Strings im worst case + 1 Padbyte gepackt als Solid Run */
		f_len = ((memwidth * (long) height * BitsPerPixel) >> 3) + ((unsigned long) strings * 2L + 1) * (long) height * (long) Planes;

		if ((ziel = (uint8_t *) SMalloc(Header + f_len)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		} else
		{
			img_header = (IMG_HEAD *) ziel;

			/* Header fÅllen */
			img_header->version = 1;
			img_header->headsize = Header >> 1;
			img_header->BitsPerPixel = BitsPerPixel;
			img_header->patlen = PattLength;
			img_header->pixwidth = 0x174;
			img_header->pixheight = 0x174;
			img_header->width = width;
			img_header->height = height;

			/* erweiterten Header fÅllen */
			if (ximg_headsize != 0)
			{
				ximg_header = (XIMG_HEAD *) (ziel + img_headsize);
				ximg_header->imgtype = 0x58494d47L; /* 'XIMG' */
				ximg_header->colmodel = RGB_;
			}

			strcpy(smurf_struct->smurf_pic->format_name, "GEM-(X)IMG .IMG");

			/* Farbpalette fÅllen */
			if (Palette != 0)
			{
				ppal = (uint16_t *) (ziel + img_headsize + ximg_headsize);

				pal = smurf_struct->smurf_pic->palette;
				cols = 1 << BitsPerPixel;
				for (k = 0; k < cols; k++)
				{
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
					*ppal++ = (uint16_t) (((unsigned long) *pal++ * 1000L) / 255);
				}
			}

			f_len = Header;

			if (BitsPerPixel == 24)
				f_len += write_RGB(buffer, ziel + Header, w, pad, height);
			else
				f_len += write_Plane(buffer, ziel + Header, w, height, BitsPerPixel);

			_Mshrink(ziel, f_len);
			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}

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
