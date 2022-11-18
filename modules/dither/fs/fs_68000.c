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

/*------------------------------------------------------------- */
/*                          FS_68000.C                          */
/*  Enth�lt die Floyd-Steinberg-C-Routinen f�r 68000-Prozessor  */
/*------------------------------------------------------------- */
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "import.h"
#include "smurfine.h"
#include "fs.h"

static void (*read_pixel)(void);


static short r, g, b;
static uint8_t *input;
static uint8_t *picdat;
static uint8_t *pic_pal;
static uint8_t *nc_table;
static uint8_t *plane_table;
short st_zoom;							/* wird in Standardformat-Assemblercode importiert! */





static void read_24_68000(void)
{
	r = *input++;
	g = *input++;
	b = *input++;
}


static void read_8_68000(void)
{
	uint8_t picpixel;
	uint8_t *picpal;

	picpixel = *input++;
	picpal = pic_pal + (picpixel + picpixel + picpixel);
	r = picpal[0];
	g = picpal[1];
	b = picpal[2];
}


static void read_16_68000(void)
{
	uint16_t pixel16;

#ifdef __PUREC__
	pixel16 = *((uint16_t *) input)++;
#else
	pixel16 = *((uint16_t *) input);
	input += 2;
#endif
	r = (pixel16 >> 11) << 3;
	g = ((pixel16 & 0x7c0) >> 6) << 3;
	b = (pixel16 & 0x1f) << 3;
}


