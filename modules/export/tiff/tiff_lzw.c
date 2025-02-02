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
#include "tiff_lzw.h"

#define MAXBITS	12
#define HSIZE	5003
#define HSHIFT	4

#define TIMER	0

/* momentan nur 8, 16 und 24 Bit pixelpacked */
long encode_LZW(uint8_t *ziel, uint8_t *buffer, unsigned long w, unsigned short height, GARGAMEL *smurf_struct)
{
	uint8_t data_size;
	short clear;
	short end_of_information;
	short entries;
	short available;
	short i;
	short clearcodebytes;
	short *ctab;
	short c;
	short disp;
	short ent;
	unsigned short x, y;
	long *vtab, fcode;
	uint8_t *pData;
	short pBitsLeft, pCodeSize;
	unsigned long pDatum;


#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	smurf_struct->services->reset_busybox(0, "encode LZW");

	ctab = (short *) Malloc(HSIZE * sizeof(*ctab));
	vtab = (long *) Malloc(HSIZE * sizeof(*vtab));

	/* Initalisieren der LZW-Variablen */
	data_size = 8;
	clear = 1 << data_size;				/* Clearcode bestimmen */
	end_of_information = clear + 1;		/* end-of-information Code */
	available = clear + 2;				/* erster freier Tabelleneintrag */
	pCodeSize = data_size + 1;			/* Bitanzahl der n�chsten Codegr��e */
	entries = 1 << pCodeSize;			/* erster Code der n�chsten Codegr��e */

	pBitsLeft = 0;
	pDatum = 0;
	pData = ziel;
	clearcodebytes = 0;

	/* ersten Clearcode schreiben */
	clearcodebytes++;

	pDatum |= (unsigned long) clear;	/* garantiert erster Wert, deshalb kein Shifting notwendig */
	pBitsLeft += pCodeSize;

	for (i = 0; i < HSIZE; i++)
		vtab[i] = -1;

	ent = *buffer++;

	y = 0;
	do
	{
		x = (unsigned short) w;
		if (y == 0)
			x--;						/* die erste Zeile hat ein Byte weniger weil oben schon weggenommen */
		while (x)
		{
			c = *buffer++;
			x--;

			fcode = (long) (((long) c << MAXBITS) | ent);
			i = (((short) c << HSHIFT) ^ ent);

			if (vtab[i] != -1)			/* empty slot */
			{
				if (vtab[i] == fcode)	/* right code found */
				{
					ent = ctab[i];
					continue;
				}

				/* secondary hash (after G. Knott) */
				if (i == 0)
					disp = 1;
				else
					disp = HSIZE - i;

				/* probe */
				for (;;)
				{
					if ((i -= disp) < 0)
						i += HSIZE;

					if (vtab[i] == fcode)	/* right code found */
					{
						ent = ctab[i];
						goto next;
					} else if (vtab[i] == -1)	/* empty slot */
						break;
				}
			}

			/* nomatch */
			pDatum = (pDatum << pCodeSize) | (unsigned long) ent;
			pBitsLeft += pCodeSize;

			while (pBitsLeft >= 8)
			{
				*pData++ = (pDatum >> (pBitsLeft - 8));
				pBitsLeft -= 8;
			}

			if (available < 4094)
			{
				if (available >= entries - 1)
				{
					pCodeSize++;
					entries <<= 1;
				}

				ctab[i] = available++;	/* code -> hashtable */
				vtab[i] = fcode;
			} else
			{
				/* Clearcode aussto�en */
				clearcodebytes++;

				pDatum = (pDatum << pCodeSize) | (unsigned long) clear;
				pBitsLeft += pCodeSize;

				while (pBitsLeft >= 8)
				{
					*pData++ = (pDatum >> (pBitsLeft - 8));
					pBitsLeft -= 8;
				}

				/* Reset Encoder */
				available = clear + 2;
				pCodeSize = data_size + 1;
				entries = 1 << pCodeSize;

				for (i = 0; i < HSIZE; i++)
					vtab[i] = -1;
			}

			ent = c;

		  next:
			continue;
		}
	} while (++y < height);

	/* letztes Byte ausgeben */
	pDatum = (pDatum << pCodeSize) | (unsigned long) ent;
	pBitsLeft += pCodeSize;

	/* EOI ausgeben */
	pDatum = (pDatum << pCodeSize) | (unsigned long) end_of_information;
	pBitsLeft += pCodeSize;

	while (pBitsLeft > 8)
	{
		*pData++ = (pDatum >> (pBitsLeft - 8));
		pBitsLeft -= 8;
	}

	if (pBitsLeft > 0)					/* bis wirklich alles geschrieben ist */
	{
		*pData++ = (pDatum << (8 - pBitsLeft));
		pBitsLeft -= 8;
	}
	
	Mfree(vtab);
	Mfree(ctab);

#if TIMER
	/* wie schnell waren wir? */
	printf("%lu\n", get_timer());
	(void)Cnecin();
#endif

	return pData - ziel;
}
