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
/*							BMP Encoder						*/
/* Version 0.1  --  06.01.98 - 09.01.98						*/
/*	  1, 4, 8 und 24 Bit, unkomprimiert						*/
/* Version 0.2  --  12.01.98								*/
/*	  1, 8 und 24 Bit, komprimiert							*/
/* Version 0.3  --  13.01.98								*/
/*	  Komprimierung �berarbeitet, beste Kompressionsrate	*/
/*	  aller Programme, au�erdem werden die Bildern jetzt	*/
/*	  auch von den PC-Programmen gelesen.					*/
/* Version 0.4  --  13.01.98								*/
/*	  4 Bit komprimiert, Kompressionsrate ebensogut			*/
/* Version 0.4.5  --  20.01.98								*/
/*	  Oberfl�che erweitert (Unm�glichkeiten in der			*/
/*	  Konfiguration im GUI abgefangen).						*/
/* Version 0.5  --  27.08.98								*/
/*	  Probleme mit der Invertierung von 1 Bit Bildern		*/
/*	  behoben. Leider auf Kosten der Modulgr��e (eigene		*/
/*	  Routinen f�r 1 Bit Ausgabe).							*/
/* Version 0.6  --  18.09.98								*/
/*	  Erzeugung von F�llbytes durch encoded runs mit L�nge	*/
/*	  v am Zeileende entfernt da durch nachfolgenden		*/
/*	  Escapecode 0 unn�tig (und spart nun Zeilen * 2 Bytes	*/
/*	  im Bild ein).											*/
/* Version 0.7  --  17.10.98								*/
/*	  Fehler in Bedienung behoben. Bei Verlassen per Return	*/
/*	  wurde der Dialog zwar geschlossen, die Config aber	*/
/*	  nicht gespeichert.									*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#include "country.h"

#if COUNTRY==1
#include "de/bmp.rsh"
#elif COUNTRY==0
#include "en/bmp.rsh"
#elif COUNTRY==2
#include "en/bmp.rsh"				/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define TIMER 0

#define LIT		1
#define ENC		2

typedef struct
{
	uint8_t comp;						/* komprimiert oder nicht */
	uint8_t format;						/* Win 2.x, Win 3.x, OS/2 1.x oder OS/2 2.x */
} CONFIG;

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

static void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);

