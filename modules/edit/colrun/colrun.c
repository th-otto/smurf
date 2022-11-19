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

/*  ------------- Farbverlauf-Modul ----------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "import.h"
#include "smurfine.h"
#include "country.h"
#include "colrun.rsh"



#if COUNTRY == 1
#define TEXT1 "Farbverlauf"
#elif COUNTRY == 0
#define TEXT1 "Color gradient"
#else
#define TEXT1 "Color gradient"
#endif


#define TIMER 0

/* Infostruktur fr Hauptmodul */
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

short num_datapoints = 2;
short x137f6 = 0;
short colrun_direction = 90;
short colrun_type = COLRUN_LINEAR; /* FIXME: should not use resource IDs here, because written to config file */

#define NUM_DATA_POINTS 16

typedef struct {
	uint8_t num_datapoints;
	uint8_t colrun_type;
	uint8_t x14224[NUM_DATA_POINTS + 1];
	uint8_t x14235[NUM_DATA_POINTS + 1];
	uint8_t x14246[NUM_DATA_POINTS + 1];
	uint8_t x14257[NUM_DATA_POINTS + 1];
	short colrun_direction;
} CONFIG;


CONFIG *config;
SLIDER red_slider;
SLIDER green_slider;
SLIDER blue_slider;
OBJECT *maintree;
WINDOW *win;
SMURF_PIC *smurf_pic;
SMURF_PIC preview;
uint8_t x14224[NUM_DATA_POINTS + 1];
uint8_t x14235[NUM_DATA_POINTS + 1];
uint8_t x14246[NUM_DATA_POINTS + 1];
uint8_t x14257[NUM_DATA_POINTS + 1];

WORD mx, my;
void (*set_slider)(SLIDER *, long);
void (*redraw_window)(WINDOW *, GRECT *, WORD, WORD);


static SMURF_PIC *compute_preview(SMURF_PIC *preview);
static short x11092(WORD button);
static void x112b4(WORD mousey);
static void do_colrun(GARGAMEL *smurf_struct, SMURF_PIC *pic);
static void x113d6(short x);


