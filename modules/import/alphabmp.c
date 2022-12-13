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
/*				Alpha Microsystems BMP-Format				*/
/* Version 0.1  --  15.01.96								*/
/*	  Aufpassen bei 4 Bit, Pixel sind gepackt!				*/
/* Version 0.2  --  24.10.96								*/
/*	  RGB-BGR-Turn aus- und setzen einer Standardpalette bei*/
/*	  monochromen Bildern eingebaut.						*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

#undef min
#define min(a,b)             ((a) < (b) ? (a) : (b))


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Alpha BMP-Format",
	0x0020,
	"Christian Eyrich",
	{ "BMP", "", "", "", "", "", "", "", "", "" },
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



static void nulltospace(uint8_t *string, unsigned short length)
{
	while (--length)
	{
		if (*string == '\0')
			*string = ' ';
		string++;
	}
}


static const char *fileext(const char *filename)
{
	const char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			Alpha Microsystems BMP-Format (BMP)		*/
/*		1, 4, 8 und 24 Bit, wahlweise RLE			*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t *ppal;
	char *fname;
	uint8_t n;
	uint8_t v1;
	uint8_t v2;
	uint8_t comp;
	uint8_t ifpal;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	char dummy[3];
	char impmessag[21];

	unsigned short x, y;
	unsigned short i, j;
	unsigned short cols;
	unsigned short width, height;

	unsigned long w;
	unsigned long src_pos = 0;
	unsigned long dst_pos = 0;

	buffer = smurf_struct->smurf_pic->pic_data;

	fname = smurf_struct->smurf_pic->filename;
	if (*(unsigned short *) buffer != 0xffff || stricmp(fileext(fname), "BMP") != 0)
		return M_INVALID;
	BitsPerPixel = *(buffer + 0x0e);

	width = *(buffer + 0x0a) + (*(buffer + 0x0b) << 8);
	height = *(buffer + 0x0c) + (*(buffer + 0x0d) << 8);

	ifpal = *(uint16_t *) (buffer + 0x10);
	comp = *(uint16_t *) (buffer + 0x12);

	cols = *(buffer + 0x50) + (*(buffer + 0x51) << 8);
	DatenOffset = 0x50 + cols * 3;

	nulltospace(buffer + 0x14, 112);
	strncpy(smurf_struct->smurf_pic->infotext, (char *)buffer + 0x14, min(112, sizeof(smurf_struct->smurf_pic->infotext)) - 1);

	strcpy(smurf_struct->smurf_pic->format_name, "Alpha BMP-File .BMP");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Alpha-BMP ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if ((ziel = smurf_struct->services->SMalloc(((long) width * (long) height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;
	if (BitsPerPixel == 1)
		w = (width + 7) / 8;
	else if (BitsPerPixel == 24)
		w = width * 3;
	else
		w = width;

	src_pos += DatenOffset;
	y = 0;
	do							/* height */
	{
		x = 0;
		do						/* width */
		{
			if (comp == 0)
			{
				ziel[dst_pos++] = buffer[src_pos++];
				x++;
			} else
			{
				v1 = buffer[src_pos++];
				if (v1 > 0x80)
				{
					n = (0x101 - v1);
					v2 = buffer[src_pos++];
					for (i = 0; i < n; i++)
						ziel[dst_pos++] = v2;
					x += n;
				} else
				{
					for (i = 0; i < v1; i++)
						ziel[dst_pos++] = buffer[src_pos++];
					x += v1;
				}
			}
		} while (x < w);		/* x */
		y++;
	} while (y < height);		/* y */

	smurf_struct->smurf_pic->pic_data = ziel;

	if (BitsPerPixel == 1)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	if (ifpal)
	{
		pal = smurf_struct->smurf_pic->palette;
		ppal = buffer + 0x52;
		j = *(uint16_t *) (buffer + 0x50) * 3;
		while (j--)
			*pal++ = *ppal++;
	} else if (BitsPerPixel == 1)
	{
		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
