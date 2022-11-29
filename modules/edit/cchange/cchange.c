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
/*						Colorchange-Modul					*/
/* Version 0.1  --  18.05.98								*/
/*	  24 Bit												*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include "demolib.h"

#include "cchange.rsh"

#include "country.h"

#if COUNTRY == 1
#  define TEXT1 "Farbe „ndern"
#elif COUNTRY == 0
#  define TEXT1 "Colorchange"
#else
#  define TEXT1 "Colorchange"
#endif

#define TIMER 0

typedef struct
{
	unsigned short index;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} PIXEL;

static void (*redraw_window)(WINDOW *window, GRECT *mwind, WORD startob, WORD flags);

MOD_INFO module_info = {
	TEXT1,
	0x0010,
	"Christian Eyrich",
	{ "", "", "", "", "", "", "", "", "", "" },
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",
	0, 64,
	0, 64,
	0, 64,
	0, 64,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	NULL, NULL, NULL, NULL, NULL, NULL
};


MOD_ABILITY module_ability = {
	2, 4, 7, 8, 16,	24, 0, 0,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	0,
};

static WINDOW window;


static void do_24Bit(SMURF_PIC *smurf_pic, PIXEL *dst, PIXEL * src)
{
	uint8_t *data;
	uint8_t dr, dg, db;
	uint8_t sr, sg, sb;
	unsigned short x, y, width, height;

	width = smurf_pic->pic_width;
	height = smurf_pic->pic_height;
	data = smurf_pic->pic_data;

	dr = dst->r;
	dg = dst->g;
	db = dst->b;

	sr = src->r;
	sg = src->g;
	sb = src->b;

	y = 0;
	do
	{
		x = 0;
		do
		{
			if (*data == dr && *(data + 1) == dg && *(data + 2) == db)
			{
				*data++ = sr;
				*data++ = sg;
				*data++ = sb;
			} else
				data += 3;
		} while (++x < width);
	} while (++y < height);
}


static void do_16Bit(SMURF_PIC * smurf_pic, PIXEL * dst, PIXEL * src)
{
	uint16_t *data16;
	unsigned short x, y;
	unsigned short width, height;
	unsigned short dest, source;

	width = smurf_pic->pic_width;
	height = smurf_pic->pic_height;
	data16 = (uint16_t *) smurf_pic->pic_data;

	dest = dst->index;

	source = src->index;

	y = 0;
	do
	{
		x = 0;
		do
		{
			if (*data16 == dest)
				*data16++ = source;
			else
				data16++;
		} while (++x < width);
	} while (++y < height);
}


static void do_pal(SMURF_PIC *smurf_pic, PIXEL *dst, PIXEL *src)
{
	uint8_t *data;
	uint8_t *pixbuf = NULL;
	uint8_t *line;
	uint8_t BitsPerPixel;
	uint8_t dindex;
	uint8_t sindex;
	unsigned short x, y;
	unsigned short width;
	unsigned short height;
	long planelength;

	width = smurf_pic->pic_width;
	height = smurf_pic->pic_height;
	BitsPerPixel = smurf_pic->depth;
	data = smurf_pic->pic_data;

	planelength = (unsigned long) (width + 7) / 8 * (unsigned long) height;

	dindex = dst->index;

	sindex = src->index;

	if (smurf_pic->format_type == FORM_STANDARD)
		pixbuf = (uint8_t *)Malloc(width + 7);

	y = 0;
	do
	{
		if (smurf_pic->format_type == FORM_STANDARD)
		{
			getpix_std_line(data, pixbuf, BitsPerPixel, planelength, width);
			line = pixbuf;
		} else
		{
			line = data;
		}

		x = 0;
		do
		{
			if (*line == dindex)
				*line++ = sindex;
			else
				line++;
		} while (++x < width);

		if (smurf_pic->format_type == FORM_STANDARD)
			data += setpix_std_line(pixbuf, data, BitsPerPixel, planelength, width);
		else
			data += width;
	} while (++y < height);

	Mfree(pixbuf);
}


static void evaluate_color(SMURF_PIC *smurf_pic, PIXEL *dst, PIXEL *src, unsigned short sx, unsigned short sy, unsigned short dx, unsigned short dy, short type)
{
	uint8_t *data;
	uint8_t *pal;
	uint8_t *palette;
	uint16_t *data16;
	unsigned short width;
	long pos;

	width = smurf_pic->pic_width;
	data = smurf_pic->pic_data;
	data16 = (uint16_t *) data;

	if (smurf_pic->depth == 24)
	{
		pos = (unsigned long) sy * (unsigned long) width * 3L + (unsigned long) sx * 3L;

		src->r = *(data + pos);
		src->g = *(data + pos + 1);
		src->b = *(data + pos + 2);

		pos = (unsigned long) dy * (unsigned long) width * 3L + (unsigned long) dx * 3L;

		dst->r = *(data + pos);
		dst->g = *(data + pos + 1);
		dst->b = *(data + pos + 2);
	} else if (smurf_pic->depth == 16)
	{
		pos = (unsigned long) sy * (unsigned long) width * 2L + (unsigned long) sx * 2L;

		src->index = *(data16 + pos);
		src->r = (src->index & 0xf800) >> 8;
		src->g = (src->index & 0x7e0) >> 3;
		src->b = (src->index & 0x1f) << 3;

		pos = (unsigned long) dy * (unsigned long) width * 2L + (unsigned long) dx * 2L;

		dst->index = *(data16 + pos);
		dst->r = (dst->index & 0xf800) >> 8;
		dst->g = (dst->index & 0x7e0) >> 3;
		dst->b = (dst->index & 0x1f) << 3;
	} else if (smurf_pic->depth <= 8)
	{
		palette = smurf_pic->palette;

		pos = (unsigned long) sy * (unsigned long) width + (unsigned long) sx;

		src->index = *(data + pos);
		pal = palette + src->index * 3L;
		src->r = *pal++;
		src->g = *pal++;
		src->b = *pal++;

		pos = (unsigned long) dy * (unsigned long) width + (unsigned long) dx;

		src->index = *(data + pos);
		pal = palette + dst->index * 3L;
		dst->r = *pal++;
		dst->g = *pal++;
		dst->b = *pal++;
	}

	if (type == SRC)
	{
		itoa(dst->r, rs_object[EDIT_SR].ob_spec.tedinfo->te_ptext, 10);
		itoa(dst->g, rs_object[EDIT_SG].ob_spec.tedinfo->te_ptext, 10);
		itoa(dst->b, rs_object[EDIT_SB].ob_spec.tedinfo->te_ptext, 10);
		redraw_window(&window, NULL, SRAHMEN, 0);
	} else
	{
		itoa(src->r, rs_object[EDIT_DR].ob_spec.tedinfo->te_ptext, 10);
		itoa(src->g, rs_object[EDIT_DG].ob_spec.tedinfo->te_ptext, 10);
		itoa(src->b, rs_object[EDIT_DB].ob_spec.tedinfo->te_ptext, 10);
		redraw_window(&window, NULL, DRAHMEN, 0);
	}
}


/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*					Colorchange						*/
/*	Žndert eine per Fadenkreuz angegebene Farbe in	*/
/*	eine andere ebenfalls per Fadenkreuz anzugebende*/
/*	Farbe.											*/
/*		1-8, 16 und 24 Bit 							*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL * smurf_struct)
{
	uint8_t t;
	uint8_t BitsPerPixel;
	static uint8_t  Color;
	WORD Button;
	static unsigned short dx, dy, sx, sy;
	static PIXEL dst, src;
	OBJECT *win_form;


#if TIMER
	/* wie schnell sind wir? */
	init_timer();
