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
#include "../../import.h"
#include "../../../src/smurfine.h"
#include "country.h"
#include "colrun.rsh"

#define COLRUN_MAIN 0

#define START 19
#define GRADIENT 0



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

short x137f4 = 2;
short x137f6 = 0;
short x137f8 = 90;
short x137fa = 5;

OBJECT *maintree;
WINDOW *win;
SMURF_PIC *smurf_pic;
void (*set_slider)(SLIDER *, long);
void (*redraw_window)(WINDOW *, GRECT *, WORD, WORD);

WORD mx, my;


void edit_module_main(GARGAMEL *smurf_struct)
{
	short d6 = 1;
	short (*f_module_window)(WINDOW *);
	short (*slider)(SLIDER *);
	char version[] = "Farbverlauf V0.1";
	short o0;
	short SmurfMessage;
	WORD i;
	short module_id; /* d5 */
	
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
		
		smurf_struct->module_mode = M_MODPIC;
		return;
	}
	
	if (SmurfMessage == MBEVT)
	{
		smurf_struct->module_mode = M_MODPIC;
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
		redraw_window(win, NULL, GRADIENT, 0);
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	if (SmurfMessage == MCH_DEFCOO)
	{
		smurf_struct->event_par[0] = mx;
		smurf_struct->event_par[1] = my;
		smurf_struct->module_mode = M_CHDEFCOO;
		return;
	}

	if (SmurfMessage == MCH_COORDS)
	{
		mx = smurf_struct->event_par[0];
		my = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_WAITING;
		return;
	}

	if (SmurfMessage == MEXEC)
	{
		smurf_struct->module_mode = M_PICDONE;
		return;
	}

	if (SmurfMessage == MTERM)
	{
		smurf_struct->module_mode = M_EXIT;
		return;
	}

	if (SmurfMessage == GETCONFIG)
	{
		smurf_struct->module_mode = M_CONFIG;
		return;
	}

	if (SmurfMessage == CONFIG_TRANSMIT)
	{
		smurf_struct->module_mode = M_MODPIC;
		return;
	}
}