/************************************************************/
/*      Hier haben wir ein sehr seltenes Exemplar:          */
/*      Einen Floyd-Steinberg Ditherus-totalus              */
/************************************************************/
short floyd_steinberg68000(SMURF_PIC *picture, DITHER_DATA *dither, uint8_t *output)
{
	int16_t *fehler;
	int16_t *fehler_u_ru;
	uint8_t *f_u;
	uint8_t *f_ru;

	int16_t *fehler_r;
	int16_t *fru_acc;
	int8_t *fehlerline1;
	int8_t *fehlerline2;
	int8_t *f1;
	int8_t *f2;
	int8_t *read_f1;
	int16_t *write_f1;
	int8_t rot_rechts, grn_rechts, blu_rechts;
	int8_t *swap;
	int16_t nct_index;
	short ClipTable[3 * 256];
	short xpalette[3 * 256];
	short *destpal;
	uint8_t *actual_pic;
	short *clipaccess;
	short nidx;
	int16_t x, y, t;
	int16_t red_diff, grn_diff, blu_diff;
	uint16_t dest_planes;
	uint16_t width, height, owidth;
	uint16_t destwidth, destheight;
	long planelen;
	long doublineoff, linelen;
	unsigned long planelen8;
	short zoom, bpp;
	long xscale, yscale;
	long par[10];
	uint8_t *line;
	uint8_t *line_ptr;

	/*
	 * Strukturen extrahieren 
	 */
	owidth = width = picture->pic_width;
	height = picture->pic_height;
	picdat = input = picture->pic_data;

	nc_table = dither->nc_tab;
	plane_table = dither->planetable;
	dest_planes = dither->dst_depth;
	zoom = dither->zoom;

	if (dither->w == 0)
		destwidth = width;
	else
		destwidth = dither->w;
	if (dither->h == 0)
		destheight = height;
	else
		destheight = dither->h;

	/* Quellzeiger auf Ausschnitt ausrichten
	 */
	if (picture->format_type == FORM_PIXELPAK)
		picdat += ((unsigned long) dither->x * (picture->depth >> 3)) + ((unsigned long) dither->y * width * (picture->depth >> 3));
	else
		picdat += (dither->x / 8) + (dither->y * ((width + 7) / 8));

	/*
	 * Palette mit 15 Bit f�r die Farbsuchroutine
	 */
	par[0] = (long) dither->red;
	par[1] = (long) dither->green;
	par[2] = (long) dither->blue;
	for (t = 0; t < 256; t++)
	{
		xpalette[t + 0] = dither->blue24[t];
		xpalette[t + 256] = dither->green24[t];
		xpalette[t + 512] = dither->red24[t];
	}

	pic_pal = picture->palette;
	actual_pic = picture->pic_data;
	planelen8 = (long) ((unsigned long) ((width + 7) >> 3) * height);	/* L�nge einer Plane in Bytes (Bytegrenzen) */

	planelen = dither->dest_planelength;

	if (picture->depth >= 8)
		bpp = picture->depth >> 3;

	if (picture->format_type == FORM_PIXELPAK)
		linelen = (width * picture->depth) >> 3;
	else
		linelen = (width + 7) / 8;


	/*
	 * Reservieren und Initialisieren der Fehlerpuffer 
	 *  Es existieren 2 Fehlerpuffer zum Schreiben und auslesen, die nach jeder Zeile
	 *  getauscht werden. Geschrieben wird immer Wordweise, und zwar in der Form 
	 *  Ru-Rru-Gu-Gru-Bu-Bru... (unten-rechtsunten-...). Das Auslesen erfolgt Byteweise.
	 */
	doublineoff = (unsigned long) width * 3L * 3L;

	f1 = (int8_t *)Malloc(doublineoff);
	f2 = (int8_t *)Malloc(doublineoff);

	if (f1 == NULL)
		return M_MEMORY;
	if (f2 == NULL)
	{
		Mfree(f1);
		return M_MEMORY;
	}

	memset(f1, 0, doublineoff);
	memset(f2, 0, doublineoff);

	fehlerline1 = f1 + 16;
	fehlerline2 = f2 + 16;


	/*
	 *  Clip-Tabelle
	 */
	for (t = 0; t < 256; t++)
		ClipTable[t] = 0;
	for (t = 0; t < 256; t++)
		ClipTable[t + 256] = t;
	for (t = 0; t < 256; t++)
		ClipTable[t + 256 + 256] = 255;
	clipaccess = ClipTable + 256;


	/*
	 * Fehlertabellen initialisieren
	 *  fehler_u_ru enth�lt die Fehlerskalierung f�r die Fehler unten und rechts unten
	 *  im Format Ru-Rru-Gu-Gru-Bu-Bru... (je ein Byte), so da� Int-weise aus fehler_u_ru in
	 *  den Fehlerpuffer geschrieben werden kann.
	 */
	if ((fehler = (int16_t *)Malloc(1600 * sizeof(*fehler))) == NULL)
		return M_MEMORY;

	for (t = 0; t < 512; t++)
		fehler[t] = (t - 256) * 7 >> 4;
	fehler_r = fehler + 256;

	fehler_u_ru = (int16_t *)Malloc(1200);
	f_u = (uint8_t *) fehler_u_ru;
	f_ru = (uint8_t *) fehler_u_ru + 1;

	for (t = 0; t < 512; t++)
	{
		*(f_u++) = (t - 256) * 5 >> 4;
		*(f_ru++) = (t - 256) * 3 >> 4;
		f_u++;
		f_ru++;
	}
	fru_acc = fehler_u_ru + 256;


	/*
	 * Scaling
	 */
	yscale = (unsigned long) zoom * width * bpp;
	xscale = (unsigned long) zoom * bpp;

	if (picture->format_type == FORM_STANDARD)
	{
		xscale = zoom;
		yscale = (unsigned long) zoom * ((width + 7) >> 3);
	}

	destheight /= (zoom + 1);
	destwidth /= (zoom + 1);

	/* 
	 * Read routine einh�ngen 
	 */
	if (picture->format_type == FORM_STANDARD)
		read_pixel = read_8_68000;
	else if (picture->depth == 24)
		read_pixel = read_24_68000;
	else if (picture->depth == 16)
		read_pixel = read_16_68000;
	else /* if (picture->depth == 8) */
		read_pixel = read_8_68000;

	line = (uint8_t *)Malloc(width + 32);
	service->reset_busybox(0, "Floyd Steinberg");

	y = 0;
	do
	{
		if (!(y & 15))
			service->busybox((short) (((long) y << 7L) / destheight));

		rot_rechts = grn_rechts = blu_rechts = 0;
		input = picdat;
		actual_pic = input;

		/*
		 *  Standardformat? -> Zeile auslesen!
		 */
		if (picture->format_type == FORM_STANDARD)
		{
			for (x = 0; x < (owidth + 15) / 16; x++)
			{
				get_standard_pix(actual_pic, line + x * 16, picture->depth, planelen8);
				actual_pic += 2;
			}

			input = line;
		}

		read_f1 = fehlerline2;
		write_f1 = (int16_t *) fehlerline1;
		line_ptr = line;

		x = 0;
		do
		{
			read_pixel();

			b = clipaccess[b + blu_rechts + *(read_f1) + *(read_f1 + 7)];
			g = clipaccess[g + grn_rechts + *(read_f1 + 2) + *(read_f1 + 2 + 7)];
			r = clipaccess[r + rot_rechts + *(read_f1 + 4) + *(read_f1 + 4 + 7)];
			read_f1 += 6;

			/*
			 * Farbindex ermitteln
			 */
			nct_index = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
			nidx = nc_table[nct_index];
			if (nidx == not_in_nct)
			{
				par[3] = (r >> 3);
				par[4] = (g >> 3);
				par[5] = (b >> 3);
				nidx = seek_nearest_col(par, 1 << dest_planes);
				nc_table[nct_index] = nidx;
			}
			*line_ptr++ = plane_table[nidx];

			/*
			 * Farbdifferenzen ermitteln + Fehler merken
			 */
			destpal = xpalette + nidx;

			blu_diff = b - *destpal;
			blu_rechts = fehler_r[blu_diff];
			*(write_f1++) = fru_acc[blu_diff];

			grn_diff = g - *(destpal + 256);
			grn_rechts = fehler_r[grn_diff];
			*(write_f1++) = fru_acc[grn_diff];

			red_diff = r - *(destpal + 512);
			rot_rechts = fehler_r[red_diff];
			*(write_f1++) = fru_acc[red_diff];

			input += xscale;
		} while (++x < destwidth);

		/*
		 *  und jetzt die Zeile ins Standardformat wandeln
		 */
		output += set_16_pixels(line, output, dest_planes, planelen, destwidth);

		swap = fehlerline1;
		fehlerline1 = fehlerline2;
		fehlerline2 = swap;

		picdat += yscale;
		picdat += linelen;
	} while (++y < destheight);

	Mfree(line);
	Mfree(f1);
	Mfree(f2);
	Mfree(fehler_u_ru);
	return M_PICDONE;
}
