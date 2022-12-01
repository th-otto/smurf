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

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Enhanced Simplex (ESM)			*/
/*	  1, 8 und 24 Bit, unkomprimiert				*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"

static void *(*SMalloc)(long amount);
static void (*SMfree)(void *ptr);

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"ESM Enhanced Simplex",			/* Name of module */
	0x0020,							/* version */
	"Christian Eyrich",				/* author */
	/* 10 Extensionen fr Importer */
	{ "ESM", "", "", "", "", "", "", "", "", "" },
	/* 4 Slider titles: max length 8 */
	"Slider 1", "Slider 2", "Slider 3", "Slider 4",
	/* 4 checkbox titles */
	"Checkbox 1", "Checkbox 2", "Checkbox 3", "Checkbox 4",
	/* 4 edit object titles */
	"Edit 1", "Edit 2", "Edit 3", "Edit 4",
	/* min/max values for slider */
	0,128, 0,128, 0,128, 0,128,
	/* min/max values for edit objects */
	0,10, 0,10, 0,10, 0,10,
	/* default values for slider */
	0, 0, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for edit objects */
	0, 0, 0, 0,
	/* how many pics? */
	0,
	/* description for pictures */
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	/* supported bit depth */
	1, 8, 24, 0, 0, 0, 0, 0,
	/* data formats */
	FORM_STANDARD,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	/* ext_flag */
	0
};

struct esm_header {
	uint32_t magic;
	uint16_t data_offset;
	uint16_t width;
	uint16_t height;
	uint16_t bits_per_pixel;
	uint16_t components;
	uint16_t depth_red;
	uint16_t depth_green;
	uint16_t depth_blue;
	uint16_t depth_black; /* ? unknown, usually 0 */
	uint16_t version;
	uint16_t hdpi;
	uint16_t vdpi;
	uint16_t line_height;
	uint16_t line_start;
	uint16_t line_end;
	uint16_t mask;
};


static void make_esm_header(struct esm_header *esm, uint16_t width, uint16_t height, uint8_t depth, uint8_t col_format)
{
	esm->magic = 0x544d5300L; /* 'TMS' */
	esm->data_offset = sizeof(struct esm_header);
	if (depth == 8 && col_format != GREY)
		esm->data_offset += 3 * 256;
	esm->width = width;
	esm->height = height;
	esm->bits_per_pixel = depth;
	switch (depth)
	{
	case 1:
		esm->components = 1;
		break;
	case 8:
	case 24:
		esm->components = 3;
		break;
	}
	esm->depth_red = 8;
	esm->depth_green = 8;
	esm->depth_blue = 8;
	esm->depth_black = 0;
	esm->version = 4;
	esm->hdpi = 100;
	esm->vdpi = 100;
	esm->line_height = height;
	esm->line_start = 0;
	esm->line_end = height - 1;
	esm->mask = 0;
}


EXPORT_PIC *exp_module_main(GARGAMEL *smurf_struct)
{
	EXPORT_PIC *exp_pic;
	uint8_t *pic_data;
	uint8_t *dst;
	uint16_t width;
	uint16_t height;
	uint8_t depth;
	uint8_t *buffer_start;
	unsigned long DataOffset;
	unsigned long linesize;
	unsigned long len;
	uint16_t palsize;
	uint8_t *buffer;
	uint8_t *pal;
	uint8_t *ppal;
	uint16_t y;
	
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
		break;

		/* Farbsystem wird vom Smurf erfragt */
	case MCOLSYS:
		if (smurf_struct->smurf_pic->depth == 8)
		{
			if (smurf_struct->smurf_pic->col_format == GREY)
				smurf_struct->event_par[0] = GREY;
			else
				smurf_struct->event_par[0] = RGB;
		} else
		{
			smurf_struct->event_par[0] = RGB;
		}
		smurf_struct->module_mode = M_COLSYS;
		break;

	case MEXEC:
		SMalloc = smurf_struct->services->SMalloc;
		SMfree = smurf_struct->services->SMfree;
		DataOffset = sizeof(struct esm_header);
		pic_data = smurf_struct->smurf_pic->pic_data;
		dst = pic_data;
		exp_pic = SMalloc(sizeof(*exp_pic));
		width = smurf_struct->smurf_pic->pic_width;
		height = smurf_struct->smurf_pic->pic_height;
		depth = smurf_struct->smurf_pic->depth;
		if (depth == 1)
		{
			linesize = (unsigned long)(width + 7) >> 3;
		} else if (depth == 8)
		{
			linesize = width;
		} else
		{
			linesize = (unsigned long)width * 3;
		}
		len = linesize * height;
		if (depth == 8 && smurf_struct->smurf_pic->col_format != GREY)
			palsize = 3 * 256;
		else
			palsize = 0;
		buffer = SMalloc(palsize + DataOffset + len);
		if (buffer == NULL)
		{
			smurf_struct->module_mode = M_MEMORY;
		} else
		{
			buffer_start = buffer;
			make_esm_header((struct esm_header *)buffer, width, height, depth, smurf_struct->smurf_pic->col_format);
			buffer += DataOffset;
			if (palsize != 0)
			{
				unsigned int i;

				ppal = smurf_struct->smurf_pic->palette;
				pal = buffer;
				for (i = 0; i < SM_PALETTE_MAX; i++)
				{
					pal[0] = *ppal++;
					pal[256] = *ppal++;
					pal[512] = *ppal++;
					pal++;
				}
				buffer += palsize;
			}
			y = 0;
			do
			{
				memcpy(buffer, pic_data, linesize);
				buffer += linesize;
				pic_data += linesize;
			} while (++y < height);
			pic_data = dst;
			buffer = buffer_start;
			exp_pic->pic_data = buffer;
			exp_pic->f_len = palsize + DataOffset + len;
			smurf_struct->module_mode = M_DONEEXIT;
		}
		return exp_pic;
	
	case MTERM:
		SMfree(exp_pic->pic_data); /* BUG: uninitialized */
		SMfree(exp_pic);
		smurf_struct->module_mode = M_EXIT;
		break;

	default:
		smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
		break;
	}
	return NULL;
}
