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
/*					Turbo Pascal-Image Decoder				*/
/* Version 0.1  --  27.01.96								*/
/*	  4 Bit, die Bilder sind etwas verschoben				*/
/*	  Schwierigkeiten bei der Erkennung "/ 2"!				*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Turbo Pascal Grafik Format",
	0x0010,
	"Christian Eyrich",
	{ "TPI", "", "", "", "", "", "", "", "", "" },
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

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				Turbo Pascal Format .TPI			*/
/*		4 Bit, unkomprimiert						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t i, p;
	unsigned short x, y, width, height, DatenOffset;
	unsigned long dst_pos;
	unsigned long src_pos;
	unsigned long pal_pos;
	unsigned long w;
	unsigned long pla, plh, plo;

	static struct pal
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	} const stdpal[] = {
		{ 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0xa1 },
		{ 0x00, 0xa1, 0x00 },
		{ 0x00, 0xa1, 0xa1 },
		{ 0xa1, 0x00, 0x00 },
		{ 0xa1, 0x00, 0xa1 },
		{ 0xa1, 0x55, 0x00 },
		{ 0xa1, 0xa1, 0xa1 },
		{ 0x55, 0x55, 0x55 },
		{ 0x55, 0x55, 0xff },
		{ 0x55, 0xff, 0x55 },
		{ 0x55, 0xff, 0xff },
		{ 0xff, 0x55, 0x55 },
		{ 0xff, 0x55, 0xff },
		{ 0xff, 0xff, 0x55 },
		{ 0xff, 0xff, 0xff },
	};

	const struct pal *ppal;

	buffer = smurf_struct->smurf_pic->pic_data;

	/* Header Check *//* (((Word0 + 1) * (Word1 + 1)) / 2) + 6 */
	if (smurf_struct->smurf_pic->file_len !=
		((swap_word(*(uint16_t *) buffer) + 1) / 2) * (swap_word(*(uint16_t *) (buffer + 2)) + 1) + 6)
		return M_INVALID;

	width = swap_word(*(uint16_t *) buffer) + 1;
	height = swap_word(*(uint16_t *) (buffer + 2)) + 1;

	DatenOffset = 6;

	strcpy(smurf_struct->smurf_pic->format_name, "Turbo Pascal Image .TPI");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 4;

	smurf_struct->services->reset_busybox(128, "Turbo Pascal 4 Bit");

	if ((ziel = smurf_struct->services->SMalloc(((long) width * (long) height) >> 1)) == 0)
		return M_MEMORY;

	src_pos = DatenOffset;
	w = (width + 7) / 8;
	plh = (height * w);			/* H”he einer Plane in Byte */

	y = 0;
	do							/* height */
	{
		plo = (y * w);			/* Offset vom Planeanfang in Bytes */
		for (p = 0; p < 4; p++)	/* 4 Planes */
		{
			pla = (plh * p);	/* Abstand dieser Plane vom Bildanfang */
			dst_pos = (pla + plo);	/* Zieladresse der dekodierten Scanline */
			x = 0;
			do
			{
				ziel[dst_pos++] = buffer[src_pos++];
				x++;
			} while (x < w);	/* width */
		}						/* p */
		y++;
	} while (y < height);		/* height */

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	pal = smurf_struct->smurf_pic->palette;

	ppal = stdpal;
	pal_pos = 0;
	for (i = 0; i < 16; i++)
	{
		pal[pal_pos++] = ppal->r;
		pal[pal_pos++] = ppal->g;
		pal[pal_pos++] = ppal->b;
		ppal++;
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);
	return M_PICDONE;
}
