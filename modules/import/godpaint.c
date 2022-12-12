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
 * The Initial Developer of the Original Code is
 * Christian Eyrich
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*                  GodPaint-Format Decoder                 */
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

/* info structure for main module */
MOD_INFO module_info = {
	"GodPaint",				/* Name of module */
	0x0100,					/* version */
	"Dodger",				/* author */
	/* 10 Extensionen fuer Importer */
	{ "GOD", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* 4 checkbox titles */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* 4 edit object titles */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",
	/* min/max values for slider */
	0,128, 0,128, 0,128, 0,128,
	/* min/max values for slider */
	0,10, 0,10, 0,10, 0,10,
	/* default values for edit objects */
	0, 0, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for edit objects */
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

struct header {
	uint16_t magic;
	uint16_t width;
	uint16_t height;
	uint8_t data[];
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				GEM Icon-Format (ICN)				*/
/*		1, 2, 4, 8 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	struct header *pic_data;
	uint16_t width;
	uint16_t height;
	unsigned long datasize;
	
	pic_data = (struct header *)smurf_struct->smurf_pic->pic_data;
	if (pic_data->magic != 0x4734) /* 'G4' */
		return M_INVALID;
	strcpy(smurf_struct->smurf_pic->format_name, "GodPaint");
	width = pic_data->width;
	height = pic_data->height;
	datasize = (unsigned long)width * height * 2;
	memmove(pic_data, pic_data->data, datasize);
	_Mshrink(pic_data, datasize);
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->bp_pal = 0;
	smurf_struct->smurf_pic->depth = 16;
	smurf_struct->smurf_pic->col_format = RGB;

	return M_PICDONE;
}
