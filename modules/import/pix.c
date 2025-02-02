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
/*				Degas (Elite)-Image Decoder					*/
/* Version 0.1  --  12.01.96								*/
/*	  Degas Importer von Dale umgeschrieben, Ausgabe in		*/
/*	  Planes, geringerer Speicherverbrauch					*/
/* Version 0.2  --  11.02.96								*/
/*	  nur PCx geht											*/
/* Version 0.3  --  26.02.96								*/
/*	  PIx geht nun auch, vielleicht gibt es bei manchen		*/
/*	  Bildern aber noch Farbprobleme						*/
/* Version 0.4  --  10.03.97								*/
/*	  Von Arrays auf Zeiger umgestellt.						*/
/*	  Erkennung war totale Schei�e. Die Palette wurde auch	*/
/*	  zu wenig geshiftet, jetzt l�uft's mit allen meinen	*/
/*	  Bildern. 												*/
/* Version 0.5  --  xx.xx.96								*/
/*	  PI4 - PI6 und PC4 bis PC6 eingef�hrt					*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Degas-Importer",
	0x0040,
	"Christian Eyrich, Dale Russell",
	{ "PI1", "PI2", "PI3", "PC1", "PC2", "PC3", "", "", "", "" },
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



static char *fileext(const char *filename)
{
	char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			Degas-(Elite) Dekomprimierer			*/
/*		1, 2, 4 Bit, keine und RLE					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	char *extender;
	uint8_t p;
	uint8_t comp;
	uint8_t Planes;
	uint8_t BitsPerPixel;
	char dummy[3];
	char impmessag[21];

	unsigned short *ppal;
	unsigned short i, n, x, y, w, v1, v2;
	unsigned short width, height;
	unsigned short DatenOffset;

	unsigned long pla, plh, plo;


	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	extender = fileext(smurf_struct->smurf_pic->filename);

	if (((*buffer != 0x00 && *buffer != 0x80) || *(buffer + 1) > 2) ||
		(strnicmp(extender, "PI", 2) != 0 && strnicmp(extender, "PC", 2) != 0) ||
		!(*(extender + 2) >= '1' && *(extender + 2) <= '3'))
		return M_INVALID;

	comp = *buffer;

	switch (*(buffer + 1))
	{
	case 0:
		width = 320;
		height = 200;
		Planes = 4;
		break;
	case 1:
		width = 640;
		height = 200;
		Planes = 2;
		break;
	case 2:
		width = 640;
		height = 400;
		Planes = 1;
		break;
#if 0
	case 3:
		width = 320;
		height = 480;
		Planes = 8;
		break;
	case 4:
		width = 640;
		height = 480;
		Planes = 4;
		break;
	case 5:
		width = 1280;
		height = 960;
		Planes = 1;
		break;
#endif
	default:
		return M_INVALID;
	}
	BitsPerPixel = Planes;

	DatenOffset = 34;

	strcpy(smurf_struct->smurf_pic->format_name, "Degas .PIx, PCx");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Degas ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	w = width / 8;

	if ((ziel = smurf_struct->services->SMalloc(((unsigned long) width * height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	oziel = ziel;

	buffer += DatenOffset;

	plh = (height * w);			/* H�he einer Plane in Byte */

	if (!comp)					/* unkomprimiert */
	{
		y = 0;
		do						/* height */
		{
			plo = (y * w);		/* Offset vom Planeanfang in Bytes */
			x = 0;
			do					/* width */
			{
				p = 0;
				do				/* Planes */
				{
					pla = (plh * p);	/* Abstand dieser Plane vom Bildanfang */
					ziel = oziel + pla + plo + x;	/* Zieladresse der dekodierten Scanline */
					*ziel++ = *buffer++;
					*ziel = *buffer++;
				} while (++p < Planes);
				x += 2;
			} while (x < w);
		} while (++y < height);
	}							/* comp? */
	else						/* komprimiert */
	{
		y = 0;
		do						/* height */
		{
			plo = (y * w);		/* Offset vom Planeanfang in Bytes */
			p = 0;
			do					/* Planes */
			{
				pla = (plh * p);	/* Abstand dieser Plane vom Bildanfang */
				ziel = oziel + pla + plo;	/* Zieladresse der dekodierten Scanline */
				x = 0;
				do				/* width */
				{
					v1 = *buffer++;
					if ((v1 & 0x80) == 0x80)
					{
						n = (0x101 - v1);
						v2 = *buffer++;
						for (i = 0; i < n; i++)
							*ziel++ = v2;
						x += n;
					} else
					{
						for (i = 0; i < v1 + 1; i++)
							*ziel++ = *buffer++;
						x += (v1 + 1);
					}
				} while (x < w);
			} while (++p < Planes);
		} while (++y < height);
	}							/* comp? */

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	pal = smurf_struct->smurf_pic->palette;
	ppal = (unsigned short *) (buffer + 2);

	if (BitsPerPixel == 1)
	{
		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
	} else
	{
		for (i = 16; i > 0; i--)
		{
			v1 = *ppal++;
			*pal++ = ((v1 & 0x700) >> 3);
			*pal++ = ((v1 & 0x070) << 1);
			*pal++ = ((v1 & 0x007) << 5);
		}
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
