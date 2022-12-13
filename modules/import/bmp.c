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
/*						BMP/CLP Decoder						*/
/* Version 0.1  --  27.10.96								*/
/*	  BMP 1-24 Bit, unkomprimiert							*/
/* Version 0.2  --  1.11.96									*/
/*	  in CLP enthaltene DIBs								*/
/* Version 0.3  --  1.11.96									*/
/*	  internes CLP-Format									*/
/* Version 0.4  --  2.11.96									*/
/*	  RLE8 komprimierte BMPs								*/
/* Version 0.5  --  12.11.96								*/
/*	  RLE4 komprimierte BMPs, noch nicht endgÅltig -		*/
/*	  es kommt z.Zt. noch als 8 Bit PP - die Wandlung fehlt */
/*	  noch, nachdem Holtorf mich durch einen Fehler wieder	*/
/*	  mal einen Tag gekostet hat ...						*/
/* Version 0.6  --  16.11.96								*/
/*	  RLE4 komprimierte BMPs kommen nun auch als 4 Bit im	*/
/*	  Standardformat.										*/
/* Version 0.7  --  02.04.97								*/
/*	  Erkennung fÅr den Fall von Windows 1.x BMP modifiziert*/
/* Version 0.8  --  08.01.98								*/
/*	  Und gibt es auch endlich Code zum Auslesen der Bild-	*/
/*	  informationen fÅr Windows 1.x BMP und damit ist auch	*/
/*	  dieses Unterformat lesbar.							*/
/* Version 0.9  --  15.05.98								*/
/*	  Auf zeilenweise Standardformatwandlung umgestellt		*/
/* Version 1.0  --  27.08.98								*/
/*	  Probleme mit der Invertierung von 1 Bit Bildern		*/
/*	  behoben. In der Reihenfolge der Farben in der Palette	*/
/*	  (s, w oder w, s) ist kodiert, ob das Bild invertiert	*/
/*	  werden muû.											*/
/* Version 1.0  --  18.09.98								*/
/*	  Probleme mit Bildern bei denen ein Escape 0 am Anfang	*/
/*	  einer	Zeile enthalten (der Ringlstetter-Effekt)		*/
/*	  behoben.												*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][ Es werden bei RIFF Bitmap's|zur Zeit nur BMP's unterstÅtzt][ Stop ]"
#elif COUNTRY==0
#define ERROR1 "[1][ At present for RIFF bitmaps|only BMP's are supported][ Stop ]"
#elif COUNTRY==2
#define ERROR1 "[1][ At present for RIFF bitmaps|only BMP's are supported][ Stop ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "import.h"
#include "smurfine.h"

#define NOMEM		(void *)0L
#define CF_BITMAP	2
#define CF_DIB		8
#define CF_PALETTE	9
#define CF_BITMAP2	0xff

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);


#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
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

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"BMP/Clipboard",			/* Name of module */
	0x0111,						/* version */
	"Christian Eyrich",			/* author */
	/* 10 Extensionen fuer Importer */
	{ "BMP", "DIB", "CLP", "RIF", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	{ NULL, NULL, NULL, NULL },
	/* 4 checkbox titles */
	{ NULL, NULL, NULL, NULL },
	/* 4 edit object titles */
	{ NULL, NULL, NULL, NULL },
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


static uint8_t const winstdpal[] = {
	0x00, 0x00, 0x00,
	0xbf, 0x00, 0x00,
	0x00, 0xbf, 0x00,
	0xbf, 0xbf, 0x00,
	0x00, 0x00, 0xbf,
	0xbf, 0x00, 0xbf,
	0x00, 0xbf, 0xbf,
	0xc0, 0xc0, 0xc0,
	0xc0, 0xdc, 0xc0,
	0xa6, 0xca, 0xf0,
	0x83, 0xab, 0xff,
	0xab, 0xab, 0xff,
	0xd7, 0xab, 0xff,
	0xff, 0xab, 0xff,
	0x00, 0xd7, 0xff,
	0x57, 0xd7, 0xff,
	0x57, 0x00, 0x00,
	0x83, 0x00, 0x00,
	0xab, 0x00, 0x00,
	0xd7, 0x00, 0x00,
	0x00, 0x2b, 0x00,
	0x57, 0x2b, 0x00,
	0x83, 0x2b, 0x00,
	0xab, 0x2b, 0x00,
	0xd7, 0x2b, 0x00,
	0xff, 0x2b, 0x00,
	0x00, 0x57, 0x00,
	0x57, 0x57, 0x00,
	0x83, 0x57, 0x00,
	0xab, 0x57, 0x00,
	0x57, 0xd7, 0x00,
	0x83, 0xd7, 0x00,
	0xab, 0xd7, 0x00,
	0xd7, 0xd7, 0x00,
	0xff, 0xd7, 0x00,
	0x57, 0xff, 0x00,
	0x83, 0xff, 0x00,
	0xab, 0xff, 0x00,
	0xd7, 0xff, 0x00,
	0x00, 0x00, 0x57,
	0x57, 0x00, 0x57,
	0x83, 0x00, 0x57,
	0xab, 0x00, 0x57,
	0xd7, 0x00, 0x57,
	0xff, 0x00, 0x57,
	0x00, 0x2b, 0x57,
	0x53, 0x2b, 0x57,
	0x83, 0x2b, 0x57,
	0xab, 0x2b, 0x57,
	0xd7, 0x2b, 0x57,
	0xff, 0x2b, 0x57,
	0x00, 0x57, 0x57,
	0x57, 0x57, 0x57,
	0x83, 0x57, 0x57,
	0xab, 0x57, 0x57,
	0xd7, 0x57, 0x57,
	0xff, 0x57, 0x57,
	0x00, 0x83, 0x57,
	0x57, 0x83, 0x57,
	0x83, 0x83, 0x57,
	0x00, 0xff, 0x57,
	0x57, 0xff, 0x57,
	0x83, 0xff, 0x57,
	0xab, 0xff, 0x57,
	0xd7, 0xff, 0x57,
	0xff, 0xff, 0x57,
	0x00, 0x00, 0x83,
	0x57, 0x00, 0x83,
	0x83, 0x00, 0x83,
	0xab, 0x00, 0x83,
	0xd7, 0x00, 0x83,
	0xff, 0x00, 0x83,
	0x00, 0x2b, 0x83,
	0x57, 0x2b, 0x83,
	0x83, 0x2b, 0x83,
	0xab, 0x2b, 0x83,
	0xd7, 0x2b, 0x83,
	0xff, 0x2b, 0x83,
	0x00, 0x57, 0x83,
	0x57, 0x57, 0x83,
	0x83, 0x57, 0x83,
	0xab, 0x57, 0x83,
	0xd7, 0x57, 0x83,
	0xff, 0x57, 0x83,
	0x00, 0x83, 0x83,
	0x57, 0x83, 0x83,
	0x83, 0x83, 0x83,
	0xab, 0x83, 0x83,
	0xd7, 0x83, 0x83,
	0xff, 0x83, 0x83,
	0xd7, 0x57, 0x00,
	0xff, 0x57, 0x00,
	0x00, 0x83, 0x00,
	0x57, 0x83, 0x00,
	0x83, 0x83, 0x00,
	0xab, 0x83, 0x00,
	0xd7, 0x83, 0x00,
	0xff, 0x83, 0x00,
	0x00, 0xab, 0x00,
	0x57, 0xab, 0x00,
	0x83, 0xab, 0x00,
	0xab, 0xab, 0x00,
	0xd7, 0xab, 0x00,
	0xff, 0xab, 0x00,
	0x00, 0xd7, 0x00,
	0xab, 0xff, 0x83,
	0xd7, 0xff, 0x83,
	0xff, 0xff, 0x83,
	0x00, 0x00, 0xab,
	0x57, 0x00, 0xab,
	0x83, 0x00, 0xab,
	0xab, 0x00, 0xab,
	0xd7, 0x00, 0xab,
	0xff, 0x00, 0xab,
	0x00, 0x2b, 0xab,
	0x57, 0x2b, 0xab,
	0x83, 0x2b, 0xab,
	0xab, 0x2b, 0xab,
	0xd7, 0x2b, 0xab,
	0xff, 0x2b, 0xab,
	0x00, 0x57, 0xab,
	0xab, 0x83, 0x57,
	0xd7, 0x83, 0x57,
	0xff, 0x83, 0x57,
	0x00, 0xab, 0x57,
	0x57, 0xab, 0x57,
	0x83, 0xab, 0x57,
	0xab, 0xab, 0x57,
	0xd7, 0xab, 0x57,
	0xff, 0xab, 0x57,
	0x00, 0xd7, 0x57,
	0x57, 0xd7, 0x57,
	0x83, 0xd7, 0x57,
	0xab, 0xd7, 0x57,
	0xd7, 0xd7, 0x57,
	0xff, 0xd7, 0x57,
	0x83, 0xff, 0xab,
	0xab, 0xff, 0xab,
	0xd7, 0xff, 0xab,
	0xff, 0xff, 0xab,
	0x00, 0x00, 0xd7,
	0x57, 0x00, 0xd7,
	0x83, 0x00, 0xd7,
	0xab, 0x00, 0xd7,
	0xd7, 0x00, 0xd7,
	0xff, 0x00, 0xd7,
	0x00, 0x2b, 0xd7,
	0x57, 0x2b, 0xd7,
	0x83, 0x2b, 0xd7,
	0xab, 0x2b, 0xd7,
	0xd7, 0x2b, 0xd7,
	0xff, 0x2b, 0xd7,
	0x00, 0xab, 0x83,
	0x57, 0xab, 0x83,
	0x83, 0xab, 0x83,
	0xab, 0xab, 0x83,
	0xd7, 0xab, 0x83,
	0xff, 0xab, 0x83,
	0x00, 0xd7, 0x83,
	0x57, 0xd7, 0x83,
	0x83, 0xd7, 0x83,
	0xab, 0xd7, 0x83,
	0xd7, 0xd7, 0x83,
	0xff, 0xd7, 0x83,
	0x00, 0xff, 0x83,
	0xf7, 0xff, 0x83,
	0x83, 0xff, 0x83,
	0x57, 0xff, 0xd7,
	0x83, 0xff, 0xd7,
	0xab, 0xff, 0xd7,
	0xd7, 0xff, 0xd7,
	0xff, 0xff, 0xd7,
	0x57, 0x00, 0xff,
	0x83, 0x00, 0xff,
	0xab, 0x00, 0xff,
	0xd7, 0x00, 0xff,
	0x00, 0x2b, 0xff,
	0x57, 0x2b, 0xff,
	0x83, 0x2b, 0xff,
	0xab, 0x2b, 0xff,
	0xd7, 0x2b, 0xff,
	0xff, 0x2b, 0xff,
	0x00, 0x57, 0xff,
	0x57, 0x57, 0xab,
	0x83, 0x57, 0xab,
	0xab, 0x57, 0xab,
	0xd7, 0x57, 0xab,
	0xff, 0x57, 0xab,
	0x00, 0x83, 0xab,
	0x57, 0x83, 0xab,
	0x83, 0x83, 0xab,
	0xab, 0x83, 0xab,
	0xd7, 0x83, 0xab,
	0xff, 0x83, 0xab,
	0x00, 0xab, 0xab,
	0x57, 0xab, 0xab,
	0x83, 0xab, 0xab,
	0xab, 0xab, 0xab,
	0xd7, 0xab, 0xab,
	0xff, 0xab, 0xab,
	0x00, 0xd7, 0xab,
	0x57, 0xd7, 0xab,
	0x83, 0xd7, 0xab,
	0xab, 0xd7, 0xab,
	0xd7, 0xd7, 0xab,
	0xff, 0xd7, 0xab,
	0x00, 0xff, 0xab,
	0x57, 0xff, 0xab,
	0x00, 0x57, 0xd7,
	0x57, 0x57, 0xd7,
	0x83, 0x57, 0xd7,
	0xab, 0x57, 0xd7,
	0xd7, 0x57, 0xd7,
	0xff, 0x57, 0xd7,
	0x00, 0x83, 0xd7,
	0x57, 0x83, 0xd7,
	0x83, 0x83, 0xd7,
	0xab, 0x83, 0xd7,
	0xd7, 0x83, 0xd7,
	0xff, 0x83, 0xd7,
	0x00, 0xab, 0xd7,
	0x57, 0xab, 0xd7,
	0x83, 0xab, 0xd7,
	0xab, 0xab, 0xd7,
	0xd7, 0xab, 0xd7,
	0xff, 0xab, 0xd7,
	0x00, 0xd7, 0xd7,
	0x57, 0xd7, 0xd7,
	0x83, 0xd7, 0xd7,
	0xab, 0xd7, 0xd7,
	0xd7, 0xd7, 0xd7,
	0xff, 0xd7, 0xd7,
	0x00, 0xff, 0xd7,
	0x57, 0x57, 0xff,
	0x83, 0x57, 0xff,
	0xab, 0x57, 0xff,
	0xd7, 0x57, 0xff,
	0xff, 0x57, 0xff,
	0x00, 0x83, 0xff,
	0x57, 0x83, 0xff,
	0x83, 0x83, 0xff,
	0xab, 0x83, 0xff,
	0xd7, 0x83, 0xff,
	0xff, 0x83, 0xff,
	0x00, 0xab, 0xff,
	0x57, 0xab, 0xff,
	0xff, 0xfb, 0xf0,
	0xa0, 0xa0, 0xa4,
	0x80, 0x80, 0x80,
	0xff, 0x00, 0x00,
	0x00, 0xff, 0x00,
	0xff, 0xff, 0x00,
	0x00, 0x00, 0xff,
	0xff, 0x00, 0xff,
	0x00, 0xff, 0xff,
	0xff, 0xff, 0xff
};


/************************************************************/
/******************** RLE 8 Dekodierung *********************/
/************************************************************/
static uint8_t *decodeRLE8(uint8_t *buffer, uint16_t width, uint16_t height, unsigned long bperz, unsigned long w)
{
	uint8_t *ziel;
	uint8_t *oziel;
	unsigned long v;
	uint8_t v1, v2, dx, dy, rest;
	uint16_t x,	 y;

	(void) width;
	if ((ziel = SMalloc(w * height)) == 0)
		return NOMEM;

	memset(ziel, 0, w * (unsigned long) height);

	v = bperz - w;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			v1 = *buffer++;

			if (v1)						/* Encoded Run */
			{
				v2 = *buffer++;

				x += v1;

				while (v1--)
					*ziel++ = v2;
			}							/* x != 0 */
			else						/* Literal Run */
			{
				v2 = *buffer++;

				/**************** Steuersequenz *************/
				if (v2 < 3)
				{
					switch (v2)
					{
					case 0:			/* Zeile beenden, nur wenn nicht am Anfang einer Zeile */
						if (x)
						{
							ziel += (bperz - x);
							x = (uint16_t) bperz;
						}
						break;
					case 1:			/* Grafik beenden */
						x = (uint16_t) bperz;
						y = height;
						break;
					case 2:			/* Verschiebung von x, y */
						dx = *buffer++;
						ziel += dx;
						x += dx;

						dy = *buffer++;
						ziel += (unsigned long) dy *bperz;

						y += dy;
						break;
					}
				}
				/************** RLE8 kodiert ******************/
				else
				{
					x += v2;
					rest = v2 & 1;		/* Modulo 2 */

					while (v2--)
						*ziel++ = *buffer++;
					if (rest)
						buffer++;
				}
			}							/* x == 0 */
		} while (x < bperz);
		ziel -= v;
	} while (++y < height);

	ziel = oziel;

	return ziel;
}



