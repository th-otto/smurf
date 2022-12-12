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
/*                  	HSI File-Format                 	*/
/* Version 0.1  --  12.01.96                                */
/*	  xxx													*/
/*==========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"HSI File-Format",
	0x0010,
	"Christian Eyrich",
	{ "RAW", "", "", "", "", "", "", "", "", "" },
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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				HSI File-Format (RAW)				*/
/*		8, 24 Bit, keine Kodierung					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t *ppal;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	uint8_t cols;
	char dummy[3];
	char impmessag[21];
	unsigned short i, width, height;
	long len;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (strncmp((char *)buffer, "mhwanh", 6) != 0)
		return M_INVALID;

	if (*(uint16_t *) (buffer + 0x0c) > 256 || *(uint16_t *) (buffer + 0x0c) == 0)
		BitsPerPixel = 24;
	else
		BitsPerPixel = 8;

	width = *(uint16_t *) (buffer + 0x08);
	height = *(uint16_t *) (buffer + 0x0a);

	if (BitsPerPixel == 8)
		cols = 255;
	else
		cols = 0;

	DatenOffset = 0x20 + cols * 3;

	strcpy(smurf_struct->smurf_pic->format_name, "HSI File-Format .RAW");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	strcpy(impmessag, "HSI Raw ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel != 24)
	{
		pal = smurf_struct->smurf_pic->palette;
		ppal = buffer + 0x20;
		for (i = 0; i < cols; i++)
		{
			*pal++ = *ppal++;
			*pal++ = *ppal++;
			*pal++ = *ppal++;
		}
	}								/* Palette */

	ziel = buffer;

	len = ((unsigned long) width * (unsigned long) height * BitsPerPixel) >> 3;

	memmove(ziel, buffer + DatenOffset, len);

	_Mshrink(ziel, len);

	smurf_struct->smurf_pic->pic_data = ziel;

	return M_PICDONE;
}
