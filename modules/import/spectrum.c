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

/* ******************************************** */
/*                                              */
/* SPECTRUM 512 - .SPC - Importer               */
/*  Format von: keine Ahnung                    */
/*      Importer von Olaf.                      */
/*                                              */
/*  Spectrum 512-Bilder haben eigentlich 9 Bit  */
/*  m”gliche Farben. Daher werden die Bilder    */
/*  beim Import nach 16Bit gewandelt.           */
/*                                              */
/*  Fertiggeworden am                           */
/*                                              */
/* ******************************************** */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"


/* info structure for main module */
MOD_INFO module_info = {
	"Spectrum 512",
	0x0030,
	"Olaf Piesche, Christian Eyrich",
	/* Extensionen */
	{ "SPU", "SPC", "", "", "", "", "", "", "", "" },

	/* Slider */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* Checkboxen */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* Editfelder */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",

	/* Minima + Maxima */
	/* Slider */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	/* Edits */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};

static short (*busybox)(short lft);

static unsigned short *palettes[200];


#define SPU 1
#define SPC 2
#define SPX 3


static void get_uncompressed(uint16_t *data, uint8_t *pixel)
{
	uint8_t t;
	uint8_t bit;
	
	memset(pixel, 0, 16);
	t = 0;
	do
	{
		bit = 15;
		do
		{
			*pixel++ |= ((*data >> bit) & 1) << t;
		} while (bit-- != 0);
		data++;
		pixel -= 16;
	} while (++t < 4);
}


static void *decompress_SPC(uint8_t *picdata, uint8_t *dest_data, unsigned long compressed_bytes)
{
	unsigned long count;
	int8_t pix;
	unsigned short C;

	count = 0;

	do
	{
		pix = *(picdata++);
		count++;
		if (pix < 0)
		{
			/*------------------------ Compressed Run */
			C = (-pix) + 2;
			pix = *(picdata++);
			count++;
			while (C-- != 0)
			{
				*(dest_data++) = pix;
			}
		} else /* if(pix>=0 && x<=127) */
		{
			/*------------------------ Literal Run */
			C = pix + 1;
			count += C;
			while (C-- != 0)
			{
				*(dest_data++) = *(picdata++);
			}
		}
	} while (count < compressed_bytes);

	return picdata;
}


/*--------------------- Find Color Palette Index--------------*/
static short FindIndex(short x, uint8_t c)
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


static void decode_sp(uint16_t *dest_zerbastel, uint8_t *data, uint8_t type)
{
	unsigned short *CurrentPalette;
	unsigned short t;
	unsigned short palpix;
	uint8_t pixel[16];
	uint8_t C;
	uint16_t x;
	uint16_t red, green, blue;
	uint8_t skip;
	uint16_t y;
	uint16_t ycount;
	short busycounter;
	long planelen;
	
	dest_zerbastel += 320;
	ycount = 19;
	busycounter = 0;
	
	planelen = (320L * 199L) / 8L;

	if (type == SPC)
		skip = 2;
	else
		skip = 8;
	y = 0;
	do
	{
		if ((y % ycount) == 0)
		{
			busybox(busycounter);
			busycounter += 12;
		}
		for (x = 0; x < 320; x += 16)
		{
			if (type == SPC)
				get_standard_pix(data, pixel, 4, planelen);
			else
				get_uncompressed((uint16_t *)data, pixel);
			CurrentPalette = palettes[y];
			t = 0;
			do
			{
				C = pixel[t];
				palpix = *(CurrentPalette + FindIndex(x + t, C));

				/* und nach 16Bit umbauen */
				blue = (palpix & 0x0007) << 2;
				green = (palpix & 0x0070) << 4;
				red = (palpix & 0x0700) << 5;
				*(dest_zerbastel++) = red | green | blue;
			} while (++t < 16);

			data += skip;
		}
	} while (++y < 199);
}


static void decode_palettes(unsigned short *picdata, uint8_t type)
{
	unsigned short t;
	unsigned short bit;
	unsigned short palette_number;
	uint16_t header_word;
	unsigned short *Cpal;

	t = 0;
	do
	{
		Cpal = palettes[t];

		if (type == SPC)
		{
			palette_number = 0;
			do
			{
				header_word = *(picdata++);
	
				bit = 0;
				do
				{
					/* Paletteneintrag vorhanden */
					if (((header_word & (1 << bit)) == 0) || bit == 0 || bit == 15)
						*(Cpal++) = 0;
					else
						*(Cpal++) = *(picdata++);
				} while (++bit < 16);
			} while (++palette_number < 3);
		} else
		{
			memcpy(Cpal, picdata, 16 * 3 * sizeof(palettes[0][0]));
			picdata += 16 * 3;
		}
	} while (++t < 199);
}


