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
/*					Macintosh PICT Decoder					*/
/* Version 0.1  --  03.11.96								*/
/*	  Erster Versuch. PICT ist ein Metafile, da momentan	*/
/*	  sowieso nur Rastergrafiken vom Smurf unterstÅtzt		*/
/*	  werden, wird auch nur der Teil gelesen. Das sollten	*/
/*	  aber sowieso die hÑufigsten sein.						*/
/*	  Komprimierung ist RLE und JPEG (Quickdraw);			*/
/*	  JPEG lasse ich auch mal auf unbestimmte Zeit weg -	*/
/*	  die GrÅnde dÅrften bekannt sein.						*/
/*	  1 Bit													*/
/*	  Version 1 und 2										*/
/* Version 0.2 -- 04.11.96									*/
/*	  8	Bit													*/
/* Version 0.3 -- 06.11.96									*/
/*	  16 Bit												*/
/* Version 0.4 -- 10.11.96									*/
/*	  Grîûere Umbauten (Dekoder in Funktionen);				*/
/*	  24 und 32 Bit, unkomprimierte							*/
/* Version 0.5 -- 12.11.96									*/
/*	  4 Bit													*/
/* Version 0.6 -- 19.11.96 - 26.11.96						*/
/*    Importer total umgeschrieben. Der Infoteil des		*/
/*    Files wird nun nicht mehr starr gelesen, sondern		*/
/*    interpretiert um alle Files lesen zu kînnen.			*/
/* Version 0.7 -- 16.03.97									*/
/*	  1 und 8 Bit Bilder mit ungerader ZeilenlÑnge werden	*/
/*	  nun auch korrekt dekodiert (die sind nÑmlich unver-	*/
/*	  stÑndlicherweise auf gerade Anzahl Pixel aligned		*/
/*	  abgespeichert. Zumindest bei RLE, wie es unkomprimiert*/
/*	  ist, weiû ich nicht, da der Grafikkonverter die nicht	*/
/*	  kann).												*/
/* Version 0.8 -- 30.05.98									*/
/*	  neue setpix_std_line eingebaut						*/
/* Version 0.9 -- xx.xx.xx									*/
/*	  Regions werden verstanden und ausgewertet.			*/
/* =========================================================*/

#include "country.h"

#if COUNTRY==1
#define ERROR1 "[2][Unbekannter Opcode! Soll versucht werden, | ihn einfach zu Åberlesen?][ Ja ][ Nein ]"
#elif COUNTRY==0
#define ERROR1 "[2][Unknown Opcode! Should one try|to simply ignore it altogether?][ Yes ][ No ]"
#elif COUNTRY==2
#define ERROR1 "[2][Unknown Opcode! Should one try|to simply ignore it altogether?][ Yes ][ No ]"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

#define WORD_LEN	(-1)
#define RGB_LEN		(6)
#define NA			(0)

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"Mac PICT Importer",
	0x0080,
	"Christian Eyrich",
	{ "PIC", "PCT", "PICT", "", "", "", "", "", "", "" },
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
};

static uint8_t *buffer;
static uint8_t *obuffer;
static uint8_t *ziel;
static uint8_t *pal;
static uint8_t got_pic = FALSE;

static unsigned long align = 0;

struct Rect
{
	unsigned short top;
	unsigned short left;
	unsigned short bottom;
	unsigned short right;
};

struct pixMap
{
	struct Rect Bounds;
	unsigned short version;
	unsigned short packType;
	unsigned long packSize;
	unsigned long hRes;
	unsigned long vRes;
	unsigned short pixelType;
	unsigned short pixelSize;
	unsigned short cmpCount;
	unsigned short cmpSize;
	unsigned long planeBytes;
	unsigned long pmTable;
	unsigned long pmReserved;
	unsigned short ctrlByte;
};

struct ct_entry
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
};

struct opdef
{
	short len;
	void (*impl)(void);
};

static struct Rect picFrame;
static GARGAMEL *giveitme;
static struct pixMap p;




