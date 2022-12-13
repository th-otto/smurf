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

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

struct floyd1624_par {
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
	long not_in_nct;
	WORD *red;
	WORD *green;
	WORD *blue;
	long destwidth;
	long destheight;
	uint8_t *pic;
};

void floyd1624(struct floyd1624_par *par) ASM_NAME("_floyd1624");	/* 24&16-Bit " " mit SysPal */
static short (*seek_nearest_col)(long *par, short maxcol);


DITHER_MOD_INFO dith_module_info = {
	"Fast Diffusion",
	"Olaf Piesche",
	0x0100,								/* Schlumpfine-Version */
	0,									/* Konfigurierbar? */
	COMPILER_ID,
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
	short rgbtab[4 * 256];
	struct floyd1624_par par;
	long planelen;
	long src_planelen;
	short width, height;

#if 0
	short endwid, endhgt;
#endif
	short src_depth;
	short bplanes;
	short t;
	short idx;
	uint8_t *plantab;

	static short const cliptable[100] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31
	};

	WORD *red, *grn, *blu;
	int not_in_nct;
	SMURF_PIC *picture;
	short destwid, desthgt;

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

	seek_nearest_col = dither->services->seek_nearest_col;
	not_in_nct = dither->not_in_nct;

	/*
	 *   Farbtabelle(15 Bit) initialisieren 
	 */
	idx = 0;
	for (t = 0; t < 256; t++)
	{
		rgbtab[idx + 0] = blu[t];
		rgbtab[idx + 1] = grn[t];
		rgbtab[idx + 2] = red[t];
		idx += 4;
	}

	/*
	 *   Variablenvorbereitung
	 */
	pix = (uint8_t *)Malloc(width + 16);
	if (pix == NULL)
		return M_MEMORY;

#if 0
	endwid = width / (dither->zoom + 1);
	endhgt = height / (dither->zoom + 1);
#endif
	pic = picture->pic_data;
	planelen = dither->dest_planelength;

	if (dither->w == 0)
		destwid = width;
	else
		destwid = dither->w;
	if (dither->h == 0)
		desthgt = height;
	else
		desthgt = dither->h;

	/*  Source-Zeiger auf den Ausschnittsbeginn ausrichten
	 */
	if (picture->format_type == FORM_PIXELPAK)
		pic += ((unsigned long) dither->x * (src_depth >> 3)) + ((unsigned long) dither->y * width * (src_depth >> 3));
	else
		pic += (dither->x >> 3) + ((unsigned long) dither->y * ((width + 7) >> 3));

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
	par.not_in_nct = not_in_nct;
	par.red = red;
	par.green = grn;
	par.blue = blu;
	par.destwidth = destwid;
	par.destheight = desthgt;
	par.pic = pic;

	dither->services->reset_busybox(0, "FDiff Dither");
	floyd1624(&par);

	Mfree(pix);

	return M_PICDONE;
}
