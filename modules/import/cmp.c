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
/*				CMP Atari Public Painter Monochrom			*/
/* Version 0.1  --  irgendwann 1996							*/
/*	  1 Bit, binÑre Version									*/
/* Version 0.2  --  18.11.98								*/
/*	  sicherer gegen kaputte CMP gemacht (EndeÅberprÅfung	*/
/*	  eingebaut)											*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Atari Public Painter",
	0x0020,
	"Christian Eyrich, Dale Russell",
	{ "CMP", "", "", "", "", "", "", "", "", "" },
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
/*		Atari Public Painter Monochrom (CMP)		*/
/*		1 Bit, RLE									*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *obuffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pal;
	uint8_t S_Byte;
	uint8_t v1, v2;
	unsigned short width, height;
	unsigned short w, x, y, n;
	unsigned long maxlen;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	if (stricmp(fileext(smurf_struct->smurf_pic->filename), "CMP") != 0)
		return M_INVALID;
	S_Byte = *buffer;
	buffer += 2;

	width = 640;
	w = 80;
	height = 400;

	strcpy(smurf_struct->smurf_pic->format_name, "Atari Public Painter");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;

	smurf_struct->services->reset_busybox(128, "Public Painter 1 Bit");

	if ((ziel = smurf_struct->services->SMalloc(32000L)) == 0)
		return M_MEMORY;
	oziel = ziel;

	maxlen = 32000L;

	y = 0;
	do
	{
		x = 0;
		do
		{
			v1 = *buffer++;
			if (v1 == S_Byte)
			{
				n = *buffer++ + 1;
				v2 = *buffer++;

				x += n;

				while (n--)
				{
					*ziel++ = v2;
					if (!--maxlen)
						goto end;
				}
			} else
			{
				*ziel++ = v1;
				x++;
				if (!--maxlen)
					goto end;
			}
		} while (x < w);
	} while (++y < width);

  end:

	buffer = obuffer;
	ziel = oziel;

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->services->SMfree(buffer);

	smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	pal = smurf_struct->smurf_pic->palette;
	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;

	return M_PICDONE;
}
