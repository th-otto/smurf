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
/*  Speichert ein 256-Farben-Bild im Falcon-Screenformat ab */
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = { "8Bit PP Raw",
	0x0010,
	"Olaf Piesche",
	{ "RA8", "", "", "", "", "", "", "", "", "" },
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
	8, 0, 0, 0, 0, 0, 0, 0,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0
};


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*              8 Bit, unkomprimiert                */
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *palette;
	uint8_t *buffer;
	uint8_t *ziel;
	short width, height;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_WAITING;
		return NULL;

	case MEXEC:
		buffer = smurf_struct->smurf_pic->pic_data;
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		palette = smurf_struct->smurf_pic->palette;
	
		exp_pic = (EXPORT_PIC *)Malloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}	
		ziel = (uint8_t *)Malloc((long) width * (long) height + 256L * 3L);
		if (!ziel)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}

	/*-------------------------- Encoding -----------------------*/
	
		memcpy(ziel, palette, 256L * 3L);
		memcpy(ziel + 256L * 3L, buffer, (long) width * (long) height);
	
		exp_pic->pic_data = ziel;
		exp_pic->f_len = (long) width * (long) height + 256L * 3L;
	
		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

	/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		/* exp_pic wird von smurf freigegeben */
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;
		break;
	}
	return NULL;
}
