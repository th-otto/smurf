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
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Indypaint-Importer",
	0x0050,
	"Dale Russell",
	{ "TRU", "", "", "", "", "", "", "", "", "" },
	/* Objekttitel */
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
/*      IndyPaint (by Lazer)            .tru        */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint16_t *buffer;
	uint8_t *ziel;
	uint16_t *obuf;
	unsigned short width, height;
	long len;
	unsigned short x, y, r, g, b;
	uint16_t h1;

	buffer = smurf_struct->smurf_pic->pic_data;
	len = smurf_struct->smurf_pic->file_len;

	if (strncmp((char *) buffer, "Indy", 4) != 0)
		return M_INVALID;

	strcpy(smurf_struct->smurf_pic->format_name, "Indypaint I - .TRU");
	if (len < 130000L)
	{
		width = 320;
		height = 200;
	} else if (len < 160000L)
	{
		width = 320;
		height = 240;
	} else if (len < 190000L)
	{
		width = 384;
		height = 240;
	} else if (len < 260000L)
	{
		width = 640;
		height = 200;
	} else
	{
		width = 640;
		height = 400;
	}

	obuf = buffer;
	buffer += 128;
	ziel = (uint8_t *)Malloc((unsigned long) width * (unsigned long) height * 3);
	if (!ziel)
		return M_MEMORY;
	smurf_struct->smurf_pic->pic_data = ziel;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			h1 = *(buffer++);
			r = (h1 & 0xf800) >> 8;
			g = (h1 & 0x07e0) >> 3;
			b = (h1 & 0x001f) << 3;
			*(ziel++) = b;
			*(ziel++) = g;
			*(ziel++) = r;
		}
	}
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->depth = 24;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	Mfree(obuf);
	return M_PICDONE;
}