/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
#ifdef __PUREC__
/* Dies bastelt direkt ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
static unsigned long swap_word2(unsigned long w) 0xE058;
static uint32_t swap_d0(uint32_t w) 0x4840;
#define swap_long(w) swap_word2(swap_d0(swap_word2(w)))
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
static uint32_t swap_long(uint32_t w)
{
	return ((w >> 24)) | ((w & 0xff0000L) >> 8) | ((w & 0xff00L) << 8) | ((w & 0xffL) << 24);
}
#endif


/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"BMP",
	0x0070,
	"Christian Eyrich",
	{ "BMP", "", "", "", "", "", "", "", "", "" },
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


MOD_ABILITY module_ability = {
	1, 4, 8, 24, 0, 0, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	M_MORE
};




/* BMP-Headerstruktur schreiben */
static long write_header(uint8_t *ziel, unsigned short width, unsigned short height, CONFIG *config, uint8_t BitsPerPixel, uint8_t *pal, uint8_t flag, long f_len)
{
	uint8_t *oziel;
	uint8_t *ppal;
	uint8_t comp;
	short i;
	short cols;
	unsigned long DatenOffset;

	oziel = ziel;

	if (config->comp == RLE)
	{
		if (BitsPerPixel == 4)
			comp = 2;					/* RLE4 */
		else
			comp = 1;					/* RLE8 */
	} else
	{
		comp = 0;						/* keine */
	}

	cols = 1 << BitsPerPixel;

	switch (config->format)
	{
	case WIN12:
		*(uint16_t *) ziel = swap_word(0x0);
		*(uint16_t *) (ziel + 0x02) = swap_word(width);
		*(uint16_t *) (ziel + 0x04) = swap_word(height);
		*(ziel + 0x08) = 1;
		*(ziel + 0x09) = BitsPerPixel;
		ziel += 0x0a;
		break;
	case WIN3:
		*(uint16_t *) ziel = 0x424d;
		if (flag)
		{
			*(uint32_t *) (ziel + 0x02) = swap_long(f_len);
		}
		/* Reserved1 (ab 0x06) und Reserved2 (ab 0x08) gleich 0 */
		DatenOffset = 0x0e + 0x28 + cols * 4;
		*(uint32_t *) (ziel + 0x0a) = swap_long(DatenOffset);

		ziel += 0x0e;
		*(uint32_t *) (ziel + 0x00) = swap_long(0x28);
		*(uint32_t *) (ziel + 0x04) = swap_long(width);
		*(uint32_t *) (ziel + 0x08) = swap_long(height);
		*(uint16_t *) (ziel + 0x0c) = swap_word(1);
		*(uint16_t *) (ziel + 0x0e) = swap_word(BitsPerPixel);
		*(uint32_t *) (ziel + 0x10) = swap_long(comp);
		if (flag)
		{
			*(uint32_t *) (ziel + 0x14) = swap_long(f_len - DatenOffset);
		}
		/* horizontale (ab 0x18) und vertikale (ab 0x1c) Aufl�sung gleich 0 */
		*(uint32_t *) (ziel + 0x20) = swap_long(cols);	/* colors used */
		*(uint32_t *) (ziel + 0x24) = 0;	/* significant colors */
		ziel += 0x28;
		break;
	case OS2_1:
		*(uint16_t *) ziel = 0x424d;
		if (flag)
		{
			*(uint32_t *) (ziel + 0x02) = swap_long(f_len);
		}
		/* Reserved1 (ab 0x06) und Reserved2 (ab 0x08) gleich 0 */
		DatenOffset = 0x0e + 0x0c + cols * 3;
		*(uint32_t *) (ziel + 0x0a) = swap_long(DatenOffset);

		ziel += 0x0e;
		*(uint32_t *) (ziel + 0x00) = swap_long(0x0c);
		*(uint16_t *) (ziel + 0x04) = swap_word(width);
		*(uint16_t *) (ziel + 0x06) = swap_word(height);
		*(uint16_t *) (ziel + 0x08) = swap_word(1);
		*(uint16_t *) (ziel + 0x0a) = swap_word(BitsPerPixel);
		ziel += 0x0c;
		break;
	case OS2_2T1:
		*(uint16_t *) ziel = 0x424d;
		if (flag)
		{
			*(uint32_t *) (ziel + 0x02) = swap_long(f_len);
		}
		/* Reserved1 (ab 0x06) und Reserved2 (ab 0x08) gleich 0 */
		DatenOffset = 0x0e + 0x40 + cols * 4;
		*(uint32_t *) (ziel + 0x0a) = swap_long(DatenOffset);

		ziel += 0x0e;
		*(uint32_t *) (ziel + 0x00) = swap_long(0x40);
		*(uint32_t *) (ziel + 0x04) = swap_long(width);
		*(uint32_t *) (ziel + 0x08) = swap_long(height);
		*(uint16_t *) (ziel + 0x0c) = swap_word(1);
		*(uint16_t *) (ziel + 0x0e) = swap_word(BitsPerPixel);
		*(uint32_t *) (ziel + 0x10) = swap_long(comp);
		if (flag)
		{
			*(uint32_t *) (ziel + 0x14) = swap_long(f_len - DatenOffset);
		}
		/* horizontale (ab 0x18) und vertikale (ab 0x1c) Aufl�sung gleich 0 */
		*(uint32_t *) (ziel + 0x20) = swap_long(cols);
		*(uint32_t *) (ziel + 0x24) = swap_long(cols);
		/* Einheiten f�r die Aufl�sung (ab 0x28) gleich 0 */
		/* Reserviert (ab 0x2a) gleich 0 */
		*(uint16_t *) (ziel + 0x2c) = swap_word(0);	/* steht auf dem Kopf */
		/* Rendering (ab 0x2e), Size1 (ab 0x30) und Size2 (ab 0x34) gleich 0 */
		/* ColorEncoding (ab 0x38) gleich 0 */
		/* Identifier (ab 0x3c) gleich 0 */
		ziel += 0x40;
		break;
	case OS2_2T2:
		*(uint16_t *) ziel = 0x4241;
		*(uint32_t *) (ziel + 0x02) = swap_long(0x0e);
		/* Offset des n�chsten BAFH (ab 0x06) gleich 0 */
		*(uint32_t *) (ziel + 0x0a) = swap_long(72);

		ziel += 0x0e;
		*(uint16_t *) ziel = 0x424d;
		if (flag)
		{
			*(uint32_t *) (ziel + 0x02) = swap_long(f_len);
		}
		/* Reserved1 (ab 0x06) und Reserved2 (ab 0x08) gleich 0 */
		DatenOffset = 0x0e + 0x0e + 0x28 + cols * 4;
		*(uint32_t *) (ziel + 0x0a) = swap_long(DatenOffset);

		ziel += 0x0e;
		*(uint32_t *) (ziel + 0x00) = swap_long(0x28);
		*(uint32_t *) (ziel + 0x04) = swap_long(width);
		*(uint32_t *) (ziel + 0x08) = swap_long(height);
		*(uint16_t *) (ziel + 0x0c) = swap_word(1);
		*(uint16_t *) (ziel + 0x0e) = swap_word(BitsPerPixel);
		*(uint32_t *) (ziel + 0x10) = swap_long(comp);
		if (flag)
		{
			*(uint32_t *) (ziel + 0x14) = swap_long(f_len - DatenOffset);
		}
		/* horizontale (ab 0x18) und vertikale (ab 0x1c) Aufl�sung gleich 0 */
		*(uint32_t *) (ziel + 0x20) = swap_long(cols);
		*(uint32_t *) (ziel + 0x24) = swap_long(cols);
		ziel += 0x28;
		break;
	}

	ppal = ziel;

	/* �bertragen der Palette */
	if (BitsPerPixel < 16 && config->format != WIN12)
	{
		if (BitsPerPixel == 1)
		{
			*ppal++ = 0;
			*ppal++ = 0;
			*ppal++ = 0;
			if (config->format != OS2_1)
				ppal++;
			*ppal++ = 255;
			*ppal++ = 255;
			*ppal++ = 255;
			if (config->format != OS2_1)
				ppal++;
		} else
			for (i = 0; i < cols; i++)
			{
				*ppal++ = *(pal++ + 2);
				*ppal++ = *pal++;
				*ppal++ = *(pal++ - 2);
				if (config->format != OS2_1)
					ppal++;
			}
	}

	return ppal - oziel;
}


