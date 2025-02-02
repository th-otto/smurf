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

/****************************************************************/
/*																*/
/*			Tiff-Module zum entpacken, etc.						*/
/*																*/
/****************************************************************/

#include "country.h"

#if COUNTRY==1
#define BUSYBOX1 "wandle RGB-Planes"
#define BUSYBOX2 "wandle 32 Bit CMYK"
#define BUSYBOX3 "4Bit->Standard"
#define BUSYBOX4 "2Bit->Standard"
#define ERROR1   "[1][ *LZWdecrunch* -> Can't | allocate RAM for LZW-Tables][Stop]"
#elif COUNTRY==0
#define BUSYBOX1 "wandle RGB-Planes"
#define BUSYBOX2 "wandle 32 Bit CMYK"
#define BUSYBOX3 "4Bit->Standard"
#define BUSYBOX4 "2Bit->Standard"
#define ERROR1   "[1][ *LZWdecrunch* -> Can't | allocate RAM for LZW-Tables][Stop]"
#elif COUNTRY==2
#define BUSYBOX1 "wandle RGB-Planes"
#define BUSYBOX2 "wandle 32 Bit CMYK"
#define BUSYBOX3 "4Bit->Standard"
#define BUSYBOX4 "2Bit->Standard"
#define ERROR1   "[1][ *LZWdecrunch* -> Can't | allocate RAM for LZW-Tables][Stop]"
#else
#error "Keine Sprache!"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "tiffimp.h"




/* ----------------------------------------------- */
/*		LZW - Dekodierarrays/Variables			   */
/* ----------------------------------------------- */
#define head_size sizeof(short)*4096+16
#define tail_size sizeof(uint8_t)*4096+16
#define out_size  sizeof(uint8_t)*2048+16
#define MAXCODEWIDTH		12			/* maximum code width, in bits */
#define MAXTABENTRIES	(1<<MAXCODEWIDTH)	/* max # of table entries */
#define CHARBITS	8
#define CLEARCODE		256
#define EOICODE			257
#define COM(X) (~(X))					/* XyWrite users:  hidden tilda */

/* ----------------------------------------------- */
/* Huffman's Termination- und Makupcodes f�r WHITE */
/* ----------------------------------------------- */
static short mask[8] = {
	0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff
};

static short w_codes[] = {						/* Termination-Codes */

/*    0 */ 0x35, 0x07, 0x07, 0x08,
/*    4 */ 0x0b, 0x0c, 0x0e, 0x0f,
/*    8 */ 0x13, 0x14, 0x07, 0x08,
/*   12 */ 0x08, 0x03, 0x34, 0x35,
/*   16 */ 0x2a, 0x2b, 0x27, 0x0c,
/*   20 */ 0x08, 0x17, 0x03, 0x04,
/*   24 */ 0x28, 0x2b, 0x13, 0x24,
/*   28 */ 0x18, 0x02, 0x03, 0x1a,
/*   32 */ 0x1b, 0x12, 0x13, 0x14,
/*   36 */ 0x15, 0x16, 0x17, 0x28,
/*   40 */ 0x29, 0x2a, 0x2b, 0x2c,
/*   44 */ 0x2d, 0x04, 0x05, 0x0a,
/*   48 */ 0x0b, 0x52, 0x53, 0x54,
/*   52 */ 0x55, 0x24, 0x25, 0x58,
/*   56 */ 0x59, 0x5a, 0x5b, 0x4a,
/*   60 */ 0x4b, 0x32, 0x33, 0x34,
										/*   64 */ 0x1b,
										/* Makeup-Codes */
/*  128 */ 0x12,
/*  192 */ 0x17,
/*  256 */ 0x37,
/*  320 */ 0x36,
/*  384 */ 0x37,
/*  448 */ 0x64,
/*  512 */ 0x65,
/*  576 */ 0x68,
/*  640 */ 0x67,
/*  704 */ 0xcc,
/*  768 */ 0xcd,
/*  832 */ 0xd2,
/*  896 */ 0xd3,
/*  960 */ 0xd4,
/* 1024 */ 0xd5,
/* 1088 */ 0xd6,
/* 1152 */ 0xd7,
/* 1216 */ 0xd8,
/* 1280 */ 0xd9,
/* 1344 */ 0xda,
/* 1408 */ 0xdb,
/* 1472 */ 0x98,
/* 1536 */ 0x99,
/* 1600 */ 0x9a,
/* 1664 */ 0x18,
/* 1728 */ 0x9b,
/* 1792 */ 0x08,
/* 1856 */ 0x0c,
/* 1920 */ 0x0d,
/* 1984 */ 0x12,
/* 2048 */ 0x13,
/* 2112 */ 0x14,
/* 2176 */ 0x15,
/* 2240 */ 0x16,
/* 2304 */ 0x17,
/* 2368 */ 0x1c,
/* 2432 */ 0x1d,
/* 2496 */ 0x1e,
/* 2560 */ 0x1f
};

