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
 * Olaf Piesche, Christian Eyrich, Dale Russell and Jîrg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*  ------------- Farbverlauf-Modul ----------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "country.h"
#include "colrun.rsh"

#if COUNTRY == 1
#define TEXT1  "Farbverlauf"
#define TITLE  "Farbverlauf V1.0"
#define ALERT1 "[1][Es kînnen nur 16 StÅtzpunkte|gesetzt werden!][ Ok ]"
#elif COUNTRY == 0
#define TEXT1 "Color gradient"
#define TITLE  "CoColor gradient V1.0"
#define ALERT1 "[1][You can only define 16|interpolation points!][ Ok ]"
#else
#define TEXT1 "Color gradient"
#define TITLE  "CoColor gradient V1.0"
#define ALERT1 "[1][You can only define 16|interpolation points!][ Ok ]"
#endif


#define TIMER 0

/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	TEXT1,
	0x0100,
	"Olaf Piesche",
	/* Extensionen */
	{ "", "", "", "", "", "", "", "", "", "" },
	/* Slider */
	"Slider 1",
	"Slider 2",
	"Slider 3",
	"Slider 4",
	/* Checkboxen */
	"Checkbox 1",
	"Checkbox 2",
	"Checkbox 3",
	"Checkbox 4",
	/* Editfelder */
	"Edit 1",
	"Edit 2",
	"Edit 3",
	"Edit 4",

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
	1,
	/* description for pictures */
	"", "", "", "", "", ""
};

/*--------------------- Was kann ich ? ----------------------*/
MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,    /* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	0 /* Extra-Flag */ 
};

static short num_datapoints = 2;
static short cur_datapoint = 0;
static short colrun_direction = 90;
static short colrun_type = COLRUN_LINEAR; /* FIXME: should not use resource IDs here, because written to config file */

#define NUM_DATA_POINTS 16

typedef struct {
	uint8_t num_datapoints;
	uint8_t colrun_type;
	uint8_t datapoint_red[NUM_DATA_POINTS + 1];
	uint8_t datapoint_green[NUM_DATA_POINTS + 1];
	uint8_t datapoint_blue[NUM_DATA_POINTS + 1];
	uint8_t datapoint_pos[NUM_DATA_POINTS + 1];
	short colrun_direction;
} CONFIG;


static CONFIG *config;
static SLIDER red_slider;
static SLIDER green_slider;
static SLIDER blue_slider;
static OBJECT *maintree;
static WINDOW *win;
static SMURF_PIC *smurf_pic;
static SMURF_PIC preview;
static uint8_t datapoint_red[NUM_DATA_POINTS + 1];
static uint8_t datapoint_green[NUM_DATA_POINTS + 1];
static uint8_t datapoint_blue[NUM_DATA_POINTS + 1];
static uint8_t datapoint_pos[NUM_DATA_POINTS + 1];

static WORD mx, my;
static void (*set_slider)(SLIDER *, long);
static void (*redraw_window)(WINDOW *, GRECT *, WORD, WORD);






static void select_datapoint(WORD button)
{
	WORD i;
	
	for (i = DATAPOINT_FIRST; i < DATAPOINT_13; i++)
		maintree[i].ob_state &= ~OS_SELECTED;
	maintree[button].ob_state |= OS_SELECTED;
	redraw_window(win, NULL, DATAPOINT_BOX, 0);
	cur_datapoint = button - DATAPOINT_FIRST;
	set_slider(&red_slider, datapoint_red[cur_datapoint]);
	set_slider(&green_slider, datapoint_green[cur_datapoint]);
	set_slider(&blue_slider, datapoint_blue[cur_datapoint]);
}