/* Liest unkomprimierte (und pseudokomprimierte) PICTs */
static uint8_t *readunpacked(unsigned short width, unsigned short height, uint8_t BitsPerPixel, uint8_t packType)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t direkt;
	unsigned short x, y;
	unsigned long w, memwidth;

	if (BitsPerPixel == 1)
	{
		w = (unsigned long) ((width + 7) / 8);	/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
		memwidth = w * 8;
	} else if (BitsPerPixel == 24 && packType == 1)
	{
		w = width;
		memwidth = width;
	} else
	{
		w = ((unsigned long) width * BitsPerPixel) >> 3;
		memwidth = width;
	}


	if (BitsPerPixel == 32 && packType == 1)
		direkt = 0;
	else
		direkt = 1;

	if ((ziel = SMalloc((memwidth * (unsigned long) height * BitsPerPixel) >> 3)) == 0)
		return NULL;

	oziel = ziel;

	y = 0;
	do
	{
		if (direkt)
		{
			memcpy(ziel, buffer, w);
			buffer += w;
		} else
		{
			x = 0;
			do
			{
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
				*ziel++ = *buffer++;
				buffer++;
			} while (++x < w);
		}
		align += w;					/* die buffer += w, bzw. buffer++ */
	} while (++y < height);

	ziel = oziel;

	got_pic = TRUE;

	return ziel;
}



/* Dekodiert PICT mit byteweisem Packing */
static uint8_t *decode(unsigned short width, unsigned short height, uint8_t BitsPerPixel, uint8_t CtrlByteLen)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t v, v1, v2;
	unsigned short x, y;
	unsigned short dpaketlength;
	unsigned long w, memwidth;

	if (BitsPerPixel == 1)
	{
		w = (unsigned long) ((width + 7) / 8);	/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
		memwidth = w * 8;
	} else
	{
		w = width;
		memwidth = width;
	}

	if (BitsPerPixel == 1 || BitsPerPixel == 8)
		v = width & 1;
	else
		v = 0;

	if ((ziel = SMalloc((memwidth * (unsigned long) height * BitsPerPixel) >> 3)) == 0)
		return NULL;

	oziel = ziel;

	y = 0;
	do
	{
		buffer += CtrlByteLen;
		align += CtrlByteLen;		/* den buffer += CtrlByteLen */

		x = 0;
		do
		{
			dpaketlength = 0;

			v1 = *buffer++;

			if (v1 > 0x7f)			/* Encoded Run */
			{
				v1 = 0x101 - v1;

				x += v1;

				v2 = *buffer++;

				align += 2;			/* die beiden buffer++ */

				while (v1--)
				{
					*ziel++ = v2;
					dpaketlength++;
				}
			}						/* x != 0 */
			else					/* Literal Run */
			{
				v1++;

				x += v1;

				align += v1 + 1;	/* den buffer++ und die v1 folgenden */

				while (v1--)
				{
					*ziel++ = *buffer++;
					dpaketlength++;
				}
			}
		} while (x < w);
		ziel -= v;
	} while (++y < height);

	ziel = oziel;

	got_pic = TRUE;

	return ziel;
}



/* Dekodiert 4 Bit PICT mit und ohne Kompression und konvertiert */
/* es gleichzeitig ins Standardformat */
static uint8_t *decode4(unsigned short width, unsigned short height, uint8_t CtrlByteLen, uint8_t packType)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *pixbuf;
	uint8_t *opixbuf;
	uint8_t v1, v2;
	unsigned short x, y;
	unsigned long w, memwidth, planelength;

	w = (unsigned long) ((width + 1) / 2);
	memwidth = (unsigned long) ((width + 7) / 8) * 8;

	planelength = (unsigned long) ((width + 7) / 8) * (unsigned long) height;

	if ((ziel = SMalloc(memwidth * (unsigned long) height + 7)) == 0)
		return NULL;

	oziel = ziel;

	opixbuf = pixbuf = (uint8_t *) Malloc(width);

	y = 0;
	do
	{
		if (packType == 1)
		{
			x = 0;
			do
			{
				v2 = *buffer++;
				*pixbuf++ = v2 >> 4;
				*pixbuf++ = v2 & 0x0f;
			} while (++x < w);

			align += w;				/* den buffer += w */
		} else
		{
			buffer += CtrlByteLen;
			align += CtrlByteLen;	/* den buffer += CtrlByteLen */

			x = 0;
			do
			{
				v1 = *buffer++;

				if (v1 > 0x7f)		/* Encoded Run */
				{
					v1 = 0x101 - v1;

					v2 = *buffer++;

					x += v1;

					align += 2;		/* die beiden buffer++ */

					while (v1--)
					{
						*pixbuf++ = v2 >> 4;
						*pixbuf++ = v2 & 0x0f;
					}
				}					/* x != 0 */
				else				/* Literal Run */
				{
					v1++;

					x += v1;

					align += v1 + 1;	/* den buffer++ und die v1 folgenden */

					while (v1--)
					{
						v2 = *buffer++;
						*pixbuf++ = v2 >> 4;
						*pixbuf++ = v2 & 0x0f;
					}
				}
			} while (x < w);
		}

		pixbuf = opixbuf;
		ziel += setpix_std_line(pixbuf, ziel, 4, planelength, width);
	} while (++y < height);

	Mfree(pixbuf);

	ziel = oziel;

	got_pic = TRUE;

	return ziel;
}



