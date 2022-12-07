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
/*				SGI - Silicon Graphic-Format				*/
/*				  developed by P. Haeberli					*/
/* Version 0.1  --  11.01.96								*/
/*	  xxx													*/
/* Version 0.2  --  13.01.96								*/
/*	  Komprimierung war immer eingeschaltet und es gab MÅll */
/*	  Wahrscheinlich noch Probleme mit komprimierten 24 Bit */
/*	  Ich brauche Testbilder!								*/
/* Version 0.3  --  26.02.97								*/
/*	  Von Arrays auf Zeiger umgestellt						*/
/*	  8 Bit Palette fÅr "dithered" Bilder wird erstellt, ob	*/
/*	  das korrekt ist, steht nirgends.						*/
/*	  Probleme bereiten 8 Bit Indexbilder, die Palette ist	*/
/*	  in einem extra File gespeichert, nur wie rankommen?	*/
/* Version 0.4  --  14.03.97								*/
/*	  16 Bit mÅûten nun funktionieren - ich habe aber keine	*/
/*	  Testbilder dafÅr.										*/
/*==========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR "[1][Zu wenig Speicher zum Korrigieren | der Orientierung][ Ok ]"
#elif COUNTRY==0
#define ERROR "[1][Zu wenig Speicher zum Korrigieren | der Orientierung][ Ok ]"
#elif COUNTRY==2
#define ERROR "[1][Zu wenig Speicher zum Korrigieren | der Orientierung][ Ok ]"
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
	"SGI-Format",
	0x0040,
	"Christian Eyrich",
	{ "SGI", "RLE", "BW", "RGB", "RGBA", "", "", "", "", "" },
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




/* Dekodiert Bilder mit einem und drei Bytes pro Pixel. */
static void decode_1bpc(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height, uint32_t *table, uint8_t Planes, uint8_t comp)
{
	uint8_t *oziel;
	uint8_t *obuffer;
	uint8_t p;
	uint8_t n;
	uint8_t v1, v2;
	unsigned short x, y;
	unsigned long tablen3;

	obuffer = buffer;
	oziel = ziel;

	buffer += 0x200;

	p = 0;
	do
	{
		tablen3 = height * p;

		ziel = oziel + p;

		y = height;
		while (y--)						/* y absteigend, damit die Tabelle von unten her gelesen wird - */
		{								/* so wird ein flip bei komprimierten Bildern ÅberflÅssig */
			if (comp)
				buffer = obuffer + table[tablen3 + y];

			x = 0;
			do							/* width */
			{
				if (!comp)
				{
					x++;
					*ziel = *buffer++;
					ziel += Planes;
				} else
				{
					v1 = *buffer++;
					if ((n = v1 & 0x7f) == 0)
						return;

					if (v1 & 0x80)
					{
						x += n;

						while (n--)
						{
							*ziel = *buffer++;
							ziel += Planes;
						}
					} else
					{
						v2 = *buffer++;

						x += n;
						while (n--)
						{
							*ziel = v2;
							ziel += Planes;
						}
					}
				}
			} while (x < width);
		}
	} while (++p < Planes);
}


/* Dekodiert Bilder mit 3 mal zwei Bytes pro Kanal. */
/* Das sind Bilder, bei denen jeder RGB-Teil mit 16 Bit Genauigkeit vorliegt. */
/* Die unteren 8 Bit shifte ich hier einfach weg. */
static void decode_2bpc(uint8_t *ziel, uint16_t *buffer, unsigned short width, unsigned short height, uint32_t *table, uint8_t Planes, uint8_t comp)
{
	uint8_t *obuffer;
	uint8_t *oziel;
	uint8_t p;
	uint8_t n;
	uint8_t v1, v2;
	unsigned short x, y;
	unsigned long tablen3;

	obuffer = (uint8_t *) buffer;
	oziel = ziel;

	buffer += 0x200 / 2;

	p = 0;
	do
	{
		tablen3 = height * p;

		ziel = oziel + p;

		y = height;
		while (y--)
		{
			if (comp)
				buffer = (uint16_t *) (obuffer + table[tablen3 + y]);

			x = 0;
			do							/* width */
			{
				if (comp == 0)
				{
					x++;
					*ziel++ = *buffer++ >> 8;
				} else
				{
					v1 = *buffer++;
					if ((n = v1 & 0x7f) == 0)
						return;
					if (v1 & 0x80)
					{
						x += n;
						while (n--)
							*ziel++ = *buffer++ >> 8;
					} else
					{
						v2 = *buffer++ >> 8;

						x += n;
						while (n--)
							*ziel++ = v2;
					}
				}
			} while (x < width);
		}
	} while (++p < Planes);
}


