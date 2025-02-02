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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "smurf_st.h"
#include "smurf_f.h"
#include "nvdi5ras.h"


void nvdi5_raster(SMURF_PIC *picture, char *ziel, short zoom)
{
	char *pal;
	WORD src_rect[4];
	WORD dst_rect[4];
	WORD i;
	WORD cols;
	uint16_t width, height, endwid, endhgt;
	unsigned long scrnformat;
	unsigned long picformat;
	GCBITMAP src_bm;
	GCBITMAP dst_bm;
	COLOR_TAB256 ctab;

	/* src */
	if (picture->format_type == FORM_STANDARD)
		picformat = PX_PLANES;
	else
		picformat = PX_PACKED;

	if (picture->depth <= 8)
		picformat |= PX_1COMP;
	else
		picformat |= PX_3COMP;

	/* PX_USESx | PX_xBIT */
	picformat |= ((long) picture->depth << 8L) | (long) picture->depth;

	width = picture->pic_width;
	height = picture->pic_height;
	endwid = width / (zoom + 1);
	endhgt = height / (zoom + 1);

	if (picture->depth <= 8)
	{
		cols = 1 << picture->depth;

		ctab.magic = 'ctab';			/* Kennung */
		ctab.length = sizeof(COLOR_TAB) + (cols * sizeof(COLOR_ENTRY));	/* L�nge */
		ctab.format = 0;				/* Format 0 */
		ctab.reserved = 0;
		ctab.map_id = v_get_ctab_id(handle);	/* Kennung der Farbtabelle */
		ctab.color_space = CSPACE_RGB;	/* Farbraum */
		ctab.flags = CSPACE_3COMPONENTS;	/* Anzahl der Komponenten angeben */
		ctab.no_colors = cols;			/* Anzahl der Farbeintr�ge */

		ctab.reserved1 = 0;
		ctab.reserved2 = 0;
		ctab.reserved3 = 0;
		ctab.reserved4 = 0;

		pal = picture->palette;
		for (i = 0; i < cols; i++)
		{
			ctab.colors[i].rgb.reserved = 0;
			ctab.colors[i].rgb.red = *pal++ << 8;
			ctab.colors[i].rgb.green = *pal++ << 8;
			ctab.colors[i].rgb.blue = *pal++ << 8;
		}
	}

	src_rect[0] = 0;
	src_rect[1] = 0;
	src_rect[2] = width - 1;
	src_rect[3] = height - 1;

	src_bm.magic = CBITMAP_MAGIC;
	src_bm.length = 64;
	src_bm.format = 0;
	src_bm.reserved = 0;
	src_bm.addr = picture->pic_data;
	if (picture->format_type == FORM_PIXELPAK)
		src_bm.width = (long) width * (long) picture->depth / 8L;
	else
		src_bm.width = (long) (width + 7) / 8L * picture->depth;
	src_bm.bits = picture->depth;
	src_bm.px_format = picformat;
	src_bm.xmin = 0;
	src_bm.ymin = 0;
	src_bm.xmax = width;
	src_bm.ymax = height;
	if (picture->depth <= 8)
		src_bm.ctab = (COLOR_TAB *) &ctab;
	else
		src_bm.ctab = 0;
	src_bm.itab = 0;
	src_bm.reserved0 = 0;
	src_bm.reserved0 = 0;

	/* dst */
	vq_px_format(handle, &scrnformat);

	dst_rect[0] = 0;
	dst_rect[1] = 0;
	dst_rect[2] = endwid - 1;
	dst_rect[3] = endhgt - 1;

	dst_bm.magic = CBITMAP_MAGIC;
	dst_bm.length = 64;
	dst_bm.format = 0;
	dst_bm.reserved = 0;
	dst_bm.addr = ziel;
	dst_bm.width = ((endwid + 15) / 16) * 16 * (bplanes >> 3);
	dst_bm.bits = bplanes;
	dst_bm.px_format = scrnformat;
	dst_bm.xmin = 0;
	dst_bm.ymin = 0;
	dst_bm.xmax = endwid;
	dst_bm.ymax = endhgt;
	dst_bm.ctab = 0;
	dst_bm.itab = 0;
	dst_bm.reserved0 = 0;
	dst_bm.reserved0 = 0;

	vr_transfer_bits(handle, &src_bm, &dst_bm, src_rect, dst_rect, T_REPLACE);
}
