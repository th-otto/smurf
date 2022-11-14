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
/*                  Pixar-Format Decoder                    */
/*  8 oder 24 Bit, unkomprimiertes PIXAR-Format             */
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../import.h"
#include "../../src/smurfine.h"

/* info structure for main module */
MOD_INFO module_info = {
	"Pixar Picio Image",	/* Name of module */
	0x0100,					/* version */
	"Dodger",				/* author */
	/* 10 Extensionen fuer Importer */
	{ "PXR", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* 4 checkbox titles */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* 4 edit object titles */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",
	/* min/max values for slider */
	0,128, 0,128, 0,128, 0,128,
	/* min/max values for edit objects */
	0,10, 0,10, 0,10, 0,10,
	/* default values for slider */
	0, 0, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for edit objects */
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				GEM Icon-Format (ICN)				*/
/*		1, 2, 4, 8 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *pic_data;
	uint16_t width;
	uint16_t height;
	unsigned long datasize;
	uint8_t lo, hi;
	short depth;
	short i;
	
	pic_data = smurf_struct->smurf_pic->pic_data;
	if (pic_data[0] != 0x80 ||
		pic_data[1] != 0xe8 ||
		pic_data[2] != 0x00 ||
		pic_data[3] != 0x00 ||
		pic_data[4] != 0x01)
		return M_INVALID;

	strcpy(smurf_struct->smurf_pic->format_name, "Pixar Picio");
	lo = pic_data[0x1a0];
	hi = pic_data[0x1a1];
	height = (hi << 8) | lo;
	lo = pic_data[0x1a2];
	hi = pic_data[0x1a3];
	width = (hi << 8) | lo;
	if (pic_data[0x1a8] == 14)
		depth = 24;
	else if (pic_data[0x1a8] == 8)
		depth = 8;
	else
		return M_UNKNOWN_TYPE;
	datasize = (unsigned long)width * height * (depth >> 3);
	
	memmove(pic_data, pic_data + 1024, datasize);
	_Mshrink(pic_data, datasize);
	if (depth == 24)
	{
		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->bp_pal = 0;
	} else if (depth == 8)
	{
		for (i = 0; i < 256; i++)
		{
			smurf_struct->smurf_pic->palette[i * 3 + 0] = i;
			smurf_struct->smurf_pic->palette[i * 3 + 1] = i;
			smurf_struct->smurf_pic->palette[i * 3 + 2] = i;
		}
		smurf_struct->smurf_pic->col_format = GREY;
	}
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = depth;

	return M_PICDONE;
}
