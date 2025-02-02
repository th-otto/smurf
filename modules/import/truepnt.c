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
		Smurf Truepaint Loader
*/

#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"Truepaint Format",
	0x0110,
	"Bj�rn Spruck",
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


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		TruepaintFormat (by Sage) 			.TPI	*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	char head[8];
	uint8_t *smbuffer;
	short x, y;

	smbuffer = smurf_struct->smurf_pic->pic_data;
	memcpy(head, smbuffer, 8);

	if (strncmp(head, "TRUP", 4) != 0)
		return M_INVALID;

	x = *((short *) &head[4]);
	y = *((short *) &head[6]);
	if ((long) x * y * 2 + 8 != smurf_struct->smurf_pic->file_len)
		return M_INVALID;

	strcpy(smurf_struct->smurf_pic->format_name, "Truepaint Format");

	memcpy(smbuffer, smbuffer + 8, (long) x * y * 2);
	_Mshrink(smbuffer, (long) x * y * 2);

	smurf_struct->smurf_pic->pic_width = x;
	smurf_struct->smurf_pic->pic_height = y;
	smurf_struct->smurf_pic->depth = 16;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = smbuffer;

	return M_PICDONE;
}
