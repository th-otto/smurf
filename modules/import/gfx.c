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
		Smurf GFX Loader
*/

#include <stdio.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"XGrabber",
	0x0110,
	"Bjîrn Spruck",
	{ "GFX", "", "", "", "", "", "", "", "", "" },
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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};

/*
Grober Aufbau:

   38 Bytes Auflîsungsinformationen
 1024 Bytes Farbinformationen 256-Farben Falcon-Palette
   32 Bytes Farbinformationen 16-Farben STE-Palette
    ? Bytes Bildinformationen

Die Auflîsungsinformationen:

 LÑnge | Hardwareadresse | Beschreibung
 ------|-----------------|-------------------------------
0  .w  | $FFFF8260.w     | ST-Shift-Mode
1  .w  | $FFFF8266.w     | Falcon-Shift-Mode
2  .w  | $FFFF8282.w     | Horizontal Hold-Timer
3  .w  | $FFFF8284.w     | Horizontal Border-Begin
4  .w  | $FFFF8286.w     | Horizontal Border-End
5  .w  | $FFFF8288.w     | Horizontal Display-Begin
6  .w  | $FFFF828A.w     | Horizontal Display-End
7  .w  | $FFFF828C.w     | Horizontal Sync-Start
8  .w  | $FFFF82A2.w     | Vertical Frequenz-Timer
9  .w  | $FFFF82A4.w     | Vertical Border-Begin
10 .w  | $FFFF82A6.w     | Vertical Border-End
11 .w  | $FFFF82A8.w     | Vertical Display-Begin  (VDB)
12 .w  | $FFFF82AA.w     | Vertical Display-End    (VDE)
13 .w  | $FFFF82AC.w     | Vertical Sync-Start
14 .w  | $FFFF82C0.w     | Video Control
15 .w  | $FFFF82C2.w     | Video Mode
16 .w  | $FFFF820E.w     | Line Offset
17 .w  | $FFFF8210.w     | Line-Wide
18 .w  | $FFFF8264.w     | H-Scroll
*/

static void convertiere_bild(unsigned short *sc1, unsigned short *sc2, unsigned short x, unsigned short y, unsigned short e)
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
			unsigned short *s1,
			*s2;

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
			unsigned short *s1,
			*s2,
			*s3,
			*s4;

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
			unsigned short *s1,
			*s2,
			*s3,
			*s4,
			*s5,
			*s6,
			*s7,
			*s8;

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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		GFX Format (by Sage) 				.GFX	*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *bs;
	uint8_t *locbuffer;
	short fmode = 1;
	short fe = 4;
	short *head;
	short w, h;

	if (smurf_struct->smurf_pic->file_len <= 38 + 1024 + 32)
		return M_INVALID;

	bs = smurf_struct->smurf_pic->pic_data;

	head = (void *) bs;
	bs += 38;							/* Header */

	w = head[16] + head[17];
	h = (head[12] - head[11]) / 2;
	if (head[15] & 0x1)
		h = h / 2;
	if (head[15] & 0x2)
		h = h * 2;
	if (head[1] & 0x10)
	{
		fe = 8;
		w += w;
	} else
	{
		if (head[1] & 0x100)
		{
			fe = 16;
		} else
		{
			fe = 4;
			w += w;
			w += w;
			if (head[0] & 0x100)
			{
				fe = 2;
				w += w;
				fmode = 0;
			}
			if (head[1] & 0x400)
			{
				fe = 1;
				w += w;
				w += w;
			}
		}
	}
	if (fe != 16)
		w = (w + 15) & 0xFFF0;

/*	if( head[5]<0xb0) fmode=0;*/
	if (smurf_struct->smurf_pic->file_len != 38 + 1024 + 32 + (long) w * h / 8 * fe)
		return M_INVALID;

	strcpy(smurf_struct->smurf_pic->format_name, "XGrabber Format");

	{
		uint8_t *f = (uint8_t *) smurf_struct->smurf_pic->palette;
		int i;

		if (fmode)
		{
			for (i = 0; i < 256; i++)
			{
				*f++ = *bs++;
				*f++ = *bs++;
				bs++;
				*f++ = *bs++;
			}
			bs += 32;
		} else
		{
			bs += 1024;
/*
  321076543210
  ||||||||||||
  ||||||||++++--Blue intensity [ Order of bits : 0321 ]
  ||||++++------Green intensity
  ++++----------Red intensity
*/
			for (i = 0; i < 16; i++)
			{
				uint16_t x;

				x = *(uint16_t *) bs;
				*f++ = (((x >> 3) & 0xE0) + ((x & 0x800) ? 0x10 : 0));
				*f++ = (((x << 1) & 0xE0) + ((x & 0x80) ? 0x10 : 0));
				*f++ = (((x << 5) & 0xE0) + ((x & 0x8) ? 0x10 : 0));
				bs += 2;
			}
		}
	}
	memcpy(head, bs, (long) w * h / 8 * fe);
	bs = (uint8_t *) head;
	if (fe <= 8 && fe > 1)
	{
		locbuffer = (uint8_t *)Malloc((long) w * h / 8 * fe);
		if (locbuffer == 0)
			return M_MEMORY;
		convertiere_bild((unsigned short *) bs, (unsigned short *) locbuffer, w, h, fe);
		Mfree(bs);
		bs = locbuffer;
	}

	smurf_struct->smurf_pic->pic_width = w;
	smurf_struct->smurf_pic->pic_height = h;
	smurf_struct->smurf_pic->depth = fe;
	smurf_struct->smurf_pic->col_format = RGB;
	if (fe <= 8)
	{
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	} else
	{
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	}
	_Mshrink(bs, (long) w * h / 8 * fe);
	smurf_struct->smurf_pic->pic_data = bs;

	return M_PICDONE;
}