/* Dekodiert 16 Bit PICT mit Kompressionsarten 0 und 3 (wordweise gepackt) */
static uint8_t *decode16(unsigned short width, unsigned short height, uint8_t CtrlByteLen)
{
	uint8_t *buffer16;
	uint16_t *ziel16;
	uint16_t *oziel16;
	unsigned short x, y, v1, v2;
	unsigned long w, more;

	buffer16 = buffer;

	w = width;

	more = align;

	if ((ziel16 = (uint16_t *) SMalloc((unsigned long) width * height * 2)) == NULL)
		return NULL;

	oziel16 = ziel16;

	y = 0;
	do
	{
		buffer16 += CtrlByteLen;
		align += CtrlByteLen;		/* den buffer += CtrlByteLen */

		x = 0;
		do
		{
			v1 = *buffer16++;

			if (v1 > 0x7f)			/* Encoded Run */
			{
				v1 = 0x101 - v1;

				v2 = (buffer16[0] << 8) | buffer16[1];
				buffer16 += 2;
				v2 = ((v2 & 0x7fe0) << 1) | (v2 & 0x001f);

				x += v1;

				align += 3;			/* die beiden buffer++ */

				while (v1--)
					*ziel16++ = v2;
			}						/* x != 0 */
			else					/* Literal Run */
			{
				v1++;

				x += v1;

				align += (v1 << 1) + 1;	/* den buffer++ und die v1 folgenden */

				while (v1--)
				{
					v2 = (buffer16[0] << 8) | buffer16[1];
					buffer16 += 2;
					*ziel16++ = ((v2 & 0x7fe0) << 1) | (v2 & 0x001f);
				}
			}
		} while (x < w);
	} while (++y < height);

	buffer += align - more;

	ziel16 = oziel16;

	got_pic = TRUE;

	return (uint8_t *) ziel16;
}



/* Dekodiert 24 Bit PICT mit Kompressionsarten 0 und 4 */
static uint8_t *decode24(unsigned short width, unsigned short height, uint8_t cmpCount, uint8_t CtrlByteLen)
{
	uint8_t *ziel;
	uint8_t *oziel;
	uint8_t *ziel2;
	uint8_t *oziel2;
	uint8_t v1, v2;
	unsigned short x, y;
	unsigned long w;

	w = (unsigned long) width *cmpCount;

	if ((ziel = SMalloc((unsigned long) width * height * 3)) == 0)
		return NULL;

	if ((ziel2 = (uint8_t *)Malloc((unsigned long) width * cmpCount)) == 0)	/* Zeilenbuffer */
	{
		SMfree(ziel);
		ziel = NULL;
		return NULL;
	}

	oziel = ziel;
	oziel2 = ziel2;

	y = 0;
	do
	{
		buffer += CtrlByteLen;
		align += CtrlByteLen;		/* den buffer += CtrlByteLen */

		x = 0;
		do
		{
			v1 = *buffer++;

			if (v1 > 0x7f)			/* Encoded Run */
			{
				v1 = 0x101 - v1;

				v2 = *buffer++;

				x += v1;

				align += 2;			/* die beiden buffer++ */

				while (v1--)
					*ziel2++ = v2;
			}						/* x != 0 */
			else					/* Literal Run */
			{
				v1++;

				x += v1;

				align += v1 + 1;	/* den buffer++ und die v1 folgenden */

				while (v1--)
					*ziel2++ = *buffer++;
			}
		} while (x < w);
		/* RGB-Folgen aus dem Zeilenbuffer wieder umsortieren */
		ziel2 = oziel2;
		if (cmpCount == 4)
			ziel2 += width;
		x = 0;
		do
		{
			*ziel++ = *ziel2;
			*ziel++ = *(ziel2 + width);
			*ziel++ = *(ziel2 + (width << 1));
			ziel2++;
		} while (++x < width);
		ziel2 = oziel2;
	} while (++y < height);

	ziel = oziel;

	Mfree(ziel2);

	got_pic = TRUE;

	return ziel;
}


/* Alle Daten in Version 2-Bildern sind wordaligned. */
/* Ungerade Daten werden mit einem Nullbyte aufgefÅllt. */

static uint8_t read_byte(void)
{
	align++;

	return *buffer++;
}

static uint16_t read_word(void)
{
	uint16_t v;

	align += 2;
	v = (buffer[0] << 8) | buffer[1];
	buffer += 2;
	return v;
}