/* BMP mit 1 Bit schreiben - mu� wegen Invertierung leider in eine eigene Funktion */
static long writeBMPdata1(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	unsigned long v;
	unsigned short x, y;

	oziel = ziel;

	buffer += (unsigned long) (height - 1) * w;

	v = (((w + 3) / 4) * 4 - w);

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = ~*buffer++;
		} while (++x < w);
		ziel += v;
		buffer -= w + w;
	} while (++y < height);

	return ziel - oziel;
}


/* BMP mit 8 Bit schreiben */
static long writeBMPdata8(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height)
{
	uint8_t *oziel;
	unsigned long v;
	unsigned short x, y;

	oziel = ziel;

	buffer += (unsigned long) (height - 1) * w;

	v = (((w + 3) / 4) * 4 - w);

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < w);
		ziel += v;
		buffer -= w + w;
	} while (++y < height);

	return ziel - oziel;
}


/* BMP mit 4 Bit schreiben - mu� leider wegen 2 Pixel in ein Byte in eigene Funktion */
static long writeBMPdata4(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height)
{
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t *line;
	unsigned long v;
	unsigned short x, y;
	unsigned long planelength;
	unsigned long w;
	unsigned long realwidth;

	oziel = ziel;

	realwidth = (width + 7) / 8;
	buffer += (unsigned long) (height - 1) * realwidth;

	w = (width + 1) / 2;
	v = (((w + 3) / 4) * 4 - w);

	planelength = realwidth * (unsigned long) height;

	pixbuf = SMalloc(width + 7);

	y = 0;
	do
	{
		memset(pixbuf, 0x0, width);
		getpix_std_line(buffer, pixbuf, 4, planelength, width);
		line = pixbuf;

		x = 0;
		do
		{
			*ziel = *line++ << 4;
			*ziel++ |= *line++;
		} while (++x < w);
		ziel += v;
		buffer -= realwidth;
	} while (++y < height);

	SMfree(pixbuf);

	return ziel - oziel;
}


