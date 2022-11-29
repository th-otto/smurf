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
/*					s/w Schwellwert-Modul					*/
/*	Sonderversion um 16 Farben Snapshots verlustfrei nach	*/
/*	2 Farben zu bringen.									*/
/* Version 0.1  --  09.10.97								*/
/* =========================================================*/

#include "country.h"

#if COUNTRY == 1
#define TEXT1 "s/w Schwellwert"
#define TEXT2 "Schwellwert"
#define TEXT3 "1 Bit ausgeben"
#elif COUNTRY == 0
#define TEXT1 "b/w Threshold"
#define TEXT2 "Threshold"
#define TEXT3 "Convert to mono"
#else
#define TEXT1 "s/w Schwellwert"
#define TEXT2 "Schwellwert"
#define TEXT3 "Convert to mono"
#endif

#undef TEXT1
#undef TEXT2
#undef TEXT3
#define TEXT1 "s/w Schwellwert"
#define TEXT2 "Schwellwert"
#define TEXT3 "1 Bit ausgeben"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#define TIMER 0

MOD_INFO module_info = {
	TEXT1,
	0x0050,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", ""},
/* 4 Sliderueberschriften: max 8 */
	TEXT2, "", "", "",
/* 4 Checkboxueberschriften: */
	TEXT3, "", "", "",
/* 4 Edit-Objekt-Ueberschriften: */
	"", "", "", "",
/* min/max-Werte fr Slider */
	0, 255,
	0, 64,
	0, 64,
	0, 64,
/* min/max fr Editobjekte */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
/* Defaultwerte fr Slider, Check und Edit */
	128, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
/* Anzahl der Bilder */
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	1, 2, 4, 7, 8, 16, 24, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	0,
};

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

static void do_mode1(SMURF_PIC *pic, unsigned long grenze);
static void do_mode2(SMURF_PIC *pic, unsigned long grenze);

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*	Sonderversion um 16 Farben Snapshots verlustfrei*/
/*	nach 2 Faren zu bringen.						*/
/*		1-8, 16 und 24 Bit							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short module_id;
	unsigned long grenze;

	module_id = smurf_struct->module_number;

	/* Wenn das Modul zum ersten Mal gestartet wurde, */
	/* ein Einstellformular anfordern....             */
	if (smurf_struct->module_mode == MSTART)
	{
		smurf_struct->services->f_module_prefs(&module_info, module_id);
		smurf_struct->module_mode = M_WAITING;
		return;
	}
	/* Einstellformular wurde mit START verlassen - Funktion ausfhren */
	else if (smurf_struct->module_mode == MEXEC)
	{
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
		
#if TIMER
		/* wie schnell sind wir? */
		init_timer();
#endif

		grenze = smurf_struct->slide1 << 12;

		if (smurf_struct->check1)
			do_mode1(smurf_struct->smurf_pic, grenze);
		else
			do_mode2(smurf_struct->smurf_pic, grenze);

#if TIMER
		/* wie schnell waren wir? */
		printf("\n%lu\n", get_timer());
		getch();
#endif

		smurf_struct->module_mode = M_PICDONE;
		return;
	}
	/* Mterm empfangen - Speicher freigeben und beenden */
	else if (smurf_struct->module_mode == MTERM)
	{
		smurf_struct->module_mode = M_EXIT;
		return;
	}
}


