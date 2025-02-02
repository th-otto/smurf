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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* **************************************************** */
/*   SCODL - Importer                                   */
/*  File Format by Agfa Corp.,                          */
/*  Importer by Olaf                                    */
/*                                                      */
/*  Wieder ein Format mit einer Expansions-             */
/*  komprimierung par excellence, bei der die           */
/*  Bilder nach der Komprimierung dreimal so            */
/*  gro� sind, wie vorher...                            */
/*                                                      */
/*  F�r SMURF Bildkonverter,                            */
/*                                                      */
/*  Im Unterricht geschrieben und dabei st�ndig         */
/*  vom Dozenten unterbrochen worden...                 */
/* **************************************************** */


#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"


MOD_INFO module_info = {
	"SCODL-Importer",
	0x0010,
	"Olaf Piesche",
/* Extensionen */
	{ "SCD", "", "", "", "", "", "", "", "", "" },
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






short imp_module_main(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	uint8_t *cpic;
	uint8_t *bitmap_data;
	uint8_t *decoded_pic;
	uint8_t *decpic_copy;
	uint8_t PicByte;
	unsigned short *pic_data;
	uint8_t *redpal;
	uint8_t *greenpal;
	uint8_t *bluepal;
	short width, height, depth;
	unsigned short pictype;
	short pal_offs;
	long decode;
	long PicLen;
	long DecodeLen = 0;
	long t;
	long dect;
	long PlaneLen;
	uint8_t red, green, blue;
	uint8_t *smurf_palette;
	uint8_t *smurf_palcopy;


	picture = smurf_struct->smurf_pic;
	pic_data = picture->pic_data;
	cpic = (uint8_t *) pic_data;
	smurf_palette = picture->palette;
	smurf_palcopy = smurf_palette;


	/* Dateikennung pr�fen */
	if (*(pic_data) != 0xe001U)
		return M_INVALID;



	pictype = *(pic_data + 1);			/* Bildtype */

	if (pictype == 0xf800U)
	{
		depth = 8;						/* 8Bit Palettenbild,     */
		pal_offs = 3 * 256;
	} else if (pictype == 0xdc00U)
	{
		depth = 24;						/* 24Bit TC-Bild,         */
		pal_offs = 0;
	} else
		return M_UNKNOWN_TYPE;		/* oder unbekanntes Bild. */




	redpal = cpic + 0x04;				/* Palettenwerte ROT ab Byte 4 */
	greenpal = cpic + 0x104;			/* Palettenwerte GR�N */
	bluepal = cpic + 0x204;				/* Palettenwerte BLAU */

#if 0
	memtype = *(cpic + pal_offs);		/* Scaleable - Non Scaleable (???) */
#endif

	width = *(cpic + pal_offs + 1);
	height = *(cpic + pal_offs + 3);





	PicLen = (long) width * (long) height * 3L;

	decoded_pic = (uint8_t *)Malloc(PicLen);
	if (decoded_pic == NULL)
		return M_MEMORY;
	decpic_copy = decoded_pic;

	bitmap_data = cpic + 0x0d + pal_offs;




	/* Expansions-Decode (RLE=RunLengthEncoding oder: Reine L�ngen-Expansion...) */
	do
	{
		decode = *(bitmap_data++);		/* RLE-Length */
		PicByte = *(bitmap_data++);		/* RLE-Value  */

		for (dect = 0; dect < decode; dect++)
		{
			*(decpic_copy++) = PicByte;
			DecodeLen++;
		}

	} while (DecodeLen < PicLen);





	/******* 24BIT-IMAGE *********/
	if (depth == 24)
	{
		bitmap_data = decoded_pic;
		PlaneLen = width * height;

		Mfree(pic_data);
		pic_data = (unsigned short *)Malloc(PicLen);
		if (pic_data == NULL)
		{
			Mfree(decoded_pic);
			return M_MEMORY;
		}
		cpic = (uint8_t *) pic_data;

		/* Format-Decode */
		for (t = 0; t < PlaneLen; t++)
		{
			red = *(bitmap_data + t);
			green = *(bitmap_data + t + PlaneLen);
			blue = *(bitmap_data + t + PlaneLen + PlaneLen);
			*(cpic++) = red;
			*(cpic++) = green;
			*(cpic++) = blue;
		}

		Mfree(decoded_pic);
	}


	/******* 8BIT-IMAGE *********/
	else if (depth == 8)
	{
		/* den Speicherblock verk�rzen... */
		PlaneLen = (long) width *(long) height;

		_Mshrink(decoded_pic, PlaneLen);

		Mfree(pic_data);
		pic_data = (unsigned short *) decoded_pic;


		/* Palette eintragen */
		for (t = 0; t < 256; t++)
		{
			*(smurf_palcopy++) = *(redpal)++;
			*(smurf_palcopy++) = *(greenpal)++;
			*(smurf_palcopy++) = *(bluepal)++;
		}

	}


	strcpy(picture->format_name, "Agfa SCODL-File (.SCD)");
	picture->pic_data = pic_data;
	picture->depth = depth;
	picture->pic_width = width;
	picture->pic_height = height;
	picture->col_format = RGB;

	return M_PICDONE;
}