/* BMP mit 24 Bit schreiben - mu� leider wegen BGR statt RGB in eigene Funktion */
static long writeBMPdata24(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height)
{
	uint8_t *oziel;
	unsigned long v;
	unsigned short x, y;
	unsigned long realwidth;

	oziel = ziel;

	realwidth = width * 3L;
	buffer += (unsigned long) (height - 1) * realwidth;

	v = (((realwidth + 3) / 4) * 4 - realwidth);	/* Zeilenl�nge auf Vielfaches von 4 */

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *(buffer++ + 2);
			*ziel++ = *buffer++;
			*ziel++ = *(buffer++ - 2);
		} while (++x < width);
		ziel += v;
		buffer -= realwidth + realwidth;
	} while (++y < height);

	return ziel - oziel;
}




/* BMP mit 1 Bit kodieren - mu� wegen Invertierung leider in eine eigene Funktion */
static long encodeBMPdata1(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height)
{
	uint8_t *oziel;
	uint8_t *linebuf;
	uint8_t *olinebuf;
	uint8_t pixel;
	uint8_t counter;
	uint8_t nextenc;
	unsigned short x, y, xx;
	unsigned long w;

	w = (width + 7) / 8;

	oziel = ziel;

	buffer += (unsigned long) (height - 1) * w;

	linebuf = (uint8_t *) SMalloc(w + 1);
	olinebuf = linebuf;

	y = 0;
	do
	{
		linebuf = olinebuf;
		memcpy(linebuf, buffer, w);
		buffer += w;

		x = 0;
		do
		{
			counter = 1;
			pixel = *linebuf++;
			x++;

			xx = x;
			while (pixel == *linebuf && counter < 0xff && xx < w)
			{
				linebuf++;
				xx++;
				counter++;
			}

			if (counter >= 2)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				*ziel++ = ~pixel;
				x = xx;
			} else						/* ... aber leider nicht! */
			{
				*ziel++ = 0;
				ziel++;
				*ziel++ = ~pixel;

				do
				{
					nextenc = 1;

					while (*linebuf != *(linebuf + 1) && counter < 0xff && xx < w)
					{
						*ziel++ = ~*linebuf++;
						xx++;
						counter++;
					}

					while (*linebuf == *(linebuf + 1) && nextenc < 0x5 && counter < 0xff && xx < w)
					{
						*ziel++ = ~*linebuf++;
						xx++;
						counter++;
						nextenc++;
					}
				} while (nextenc < 5 && counter < 0xff && xx < w);

				if (nextenc > 4)
				{
					counter -= nextenc - 1;
					xx -= nextenc - 1;
					linebuf -= nextenc - 1;
					ziel -= nextenc - 1;
				}

				if (counter < 3)
				{
					if (counter == 1)
					{
						ziel -= 3;
						*ziel++ = 1;
						*ziel++ = ~pixel;
					} else
					{
						ziel -= 4;
						*ziel++ = 1;
						*ziel++ = ~pixel;
						*ziel++ = 1;
						*ziel++ = ~*(linebuf - 1);
					}
				} else
				{
					*(ziel - counter - 1) = counter;
					if (counter & 1)	/* wordalignement */
						*ziel++ = 0;
				}

				x = xx;
			}
		} while (x < w);

		/* Ende-der-Zeile-Escape */
		*ziel++ = 0;
		*ziel++ = 0;

		buffer -= w + w;
	} while (++y < height);

	/* Ende-der-Grafik-Escape */
	*ziel++ = 0;
	*ziel++ = 1;

	return ziel - oziel;
}


