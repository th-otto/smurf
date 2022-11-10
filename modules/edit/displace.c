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

#include "country.h"

#if COUNTRY==1
#define TEXT1 "Displacement"
#define TEXT2 "X-Scale"
#define TEXT3 "Y-Scale"
#define TEXT4 "Destination"
#define TEXT5 "Displacment-Map"
#elif COUNTRY==0
#define TEXT1 "Displacement"
#define TEXT2 "X-Scale"
#define TEXT3 "Y-Scale"
#define TEXT4 "Destination"
#define TEXT5 "Displacment-Map"
#elif COUNTRY==2
#define TEXT1 "Displacement"
#define TEXT2 "X-Scale"
#define TEXT3 "Y-Scale"
#define TEXT4 "Destination"
#define TEXT5 "Displacment-Map"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../import.h"
#include "../../src/smurfine.h"

#define DEST_PIC    0
#define BUMP_PIC    1



MOD_INFO module_info = {
	TEXT1,                                  /* Name of module */
	0x0100,									/* version */
	"Olaf Piesche",							/* author */
	/* 10 Extensionen fr Importer */
	{ " ", " ", " ", " ", " ", " ", " ", " ", " ", " " },
	/* 4 Slider titles: max length 8 */
	TEXT2, TEXT3, "", "",
	/* 4 checkbox titles */
	"", "", "", "",
	/* 4 edit object titles */
	"", "",	"", "",
	/* min/max values for slider */
	1,100, 1,100, 0,0, 0,0,
	/* min/max values for edit objects */
	0,0, 0,0, 0,0, 0,0,
	/* default values for slider */
	10, 10, 0, 0,
	/* default values for checkboxes */
	0, 0, 0, 0,
	/* default values for editobjects */
	0, 0, 0, 0,
	/* how many pics? */
	2,
	/* description for pictures */
	TEXT4,
	TEXT5,
	"",
	"",
	"",
	""
};


MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

static SMURF_PIC *picture;
static SMURF_PIC *bump_pic;

static short doit(GARGAMEL *smurf_struct);

void edit_module_main(GARGAMEL *smurf_struct)
{
	short SmurfMessage;
	static short module_id;

	SmurfMessage = smurf_struct->module_mode;
	if (SmurfMessage == MSTART)
	{
		module_id = smurf_struct->module_number;
		smurf_struct->services->f_module_prefs(&module_info, module_id);
		smurf_struct->module_mode = M_WAITING;
		return;
	}
	
	if (SmurfMessage == MPICS)
	{
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;
			smurf_struct->module_mode = M_PICTURE;
			break;
		
		case BUMP_PIC:
			smurf_struct->event_par[0] = 8;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = GREY;
			smurf_struct->module_mode = M_PICTURE;
			break;
		
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		return;
	}

	if (SmurfMessage == MPICTURE)
	{
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			picture = smurf_struct->smurf_pic;
			break;
		case BUMP_PIC:
			bump_pic = smurf_struct->smurf_pic;
			break;
		}
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	if (SmurfMessage == MEXEC)
	{
		smurf_struct->module_mode = doit(smurf_struct);
		smurf_struct->event_par[0] = 0;
		return;
	}
	
	if (SmurfMessage == MTERM)
	{
		smurf_struct->module_mode = M_EXIT;
		return;
	}

	smurf_struct->module_mode = M_WAITING;
}


static short doit(GARGAMEL *smurf_struct)
{
	long width;
	long height;
	long x;
	long y;
	WORD dy;
	WORD dx;
	WORD pixel;
	long bump_width;
	long bump_height;
	long dst_offset;
	long src_offset;
	uint8_t *picdata;
	uint8_t *bumpdata;
	uint8_t *buffer;
	
	dst_offset = 0;	
	width = picture->pic_width;
	height = picture->pic_height;
	picdata = picture->pic_data;
	bump_width = bump_pic->pic_width;
	bump_height = bump_pic->pic_height;
	bumpdata = bump_pic->pic_data;
	smurf_struct->slide1 = ((long)(short)smurf_struct->slide1 << 8) / 100;
	smurf_struct->slide2 = ((long)(short)smurf_struct->slide2 << 8) / 100;
	buffer = smurf_struct->services->SMalloc(width * height * 3);
	if (buffer == NULL)
		return M_MEMORY;
	memset(buffer, 0, width * height * 3); /* FIXME: SMalloc already does this */
	
	smurf_struct->services->reset_busybox(0, "Displacement");
	for (y = 0; y < height; y++)
	{
		if ((y & 15) == 0)
			smurf_struct->services->busybox((int) ((y << 7) / height));
		dy = (WORD) y;
		if (y > (bump_height - 1))
			dy = (WORD)(y % (bump_height - 1));
		src_offset = dy * bump_width;
		for (x = 0; x < width; x++)
		{
			long xscale;
			long yscale;

			dx = (WORD)x;
			if ((bump_width - 1) < x)
				dx = (WORD)(x % (bump_width - 1));
			pixel = *(bumpdata + dx + src_offset) - 128;
			xscale = ((pixel * smurf_struct->slide1) >> 8) + x;
			yscale = ((pixel * smurf_struct->slide2) >> 8) + y;
			if (xscale < 0)
				xscale = (xscale % width) + width;
			else if (xscale > width) /* FIXME: should be >= */
				xscale = xscale % width;
			if (yscale < 0)
				yscale = (yscale % height) + height;
			else if (yscale > height) /* FIXME: should be >= */
				yscale = yscale % height;
			*(buffer + x + x + x + dst_offset + 0) = picdata[yscale * (width + width + width) + (xscale + xscale + xscale) + 0];
			*(buffer + x + x + x + dst_offset + 1) = picdata[yscale * (width + width + width) + (xscale + xscale + xscale) + 1];
			*(buffer + x + x + x + dst_offset + 2) = picdata[yscale * (width + width + width) + (xscale + xscale + xscale) + 2];
		}
		dst_offset += width * 3;
	}
	
	smurf_struct->services->SMfree(picdata);
	picture->pic_data = buffer;
	
	return M_PICDONE;
}