/* ----------------------------------------------- */
/* Huffman's Termination- und Makupcodes f�r BLACK */
/* ----------------------------------------------- */
static short b_codes[] = {
/*    0 */ 0x37, 0x02, 0x03, 0x02,
/*    4 */ 0x03, 0x03, 0x02, 0x03,
/*    8 */ 0x05, 0x04, 0x04, 0x05,
/*   12 */ 0x07, 0x04, 0x07, 0x18,
/*   16 */ 0x17, 0x18, 0x08, 0x67,
/*   20 */ 0x68, 0x6c, 0x37, 0x28,
/*   24 */ 0x17, 0x18, 0xca, 0xcb,
/*   28 */ 0xcc, 0xcd, 0x68, 0x69,
/*   32 */ 0x6a, 0x6b, 0xd2, 0xd3,
/*   36 */ 0xd4, 0xd5, 0xd6, 0xd7,
/*   40 */ 0x6c, 0x6d, 0xda, 0xdb,
/*   44 */ 0x54, 0x55, 0x56, 0x57,
/*   48 */ 0x64, 0x65, 0x52, 0x53,
/*   52 */ 0x24, 0x37, 0x38, 0x27,
/*   56 */ 0x28, 0x58, 0x59, 0x2b,
/*   60 */ 0x2c, 0x5a, 0x66, 0x67,
/*   64 */ 0x0f,
/*  128 */ 0xc8,
/*  192 */ 0xc9,
/*  256 */ 0x5b,
/*  320 */ 0x33,
/*  384 */ 0x34,
/*  448 */ 0x35,
/*  512 */ 0x6c,
/*  576 */ 0x6d,
/*  640 */ 0x4a,
/*  704 */ 0x4b,
/*  768 */ 0x4c,
/*  832 */ 0x4d,
/*  896 */ 0x72,
/*  960 */ 0x73,
/* 1024 */ 0x74,
/* 1088 */ 0x75,
/* 1152 */ 0x76,
/* 1216 */ 0x77,
/* 1280 */ 0x52,
/* 1344 */ 0x53,
/* 1408 */ 0x54,
/* 1472 */ 0x55,
/* 1536 */ 0x5a,
/* 1600 */ 0x5b,
/* 1664 */ 0x64,
/* 1728 */ 0x65,
/* 1792 */ 0x08,
/* 1856 */ 0x0c,
/* 1920 */ 0x0d,
/* 1984 */ 0x12,
/* 2048 */ 0x13,
/* 2112 */ 0x14,
/* 2176 */ 0x15,
/* 2240 */ 0x16,
/* 2304 */ 0x17,
/* 2368 */ 0x1c,
/* 2432 */ 0x1d,
/* 2496 */ 0x1e,
/* 2560 */ 0x1f
};

/* ----------------------------------------------- */
/* 			Huffman's Codelength f�r WHITE 		   */
/* ----------------------------------------------- */