/************************************************************/
/******************** RLE 4 Dekodierung *********************/
/************************************************************/
static uint8_t *decodeRLE4(uint8_t *buffer, uint16_t width, uint16_t height, unsigned long bperz)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t *opixbuf;
	uint8_t val;
	uint8_t v1, v2, dx, dy, rest;
	uint16_t x, y;
	uint16_t k = 0;
	unsigned long memwidth;
	unsigned long planelength;

	memwidth = (unsigned long) ((width + 7) / 8) * 4L;

	if ((ziel = SMalloc(memwidth * height + 1L)) == 0)
		return NOMEM;

	pixbuf = (uint8_t *)Malloc((unsigned long) width + 4L);
	memset(ziel, 0, (unsigned long) width + 4L);

	planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

	bperz <<= 1;

	oziel = ziel;
	opixbuf = pixbuf;

	y = 0;
	do
	{
		x = 0;
		do
		{
			v1 = *buffer++;

			if (v1)						/* Encoded Run */
			{
				v2 = *buffer++;

				x += v1;

				rest = v1 & 1;
				v1 >>= 1;
				while (v1--)
				{
					*pixbuf++ = v2 >> 4;
					*pixbuf++ = v2 & 0x0f;
				}

				if (rest)
					*ziel++ = v2 >> 4;
			}							/* v1 != 0 */
			else						/* Literal Run */
			{
				v2 = *buffer++;

				/**************** Steuersequenz *************/
				if (v2 < 3)
				{
					switch (v2)
					{
					case 0:			/* Zeile beenden, nur wenn nicht am Anfang einer Zeile */
						if (x)
						{
							ziel += (bperz - x);
							x = (uint16_t) bperz;
						}
						break;
					case 1:			/* Grafik beenden */
						k = height - y;
						x = (uint16_t) bperz;
						y = height;
						break;
					case 2:			/* Verschiebung von x, y */
						dx = *buffer++;
						x += dx;

						dy = *buffer++;
						y += dy;

						k = dy;

						while (k--)
						{
							pixbuf = opixbuf;
							ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);

							memset(pixbuf, 0x0, (unsigned long) width);
						}
						break;
					}
				}
				/************** RLE4 kodiert ******************/
				else
				{
					x += v2;

					v2 >>= 1;
					rest = v2 & 1;		/* Modulo 2 */

					while (v2--)
					{
						val = *buffer++;
						*pixbuf++ = val >> 4;
						*pixbuf++ = val & 0x0f;
					}

					if (rest)
						buffer++;
				}
			}							/* v1 == 0 */
		} while (x < bperz);

		pixbuf = opixbuf;
		ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);
	} while (++y < height);

	ziel = oziel;

	Mfree(pixbuf);

	return ziel;
}


