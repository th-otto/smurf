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


/* TODO: */
/* Standardpalette auch f�r 8 Farben umsetzen */

/* =========================================================*/
/*						GEM-Image Decoder					*/
/* Version 0.1  --  24.12.95								*/
/*	  1 Bit IMG	im Standardformat							*/
/* Version 0.2  --  xx.xx.xx								*/
/*	  4 + 8 + 24 Bit (X)IMG									*/
/*	  imgtype = I: Image, imgtype = X: XImage				*/
/* Version 0.3  --  27.01.96								*/
/*	  Die sp�te (aber immerhin) Erkenntnis, da� im XImage	*/
/*	  Palettenwerte im Promillebereich benutzen hatte bahn-	*/
/*	  brechende Wirkung! Olaf machte eine Menge Wind (ins	*/
/*	  Bild, manche konnten darin auch um 90 Grad gedrehte	*/
/*	  Eiszapfen sehen), Dale sagte gar nichts und ich		*/
/*	  schrieb eine bl�de und viel zu lange Anmerkung in die	*/
/*	  Source.												*/
/* Version 0.4  --  11.02.96								*/
/*	  GEM-Views Truecolor Images sind auf 16 Pixel alligned,*/
/*	  dank des Tips von Dieter Fiebelkorn werden nun alle	*/
/*	  GEM-View-24 Bit Images richtig dargestellt.			*/
/* Version 0.5  --  11.02.96								*/
/*	  Kleiner Bug seit der 0.3, die includeten Palettenwerte*/
/*	  waren schon korrekt in Werten bis 256, wurden jedoch	*/
/*	  trotzdem umgerechnet.									*/
/* Version 0.6  --  18.02.96								*/
/*	  Probleme mit IMGs (Indizes auf Farbpalette) sind bei-	*/
/*	  gelegt. 256 wurde in ein char geschoben ...			*/
/* Version 0.7  --  25.02.96								*/
/*	  Probleme mit yel_ston.IMG (7 Bit Image) behoben.		*/
/* Version 0.8  --  28.05.96								*/
/*	  Ab jetzt wird direkt addressiert und nicht mehr per	*/
/*	  Index. 25% Geschwindigkeitsgewinn sind der Lohn.		*/
/* Version 0.9  --  12.01.97								*/
/*	  Bugfix, bei nicht auf 16 Pixel ausgerichteten TC-IMGs	*/
/*	  kopierte der vrc zu viel.								*/
/* Version 1.0  --  17.01.97								*/
/*	  VRC-Erkennung aus x-Schleife genommen. Das ist etwas	*/
/*	  schneller und auch korrekt.							*/
/* Version 1.1 --  22.01.97									*/
/*	  CMY-IMGs werden korrekt an Smurf weitergegeben.		*/
/* Version 1.2 --  12.02.97									*/
/*	  Verbesserte Erkennung, bzw. Ablehnung falscher Files	*/
/* Version 1.3  --  03.12.97								*/
/*	  �berwachung gegen zuviele vrc-Wiederholungen im Bild.	*/
/* Version 1.4  --  06.01.98								*/
/*	  in Versin 1.3 wurde y f�lschlicherweise in der p-		*/
/*	  Schleife hochgez�hlt - das war zwar bei 1 Bit-Bildern	*/
/*	  ok, aber Farbbildern wurden damit zermatscht.			*/
/*	  Hilfsvariable y2 eingef�hrt, damit geht's nun.		*/
/* Version 1.5  --  xx.xx.97								*/
/*	  PixArts 24 Bit-Images werden nun  korrekt gelesen.	*/
/* Version 1.6  --  01.01.2009								*/
/*	  Fehlermeldung bei TIMG GEM-Images Bilder              */
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[1][TrueColor GEM-Image werden noch nicht|unterst�tzt.][ OK ]"
#define ERROR2 "[1][Ventura-Images werden noch nicht|unterst�tzt. Bild bitte an uns einschicken!][ OK ]"
#elif COUNTRY==0
#define ERROR1 "[1][TrueColor GEM-Image werden noch nicht|unterst�tzt.][ OK ]"
#define ERROR2 "[1][Ventura images are not yet|supported. Please send the image to use!][ OK ]"
#elif COUNTRY==2
#define ERROR1 "[1][TrueColor GEM-Image werden noch nicht|unterst�tzt.][ OK ]"
#define ERROR2 "[1][Ventura images are not yet|supported. Please send the image to use!][ OK ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define GVW	1
#define PA	2
#define GC	3

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

