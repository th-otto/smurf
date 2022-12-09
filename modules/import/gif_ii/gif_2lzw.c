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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "import.h"
#include "gif_2lzw.h"
#include "demolib.h"

#define MaxStackSize	4096

#define DEBUG	0
#define TIMER	0

#define ASSEMBLER	1

struct gif_par {
	short data_size;
	short clear;
	short end_of_information;
	short available;
	short pCodeSize;
	short entries;
	short pCodeMask;
	uint8_t *ziel;
	uint8_t *pData;
	short *srclen;
	uint8_t **src;
};


/* Set 16 Pixel (Standard Format) Assembler-Rout */
void decode_fast(struct gif_par *par) ASM_NAME("_decode_fast");
void decode_fast020(struct gif_par *par) ASM_NAME("_decode_fast020");

/* decodiert alle GIF mit 1-7 Bit */
short decode_lzw_normal(uint8_t *buffer, uint8_t *ziel, short width, short height, uint8_t BitsPerPixel)
{
	short *prefix;
	uint8_t *suffix;
	uint8_t *out;
	uint8_t *opos;
	uint8_t *pixbuf;
	uint8_t data_size;

	short code;
	short oldcode = 0;
	short clear, end_of_information, entries, available;
	short x, l, c, k, finished, out_count, width2, v, d;

	uint8_t *pData;
	unsigned short pBitsLeft, pCodeMask, pCodeSize, pCount;
	unsigned long pDatum;

	long planelength, len_to_go;

#if TIMER
/* wie schnell sind wir? */
	init_timer();
#endif

	planelength = (((long) width + 7) / 8) * height;	/* Auf volle Byte gerundete ZeilenlÑnge in Byte */
	width2 = width / 16 * 16;
	v = width % 16;

	if (v == 0)
	{
		width2 -= 16;
		v = 16;
	}

	/* Speicher fÅr LZW-Arrays anfordern */
	prefix = (short *) Malloc(MaxStackSize * sizeof(*prefix) + MaxStackSize * sizeof(*suffix) * 2 + ((width + 15) / 16) * 16 * sizeof(*pixbuf));
	if (prefix == NULL)
		return -1;
	suffix = (uint8_t *) (prefix + MaxStackSize);
	out = suffix + MaxStackSize;
	pixbuf = out + ((width + 15) / 16) * 16;

	/* Initalisieren der LZW-Variablen */
	data_size = *buffer++;				/* aktuelle Codegrîûe auslesen */
	clear = 1 << data_size;				/* Clearcode bestimmen */
	end_of_information = clear + 1;		/* end-of-information Code */
	available = clear + 2;				/* erster freier Tabelleneintrag */
	pCodeSize = data_size + 1;			/* Bitanzahl der nÑchsten Codegrîûe */
	entries = 1 << pCodeSize;			/* erster Code der nÑchsten Codegrîûe */
	pCodeMask = (1 << pCodeSize) - 1;	/* Codemaske setzen */

	/* initialisieren der ersten 1 << Codegrîûe EintrÑge */
	for (code = 0; code < clear; code++)
	{
		prefix[code] = -1;
		suffix[code] = code;
	}

#if DEBUG
	printf("\ndata_size: %d, Position: %p", data_size, buffer);
	(void)Cnecin();
#endif

	pBitsLeft = 0;
	pDatum = 0;
	pCount = 0;
	pData = buffer;

	len_to_go = filelen;
	out_count = 0;
	x = 0;
	finished = FALSE;
	while (!finished)
	{
		/* Code holen */
		if (pBitsLeft < pCodeSize)
		{
			if (pCount == 0)			/* neuen Block beginnen */
			{
				pCount = *pData++;		/* BlocklÑnge auslesen */
				if (pCount == 0 || (len_to_go -= pCount) < 0)
					break;
			}

			pDatum |= (unsigned long) *pData++ << pBitsLeft;
			pCount--;
			pBitsLeft += 8;

			if (pCount == 0)			/* neuen Block beginnen */
			{
				pCount = *pData++;		/* BlocklÑnge auslesen */
				if (pCount == 0 || (len_to_go -= pCount) < 0)
					break;
			}

			pDatum |= (unsigned long) *pData++ << pBitsLeft;
			pCount--;
			pBitsLeft += 8;
		}

		/* long muû auf int und gebracht und dann verundet werden, */
		/* oder die Maske muû auf long gebracht werden - aber die long- */
		/* VerknÅpfung wÑre wahrscheinlich langsamer. */
		code = (short) pDatum & pCodeMask;
		pDatum >>= pCodeSize;
		pBitsLeft -= pCodeSize;

		if (code == clear)				/* Clearcode? */
		{
			/* Reset Decoder */
			available = clear + 2;
			pCodeSize = data_size + 1;
			entries = 1 << pCodeSize;
			pCodeMask = (1 << pCodeSize) - 1;

			/* Code holen */
			if (pBitsLeft < pCodeSize)
			{
				if (pCount == 0)		/* neuen Block beginnen */
					pCount = *pData++;	/* BlocklÑnge auslesen */

				pDatum |= (unsigned long) *pData++ << pBitsLeft;
				pCount--;
				pBitsLeft += 8;

				if (pCount == 0)		/* neuen Block beginnen */
					pCount = *pData++;	/* BlocklÑnge auslesen */

				pDatum |= (unsigned long) *pData++ << pBitsLeft;
				pCount--;
				pBitsLeft += 8;
			}

			/* long muû auf int und gebracht und dann verundet werden, */
			/* oder die Maske muû auf long gebracht werden - aber die long- */
			/* VerknÅpfung wÑre wahrscheinlich langsamer. */
			code = (short) pDatum & pCodeMask;
			pDatum >>= pCodeSize;
			pBitsLeft -= pCodeSize;

			pixbuf[x++] = code;			/* Ist garantiert ein bekannter Code */

			if (x == width)
			{
				out_count = 0;
				while (out_count < width2)
				{
					setpix_standard(pixbuf + out_count, ziel, BitsPerPixel, planelength, 16);
					ziel += 2;
					out_count += 16;
				}

				d = setpix_standard(pixbuf + out_count, ziel, BitsPerPixel, planelength, v);
				ziel += d;

				x = 0;
			}

			oldcode = code;
		} else
		{
			if (code == end_of_information)
				finished = TRUE;
			else
			{
				/* Dekodierbarer Code */
				if (code < available)	/* Code schon bekannt? */
				{
					/* Schreibe Muster in Ausgabe */
					opos = out;
					l = 0;
					c = code;
					do
					{
						*opos++ = suffix[c];
						l++;
					} while ((c = prefix[c]) != -1);	/* von hinten her aufrollen */

					k = *(opos - 1);	/* 1. Zeichen output stream merken */
				} else
				{
					opos = out + 1;		/* vorne Platz lassen */
					l = 1;
					c = oldcode;
					do
					{
						*opos++ = suffix[c];
						l++;
					} while ((c = prefix[c]) != -1);	/* von hinten her aufrollen */

					k = *(opos - 1);	/* 1. Zeichen output stream merken */
					*out = k;			/* und vorne anhÑngen */
				}						/* else */

				/* Ausgabe der Zeichenkette richtig herum */
				do
				{
					pixbuf[x++] = *(--opos);

					if (x == width)
					{
						out_count = 0;
						while (out_count < width2)
						{
							setpix_standard(pixbuf + out_count, ziel, BitsPerPixel, planelength, 16);
							ziel += 2;
							out_count += 16;
						}

						d = setpix_standard(pixbuf + out_count, ziel, BitsPerPixel, planelength, v);
						ziel += d;

						x = 0;
					}
				} while (--l);

				/* pflegen der Tabelle */
				prefix[available] = oldcode;
				suffix[available] = k;
				available++;
				oldcode = code;
			}

			/* Codegrîûe erhîhen wenn nîtig und mîglich */
			if (available >= entries && pCodeSize < 12)
			{
				pCodeSize++;
				pCodeMask += available;
				entries <<= 1;
			}
		}
	}									/* while */

#if TIMER
	/* wie schnell waren wir? */
	printf("\nZeit: %lu", get_timer());
	(void)Cnecin();
#endif

	Mfree(prefix);

	return 0;
}