/****************************************************/
/****************** BMP auslesen ********************/
/****************************************************/
static uint8_t *getBMPdata(uint8_t *buffer, uint16_t height, unsigned long bperz, unsigned long w)
{
	uint8_t *ziel;
	uint8_t *oziel;
	unsigned long v;
	uint16_t x, y;

	if ((ziel = SMalloc(w * height)) == 0)
		return NOMEM;

	v = bperz - w;

	oziel = ziel;

	y = 0;
	do
	{
		x = 0;
		do
		{
			*ziel++ = *buffer++;
		} while (++x < w);
		buffer += v;
	} while (++y < height);

	ziel = oziel;

	return ziel;
}


/****************************************************/
/********** 4 Bit BMP auslesen und wandeln **********/
/****************************************************/
static uint8_t *getBMPdata4(uint8_t *buffer, uint16_t width, uint16_t height, unsigned long bperz, unsigned long w)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	unsigned long v;
	uint8_t val;
	uint16_t x, y;
	unsigned long memwidth;
	unsigned long planelength;

	memwidth = ((width + 7) / 8) * 4L;

	if ((ziel = SMalloc(memwidth * height + 1L)) == 0)
		return NOMEM;

	memset(ziel, 0x0, memwidth * height + 1L);

	oziel = ziel;

	v = bperz - w;

	planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

	pixbuf = (uint8_t *) Malloc(width + 7);

	y = 0;
	do
	{
		x = 0;
		do
		{
			val = *buffer++;
			pixbuf[x++] = val >> 4;
			pixbuf[x++] = val & 0x0f;
		} while (x < width);

		ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);
		buffer += v;
	} while (++y < height);

	ziel = oziel;

	Mfree(pixbuf);

	return ziel;
}


