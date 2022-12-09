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
/*				Truepaint Image-Format Decoder				*/
/* Version 0.1  --  24.02.96								*/
/*	  1 Bit													*/
/* Version 0.2  --  27.02.96								*/
/*	  2, 4 und 8 Bit, obwohl ein Falcon-Malprogramm	werden	*/
/*	  Bilder im TPI-Format als interleaved Bitmaps ge-		*/
/*	  speichert ... Noch falsche Farben bei 4 Bit-Bildern.	*/
/* Version 0.3  --  03.03.96								*/
/*	  16 Bit werden nun auch dekodiert.						*/
/* Version 0.4  --  04.03.96								*/
/*	  16 Bit wurden aus RÅcksicht auf den Smurf als 24 Bit  */
/*	  ausgegeben. Jetzt sind es echte 16 Bit-Bilder.		*/
/* Version 0.5  --  07.03.96								*/
/*	  Kleinen aber erheblichen Fehler in der 16 Bit			*/
/*	  Dekodierung gefixt.									*/
/* Version 0.6  --  11.08.96								*/
/*	  Kleine énderung in der 16 Bit Dekodierung				*/
/* Version 0.7  --  13.06.97								*/
/*	  Die Palette von 2 und 4 Bit Bildern wird nun			*/
/*	  korrigiert und es erscheinen endlich richtige Farben.	*/
/* Version 0.8  --  10.08.98								*/
/*	  öberarbeitung des Palettenhandlings					*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Truepaint Image-Format",
	0x0080,
	"Christian Eyrich",
	{ "TPI", "", "", "", "", "", "", "", "", "" },
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
/*			Truepaint Image-Format (TPI)			*/
/*		1, 2, 4, 8 und 16 Bit, unkomprimiert		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	const uint8_t *plane_table;
	uint8_t p;
	uint8_t v;
	uint8_t Planes;
	uint8_t BitsPerPixel;
	static uint8_t const table2bit[] = { 0, 2, 3, 1 };
	static uint8_t const table3bit[] = { 0, 2, 3, 6, 4, 7, 5, 1 };
	static uint8_t const table4bit[] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };
	char dummy[3];
	char impmessag[21];

	uint16_t *buffer16;
	uint16_t *ziel16;
	uint16_t *ppal;
	unsigned short i, j, x, y, w, memwidth, width, height;
	unsigned short cols;
	unsigned short DatenOffset;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (*(uint32_t *) buffer != 0x504e5400L) /* 'PNT\0' */
		return M_INVALID;

	cols = *(uint16_t *) (buffer + 0x06);

	width = *(uint16_t *) (buffer + 0x08);
	height = *(uint16_t *) (buffer + 0x0a);

	BitsPerPixel = *(uint16_t *) (buffer + 0x0c);
	Planes = BitsPerPixel;

	DatenOffset = 0x80 + *(uint16_t *) (buffer + 0x06) * 6;

	strcpy(smurf_struct->smurf_pic->format_name, "Truepaint Image .TPI");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Truepaint ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel != 16)
	{
		w = (width + 7) / 8;
		memwidth = w * 8;
		v = (width + 15) / 16;
		v = v * 2 - w;
	} else
	{
		w = (width + 15) / 16;
		w = w * 16;
		memwidth = width;
		v = w - width;
	}

	if ((ziel = smurf_struct->services->SMalloc((((unsigned long) memwidth * height * BitsPerPixel) >> 3) + 1)) == 0)
		return M_MEMORY;

	obuffer = buffer;
	oziel = ziel;

	buffer += DatenOffset;

	if (BitsPerPixel != 16)
	{
		ziel16 = (uint16_t *) ziel;

		p = 0;
		do
		{
			buffer16 = (uint16_t *) buffer + p;

			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel16++ = *buffer16;
					buffer16 += Planes;
					x += 2;
				} while (x < w);

				ziel16 = (uint16_t *)((uint8_t *)ziel16 - v);
			} while (++y < height);
		} while (++p < Planes);

		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	} else
	{
		ziel16 = (uint16_t *) ziel;
		buffer16 = (uint16_t *) buffer;

		y = 0;
		do
		{
			x = 0;
			do
			{
				*ziel16++ = *buffer16++;
			} while (++x < width);

			buffer16 += v;
		} while (++y < height);

		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	}

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	if (cols > 0)
	{
		ppal = (uint16_t *) (buffer + 0x80);
		pal = smurf_struct->smurf_pic->palette;

		if (cols == 4)
			plane_table = table2bit;
		else if (cols == 8)
			plane_table = table3bit;
		else
			plane_table = table4bit;

		for (i = 0; i < cols; i++)
		{
			if (cols < 256)
				j = plane_table[i];
			else
				j = i;

			pal[3 * i + 0] = (((unsigned long) ppal[3 * j + 0] * 255L) / 1000L);
			pal[3 * i + 1] = (((unsigned long) ppal[3 * j + 1] * 255L) / 1000L);
			pal[3 * i + 2] = (((unsigned long) ppal[3 * j + 2] * 255L) / 1000L);
		}
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