static uint32_t read_long(void)
{
	uint32_t v;

	align += 4;
	v = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | ((uint32_t)buffer[3] << 0);
	buffer += 4;
	return v;
}


static uint16_t get_op(unsigned short version)
{
	if ((align & 1) && version == 2)
		read_byte();

	if (version == 1)
		return read_byte();
	else
		return read_word();
}


static void skip(unsigned long n)
{
	buffer += n;
	align += n;
}


static void unpackbits(struct Rect *bounds, unsigned short rowBytes, short pixelSize)
{
	unsigned short pixwidth, pixheight;

	if (pixelSize <= 8)
		rowBytes &= 0x7fff;

	pixwidth = bounds->right - bounds->left;
	pixheight = bounds->bottom - bounds->top;

	if (pixelSize == 16)
		pixwidth *= 2;
	else if (pixelSize == 32)
		pixwidth *= 3;

	if (rowBytes == 0)
		rowBytes = pixwidth;

	if (rowBytes < 8)
	{
		skip((unsigned long) rowBytes * pixheight);
	} else
	{
		if (rowBytes > 250 || pixelSize > 8)
		{
			while (pixheight--)
				skip(read_word());
		} else
		{
			while (pixheight--)
				skip(read_byte());
		}
	}
}


static void read_rect(struct Rect *r)
{
	r->top = read_word();
	r->left = read_word();
	r->bottom = read_word();
	r->right = read_word();
}


static void read_pixmap(struct pixMap *p, unsigned short *rowBytes)
{
	if (rowBytes != NULL)
		*rowBytes = read_word();

	read_rect(&p->Bounds);

	p->version = read_word();
	p->packType = read_word();
	p->packSize = read_long();
	p->hRes = read_long();
	p->vRes = read_long();

	p->pixelType = read_word();
	p->pixelSize = read_word();
	p->cmpCount = read_word();

	p->cmpSize = read_word();
	p->planeBytes = read_long();
	p->pmTable = read_long();
	p->pmReserved = read_long();
}


static void read_color_table(void)
{
	unsigned short i, ctSize;

	/* ctSeed = */ read_long();
	/* ctFlags = */ read_word();
	ctSize = read_word();

	pal = giveitme->smurf_pic->palette;

	for (i = 0; i <= ctSize; i++)
	{
		skip(2); /* value */
		*pal++ = read_word() >> 8;
		*pal++ = read_word() >> 8;
		*pal++ = read_word() >> 8;
	}
}


/* öberliest einfach das nÑchste Version 2 Pattern. */
static void read_pattern(void)
{
	unsigned short PatType;
	unsigned short rowBytes;

	PatType = read_word();

	switch (PatType)
	{
	case 2:
		skip(8);						/* old pattern data */
		skip(5);						/* RGB for pattern */
		break;
	case 1:
		skip(8);						/* old pattern data */
		read_pixmap(&p, &rowBytes);
		read_color_table();
		unpackbits(&p.Bounds, rowBytes, p.pixelSize);
		break;
	default:							/* unknown pattern type in read_pattern */
		break;
	}
}


/* Diese 3 Åberspringen einfach ihre Daten. */
static void BkPixPat(void)
{
	read_pattern();
}


static void PnPixPat(void)
{
	read_pattern();
}


static void FillPixPat(void)
{
	read_pattern();
}


static void Clip(void)
{
	skip(read_word() - 2);
}


/* Zeugs fÅr Textausgabe */
static void skip_text(void)
{
	skip(read_byte());
}


static void LongText(void)
{
	skip(4);
	skip_text();
}


static void DHText(void)
{
	skip(1);
	skip_text();
}


static void DVText(void)
{
	skip(1);
	skip_text();
}


static void DHDVText(void)
{
	skip(2);
	skip_text();
}


static void LongComment(void)
{
	skip(2);
	skip(read_word());
}


static void skip_poly_or_region(void)
{
	skip(read_word() - 2);
}