#endif

	switch (smurf_struct->module_mode)
	{
		/* Wenn das Modul zum ersten Mal gestartet wurde */
	case MSTART:
		win_form = rs_trindex[COL_CHANGE];	/* Resourcebaum holen */

		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(&rs_object[t], 0);

		redraw_window = smurf_struct->services->redraw_window;	/* Redrawfunktion */

		window.whandlem = 0;			/* evtl. Handle l”schen */
		window.module = smurf_struct->module_number;	/* ID in die Fensterstruktur eintragen  */
		window.wnum = 1;				/* Fenster nummer 1...  */
		window.wx = -1;					/* Fenster X- ...       */
		window.wy = -1;					/* ... und Y-Pos        */
		window.ww = win_form->ob_width;	/* Fensterbreite        */
		window.wh = win_form->ob_height;	/* Fensterh”he          */
		strcpy(window.wtitle, module_info.mod_name);		/* Titel reinkopieren   */
		window.resource_form = win_form;	/* Resource             */
		window.picture = NULL;			/* kein Bild.           */
		window.editob = 0;				/* erstes Editobjekt    */
		window.nextedit = 0;			/* n„chstes Editobjekt  */
		window.editx = 0;

		smurf_struct->wind_struct = &window;	/* und die Fensterstruktur in die Gargamel */

		/* Defaults bestcken */
		Color = DST;
		sx = smurf_struct->smurf_pic->pic_width / 2;
		sy = smurf_struct->smurf_pic->pic_height / 2;

		dx = smurf_struct->smurf_pic->pic_width / 2;
		dy = smurf_struct->smurf_pic->pic_height / 2;

		if (smurf_struct->services->f_module_window(&window) == -1)	/* Gib mir 'n Fenster! */
			smurf_struct->module_mode = M_EXIT;	/* keins mehr da? */
		else
			smurf_struct->module_mode = M_WAITING;	/* doch? Ich warte... */

		break;

		/* Buttonevent */
	case MBEVT:
		Button = smurf_struct->event_par[0];

		if (Button == DST || Button == SRC)
		{
			evaluate_color(smurf_struct->smurf_pic, &dst, &src, sx, sy, dx, dy, Button);
			Color = Button;
			smurf_struct->module_mode = M_WAITING;
		} else if (Button == STARTIT)
		{
			smurf_struct->module_mode = M_STARTED;
		}
		break;

		/* Keyboardevent */
	case MKEVT:
		Button = smurf_struct->event_par[0];

		if (Button == STARTIT)
			smurf_struct->module_mode = M_STARTED;

		break;

		/*---- Smurf fragt: Fadenkreuz? */
	case MCROSSHAIR:
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 0;
		smurf_struct->module_mode = M_CROSSHAIR;

		break;

		/*--- Smurf fragt: und die Start-Koordinaten? */
	case MCH_DEFCOO:
		if (Color == DST)
		{
			smurf_struct->event_par[0] = dx;
			smurf_struct->event_par[1] = dy;
		} else
		{
			smurf_struct->event_par[0] = sx;
			smurf_struct->event_par[1] = sy;
		}

		smurf_struct->module_mode = M_CHDEFCOO;

		break;

		/*--- Smurf sagt: hier sind die eingestellten Koordinaten */
	case MCH_COORDS:
		if (Color == DST)
		{
			dx = smurf_struct->event_par[0];
			dy = smurf_struct->event_par[1];
		} else
		{
			sx = smurf_struct->event_par[0];
			sy = smurf_struct->event_par[1];
		}

		smurf_struct->module_mode = M_WAITING;

		break;

	case MEXEC:
		BitsPerPixel = smurf_struct->smurf_pic->depth;

		if (BitsPerPixel == 24)
			do_24Bit(smurf_struct->smurf_pic, &dst, &src);
		else if (BitsPerPixel == 16)
			do_16Bit(smurf_struct->smurf_pic, &dst, &src);
		else if (BitsPerPixel <= 8)
			do_pal(smurf_struct->smurf_pic, &dst, &src);

#if TIMER
		/* wie schnell waren wir? */
		printf("%lu\n", get_timer());
		(void) Cnecin();
#endif

		smurf_struct->module_mode = M_DONEEXIT;

		break;

		/* Mterm empfangen - Speicher freigeben und beenden */
	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
