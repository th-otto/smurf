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
/*				Inshape Image-Format Decoder				*/
/* Version 0.1  --  25.01.96								*/
/*	  1, 8 und 24 Bit										*/
/*	  Der 2 Minuten Decoder									*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Inshape-Format",
	0x0010,
	"Christian Eyrich",
	{ "IIM", "", "", "", "", "", "", "", "", "" },
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
/*			Inshape Image-Format (IIM)				*/
/*		1, 8 und 24 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t BitsPerPixel;
	unsigned short DatenOffset;
	char dummy[3];
	char impmessag[21];
	unsigned short i, width, height;
	unsigned long len;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (strncmp((char *)buffer, "IS_IMAGE", 8) != 0)
		return M_INVALID;

	BitsPerPixel = *(uint16_t *) (buffer + 0x0a);

	width = *(uint16_t *) (buffer + 0x0c);
	height = *(uint16_t *) (buffer + 0x0e);

	DatenOffset = 0x10;

	strcpy(smurf_struct->smurf_pic->format_name, "Inshape Image-Format .IIM");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Inshape Image ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	ziel = buffer;

	len = (((unsigned long) width * (unsigned long) height) * BitsPerPixel) >> 3;

	memmove(ziel, buffer + DatenOffset, len);

	_Mshrink(ziel, len);

	smurf_struct->smurf_pic->pic_data = ziel;

	if (BitsPerPixel == 1)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	pal = smurf_struct->smurf_pic->palette;

	if (BitsPerPixel == 1)
	{
		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
	} else if (BitsPerPixel == 8)
	{
		for (i = 0; i < 256; i++)
		{
			*pal++ = i;
			*pal++ = i;
			*pal++ = i;
		}
	}

	if (BitsPerPixel == 8)
		smurf_struct->smurf_pic->col_format = GREY;
	else
		smurf_struct->smurf_pic->col_format = RGB;

	return M_PICDONE;
}
