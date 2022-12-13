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
/*                  Pixar-Format Encoder                    */
/* Version 0.1  --  01.08.97                                */
/*  8 oder 24 Bit, unkomprimiertes PIXAR-Format             */
/* =========================================================*/


#include <stdio.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Pixar image",			/* Name of module */
	0x0010,					/* version */
	"Dodger",				/* author */
	/* 10 Extensionen fuer Importer */
	{ "PXR", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	{ "Slider 1", "Slider 2", "Slider 3", "Slider 4" },
	/* 4 checkbox titles */
	{ "Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4" },
	/* 4 edit object titles */
	{ "Edit 1", "Edit 2", "Edit 3", "Edit 4" },
	/* min/max values for slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	/* min/max values for edit objects */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
	/* default values for slider */
	{ 0, 0, 0, 0 },
	/* default values for checkboxes */
	{ 0, 0, 0, 0 },
	/* default values for edit objects */
	{ 0, 0, 0, 0 },
	/* how many pics? */
	0,
	COMPILER_ID,
	/* description for pictures */
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	8, 24, 0, 0, 0, 0, 0, 0,
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
	uint8_t *dest;
	unsigned short width, height;
	long piclen;
	short BPP;
	uint8_t Byte1, Byte2;

	BPP = smurf_struct->smurf_pic->depth;

	switch (smurf_struct->module_mode)
	{
			/*---------------- Export-Start ----------------*/
	case MEXTEND:
		smurf_struct->event_par[0] = 1;
		smurf_struct->module_mode = M_EXTEND;
		return NULL;

	case MCOLSYS:
#if 0
		if (smurf_struct->smurf_pic->depth == 8)
			smurf_struct->event_par[0] = GREY;
		else if (smurf_struct->smurf_pic->depth == 24)
#endif
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

		piclen = (long)width * (long)height * (long)(BPP >> 3) + 1024;	/* Bildl„nge */

		exp_pic = smurf_struct->services->SMalloc(sizeof(EXPORT_PIC));
		if (exp_pic == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}
		exp_pic->f_len = piclen;

		dest = smurf_struct->services->SMalloc(exp_pic->f_len + 1024);
		if (dest == NULL)
		{
			smurf_struct->services->SMfree(exp_pic);
			smurf_struct->module_mode = M_MEMORY;
			return NULL;
		}
		/* Kennung schreiben */
		*(dest) = 0x80;
		*(dest + 1) = 0xE8;
		*(dest + 2) = 0x00;
		*(dest + 3) = 0x00;
		*(dest + 4) = 0x01;

		/*--------------------- Breite und H”he schreiben */
		Byte2 = height & 0x00FF;
		Byte1 = (height & 0xFF00) >> 8;
		*(dest + 0x1A0) = Byte2;
		*(dest + 0x1A1) = Byte1;
		*(dest + 0x1A4) = Byte2;		/* Sowohl H”he als auch Breite sind in allen */
		*(dest + 0x1A5) = Byte1;		/* meinen Bildern zweimal enthalten... */

		Byte2 = width & 0x00FF;
		Byte1 = (width & 0xFF00) >> 8;
		*(dest + 0x1A2) = Byte2;
		*(dest + 0x1A3) = Byte1;
		*(dest + 0x1A6) = Byte2;		/* ...also schreib' ichs auch zweimal rein. */
		*(dest + 0x1A7) = Byte1;

		/*--------------------------- Bits per Pixel: */
		if (BPP == 24)
			*(dest + 0x1A8) = 0x0E;
		else if (BPP == 8)
			*(dest + 0x1A8) = 0x08;

		memcpy(dest + 1024, buffer, piclen);
		exp_pic->pic_data = dest;

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
