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
/*                  Vivid Ray-Tracer-Format                 */
/* Version 0.1  --  12.01.96                                */
/*	  xxx													*/
/* Version 0.1  --  19.10.97                                */
/*	  Ab jetzt wird direkt addressiert und nicht mehr per	*/
/*	  Index.												*/
/*==========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Vivid-Format",
	0x0020,
	"Christian Eyrich",
	{ "IMG", "", "", "", "", "", "", "", "", "" },
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
/*			Vivid Raytracer-Format (IMG)			*/
/*		24 Bit, RLE									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	short n;
	uint8_t v1, v2, v3;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	unsigned short x, y, width, height;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (*(uint16_t *) (buffer + 0x08) != 0x0018)
		return M_INVALID;

	BitsPerPixel = *(uint16_t *) (buffer + 0x08);

	width = *(uint16_t *) buffer;
	height = *(uint16_t *) (buffer + 0x02);

	strcpy(smurf_struct->smurf_pic->format_name, "Vivid Raytracer .IMG");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	smurf_struct->services->reset_busybox(128, "Vivid Raytracer 24 Bit");

	if ((ziel = smurf_struct->services->SMalloc((unsigned long) width * height * 3)) == NULL)
		return M_MEMORY;

	oziel = ziel;

	DatenOffset = 0x0a;
	buffer += DatenOffset;

	y = 0;
	do
	{
		x = 0;
		do
		{
			n = *buffer++;
			v1 = *buffer++;
			v2 = *buffer++;
			v3 = *buffer++;

			x += n;

			while (--n >= 0)
			{
				*ziel++ = v1;
				*ziel++ = v2;
				*ziel++ = v3;
			}
			y = y;
		} while (x < width);
	} while (++y < height);

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	smurf_struct->services->SMfree(buffer);

	return M_PICDONE;
}