static short w_length[] = {
/*    0 */ 0x08, 0x06, 0x04, 0x04,
/*    4 */ 0x04, 0x04, 0x04, 0x04,
/*    8 */ 0x05, 0x05, 0x05, 0x05,
/*   12 */ 0x06, 0x06, 0x06, 0x06,
/*   16 */ 0x06, 0x06, 0x07, 0x07,
/*   20 */ 0x07, 0x07, 0x07, 0x07,
/*   24 */ 0x07, 0x07, 0x07, 0x07,
/*   28 */ 0x07, 0x08, 0x08, 0x08,
/*   32 */ 0x08, 0x08, 0x08, 0x08,
/*   36 */ 0x08, 0x08, 0x08, 0x08,
/*   40 */ 0x08, 0x08, 0x08, 0x08,
/*   44 */ 0x08, 0x08, 0x08, 0x08,
/*   48 */ 0x08, 0x08, 0x08, 0x08,
/*   52 */ 0x08, 0x08, 0x08, 0x08,
/*   56 */ 0x08, 0x08, 0x08, 0x08,
/*   60 */ 0x08, 0x08, 0x08, 0x08,
/*   64 */ 0x05,
/*  128 */ 0x05,
/*  192 */ 0x06,
/*  256 */ 0x07,
/*  320 */ 0x08,
/*  384 */ 0x08,
/*  448 */ 0x08,
/*  512 */ 0x08,
/*  576 */ 0x08,
/*  640 */ 0x08,
/*  704 */ 0x09,
/*  768 */ 0x09,
/*  832 */ 0x09,
/*  896 */ 0x09,
/*  960 */ 0x09,
/* 1024 */ 0x09,
/* 1088 */ 0x09,
/* 1152 */ 0x09,
/* 1216 */ 0x09,
/* 1280 */ 0x09,
/* 1344 */ 0x09,
/* 1408 */ 0x09,
/* 1472 */ 0x09,
/* 1536 */ 0x09,
/* 1600 */ 0x09,
/* 1664 */ 0x06,
/* 1728 */ 0x09,
/* 1792 */ 0x0B,
/* 1856 */ 0x0B,
/* 1920 */ 0x0B,
/* 1984 */ 0x0C,
/* 2048 */ 0x0C,
/* 2112 */ 0x0C,
/* 2176 */ 0x0C,
/* 2240 */ 0x0C,
/* 2304 */ 0x0C,
/* 2368 */ 0x0C,
/* 2432 */ 0x0C,
/* 2496 */ 0x0C,
/* 2560 */ 0x0C,
};

/* ----------------------------------------------- */
/* 			Huffman's Codelength f�r BLACK 		   */
/* ----------------------------------------------- */
static short b_length[] = {
/*    0 */ 0x0A, 0x03, 0x02, 0x02,
/*    4 */ 0x03, 0x04, 0x04, 0x05,
/*    8 */ 0x06, 0x06, 0x07, 0x07,
/*   12 */ 0x07, 0x08, 0x08, 0x09,
/*   16 */ 0x0A, 0x0A, 0x0A, 0x0B,
/*   20 */ 0x0B, 0x0B, 0x0B, 0x0B,
/*   24 */ 0x0B, 0x0B, 0x0C, 0x0C,
/*   28 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   32 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   36 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   40 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   44 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   48 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   52 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   56 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   60 */ 0x0C, 0x0C, 0x0C, 0x0C,
/*   64 */ 0x0A,
/*  128 */ 0x0C,
/*  192 */ 0x0C,
/*  256 */ 0x0C,
/*  320 */ 0x0C,
/*  384 */ 0x0C,
/*  448 */ 0x0C,
/*  512 */ 0x0D,
/*  576 */ 0x0D,
/*  640 */ 0x0D,
/*  704 */ 0x0D,
/*  768 */ 0x0D,
/*  832 */ 0x0D,
/*  896 */ 0x0D,
/*  960 */ 0x0D,
/* 1024 */ 0x0D,
/* 1088 */ 0x0D,
/* 1152 */ 0x0D,
/* 1216 */ 0x0D,
/* 1280 */ 0x0D,
/* 1344 */ 0x0D,
/* 1408 */ 0x0D,
/* 1472 */ 0x0D,
/* 1536 */ 0x0D,
/* 1600 */ 0x0D,
/* 1664 */ 0x0D,
/* 1728 */ 0x0D,
/* 1792 */ 0x0B,
/* 1856 */ 0x0B,
/* 1920 */ 0x0B,
/* 1984 */ 0x0C,
/* 2048 */ 0x0C,
/* 2112 */ 0x0C,
/* 2176 */ 0x0C,
/* 2240 */ 0x0C,
/* 2304 */ 0x0C,
/* 2368 */ 0x0C,
/* 2432 */ 0x0C,
/* 2496 */ 0x0C,
/* 2560 */ 0x0C,
};

