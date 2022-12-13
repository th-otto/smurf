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
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* ******************************************** */
/*                                              */
/*                                              */
/* ******************************************** */


#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


MOD_INFO module_info = {
	"NeoN-Mapfile Import",
	0x0010,
	"Olaf Piesche",
/* Extensionen */
	{ "MAP", "", "", "", "", "", "", "", "", "" },
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






short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *dest;
	uint8_t *pd;
	uint8_t *pal;
	unsigned short width, height, t;

	pd = smurf_struct->smurf_pic->pic_data;
	pal = smurf_struct->smurf_pic->palette;

	/* Kennung prÅfen */
	if (strncmp((char *)pd, "Rip-Mapfile:", 12) != 0)
		return M_INVALID;

	width = *(pd + 18);
	height = *(pd + 20);

	dest = (uint8_t *)Malloc((unsigned long) width * height);
	memcpy(dest, pd + 24, (unsigned long) width * height);

	Mfree(pd);

	for (t = 0; t < 256; t++)
	{
		*(pal + (t * 3) + 0) = t;
		*(pal + (t * 3) + 1) = t;
		*(pal + (t * 3) + 2) = t;
	}

	smurf_struct->smurf_pic->pic_data = dest;
	smurf_struct->smurf_pic->depth = 8;

	strcpy(smurf_struct->smurf_pic->format_name, "NeoN-Mapfile");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	return M_PICDONE;					/* das wars. */
}
