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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
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


/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Zeiss-Importer",
	0x0050,
	"Dale Russell",
	{ "DTA", "", "", "", "", "", "", "", "", "" },
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
/*      Zeiss BIVAS Dekomprimierer (DTA)            */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *pal;
	uint8_t *retbuf;
	long len;
	unsigned short width, height;
	unsigned short FarbCode;
	unsigned short c;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (*(buffer + 2) != 0x47 || *(buffer + 3) != 0x12 || *(buffer + 4) != 0x6d
		|| *(buffer + 5) != 0xb0 || *(buffer + 14) != 0xff || *(buffer + 15) != 0)
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "Zeiss BIVAS 8 Bit");

	strcpy(smurf_struct->smurf_pic->format_name, "Zeiss BIVAS .DTA");
	width = *(buffer + 6) + (*(buffer + 7) << 8);
	height = *(buffer + 8) + (*(buffer + 9) << 8);
	FarbCode = *(buffer + 0x0e) + (*(buffer + 0x0f) << 8);

/* Graustufen ? */

	if (FarbCode != 255)
	{
		pal = smurf_struct->smurf_pic->palette;
		for (c = 0; c < 256; c++)
		{
			*(pal++) = c;
			*(pal++) = c;
			*(pal++) = c;
		}
	}
/* nein: Farbpalette inclusive! */

	else
		memcpy(smurf_struct->smurf_pic->palette, buffer + 0x80, 768L);

	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 8;
	smurf_struct->smurf_pic->bp_pal = 24;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	len = (unsigned long) width * (unsigned long) height;

	retbuf = (uint8_t *)Malloc(len);
	if (!retbuf)
		return M_MEMORY;
	memcpy(retbuf, buffer + 0x380, len);
	Mfree(buffer);
	smurf_struct->smurf_pic->pic_data = retbuf;
	return M_PICDONE;
}
