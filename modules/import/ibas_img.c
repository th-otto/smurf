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
/*					Kontron IBAS-Image Decoder				*/
/* Version 0.1  --  11.01.96								*/
/*	  24 Bit												*/
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"IBAS Image-Modul",
	0x0010,
	"Christian Eyrich",
	{ "IMG", "", "", "", "", "", "", "", "", "" },
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



static void nulltospace(uint8_t *string, unsigned short length)
{
	while (--length)
	{
		if (*string == '\0')
			*string = ' ';
		string++;
	}
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*      Kontron IBAS-Image Dekomprimierer (IMG)		*/
/*		8 Bit: Graustufen							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *pal;
	uint8_t BitsPerPixel;

	unsigned short i, width, height;
	unsigned short DatenOffset;

	long len;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (*(uint32_t *) (buffer + 2) != 0x47126db0L || *(uint16_t *) (buffer + 14) != 0x00)
		return M_INVALID;

	width = *(buffer + 0x06) + (*(buffer + 0x07) << 8);
	height = *(buffer + 0x08) + (*(buffer + 0x09) << 8);

	DatenOffset = 0x80;

	BitsPerPixel = 8;

	nulltospace(buffer + 0x10, 112);
	strncpy(smurf_struct->smurf_pic->infotext, (char *)buffer + 0x10, sizeof(smurf_struct->smurf_pic->infotext) - 1);
	strcpy(smurf_struct->smurf_pic->format_name, "Kontron IBAS-Image .IMG");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	smurf_struct->services->reset_busybox(128, "Kontron IBAS 8 Bit");

	/* Klaus erz„hlt was von linear fllen, leider wird so */
	/* TOPSECRK.IMG nur schwarz angezeigt, da Farbindex 1 = fast schwarz */

	pal = smurf_struct->smurf_pic->palette;
	for (i = 0; i < 256; i++)
	{
		*pal++ = i;
		*pal++ = i;
		*pal++ = i;
	}

	smurf_struct->smurf_pic->col_format = RGB;

	ziel = buffer;

	len = (unsigned long) width * height;

	memmove(ziel, buffer + DatenOffset, len);

	_Mshrink(ziel, len);

	smurf_struct->smurf_pic->pic_data = ziel;

	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;

	return M_PICDONE;
}