/* Get 16 Pixel (Interleaved Standard Format) Assembler-Rout */
void get_IBPLine(uint8_t *dest, uint8_t *src, long lineplanelen) ASM_NAME("_get_IBPLine");

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = {
	"GEM-(X)IMG Importer",
	0x0160,
	"Christian Eyrich",
	{ "IMG", "", "", "", "", "", "", "", "", "" },
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

static const char *fileext(const char *filename)
{
	const char *extstart;

	if ((extstart = strrchr(filename, '.')) != NULL)
		extstart++;
	else
		extstart = strrchr(filename, '\0');

	return extstart;
}


/* decodiert 24 Bit IMGs im Pixart-Format (Standardformat mit 24 Planes) */
static void decode_PA_TC(uint8_t *ziel, uint8_t *buffer, unsigned short height, unsigned short PattLength, long w, long srcw)
{
	uint8_t *ooziel;
	uint8_t *pixbuf;
	unsigned short j;
	uint8_t p;
	uint8_t vrc;
	uint8_t v1, v2, v3;
	unsigned short i, x, y;
	unsigned long pla;

	pixbuf = (uint8_t *) SMalloc(w * 24L + 32);	/* Puffer f�r Standarddaten */

	ooziel = ziel;
	y = 0;
	do
	{
		vrc = 1;
		pla = y * 24L * srcw;			/* Abstand dieser Plane vom Bildanfang */
		for (p = 0; p < 24; p++)		/* Planes */
		{
			v1 = *buffer++;
			v2 = *buffer++;
			v3 = *buffer++;
			if (v1 == 0x0 && v2 == 0x0 && v3 == 0xff)	/* Vertical Replication */
				vrc = *buffer++;
			else
				buffer -= 3;

			x = 0;
			do
			{
				v1 = *buffer++;

				switch (v1)
				{
				case 0x0:				/* Pattern Run */
					v2 = *buffer++;
					x += (v2 * PattLength);
					for (i = 0; i < v2; i++)
						for (j = 0; j < PattLength; j++)
							*ziel++ = *(buffer + j);
					buffer += PattLength;
					break;
				case 0x80:				/* Bit String */
					v2 = *buffer++;
					x += v2;
					for (i = 0; i < v2; i++)
						*ziel++ = *buffer++;
					break;
				default:				/* Solid Run */
					v2 = v1 & 0x7f;
					x += v2;
					if (v1 > 0x80)
						while (v2--)
							*ziel++ = 0xff;
					else
						while (v2--)
							*ziel++ = 0x00;
					break;
				}						/* case */
			} while (x < w);
#if 0
			if (x != w)
			{
				printf("y: %u, p: %u\n", y, p);
				printf("x: %u, w: %lu\n", x, w);
				getch();
			}
#endif
#if 0
			for (i = 1; i < vrc; i++)	/* vrc, aber nicht auf sich selbst */
				memcpy(oziel + i * srcw, oziel, srcw);
#endif
		}
		y += vrc;
	} while (y < height);

	ziel = ooziel;

	pla = 24L * w;
	y = 0;
	do
	{
		memcpy(pixbuf, ziel, pla);
		get_IBPLine(ziel, pixbuf, w);
		ziel += pla;
	} while (++y < height);

	SMfree(pixbuf);
}


/* decodiert alle 1-8 Bit IMGs und 24 Bit IMGs im GEM-View-Format */
static void decode_GVW_TC(uint8_t *ziel, uint8_t *buffer, unsigned short height, unsigned short PattLength, uint8_t Planes, long w, long srcw)
{
	uint8_t *oziel;
	unsigned short j;
	uint8_t p;
	uint8_t vrc;
	uint8_t v1, v2, v3;
	unsigned short i, x, y, y2;
	unsigned long pla, plh, plo;

	plh = (unsigned long) height * srcw;	/* H�he einer Plane in Byte */

	oziel = ziel;
	y = 0;
	do
	{
		vrc = 1;
		plo = y * srcw;					/* Offset vom Planeanfang in Bytes */
		for (p = 0; p < Planes; p++)
		{
			pla = plh * p;				/* Abstand dieser Plane vom Bildanfang */
			ziel = oziel + pla + plo;	/* Zieladresse der dekodierten Scanline */

			v1 = *buffer++;
			v2 = *buffer++;
			v3 = *buffer++;
			if (v1 == 0x0 && v2 == 0x0 && v3 == 0xff)	/* Vertical Replication */
				vrc = *buffer++;
			else
				buffer -= 3;

			x = 0;
			do
			{
				v1 = *buffer++;

				switch (v1)
				{
				case 0x0:				/* Pattern Run */
					v2 = *buffer++;
					x += (v2 * PattLength);
					for (i = 0; i < v2; i++)
						for (j = 0; j < PattLength; j++)
							*ziel++ = *(buffer + j);
					buffer += PattLength;
					break;
				case 0x80:				/* Bit String */
					v2 = *buffer++;
					x += v2;
					for (i = 0; i < v2; i++)
						*ziel++ = *buffer++;
					break;
				default:				/* Solid Run */
					v2 = v1 & 0x7f;
					x += v2;
					if (v1 > 0x80)
						while (v2--)
							*ziel++ = 0xff;
					else
						while (v2--)
							*ziel++ = 0x00;
					break;
				}						/* case */
			} while (x < w);
#if 0
			if (x > w)
			{
				printf("y: %u, p: %u\n", y, p);
				printf("x: %u, w: %lu\n", x, w);
				(void)Cnecin();
			}
#endif
			y2 = y;
			/* vrc, aber nicht auf sich selbst */
			for (i = 1; i < vrc && ++y2 < height; i++)
				memcpy(oziel + pla + (y2 * srcw), oziel + pla + plo, srcw);
		}
		y += vrc;
	} while (y < height);
}


#if 0

/* From the imgtool from Guido Vollbeding */

/* Here is the one and only Level-3 decoder...
 * It is designed to be efficient as well as flexible in usage,
 * and it is capable of decoding all Level 1, 2, and 3 image files
 * without explicitly checking for level number of input stream.
 * So this one decoder is really sufficient, there is no need for
 * lower level decoders. In fact, a lower level decoder could be
 * programmed somewhat more straightforward by assuming, that the
 * line limit is not reached during inner decoding loops, but this
 * approach would have the disadvantage to loose reliability on
 * messy input data. The Level-3 decoder, however, automatically
 * checks for buffer overflows at any time, so it is the most
 * reliable and should work without any problems on arbitrary
 * messy data input.
 *
 * Aside from getting away with any line limit coding (but going
 * on to require only a single line buffer by the given decoding
 * scheme!), Level-3 provides an 'extended vertical run mode',
 * which is introduced with Level-2. This is quite easy to
 * implement in the decoder, and is also easy to exploit in
 * encoding. It may save a lot of bytes on certain images,
 * especially desktop snapshots or line drawings.
 * This mode places a run code in the stream for a series of bytes
 * in the line that equals the above line. This may be considered
 * as a generalization of the known 'vertical run code', which is
 * used for (multiple) full line repeats. This vrc has a 'stuffed
 * prefix byte' (0xFF), and the new generalized vertical run is
 * introduced as an extension of this stuffing byte by allowing it
 * to be an arbitrary value to be 0xFF for known full vrc and the
 * extended vertical run count minus one else. The 'minus one'
 * allows the real run count to be from 1 to 255 (0 is not needed,
 * and so is reserved for full vrc), which is also straightforward
 * to implement.
 * Implementation of extended vrc is very easy and efficient by
 * simply skipping the line output buffer bytewise runcount times.
 * This, however, assumes, that the line buffer always contains
 * the last decoded bytes. But this is easy to guarantee by only
 * forbid the put_line function to change the buffer contents.
 *
 * Furthermore, the Level-3 as well as Level-2 extends the meaning
 * of byte string and full vrc counts. We consider a zero value to
 * be a count of 256. This is straightforward to implement in the
 * decoder by using a do-while-decrement loop, without additional
 * code. The encoders can also take care of this easily.
 *
 * The decoder is implemented as endless loop for processing data.
 * Conditions for breakdown are assumed to be checked within the
 * callback functions (data_func and put_line) and to be handled
 * via a standard setjmp/longjmp scheme. This approach avoids
 * additional checking code in the decoder loop. If it should be
 * unlikely, it is possible to define the callback functions as
 * returning a boolean continue/abort value and adapt the decoder.
 * But, again, the actual approach is more compact and efficient.
 */

static void level_3_decode(FBUFPUB * input, IBUFPUB * image)
{
	uint8_t cdata;
	uint8_t c_val;
	uint8_t pat_ptr;
	short i;

	for (;;)
	{
		FGETC(input, cdata);
		if ((c_val = cdata) != 0)
		{
			if (cdata <<= 1)			/* solid run */
			{
				c_val >>= 7;
				do
					IPUTC(image, c_val);
				while (cdata -= 2);
			} else						/* byte string */
			{
				FGETC(input, cdata);
				do
					FICOPYC(input, image);
				while (--cdata);
			}
		} else
		{
			FGETC(input, cdata);
			if (cdata)					/* pattern run */
			{
				pat_ptr = image->pat_buf;
				for (i = image->pat_run; --i >= 0;)
					FGETC(input, *pat_ptr++);
				do
				{
					pat_ptr = image->pat_buf;
					for (i = image->pat_run; --i >= 0;)
						IPUTC(image, *pat_ptr++);
				} while (--cdata);
			} else						/* vertical replication */
			{
				FGETC(input, cdata);
				if (++cdata)
					do
						ISKIPC(image);
					while (--cdata);
				else
					FGETC(input, image->vrc);
			}
		}
	}
}


/* decodiert 24 Bit IMGs im Grafikkonverter-Format (3 Planes) */
static void decode_GC_TC(uint8_t *ziel, uint8_t *buffer, unsigned short width, unsigned short height, unsigned short PattLength, uint8_t Planes)
{
	int8_t shiftval;
	uint8_t *oziel;
	uint8_t *ziel2;
	unsigned short j;
	uint8_t p;
	uint8_t vrc;
	uint8_t v1, v2, v3;
	uint8_t val;
	unsigned short i, k, x, y, x2;
	unsigned short linelen;
	unsigned long w, memwidth, srcw, pla, plh, plo;


	plh = (unsigned long) height * srcw;	/* H�he einer Plane in Byte */

	linelen = (unsigned long) width * 3L;

	oziel = ziel;

	for (p = 0; p < Planes; p++)		/* Planes */
	{
		ziel = oziel + p;				/* Zieladresse der dekodierten Scanline */
		ziel2 = ziel;

		y = 0;
		do								/* height */
		{
			vrc = 1;
			plo = y * w;				/* Offset vom Planeanfang in Bytes */

			x = 0;
			x2 = 0;
			do							/* width */
			{
				v1 = *buffer++;
				v2 = *buffer++;
				v3 = *buffer++;
				if (v1 == 0x0 && v2 == 0x0 && v3 == 0xff)	/* Vertical Replication */
					vrc = *buffer++;
				else
					buffer -= 3;

				v1 = *buffer++;

				switch (v1)
				{
				case 0x0:				/* Pattern Run */
					v2 = *buffer++;
					x += (v2 * PattLength);
					for (i = 0; i < v2; i++)
						for (j = 0; j < PattLength; j++)
						{
							val = *(buffer + j);
							shiftval = 7;
							do
							{
								*ziel2 |= ((val >> shiftval) & 1) << p;
								ziel2 += 3;
								x2++;
							} while (--shiftval > -1 && x2 < width);	/* shiftval */
						}
					buffer += PattLength;
					break;
				case 0x80:				/* Bit String */
					v2 = *buffer++;
					x += v2;
					for (i = 0; i < v2; i++)
					{
						val = *buffer++;
						shiftval = 7;
						do
						{
							*ziel2 |= ((val >> shiftval) & 1) << p;
							ziel2 += 3;
							x2++;
						} while (--shiftval > -1 && x2 < width);
					}
					break;
				default:				/* Solid Run */
					v2 = v1 & 0x7f;
					x += v2;
					if (v1 > 0x80)
						while (v2--)
						{
							val = 0xff;
							shiftval = 7;
							do
							{
								*ziel2 |= ((val >> shiftval) & 1) << p;
								ziel2 += 3;
								x2++;
							} while (--shiftval > -1 && x2 < width);
					} else
						while (v2--)
						{
							val = 0x0;
							shiftval = 7;
							do
							{
								*ziel2 |= ((val >> shiftval) & 1) << p;
								ziel2 += 3;
								x2++;
							} while (--shiftval > -1 && x2 < width);
						}
					break;
				}						/* case */
			} while (x < width);		/* x */
#if 0
			if (x != width)
			{
				printf("x: %u, width: %u\n", x, w);
				(void)Cnecin();
			}
#endif
			ziel += linelen;

			for (i = 1; i < vrc; i++)	/* vrc, aber nicht auf sich selbst */
				memcpy(oziel + ((y + i) * w), oziel + plo, w);
			y += vrc;
		} while (y < height);			/* y */
	}									/* p */
}
#endif


static void convert_palette(uint8_t *pal, uint8_t *buffer, uint8_t BitsPerPixel, char imgtype)
{
	const uint8_t *plane_table;

	static uint8_t const table2bit[] = { 0, 2, 3, 1 };
	static uint8_t const table4bit[] = { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };

	unsigned short i, k, cols;

	struct pal
	{
		uint16_t r;
		uint16_t g;
		uint16_t b;
	} *ppal;

	const struct pal *stdpal;

	static struct pal const stdpal2bit[] = {
		{ 0xff, 0xff, 0xff },
		{ 0x00, 0x00, 0x00 },
		{ 0xff, 0x00, 0x00 },
		{ 0xd3, 0xd3, 0xd3 }
	};
	static struct pal const stdpal4bit[] = {
		{ 0xff, 0xff, 0xff },
		{ 0x00, 0x00, 0x00 },
		{ 0xff, 0x00, 0x00 },
		{ 0x00, 0xff, 0x00 },
		{ 0x00, 0x00, 0xff },
		{ 0x00, 0xff, 0xff },
		{ 0xff, 0xff, 0x00 },
		{ 0xff, 0x00, 0xff },
		{ 0xc0, 0xc0, 0xc0 },
		{ 0x80, 0x80, 0x80 },
		{ 0xb6, 0x00, 0x00 },
		{ 0x00, 0xb6, 0x00 },
		{ 0x00, 0x00, 0xb6 },
		{ 0x00, 0xb6, 0xb6 },
		{ 0xb6, 0xb6, 0x00 },
		{ 0xb6, 0x00, 0xb6 }
	};


	if (BitsPerPixel > 1)
	{
		cols = 1 << BitsPerPixel;

		if (imgtype == 'X')
		{
			ppal = (struct pal *) (buffer + 0x16);
			for (k = 0; k < cols; k++)
			{
				*pal++ = ((((unsigned long) ppal->r - ((ppal->r == 0x3e8) ? 1 : 0)) << 8) / 1000L);
				*pal++ = ((((unsigned long) ppal->g - ((ppal->g == 0x3e8) ? 1 : 0)) << 8) / 1000L);
				*pal++ = ((((unsigned long) ppal->b - ((ppal->b == 0x3e8) ? 1 : 0)) << 8) / 1000L);
				ppal++;
			}
		} else
		{
			/* umbiegen weil VDI-Indizes im Bild stehen */
			if (cols == 4 || cols == 16)
			{
				if (cols == 4)
				{
					stdpal = stdpal2bit;
					plane_table = table2bit;
				} else
				{
					stdpal = stdpal4bit;
					plane_table = table4bit;
				}

				for (i = 0; i < cols; i++)
				{
					k = plane_table[i];
					*pal++ = stdpal[k].r;
					*pal++ = stdpal[k].g;
					*pal++ = stdpal[k].b;
				}
			}
		}
	} else
	{
		pal[0] = 255;
		pal[1] = 255;
		pal[2] = 255;
		pal[3] = 0;
		pal[4] = 0;
		pal[5] = 0;
	}
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*			GEM (X)IMG Dekomprimierer (IMG)			*/
/*		1, 2, 4, 7, 8, 24 Bit,						*/
/*		eigene Komprimierungen						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t *ziel;
	char *fname;
	uint8_t BitsPerPixel;
	uint8_t Planes;
	unsigned short PattLength;
	char imgtype;
	uint8_t tcmode;
	uint8_t colmodel;
	char dummy[3];
	char impmessag[17];

	unsigned short width, height;
	unsigned short DatenOffset;

	unsigned long w, memwidth, srcw;

	enum
	{
		_RGB,
		_CMY,
		_HLS
	};


	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	buffer = smurf_struct->smurf_pic->pic_data;

	/* falls XIMG, steht in Byte 0x10 - 0x13  XIMG */
	if (*(uint32_t *) (buffer + 0x10) == 0x58494d47L) /* 'XIMG' */
		imgtype = 'X';
	else if (*(uint32_t *) (buffer + 0x10) == 0x54494d47L) /* 'TIMG' */
		imgtype = 'T';
	else
		imgtype = 'I';

	/* erstes Word ist leider nicht immer 1, da gibt es */
	/* zumindest ein Proramm, das 2 reinschreibt (s. DONNA.IMG) */
	fname = smurf_struct->smurf_pic->filename;
	if (*(unsigned int *) buffer > 0x02 ||
		(*(unsigned int *) (buffer + 0x02) != 8 && imgtype != 'X' && imgtype != 'T') ||
		stricmp(fileext(fname), "IMG") != 0)
		return M_INVALID;

	if (imgtype == 'T')
	{
		form_alert(1, ERROR1);
		return M_PICERR;
	}

	if (*((unsigned int *) (buffer + 0x02)) == 9)
	{
		form_alert(0, ERROR2);
		return (M_PICERR);
	}

	DatenOffset = *((unsigned int *) (buffer + 0x02)) << 1;

	BitsPerPixel = *(unsigned int *) (buffer + 0x04);

	PattLength = *(unsigned int *) (buffer + 0x06);
	if (BitsPerPixel == 24)
	{
		if (PattLength == 3)
			tcmode = GVW;
		else
			tcmode = PA;
	}

	width = *((unsigned int *) (buffer + 0x0c));
	height = *((unsigned int *) (buffer + 0x0e));

	if (imgtype == 'X')
		colmodel = *(buffer + 0x14);
	else
		colmodel = _RGB;

	strcpy(smurf_struct->smurf_pic->format_name, "GEM-Image .IMG");
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "GEM-Image ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	smurf_struct->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 24 && tcmode == GVW)
	{
		w = (width + 15) / 16;		/* Auf volle Byte gerundete Zeilenl�nge in Byte */
		memwidth = w * 16;
		w = (w * 16 * 3);
		srcw = (unsigned long) width *3L;

		Planes = 1;
	} else
	{
		w = (width + 7) / 8;		/* Auf volle Byte gerundete Zeilenl�nge in Byte */
		memwidth = w * 8;
		srcw = w;
		Planes = BitsPerPixel;
	}

	if ((ziel = SMalloc(((long) memwidth * (long) height * BitsPerPixel) >> 3)) == 0)
		return M_MEMORY;

	if (BitsPerPixel == 24 && tcmode == PA)
		decode_PA_TC(ziel, buffer + DatenOffset, height, PattLength, w, srcw);
#if 0
	else if (BitsPerPixel == 24 && tcmode == GC)
		decode_GC_TC(ziel, buffer + DatenOffset, height, PattLength, w, srcw);
#endif
	else
		decode_GVW_TC(ziel, buffer + DatenOffset, height, PattLength, Planes, w, srcw);

	smurf_struct->smurf_pic->pic_data = ziel;

	if (BitsPerPixel == 24)
	{
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	} else
	{
		convert_palette(smurf_struct->smurf_pic->palette, buffer, BitsPerPixel, imgtype);
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	}

	if (colmodel == _RGB)
		smurf_struct->smurf_pic->col_format = RGB;
	else
		smurf_struct->smurf_pic->col_format = CMY;

	SMfree(buffer);

	return M_PICDONE;
}
