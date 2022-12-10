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
/*				PCPaint/Pictor Page Format					*/
/* Version 0.1  --  22.02.97 - 27.02.97						*/
/*	  1, 4, 8 Bit - bei manchen 4 Bit gibt es noch Probleme	*/
/*	  (ULX001BT.PIC), die sind anscheinend in einem anderen	*/
/*	  Format.												*/
/* Version 0.2  --  30.05.98								*/
/*	  neue setpix_std_line eingebaut						*/
/* Version 0.3  --  xx.xx.98								*/
/*	  16, 24 Bit											*/
/* Version 0.4  --  xx.xx.98								*/
/*	  BSAVE Format wird unterst�tzt							*/
/* Version 0.5  --  xx.xx.98								*/
/*	  Clipping Format wird unterst�tzt						*/
/* Version 0.6  --  xx.xx.98								*/
/*	  Overlay Format wird unterst�tzt						*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][Zu wenig Speicher zum Korrigieren | der Orientierung und Formatwandlung][ Ok ]"
#define ERROR2 "[1][Zu wenig Speicher zum Korrigieren | der Orientierung][ Ok ]"
#elif COUNTRY==0
#define ERROR1 "[1][Not enough memory to correct the  | orientation and format conversion! ][ OK ]"
#define ERROR2 "[1][Not enough memory to correct | the orientation!][ OK ]"
#elif COUNTRY==2
#define ERROR1 "[1][Not enough memory to correct the  | orientation and format conversion! ][ OK ]"
#define ERROR2 "[1][Not enough memory to correct | the orientation!][ OK ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define TIMER 0

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Pictor PC Paint",
	0x0020,
	"Christian Eyrich",
	{ "PIC", "", "", "", "", "", "", "", "", "" },
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



/* �ndert die Orientierung der Bilder */
static short switch_orient(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel, uint8_t Planes)
{
	uint8_t *buffero;
	uint8_t *bufferu;
	uint8_t *temp;
	uint8_t p;
	unsigned short t;
	unsigned long w, len, planelength;

	if (BitsPerPixel == 1 || Planes == 2 || Planes == 4 || Planes == 8)
		w = (unsigned long) (width + 7) / 8 * 8;
	else
		w = (unsigned long) width;

	len = (w * BitsPerPixel) >> 3;
	planelength = len * height;

	if ((temp = (uint8_t *)Malloc(len)) == 0)
		return -1;

	p = 0;
	do
	{
		buffero = buffer + planelength * p;
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
	} while (++p < Planes);

	Mfree(temp);

	return 0;
}


/* Wandelt 2 und 4 Bit Pixelpacked nach Standardformat */
/* und �ndert gleich die Orientierung mit */
static uint8_t *tfm_pp_to_std(uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *bufferu;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t val;
	unsigned short x, y, oline;
	unsigned long planelength;

	/* Hier kommen sowieso nur 2 und 4 Bit rein */
	planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

	if (BitsPerPixel == 2)
		oline = (width + 3) / 4;
	else
		oline = (width + 1) / 2;

	if ((ziel = SMalloc(planelength * BitsPerPixel)) == 0)
		return NULL;

	oziel = ziel;

	pixbuf = (uint8_t *) Malloc(width);

	bufferu = buffer + (unsigned long) oline * (height - 1);

	y = 0;
	do
	{
		x = 0;
		do
		{
			val = *bufferu++;

			if (BitsPerPixel == 2)
			{
				pixbuf[x++] = val >> 6;
				pixbuf[x++] = val >> 4;
				pixbuf[x++] = val >> 2;
				pixbuf[x++] = val & 0x03;
			} else
			{
				pixbuf[x++] = val >> 4;
				pixbuf[x++] = val & 0x0f;
			}
		} while (x < width);

		ziel += setpix_std_line(pixbuf, ziel, BitsPerPixel, planelength, width);
		bufferu -= oline + oline;
	} while (++y < height);

	Mfree(pixbuf);

	ziel = oziel;

	SMfree(buffer);

	return ziel;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Pictor PC Paint					*/
/*		1, 2, 4, 8, 16, 24 Bit, ungepackt, RLE		*/
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
	uint8_t *temp;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	uint8_t palflag;
	uint8_t RunMarker;
	uint8_t v1, v2;
	char dummy[3];
	char impmessag[21];

	unsigned short width, height;
	unsigned short Offset;
	unsigned short PalType;
	unsigned short PalSize; 
	unsigned short Blocks;
	unsigned short x;
	unsigned short z;
	unsigned short osize;
	unsigned short n;
	unsigned long w;

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*(uint16_t *) buffer != 0x3412)
		return M_INVALID;

	width = swap_word(*(uint16_t *) (buffer + 0x02));
	height = swap_word(*(uint16_t *) (buffer + 0x04));

	/* Erweiterte Planeinfo f�r HiColor-Format
	   0x01 = 1 Plane,   1 BPP, 2 Farben
	   0x02 = 1 Plane,   2 BPP, 4 Farben
	   0x04 = 1 Plane,   4 BPP, 16 Farben
	   0x08 = 1 Plane,   8 BPP, 256 Farben
	   0x10 = 1 Plane,  16 BPP, 32768/65536 Farben
	   0x11 = 2 Planes,  1 BPP, 4 Farben
	   0x18 = 1 Plane,  24 BPP, 16777216 Farben
	   0x28 = 3 Planes,  8 BPP, 16777216 Farben
	   0x31 = 4 Planes,  1 BPP, 16 Farben */

	BitsPerPixel = *(buffer + 0x0a);
	switch (BitsPerPixel)
	{
	case 0x10:
		BitsPerPixel = 16;
		Planes = 1;
		break;
	case 0x11:
		BitsPerPixel = 2;
		Planes = 2;
		break;
	case 0x18:
		BitsPerPixel = 24;
		Planes = 1;
		break;
	case 0x28:
		BitsPerPixel = 24;
		Planes = 3;
		break;
	case 0x31:
		BitsPerPixel = 4;
		Planes = 4;
		break;
	case 0x71:
		BitsPerPixel = 8;
		Planes = 8;
		break;
	default:
		Planes = 1;
		break;
	}

	palflag = *(buffer + 0x0b);

