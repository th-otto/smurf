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
#include "import.h"
#include "smurfine.h"
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
#define MSG1 "[3][Brainstorm JPEG DSP Driver|not found. Run it?|(Note: danger of crash if|DSP is used multiple!)][Run|Cancel]"
#else
#define MSG1 "[3][Brainstorm JPEG DSP Driver|not found. Run it?|(Note: danger of crash if|DSP is used multiple!)][Run|Cancel]"
#endif


static BOOLEAN asked_already;
static unsigned short mfdb_div;
static uint8_t *OutPtr;


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
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


static BOOLEAN get_cookie(unsigned long cookie, unsigned long *value)
{
	unsigned long *cookiejar;

	/* ansonsten value auf 0L */
	*value = 0;

	/* Zeiger auf Cookie Jar holen */
	cookiejar = (unsigned long *) Setexc(0x5a0 / 4, (void (*)()) -1);
	/* Cookie Jar installiert? */
	if (cookiejar)
	{
		/* Keksdose nach cookie durchsuchen */
		while (*cookiejar)
		{
			if (*cookiejar++ == cookie)
			{
				/* wenn cookie gefunden wurde, value auf Cookiewert setzen, */
				*value = *cookiejar;
				return TRUE;
			}
			cookiejar++;
		}
	}
	return FALSE;
}


static long jpg_create(JPGD_STRUCT *jpgd)
{
	(void)jpgd;
	return NOERROR;
}


static long jpg_close(JPGD_STRUCT *jpgd)
{
	(void)jpgd;
	return NOERROR;
}


static long jpg_user(JPGD_STRUCT *jpgd)
{
	(void)jpgd;
	return NOERROR;
}


static void jpg_sigterm(JPGD_STRUCT *jpgd)
{
	(void)jpgd;
}


#ifndef __PUREC__
static long jpg_write(JPGD_STRUCT *_jpgd)
#else
static long jpg_write(JPGD_STRUCT *jpgd)
#endif
{
	uint16_t words;
	uint16_t count;
	uint8_t *src;
	uint8_t *dst;
	uint16_t height;

#ifdef __GNUC__
	/* fetch the JPGD_STRUCT pointer from a0 */
	JPGD_STRUCT *jpgd;
	
	(void)_jpgd;
	__asm__ __volatile__(
		"\tmove.l %%a0,%0\n"
		: "=r"(jpgd)
		:
		: );
#endif

	words = jpgd->MFDBWordSize / mfdb_div;
	height = jpgd->OutTmpHeight;
	if (words != 0 && height != 0)
	{
		src = jpgd->OutTmpPointer;
		dst = OutPtr;
		do
		{
			count = words;
			do
			{
				/* copy 8 pixels */
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
				*dst++ = *src++;
			} while (--count != 0);
		} while (--height != 0);
		OutPtr = dst;
	}
		
	return NOERROR;
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		JPEG File Interchange Format (JPG)			*/
/*		8 und 24 Bit								*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *picbuf;
	unsigned long snd;
	JPGD_STRUCT *jpgd;
	long struct_size;
	JPGDDRV_STRUCT *jpgd_driver;
	uint16_t pixel_width;
	uint16_t pixel_wdwidth;
	uint16_t pixel_height;

	picbuf = smurf_struct->smurf_pic->pic_data;
#if 0
	if (picbuf[6] != 'J' ||
		picbuf[7] != 'F' ||
		picbuf[8] != 'I' ||
		picbuf[9] != 'F')
		return M_INVALID;
#endif
	
	/* DSP present? */
	if (!get_cookie(0x5F534E44L, &snd) || (snd & 0x08) == 0)
		return M_INVALID;
	
	asked_already = FALSE;
	for (;;)
	{
		if (!get_cookie(JPGD_MAGIC, (unsigned long *)&jpgd_driver))
		{
#if START_DRIVER
			if (!asked_already)
			{
				asked_already = TRUE;
				if (form_alert(1, MSG1) == 1)
					shel_write(1, 1, 0, "JPEGD.PRG", "");
				continue;
			}
#endif
			return M_INVALID;
		}
		break;
	}
	
	struct_size = JPGDGetStructSize(jpgd_driver);
	jpgd = (JPGD_STRUCT *)Malloc(struct_size);
	if (jpgd == NULL)
	{
		return M_MEMORY;
	}
	memset(jpgd, 0, struct_size);

	if (JPGDOpenDriver(jpgd_driver, jpgd) != NOERROR)
	{
		Mfree(jpgd);
		return M_INVALID;
	}

	jpgd->InPointer = picbuf;
	jpgd->InSize = smurf_struct->smurf_pic->file_len;
	jpgd->OutPointer = NULL;
	
	mfdb_div = (8 * 3) / 2;
	jpgd->OutComponents = 3;
	jpgd->OutPixelSize = 3;
	jpgd->OutFlag = -1;
	jpgd->Create = jpg_create;
	jpgd->Write = jpg_write;
	jpgd->Close = jpg_close;
	jpgd->SigTerm = jpg_sigterm;
	jpgd->UserRoutine = jpg_user;
	
	if (JPGDGetImageInfo(jpgd_driver, jpgd) != NOERROR)
	{
		JPGDCloseDriver(jpgd_driver, jpgd);
		Mfree(jpgd);
		return M_INVALID;
	}

	strcpy(smurf_struct->smurf_pic->format_name, "JFIF Format");
	smurf_struct->services->reset_busybox(67, "DSP Brainstorming");
	
	if (JPGDGetImageSize(jpgd_driver, jpgd) != NOERROR)
	{
		JPGDCloseDriver(jpgd_driver, jpgd);
		Mfree(jpgd);
		return M_INVALID;
	}
	pixel_width = jpgd->MFDBPixelWidth;
	pixel_wdwidth = (pixel_width + 15) & -16;
	pixel_height = jpgd->MFDBPixelHeight;
	if ((jpgd->OutPointer = (void *)Malloc(jpgd->OutSize)) == NULL)
	{
		JPGDCloseDriver(jpgd_driver, jpgd);
		return M_MEMORY;
	}
	OutPtr = jpgd->OutPointer;
	if (JPGDDecodeImage(jpgd_driver, jpgd) != NOERROR)
	{
		Mfree(jpgd->OutPointer);
		JPGDCloseDriver(jpgd_driver, jpgd);
		Mfree(jpgd);
		return M_INVALID;
	}

	Mfree(picbuf);
	picbuf = jpgd->OutPointer;
	JPGDCloseDriver(jpgd_driver, jpgd);
	Mfree(jpgd);

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
		src = picbuf;
		dst = src;
		for (y = 0; y < pixel_height; y++)
		{
			memmove(dst, src, line_width);
			src += line_wdwidth;
			dst += line_width;
		}
		_Mshrink(picbuf, line_width * pixel_height);
	}
	
	smurf_struct->smurf_pic->pic_width = pixel_width;
	smurf_struct->smurf_pic->pic_height = pixel_height;
	smurf_struct->smurf_pic->depth = 24;
	smurf_struct->smurf_pic->col_format = RGB;
	smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	smurf_struct->smurf_pic->pic_data = picbuf;
	
	return M_PICDONE;
}
