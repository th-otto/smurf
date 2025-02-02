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
	".RAW Modul",
	0x0010,
	"Dale Russell",
	{ "", "", "", "", "", "", "", "", "", "" },
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
/*      raw hsi Dekomprimierer                      */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *Daten;
	unsigned short width, height;
	short BitsPerPixel;
	short colors;

	buffer = smurf_struct->smurf_pic->pic_data;
	if (strncmp((char *)buffer, "mhwanh", 6) != 0)
		return M_INVALID;

	width = (*(buffer + 8) << 8) + *(buffer + 9);
	height = (*(buffer + 10) << 8) + *(buffer + 11);
	colors = (*(buffer + 12) << 8) + *(buffer + 13);
	if (colors == 0 || colors > 255)
	{
		BitsPerPixel = 24;
		Daten = buffer + 0x20;
	} else
	{
		BitsPerPixel = 8;
		smurf_struct->smurf_pic->palette = buffer + 0x20;
		smurf_struct->smurf_pic->bp_pal = 24;
		Daten = buffer + 0x20 + (colors + 1) * 3;
	}
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->pic_data = Daten;
	smurf_struct->smurf_pic->depth = BitsPerPixel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	strcpy(smurf_struct->smurf_pic->format_name, "RAW-Hsi File Format");

	return M_PICDONE;
}