/*
 * decodiert alle GIF mit 8 Bit 
 */
short decode_lzw_fast(uint8_t *buffer, uint8_t *ziel)
{
	uint8_t *firstcodes;
	short *srclen;
	uint8_t data_size;

	short code, clear, end_of_information, entries, available;

	uint8_t **src;

	uint8_t *pData;
	unsigned short pCodeMask, pCodeSize;

#if ASSEMBLER
	struct gif_par par;
#else
	short i;
	short finished;
	short codelen;
	short oldcodelen;
	uint8_t *out;
	uint8_t *dst;
	short oldcode;
	unsigned short pCount;
	unsigned short pBitsLeft;
	unsigned long pDatum;
#endif

#if TIMER
	init_timer();
#endif

	/*
	 * Speicher fÅr LZW-Arrays anfordern
	 */
	srclen = (short *) Malloc(MaxStackSize * sizeof(*srclen) + MaxStackSize * sizeof(*src) + 256 * sizeof(*firstcodes));
	if (srclen == NULL)
		return -1;
	src = (uint8_t **) (srclen + MaxStackSize);
	firstcodes = (uint8_t *) (src + MaxStackSize);	/* maximum needed */

	/*
	 * initalisieren der LZW-Variablen
	 */
	data_size = *buffer++;				/* aktuelle Codegrîûe auslesen */
	clear = 1 << data_size;				/* Clearcode bestimmen */
	end_of_information = clear + 1;		/* end-of-information Code */
	available = clear + 2;				/* erster freier Tabelleneintrag */
	pCodeSize = data_size + 1;			/* Bitanzahl der nÑchsten Codegrîûe */
	entries = 1 << pCodeSize;			/* erster Code der nÑchsten Codegrîûe */
	pCodeMask = (1 << pCodeSize) - 1;	/* Codemaske setzen */

	/*
	 * initialisieren der ersten 1 << Codegrîûe EintrÑge
	 */
	for (code = 0; code < clear; code++)
	{
#if ASSEMBLER
		srclen[code] = 0;				/* mÅûte eigentlich 1 sein, aber in der ASS-Rout wird dbra verwendet, um das Muster zu schreiben */
#else
		srclen[code] = 1;
#endif

		*firstcodes = code;
		src[code] = firstcodes;
		firstcodes++;
	}

	pData = buffer;

#if ASSEMBLER
	par.data_size = data_size;
	par.clear = clear;
	par.end_of_information = end_of_information;
	par.available = available;
	par.pCodeSize = pCodeSize;
	par.entries = entries;
	par.pCodeMask = pCodeMask;
	par.src = src;
	par.srclen = srclen;
	par.ziel = ziel;
	par.pData = pData;
	if (PROCESSOR & (MC68020 | MC68030 | MC68040 | MC68060))
		decode_fast020(&par);
	else
		decode_fast(&par);

#if DEBUG
	printf("Decoded!");
	(void)Cnecin();
#endif

#else

	pBitsLeft = 0;
	pDatum = 0;
	pCount = 0;

	finished = FALSE;
	while (!finished)
	{
		/* Code holen */
		if (pBitsLeft < pCodeSize)
		{
			if (pCount == 0)			/* neuen Block beginnen */
			{
				pCount = *pData++;		/* BlocklÑnge auslesen */
				if (pCount == 0)
					break;
			}

			pDatum |= (unsigned long) *pData++ << pBitsLeft;
			pCount--;
			pBitsLeft += 8;

			if (pCount == 0)			/* neuen Block beginnen */
			{
				pCount = *pData++;		/* BlocklÑnge auslesen */
				if (pCount == 0)
					break;
			}

			pDatum |= (unsigned long) *pData++ << pBitsLeft;
			pCount--;
			pBitsLeft += 8;
		}

		code = (short) pDatum & pCodeMask;
		pDatum >>= pCodeSize;
		pBitsLeft -= pCodeSize;

		if (code == clear)				/* Clearcode? */
		{
			/* Reset Decoder */
			available = clear + 2;
			pCodeSize = data_size + 1;
			entries = 1 << pCodeSize;
			pCodeMask = (1 << pCodeSize) - 1;

			/* Code holen */
			if (pBitsLeft < pCodeSize)
			{
				if (pCount == 0)		/* neuen Block beginnen */
					pCount = *pData++;	/* BlocklÑnge auslesen */

				pDatum |= (unsigned long) *pData++ << pBitsLeft;
				pCount--;
				pBitsLeft += 8;

				if (pCount == 0)		/* neuen Block beginnen */
					pCount = *pData++;	/* BlocklÑnge auslesen */

				pDatum |= (unsigned long) *pData++ << pBitsLeft;
				pCount--;
				pBitsLeft += 8;
			}

			code = (short) pDatum & pCodeMask;
			pDatum >>= pCodeSize;
			pBitsLeft -= pCodeSize;

			*ziel++ = code;				/* Ist garantiert ein bekannter Code */

			oldcode = code;
			oldcodelen = 1;
		} else
		{
			if (code == end_of_information)
			{
				finished = TRUE;
			} else
			{
				/* Dekodierbarer Code */
				if (code < available)	/* Code schon bekannt? */
				{
					dst = ziel - oldcodelen;

					/* Schreibe Muster in Ausgabe */
					i = codelen = srclen[code];
					out = src[code];
					do
					{
						*ziel++ = *out++;
					} while (--i);
				} else					/* neuer Code */
				{
					dst = ziel;

					/* Schreibe Muster in Ausgabe */
					i = codelen = srclen[oldcode];
					out = src[oldcode];
					do
					{
						*ziel++ = *out++;
					} while (--i);
					*ziel++ = *(src[oldcode]);
					codelen++;
				}

				/* pflegen der Tabelle */
				srclen[available] = oldcodelen + 1;
				src[available] = dst;
				available++;
				oldcode = code;
				oldcodelen = codelen;
			}

			/* Codegrîûe erhîhen wenn nîtig und mîglich */
			if (available >= entries && pCodeSize < 12)
			{
				pCodeSize++;
				pCodeMask += available;
				entries <<= 1;
			}
		}
	}
#endif

#if TIMER
	printf("\nZeit: %lu", get_timer());
	(void)Cnecin();
#endif

	Mfree(srclen);

	return 0;
}