#ifdef __PUREC__
/* Dies bastelt direct ein rol.w #8,d0 inline ein. */
static unsigned short swap_word(unsigned short w) 0xE058;
#else
static unsigned short swap_word(unsigned short w)
{
	return (w >> 8) | (w << 8);
}
#endif

/******************************/
/*	---- 24 Bit Planes ----   */
/******************************/
void tiff24Planes_24Normal(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, long DataCount, long *DataArray)
{
	uint8_t *source1;
	uint8_t *source2;
	uint8_t *source3;
	unsigned short bh, bl;
	long planeoffset, x, y;

	smurf_struct->services->reset_busybox(0, BUSYBOX1);

	planeoffset = Width * Height;
	if (DataCount == 3)
	{
		source1 = source + *DataArray++;
		source2 = source + *DataArray++;
		source3 = source + *DataArray++;
	} else
	{
		source1 = source + *DataArray;
		source2 = source1 + planeoffset;
		source3 = source2 + planeoffset;
	}

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */

	for (y = 0; y < Height; y++)
	{
		if (!(y % bh))
		{
			smurf_struct->services->busybox(bl);
			bl += 12;
		}

		for (x = 0; x < Width; x++)
		{
			*dest++ = *source1++;
			*dest++ = *source2++;
			*dest++ = *source3++;
		}
	}
}

/****************************/
/*	---- 32 Bit CMYK ----   */
/****************************/
void tiff32CMYK_24Normal(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, long DataOffset)
{
	unsigned short bh, bl;
	long x, y;

	smurf_struct->services->reset_busybox(0, BUSYBOX2);

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */

	source += DataOffset;

	for (y = 0; y < Height; y++)
	{
		if (!(y % bh))
		{
#if !PRG
			smurf_struct->services->busybox(bl);
#endif
			bl += 12;
		}

		for (x = 0; x < Width; x++)
		{
			*dest++ = ~*source++;
			*dest++ = ~*source++;
			*dest++ = ~*source++;
			source++;
		}
	}
}

/*****************************************************/
/*		Lese 4 Bit RGB Palettenbild					 */
/*****************************************************/
void tiff4_to_stf(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height)
{
	uint8_t *pixbuf, v;
	unsigned short bh, bl;
	long y, x;
	long planelength;

	smurf_struct->services->reset_busybox(0, BUSYBOX3);
	planelength = (Width + 7) / 8 * Height;

	pixbuf = (uint8_t *) SMalloc(Width + 7);

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			smurf_struct->services->busybox(bl);
			bl += 12;
		}

		x = 0;
		do
		{
			v = *source++;
			pixbuf[x++] = v >> 4;
			pixbuf[x++] = v & 0x0f;
		} while (x < Width);
		dest += setpix_std_line(pixbuf, dest, 4, planelength, (short)Width);
	} while (++y < Height);

	SMfree(pixbuf);
}