short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *picdata;
	uint8_t *dest_data;
	uint8_t *DecodedPic;
	const char *ext;
	uint8_t type;
	unsigned short t;
	long compressed_size;
	long map_offset;
	
	busybox = smurf_struct->services->busybox;

	picdata = smurf_struct->smurf_pic->pic_data;
	ext = smurf_struct->smurf_pic->filename + strlen(smurf_struct->smurf_pic->filename) - 3;
	
	/* Kennung prfen */
	type = 0;
	if (memcmp(picdata, "\0\0\0\0\0\0\0\0\0\0", 10) == 0 &&
		strnicmp(ext, "SPU", 3) == 0)
		type = SPU;
	else if (memcmp(picdata, "SP\0\0", 4) == 0)
		type = SPC;
	else if (strnicmp(picdata, "SPX", 3) == 0)
		type = SPX;
	if (type != SPU && type != SPC && type != SPX)
		return M_INVALID;				/* Kenn ich nicht */
	strcpy(smurf_struct->smurf_pic->format_name, "Spectrum 512 .SP");
	if (type == SPU)
		smurf_struct->smurf_pic->format_name[16] = 'U';
	else if (type == SPC)
		smurf_struct->smurf_pic->format_name[16] = 'C';
	else
		smurf_struct->smurf_pic->format_name[16] = 'X';
	
	smurf_struct->smurf_pic->pic_width = 320;
	smurf_struct->smurf_pic->pic_height = 200;
	smurf_struct->smurf_pic->depth = 16;

	smurf_struct->services->reset_busybox(0, "Spectrum 512");


	/****************************************************************************/
	/*                          Bilddaten Dekomprimieren                        */
	/****************************************************************************/
	dest_data = (uint8_t *)smurf_struct->services->SMalloc(32000L * 4L);
	if (dest_data == NULL)
		return M_MEMORY;

	if (type == SPC)
	{
		compressed_size = *((int32_t *)(picdata + 4));
		map_offset = compressed_size + 12;
		/* FIXME: translate */
		smurf_struct->services->reset_busybox(0, "Bild dekodieren");
		DecodedPic = (uint8_t *)Malloc(31840L);
		if (DecodedPic == NULL)
		{
			smurf_struct->services->SMfree(dest_data);
			return M_MEMORY;
		}
		decompress_SPC(picdata + 12, DecodedPic, compressed_size);
	} else
	{
		compressed_size = 31840L;
		map_offset = 32000L;
		DecodedPic = (uint8_t *)Malloc(31840L);
		if (DecodedPic == NULL)
		{
			smurf_struct->services->SMfree(dest_data);
			return M_MEMORY;
		}
		memcpy(DecodedPic, picdata + 160, 31840L);
	}
	
	/****************************************************************************/
	/* -------------------  Paletten 'dekomprimieren'   ----------------------- */
	/****************************************************************************/

	/* FIXME: translate */
	smurf_struct->services->reset_busybox(0, "Paletten dekodieren");

	if ((palettes[0] = (unsigned short *)Malloc(16 * 3 * sizeof(palettes[0][0] * 200))) == NULL)
	{
		Mfree(DecodedPic);
		smurf_struct->services->SMfree(dest_data);
		return M_MEMORY;
	}
	for (t = 1; t < 199; t++)
		palettes[t] = palettes[0] + 16 * 3 * t;

	decode_palettes((unsigned short *) (picdata + map_offset), type);

	/* FIXME: translate */
	smurf_struct->services->reset_busybox(0, "SP dekodieren");
	decode_sp((uint16_t *)dest_data, DecodedPic, type);

	Mfree(palettes[0]);
	Mfree(DecodedPic);

	smurf_struct->services->SMfree(picdata);						/* Ur-Bilddaten freigeben */
	smurf_struct->smurf_pic->pic_data = dest_data;

	smurf_struct->smurf_pic->col_format = RGB;	/* PP */
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;	/* PP */

	return M_PICDONE;					/* das wars. */
}
