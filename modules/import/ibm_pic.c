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
/*					IBM Picture Maker-Format				*/
/* Version 0.1  --  26.01.96								*/
/*	  8 Bit													*/
/* Version 0.2  --  18.10.97								*/
/*	  Ab jetzt wird direkt addressiert und nicht mehr per	*/
/*	  Index.												*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"IBM Picture Maker-Format",
	0x0020,
	"Christian Eyrich",
	{ "PIC", "", "", "", "", "", "", "", "", "" },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ NULL, NULL, NULL, NULL },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	0,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			IBM Picture Maker-Format (PIC)			*/
/*		8 Bit, wahlweise RLE						*/
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
	uint8_t v1, v2;
	uint8_t comp;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;

	unsigned short x, y;
	unsigned short j;
	unsigned short n;
	unsigned short cols;
	unsigned short width, height;
	unsigned long i;


	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*buffer != 0x00 || (*(buffer + 1) != 0x85 && *(buffer + 1) != 0x86) || *(buffer + 2) != 0xc1)
		return M_INVALID;

	comp = *(buffer + 0x09);
	BitsPerPixel = 8;

	width = *(buffer + 0x05) + (*(buffer + 0x06) << 8);
	height = *(buffer + 0x07) + (*(buffer + 0x08) << 8);

	cols = *(buffer + 0xa) + (*(buffer + 0xb) << 8);

	DatenOffset = 0x80 + cols * 3;

	strcpy(smurf_struct->smurf_pic->format_name, "IBM Picture Maker-File .PIC");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	smurf_struct->services->reset_busybox(128, "IBM Picturemaker 8 Bit");

	if ((ziel = smurf_struct->services->SMalloc(((unsigned long) width * height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	oziel = ziel;

	buffer += DatenOffset;

	y = 0;
	do
	{
		x = 0;
		do
		{
			if (!comp)
			{
				*ziel++ = *buffer++;
				x++;
			} else
			{
				v1 = *buffer++;

				if (v1 > 0x7f)
				{
					if (v1 == 0xff)
					{
						n = *buffer++;
						n |= (*buffer++ << 8);
					} else
					{
						n = v1 - 0x80;
					}
					v2 = *buffer++;

					x += n;

					while (n--)
						*ziel++ = v2;
				} else
				{
					x += v1;

					while (v1--)
						*ziel++ = *buffer++;
				}
			}
		} while (x < width);
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	ppal = buffer + 0x80;
	j = 0;
	while (j++ < 3)
	{
		pal = smurf_struct->smurf_pic->palette + j;
		for (i = 0; i < cols; i++)
		{
			*pal = *ppal++;
			pal += 3;
		}
	}

	smurf_struct->smurf_pic->col_format = RGB;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
