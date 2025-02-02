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
	COMPILER_ID,
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
	unsigned short width, height;
	long len, piclen;

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

	piclen = (unsigned long) width * (unsigned long) height * 2;

	/* Bild dekodieren (Ha, Ha, Ha!) */
	memmove(buffer, buffer + 128, piclen);

	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->depth = 16;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	return M_PICDONE;
}
