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

/* --------------------------------------------------*/
/* --------------------------------------------------*/
/* --------------------------------------------------*/
/* --------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"TRP Graphix-Importer v0.1",
	0x0010,
	"Dale Russell",
	{ "TRP", "", "", "", "", "", "", "", "", "" },
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

/***********************************************************/
/*  Header                                                 */
/***********************************************************/
typedef struct
{
	char magic[8];						/* Erkennung: 'TRP_GFX!' */
	uint8_t type;						/* Typ(Bitfield; Gesetzt: vorhanden;
										   Gel”scht: nicht vorhanden) 
										   Bit 0: Bitmap enthalten
										   Bit 1: Animation enthalten
										   Bit 2: Vektorgraphik enthalten  */
	int32_t bitmap;						/* Offset auf Bildstruktur */
	int32_t animation;					/* Offset auf Animationsstruktur */
	int32_t vector;						/* Offset auf Vektorstruktur */
} trp_header;

/***********************************************************/
/*  Bildbeschreibungsstruktur                              */
/***********************************************************/
typedef struct
{
	int32_t data;						/* Offset auf Bilddaten,
										   relativ zum Dateianfang */
	int32_t width;						/* Bildbreite in Pixel */
	int32_t height;						/* Bildh”he in Pixel */
	uint8_t depth;						/* Farbtiefe in Bits */
	uint8_t planes;						/* Anzahl der Farbebenen */
	uint8_t mode;						/* Kompressionsart */
	uint8_t incoming;					/* current Bitstream(0) oder 
										   interlaced(1) */
	uint8_t coltable;					/* Farbpalette (0=Nein; 1=Ja; 
										   2=Graufstufen linear, 
										   nicht enthalten) */
	uint8_t table_depth;				/* Palettenfarbtiefe */
	int16_t colors;						/* Anzahl der Farben in der Palette */
	int32_t tableoffset;				/* Offset der Palettendaten,
										   relativ zum Dateianfang */
	uint8_t colormodel;					/* Farbmodell: (0=RGB; 1=YCbCr;
										   2=CMYK; 3=YUV; 4=YIQ) */
	char text[64];						/* Text, z.B. zu Beschreibung ect. */
} bitmap_struct;


short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *input;
	uint8_t *output;
	uint8_t *retbuf;
	uint8_t *GetBuf;
	long xc, yc, len, gox, goy, calc;
	short x, c, ID_Run, MaxRun;
	uint8_t r, g, b, y, cr;
	uint8_t cb;

	bitmap_struct bitmap;

	input = smurf_struct->smurf_pic->pic_data;

	if (strncmp((char *)input, "TRP_GFX!", 8))
		return M_INVALID;
	memcpy(&bitmap, input + sizeof(trp_header), sizeof(bitmap_struct));
	if (bitmap.coltable)
	{
		memcpy(smurf_struct->smurf_pic->palette,
			   input + (bitmap.tableoffset), (long) (bitmap.colors) * (long) (bitmap.table_depth / 8));
	}
	if (bitmap.colormodel == 0)
		smurf_struct->smurf_pic->col_format = RGB;
	if (bitmap.depth >= 8)
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	else
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;

	len = bitmap.width * bitmap.height * (long) (bitmap.depth / 8);
	output = retbuf = (uint8_t *)Malloc(len);
	if (!retbuf)
	{
		return M_MEMORY;
	}
	GetBuf = input + (bitmap.data);
	goy = bitmap.height;
	gox = bitmap.width;

	/* Dynamic RLE (c)Coke of Therapy */

	if (bitmap.mode == 1 && bitmap.depth == 8)
	{
		xc = 0;
		yc = 0;
		ID_Run = *(GetBuf++);
		MaxRun = 0xff - ID_Run;
		while (yc < goy)
		{
			for (;;)
			{
				x = *(GetBuf++);
				if (x >= ID_Run)
				{
					x &= MaxRun;
					xc += x;
					c = *(GetBuf++);
					while (x--)
						*(output++) = c;
				} else
				{
					xc++;
					*(output++) = x;
				}

				if (xc >= gox)
				{
					xc -= gox;
					yc++;
					break;
				}
			}
		}

		Mfree(input);
		smurf_struct->smurf_pic->pic_data = retbuf;
	} else if (bitmap.depth == 24)
	{
		/****************************************************************/
		/*      24 Bit Y,Cb,Cr RLE                                      */
		/****************************************************************/
		yc = 0;
		while (yc++ < goy)
		{
			xc = 0;
			while (xc++ < gox)
			{
				y = *(GetBuf++);
				cb = *(GetBuf++);
				cr = *(GetBuf++);
				cb = cr = 0;
				calc = ((long) cr * 1402l) / 1000 + (long) y;
				r = (uint8_t) calc;
				calc = ((long) cb * 1772l) / 1000 + (long) y;
				b = (uint8_t) calc;
				calc = (1000l * (long) y - 114l * (long) b - 299000l * (long) r) / 587;
				g = (uint8_t) calc;
				*(output++) = r;
				*(output++) = g;
				*(output++) = b;
			}
		}
		Mfree(input);
		smurf_struct->smurf_pic->pic_data = retbuf;
	}
	strcpy(smurf_struct->smurf_pic->format_name, "Therapy Graphix(TRP)");
	smurf_struct->smurf_pic->pic_width = (short) bitmap.width;
	smurf_struct->smurf_pic->pic_height = (short) bitmap.height;
	smurf_struct->smurf_pic->depth = bitmap.depth;

	return M_PICDONE;
}
