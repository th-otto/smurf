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

/* =========================================================*/
/*                  Falcon-Raw-Format Encoder               */
/* Version 0.1  --  25.05.96                                */
/*  Speichert ein 32768-Farben-Bild im Falcon-Screenformat  */
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"16Bit Falcon Raw",
	0x0010,
	"Dodger",
	{ "DAT", "", "", "", "", "", "", "", "", "" },
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


MOD_ABILITY module_ability = {
	16, 0, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	0,
};





/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*              16 Bit TRP-Format                   */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;

	/*---------------- Export-Start ----------------*/
	switch (smurf_struct->module_mode)
	{
	case MEXTEND:
		smurf_struct->event_par[0] = 1;
		smurf_struct->module_mode = M_EXTEND;
		return NULL;

	case MCOLSYS:
		smurf_struct->event_par[0] = RGB;
		smurf_struct->module_mode = M_COLSYS;
		return NULL;

	case MSTART:
		smurf_struct->module_mode = M_WAITING;
		return NULL;

	case MEXEC:
		buffer = smurf_struct->smurf_pic->pic_data;
		exp_pic = (EXPORT_PIC *)Malloc(sizeof(EXPORT_PIC));

		exp_pic->pic_data = buffer;
		exp_pic->f_len = (long) smurf_struct->smurf_pic->pic_width * (long) smurf_struct->smurf_pic->pic_height * 2L;

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}

	return NULL;
}