void edit_module_main(GARGAMEL *smurf_struct)
{
	short wnum = 1;
	short (*f_module_window)(WINDOW *);
	short (*slider)(SLIDER *);
	char title[] = "Farbverlauf V0.1";
	unsigned long hi;
	unsigned short lo;
	short SmurfMessage;
	WORD i; /* d4 */
	short module_id;
	WORD Button;
	WORD mousex;
	WORD mousey;
	short d7;

	SmurfMessage = smurf_struct->module_mode;
	f_module_window = smurf_struct->services->f_module_window;
	slider = smurf_struct->services->slider;
	set_slider = smurf_struct->services->set_slider;
	redraw_window = smurf_struct->services->redraw_window;
	smurf_pic = smurf_struct->smurf_pic;
	
	if (SmurfMessage == MSTART)
	{
		mx = smurf_pic->pic_width / 2;
		my = smurf_pic->pic_height / 2;
		maintree = rs_trindex[COLRUN_MAIN];

		/* Resource fix ---------------------------------------- */
		for (i = 0; i < NUM_OBS; i++)
			rsrc_obfix(&rs_object[i], 0);

		module_id = smurf_struct->module_number;
		win = smurf_struct->services->SMalloc(sizeof(*win));
		memset(win, 0, sizeof(*win));
		
		/*  ------------------- Fensterstruktur init -------------------    */
		win->whandlem = 0;
		win->module = module_id;
		strcpy(win->wtitle, title);
		win->wnum = wnum;
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
		x14224[0] = x14235[0] = x14246[0] = 0;
		x14224[1] = x14235[1] = x14246[1] = 255;
		x14257[0] = 0;
		x14257[1] = 255;
		
		maintree[DATAPOINT_LAST].ob_y = 255;
		for (i = DATAPOINT_00; i <= DATAPOINT_13; i++)
		{
			maintree[i].ob_flags |= OF_HIDETREE;
			x14257[i - DATAPOINT_00 + 2] = 0;
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
		if ((unsigned short)f_module_window(win) == (unsigned short)-1)
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
		return;
	}
	
	if (SmurfMessage == MBEVT)
	{
		Button = smurf_struct->event_par[0];
		mousex = smurf_struct->mousex;
		mousey = smurf_struct->mousey;
		
		mousex = mousex;
		if (Button == M_RED_SLIDE || Button == M_RED_FHR)
		{
			x14224[x137f6] = slider(&red_slider);
		} else if (Button == M_GREEN_SLIDE || Button == M_GREEN_FHR)
		{
			x14235[x137f6] = slider(&green_slider);
		} else if (Button == M_BLUE_SLIDE || Button == M_BLUE_FHR)
		{
			x14246[x137f6] = slider(&blue_slider);
		} else if (Button >= DATAPOINT_FIRST && Button <= DATAPOINT_13)
		{
			d7 = x11092(Button);
		} else if (Button == DATAPOINT_BOX)
		{
			x112b4(mousey);
		} else if (Button == COLRUN_RADIAL && colrun_type != COLRUN_RADIAL)
		{
			smurf_struct->module_mode = M_CROSSHAIR;
			colrun_type = COLRUN_RADIAL;
		} else if (Button == COLRUN_LINEAR && colrun_type != COLRUN_LINEAR)
		{
			smurf_struct->module_mode = M_CH_OFF;
			colrun_type = COLRUN_LINEAR;
		} else if (Button == START)
		{
			smurf_struct->module_mode = M_STARTED;
		} else
		{
			smurf_struct->module_mode = M_WAITING;
		}
		if (Button == M_RED_SLIDE ||
			Button == M_GREEN_SLIDE ||
			Button == M_BLUE_SLIDE ||
			Button == M_RED_FHR ||
			Button == M_GREEN_FHR ||
			Button == M_BLUE_FHR ||
			(Button >= DATAPOINT_00 && Button <= DATAPOINT_13 && d7 != 0) ||
			Button == DATAPOINT_BOX ||
			Button == PREVIEW)
		{
			compute_preview(&preview);
			win->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
		}
		return;
	}

	if (SmurfMessage == MKEVT)
	{
		if (smurf_struct->event_par[0] == START)
			smurf_struct->module_mode = M_STARTED;
		return;
	}

	if (SmurfMessage == MDITHER_READY)
	{
		redraw_window(win, NULL, PREVIEW, 0);
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	if (smurf_struct->module_mode == MCH_DEFCOO) /* XXX */
	{
		smurf_struct->event_par[0] = mx;
		smurf_struct->event_par[1] = my;
		smurf_struct->module_mode = M_CHDEFCOO;
		return;
	}

	if (smurf_struct->module_mode == MCH_COORDS) /* XXX */
	{
		mx = smurf_struct->event_par[0];
		my = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	if (SmurfMessage == MEXEC)
	{
		colrun_direction = atoi(maintree[COLRUN_DIR].ob_spec.tedinfo->te_ptext);
		do_colrun(smurf_struct, smurf_struct->smurf_pic);
		smurf_struct->module_mode = M_PICDONE;
		return;
	}

	if (SmurfMessage == MTERM)
	{
		smurf_struct->services->SMfree(win);
		smurf_struct->services->SMfree(preview.pic_data);
		smurf_struct->module_mode = M_EXIT;
		return;
	}

	if (SmurfMessage == GETCONFIG)
	{
		colrun_direction = atoi(maintree[COLRUN_DIR].ob_spec.tedinfo->te_ptext);
		config = smurf_struct->services->SMalloc(sizeof(*config));
		config->num_datapoints = num_datapoints;
		config->colrun_type = colrun_type;
		config->colrun_direction = colrun_direction;
		for (i = 0; i < NUM_DATA_POINTS; i++)
		{
			config->x14224[i] = x14224[i];
			config->x14235[i] = x14235[i];
			config->x14246[i] = x14246[i];
			config->x14257[i] = x14257[i];
		}
		smurf_struct->event_par[0] = (WORD)((long)config >> 16);
		smurf_struct->event_par[1] = (WORD)((long)config);
		smurf_struct->event_par[2] = (WORD)sizeof(*config);
		smurf_struct->module_mode = M_CONFIG;
		return;
	}

	if (SmurfMessage == CONFIG_TRANSMIT)
	{
		hi = (unsigned long)smurf_struct->event_par[0] << 16;
		lo = smurf_struct->event_par[1];
		config = (CONFIG *)(hi | lo);
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
			x14224[i] = config->x14224[i];
			x14235[i] = config->x14235[i];
			x14246[i] = config->x14246[i];
			x14257[i] = config->x14257[i];
		}
		for (i = 2; i < NUM_DATA_POINTS; i++)
		{
			maintree[i + DATAPOINT_00 - 2].ob_y = x14257[i];
			if (x14257[i] != 0)
				maintree[i + DATAPOINT_00 - 2].ob_flags &= ~OF_HIDETREE;
		}
		x113d6(22);
		compute_preview(&preview);
		smurf_struct->module_mode = M_MODPIC;
		return;
	}
}


static void do_colrun(GARGAMEL *smurf_struct, SMURF_PIC *pic)
{
	(void)smurf_struct;
	(void)pic;
}


static SMURF_PIC *compute_preview(SMURF_PIC *preview)
{
	return preview;
}


static short x11092(WORD button)
{
	return button;
}


static void x112b4(WORD mousey)
{
	(void)mousey;
}


static void x113d6(short x)
{
	(void)x;
}
