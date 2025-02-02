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
/*					Erdas Image-Format						*/
/* Version 0.1  --  17.01.96								*/
/*	  8 und 24 Bit											*/
/* Version 0.2  --  19.10.97								*/
/*	  Runderneuerung, Bugfixes und in place-Bearbeitung		*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

#ifdef __PUREC__
/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Erdas Image-Format",
	0x0020,
	"Christian Eyrich",
	{ "LAN", "GIS", "", "", "", "", "", "", "", "" },
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
/*              Erdas Image-Format (LAN)            */
/*      8 und 24 Bit, unkomprimiert                 */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t Planes;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	char dummy[3];
	char impmessag[21];
	unsigned short x, y, width, height, i;
	unsigned long len;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*(uint32_t *) buffer != 0x48454144L || *(uint16_t *) (buffer + 4) != 0x3734)
		return M_INVALID;
	/* Liegt zwar als DWord vor, ich lese aber nur die hintersten beiden Bytes */
	Planes = (char) swap_word(*(uint16_t *) (buffer + 0x0a));
	if (Planes == 1)
		BitsPerPixel = 8;
	else
		BitsPerPixel = 24;

	/* Liegen zwar als DWord vor, ich lese aber nur die hintersten beiden Bytes */
	width = swap_word(*(uint16_t *) (buffer + 0x12));
	height = swap_word(*(uint16_t *) (buffer + 0x16));

	DatenOffset = 0x80;

	strcpy(smurf_struct->smurf_pic->format_name, "Erdas Image-Format .LAN, .SUN");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "LAN Erdas ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	ziel = buffer;

	len = (((unsigned long) width * (unsigned long) height) * BitsPerPixel) >> 3;

	if (BitsPerPixel == 8)
	{
		memmove(ziel, buffer + DatenOffset, len);
	} else
	{
		buffer += DatenOffset;

		y = 0;
		do
		{
			x = 0;
			do
			{
				*ziel++ = *buffer;
				*ziel++ = *(buffer + width);
				*ziel++ = *(buffer++ + (width << 1));
			} while (++x < width);
			buffer += (width << 1);
		} while (++y < height);
	}

	buffer = obuffer;
	ziel = buffer;

	_Mshrink(ziel, len);

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	if (BitsPerPixel == 8)
	{
		pal = smurf_struct->smurf_pic->palette;

		for (i = 0; i < 256; i++)
		{
			*pal++ = (char) i;
			*pal++ = (char) i;
			*pal++ = (char) i;
		}
	}

	smurf_struct->smurf_pic->col_format = RGB;

	return M_PICDONE;
}