/****************************************************/
/****** Bitmap-Datenbereich aus CLP auslesen ********/
/****************************************************/
static uint8_t *getCLP(GARGAMEL *picture, unsigned long BMPOffset, unsigned long *UsedColors)
{
	uint8_t *buffer;
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t BitsPerPixel;
	unsigned long v;
	uint8_t val;
	char dummy[3];
	char impmessag[21];
	uint16_t *buffer16;
	uint16_t *ziel16;
	uint16_t width;
	uint16_t height;
	uint16_t x, y;
	unsigned long bperz;
	unsigned long w;
	unsigned long memwidth;
	unsigned long planelength;

	buffer = (uint8_t *) picture->smurf_pic->pic_data + BMPOffset;

	width = swap_word(*(uint16_t *) (buffer + 0x02));
	height = swap_word(*(uint16_t *) (buffer + 0x04));
	bperz = swap_word(*(uint16_t *) (buffer + 0x06));

	BitsPerPixel = *(buffer + 0x09);

	*UsedColors = 1 << BitsPerPixel;

	strcpy(picture->smurf_pic->format_name, "Windows Clipboard (CLP)");
	picture->smurf_pic->pic_width = width;
	picture->smurf_pic->pic_height = height;
	picture->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Windows CLP ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	picture->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 1)
	{
		w = (unsigned long) ((width + 7) / 8);
		memwidth = (unsigned long) w *8L;
	} else if (BitsPerPixel == 4)
	{
		w = (unsigned long) ((width + 3) / 4) * 2L;
		memwidth = (unsigned long) ((width + 7) / 8) * 8L;
	} else
	{
		w = ((unsigned long) width * BitsPerPixel) >> 3;
		memwidth = (unsigned long) width;
	}

	if ((ziel = SMalloc(((memwidth * (long) height * BitsPerPixel) >> 3) + 1L)) == 0)
		return NOMEM;

	memset(ziel, 0, ((memwidth * (long) height * BitsPerPixel) >> 3) + 1L);

	/* buffer auf Bilddaten setzen */
	buffer += 0x0e;

	oziel = ziel;

	if (BitsPerPixel == 4)
	{
		planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

		pixbuf = (uint8_t *) Malloc(width + 7);

		y = 0;
		do
		{
			x = 0;
			do
			{
				val = *buffer++;
				pixbuf[x++] = val >> 4;
				pixbuf[x++] = val & 0x0f;
			} while (x < width);
			ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);
		} while (++y < height);

		Mfree(pixbuf);
	} else if (BitsPerPixel == 16)
	{
		v = (bperz - w) / 2;
		w = w / 2;

		buffer16 = (uint16_t *) buffer;
		ziel16 = (uint16_t *) ziel;

		y = 0;
		do
		{
			x = 0;
			do
			{
				*ziel16++ = swap_word(*buffer16++);
			} while (++x < w);
			buffer16 += v;
		} while (++y < height);
	} else
	{
		v = bperz - w;

		y = 0;
		do
		{
			x = 0;
			do
			{
				*ziel++ = *buffer++;
			} while (++x < w);
			buffer += v;
		} while (++y < height);
	}

	ziel = oziel;

	if (BitsPerPixel == 1 || BitsPerPixel == 4)
		picture->smurf_pic->format_type = FORM_STANDARD;
	else
		picture->smurf_pic->format_type = FORM_PIXELPAK;
	picture->smurf_pic->col_format = RGB;

	return ziel;
}