/* BMP mit 8 und 24 Bit kodieren */
static long encodeBMPdata(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height, uint8_t BitsPerPixel)
{
	uint8_t *oziel;
	uint8_t *linebuf;
	uint8_t *olinebuf;
	uint8_t pixel;
	uint8_t counter;
	uint8_t nextenc;
	unsigned short x, y, xx;
	unsigned long w;

	switch (BitsPerPixel)
	{
	case 1:
		w = (width + 7) / 8;
		break;
	case 8:
		w = width;
		break;
	case 24:
		w = width * 3L;
		break;
	default:
		return 0;
	}

	oziel = ziel;

	buffer += (unsigned long) (height - 1) * w;

	linebuf = (uint8_t *) SMalloc(w + 1);
	olinebuf = linebuf;

	y = 0;
	do
	{
		linebuf = olinebuf;
		if (BitsPerPixel == 24)
		{
			x = 0;
			do
			{
				*linebuf++ = *(buffer++ + 2);
				*linebuf++ = *buffer++;
				*linebuf++ = *(buffer++ - 2);
			} while (++x < width);
			linebuf = olinebuf;
		} else
		{
			memcpy(linebuf, buffer, w);
			buffer += w;
		}

		x = 0;
		do
		{
			counter = 1;
			pixel = *linebuf++;
			x++;

			xx = x;
			while (pixel == *linebuf && counter < 0xff && xx < w)
			{
				linebuf++;
				xx++;
				counter++;
			}

			if (counter >= 2)			/* es lohnt sich! */
			{
				*ziel++ = counter;
				*ziel++ = pixel;
				x = xx;
			} else						/* ... aber leider nicht! */
			{
				*ziel++ = 0;
				ziel++;
				*ziel++ = pixel;

				do
				{
					nextenc = 1;

					while (*linebuf != *(linebuf + 1) && counter < 0xff && xx < w)
					{
						*ziel++ = *linebuf++;
						xx++;
						counter++;
					}

					while (*linebuf == *(linebuf + 1) && nextenc < 0x5 && counter < 0xff && xx < w)
					{
						*ziel++ = *linebuf++;
						xx++;
						counter++;
						nextenc++;
					}
				} while (nextenc < 5 && counter < 0xff && xx < w);

				if (nextenc > 4)
				{
					counter -= nextenc - 1;
					xx -= nextenc - 1;
					linebuf -= nextenc - 1;
					ziel -= nextenc - 1;
				}

				if (counter < 3)
				{
					if (counter == 1)
					{
						ziel -= 3;
						*ziel++ = 1;
						*ziel++ = pixel;
					} else
					{
						ziel -= 4;
						*ziel++ = 1;
						*ziel++ = pixel;
						*ziel++ = 1;
						*ziel++ = *(linebuf - 1);
					}
				} else
				{
					*(ziel - counter - 1) = counter;
					if (counter & 1)	/* wordalignement */
						*ziel++ = 0;
				}

				x = xx;
			}
		} while (x < w);

		/* Ende-der-Zeile-Escape */
		*ziel++ = 0;
		*ziel++ = 0;

		buffer -= w + w;
	} while (++y < height);

	/* Ende-der-Grafik-Escape */
	*ziel++ = 0;
	*ziel++ = 1;

	return ziel - oziel;
}