/* Dispatcher - entscheidet, welche Routine fÅr die Dekodierung */
/* des eigentlichen Bildes aufgerufen wird */
static void Dispatch(struct pixMap *p)
{
	uint8_t BitsPerPixel;
	char dummy[3];
	char impmessag[21];
	unsigned short width, height, cmpCount, packType, ctrlByteLen;

	/*
	   packType:
	   0 - default packing (pixelSize 16 defaults to packType 3 and
	   pixelSize 32 defaults to packType 4)
	   1 - no packing
	   2 - remove pad byte (32-bit pixels only)
	   3 - run-length encoding by pixel size chunks (16-bit pixels only)
	   4 - run-length encoding, all of one component at the time, one scan
	   line at a time (24-bit pixels only)
	 */
	packType = p->packType;
	cmpCount = p->cmpCount;				/* 3 = RGB, 4 = aRGB */
	BitsPerPixel = p->cmpSize * cmpCount;
	if (BitsPerPixel == 15)
		BitsPerPixel = 16;

	if (p->ctrlByte & 0x8000)
		ctrlByteLen = 1;
	else
		ctrlByteLen = 2;

	width = picFrame.right - picFrame.left;
	height = picFrame.bottom - picFrame.top;

	strcpy(giveitme->smurf_pic->format_name, "Macintosh PICT .PICT");
	giveitme->smurf_pic->pic_width = width;
	giveitme->smurf_pic->pic_height = height;
	giveitme->smurf_pic->depth = BitsPerPixel;

	strcpy(impmessag, "Macintosh PICT ");
	strcat(impmessag, itoa(BitsPerPixel, dummy, 10));
	strcat(impmessag, " Bit");
	giveitme->services->reset_busybox(128, impmessag);

	if (BitsPerPixel == 16 && (packType == 0 || packType == 3))
		ziel = decode16(width, height, ctrlByteLen);
	else if (BitsPerPixel == 24 && (packType == 0 || packType == 4))
		ziel = decode24(width, height, cmpCount, ctrlByteLen);
	else if (BitsPerPixel == 4)
		ziel = decode4(width, height, ctrlByteLen, packType);
	else if (packType == 1 || packType == 2)
		ziel = readunpacked(width, height, BitsPerPixel, packType);
	else
		ziel = decode(width, height, BitsPerPixel, ctrlByteLen);

	if (BitsPerPixel == 1 || BitsPerPixel == 4)
		giveitme->smurf_pic->format_type = FORM_STANDARD;
	else
		giveitme->smurf_pic->format_type = FORM_PIXELPAK;

	giveitme->smurf_pic->col_format = RGB;
}


static void do_bitmap(int is_region)
{
	struct Rect Bounds;
	struct Rect srcRect;
	struct Rect dstRect;

	read_rect(&Bounds);
	read_rect(&srcRect);
	read_rect(&dstRect);
	read_word();						/* mode */

	if (is_region)
		skip_poly_or_region();

	Dispatch(&p);
}


static void do_pixmap(int is_region)
{
	struct Rect srcRect;
	struct Rect dstRect;

	read_pixmap(&p, NULL);
	read_color_table();
	read_rect(&srcRect);
	read_rect(&dstRect);

	read_word();						/* mode */

	if (is_region)
		skip_poly_or_region();

	Dispatch(&p);
}


static void BitsRect(void)
{
	unsigned short rowBytes;

	rowBytes = read_word();

	p.ctrlByte = rowBytes;

	if (rowBytes & 0x8000)
		do_pixmap(FALSE);
	else
		do_bitmap(FALSE);
}

static void BitsRegion(void)
{
	unsigned short rowBytes;

	rowBytes = read_word();

	p.ctrlByte = rowBytes;

	if (rowBytes & 0x8000)
		do_pixmap(TRUE);
	else
		do_bitmap(TRUE);
}


static void DirectBitsRect(void)
{
	unsigned short rowBytes;

	/*
	 * The DirectBitsRect and DirectBitsRgn opcodes store the baseAddr field
	 * of the PixMap record in a version 2 picture.
	 * For compatibility with existing systems, the baseAddr field is set to $000000FF.
	 */
	skip(4);							/* baseAddr */
	rowBytes = read_word();

	p.ctrlByte = rowBytes;

	if (rowBytes & 0x8000)
		do_pixmap(FALSE);
	else
		do_bitmap(FALSE);
}


static void DirectBitsRegion(void)
{
	unsigned short rowBytes;

	/*
	 * The DirectBitsRect and DirectBitsRgn opcodes store the baseAddr field
	 * of the PixMap record in a version 2 picture.
	 * For compatibility with existing systems, the baseAddr field is set to $000000FF.
	 */
	skip(4);							/* baseAddr */
	rowBytes = read_word();

	p.ctrlByte = rowBytes;

	if (rowBytes & 0x8000)
		do_pixmap(TRUE);
	else
		do_bitmap(TRUE);
}


static void nop(void)
{
}


/* for reserved opcodes of known length */
#define res(length) { length, NULL }

/* for reserved opcodes of length determined by a function */
#define resf(skipfunction) { NA, skipfunction }

