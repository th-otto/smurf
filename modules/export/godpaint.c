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
/*                  GodPaint-Format Encoder                 */
/* Version 0.1  --  01.08.97                                */
/*  Speichert ein 32768-Farben-Bild im GodPaint-Format      */
/* =========================================================*/
/*
    Leon says:
    
        These should have dimensions of 160x144 so the header will look like:
        
        offset     value
        0          G4
        2          160
        4          144
        6          ....gfx data
*/


#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Reservoir Gods GodPaint",
	0x0010,
	"Dodger",
	"GOD", "", "", "", "",
	"", "", "", "", "",
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
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
	short width, height;
	uint16_t *dest;

	switch (smurf_struct->module_mode)
	{
			/*---------------- Export-Start ----------------*/
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
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;

		exp_pic = (EXPORT_PIC *)Malloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		}
		exp_pic->f_len = (unsigned long) width * (unsigned long) height * 2 + 6;

		dest = (uint16_t *)Malloc(exp_pic->f_len);
		if (dest == NULL)
		{
			Mfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}
		*(dest) = 0x4734; /* 'G4' */
		*(dest + 1) = width;
		*(dest + 2) = height;
		memcpy(dest + 3, buffer, (unsigned long) width * (unsigned long) height * 2);
		exp_pic->pic_data = dest;

		Mfree(buffer);

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

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