static void add_datapoint(WORD mousey)
{
	WORD boxx;
	WORD boxy;

	if (num_datapoints == NUM_DATA_POINTS)
	{
		form_alert(1, ALERT1);
	} else
	{
		objc_offset(maintree, DATAPOINT_BOX, &boxx, &boxy);
		datapoint_pos[num_datapoints] = mousey - boxy;
		maintree[num_datapoints + DATAPOINT_FIRST].ob_y = mousey - boxy;
		maintree[num_datapoints + DATAPOINT_FIRST].ob_flags &= ~OF_HIDETREE;
		select_datapoint(num_datapoints + DATAPOINT_FIRST);
		num_datapoints++;
	}
}


static void del_datapoint(short datapoint)
{
	while (datapoint <= num_datapoints)
	{
		datapoint_pos[datapoint] = datapoint_pos[datapoint + 1];
		datapoint_red[datapoint] = datapoint_red[datapoint + 1];
		datapoint_green[datapoint] = datapoint_green[datapoint + 1];
		datapoint_blue[datapoint] = datapoint_blue[datapoint + 1];
		maintree[datapoint + DATAPOINT_FIRST].ob_y = maintree[datapoint + DATAPOINT_FIRST + 1].ob_y;
		++datapoint;
	}
	datapoint_pos[num_datapoints - 1] = 0;
	datapoint_red[num_datapoints - 1] = 0;
	datapoint_green[num_datapoints - 1] = 0;
	datapoint_blue[num_datapoints - 1] = 0;
	maintree[num_datapoints + DATAPOINT_FIRST - 1].ob_flags |= OF_HIDETREE;
	num_datapoints--;
	select_datapoint(DATAPOINT_FIRST);
}