/*****************************************************/
/*		Lese 2 Bit RGB Palettenbild					 */
/*****************************************************/
void tiff2_to_stf(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height)
{
	uint8_t *pixbuf;
	uint8_t v;
	unsigned short bh, bl;
	long y, x;
	long planelength;

	smurf_struct->services->reset_busybox(0, BUSYBOX4);
	planelength = (Width + 7) / 8 * Height;

	pixbuf = (uint8_t *) SMalloc(Width + 7);

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */

	y = 0;
	do
	{
		if (!(y % bh))
		{
			smurf_struct->services->busybox(bl);
			bl += 12;
		}

		x = 0;
		do
		{
			v = *source++;
			pixbuf[x++] = (v & 0xc0) >> 6;
			pixbuf[x++] = (v & 0x30) >> 4;
			pixbuf[x++] = (v & 0x0c) >> 2;
			pixbuf[x++] = (v & 0x03);
		} while (x < Width);
		dest += setpix_std_line(pixbuf, dest, 2, planelength, (short) Width);
	} while (++y < Height);

	SMfree(pixbuf);
}


/****************************************************************/
/*		CCITT 3, 1-dimensionalen Huffman dekomprimieren			*/
/****************************************************************/
short tiffCCITT3_depack(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, short Depth)
{
	short codelength;
	short bitpos;
	short maxlength;
	short code;
	short buf;
	short c;
	short B_W;
	short maxc;
	unsigned short bh, bl;
	long bitcount;
	long bytepos;
	long codeok;
	long codeleft;
	long linec;
	long offset;
	long ymax;
	long l;

	(void) Depth;
	bitcount = 0;
	codelength = 0;
	B_W = 0;
	linec = 0;
	ymax = 0;

	smurf_struct->services->reset_busybox(0, "CCITT 3");

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */

	while (ymax < Height)
	{
		if (!(ymax % bh))
		{
			smurf_struct->services->busybox(bl);
			bl += 12;
		}

		bytepos = bitcount >> 3L;		/* / 8 */
		bitpos = (short) (bitcount & 0x07);
		maxlength = 7 - bitpos;
		if (!B_W)
			maxc = 4;
		else
			maxc = 2;

		do
		{
			if (codelength <= maxlength)	/* Code pa�t ins Byte rein */
			{
				code = source[bytepos];
				code >>= (maxlength - codelength);
				code &= mask[codelength];
			} else						/* Code pa�t nicht rein -> WORD */
			{
				code = source[bytepos];
				codeok = 7 - bitpos;
				code &= mask[codeok];
				codeleft = codelength - codeok - 1;
				code <<= (codeleft + 1);
				if (codeleft <= 7)
				{
					buf = source[bytepos + 1];
					buf >>= (7 - codeleft);
					buf &= mask[codeleft];
					code += buf;
				} else
				{
					buf = source[bytepos + 1];
					codeleft -= 7 + 1;
					code += buf << (codeleft + 1);
					buf = source[bytepos + 2];
					buf >>= (7 - codeleft);

					buf &= mask[codeleft];
					code += buf;
				}
			}
		} while (++codelength < maxc);

		if (codelength == 13 && code == 1)
			break;

		if (!B_W)
		{
			for (c = 0; c <= 103; c++)
				if (w_length[c] == codelength)
					if (w_codes[c] == code)
					{
						bitcount += codelength;
						codelength = 0;
						if (c <= 63)
						{
							B_W = 1;
							linec += (long) c;
							for (l = 0; l < c; l++)
								*dest++ = 1;
							goto checkIt;
						} else
						{
							offset = (long) ((long) c - 63L) << 6L;	/* *64 */
							for (l = 0; l < offset; l++)
								*dest++ = 1;
							ymax += (linec + offset) / Width;
							linec = (linec + offset) % Width;
							break;
						}
					}
			continue;
		} else
		{
			for (c = 0; c <= 103; c++)
				if (b_length[c] == codelength)
					if (b_codes[c] == code)
					{
						bitcount += codelength;
						codelength = 0;
						if (c <= 63)
						{
							B_W = 0;
							linec += (long) c;
							for (l = 0; l < c; l++)
								*dest++ = 0;
							goto checkIt;
						} else
						{
							offset = (long) ((long) c - 63L) << 6L;
							for (l = 0; l < offset; l++)
								*dest++ = 0;
							ymax += (linec + offset) / Width;
							linec = (linec + offset) % Width;
							break;
						}
					}
			continue;
		}

	  checkIt:

		if (linec >= Width)
		{
			B_W = 0;
			linec = 0;
			bitcount = ((bitcount + 7L) >> 3L) << 3L;	/* Byte Alignment */
			ymax++;
		}

		if (codelength > 13)
			return -1;
	}

	return 0;
}


