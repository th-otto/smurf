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

/* ******************************************** */
/*                                              */
/* SPECTRUM 512 - .SPC - Importer               */
/*  Format von: keine Ahnung                    */
/*      Importer von Olaf.                      */
/*                                              */
/*  Spectrum 512-Bilder haben eigentlich 9 Bit  */
/*  m�gliche Farben. Daher werden die Bilder    */
/*  beim Import nach 16Bit gewandelt.           */
/*                                              */
/*  Fertiggeworden am                           */
/*                                              */
/* ******************************************** */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../import.h"
#include "../../src/smurfine.h"


/* info structure for main module */
MOD_INFO module_info = {
	"Spectrum 512-Importer",
	0x0010,
	"Olaf Piesche",
	/* Extensionen */
	{ "SPC", "SPU", "", "", "", "", "", "", "", "" },

	/* Slider */
	"", "", "", "",
	/* Editfelder */
	"", "", "", "",
	/* Checkboxen */
	"", "", "", "",

	/* Minima + Maxima */
	/* Slider */
	0, 0,
	0, 0,
	0, 0,
	0, 0,
	/* Edits */
	0, 0,
	0, 0,
	0, 0,
	0, 0,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};



unsigned short *palettes[200];



static short FindIndex(short x, short c);
static void *decompress_SPC(uint8_t *picdata, uint8_t *dest_data, long decompressed_bytes);
static void decode_palettes(unsigned short *picdata);


short imp_module_main(GARGAMEL * smurf_struct)
{
	uint8_t *picdata;
	uint8_t *pdcopy;
	uint8_t *dest_data;
	uint8_t *DecodedPic;
	uint8_t *EndPic;
	uint16_t *dest_zerbastel;
	unsigned short *CurrentPalette;
	uint8_t *CompressedPalette;
	short t;
	unsigned short palpix;
	uint8_t pixel[17];
	uint16_t C;
	uint16_t x;
	uint16_t y;
	uint16_t red, green, blue;
	long planelen;

	void (*reset_busybox)(short lft, const char *txt);

	reset_busybox = smurf_struct->services->reset_busybox;

	pdcopy = picdata = smurf_struct->smurf_pic->pic_data;

	/* Kennung pr�fen */
	if (memcmp(picdata, "SP\0\0", 4) == 0)
		;
	else
		return M_INVALID;				/* Kenn ich nicht */

	reset_busybox(0, "Spectrum 512 C");


	/****************************************************************************/
	/*                          Bilddaten Dekomprimieren                        */
	/****************************************************************************/
	dest_data = (uint8_t *)Malloc(32000L * 2L);
	DecodedPic = dest_data;

	picdata += 12;

	picdata = decompress_SPC(picdata, dest_data, 31840);


	/****************************************************************************/
	/* -------------------  Paletten 'dekomprimieren'   ----------------------- */
	/****************************************************************************/
	CompressedPalette = picdata;

	reset_busybox(0, "Paletten decodieren");
	decode_palettes((unsigned short *) CompressedPalette);

	Mfree(pdcopy);						/* Ur-Bilddaten freigeben */




	picdata = DecodedPic;				/* f�rs Zerbasteln auf dekomprimiertes Bild legen */
	EndPic = (uint8_t *)Malloc(320L * 199L * 2L);	/* und Speicher f�rs Zerbasteln anfordern */
	dest_zerbastel = (uint16_t *) EndPic;

	planelen = (320L * 199L) / 8L;

	reset_busybox(0, "SPC zerbasteln");


	for (y = 0; y < 199; y++)
	{
		for (x = 0; x < 320; x += 16)
		{
			memset(pixel, 0, 16);

			get_standard_pix(picdata, pixel, 4, planelen);

			for (t = 0; t < 16; t++)
			{
#if 0
				pal = CurrentPalette + (EinePalette * FindIndex(x + t, C));
				palpix = *(pal + pixel[t]);
#endif

				C = pixel[t];
				CurrentPalette = palettes[y];
				palpix = *(CurrentPalette + FindIndex(x + t, C));

				/* und nach 16Bit umbauen */
				blue = (palpix & 0x0003) << 2;
				green = (palpix & 0x0030) << 3;
				red = (palpix & 0x0300) << 2;
				*(dest_zerbastel++) = red | green | blue;
			}

			picdata += 2;
		}
	}

	Mfree(DecodedPic);
	for (t = 0; t < 199; t++)
		free(palettes[t]);


	smurf_struct->smurf_pic->pic_data = EndPic;
	smurf_struct->smurf_pic->depth = 16;	/*16 */

	strncpy(smurf_struct->smurf_pic->format_name, "Spectrum 512 SPC     ", 21);
	smurf_struct->smurf_pic->pic_width = 320;
	smurf_struct->smurf_pic->pic_height = 199;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;	/* PP */

	return M_PICDONE;					/* das wars. */
}




/*--------------------- Find Color Palette Index--------------*/
static short FindIndex(short x, short c)
{
	short x1;

	x1 = 10 * c;

	if (1 & c)							/* If c is odd */
		x1 = x1 - 5;
	else								/* If c is even */
		x1 = x1 + 1;

	if (x >= x1 && x < x1 + 160)
		c = c + 16;
	else if (x >= x1 + 160)
		c = c + 32;

	return c;
}





static void *decompress_SPC(uint8_t *picdata, uint8_t *dest_data, long decompressed_bytes)
{
	long count;
	int8_t pix;
	short C;
	short t;

	count = 0;

	do
	{
		pix = *(picdata++);
		count++;
		if ((int) pix >= -128 && (int) pix <= -1)
		{
			/*------------------------ Compressed Run */
			C = (-pix) + 2;
			pix = *(picdata++);
			for (t = 0; t < C; t++)
			{
				*(dest_data++) = pix;
				count++;
			}
		} else /* if(pix>=0 && x<=127) */
		{
			/*------------------------ Literal Run */
			C = pix + 1;
			for (t = 0; t < C; t++)
			{
				count++;
				*(dest_data++) = *(picdata++);
			}
		}
	} while (count < decompressed_bytes);

	return picdata;
}




static void decode_palettes(unsigned short *picdata)
{
	short t;
	short bit;
	short palette_number;
	uint16_t header_word;
	uint16_t counter;
	unsigned short *Cpal;

	for (t = 0; t < 199; t++)
	{
		palettes[t] = malloc(97);
		counter = 0;
		Cpal = palettes[t];

		for (palette_number = 0; palette_number < 3; palette_number++)
		{
			header_word = *(picdata++);

			bit = 0;

			do
			{
				/* Paletteneintrag vorhanden */
				if (((header_word & (1 << bit)) == 0) || bit == 15 /*|| bit==0 */ )
					*(Cpal++) = 0;
				else
					*(Cpal++) = *(picdata++);

				counter++;
			} while (++bit < 16);
		}
	}
}
