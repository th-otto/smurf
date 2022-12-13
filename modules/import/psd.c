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
/*				PSD - Adobe Photoshop 2.5/3.0/4.0			*/
/* Version 0.1  --  15.02.96								*/
/*	  xxx													*/
/* Version 0.2  --  27.05.96								*/
/*	  PSDs mit > 3 Planes werden nun auch korrekt gelesen	*/
/*	  indem die Åbrigen Planes ignoriert werden.			*/
/* Version 0.3  --  20.03.97								*/
/*	  Nach Photoshop-Doku von Mario runderneuert			*/
/*==========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][Versionsnummer ungleich 1! | Bild bitte an uns einschicken][ OK ]"
#elif COUNTRY==0
#define ERROR1 "[1][Version number is not 1! | Please send the picture to us.][ OK ]"
#elif COUNTRY==2
#define ERROR1 "[1][Version number is not 1! | Please send the picture to us.][ OK ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Photoshop 3.0",
	0x0030,
	"Christian Eyrich",
	{ "PSD", "", "", "", "", "", "", "", "", "" },
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
/*      PSD - Photoshop 2.5/3.0/4.0-Format (PSD)	*/
/*		1, 8, 16 und 24 Bit (in 3 Ebenen),			*/
/*		unkomprimiert, RLE							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t v1, v2;
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t *ppal;
	uint8_t n;
	uint8_t p;
	uint8_t comp;
	uint8_t BitsPerPixel;
	uint8_t Channels;
	uint8_t Planes;
	uint8_t ColMode;
	char dummy[3];
	char impmessag[21];
	unsigned short i, x, y, w, width, height, cols;
	unsigned long length;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*(uint32_t *) buffer != 0x38325053L) /* '8BPS' */
		return M_INVALID;

	if (*(uint16_t *) (buffer + 4) != 1)
	{
		form_alert(1, ERROR1);
		return M_PICERR;
	}

	Channels = Planes = *(uint16_t *) (buffer + 0x0c);
	if (Planes > 3)
		Planes = 3;

	/* BitsPerPixel  = BitsPerChannel * Planes */
	BitsPerPixel = (*(uint16_t *) (buffer + 0x016) * Planes);

	height = (unsigned short) *(uint32_t *) (buffer + 0x0e);
	width = (unsigned short) *(uint32_t *) (buffer + 0x12);

	/* 0 = Bitmap, 1 = Grayscale, 2 = indexed, 3 = RGB */
	/* 4 = CMYK, 7 = Multichannel, 8 = Duotone, 9 = Lab */
	ColMode = *(uint16_t *) (buffer + 0x18);

	buffer += 0x1a;
	/* Color Mode Data */
	length = *((uint32_t *) buffer);
	buffer += 4;
	if (ColMode == 2)				/* Palettenbild */
		cols = (unsigned short) (length / 3);
	else if (ColMode == 1 || ColMode == 8)	/* Graustufen oder Duotone */
		cols = 1 << BitsPerPixel;
	else
		cols = 0;
	ppal = buffer + 4;
	buffer += length;				/* Block Åberspringen */

	/* Image Resources Block */
	length = *((uint32_t *) buffer);
	buffer += 4;
	buffer += length;				/* Block Åberspringen */

	/* Layer and Mask Information Block */
	length = *((uint32_t *) buffer);
	buffer += 4;
	buffer += length;				/* Block Åberspringen */

	/* Image Data Block */
	comp = *((uint16_t *) buffer);
	buffer += 2;
	if (comp)
		buffer += (unsigned long) Channels * height * 2;	/* LÑngentabelle Åberspringen */

	strcpy(smurf_struct->smurf_pic->format_name, "Photoshop 2.5/3.0 .PSD");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Photoshop ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if ((ziel = smurf_struct->services->SMalloc(((long) width * (long) height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	oziel = ziel;

	if (BitsPerPixel == 1)
		w = (width + 7) / 8;
	else
		w = width;

	for (p = 0; p < Planes; p++)	/* Planes */
	{
		ziel = oziel + p;

		y = 0;
		do
		{
			x = 0;
			do
			{
				if (!comp)
				{
					*ziel = *buffer++;
					ziel += Planes;
					x++;
				} else
				{
					v1 = *buffer++;
#if 0
					/* -128 kommt lt. Doku nie vor */
					if (v1 != -128)
#endif
					{
						if (v1 > 0x7f)
						{
							n = 0x101 - v1;

							v2 = *buffer++;

							x += n;

							while (n--)
							{
								*ziel = v2;
								ziel += Planes;
							}
						} else
						{
							n = v1 + 1;

							x += n;

							while (n--)
							{
								*ziel = *buffer++;
								ziel += Planes;
							}
						}
					}
				}
			} while (x < w);
			y++;
		} while (y < height);
	}

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	if (BitsPerPixel == 1)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	pal = smurf_struct->smurf_pic->palette;
	switch (ColMode)
	{
		/* Bitmap */
	case 0:
		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
		break;
		/* Grayscale und Duotone */
	case 1:
	case 8:
		for (i = 0; i < cols; i++)
		{
			*pal++ = i;
			*pal++ = i;
			*pal++ = i;
		}
		break;
		/* Indexed */
	case 2:
		for (i = 0; i < cols; i++)
		{
			*pal++ = *ppal;
			*pal++ = *(ppal + cols);
			*pal++ = *(ppal + cols + cols);
			ppal++;
		}
		break;
	default:
		break;
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
