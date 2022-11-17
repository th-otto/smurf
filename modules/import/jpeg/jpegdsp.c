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
 * The Initial Developer of the Original Code are
 * Olaf Piesche, Christian Eyrich, Dale Russell and J”rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* =========================================================*/
/*			JPEG File Interchange Format Decoder			*/
/* DSP version                                              */
/* =========================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../import.h"
#include "../../../src/smurfine.h"
#include "country.h"

#undef LOCAL
#undef GLOBAL
#undef INLINE
#undef EXTERN

#include "jpeglib.h"
#include <setjmp.h>
#include "jpgdh.h"

#define TIMER 0
#define START_DRIVER 0

#if COUNTRY==1
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#elif COUNTRY==0
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#else
#define MSG1 "[3][Brainstorm JPEG DSP Driver|nicht gefunden. Nachstarten?|(Vorsicht Crashgefahr wenn|DSP mehrfach benutzt!)][Starten|Abbruch]"
#endif

extern JPGDDRV_STRUCT *jpgd_driver ASM_NAME("jpgd_driver");
extern uint16_t pixel_width ASM_NAME("pixel_width");
extern uint16_t pixel_wdwidth ASM_NAME("pixel_wdwidth");
extern uint16_t pixel_height ASM_NAME("pixel_height");
extern unsigned long image_size ASM_NAME("image_size");
extern uint8_t *pic_data ASM_NAME("pic_data");
short jpg_convert(uint8_t *pic_data, unsigned long filesize) ASM_NAME("jpg_convert");

JPGD_STRUCT jpgd_struct ASM_NAME("jpgd_struct");
char unsued[5182]; /* reserved for decoder */
uint8_t *InPointer ASM_NAME("InPointer");
unsigned long InSize ASM_NAME("InSize");


/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"JFIF Format",
	0x0130,
	"Bj\224rn Spruck",
	/* Extensionen */
	{ "JPG", "JPEG", "JFIF", "", "", "", "", "", "", "" },
	/* Slider */
	"",
	"",
	"",
	"",
	/* Checkboxen */
	"",
	"",
	"",
	"",
	/* Editfelder */
	"",
	"",
	"",
	"",

	/* Minima + Maxima */
	/* Slider */
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	/* Edits */
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	/* Defaults */
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};


#if START_DRIVER
void start_driver(void) ASM_NAME("start_driver");

void start_driver(void)
{
	if (form_alert(1, MSG1) == 1)
		shel_write(1, 1, 0, "JPEGD.PRG", "");
}
#endif

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		JPEG File Interchange Format (JPG)			*/
/*		8 und 24 Bit								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *picbuf;
	
	picbuf = smurf_struct->smurf_pic->pic_data;
	if (picbuf[6] != 'J' ||
		picbuf[7] != 'F' ||
		picbuf[8] != 'I' ||
		picbuf[9] != 'F')
		return M_INVALID;
	
	strcpy(smurf_struct->smurf_pic->format_name, "JFIF Format");
	smurf_struct->services->reset_busybox(67, "DSP Brainstorming");
	
	if (jpg_convert(picbuf, smurf_struct->smurf_pic->file_len) == 0)
		return M_INVALID;
	
	if (pic_data == NULL)
	{
		if (image_size != 0)
			return M_MEMORY;
		return M_INVALID;
	}
	
	Mfree(picbuf);
	if (pixel_wdwidth != pixel_width)
	{
		uint8_t *dst;
		uint8_t *src;
		unsigned long line_wdwidth;
		unsigned long line_width;
		uint16_t y;
		
		/*
		 * The DSP decoder creates output images with a multiple of 16 pixel.
		 * shrink that to the real size, as expected by other routines
		 */
		line_wdwidth = (unsigned long)pixel_wdwidth * 3;
		line_width = (unsigned long)pixel_width * 3;
		src = pic_data;
		dst = pic_data;
		for (y = 0; y < pixel_height; y++)
		{
			memmove(dst, src, line_width);
			src += line_wdwidth;
			dst += line_width;
		}
		_Mshrink(pic_data, (long) line_width * pixel_height);
	}
	
	smurf_struct->smurf_pic->pic_width = pixel_width;
	smurf_struct->smurf_pic->pic_height = pixel_height;
	smurf_struct->smurf_pic->depth = 24;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = pic_data;
	
	return M_PICDONE;
}