/**********************************************************/
/*			Orientierung von oben nach unten 			  */
/**********************************************************/
static void SwitchOrient(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t BitsPerPixel)
{
	uint8_t *obuf;
	uint8_t *buffer2;
	uint8_t *mirrbuf;
	uint8_t p;
	uint8_t Planes;
	uint16_t y;
	unsigned long realwidth;

	if (BitsPerPixel == 1 || BitsPerPixel == 4)
	{
		realwidth = (width + 7) / 8;

		Planes = BitsPerPixel;
	} else
	{
		if (BitsPerPixel == 8)
			realwidth = (unsigned long) width;
		else
			realwidth = (unsigned long) width * 3L;

		Planes = 1;
	}

	if ((mirrbuf = SMalloc(realwidth)) != NULL)
	{
		obuf = buffer;

		p = 0;
		do
		{
			buffer += p * (unsigned long) height *realwidth;

			buffer2 = buffer + (unsigned long) (height - 1) * realwidth;

			y = (height >> 1);
			do
			{
				memcpy(mirrbuf, buffer, realwidth);
				memcpy(buffer, buffer2, realwidth);
				memcpy(buffer2, mirrbuf, realwidth);
				buffer += realwidth;
				buffer2 -= realwidth;
			} while (--y);

			buffer = obuf;
		} while (++p < Planes);

		SMfree(mirrbuf);
	}
}