static void do_mode2(SMURF_PIC *smurf_pic, unsigned long grenze)
{
	uint8_t *data;
	uint16_t *data16;
	unsigned short width, height;
	unsigned long length;
	uint8_t depth;
	
	depth = smurf_pic->depth;
	
	if (depth != 16)
	{
		if (depth == 24)
		{
			data = smurf_pic->pic_data;
			width = smurf_pic->pic_width;
			height = smurf_pic->pic_height;
			length = (unsigned long) width * (unsigned long) height;
		} else
		{
			data = smurf_pic->palette;
			length = 256;
		}
		do
		{
			if (((unsigned long) *data * 872L + (unsigned long) *(data + 1) * 2930L + (unsigned long) *(data + 2) * 296L) < grenze)
			{
				*data++ = 0x0;
				*data++ = 0x0;
				*data++ = 0x0;
			} else
			{
				*data++ = 0xff;
				*data++ = 0xff;
				*data++ = 0xff;
			}
		} while (--length != 0);
	} else if (depth == 16)
	{
		data16 = (uint16_t *)smurf_pic->pic_data;
		width = smurf_pic->pic_width;
		height = smurf_pic->pic_height;
		length = (unsigned long) width * (unsigned long) height;
		do
		{
			if (((unsigned long) ((*data16 & 0xf800) >> 8) * 872L + (unsigned long) ((*data16 & 0x7e0u) >> 3) * 2930L + (unsigned long) ((*data16 & 0x1fu) << 3) * 296L) < grenze)
			{
				*data16++ = 0;
			} else
			{
				*data16++ = 0xffff;
			}
		} while (--length != 0);
	}
}


static void do_mode1(SMURF_PIC *smurf_pic, unsigned long grenze)
{
	uint8_t *data;
	uint8_t *dest;
	uint8_t *orig_dest;
	uint8_t *linebuf;
	uint8_t *pal;
	uint8_t *ppal;
	uint8_t *src;
	uint8_t depth;
	uint8_t pad;
	uint16_t *data16;
	unsigned short y;
	unsigned short width;
	unsigned short height;
	unsigned short i;
	unsigned long length;
	unsigned long linelen;
	uint8_t bit;
	uint16_t pixval;
	
	data = smurf_pic->pic_data;
	data16 = (uint16_t *)data;
	width = smurf_pic->pic_width;
	height = smurf_pic->pic_height;
	depth = smurf_pic->depth;
	
	linelen = (long)(width + 7) / 8L;
	length = linelen * height;

	orig_dest = dest = SMalloc(length);
	linebuf = SMalloc(width + 7);
	
	pal = smurf_pic->palette;

	pad = (width & 7) == 0 ? 0 : 1;
	y = 0;
	do
	{
		if (depth <= 8)
		{
			if (smurf_pic->format_type == FORM_STANDARD)
			{
				memset(linebuf, 0, width);
				getpix_std_line(data, linebuf, depth, length, width);
				data += linelen;
				src = linebuf;
			} else
			{
				src = data;
				data += width;
			}
			i = 0;
			bit = 7;
			do
			{
				pixval = *src++;
				ppal = pal + pixval + pixval + pixval;
				if (((unsigned long) *ppal++ * 872L + (unsigned long) *ppal++ * 2930L + (unsigned long) *ppal * 296L) < grenze)
					*dest |= 1 << bit;
				if (bit-- == 0)
				{
					bit = 7;
					dest++;
				}
			} while (++i < width);
		} else if (depth == 24)
		{
			i = 0;
			bit = 7;
			do
			{
				if (((unsigned long) *data++ * 872L + (unsigned long) *data++ * 2930L + (unsigned long) *data++ * 296L) < grenze)
					*dest |= 1 << bit;
				if (bit-- == 0)
				{
					bit = 7;
					dest++;
				}
			} while (++i < width);
			
		} else
		{
			i = 0;
			bit = 7;
			do
			{
				pixval = *data16++;
				if (((unsigned long) ((pixval & 0xf800) >> 8) * 872L + (unsigned long) ((pixval & 0x7e0u) >> 3) * 2930L + (unsigned long) ((pixval & 0x1fu) << 3) * 296L) < grenze)
					*dest |= 1 << bit;
				if (bit-- == 0)
				{
					bit = 7;
					dest++;
				}
			} while (++i < width);
		}
		dest += pad;
	} while (++y < height);
	
	dest = orig_dest;
	pal[0] = 255;
	pal[1] = 255;
	pal[2] = 255;
	pal[3] = 0;
	pal[4] = 0;
	pal[5] = 0;
	SMfree(linebuf);
	SMfree(smurf_pic->pic_data);
	smurf_pic->pic_data = dest;
	smurf_pic->format_type = FORM_STANDARD;
	smurf_pic->depth = 1;
}
