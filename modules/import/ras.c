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

/* Probleme mit komprimierten 32 Bit Bildern */

/* =========================================================*/
/*					Sun Rasterfile-Format					*/
/* Version 0.1  --  11.01.96								*/
/*	  1 und 8 Bit verifiziert, k�nnte sich bei 4 Bit		*/
/*	  seltsam verhalten. Ich brauche da noch Testbilder. 	*/
/* Version 0.2  --  07.02.96								*/
/*	  Klaus hatte noch RLE-komprimierte RAS vergessen.		*/
/*	  Type 2 RAS sollten nun auch richtig dekodiert werden.	*/
/*	  Der Versatz bei 8 Bit Bildern ist immernoch drinnen -	*/
/*	  Testbilder fehlen mir leider immer noch.				*/
/*	  Laut der Enzyklop�die gibt es keine 4 Bit RAS!		*/
/* Version 0.3  --  01.03.96								*/
/*	  Der Versatz bei 8 Bit Bildern ist drau�en				*/
/* Version 0.4  --  26.02.97								*/
/*	  Von Arrays auf Zeiger umgestellt						*/
/* Version 0.5  --  02.03.97								*/
/*	  Die Smurf �bergebene Breite wird nun auf Vielfaches	*/
/*	  von 2 korrigiert. Die Bilddaten k�nnen leider nicht	*/
/*	  ohne gro�en Aufwand berichtigt werden, da Runs nicht	*/
/*	  an Zeilengrenzen aufh�ren.							*/
/* Version 0.6  --  26.07.98								*/
/*	  Sicherheitsabfrage gegen unkomprimierte Files mit		*/
/*	  falscher Filel�nge korrigiert und Sicherheitsabfrage	*/
/*	  gegen zu lange komprimierte Files eingebaut.			*/
/* Version 0.7  --  xx.xx.98								*/
/*	  4 Bit von Holtorfs Pixview gehen nun					*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Sun Rasterfile",
	0x0060,
	"Christian Eyrich",
	{ "RAS", "SUN", "", "", "", "", "", "", "", "" },
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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*          Sun Rasterfile-Format (RAS)             */
/*		1, 8 und 24, 32 Bit, unkomprimiert, RLE		*/
/*		Vom Holtorf gibt es noch 4 Bit dazu ...		*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t *ppal;
	uint8_t BitsPerPixel;
	uint8_t imgtype;
	uint8_t v1, v2;
	uint8_t colmaptype;
	char dummy[3];
	char impmessag[21];

	unsigned short x, y, i, n;
	unsigned short DatenOffset;
	unsigned short colmaplength;
	unsigned short width, height;

	unsigned long w, length, written, flen;


	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*(uint32_t *) buffer != 0x59a66a95L)
		return M_INVALID;

	width = (unsigned int) *(unsigned long *) (buffer + 0x04);
	height = (unsigned int) *(unsigned long *) (buffer + 0x08);

	BitsPerPixel = *(unsigned long *) (buffer + 0x0c);

	imgtype = (char) *(unsigned long *) (buffer + 0x14);
	if (imgtype == 0xff)
		return M_UNKNOWN_TYPE;

	/* Scanline auf ein Vielfaches von 16 Bit alignen wenn komprimiert! */
	if (imgtype == 0x02)
		width = (width + 1) / 2 * 2;

	colmaptype = *(uint32_t *) (buffer + 0x18);

	colmaplength = *(uint32_t *) (buffer + 0x1c);

	DatenOffset = colmaplength + 0x20;

	strcpy(smurf_struct->smurf_pic->format_name, "Sun-Rasterfile .RAS, .SUN");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Sun Raster ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 1)
		w = (width + 7) / 8;
	else
		w = ((unsigned long) width * BitsPerPixel) >> 3;

	if ((ziel = smurf_struct->services->SMalloc(w * height)) == 0)
		return M_MEMORY;

	oziel = ziel;

	buffer += DatenOffset;

	flen = smurf_struct->smurf_pic->file_len - DatenOffset;

	switch (imgtype)
	{
	case 2:
		length = w * (long) height;
		written = 0;
		while (flen && written < length)	/* Byte-encoded */
		{
			if ((v1 = *buffer++) != 0x80)
			{
				*ziel++ = v1;
				written++;
				flen--;
			} else if ((n = *buffer++) == 0x00)
			{
				*ziel++ = 0x80;
				written++;
				flen -= 2;
			} else
			{
				v2 = *buffer++;
				n++;
				written += n;
				while (n--)
					*ziel++ = v2;
				flen -= 3;
			}
		}
		break;

	default:					/* Old, Standard, RGB, TIFF, IFF */
		if (BitsPerPixel == 32)
		{
			/* Alphachannel rausoperieren */
			y = 0;
			do
			{
				x = 0;
				do
				{
					buffer++;
					*ziel++ = *buffer++;
					*ziel++ = *buffer++;
					*ziel++ = *buffer++;
				} while (++x < width);
			} while (++y < height);
		} else
		{
			y = 0;
			do
			{
				memcpy(ziel, buffer, w);
				buffer += w;
				ziel += w;
			} while (++y < height);
		}
		break;
	}							/* imagetype */

	buffer = obuffer;
	ziel = oziel;


	if (BitsPerPixel <= 8)
	{
		pal = smurf_struct->smurf_pic->palette;

		switch (colmaptype)
		{
			/* siehe auch xvsunras */
		case 0:
			if (BitsPerPixel == 1)
			{
				pal[0] = 0;
				pal[1] = 0;
				pal[2] = 0;
				pal[3] = 255;
				pal[4] = 255;
				pal[5] = 255;
			} else
			{
				for (i = 0; i < 256; i++)
				{
					*pal++ = (char) i;
					*pal++ = (char) i;
					*pal++ = (char) i;
				}
			}
			break;
		case 1:
			ppal = buffer + 0x20;
			colmaplength /= 3;
			for (i = 0; i < colmaplength; i++)
			{
				*pal++ = *ppal;
				*pal++ = *(ppal + colmaplength);
				*pal++ = *(ppal + colmaplength + colmaplength);
				ppal++;
			}
			break;
		case 2:
			ppal = buffer + 0x20;
			colmaplength /= 3;
			for (i = 0; i < colmaplength; i++)
			{
				*pal++ = *ppal++;
				*pal++ = *ppal++;
				*pal++ = *ppal++;
			}
			break;
		}						/* Palette */
	}

	if (BitsPerPixel == 1)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	if (imgtype != 3 && BitsPerPixel >= 24)
		smurf_struct->smurf_pic->col_format = BGR;
	else
		smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