/* BMP mit 4 Bit kodieren - mu� leider wegen 2 Pixel in ein Byte in eigene Funktion */
static long encodeBMPdata4(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height)
{
	uint8_t *oziel;
	uint8_t *linebuf;
	uint8_t *olinebuf;
	uint8_t *pixbuf;
	uint8_t *line;
	uint8_t pixel;
	uint8_t counter;
	uint8_t nextenc;
	unsigned short x, y, xx;
	unsigned long planelength;
	unsigned long w;
	unsigned long realwidth;

	oziel = ziel;

	realwidth = (width + 7) / 8;
	buffer += (unsigned long) (height - 1) * realwidth;

	w = (width + 1) / 2;

	linebuf = (uint8_t *) SMalloc(w + 1);
	olinebuf = linebuf;

	planelength = realwidth * (unsigned long) height;

	pixbuf = SMalloc(width + 7);

	y = 0;
	do
	{
		linebuf = olinebuf;
		memset(pixbuf, 0x0, width);
		getpix_std_line(buffer, pixbuf, 4, planelength, width);
		line = pixbuf;

		x = 0;
		do
		{
			*linebuf = *line++ << 4;
			*linebuf++ |= *line++;
		} while (++x < w);
		linebuf = olinebuf;

		x = 0;
		do
		{
			counter = 1;
			pixel = *linebuf++;
			x++;

			xx = x;
			while (pixel == *linebuf && counter < 0x7f && xx < w)
			{
				linebuf++;
				xx++;
				counter++;
			}

			if (counter >= 2)			/* es lohnt sich! */
			{
				*ziel++ = counter << 1;
				*ziel++ = pixel;
				x = xx;
			} else						/* ... aber leider nicht! */
			{
				*ziel++ = 0;
				ziel++;
				*ziel++ = pixel;

				do
				{
					nextenc = 1;

					while (*linebuf != *(linebuf + 1) && counter < 0x7f && xx < w)
					{
						*ziel++ = *linebuf++;
						xx++;
						counter++;
					}

					while (*linebuf == *(linebuf + 1) && nextenc < 0x5 && counter < 0x7f && xx < w)
					{
						*ziel++ = *linebuf++;
						xx++;
						counter++;
						nextenc++;
					}
				} while (nextenc < 5 && counter < 0x7f && xx < w);

				if (nextenc > 4)
				{
					linebuf -= nextenc - 1;
					ziel -= nextenc - 1;
					xx -= nextenc - 1;
					counter -= nextenc - 1;
				}

				if (counter < 3)
				{
					if (counter == 1)
					{
						ziel -= 3;
						*ziel++ = 2;
						*ziel++ = pixel;
					} else
					{
						ziel -= 4;
						*ziel++ = 2;
						*ziel++ = pixel;
						*ziel++ = 2;
						*ziel++ = *(linebuf - 1);
					}
				} else
				{
					*(ziel - counter - 1) = counter << 1;
					if (counter & 1)	/* wordalignement */
						*ziel++ = 0;
				}

				x = xx;
			}
		} while (x < w);
		/* Ende-der-Zeile-Escape */
		*ziel++ = 0;
		*ziel++ = 0;

		buffer -= realwidth;
	} while (++y < height);

	/* Ende-der-Grafik-Escape */
	*ziel++ = 0;
	*ziel++ = 1;

	SMfree(pixbuf);

	return ziel - oziel;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				BMP Image Format (BMP)				*/
/*		Windows 2.x, Windows 3.x, OS/2 1.x,			*/
/*		OS/2 2.x,									*/
/*		1, 4, 8, 24 Bit, 							*/
/*		unkomprimiert, RLE4 und RLE8)				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t BitsPerPixel;
	unsigned short width, height;
	unsigned short headsize;
	unsigned short Button;
	unsigned short pallen;
	WORD t;
	CONFIG **pp;
	
	unsigned long f_len;
	unsigned long w;
	unsigned long memwidth;

	static WINDOW window;
	static OBJECT *win_form;

	static CONFIG config;


	switch (smurf_struct->module_mode)
	{
	case MSTART:
		/* falls �bergeben, Konfig �bernehmen */
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
		{
			config = **pp;
		} else
		{
			config.format = WIN3;
			config.comp = KEINE;
		}

		win_form = rs_trindex[BMP_EXPORT];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		smurf_struct->module_mode = M_WAITING;

		break;

	case MMORE:
		/* Ressource aktualisieren */
		win_form[WIN12].ob_state &= ~OS_SELECTED;
		win_form[WIN3].ob_state &= ~OS_SELECTED;
		win_form[OS2_1].ob_state &= ~OS_SELECTED;
		win_form[OS2_2T1].ob_state &= ~OS_SELECTED;
		win_form[OS2_2T2].ob_state &= ~OS_SELECTED;
		win_form[config.format].ob_state |= OS_SELECTED;

		if (config.comp == KEINE)
		{
			win_form[KEINE].ob_state |= OS_SELECTED;
			win_form[RLE].ob_state &= ~OS_SELECTED;
		} else
		{
			win_form[KEINE].ob_state &= ~OS_SELECTED;
			win_form[RLE].ob_state |= OS_SELECTED;
		}

		/* Zwangskorrektur weil Win 1.x/2.x und OS/2 1.x keine Kompression unterst�tzten */
		if (config.format == WIN12 || config.format == OS2_1)
		{
			win_form[KEINE].ob_state |= OS_SELECTED;
			win_form[RLE].ob_state &= ~OS_SELECTED;

			win_form[KEINE].ob_state |= OS_DISABLED;
			win_form[RLE].ob_state |= OS_DISABLED;
		} else if (config.format == WIN3 || config.format == OS2_2T1 || config.format == OS2_2T2)
		{
			win_form[KEINE].ob_state &= ~OS_DISABLED;
			win_form[RLE].ob_state &= ~OS_DISABLED;
		}

		redraw_window = smurf_struct->services->redraw_window;	/* Redrawfunktion */

		window.whandlem = 0;			/* evtl. Handle l�schen */
		window.module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X-...        */
		window.wy = -1;					/* ...und Y-Pos         */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterh�he          */
		strcpy(window.wtitle, "BMP Exporter");		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* n�chstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		break;

		/* Closer geklickt, Default wieder her */
	case MMORECANC:
		/* falls �bergeben, Konfig �bernehmen */
		pp = (CONFIG **) &smurf_struct->event_par[0];
		if (*pp != NULL)
		{
			config = **pp;
		} else
		{
			config.format = WIN3;
			config.comp = KEINE;
		}

		smurf_struct->module_mode = M_WAITING;

		break;

		/* Buttonevent */
	case MBEVT:
		Button = smurf_struct->event_par[0];

		if (Button == OK)
		{
			/* Konfig �bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);

			smurf_struct->module_mode = M_MOREOK;
		} else if (Button == SAVE)
		{
			/* Konfig �bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);

			smurf_struct->module_mode = M_CONFSAVE;
		} else
		{
			if (Button == WIN12 || Button == WIN3 || Button == OS2_1 || Button == OS2_2T1 || Button == OS2_2T2)
				config.format = Button;
			else if (Button == KEINE || Button == RLE)
				config.comp = Button;

			/* Zwangskorrektur weil Win 1.x/2.x und OS/2 1.x keine Kompression unterst�tzten */
			if (config.format == WIN12 || config.format == OS2_1)
			{
				win_form[KEINE].ob_state |= OS_SELECTED;
				win_form[RLE].ob_state &= ~OS_SELECTED;

				win_form[KEINE].ob_state |= OS_DISABLED;
				win_form[RLE].ob_state |= OS_DISABLED;

				redraw_window(&window, NULL, COMP_BOX, 0);
			} else if (config.format == WIN3 || config.format == OS2_2T1 || config.format == OS2_2T2)
			{
				win_form[KEINE].ob_state &= ~OS_DISABLED;
				win_form[RLE].ob_state &= ~OS_DISABLED;

				redraw_window(&window, NULL, COMP_BOX, 0);
			}

			smurf_struct->module_mode = M_WAITING;
		}

		break;

		/* Keyboardevent */
	case MKEVT:
		Button = smurf_struct->event_par[0];

		if (Button == OK)
		{
			/* Konfig �bergeben */
			pp = (CONFIG **) &smurf_struct->event_par[0];
			*pp = &config;
			smurf_struct->event_par[2] = (short) sizeof(CONFIG);

			smurf_struct->module_mode = M_MOREOK;
		} else
		{
			smurf_struct->module_mode = M_WAITING;
		}
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		smurf_struct->event_par[0] = RGB;

		smurf_struct->module_mode = M_COLSYS;

		break;

	case MEXEC:
#if TIMER
/* wie schnell sind wir? */
		init_timer();
#endif
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;

		buffer = smurf_struct->smurf_pic->pic_data;

		exp_pic = (EXPORT_PIC *) SMalloc(sizeof(EXPORT_PIC));

		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		/* Zwangskorrektur weil Win 1.x/2.x nur 1 Bit beherrscht */
		if (config.format == WIN12 && BitsPerPixel > 1)
			config.format = WIN3;

		/* Zwangskorrektur weil Win 1.x/2.x und OS/2 1.x keine Kompression unterst�tzten */
		if ((config.format == WIN12 || config.format == OS2_1) && config.comp == RLE)
			config.comp = KEINE;

		switch (config.format)
		{
		case WIN12:
			headsize = 10;
			break;
		case WIN3:
			headsize = 14 + 40;
			break;
		case OS2_1:
			headsize = 14 + 12;
			break;
		case OS2_2T1:
			headsize = 14 + 64;			/* OS/2 2.x Variante 1 */
			break;
		case OS2_2T2:
			headsize = 14 + 14 + 40;	/* OS/2 2.x Variante 2 */
			break;
		default:
			return NULL;
		}

		if (BitsPerPixel <= 8)
			pallen = 1024;
		else
			pallen = 0;

		switch (BitsPerPixel)
		{
		case 1:
			w = (width + 7) / 8;
			break;
		case 4:
			w = (width + 1) / 2;
			break;
		case 8:
			w = width;
			break;
		case 24:
			w = width * 3L;
			break;
		default:
			return NULL;
		}

		memwidth = ((w + 3) / 4) * 4;

		f_len = memwidth * (long) height;
		if (config.comp == RLE)
			f_len += f_len * 10 / 100;	/* plus 10% Sicherheitsreserve */

		if ((ziel = (uint8_t *) SMalloc(headsize + pallen + f_len)) == 0)
		{
			smurf_struct->module_mode = M_MEMORY;
			return exp_pic;
		} else
		{
			f_len = write_header(ziel, width, height, &config, BitsPerPixel, smurf_struct->smurf_pic->palette, 0, 0);

			if (config.comp == KEINE)
			{
				switch (BitsPerPixel)
				{
				case 1:
					f_len += writeBMPdata1(ziel + f_len, buffer, w, height);
					break;
				case 8:
					f_len += writeBMPdata8(ziel + f_len, buffer, w, height);
					break;
				case 24:
					f_len += writeBMPdata24(ziel + f_len, buffer, width, height);
					break;
				case 4:
					f_len += writeBMPdata4(ziel + f_len, buffer, width, height);
					break;
				}
			} else
			{
				switch (BitsPerPixel)
				{
				case 1:
					f_len += encodeBMPdata1(ziel + f_len, buffer, width, height);
					break;
				case 8:
				case 24:
					f_len += encodeBMPdata(ziel + f_len, buffer, width, height, BitsPerPixel);
					break;
				case 4:
					f_len += encodeBMPdata4(ziel + f_len, buffer, width, height);
					break;
				}
			}

			/* erst jetzt bekannte Headerwerte nachtragen */
			write_header(ziel, width, height, &config, BitsPerPixel, smurf_struct->smurf_pic->palette, 1, f_len);

			exp_pic->pic_data = ziel;
			exp_pic->f_len = f_len;
		}								/* Malloc */

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void)Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;
		return exp_pic;

	/* Mterm empfangen - Speicher freigeben und beenden */
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
