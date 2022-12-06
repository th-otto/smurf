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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

/*
		Smurf MIM Loader
*/

#include "country.h"

#if COUNTRY==1
#define alertstr "[2][Die Datei enth„lt keine|Farbtabelle! Welche Farbpalette|soll generiert werden?|(Ebenenbilder sollten als|Graustufen geladen werden.)][Standard|Graustufen]"
#elif COUNTRY==0
#define alertstr "[2][The file contains no|colortable! Which colortable should|be generated?|(...images should be loaded|with greyscales.)][Standard|Greyscale]"
#elif COUNTRY==2
#define alertstr "[2][The file contains no|colortable! Which colortable should|be generated?|(...images should be loaded|with greyscales.)][Standard|Greyscale]"
#else
#error "keine Sprache!"
#endif

#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"

static unsigned char const orgft[3 * 16] = {
	255, 255, 255,
	255, 0, 0,
	0, 255, 0,
	255, 255, 0,
	0, 0, 255,
	255, 0, 255,
	0, 255, 255,
	200, 200, 200,
	128, 128, 128,
	180, 0, 0,
	0, 180, 0,
	180, 180, 0,
	0, 0, 180,
	180, 0, 180,
	0, 180, 180,
	0, 0, 0
};

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"MIM Format",
	0x0120,
	"Bj”rn Spruck",
	{ "MIM", "", "", "", "", "", "", "", "", "" },
/* Objekttitel */
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
/* Objektgrenzwerte */
	0, 128, 0, 128, 0, 128, 0, 128,
	0, 10, 0, 10, 0, 10, 0, 10,
/* Slider-Defaultwerte */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0,
	NULL, NULL, NULL, NULL, NULL, NULL
};

static uint8_t suf[256];
static short pre[256];
static short tabl;


static long unpack_upd(uint8_t *s, uint8_t *d, long k)
{
	long l = 0;
	uint8_t vorz, ina;
	short w;
	short ll = 0;
	short i;
	short invers[256];

	tabl = 0;

	vorz = *s++;
	while (k > 0)
	{
		w = *s++;
		if (vorz & 128)
			w += 256;
		if (ll == 7)
		{
			ll = 0;
			vorz = *s++;
			k--;
			if (k == 0)
				break;
		} else
		{
			vorz <<= 1;
			ll++;
		}
		ina = 0;
		pre[tabl] = w;
		while (w > 255)
		{
			w -= 256;
			/* inverse Ausgabe von suf[w]   */
			invers[ina] = w;
			w = pre[w];
			ina++;
		}
		*(d++) = w;
		l++;
		if (tabl > 0)
			suf[tabl - 1] = w;
		tabl++;
		if (tabl == 256)
			tabl = 0;
		for (i = 0; i < ina; i++)
		{
			*(d++) = suf[invers[ina - i - 1]];
			l++;
		}

		k--;
	}

	return (l);
}


static void convertiere_bild(uint16_t *sc1, uint16_t *sc2, unsigned short x, unsigned short y, unsigned short e)
{
	long i;

	switch (e)
	{
	case 1:
		{
			for (i = 0; i < (x + 15) / 16 * y; i++)
			{
				*sc2++ = *sc1++;
			}
		}
		break;
	case 2:
		{
			uint16_t *s1, *s2;

			s1 = sc2;
			s2 = s1 + (x + 15) / 16 * y;
			for (i = 0; i < (x + 15) / 16 * y; i++)
			{
				*s1++ = *sc1++;
				*s2++ = *sc1++;
			}
		}
		break;
	case 4:
		{
			uint16_t *s1, *s2, *s3, *s4;

			s1 = sc2;
			s2 = s1 + (x + 15) / 16 * y;
			s3 = s2 + (x + 15) / 16 * y;
			s4 = s3 + (x + 15) / 16 * y;
			for (i = 0; i < (x + 15) / 16 * y; i++)
			{
				*s1++ = *sc1++;
				*s2++ = *sc1++;
				*s3++ = *sc1++;
				*s4++ = *sc1++;
			}
		}
		break;
	case 8:
		{
			uint16_t *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8;

			s1 = sc2;
			s2 = s1 + (x + 15) / 16 * y;
			s3 = s2 + (x + 15) / 16 * y;
			s4 = s3 + (x + 15) / 16 * y;
			s5 = s4 + (x + 15) / 16 * y;
			s6 = s5 + (x + 15) / 16 * y;
			s7 = s6 + (x + 15) / 16 * y;
			s8 = s7 + (x + 15) / 16 * y;
			for (i = 0; i < (x + 15) / 16 * y; i++)
			{
				*s1++ = *sc1++;
				*s2++ = *sc1++;
				*s3++ = *sc1++;
				*s4++ = *sc1++;
				*s5++ = *sc1++;
				*s6++ = *sc1++;
				*s7++ = *sc1++;
				*s8++ = *sc1++;
			}
		}
		break;
	case 16:
		{
			for (i = 0; i < (x + 15) / 16 * y * 32; i++)
			{
				*sc2++ = *sc1++;
			}
		}
		break;
	}
}