/* Legt eine Palette fÅr "dithered" Bilder an */
static void make_pal(uint8_t *buffer, uint8_t *palette, uint8_t BitsPerPixel, unsigned long length, uint8_t ColorMap)
{
	uint8_t *pal;
	uint8_t pixval;
	unsigned short i;

	if (BitsPerPixel == 1)
	{
		pal = palette;

		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
	} else if (BitsPerPixel == 8)
	{
		if (ColorMap == 0x0)			/* Graustufenpalette fÅr 1 Channel 8 Bit erstellen */
		{
			pal = palette;

			for (i = 0; i < 256; i++)
			{
				*pal++ = i;
				*pal++ = i;
				*pal++ = i;
			}
		} else if (ColorMap == 0x01)	/* Palette aus 3r3g2b Bits erstellen */
			while (length--)
			{
				pixval = *buffer++;
				pal = palette + pixval + pixval + pixval;

				*pal++ = (pixval & 0x07) << 5;
				*pal++ = (pixval & 0x38) << 2;
				*pal++ = pixval & 0xc0;
		} else if (ColorMap == 0x03)	/* Palette aus extra File Åbertragen */
			/* da ich das nicht kann, wird momentan eine Graustufenpalette erzeugt */
		{
			pal = palette;

			for (i = 0; i < 256; i++)
			{
				*pal++ = i;
				*pal++ = i;
				*pal++ = i;
			}
		}
	}
}


/* éndert die Orientierung der Bilder */
static short switch_orient(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *buffero;
	uint8_t *bufferu;
	uint8_t *temp;
	unsigned short t;
	unsigned long w, len, planelength;

	w = width;

	len = (w * BitsPerPixel) >> 3;
	planelength = len * height;

	if ((temp = (uint8_t *)Malloc(len)) == NULL)
		return -1;

	buffero = buffer;
	bufferu = buffero + planelength - len;

	t = height >> 1;
	while (t--)
	{
		memcpy(temp, buffero, len);
		memcpy(buffero, bufferu, len);
		memcpy(bufferu, temp, len);

		buffero += len;
		bufferu -= len;
	}

	Mfree(temp);

	return 0;
}


static void nulltospace(uint8_t *string, uint8_t length)
{
	while (--length)
	{
		if (*string == '\0')
			*string = ' ';
		string++;
	}
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      SGI - Silicon Graphic-Format (SGI)          */
/*		8, 24 Bit (in 3 Ebenen),					*/
/*		ohne Komprimierung/RLE						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t comp;
	uint8_t Planes;
	uint8_t BitsPerPixel;
	uint8_t ColorMap;
	uint8_t Bpc;
	char dummy[3];
	char impmessag[21];
	unsigned short width, height;
	uint32_t *table;
	unsigned long tablen;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (*(uint16_t *) buffer != 0x01da)
		return M_INVALID;

	comp = *(buffer + 0x02);

	Bpc = *(buffer + 0x03);

#if 0
	/* unwichtig */
	Dim = *(uint16_t *) (buffer + 0x04);
#endif

	width = *(uint16_t *) (buffer + 0x06);
	height = *(uint16_t *) (buffer + 0x08);

	/* 1 = BW, dithered, colormapped; 3 = RGB; 4 = RGB + alpha channel */
	Planes = *(uint16_t *) (buffer + 0x0a);
	if (Planes > 3)					/* sonst wÑren das ja ZustÑnde */
		Planes = 3;

	/* 0x00 - Normal pixel values: 1 Channel Greyscale, 3 Channels RGB */
	/* 0x01 - Dithered Images: (8 BPP 3+3+2) */
	/* 0x02 - single Channel Images */
	/* 0x03 - color Map */
	ColorMap = *(uint32_t *) (buffer + 0x6c);
	if (ColorMap > 3)
		return M_UNKNOWN_TYPE;

	BitsPerPixel = Planes * 8;


	nulltospace(buffer + 0x18, 80);
	strncpy(smurf_struct->smurf_pic->infotext, buffer + 0x18, sizeof(smurf_struct->smurf_pic->infotext) - 1);
	strcpy(smurf_struct->smurf_pic->format_name, "SGI-Format .SGI, .RLE");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "SGI ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if ((ziel = (uint8_t *) smurf_struct->services->SMalloc(((unsigned long) width * height * BitsPerPixel) >> 3)) == NULL)
		return M_MEMORY;

	if (comp)
	{
		tablen = (unsigned long) height * Planes * sizeof(*table) * 2;

		if ((table = (uint32_t *) Malloc(tablen)) == NULL)
		{
			smurf_struct->services->SMfree(ziel);
			return M_MEMORY;
		} else
		{
			memcpy(table, buffer + 0x200, tablen);
		}
	} else
	{
		table = NULL;
	}

	if (Bpc == 1)
		decode_1bpc(ziel, buffer, width, height, table, Planes, comp);
	else
		decode_2bpc(ziel, (uint16_t *) buffer, width, height, table, Planes, comp);

	Mfree(table);

	if (!comp)
		if (switch_orient(ziel, width, height, BitsPerPixel) != 0)
			form_alert(1, ERROR);

	if (BitsPerPixel <= 8)
		make_pal(ziel, smurf_struct->smurf_pic->palette, BitsPerPixel, (unsigned long) width * height, ColorMap);

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	if (BitsPerPixel == 8)
		smurf_struct->smurf_pic->col_format = GREY;
	else
		smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