static BOOLEAN move_datapoint(WORD button)
{
	WORD dummy;
	WORD bstate;
	WORD boxx;
	WORD boxy;
	WORD buttonx;
	WORD buttony;
	WORD buttonw;
	WORD buttonh;
	WORD mox;
	WORD moy;
	GRECT gr;
	WORD y;
	WORD buttony_offset;
	WORD orig_buttony_offset;
	WORD ydiff;
	WORD val;
	short i;

	select_datapoint(button);
	objc_offset(maintree, DATAPOINT_BOX, &boxx, &boxy);
	objc_offset(maintree, button, &buttonx, &buttony);
	buttonw = maintree[button].ob_width;
	buttonh = maintree[button].ob_height;
	buttony_offset = maintree[button].ob_y;
	orig_buttony_offset = buttony_offset;
	if (button == DATAPOINT_FIRST || button == DATAPOINT_LAST)
	{
		do
		{
			graf_mkstate(&dummy, &dummy, &bstate, &dummy);
		} while (bstate != 0);
	} else
	{
		graf_mkstate(&mox, &moy, &bstate, &dummy);
		if (bstate != 0)
		{
			ydiff = moy - buttony;
			y = moy;
			graf_mouse(POINT_HAND, NULL);
			do
			{
				do
				{
					graf_mkstate(&mox, &moy, &bstate, &dummy);
				} while (moy == y && bstate != 0);
				maintree[button].ob_y = moy - boxy - ydiff;
				if (maintree[button].ob_y < 0)
					maintree[button].ob_y = 0;
				else if (maintree[button].ob_y > 255)
					maintree[button].ob_y = 255;
				redraw_window(win, NULL, button, 0);
				gr.g_x = buttonx - 1;
				gr.g_w = buttonw + 2;
				gr.g_y = buttony_offset + boxy - 5;
				gr.g_h = buttonh + 10;
				redraw_window(win, &gr, DATAPOINT_BOX, 0);
				y = moy;
				buttony_offset = maintree[button].ob_y;
				datapoint_pos[button - DATAPOINT_FIRST] = maintree[button].ob_y;
			} while (bstate != 0);
			graf_mouse(ARROW, NULL);
			redraw_window(win, NULL, DATAPOINT_BOX, 0);
			val = datapoint_pos[button - DATAPOINT_FIRST];
			if (val == 0 || val == 255)
			{
				maintree[button].ob_y = orig_buttony_offset;
				datapoint_pos[button - DATAPOINT_FIRST] = orig_buttony_offset;
				del_datapoint(button - DATAPOINT_FIRST);
			} else
			{
				for (i = 0; i < num_datapoints; i++)
				{
					if (i != button - DATAPOINT_FIRST && val == datapoint_pos[i])
					{
						maintree[button].ob_y = orig_buttony_offset;
						datapoint_pos[button - DATAPOINT_FIRST] = orig_buttony_offset;
						del_datapoint(button - DATAPOINT_FIRST);
					}
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}


static void compute_preview(SMURF_PIC *preview)
{
	uint8_t *picdata;
	short ystart;
	short yend;
	short order[NUM_DATA_POINTS + 1];
	short pointnum;
	short startpoint;
	short endpoint;
	long redval;
	long greenval;
	long blueval;
	long redinc;
	long greeninc;
	long blueinc;

	picdata = preview->pic_data;
	{
		short i, j;

		for (i = 0; i < NUM_DATA_POINTS; i++)
			order[i] = i;
		for (i = 0; i < num_datapoints - 1; i++)
		{
			for (j = 0; j < num_datapoints - 1; j++)
			{
				if (datapoint_pos[order[j + 1]] < datapoint_pos[order[j]])
				{
					short tmp = order[j];
					order[j] = order[j + 1];
					order[j + 1] = tmp;
				}
			}
		}
	}

	for (pointnum = 0; pointnum < num_datapoints - 1; pointnum++)
	{
		short i, y;

		startpoint = order[pointnum];
		endpoint = order[pointnum + 1];
		redval = (long)datapoint_red[startpoint] << 10;
		greenval = (long)datapoint_green[startpoint] << 10;
		blueval = (long)datapoint_blue[startpoint] << 10;
		ystart = datapoint_pos[startpoint];
		yend = datapoint_pos[endpoint];
		redinc = (((long)datapoint_red[endpoint] << 10) - redval) / (yend - ystart);
		greeninc = (((long)datapoint_green[endpoint] << 10) - greenval) / (yend - ystart);
		blueinc = (((long)datapoint_blue[endpoint] << 10) - blueval) / (yend - ystart);
		for (y = ystart; y < yend; y++)
		{
			for (i = 0; i < NUM_DATA_POINTS; i++)
			{
				picdata[0] = redval >> 10;
				picdata[1] = greenval >> 10;
				picdata[2] = blueval >> 10;
				picdata += 3;
			}
			redval += redinc;
			greenval += greeninc;
			blueval += blueinc;
		}
	}
}


static void do_colrun(GARGAMEL *smurf_struct, SMURF_PIC *pic, short direction)
{
	double o122;
	double o112;
	short y;
	short ystart;
	short yend;
	uint8_t *buffer;
	short order[NUM_DATA_POINTS + 1];
	short pointnum;
	short startpoint;
	short endpoint;
	long redval;
	long greenval;
	long blueval;
	long redinc;
	long greeninc;
	long blueinc;
	long xdir;
	long ydir;
	long ypos;
	long xpos;
	long lineoffset;
	long xposstart;
	uint8_t *picdata;
	long distance;
	long topleft_distance;
	long topright_distance;
	long xdiff;
	long ydiff;
	WORD width;
	WORD height;
	short i;
	uint8_t *ptr;
	short x;
	long dstoffset;
	long srcoffset;
	long srcx;
	long xstart;
	
	picdata = pic->pic_data;
	width = pic->pic_width;
	height = pic->pic_height;
	for (i = 0; i < NUM_DATA_POINTS; i++)
		order[i] = i;
	for (i = 0; i < num_datapoints - 1; i++)
	{
		short j;
		for (j = 0; j < num_datapoints - 1; j++)
		{
			if (datapoint_pos[order[j + 1]] < datapoint_pos[order[j]])
			{
				short tmp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = tmp;
			}
		}
	}

	if (maintree[COLRUN_LINEAR].ob_state & OS_SELECTED)
	{
		o112 = fabs(cos((direction * M_PI) / 180.0));
		o122 = fabs(sin((direction * M_PI) / 180.0));
		distance = pic->pic_width * o112 + pic->pic_height * o122;
	} else
	{
		long bottomleft_distance; /* d5 */
		long bottomright_distance; /* d1 */

		xdiff = (long)(0 - mx) * (long)(0 - mx);
		ydiff = (long)(0 - my) * (long)(0 - my);
		o112 = xdiff + ydiff;
		o122 = sqrt(o112);
		topleft_distance = o122;
		ydiff = (long)(height - my) * (long)(height - my);
		o112 = xdiff + ydiff;
		o122 = sqrt(o112);
		bottomleft_distance = o122;
		
		xdiff = (long)(width - mx) * (long)(width - mx);
		ydiff = (long)(0 - my) * (long)(0 - my);
		o112 = xdiff + ydiff;
		o122 = sqrt(o112);
		topright_distance = o122;
		ydiff = (long)(height - my) * (long)(height - my);
		o112 = xdiff + ydiff;
		o122 = sqrt(o112);
		bottomright_distance = o122;
		
		distance = topleft_distance;
		if (bottomleft_distance > distance)
			distance = bottomleft_distance;
		if (topright_distance > distance)
			distance = topright_distance;
		if (bottomright_distance > distance)
			distance = bottomright_distance;
	}
	
	o122 = 256.0 / distance;
	ptr = smurf_struct->services->SMalloc(distance * 3 + 2048);
	buffer = ptr;
	for (i = 0; i < (distance / 2) * 3; i += 3)
	{
		ptr[i + 0] = datapoint_red[0];
		ptr[i + 1] = datapoint_green[0];
		ptr[i + 2] = datapoint_blue[0];
	}
	for (i = (distance / 2) * 3; i < distance * 3 + 1024; i += 3)
	{
		ptr[i + 0] = datapoint_red[num_datapoints - 1];
		ptr[i + 1] = datapoint_green[num_datapoints - 1];
		ptr[i + 2] = datapoint_blue[num_datapoints - 1];
	}
	ptr += 1024;
	for (pointnum = 0; pointnum < num_datapoints - 1; pointnum++)
	{
		startpoint = order[pointnum];
		endpoint = order[pointnum + 1];
		redval = (long)datapoint_red[startpoint] << 10;
		greenval = (long)datapoint_green[startpoint] << 10;
		blueval = (long)datapoint_blue[startpoint] << 10;
		ystart = datapoint_pos[startpoint] / o122;
		yend = datapoint_pos[endpoint] / o122;
		redinc = (((long)datapoint_red[endpoint] << 10) - redval) / (yend - ystart);
		greeninc = (((long)datapoint_green[endpoint] << 10) - greenval) / (yend - ystart);
		blueinc = (((long)datapoint_blue[endpoint] << 10) - blueval) / (yend - ystart);
		for (y = ystart; y < yend; y++)
		{
			ptr[0] = redval >> 10;
			ptr[1] = greenval >> 10;
			ptr[2] = blueval >> 10;
			ptr += 3;
			redval += redinc;
			greenval += greeninc;
			blueval += blueinc;
		}
	}
	
	ptr = buffer + (distance / 2) * 3 + 1024;
	xdir = cos((direction * M_PI) / 180.0) * 1024.0;
	ydir = sin((direction * M_PI) / 180.0) * 1024.0;
	ypos = xpos = 0;
	xposstart = (-xdir * pic->pic_width) >> 1;
	ypos = (-ydir * pic->pic_height) >> 1;
	srcx = (ypos >> 10) * 3;
	if (maintree[COLRUN_LINEAR].ob_state & OS_SELECTED)
	{
		for (y = 0; y < pic->pic_height; y++)
		{
			if ((y & 31) == 0)
				smurf_struct->services->busybox((short)(((long)y << 7) / pic->pic_height));
			xpos = xposstart;
			lineoffset = (long)y * pic->pic_width * 3;
			for (x = 0; x < pic->pic_width; x++)
			{
				dstoffset = (long)x * 3 + lineoffset;
				srcoffset = (xpos >> 10) * 3 + srcx;
				picdata[dstoffset + 0] = ptr[srcoffset + 0];
				picdata[dstoffset + 1] = ptr[srcoffset + 1];
				picdata[dstoffset + 2] = ptr[srcoffset + 2];
				xpos += xdir;
			}
			ypos += ydir;
			srcx = (ypos >> 10) * 3;
		}
	} else
	{
		ptr = buffer + 1024;
		for (y = 0; y < pic->pic_height; y++)
		{
			if ((y & 15) == 0)
				smurf_struct->services->busybox((short)(((long)y << 7) / pic->pic_height));
			lineoffset = (long)y * pic->pic_width * 3;
			ydiff = (long)(y - my) * (long)(y - my);
			xstart = mx;
			for (x = 0; x < pic->pic_width; x++)
			{
				xstart--;
				xdiff = xstart * xstart;
				srcoffset = (long)sqrt(xdiff + ydiff) * 3;
				dstoffset = x * 3L + lineoffset;
				picdata[dstoffset + 0] = ptr[srcoffset + 0];
				picdata[dstoffset + 1] = ptr[srcoffset + 1];
				picdata[dstoffset + 2] = ptr[srcoffset + 2];
			}
		}
	}

	smurf_struct->services->SMfree(buffer);
}


void edit_module_main(GARGAMEL *smurf_struct)
{
	short (*f_module_window)(WINDOW *);
	short (*slider)(SLIDER *);
	WORD i;
	WORD Button;
	WORD mousey;
	BOOLEAN changed;

	f_module_window = smurf_struct->services->f_module_window;
	slider = smurf_struct->services->slider;
	set_slider = smurf_struct->services->set_slider;
	redraw_window = smurf_struct->services->redraw_window;
	smurf_pic = smurf_struct->smurf_pic;
	
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		mx = smurf_pic->pic_width / 2;
		my = smurf_pic->pic_height / 2;
		maintree = rs_trindex[COLRUN_MAIN];

		/* Resource fix ---------------------------------------- */
		for (i = 0; i < NUM_OBS; i++)
			rsrc_obfix(&rs_object[i], 0);

		win = smurf_struct->services->SMalloc(sizeof(*win));
		memset(win, 0, sizeof(*win));
		
		/*  ------------------- Fensterstruktur init -------------------    */
		win->whandlem = 0;
		win->module = smurf_struct->module_number;
		strcpy(win->wtitle, TITLE);
		win->wnum = 1;
		win->wx = -1;
		win->wy = -1;
		win->ww = maintree[ROOT].ob_width;
		win->wh = maintree[ROOT].ob_height;
		win->resource_form = maintree;
		win->picture = NULL;
		win->pic_xpos = maintree[PREVIEW].ob_x;
		win->pic_ypos = maintree[PREVIEW].ob_y + 1;
		win->clipwid = 15;
		win->cliphgt = 256;
		win->editob = COLRUN_DIR;
		win->nextedit = COLRUN_DIR;
		win->editx = 0;
		smurf_struct->wind_struct = win;
		
		/*--------------------- Slider initialisieren -------------------*/
		red_slider.regler = M_RED_SLIDE;
		red_slider.schiene = M_RED_FHR;
		red_slider.rtree = maintree;
		red_slider.txt_obj = M_RED_EDIT;
		red_slider.min_val = 0;
		red_slider.max_val = 255;
		red_slider.window = win;
		
		green_slider.regler = M_GREEN_SLIDE;
		green_slider.schiene = M_GREEN_FHR;
		green_slider.rtree = maintree;
		green_slider.txt_obj = M_GREEN_EDIT;
		green_slider.min_val = 0;
		green_slider.max_val = 255;
		green_slider.window = win;
		
		blue_slider.regler = M_BLUE_SLIDE;
		blue_slider.schiene = M_BLUE_FHR;
		blue_slider.rtree = maintree;
		blue_slider.txt_obj = M_BLUE_EDIT;
		blue_slider.min_val = 0;
		blue_slider.max_val = 255;
		blue_slider.window = win;
		
		/*--------------------- -------------------*/
		datapoint_red[0] = datapoint_green[0] = datapoint_blue[0] = 0;
		datapoint_red[1] = datapoint_green[1] = datapoint_blue[1] = 255;
		datapoint_pos[0] = 0;
		datapoint_pos[1] = 255;
		
		maintree[DATAPOINT_LAST].ob_y = 255;
		for (i = DATAPOINT_00; i <= DATAPOINT_13; i++)
		{
			maintree[i].ob_flags |= OF_HIDETREE;
			datapoint_pos[i - DATAPOINT_00 + 2] = 0;
		}
		
		preview.pic_width = 16;
		preview.pic_height = 256;
		preview.depth = 24;
		preview.pic_data = smurf_struct->services->SMalloc(16 * (256 + 4) * 3L);
		preview.screen_pic = NULL;
		preview.changed = 0;
		preview.format_type = FORM_PIXELPAK;
		preview.col_format = RGB;
		preview.zoom = 0;
		preview.block = NULL;
		preview.local_nct = NULL;
		preview.prev_picture = NULL;
		preview.next_picture = NULL;
		memset(preview.pic_data, 0, 16 * 256 * 3L);
		if (f_module_window(win) == -1)
		{
			smurf_struct->module_mode = M_EXIT;
			smurf_struct->services->SMfree(win);
		} else
		{
			smurf_struct->module_mode = M_WAITING;
			set_slider(&red_slider, 0);
			set_slider(&green_slider, 0);
			set_slider(&blue_slider, 0);
			compute_preview(&preview);
			win->picture = &preview;
			redraw_window(win, NULL, ROOT, 0);
			smurf_struct->module_mode = M_MODPIC;
		}
		break;
	
	case MBEVT:
		Button = smurf_struct->event_par[0];
		mousey = smurf_struct->mousey;
		
		switch (Button)
		{
		case M_RED_SLIDE:
		case M_RED_FHR:
			datapoint_red[cur_datapoint] = slider(&red_slider);
			changed = TRUE;
			break;
		case M_GREEN_SLIDE:
		case M_GREEN_FHR:
			datapoint_green[cur_datapoint] = slider(&green_slider);
			changed = TRUE;
			break;
		case M_BLUE_SLIDE:
		case M_BLUE_FHR:
			datapoint_blue[cur_datapoint] = slider(&blue_slider);
			changed = TRUE;
			break;
		case DATAPOINT_FIRST:
		case DATAPOINT_LAST:
		case DATAPOINT_00:
		case DATAPOINT_01:
		case DATAPOINT_02:
		case DATAPOINT_03:
		case DATAPOINT_04:
		case DATAPOINT_05:
		case DATAPOINT_06:
		case DATAPOINT_07:
		case DATAPOINT_08:
		case DATAPOINT_09:
		case DATAPOINT_10:
		case DATAPOINT_11:
		case DATAPOINT_12:
		case DATAPOINT_13:
			changed = move_datapoint(Button);
			break;
		case DATAPOINT_BOX:
			add_datapoint(mousey);
			changed = TRUE;
			break;
		case COLRUN_RADIAL:
			if (colrun_type != COLRUN_RADIAL)
			{
				smurf_struct->module_mode = M_CROSSHAIR;
				colrun_type = COLRUN_RADIAL;
				changed = TRUE;
			}
			break;
		case COLRUN_LINEAR:
			if (colrun_type != COLRUN_LINEAR)
			{
				smurf_struct->module_mode = M_CH_OFF;
				colrun_type = COLRUN_LINEAR;
				changed = TRUE;
			}
			break;
		case STARTIT:
			smurf_struct->module_mode = M_STARTED;
			break;
		case PREVIEW:
			changed = TRUE;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		if (changed)
		{
			compute_preview(&preview);
			win->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
		}
		break;

	case MKEVT:
		if (smurf_struct->event_par[0] == STARTIT)
			smurf_struct->module_mode = M_STARTED;
		break;

	case MDITHER_READY:
		redraw_window(win, NULL, PREVIEW, 0);
		smurf_struct->module_mode = M_WAITING;
		break;

	case MCH_DEFCOO:
		smurf_struct->event_par[0] = mx;
		smurf_struct->event_par[1] = my;
		smurf_struct->module_mode = M_CHDEFCOO;
		break;

	case MCH_COORDS:
		mx = smurf_struct->event_par[0];
		my = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		break;

	case MEXEC:
		colrun_direction = atoi(maintree[COLRUN_DIR].ob_spec.tedinfo->te_ptext);
		do_colrun(smurf_struct, smurf_struct->smurf_pic, colrun_direction);
		smurf_struct->module_mode = M_PICDONE;
		break;

	case MTERM:
		smurf_struct->services->SMfree(win);
		smurf_struct->services->SMfree(preview.pic_data);
		smurf_struct->module_mode = M_EXIT;
		break;

	case GETCONFIG:
		colrun_direction = atoi(maintree[COLRUN_DIR].ob_spec.tedinfo->te_ptext);
		config = smurf_struct->services->SMalloc(sizeof(*config));
		config->num_datapoints = num_datapoints;
		config->colrun_type = colrun_type;
		config->colrun_direction = colrun_direction;
		for (i = 0; i < NUM_DATA_POINTS; i++)
		{
			config->datapoint_red[i] = datapoint_red[i];
			config->datapoint_green[i] = datapoint_green[i];
			config->datapoint_blue[i] = datapoint_blue[i];
			config->datapoint_pos[i] = datapoint_pos[i];
		}
		*((CONFIG **)&smurf_struct->event_par[0]) = config;
		smurf_struct->event_par[2] = (WORD)sizeof(*config);
		smurf_struct->module_mode = M_CONFIG;
		break;

	case CONFIG_TRANSMIT:
		config = *((CONFIG **)&smurf_struct->event_par[0]);
		num_datapoints = config->num_datapoints;
		colrun_type = config->colrun_type;
		colrun_direction = config->colrun_direction;
		maintree[COLRUN_RADIAL].ob_state &= ~OS_SELECTED;
		maintree[COLRUN_LINEAR].ob_state &= ~OS_SELECTED;
		maintree[colrun_type].ob_state |= OS_SELECTED;
		itoa(colrun_direction, maintree[COLRUN_DIR].ob_spec.tedinfo->te_ptext, 10);
		redraw_window(win, NULL, COLRUN_DIR, 0);
		for (i = 0; i < NUM_DATA_POINTS - 2; i++) /* why -2 here? */
		{
			datapoint_red[i] = config->datapoint_red[i];
			datapoint_green[i] = config->datapoint_green[i];
			datapoint_blue[i] = config->datapoint_blue[i];
			datapoint_pos[i] = config->datapoint_pos[i];
		}
		for (i = 2; i < NUM_DATA_POINTS; i++)
		{
			maintree[i + DATAPOINT_00 - 2].ob_y = datapoint_pos[i];
			if (datapoint_pos[i] != 0)
				maintree[i + DATAPOINT_00 - 2].ob_flags &= ~OF_HIDETREE;
		}
		select_datapoint(DATAPOINT_FIRST);
		compute_preview(&preview);
		smurf_struct->module_mode = M_MODPIC;
		break;
	}
}