static unsigned long unpack(uint8_t *s, uint8_t *d, unsigned long ls)
{
	unsigned long i;
	unsigned long le = 0;
	uint8_t w, len;

	for (i = 0; i < ls; i++)
	{
		if (*s > 0 && *s < 128)
		{
			len = *s;
			s++;
			w = *s;
			s++;
			le += len;
			i++;
			memset(d, w, len);
			d += len;
		}
	}
	return (le);
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		MIM Format (by Sage) 				.MIM	*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *buffer;
	uint8_t t[32];
	unsigned short width, height, feb;
	void *bs;
	void *bso = 0;
	void *smbuffer;
	void *locbuffer = 0;
	short not_conv = 0;
	long l;
	short grey = 0;

	smbuffer = smurf_struct->smurf_pic->pic_data;
	buffer = smurf_struct->smurf_pic->pic_data;
	memcpy(t, buffer, 32);

	if (*(uint32_t *) buffer != 0x4d494d46L) /* 'MIMF' */
	{
		return M_INVALID;
	}

	strcpy(smurf_struct->smurf_pic->format_name, "MIM Format");

	if (t[4] >= 3)
		not_conv = 1;
	feb = t[5];
	if (feb >= 16)
		not_conv = 1;
	width = *((uint16_t *) &t[6]);
	height = *((uint16_t *) &t[8]);

	buffer += 32;

	{
		uint8_t *f = (uint8_t *) smurf_struct->smurf_pic->palette;

		if (t[11] & 1)
		{								/*    Farbpalette lesen   */
			unsigned short i;

			for (i = 0; i < *((uint16_t *) &t[16]); i++)
			{
				*f++ = *buffer++;
				*f++ = *buffer++;
				*f++ = *buffer++;
			}
		} else
		{
			if (feb <= 8)
			{
				short i;
				short flag = 2;

				if (feb != 1)
					flag = form_alert(2, alertstr);
				if (flag == 1)
				{
					for (i = 0; i < 16; i++)
					{
						*f++ = orgft[i * 3];
						*f++ = orgft[i * 3 + 1];
						*f++ = orgft[i * 3 + 2];
					}
				} else
				{
					grey = 1;
					switch (feb)
					{
					case 1:
						*f++ = 0;
						*f++ = 0;
						*f++ = 0;
						*f++ = 255;
						*f++ = 255;
						*f = 255;
						break;
					case 2:
						*f++ = 0;
						*f++ = 0;
						*f++ = 0;
						*f++ = 100;
						*f++ = 100;
						*f++ = 100;
						*f++ = 190;
						*f++ = 190;
						*f++ = 190;
						*f++ = 255;
						*f++ = 255;
						*f = 255;
						break;
					case 4:
						for (i = 0; i < 16; i++)
						{
							*f++ = 255 * i / 15;
							*f++ = 255 * i / 15;
							*f++ = 255 * i / 15;
						}
						break;
					case 8:
						for (i = 0; i < 256; i++)
						{
							*f++ = i;
							*f++ = i;
							*f++ = i;
						}
						break;
					}
				}
			}
		}
	}
	if (t[11] & 4)
		not_conv = 1;

	memmove(smbuffer, buffer, *((uint32_t *) &t[12]));
	bs = smbuffer;
	l = (unsigned long) (width + 15) / 16 * 2 * feb * height;

	if (t[11] & 2 && t[11] & 4)
	{
		locbuffer = (uint8_t *)Malloc(l);
		if (locbuffer == 0)
		{
			return (M_MEMORY);
		}
		/*l2 = */
		unpack(bs, locbuffer, *((uint32_t *) &t[12]));
		bs = locbuffer;
		bso = smbuffer;
	} else
	{
		if (t[11] & 8)
		{
			locbuffer = (uint8_t *)Malloc(l);
			if (locbuffer == 0)
			{
				return (M_MEMORY);
			}
			/*l2 = */
			unpack_upd(bs, locbuffer, *((uint32_t *) &t[12]) - 1);
			bs = locbuffer;
			bso = smbuffer;
		} else
		{
			memmove(smurf_struct->smurf_pic->pic_data, bs, (unsigned long) (width + 15) / 16 * 2 * feb * height);
			bs = smurf_struct->smurf_pic->pic_data;
			_Mshrink(bs, (unsigned long) (width + 15) / 16 * 2 * feb * height);
		}
	}

	if (not_conv == 0)
	{
		if (locbuffer == 0)
		{
			locbuffer = (uint8_t *)Malloc(l);
			if (locbuffer)
			{
				convertiere_bild(bs, locbuffer, width, height, feb);
				bso = bs;
				bs = locbuffer;
			}
		}
	}

	if (bso)
	{
		Mfree(bso);
	}

	if (t[11] & 4)
	{
		if (width & 0xF)
		{								/* Keine MOD 16 Pixelbreite! */
			unsigned short j;
			uint8_t *bs1, *bs2;

			bs1 = bs;
			bs2 = bs;
			for (j = 0; j < height; j++)
			{
				memcpy(bs1, bs2, width);
				bs1 += width;
				bs2 += (width + 15) & 0xFFF0;
			}
			_Mshrink(bs, (unsigned long) width * height);
		}
	} else
	{
		if ((width & 0xF) <= 8 && (width & 0xF) != 0)
		{
			unsigned short i, j;
			uint8_t *bs1, *bs2;

			bs1 = bs;
			bs2 = bs;
			for (i = 0; i < feb; i++)
			{
				for (j = 0; j < height; j++)
				{
					memcpy(bs1, bs2, (width + 7) / 8);
					bs1 += (unsigned long) (width + 7) / 8;
					bs2 += (unsigned long) (width + 15) / 16 * 2;
				}
			}
			_Mshrink(bs, (unsigned long) (width + 7) / 8 * feb * height);
		}
	}

	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = feb;
	if (grey)
	{
		smurf_struct->smurf_pic->col_format = GREY;
	} else
	{
		smurf_struct->smurf_pic->col_format = RGB;
	}
	if (feb <= 8 && (t[11] & 4) == 0)
	{
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	} else
	{									/* Pixelpacked auch fr Ebenen!!! */
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	}
	smurf_struct->smurf_pic->pic_data = bs;

	return M_PICDONE;
}