/****************************************************************/
/*			Lempel-Ziv and Welch dekodieren						*/
/****************************************************************/


static void decode_difference(uint8_t *data, long width, long height, uint8_t BitsPerPixel)
{
	unsigned short x, y;
	uint8_t v;

	if (BitsPerPixel == 8)
	{
		y = 0;
		do
		{
			data++;

			x = 1;
			do
			{
				v = (*data + *(data - 1));
				*data++ = v;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 24)
	{
		y = 0;
		do
		{
			data += 3;

			x = 1;
			do
			{
				uint8_t v;
				v = (*data + *(data - 3));
				*data++ = v;
				v = (*data + *(data - 3));
				*data++ = v;
				v = (*data + *(data - 3));
				*data++ = v;
			} while (++x < width);
		} while (++y < height);
	} else if (BitsPerPixel == 32)
	{
		y = 0;
		do
		{
			data += 4;

			x = 1;
			do
			{
				v = (*data + *(data - 4));
				*data++ = v;
				v = (*data + *(data - 4));
				*data++ = v;
				v = (*data + *(data - 4));
				*data++ = v;
				v = (*data + *(data - 4));
				*data++ = v;
			} while (++x < width);
		} while (++y < height);
	}
}


short tiffLZW_depack(GARGAMEL *smurf_struct, uint8_t *source, uint8_t *dest, long Width, long Height, short BitsPerPixel,
	long DataCount, long *DataArray, long RowsPerStrip, short Difference)
{
	uint8_t *odest;
	uint8_t *out;
	uint8_t *opos;
	uint8_t *tail;
	uint8_t *pData;
	uint8_t pOff = 1;
	short pBits;
	short pCodeSize;
	short i;
	short k;
	short code;
	short *head;
	short c;
	short Line;
	short csize;
	short CC;
	short EOI;
	short LCC;
	short merk_LCC;
	short ftable;
	short merk_ftable;
	short oldcode = 0;
	unsigned long pCurrent;
	unsigned short bh, bl;
	long l;
	long EOI_count;
	long Heightinv = Height;
	long AllInAll;
	long MaxBytes;
	long RowCount;
	long busycount;
	long RealWidth;

	/* LZW-Vorbereitungen */
	smurf_struct->services->reset_busybox(0, "decode LZW");

	switch (BitsPerPixel)
	{
	case 1:
		RealWidth = (Width + 7) / 8;
		break;
	case 2:
		RealWidth = (Width + 3) / 4;
		break;
	case 4:
		RealWidth = (Width + 1) / 2;
		break;
	case 8:
		RealWidth = Width;
		break;
	case 24:
		RealWidth = Width * 3L;
		break;
	case 32:
		RealWidth = Width * 4L;
		break;
	default:
		return 0;
	}

	MaxBytes = RealWidth * Height;

	if ((bh = (unsigned short) (Height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) Height;
	bl = 0;								/* busy-length */
	busycount = 0;

	head = (short *)Malloc(head_size + tail_size + out_size + 256L);

	if (head == 0)
	{
		form_alert(1, ERROR1);
		return -1;
	}

#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	tail = (uint8_t *) (head + head_size);
	out = (uint8_t *) (head + head_size + tail_size);

	csize = 9;
	pData = source + DataArray[0];
	pCodeSize = csize;
	pBits = 24;
	AllInAll = 0;
	EOI_count = 0;
	pCurrent = 0;
	RowCount = 0;
	CC = 1 << 8;
	EOI = CC + 1;
	ftable = CC + 2;
	LCC = 1 << csize;
	merk_LCC = LCC;
	merk_ftable = ftable;

	odest = dest;

	for (i = 0; i < CC; i++)
	{
		head[i] = -1;
		tail[i] = i;
	}

	Line = 0;

	for (;;)
	{
		if (RowCount >= RealWidth)
		{
			if (++busycount >= bh)
			{
				busycount = 0;
				smurf_struct->services->busybox(bl);
				bl += 13;
			}

			RowCount -= RealWidth;

			if (++Line >= RowsPerStrip)
			{
				Line = 0;
				code = EOI;
				goto _checkEOI;
			}
		}

		if (AllInAll > MaxBytes)		/* WARNING */
			break;

		while ((24 - pBits) < pCodeSize)
		{
			pCurrent |= (unsigned long) *pData++ << pBits;
			pBits -= 8;
		}

		code = (short) (pCurrent >> (32 - pCodeSize));
		pCurrent <<= pCodeSize;
		pBits += pCodeSize;


	  _checkEOI:
		if (code == EOI)
		{
			if (Heightinv < RowsPerStrip)
				RowsPerStrip = Heightinv;
			Heightinv -= RowsPerStrip;
			Line = 0;
			if (Difference == 2)
				decode_difference(odest, Width, RowsPerStrip, BitsPerPixel);
			odest = dest;

			if (++EOI_count >= DataCount)
				break;
			pData = source + DataArray[EOI_count];
			pBits = 24;
			pCurrent = 0;
			pCodeSize = csize;
			LCC = merk_LCC;
			ftable = merk_ftable;
			RowCount = 0;
		} else
		{
			if (AllInAll > MaxBytes)	/* WARNING */
				break;

			if (code == CC)
			{
				pCodeSize = csize;
				LCC = merk_LCC;
				ftable = merk_ftable;

				while ((24 - pBits) < pCodeSize)
				{
					pCurrent |= (unsigned long) *pData++ << pBits;
					pBits -= 8;
				}

				code = (short) (pCurrent >> (32 - pCodeSize));
				pCurrent <<= pCodeSize;
				pBits += pCodeSize;

				if (code == EOI)
					goto _checkEOI;
				RowCount++;
				AllInAll++;
				*dest++ = code;
				oldcode = code;
			} else						/* dekodierbarer Code! */
			{
				/* Korrekturroutine damit der Decoder auch mit Code */
				/* zurechtkommt die nicht schon bei ftable >= LCC - 1 */
				/* sondern erst bei ftable >= LCC (wie bei GIF) auf */
				/* die n�chste Codegr��e wechseln. */
				if (code > ftable)
				{
					code >>= 1;
					pCodeSize--;
					LCC >>= 1;
					pBits -= pOff;
					pOff = 0;
				}

				if (code < ftable)
				{
					l = 0;
					opos = out;
					c = code;
					do
					{
						*opos++ = tail[c];
						l++;
						c = head[c];
					} while (c != -1);

					k = *(opos - 1);	/* 1. Zeichen output stream merken */
				} else					/* Sonderfall */
				{
					l = 1;
					opos = out + 1;		/* vorne Platz lassen */
					c = oldcode;

					do
					{
						*(opos++) = tail[c];
						l++;
						c = head[c];
					}
					while (c != -1);

					k = *(opos - 1);	/* 1. Zeichen output stream merken */
					*out = k;			/* und vorne anh�ngen */
				}


				AllInAll += l;
				RowCount += l;

				while (l-- > 0)
					*dest++ = *(--opos);

				if (ftable <= 4095)
				{
					head[ftable] = oldcode;
					tail[ftable] = k;
					oldcode = code;
					ftable++;
					if (ftable >= LCC - pOff)
					{
						if (pCodeSize < 12)
						{
							pCodeSize++;
							LCC += LCC;
						}
					}
				}
			}
		}
	}

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu", get_timer());
	(void)Cnecin();
#endif

	Mfree(head);
	return 0;
}


/****************************************************************/
/*				32773 Packbits dekomprimieren					*/
/****************************************************************/
short tiff32773_depack(GARGAMEL *smurf_struct, uint8_t *buffer, uint8_t *ziel, long width, long height, short BitsPerPixel,
	long RowsPerStrip, long *DataArray)
{
	uint8_t *actbuffer;
	uint8_t v1, v2, n;
	unsigned short x, y, yall, bh, bl, busycount;
	long w;
	long Heightinv = height;

	smurf_struct->services->reset_busybox(0, "32773 PackBits");

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
	case 32:
		w = width * 4L;
		break;
	default:
		return 0;
	}

	if ((bh = (unsigned short) (height / 10)) == 0)	/* busy-height */
		bh = (unsigned short) height;
	bl = 0;								/* busy-length */
	busycount = 0;

	yall = 0;
	while (yall < height)
	{
		if (Heightinv < RowsPerStrip)
			RowsPerStrip = Heightinv;
		Heightinv -= RowsPerStrip;

		actbuffer = buffer + *DataArray++;

		y = 0;
		do
		{
			if (++busycount >= bh)
			{
				busycount = 0;
				smurf_struct->services->busybox(bl);
				bl += 13;
			}

			x = 0;
			do
			{
				v1 = *actbuffer++;

				if (v1 & 0x80)
				{
					v2 = *actbuffer++;

					n = 0x101 - v1;

					x += n;

					while (n--)
						*ziel++ = v2;
				} else
				{
					n = v1 + 1;

					x += n;

					while (n--)
						*ziel++ = *actbuffer++;
				}
			} while (x < w);
		} while (++y < RowsPerStrip);
		yall += y;
	}

	return 0;
}


/*****************************************************/
/*	Farbpalette preemptiv auslesen					 */
/*****************************************************/
void tiff24_colormap(uint16_t *source, uint8_t *dest, long colors, short format)
{
	long c;

	if (format == INTEL)
	{
		for (c = 0; c < colors; c++)
		{
			*dest++ = (swap_word(*source) >> 8);
			*dest++ = (swap_word(*(source + colors)) >> 8);
			*dest++ = (swap_word(*(source + colors + colors)) >> 8);
			source++;
		}
	} else
	{
		for (c = 0; c < colors; c++)
		{
			*dest++ = (*source >> 8);
			*dest++ = (*(source + colors) >> 8);
			*dest++ = (*(source + colors + colors) >> 8);
			source++;
		}
	}
}

/*****************************************************/
/*		Farbpalette MIN-IS-BLACK generieren			 */
/*****************************************************/
void tiff24_min_is_black(uint8_t *dest, short Depth)
{
	long c;
	long counter;
	long interpolate;
	long endcol;
	uint8_t farbe;

	endcol = (1L << (long) Depth) - 1;
	interpolate = (256L << 16L) / endcol;
	counter = 0;

	for (c = 0; c <= endcol; c++)
	{
		farbe = (uint8_t) (counter >> 16);
		*dest++ = farbe;
		*dest++ = farbe;
		*dest++ = farbe;
		counter += interpolate;
	}
}


/*****************************************************/
/*		Farbpalette MIN-IS-WHITE generieren			 */
/*****************************************************/
void tiff24_min_is_white(uint8_t *dest, short Depth)
{
	long c;
	long counter;
	long interpolate;
	long endcol;
	uint8_t farbe;

	endcol = (1L << (long) Depth) - 1;
	interpolate = (256L << 16L) / endcol;
	counter = 0;

	for (c = 0; c <= endcol; c++)
	{
		farbe = (uint8_t) (counter >> 16);
		*dest++ = (255 - farbe);
		*dest++ = (255 - farbe);
		*dest++ = (255 - farbe);
		counter += interpolate;
	}
}


void invert_1Bit(uint8_t *data, long length)
{
	uint8_t v;
	do
	{
		v = ~*data;
		*data++ = v;
	} while (--length);
}