struct opdef optable[] = {
	/* 0x00 */ { 0, NULL },					/* NOP */
	/* 0x01 */ { NA, Clip },				/* Clip */
	/* 0x02 */ { 8, NULL },					/* BkPat */
	/* 0x03 */ { 2, NULL },					/* TxFont */
	/* 0x04 */ { 1, NULL },					/* TxFace */
	/* 0x05 */ { 2, NULL },					/* TxMode */
	/* 0x06 */ { 4, NULL },					/* SpExtra */
	/* 0x07 */ { 4, NULL },					/* PnSize */
	/* 0x08 */ { 2, NULL },					/* PnMode */
	/* 0x09 */ { 8, NULL },					/* PnPat */
	/* 0x0a */ { 8, NULL },					/* FillPat */
	/* 0x0b */ { 4, NULL },					/* OvSize */
	/* 0x0c */ { 4, NULL },					/* Origin */
	/* 0x0d */ { 2, NULL },					/* TxSize */
	/* 0x0e */ { 4, NULL },					/* FgColor */
	/* 0x0f */ { 4, NULL },					/* BkColor */
	/* 0x10 */ { 8, NULL },					/* TxRatio */
	/* 0x11 */ { 1, NULL },					/* Version */
	/* 0x12 */ { NA, BkPixPat },			/* BkPixPat */
	/* 0x13 */ { NA, PnPixPat },			/* PnPixPat */
	/* 0x14 */ { NA, FillPixPat },			/* FillPixPat */
	/* 0x15 */ { 2, NULL },					/* PnLocHFrac */
	/* 0x16 */ { 2, NULL },					/* ChExtra */
	/* 0x17 */ res(0),
	/* 0x18 */ res(0),
	/* 0x19 */ res(0),
	/* 0x1a */ { RGB_LEN, NULL },			/* RGBFgCol */
	/* 0x1b */ { RGB_LEN, NULL },			/* RGBBkCol */
	/* 0x1c */ { 0, NULL },					/* HiliteMode */
	/* 0x1d */ { RGB_LEN, NULL },			/* HiliteColor */
	/* 0x1e */ { 0, NULL },					/* DefHilite */
	/* 0x1f */ { RGB_LEN, NULL },			/* OpColor */
	/* 0x20 */ { 8, NULL },					/* Line */
	/* 0x21 */ { 4, NULL },					/* LineFrom */
	/* 0x22 */ { 6, NULL },					/* ShortLine */
	/* 0x23 */ { 2, NULL },					/* ShortLineFrom */
	/* 0x24 */ res(WORD_LEN),
	/* 0x25 */ res(WORD_LEN),
	/* 0x26 */ res(WORD_LEN),
	/* 0x27 */ res(WORD_LEN),
	/* 0x28 */ { NA, LongText },			/* LongText */
	/* 0x29 */ { NA, DHText },				/* DHText */
	/* 0x2a */ { NA, DVText },				/* DVText */
	/* 0x2b */ { NA, DHDVText },			/* DHDVText */
	/* 0x2c */ res(WORD_LEN),				/* fontName */
	/* 0x2d */ res(WORD_LEN),				/* lineJustify */
	/* 0x2e */ res(WORD_LEN),				/* glyphState */
	/* 0x2f */ res(WORD_LEN),
	/* 0x30 */ { 8, NULL },					/* frameRect */
	/* 0x31 */ { 8, NULL },					/* paintRect */
	/* 0x32 */ { 8, NULL },					/* eraseRect */
	/* 0x33 */ { 8, NULL },					/* invertRect */
	/* 0x34 */ { 8, NULL },					/* fillRect */
	/* 0x35 */ res(8),
	/* 0x36 */ res(8),
	/* 0x37 */ res(8),
	/* 0x38 */ { 0, NULL },					/* frameSameRect */
	/* 0x39 */ { 0, NULL },					/* paintSameRect */
	/* 0x3a */ { 0, NULL },					/* eraseSameRect */
	/* 0x3b */ { 0, NULL },					/* invertSameRect */
	/* 0x3c */ { 0, NULL },					/* fillSameRect */
	/* 0x3d */ res(0),
	/* 0x3e */ res(0),
	/* 0x3f */ res(0),
	/* 0x40 */ { 8, NULL },					/* frameRRect */
	/* 0x41 */ { 8, NULL },					/* paintRRect */
	/* 0x42 */ { 8, NULL },					/* eraseRRect */
	/* 0x43 */ { 8, NULL },					/* invertRRect */
	/* 0x44 */ { 8, NULL },					/* fillRRrect */
	/* 0x45 */ res(8),
	/* 0x46 */ res(8),
	/* 0x47 */ res(8),
	/* 0x48 */ { 0, NULL },					/* frameSameRRect */
	/* 0x49 */ { 0, NULL },					/* paintSameRRect */
	/* 0x4a */ { 0, NULL },					/* eraseSameRRect */
	/* 0x4b */ { 0, NULL },					/* invertSameRRect */
	/* 0x4c */ { 0, NULL },					/* fillSameRRect */
	/* 0x4d */ res(0),
	/* 0x4e */ res(0),
	/* 0x4f */ res(0),
	/* 0x50 */ { 8, NULL },					/* frameOval */
	/* 0x51 */ { 8, NULL },					/* paintOval */
	/* 0x52 */ { 8, NULL },					/* eraseOval */
	/* 0x53 */ { 8, NULL },					/* invertOval */
	/* 0x54 */ { 8, NULL },					/* fillOval */
	/* 0x55 */ res(8),
	/* 0x56 */ res(8),
	/* 0x57 */ res(8),
	/* 0x58 */ { 0, NULL },					/* frameSameOval */
	/* 0x59 */ { 0, NULL },					/* paintSameOval */
	/* 0x5a */ { 0, NULL },					/* eraseSameOval */
	/* 0x5b */ { 0, NULL },					/* invertSameOval */
	/* 0x5c */ { 0, NULL },					/* fillSameOval */
	/* 0x5d */ res(0),
	/* 0x5e */ res(0),
	/* 0x5f */ res(0),
	/* 0x60 */ { 12, NULL },				/* frameArc */
	/* 0x61 */ { 12, NULL },				/* paintArc */
	/* 0x62 */ { 12, NULL },				/* eraseArc */
	/* 0x63 */ { 12, NULL },				/* invertArc */
	/* 0x64 */ { 12, NULL },				/* fillArc */
	/* 0x65 */ res(12),
	/* 0x66 */ res(12),
	/* 0x67 */ res(12),
	/* 0x68 */ { 4, NULL },					/* frameSameArc */
	/* 0x69 */ { 4, NULL },					/* paintSameArc */
	/* 0x6a */ { 4, NULL },					/* eraseSameArc */
	/* 0x6b */ { 4, NULL },					/* invertSameArc */
	/* 0x6c */ { 4, NULL },					/* fillSameArc */
	/* 0x6d */ res(4),
	/* 0x6e */ res(4),
	/* 0x6f */ res(4),
	/* 0x70 */ { NA, skip_poly_or_region },	/* framePoly */
	/* 0x71 */ { NA, skip_poly_or_region },	/* paintPoly */
	/* 0x72 */ { NA, skip_poly_or_region },	/* erasePoly */
	/* 0x73 */ { NA, skip_poly_or_region },	/* invertPoly */
	/* 0x74 */ { NA, skip_poly_or_region },	/* fillPoly */
	/* 0x75 */ resf(skip_poly_or_region),
	/* 0x76 */ resf(skip_poly_or_region),
	/* 0x77 */ resf(skip_poly_or_region),
	/* 0x78 */ { 0, NULL },					/* frameSamePoly */
	/* 0x79 */ { 0, NULL },					/* paintSamePoly */
	/* 0x7a */ { 0, NULL },					/* eraseSamePoly */
	/* 0x7b */ { 0, NULL },					/* invertSamePoly */
	/* 0x7c */ { 0, NULL },					/* fillSamePoly */
	/* 0x7d */ res(0),
	/* 0x7e */ res(0),
	/* 0x7f */ res(0),
	/* 0x80 */ { NA, skip_poly_or_region },	/* frameRgn */
	/* 0x81 */ { NA, skip_poly_or_region },	/* paintRgn */
	/* 0x82 */ { NA, skip_poly_or_region },	/* eraseRgn */
	/* 0x83 */ { NA, skip_poly_or_region },	/* invertRgn */
	/* 0x84 */ { NA, skip_poly_or_region },	/* fillRgn */
	/* 0x85 */ resf(skip_poly_or_region),
	/* 0x86 */ resf(skip_poly_or_region),
	/* 0x87 */ resf(skip_poly_or_region),
	/* 0x88 */ { 0, NULL },					/* frameSameRgn */
	/* 0x89 */ { 0, NULL },					/* paintSameRgn */
	/* 0x8a */ { 0, NULL },					/* eraseSameRgn */
	/* 0x8b */ { 0, NULL },					/* invertSameRgn */
	/* 0x8c */ { 0, NULL },					/* fillSameRgn */
	/* 0x8d */ res(0),
	/* 0x8e */ res(0),
	/* 0x8f */ res(0),
	/* 0x90 */ { NA, BitsRect },			/* BitsRect */
	/* 0x91 */ { NA, BitsRegion },			/* BitsRgn */
	/* 0x92 */ res(WORD_LEN),
	/* 0x93 */ res(WORD_LEN),
	/* 0x94 */ res(WORD_LEN),
	/* 0x95 */ res(WORD_LEN),
	/* 0x96 */ res(WORD_LEN),
	/* 0x97 */ res(WORD_LEN),
	/* 0x98 */ { NA, BitsRect },			/* PackBitsRect */
	/* 0x99 */ { NA, BitsRegion },			/* PackBitsRgn */
	/* 0x9a */ { NA, DirectBitsRect },		/* DirectBitsRect */
	/* 0x9b */ { NA, DirectBitsRegion },	/* DirectBitsRgn */
	/* 0x9c */ res(WORD_LEN),
	/* 0x9d */ res(WORD_LEN),
	/* 0x9e */ res(WORD_LEN),
	/* 0x9f */ res(WORD_LEN),
	/* 0xa0 */ { 2, NULL },					/* ShortComment */
	/* 0xa1 */ { NA, LongComment }			/* LongComment */
};


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		Macintosh PICT Dekomprimierer (PICT)		*/
/*		1, 4, 8, 16, 24 Bit, RLE					*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	short plus, res;
	unsigned short opcode, version, len, PictSize;

	giveitme = smurf_struct;

	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	buffer = smurf_struct->smurf_pic->pic_data;
	obuffer = buffer;

	/* nichtsnutzigen Header Åberspringen */
	if (*buffer == 0x0 && *(buffer + 1) == 0x0)
	{
		buffer += 0x200;
		plus = 0x200;
	} else
	{
		buffer += 0x280;
		plus = 0x280;
	}

	PictSize = read_word();

	if (smurf_struct->smurf_pic->file_len < 520 ||
/*	   Diese PrÅfung ist leider nicht hundertprozentig zuverlÑssig, da es Apple geschafft hat,
	   fÅr die Filegrîûe nur ein int zu reservieren, die gespeicherte Grîûe nach 2 Byte
	   abgeschnitten wird und somit halt Åberhaupt nicht stimmt. Bravo :-( */
		(PictSize + plus != (unsigned short ) smurf_struct->smurf_pic->file_len &&
		 PictSize != (unsigned short ) smurf_struct->smurf_pic->file_len))
		return M_INVALID;

	read_rect(&picFrame);

	if (*buffer == 0x11 && *(buffer + 0x01) == 0x01)
	{
		version = 1;
		buffer += 0x02;
	} else if (*(uint16_t *) buffer == 0x0011 && *(buffer + 0x02) == 0x02)
	{
		version = 2;
		if (*(buffer + 0x03) != 0xff)	/* Nur Unterversion 0xff ist bekannt */
			return M_UNKNOWN_TYPE;

		buffer += 0x04;
	} else
	{
		return M_INVALID;
	}

	while ((opcode = get_op(version)) != 0xff)
	{
		if (opcode < 0xa2)
		{
			if (optable[opcode].impl != NULL)
			{
				(*optable[opcode].impl) ();
			} else if (optable[opcode].len >= 0)
			{
				skip(optable[opcode].len);
			} else
			{
				switch (optable[opcode].len)
				{
				case WORD_LEN:
					len = read_word();
					skip(len);
					break;
				default:
					break;
				}
			}
		} else if (opcode == 0xc00)
		{
			/* HeaderOp */
			skip(24);
		} else if (opcode >= 0xa2 && opcode <= 0xaf)
		{
			skip(read_word());
		} else if (opcode >= 0xb0 && opcode <= 0xcf)
		{
			/* just a reserved opcode, no data */
			nop();
		} else if (opcode >= 0xd0 && opcode <= 0xfe)
		{
			skip(read_long());
		} else if (opcode >= 0x100 && opcode <= 0x7fff)
		{
			skip((opcode >> 8) << 1);
		} else if (opcode >= 0x8000 && opcode <= 0x80ff)
		{
			/* just a reserved opcode */
			nop();
		} else if (opcode >= 0x8100 /* && opcode <= 0xffff */)
		{
			skip((unsigned short) read_long());
		} else
		{
			/* not reached */
			res = form_alert(2, ERROR1);
			if (res == 2)
				return M_PICERR;
		}
	}

	if (opcode == 0x00ff && got_pic)
	{
		if (ziel == NULL)
			return M_MEMORY;
		smurf_struct->smurf_pic->pic_data = ziel;
		SMfree(obuffer);
	} else
	{
		return M_UNKNOWN_TYPE;
	}

	return M_PICDONE;
}
