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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../import.h"
#include "../../src/smurfine.h"

/* info structure for main module */
MOD_INFO module_info = {
	"Macintosh Giffer-Format",	/* Name of module */
	0x0030,						/* version */
	"Christian Eyrich",			/* author */
	/* 10 Extensionen fuer Importer */
	{ "QDV", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* 4 checkbox titles */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* 4 edit object titles */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",
	/* min/max values for slider */
	0,128, 0,128, 0,128, 0,128,
	/* min/max values for slider */
	0,10, 0,10, 0,10, 0,10,
	/* default values for slider */
	0, 0, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for editobjects */
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

struct header {
	uint16_t width;
	uint16_t height;
	uint8_t colors;
	uint8_t data[];
};

static short SwitchOrient(uint8_t *est, uint16_t width, uint16_t height, uint8_t depth, uint8_t o31);

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*				GEM Icon-Format (ICN)				*/
/*		1, 2, 4, 8 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
short imp_module_main(GARGAMEL *smurf_struct)
{
	uint8_t *pic_data;
	uint16_t width;
	uint16_t height;
	uint8_t colors;
	uint8_t depth;
	void *pixbuf;
	uint8_t *pal;
	uint8_t *palbuf;
	uint8_t planes;
	char help_str[3];
	char impmessage[22];
	uint16_t y;
	long dataoffset;
	uint8_t *dest;
	long planelength;
	uint8_t *linebuf;
	
	SMalloc = smurf_struct->services->SMalloc;
	SMfree = smurf_struct->services->SMfree;

	pic_data = smurf_struct->smurf_pic->pic_data;
	if (smurf_struct->smurf_pic->file_len - 3 - 5 != *((uint16_t *)pic_data + 0) * *((uint16_t *)pic_data + 1) + pic_data[4] * 3)
		return M_INVALID;

	colors = pic_data[4] + 1;
	if (colors == 2)
	{
		depth = 1;
		planes = 1;
	} else if (colors == 16)
	{
		depth = 4;
		planes = 4;
	} else
	{
		depth = 8;
		planes = 1;
	}
	width = *((uint16_t *)pic_data + 0);
	height = *((uint16_t *)pic_data + 1);
	dataoffset = colors * 3 + 5;
	strncpy(smurf_struct->smurf_pic->format_name, "Macintosh Giffer .QDV", 21);
	smurf_struct->smurf_pic->pic_width = width;
	smurf_struct->smurf_pic->pic_height = height;
	smurf_struct->smurf_pic->depth = depth;
	strcpy(impmessage, "Mac Giffer ");
	strcat(impmessage, itoa(depth, help_str, 10));
	strcat(impmessage, " Bit");
	smurf_struct->services->reset_busybox(128, impmessage);
	
	if (depth != 4)
	{
		dest = pic_data;
		planelength = ((long) width * height * depth) >> 3;
		memmove(dest, &pic_data[dataoffset], planelength);
		_Mshrink(dest, planelength);
	} else
	{
		uint16_t x;

		planelength = ((long) width * height) >> 3;
		dest = SMalloc(((long) width * height) / 2 + 7);
		if (dest == NULL)
			return M_MEMORY;
		pixbuf = dest;
		linebuf = (uint8_t *)Malloc(width);
		y = 0;
		do
		{
			x = 0;
			do
			{
				/* BUG? pic_data still points to the header, not the data */
				linebuf[x] = *pic_data++;
			} while (++x < width);
			dest += setpix_std_line_TC(linebuf, dest, 4, planelength, width);
		} while (++y < height);
		Mfree(linebuf);
		dest = pixbuf;
		SMfree(pic_data);
	}
	
	SwitchOrient(dest, width, height, depth, planes);
	
	smurf_struct->smurf_pic->pic_data = dest;
	if (depth < 8)
		smurf_struct->smurf_pic->format_type = FORM_STANDARD;
	else
		smurf_struct->smurf_pic->format_type = FORM_PIXELPAK;
	
	pal = smurf_struct->smurf_pic->palette;
	palbuf = pic_data + 5;
	{
		uint16_t x;
		for (x = 0; x < colors; x++)
		{
			*pal++ = *palbuf++;
			*pal++ = *palbuf++;
			*pal++ = *palbuf++;
		}
	}
	smurf_struct->smurf_pic->col_format = RGB;
	
	return M_PICDONE;
}


static short SwitchOrient(uint8_t *dest, uint16_t width, uint16_t height, uint8_t depth, uint8_t planes)
{
	unsigned long linesize;
	unsigned long planesize;
	uint8_t *linebuf;
	uint8_t plane;
	uint8_t *buffer;
	uint8_t *buffer2;
	uint16_t y;
	
	if (depth == 1 || planes == 2 || planes == 4 || planes == 8)
		linesize = ((unsigned long)(width + 7) >> 3) << 3;
	else
		linesize = width;
	linesize = (linesize * depth) >> 3;
	planesize = linesize * height;
	linebuf = (uint8_t *)Malloc(linesize);
	if (linebuf == NULL)
		return -1;
	
	plane = 0;
	do
	{
		buffer = &dest[planesize * plane];
		buffer2 = &buffer[planesize];
		buffer2 -= linesize;
		for (y = height >> 1; y-- != 0; )
		{
			memcpy(linebuf, buffer, linesize);
			memcpy(buffer, buffer2, linesize);
			memcpy(buffer2, linebuf, linesize);
			buffer += linesize;
			buffer2 -= linesize;
		}
	} while (++plane < planes);
	
	Mfree(linebuf);
	
	return 0;
}
