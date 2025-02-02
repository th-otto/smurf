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
/*					X Window Dump Format					*/
/* Version 0.1  --  12.01.96 - 26.01.97						*/
/*	  1, 4, 8, 24 Bit										*/
/* Version 0.2  --  30.05.98								*/
/*	  neue setpix_std_line eingebaut						*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR "[3][ Auch wenn das Bild korrekt | dargestellt werden sollte, | bitte schicken Sie es uns. |          Danke!        ][ Ok ]"
#elif COUNTRY==0
#define ERROR "[3][ Auch wenn das Bild korrekt | dargestellt werden sollte, | bitte schicken Sie es uns. |          Danke!        ][ Ok ]"
#elif COUNTRY==2
#define ERROR "[3][ Auch wenn das Bild korrekt | dargestellt werden sollte, | bitte schicken Sie es uns. |          Danke!        ][ Ok ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

#define LSB 0
#define MSB 1

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"X Window Dump-Format",
	0x0010,
	"Christian Eyrich",
	{ "XWD", "", "", "", "", "", "", "", "", "" },
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
/*			X Window Dump-Format (XWD)				*/
/*		1, 4, 8 und 24 Bit, unkomprimiert			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t *opixbuf;
	uint8_t *pal;
	uint8_t *ppal;
	char *fname;
	uint8_t BitsPerPixel;
	uint8_t version;
	uint8_t ByteOrder;
	uint8_t BitOrder;
	uint8_t colent;
	uint8_t translate[256];
	uint8_t PalentrySize;
	uint8_t Palpad;
	uint8_t val;
	uint8_t map;
	char dummy[3];
	char impmessag[21];
	unsigned short i, x, y, width, height, cols;

	unsigned long w;
	unsigned long memwidth;
	unsigned long DatenOffset;
	unsigned long pal_of;
	unsigned long planelength;


	buffer = smurf_struct->smurf_pic->pic_data;

	/* Header Check */
	fname = smurf_struct->smurf_pic->filename;
	if (stricmp(fileext(fname), "XWD") != 0 ||
/* Kannste l�ten, die Headerl�nge ist nicht garantiert */
/*	   (*(uint32_t *)buffer != 40 && *(uint32_t *)buffer != 100) || */
		(*(buffer + 0x07) != 6 && *(buffer + 0x07) != 7))
		return M_INVALID;

	version = *(buffer + 0x04);

	/* X10 */
	if (version == 6)
	{
		BitsPerPixel = *(uint32_t *) (buffer + 0x0c);
		/* 0 = monochrom, 2 = pseudo color */
#if 0
		PixmapFormat = *(uint32_t *) (buffer + 0x10);
#endif
		width = (unsigned int) *(uint32_t *) (buffer + 0x14);
		height = (unsigned int) *(uint32_t *) (buffer + 0x18);
		ByteOrder = 0;
		BitOrder = 0;
#if 0
		PadBytes = 0;
#endif
		colent = 8;
		PalentrySize = 2;
		Palpad = 0;
	}
	/* X11 */
	else
	{
		/* 0 = monochrom, 1 = single planes, 2 = pseudo color */
#if 0
		PixmapFormat = *(uint32_t *) (buffer + 0x08);
#endif
		BitsPerPixel = *(uint32_t *) (buffer + 0x0c);
		width = (unsigned int) *(uint32_t *) (buffer + 0x10);
		height = (unsigned int) *(uint32_t *) (buffer + 0x14);
		/* 0 = LSB, 1 = MSB */
		ByteOrder = *(uint32_t *) (buffer + 0x1c);
		/* 0 = LSB, 1 = MSB */
		BitOrder = *(uint32_t *) (buffer + 0x24);
#if 0
		PadBytes = (*(uint32_t *) (buffer + 0x28)) >> 3;
#endif
		colent = 12;
		PalentrySize = 4;
		Palpad = 2;
	}

	if (ByteOrder == LSB || BitOrder == LSB)
		form_alert(1, ERROR);

	if (ByteOrder == LSB)
		map = 2;

	i = 0;
	do
	{
		/* Bitreihenfolge umdrehen ... */
		if (BitOrder == LSB)
		{
			translate[i]  = ((i & 0x01) << 7);
			translate[i] |= ((i & 0x02) << 5);
			translate[i] |= ((i & 0x04) << 3);
			translate[i] |= ((i & 0x08) << 1);
			translate[i] |= ((i & 0x10) >> 1);
			translate[i] |= ((i & 0x20) >> 3);
			translate[i] |= ((i & 0x40) >> 5);
			translate[i] |= ((i & 0x80) >> 7);
		}
		/* ... oder auch nicht */
		else
			translate[i] = i;
	} while (++i < 256);

	pal_of = *(uint32_t *) buffer;
	if (version == 6)
	{
		if (BitsPerPixel <= 8)
			cols = 1 << BitsPerPixel;
		else
			cols = 0;
	} else
	{
		cols = (unsigned int) *(uint32_t *) (buffer + 0x4c);
	}
	DatenOffset = pal_of + (colent * cols);

	strcpy(smurf_struct->smurf_pic->format_name, "X Window Dump .XWD");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "X Window Dump ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 24)
	{
		w = width;
		memwidth = width;
	} else if (BitsPerPixel == 1)
	{
		w = ((width + 7) / 8);
		memwidth = w * 8;
	} else if (BitsPerPixel == 4)
	{
		w = (width + 1) / 2;
		memwidth = ((width + 7) / 8) * 8;
	} else
	{
		w = width;
		memwidth = width;
	}

	if ((ziel = smurf_struct->services->SMalloc((memwidth * height * BitsPerPixel) >> 3)) == NULL)
		return M_MEMORY;

	oziel = ziel;
	obuffer = buffer;
	buffer += DatenOffset;

	planelength = (unsigned long) ((width + 7) / 8) * height;

	if (BitsPerPixel <= 8)
	{
		if (BitsPerPixel == 1 || BitsPerPixel == 8)
		{
			y = 0;
			do
			{
				x = 0;
				do
				{
					*ziel++ = translate[*buffer++];
				} while (++x < w);
			} while (++y < height);
		} else if (BitsPerPixel == 4)
		{
			opixbuf = pixbuf = (uint8_t *) Malloc(width);

			y = 0;
			do
			{
				x = 0;
				do
				{
					val = translate[*buffer++];
					*pixbuf++ = val >> 4;
					*pixbuf++ = val & 0x0f;
				} while (++x < w);

				pixbuf = opixbuf;
				ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);
			} while (++y < height);

			Mfree(pixbuf);
		}
	} else
	{
		y = 0;
		do
		{
			x = 0;
			do
			{
				buffer++;
				*(ziel++ + map) = translate[*buffer++];
				*ziel++ = translate[*buffer++];
				*(ziel++ - map) = translate[*buffer++];
			} while (++x < w);
		} while (++y < height);
	}

	ziel = oziel;
	buffer = obuffer;

	smurf_struct->smurf_pic->pic_data = ziel;

	if (BitsPerPixel < 8)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

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
			ppal = buffer + pal_of;
			for (i = 0; i < cols; i++)
			{
				ppal += PalentrySize;
				*pal++ = *ppal;
				ppal += 2;
				*pal++ = *ppal;
				ppal += 2;
				*pal++ = *ppal;
				ppal += 2;
				ppal += Palpad;
			}
		}
	}

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