#if 0
	VideoMode = *(buffer + 0x0c);
#endif

	if (palflag != 0xff)			/* Version < 2.0 */
	{
		Offset = 0x0c;
		PalSize = 0;
		PalType = 0;
	} else							/* Version >= 2.0 */
	{
		Offset = 0x11;

		/* Art der Palette */
		/* 0 = keine, 1 = CGA-Palette, 2 = PCjr.-Palette,
		   3 = EGA-Palette, 4 = VGA-Palette */
		PalType = (buffer[14] << 8) | buffer[13];
		/* L�nge der Palette */
		PalSize = (buffer[16] << 8) | buffer[15];

		Offset += PalSize;
	}

	/* gepackte Bl�cke, 0 = ungepackte Daten */
	Blocks = (buffer[Offset + 1] << 8) | buffer[Offset];

	strcpy(smurf_struct->smurf_pic->format_name, "Pictor PC Paint");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Pictor Paint ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 1 || Planes == 2 || Planes == 4 || Planes == 8)
		w = (unsigned long) (width + 7) / 8 * 8;
	else
		w = (unsigned long) width;


	if ((ziel = SMalloc((w * height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	oziel = ziel;

	buffer += 0x02 + Offset;	/* auf ersten Paketkopf setzen */

	z = 0;
	do
	{
		/* BlockSize = */ buffer += 2;
		/* Blockgr��e inkl. Header */
		osize = (buffer[1] << 8) | buffer[0];	/* Gr��e der Originaldaten */
		buffer += 2;
		RunMarker = *buffer++;	/* Marker f�r Bytefolgen */

		x = 0;
		while (x < osize)
		{

			v1 = *buffer++;
			if (v1 == RunMarker)
			{
				n = *buffer++;
				if (n)
				{
					v2 = *buffer++;

					x += n;
					while (n--)
						*ziel++ = v2;
				} else
				{
					n = (buffer[1] << 8) | buffer[0];
					buffer += 2;
					v2 = *buffer++;

					x += n;
					while (n--)
						*ziel++ = v2;
				}
			} else
			{
				x++;

				*ziel++ = v1;
			}
		}						/* noch Daten im Block */
	} while (++z < Blocks);		/* Bl�cke */

	buffer = obuffer;
	ziel = oziel;

	if ((BitsPerPixel == 2 || BitsPerPixel == 4) && Planes == 1)
	{
		if ((temp = tfm_pp_to_std(ziel, width, height, BitsPerPixel)) == 0)
		{
			form_alert(1, ERROR1);
			SMfree(ziel);
			return M_MEMORY;
		} else
		{
			ziel = temp;
		}
	} else if (switch_orient(ziel, width, height, BitsPerPixel, Planes) != 0)
	{
		form_alert(1, ERROR2);
	}

	if (BitsPerPixel <= 8)
	{
		pal = smurf_struct->smurf_pic->palette;

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
#if 0
			if (PalType == 1)	/* CGA-style Palette */
				if (PalType == 2)	/* PC jr-style Palette */
					if (PalType == 3)	/* EGA-style Palette */
#endif
						if (PalType == 4)	/* VGA-style Palette */
						{
							ppal = buffer + 0x11;

							for (x = 0; x < PalSize; x++)
								*pal++ = *ppal++ << 2;	/* Palette hat lt. Holtorf pro Element 6 Bit */
						}
		}
	}

	if ((BitsPerPixel == 8 && Planes == 1) || BitsPerPixel == 16 || BitsPerPixel == 24)
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	else
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->smurf_pic->pic_data = ziel;

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu", get_timer());
	(void)Cnecin();
#endif

	SMfree(buffer);

	return M_PICDONE;
}
