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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

struct nearestcolor_par {
	const short *cliptable;
	uint8_t *nct;
	uint8_t *planetable;
	uint8_t *pix;
	short *rgbtab;
	long planelen;
	uint8_t *where_to;
	long width;
	long height;
	long bplanes;
	short (*busybox)(short lft);
	long src_depth;
	uint8_t *palette;
	long src_planelen;
	long zoom;
	short (*set_16pixels)(uint8_t *buf16, uint8_t *dest, short depth, long planelen, short howmany);
	long SkipBytes;
	long destwidth;
	long destheight;
	uint8_t *pic;
};

void nearest_color(struct nearestcolor_par *par) ASM_NAME("_nearest_color");			/* 24&16-Bit " " mit SysPal */

DITHER_MOD_INFO dith_module_info = {
	"Nearest Color",
	"Olaf Piesche",
	0x0100,								/* Schlumpfine-Version */
	0,									/* Konfigurierbar? */
	ALLPAL								/* Palettenmodi */
};



/*  --------------------------------------------------------------  */
/*  --------------------- 8-24-Bit Bild -------------------------   */
/*  --------------------------------------------------------------  */
short dither_module_main(DITHER_DATA * dither)
{
	uint8_t *pix;
	uint8_t *pic;
	uint8_t *nct;
	WORD *red, *grn, *blu;
	short rgbtab[256 * 4];
	struct nearestcolor_par par;
	long planelen, src_planelen;
	short width, height;
	short destwid, desthgt;
	short src_depth;
	short bplanes;
	short t;
	long SkipBytes;
	uint8_t *plantab;

	static short const cliptable[100] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31
	};

	SMURF_PIC *picture;

	picture = dither->picture;

	width = picture->pic_width;
	height = picture->pic_height;
	src_depth = picture->depth;
	bplanes = dither->dst_depth;
	red = dither->red;
	grn = dither->green;
	blu = dither->blue;
	plantab = dither->planetable;
	nct = dither->nc_tab;

	/*
	 * Farbtabelle(15 Bit) initialisieren
	 */
	memcpy(rgbtab, blu, 256 * sizeof(*blu));
	memcpy(rgbtab + 256, grn, 256 * sizeof(*grn));
	memcpy(rgbtab + 512, red, 256 * sizeof(*red));

	/*
	 * 15Bit-rgbtable anpassen
	 *   Um im geditherten Bild ein reines Weiž zu kriegen,
	 *   darf Weiž in der rgbtab nicht ganz Weiž sein - Antipersil sozusagen
	 */
	for (t = 0; t < 256; t++)
	{
		if (rgbtab[t] > 31)
			rgbtab[t]--;
		if (rgbtab[t + 256] > 31)
			rgbtab[t + 256]--;
		if (rgbtab[t + 512] > 31)
			rgbtab[t + 512]--;
	}

	/*
	 * Variablenvorbereitung
	 */
	if (dither->w == 0)
		destwid = width;
	else
		destwid = dither->w;
	if (dither->h == 0)
		desthgt = height;
	else
		desthgt = dither->h;

	pic = picture->pic_data;
	planelen = dither->dest_planelength;

	pix = (uint8_t *)Malloc(width + 20);
	if (pix == NULL)
		return M_MEMORY;

	/*  Source-Skipbytes (fr Ausschnitte) berechnen
	 */
	if (dither->w == 0)
	{
		SkipBytes = 0;
	} else
	{
		if (picture->format_type == FORM_PIXELPAK)
			SkipBytes = (width - dither->w) * (src_depth / 8);
	}


	/*  Source-Zeiger auf den Ausschnittsbeginn ausrichten
	 */
	if (picture->format_type == FORM_PIXELPAK)
		pic += ((unsigned long) dither->x * (src_depth >> 3)) + ((unsigned long) dither->y * width * (src_depth >> 3));
	else
		pic += (dither->x >> 3) + ((unsigned long) dither->y * ((width + 7) >> 3));

	/*  L„nge einer SRC-Plane in Bytes (Bytegrenzen)
	 */
	src_planelen = 0;
	if (picture->format_type == FORM_STANDARD)
		src_planelen = ((unsigned long) (width + 7) >> 3) * height;

	par.cliptable = &cliptable[31];
	par.nct = nct;
	par.planetable = plantab;
	par.pix = pix;
	par.rgbtab = rgbtab;
	par.planelen = planelen;
	par.where_to = dither->dest_mem;
	par.width = width;
	par.height = height;
	par.bplanes = bplanes;
	par.busybox = dither->services->busybox;
	par.src_depth = src_depth;
	par.palette = picture->palette;
	par.src_planelen = src_planelen;
	par.zoom = picture->zoom;
	par.set_16pixels = dither->set_16pixels;
	par.SkipBytes = SkipBytes;
	par.destwidth = destwid;
	par.destheight = desthgt;
	par.pic = pic;

	dither->services->reset_busybox(0, "Nearest Color");
	nearest_color(&par);

	Mfree(pix);

	return M_PICDONE;
}
