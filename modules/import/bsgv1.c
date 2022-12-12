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

#define NOMEM   ( (void*) 0L )
#define DEBUG           0

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"BSG-Importer",
	0x0010,
	"Dale Russell",
	{ "BSG", "", "", "", "", "", "", "", "", "" },
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
/*      BSG Fontasy Monochrom Format                */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *retbuf;
	short width, height;
	short bperz;
	long check, len;

	/***************************************************************/
	/*      Kopfdaten analysieren                                  */
	/***************************************************************/
	buffer = smurf_struct->smurf_pic->pic_data;
	check = smurf_struct->smurf_pic->file_len;
	check -= 4;
	width = *(buffer) + (*(buffer + 1) << 8);
	height = *(buffer + 2) + (*(buffer + 3) << 8);
	bperz = (width + 7) / 8;
	if ((long) bperz * (long) height != check)
		return M_INVALID;

	smurf_struct->services->reset_busybox(128, "BSG Fontasy 1 Bit");

	len = (long) ((width + 7) / 8) * (long) height;
	strcpy(smurf_struct->smurf_pic->format_name, "Fontasy Bitmap (BSG) ");

	/***************************************************************/
	/*      Strukturen fllen                                      */
	/***************************************************************/
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = 1;
	smurf_struct->smurf_pic->bp_pal = 0;
	retbuf = (uint8_t *)Malloc(len);
	if (retbuf == NULL)
		return M_MEMORY;
	memcpy(retbuf, buffer + 4, len);
	Mfree(buffer);
	smurf_struct->smurf_pic->pic_data = retbuf;
	smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	smurf_struct->smurf_pic->col_format = WURSCHT;
	return M_PICDONE;					/* Alles Klar. */
}