static void invert_1Bit(uint8_t *data, uint16_t width, uint16_t height)
{
	long length;
	uint8_t v;
	
	length = (unsigned long) ((width + 7) >> 3) * (unsigned long) height;

	do
	{
		v = ~*data;
		*data++ = v;
	} while (--length);
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				BMP-/CLP-Importer (BMP/CLP)			*/
/*		Windows 2.x, Windows 3.x, OS/2 1.x,			*/
/*		OS/2 2.x, Clipboard,						*/
/*		1, 4, 8, 16, 24 Bit, 						*/
/*		unkomprimiert, RLE4 und RLE8				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *ziel = 0;
	uint8_t *buffer;
	uint8_t *pal;
	const uint8_t *palbuf;
	char dummy[3];
	char impmessag[21];
	uint8_t BitsPerPixel;
	BOOLEAN WinBMPv1 = FALSE;
	BOOLEAN WinBMPv3 = FALSE;
	BOOLEAN Os2BMPv1 = FALSE;
	BOOLEAN Os2BMPv21 = FALSE;
	BOOLEAN Os2BMPv22 = FALSE;
	BOOLEAN RIFF = FALSE;
	BOOLEAN CLP = FALSE;
	BOOLEAN DIB = FALSE;

	uint16_t x;
	uint16_t width;
	uint16_t height;
	uint16_t PicOrient = 0;
	uint16_t CLPBereiche;
	uint16_t CLPFormat;

	unsigned long BMPOffset = 0;
	unsigned long DatenOffset = 0;
	unsigned long PalettenOffset = 0;
	unsigned long UsedColors = 0;
	unsigned long KompFlag = 0;
	unsigned long bperz;
	unsigned long w;

	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	buffer = smurf_struct->smurf_pic->pic_data;

	if (*(uint32_t *) buffer == 0x52494646L)	/* 'RIFF' Container */
	{
		if (strncmp((char *)buffer + 8, "RDIBdata", 8) != 0)
		{
			form_alert(1, ERROR1);
			return M_UNKNOWN_TYPE;
		}

		RIFF = TRUE;
		/* RIFF Header abschneiden */
		BMPOffset = 0x08 + 0x0c;
		buffer += BMPOffset;
		strcpy(smurf_struct->smurf_pic->format_name, "RIFF Bitmap (RIF)");
	} else if (*buffer == 0x50 && *(buffer + 1) == 0xc3)
	{
		CLPBereiche = swap_word(*(uint16_t *) (buffer + 0x02));

		/* Auf Anfang des ersten Datenbereiches setzen */
		buffer += 0x04;

		while (CLPBereiche-- && DIB == FALSE)
		{
			CLPFormat = swap_word(*(uint16_t *) buffer);

			switch (CLPFormat)
			{
			case CF_DIB:
				BMPOffset = swap_long(*(uint32_t *) (buffer + 0x06));
				DIB = TRUE;
				CLP = FALSE;
				break;
			case CF_BITMAP:
			case CF_BITMAP2:
				BMPOffset = swap_long(*(uint32_t *) (buffer + 0x06));
				DIB = FALSE;
				CLP = TRUE;
				break;
/* Bis auf weiteres gekillt, da mit der mitgelieferten Palette bloû Scheiûe rauskommt */
/* und auch das Windows Clipboard diese Palette anscheinend nicht auswertet. */
#if 0
			case CF_PALETTE:
				PalettenOffset = swap_long(*(uint32_t *) (buffer + 0x06));

				palbuf = smurf_struct->smurf_pic->pic_data + PalettenOffset;

				UsedColors = swap_word(*(unsigned short *) (palbuf + 0x02));

				/* palbuf auf Anfang Palette setzen */
				palbuf += 0x04;

				pal = smurf_struct->smurf_pic->palette;

				for (x = 0; x < UsedColors; x++)
				{
					*pal++ = *palbuf++;
					*pal++ = *palbuf++;
					*pal++ = *palbuf++;
					palbuf++;
				}
				break;
#endif
			default:
				break;
			}
			buffer += 0x59;				/* Grîûe einer Datenbereich-Definition */
		}

		if (CLP == FALSE && DIB == FALSE)
			return M_UNKNOWN_TYPE;

		if (CLP)
		{
			if ((ziel = getCLP(smurf_struct, BMPOffset, &UsedColors)) == NOMEM)
			{
				return M_MEMORY;
			} else
			{
				smurf_struct->smurf_pic->pic_data = ziel;

				if (PalettenOffset == 0)
				{
					palbuf = winstdpal;
					pal = smurf_struct->smurf_pic->palette;

					for (x = 0; x < UsedColors; x++)
					{
						*pal++ = *palbuf++;
						*pal++ = *palbuf++;
						*pal++ = *palbuf++;
					}
				}

				return M_PICDONE;
			}
		} else
		{
			buffer = (uint8_t *) smurf_struct->smurf_pic->pic_data + BMPOffset;	/* buffer auf Datenbereich setzen */
		}
	}

	/***********************************************************/
	/************** Auslesen der BMP-Versionsart ***************/
	/***********************************************************/
	if (*(uint16_t *) buffer == 0x424d)
	{
		DatenOffset = swap_long(*(uint32_t *) (buffer + 14));
		if (DatenOffset == 12)
		{
			Os2BMPv1 = TRUE;
			if (RIFF == FALSE)
				strcpy(smurf_struct->smurf_pic->format_name, "OS2 v1.x Bitmap (BMP)");
			strcpy(impmessag, "OS/2 1.x BMP ");
		} else if (DatenOffset == 64)
		{
			Os2BMPv21 = TRUE;
			if (RIFF == FALSE)
				strcpy(smurf_struct->smurf_pic->format_name, "OS2 v2.x Bitmap (BMP)");
			strcpy(impmessag, "OS/2 2.x BMP ");
		} else
		{
			WinBMPv3 = TRUE;
			if (RIFF == FALSE)
				strcpy(smurf_struct->smurf_pic->format_name, "Windows 3.x Bitmap (BMP)");
			strcpy(impmessag, "Win 3.x BMP ");
		}
	} else if (*(uint16_t *) buffer == 0x4241 && *(uint16_t *) (buffer + 0x0e) == 0x424d)
	{
		Os2BMPv22 = TRUE;
		if (RIFF == FALSE)
			strcpy(smurf_struct->smurf_pic->format_name, "OS2 v2.x Bitmap (BMP)");
		strcpy(impmessag, "OS/2 2.x BMP ");
	} else if (*(uint16_t *) buffer == 0x0 && *(uint16_t *) (buffer + 2) != 0x0)
	{
		/* lt. Doku sind die ersten beiden Bytes 0, diese lasche Erkennung fÅhrt jedoch
		   leicht zu Verirrungen und so wird schon mal z.B. ein PICT als Win 1.x BMP erkannt,
		   da hier ja die ersten 512 Bytes 0 sind. Deshalb die Abfrage auf Byte 3 und 4 != 0
		   was zwar auch nicht das Nonplusultra ist, aber doch eine Verbesserung */
		WinBMPv1 = TRUE;
		if (RIFF == FALSE)
			strcpy(smurf_struct->smurf_pic->format_name, "Windows 1.x Bitmap (BMP)");
		strcpy(impmessag, "Win 1.x BMP ");
	} else if (DIB && *buffer == 0x28)
	{
		WinBMPv3 = TRUE;
		strcpy(smurf_struct->smurf_pic->format_name, "Windows 3.x Bitmap (BMP)");
		strcpy(impmessag, "Win 3.x BMP ");
	} else
	{
		return M_INVALID;
	}

	/***********************************************************/
	/**************** Auslesen der Bildgeometrie ***************/
	/***********************************************************/
	if (Os2BMPv1 || WinBMPv3)	/* OS/2 Version 1.x BMP oder Windows BMP */
	{
		if (DIB == FALSE)
		{
			/* der Fileheader fehlt bei includeten BMPs leider ... */
			DatenOffset = swap_long(*(uint32_t *) (buffer + 0x0a));
			/* Auf Anfang Bitmap Header setzen */
			buffer += 0x0e;
		} else
		{
			/* ... deshalb muû der Abstand angenommen werden */
			DatenOffset = 0x28;
		}

		PalettenOffset = swap_long(*(uint32_t *) buffer);

		if (Os2BMPv1)
		{
			width = swap_word(*(uint16_t *) (buffer + 0x04));
			height = swap_word(*(uint16_t *) (buffer + 0x06));

			BitsPerPixel = swap_word(*(uint16_t *) (buffer + 0x0a));
		} else
		{
			width = swap_long(*(uint32_t *) (buffer + 0x04));
			height = swap_long(*(uint32_t *) (buffer + 0x08));

			BitsPerPixel = swap_word(*(uint16_t *) (buffer + 0x0e));

			KompFlag = swap_long(*(uint32_t *) (buffer + 0x10));
			if (BitsPerPixel < 16)
				UsedColors = swap_long(*(uint32_t *) (buffer + 0x20));
			if ((int16_t) height < 0)
			{
				height = -height;
				PicOrient = 1;
			}
		}

		if (BitsPerPixel != 1 && BitsPerPixel != 4 && BitsPerPixel != 8 && BitsPerPixel != 24)
			return M_PICERR;
	}									/* OS/2 1.x und Win Format */
	else if (Os2BMPv21 || Os2BMPv22)	/* OS/2 Version 2.x BMP beide Varianten */
	{
		/* Bitmaparrayfileheader Åberspringen */
		if (Os2BMPv22)
			buffer += 0x0e;

		DatenOffset = swap_long(*(uint32_t *) (buffer + 0x0a));

		/* Auf Anfang Bitmapheader setzen */
		buffer += 0x0e;

		PalettenOffset = swap_long(*(uint32_t *) buffer);

		width = swap_long(*(uint32_t *) (buffer + 0x04));
		height = swap_long(*(uint32_t *) (buffer + 0x08));

		BitsPerPixel = swap_word(*(uint16_t *) (buffer + 0x0e));

		KompFlag = swap_long(*(uint32_t *) (buffer + 0x10));
		if (BitsPerPixel < 16)
			UsedColors = swap_long(*(uint16_t *) (buffer + 0x20));

		if (Os2BMPv21)
			PicOrient = swap_word(*(uint16_t *) (buffer + 0x2c));

		if (BitsPerPixel != 1 && BitsPerPixel != 4 && BitsPerPixel != 8 && BitsPerPixel != 24)
			return M_PICERR;
	}									/* OS/2 2.x Format */
	else if (WinBMPv1)
	{
		width = swap_word(*(uint16_t *) (buffer + 0x02));
		height = swap_word(*(uint16_t *) (buffer + 0x04));

		BitsPerPixel = *(buffer + 0x09);

		DatenOffset = 0x0a;
	}									/* Win 1.x/2.x Format */

	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	/***********************************************************/
	/**************** Auswerten der Bilddaten ******************/
	/***********************************************************/

	/* öbertragen der Palette */
	if (BitsPerPixel < 16)
	{
		if (UsedColors == 0)
			UsedColors = 1 << BitsPerPixel;

		pal = smurf_struct->smurf_pic->palette;

		if (WinBMPv1)
		{
			/* Hier muû die Windows Standardpalette rein */
			pal[0] = 255;
			pal[1] = 255;
			pal[2] = 255;
			pal[3] = 0;
			pal[4] = 0;
			pal[5] = 0;
		} else
		{
			palbuf = buffer + PalettenOffset;

			for (x = 0; x < UsedColors; x++)
			{
				*pal++ = palbuf[2];
				*pal++ = palbuf[1];
				*pal++ = palbuf[0];
				palbuf += 3;
				if (!Os2BMPv1)
					palbuf++;
			}
		}
	}


	/* Anpassen der Bilddimensionen */
	switch (BitsPerPixel)
	{
	case 1:
		bperz = (unsigned long) (width + 7) / 8L;
		bperz = (unsigned long) (((bperz + 3) / 4) * 4);
		w = (unsigned long) (width + 7) / 8L;

		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		break;
	case 4:
		bperz = (unsigned long) ((width + 1) / 2);
		bperz = ((bperz + 3) / 4) * 4;
		w = (unsigned long) (width + 1) / 2L;

		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
		break;
	case 8:
		bperz = (unsigned long) (((width + 3) / 4) * 4);
		w = (unsigned long) width;

		smurf_struct->smurf_pic->col_format = RGB;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
		break;
	case 24:
		bperz = (unsigned long) width * 3L;

		bperz = ((bperz + 3) / 4) * 4;
		w = (unsigned long) width * 3L;

		smurf_struct->smurf_pic->col_format = BGR;
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
		break;
	default:
		return M_INVALID;
	}

	/* der Fileheader fehlt bei includeten BMPs leider und der Abstand */
	/* muû deshalb manuell um die Palette erweitert werden */
	if (DIB)
		DatenOffset += UsedColors * 4;
	buffer = (uint8_t *) smurf_struct->smurf_pic->pic_data + BMPOffset + DatenOffset;	/* buffer auf Bilddatenanfang setzen */

	if (KompFlag == 0)					/* unkomprimiert */
	{
		if (BitsPerPixel == 4)
			ziel = getBMPdata4(buffer, width, height, bperz, w);
		else
			ziel = getBMPdata(buffer, height, bperz, w);

		if (ziel == NOMEM)
			return M_MEMORY;
	} else if (KompFlag == 1)			/* RLE 8 */
	{
		ziel = decodeRLE8(buffer, width, height, bperz, w);

		if (ziel == NOMEM)
			return M_MEMORY;
	} else if (KompFlag == 2)			/* RLE 4 */
	{
		ziel = decodeRLE4(buffer, width, height, bperz);

		if (ziel == NOMEM)
			return M_MEMORY;
	}

	SMfree(smurf_struct->smurf_pic->pic_data);

	if (PicOrient == 0)
		SwitchOrient(ziel, width, height, BitsPerPixel);
	smurf_struct->smurf_pic->pic_data = ziel;

	pal = smurf_struct->smurf_pic->palette;
	if (BitsPerPixel == 1 && ((pal[0] == 0 && pal[3] == 255) || WinBMPv1))
		invert_1Bit(ziel, width, height);

	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	return M_PICDONE;
}
