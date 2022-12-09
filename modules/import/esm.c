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
/*				Enhanced Simplex-Format Decoder				*/
/* Version 0.1  --  15.10.97								*/
/*	  1, 8 und 24 Bit										*/
/*	  Ein Modul aus der Reihe der 2 Minuten Decoder			*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"ESM Bitmap Modul",
	0x0050,
	"Dale Russell",
	{ "ESM", "", "", "", "", "", "", "", "", "" },
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


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Enhanced Simplex (ESM)			*/
/*	  1, 8 und 24 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t *ppal;
	unsigned short BitsPerPixel;
	char dummy[4];
	char impmessag[22];

	unsigned short width, height;
	unsigned short i;
	unsigned long DataOffset;

	unsigned long len;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (strncmp((char *)buffer, "TMS", 3) != 0)
	{
		return M_INVALID;
	} else
	{
		DataOffset = *(uint16_t *) (buffer + 0x04);

		width = *(uint16_t *) (buffer + 0x06);
		height = *(uint16_t *) (buffer + 0x08);

		BitsPerPixel = *(uint16_t *) (buffer + 0x0a);

		strcpy(smurf_struct->smurf_pic->format_name, "Enhanced Simplex");

		strcpy(impmessag, "ESM ");
		strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
		strcat(impmessag, " Bit");
		smurf_struct->services->reset_busybox(128, impmessag);

		pal = smurf_struct->smurf_pic->palette;

		if (BitsPerPixel == 8)
		{
			if (DataOffset > 768)		/* Platz fÅr Palette? */
			{
				ppal = buffer + 0x24;

				for (i = 0; i < 256; i++)
				{
					*pal++ = *ppal;
					*pal++ = *(ppal + 256);
					*pal++ = *(ppal++ + 512);
				}
			} else
			{
				for (i = 0; i < 256; i++)
				{
					*pal++ = i;
					*pal++ = i;
					*pal++ = i;
				}
			}
			smurf_struct->smurf_pic->bp_pal = 24;
		} else
		{
			pal[0] = 255;
			pal[1] = 255;
			pal[2] = 255;
			pal[3] = 0;
			pal[4] = 0;
			pal[5] = 0;
			smurf_struct->smurf_pic->bp_pal = 0;
		}

		ziel = buffer;

		switch (BitsPerPixel)
		{
		case 1:
			len = (unsigned long) ((width + 7) >> 3) * (unsigned long) height;
			break;

		case 8:
			len = (unsigned long) width * (unsigned long) height;
			break;

		case 24:
			len = (unsigned long) width * (unsigned long) height * 3L;
			break;

		default:
			return M_UNKNOWN_TYPE;
		}

		if (len + DataOffset > (unsigned long)smurf_struct->smurf_pic->file_len)
			return M_INVALID;
		memmove(ziel, buffer + DataOffset, len);

		_Mshrink(ziel, len);

		smurf_struct->smurf_pic->pic_data = ziel;

		smurf_struct->smurf_pic->pic_width = width;
		smurf_struct->smurf_pic->pic_height = height;
		smurf_struct->smurf_pic->depth = BitsPerPixel;

		if (BitsPerPixel == 1)
			smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		else
			smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

		if (BitsPerPixel == 8 && DataOffset < 768)	/* keine Palette -> Graustufenbild */
			smurf_struct->smurf_pic->col_format = GREY;
		else
			smurf_struct->smurf_pic->col_format = RGB;
	}									/* Erkennung */

	return M_PICDONE;
}
