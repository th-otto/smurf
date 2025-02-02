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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

/*
		Smurf GRA Loader
*/

#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"GRA Format",
	0x0130,
	"Bj�rn Spruck",
	{ "GRA", "", "", "", "", "", "", "", "", "" },
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
/*		GRA Format (by Sage) 				.GRA	*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *smbuffer;
	long x, y;

	if (smurf_struct->smurf_pic->file_len <= 8)
		return (M_INVALID);
	smbuffer = smurf_struct->smurf_pic->pic_data;

	x = *((int32_t *) &smbuffer[0]);
	y = *((int32_t *) &smbuffer[4]);
	if (x * y + 8 != smurf_struct->smurf_pic->file_len)
		return M_INVALID;

	strcpy(smurf_struct->smurf_pic->format_name, "GRA Format");

	{
		uint8_t *f = (uint8_t *) smurf_struct->smurf_pic->palette;
		int i;

		for (i = 0; i < 256; i++)
		{
			*f++ = i;
			*f++ = i;
			*f++ = i;
		}
	}

	memcpy(smbuffer, smbuffer + 8, x * y);
	_Mshrink(smbuffer, x * y);

	smurf_struct->smurf_pic->pic_width = (short) x;
	smurf_struct->smurf_pic->pic_height = (short) y;
	smurf_struct->smurf_pic->depth = 8;
	smurf_struct->smurf_pic->col_format = GREY;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = smbuffer;

	return (M_PICDONE);
}
