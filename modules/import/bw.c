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
		Smurf B&W 256 Loader
*/

#include "country.h"

#if COUNTRY==1
#define fehler "[1][Dieses Bild scheint defekt zu sein.|Trotzdem laden?][Ja|Nein]"
#elif COUNTRY==0
#define fehler "[1][Picture seems to be corupted.|Load it anyway?][Yes|No]"
#elif COUNTRY==2
#define fehler "[1][Picture seems to be corupted.|Load it anyway?][Yes|No]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"BW Format",
	0x0130,
	"Bj”rn Spruck",
	{ "B_W", "", "", "", "", "", "", "", "", "" },
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
/*		BW Format (by Sage) 				.B_W	*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *smbuffer;
	unsigned short x, y;
	unsigned short defekt = 0;

	if (smurf_struct->smurf_pic->file_len <= 10)
		return M_INVALID;
	smbuffer = smurf_struct->smurf_pic->pic_data;
	if (strncmp((char *)smbuffer, "B&W256", 6))
		return M_INVALID;

	x = *((uint16_t *) &smbuffer[6]);
	y = *((uint16_t *) &smbuffer[8]);
	if (x == 0 || y == 0)
		return M_INVALID;
	if ((long) x * y + 10 != smurf_struct->smurf_pic->file_len)
	{
		if (form_alert(1, fehler) == 2)
			return M_INVALID;
		defekt = 1;
	}

	strcpy(smurf_struct->smurf_pic->format_name, "B&W256 Format");

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

	if (defekt == 0)
	{
		memcpy(smbuffer, smbuffer + 10, (long) x * y);
		_Mshrink(smbuffer, (long) x * y);
	} else
	{
		if ((long) x * y + 10 < smurf_struct->smurf_pic->file_len)
		{
			memmove(smbuffer, smbuffer + 10, (long) x * y);
			_Mshrink(smbuffer, (long) x * y);
		} else
		{
			void *z;

			z = (void *)Malloc((long) x * y);
			if (z == 0)
				return M_MEMORY;
			memset(z, 0, (long) x * y);
			memcpy(z, smbuffer + 10, smurf_struct->smurf_pic->file_len - 10);
			Mfree(smbuffer);
			smbuffer = z;
		}
	}

	smurf_struct->smurf_pic->pic_width = x;
	smurf_struct->smurf_pic->pic_height = y;
	smurf_struct->smurf_pic->depth = 8;
	smurf_struct->smurf_pic->col_format = GREY;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = smbuffer;

	return M_PICDONE;
}
