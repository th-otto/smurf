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
 * Olaf Piesche, Christian Eyrich, Dale Russell and J�rg Dittmer
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/*  ----------- Bump Mapping-Modul V1.2 --------------  */
/*          F�r SMURF Bildkonverter, 26.04.96           */

#include "country.h"

#if COUNTRY==1
#define TEXT2 "Rot"
#define TEXT3 "Gr�n"
#define TEXT4 "Blau"
#elif COUNTRY==0
#define TEXT2 "Red"
#define TEXT3 "Green"
#define TEXT4 "Blue"
#elif COUNTRY==2
#define TEXT2 "Rouge"
#define TEXT3 "Vert"
#define TEXT4 "Bleu"
#else
#error "Keine Sprache!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "bumpmap6.rsh"


#define Obj_Selected(a) (main_form[a].ob_state & OS_SELECTED)



short do_it(GARGAMEL *smurf_struct);

void f_init_sliders(void);
void f_init_window(short mod_id);
void f_default_sliders(void);

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);

static char p1string[20] = "Bumpmap";

/*------ Infostruktur f�r Hauptprogramm -----*/
MOD_INFO module_info = {
	"Bump Mapping",						/* Name des Moduls */
	0x0120,
	"J�rg Dittmer",						/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen f�r Importer */
/* 4 Slider�berschriften: max 8 */
	"Rot",
	"Gr�n",
	"Blau",
	"St�rke %",
/* 4 Checkbox�berschriften: */
	"",
	"",
	"",
	"",
/* 4 Edit-Objekt-�berschriften: */
	"",
	"",
	"",
	"",
/* min/max-Werte f�r Slider */
	0, 255,
	0, 255,
	0, 255,
	0, 1000,
/* min/max f�r Editobjekte */
	0, 0,
	0, 0,
	0, 0,
	0, 0,
/* Defaultwerte f�r Slider, Check und Edit */
	255, 255, 255, 100,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,										/* Anzahl der Bilder */
	p1string, NULL, NULL, NULL, NULL, NULL	/* Bezeichnung f�r Bilder */
};


/*--------------------- Was kann ich ? ----------------------*/
MOD_ABILITY module_ability = {
	24, 0, 0, 0, 0, 0, 0, 0,			/* Farbtiefen */
	/* Grafikmodi: */
	FORM_PIXELPAK,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	FORM_BOTH,
	0									/* Extra Flag */
};


uint8_t red, green, blue;
uint8_t gred, ggreen, gblue;
uint8_t diffuse_strengh, glanz_strengh;
uint8_t diffuse_hard, glanz_hard;
short bm_strengh;
short kreuz_x, kreuz_y;


OBJECT *main_form;

WINDOW *my_window;

SLIDER red_slider;
SLIDER green_slider;
SLIDER blue_slider;
SLIDER m_str_slider;
SLIDER m_hard_slider;
SLIDER bm_str_slider;


void (*set_slider)(SLIDER *sliderstruct, long value);	/* Funktion deklarieren */

/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short (*slider)(SLIDER *slider_struct);	/* Funktion deklarieren */
	static short module_id;
	short SmurfMessage;
	short t;
	short object;

#if 0
	WORD mousex, mousey;
#endif

	SmurfMessage = smurf_struct->module_mode;

	slider = smurf_struct->services->slider;
	set_slider = smurf_struct->services->set_slider;

	main_form = rs_trindex[BUMPMAIN];

	/* Wenn das Modul aufgerufen wurde, */
	if (SmurfMessage == MSTART)
	{

		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(&rs_object[t], 0);

		module_id = smurf_struct->module_number;

		my_window = (WINDOW *)Malloc(sizeof(WINDOW));

		f_init_sliders();
		f_init_window(module_id);

		smurf_struct->wind_struct = my_window;

		smurf_struct->services->f_module_window(my_window);

		f_default_sliders();

		smurf_struct->module_mode = M_WAITING;
		return;
	}


	else if (SmurfMessage == MBEVT)
	{
		object = smurf_struct->event_par[0];
#if 0
		mousex = smurf_struct->mousex;
		mousey = smurf_struct->mousey;
#endif

		switch (object)
		{
		case DO_IT:
			smurf_struct->module_mode = M_STARTED;
			return;

		case M_RED_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				red = slider(&red_slider);
			else
				gred = slider(&red_slider);
			break;

		case M_GREEN_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				green = slider(&green_slider);
			else
				ggreen = slider(&green_slider);
			break;

		case M_BLUE_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				blue = slider(&blue_slider);
			else
				gblue = slider(&blue_slider);
			break;
		case M_BRIGHTNESS_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				diffuse_strengh = slider(&m_str_slider);
			else
				glanz_strengh = slider(&m_str_slider);
			break;
		case M_HARD_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				diffuse_hard = slider(&m_hard_slider);
			else
				glanz_hard = slider(&m_hard_slider);
			break;
		case BM_STRENGTH_SLIDE:
			bm_strengh = slider(&bm_str_slider);
			break;
		case M_RADIO_DIFFUSE:
			set_slider(&red_slider, red);
			set_slider(&green_slider, green);
			set_slider(&blue_slider, blue);
			set_slider(&m_str_slider, diffuse_strengh);
			set_slider(&m_hard_slider, diffuse_hard);
			break;
		case M_RADIO_GLANZ:
			set_slider(&red_slider, gred);
			set_slider(&green_slider, ggreen);
			set_slider(&blue_slider, gblue);
			set_slider(&m_str_slider, glanz_strengh);
			set_slider(&m_hard_slider, glanz_hard);
			break;

		}

		smurf_struct->module_mode = M_WAITING;
		return;
	}

/* Smurf fragt: Fadenkreuz? --------------*/
	else if (SmurfMessage == MCROSSHAIR)
	{
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 1;

		smurf_struct->module_mode = M_CROSSHAIR;
	}

/* Smurf �bergibt mir das Fadenkreuz(e) -----------*/
	else if (SmurfMessage == MCH_COORDS)
	{
		kreuz_x = smurf_struct->event_par[0];
		kreuz_y = smurf_struct->event_par[1];
	}


/* Wenn das Modul gestartet wurde */
	else if (SmurfMessage == MEXEC)
	{
		smurf_struct->module_mode = do_it(smurf_struct);
		return;
	}



/* Wenn das Modul sich verpissen soll */
	if (SmurfMessage == MTERM)
	{
		Mfree(my_window);
		smurf_struct->module_mode = M_EXIT;
		return;
	}
}




/*------------------------- Bambm�bbing ---------------------*/
short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	short width, height;
	uint8_t *mainpic;
	uint8_t *greypic;
	uint8_t *coltab;
	uint8_t *c_offset;
	uint8_t *offset;
	uint8_t *noffset;
	uint8_t *y_offset;
	uint8_t *y_goffset;
	uint8_t *g_offset;

	short grey;
	short x, y;
	uint8_t red2, green2, blue2;
	uint8_t gred2, ggreen2, gblue2;
	short nred, ngreen, nblue;
	long n;
	long t;
	long bpl;
	long gbpl;

	long col_size;
	long coltab_size;
	long coltab_size2;
	long glanz_size;
	long glanztab_size;
	signed long glanz_x, glanz_y;
	long bump_fakt;

	signed short x_vekt, y_vekt;
	uint8_t o_height;
	signed long x_dif, y_dif;
	long abstand;

	long bs_diago;


	/*---Bilddaten auslesen----------------------*/
	picture = smurf_struct->smurf_pic;
	width = picture->pic_width;
	height = picture->pic_height;
	mainpic = picture->pic_data;

	bpl = width * 3L;
	gbpl = width;

	bs_diago = sqrt((long) width * width + (long) height * height);

	/*---Slider umrechnen----------------------*/

	col_size = (long) ((100L - diffuse_hard) * 6.0 + 50);
	glanz_size = (long) ((100L - glanz_hard) / 2.04 + 1);

	red2 = (uint8_t) ((long) red * diffuse_strengh / 100);
	green2 = (uint8_t) ((long) green * diffuse_strengh / 100);
	blue2 = (uint8_t) ((long) blue * diffuse_strengh / 100);

	gred2 = (uint8_t) ((long) gred * glanz_strengh / 100);
	ggreen2 = (uint8_t) ((long) ggreen * glanz_strengh / 100);
	gblue2 = (uint8_t) ((long) gblue * glanz_strengh / 100);


	bump_fakt = (long) (bs_diago / 360.0F * (bm_strengh / 80.0F) * 1024.0F);


	/*-----------------------------------------------------*/

	coltab_size = (long) (bs_diago * col_size / 100L);
	coltab_size2 = coltab_size * coltab_size;
	glanztab_size = (long) (bs_diago * glanz_size / 100L);

	if (Obj_Selected(BM_PARALEL))
	{
		glanz_x = (kreuz_x - width / 2);
		glanz_y = (kreuz_y - height / 2);
	} else
	{
		glanz_x = kreuz_x;
		glanz_y = kreuz_y;
	}

	/*---Speicher anfordern-----------------------------------*/
	if ((coltab = (uint8_t *)Malloc((long) coltab_size * 3L)) == NULL)
		return M_MEMORY;
	if ((greypic = (uint8_t *)Malloc((long) width * (long) height)) == NULL)
	{
		Mfree(coltab);
		return M_MEMORY;
	}


	/*---Graubild erzeugen------------------------------------*/

	offset = mainpic;
	noffset = greypic;
	if (Obj_Selected(HW_RADIO_GREY))
	{
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				grey = *(offset++);
				grey += *(offset++);
				grey += *(offset++);
				*(noffset++) = (uint8_t) (grey / 3);
			}
	} else
	{
		if (Obj_Selected(HW_RADIO_GREEN))
			offset++;
		else if (Obj_Selected(HW_RADIO_BLUE))
			offset += 2;

		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				*(noffset++) = *(offset++);
				offset += 2;
			}
	}


	/*---Farbtabelle generieren--------------------------------*/

	/*---Farbverlauf----------------------*/
	c_offset = coltab;
	for (t = coltab_size; t > 0; t--)
	{
		n = t * t;
		*(c_offset++) = (uint8_t) ((long) red2 * (n) / coltab_size2);
		*(c_offset++) = (uint8_t) ((long) green2 * (n) / coltab_size2);
		*(c_offset++) = (uint8_t) ((long) blue2 * (n) / coltab_size2);
	}

	/*---Glanzpunkt-----------------------*/
	c_offset = coltab;
	for (t = 0; t < glanztab_size; t++)
	{
		nred = (short) ((long) (*(c_offset + 0L)) + (long) ((long) gred2 * (glanztab_size - t) / glanztab_size));
		ngreen = (short) ((long) (*(c_offset + 1L)) + (long) ((long) ggreen2 * (glanztab_size - t) / glanztab_size));
		nblue = (short) ((long) (*(c_offset + 2L)) + (long) ((long) gblue2 * (glanztab_size - t) / glanztab_size));

		if (nred > 255)
			nred = 255;
		if (ngreen > 255)
			ngreen = 255;
		if (nblue > 255)
			nblue = 255;

		*(c_offset++) = nred;
		*(c_offset++) = ngreen;
		*(c_offset++) = nblue;
	}

	/*---Bump Mapping--------------------------------------------*/
	for (y = 0; y < height; y++)
	{
		if (!(y % 20))
			smurf_struct->services->busybox((short) (((long) y << 7L) / (long) height));

		y_offset = mainpic + y * bpl;
		y_goffset = greypic + y * gbpl;
		for (x = 0; x < width; x++)
		{
			offset = y_offset + x * 3L;
			g_offset = y_goffset + x;

			o_height = *(g_offset);

			x_vekt = 0;
			y_vekt = 0;

			if (x < (width - 1))
				x_vekt += o_height - *(g_offset + 1);
			if (y < (height - 1))
				y_vekt += o_height - *(g_offset + gbpl);
			if (x > 0)
				x_vekt -= o_height - *(g_offset - 1);
			if (y > 0)
				y_vekt -= o_height - *(g_offset - gbpl);


			x_vekt = (short) (((signed long) x_vekt * bump_fakt) >> 10);
			y_vekt = (short) (((signed long) y_vekt * bump_fakt) >> 10);

			if (Obj_Selected(BM_PARALEL))
			{
				x_dif = (signed long) glanz_x - x_vekt;
				y_dif = (signed long) glanz_y - y_vekt;
			} else
			{
				x_dif = (signed long) glanz_x - (x_vekt + x);
				y_dif = (signed long) glanz_y - (y_vekt + y);
			}


			abstand = sqrt(x_dif * x_dif + y_dif * y_dif);

			if (abstand >= coltab_size)
				abstand = coltab_size - 1;

			c_offset = coltab + abstand * 3L;

			*(offset++) = *(c_offset++);
			*(offset++) = *(c_offset++);
			*(offset++) = *(c_offset++);

		}
	}


	Mfree(coltab);
	Mfree(greypic);

	return M_PICDONE;
}



/*  ------------------- Fensterstruktur init -------------------    */
void f_init_window(short mod_id)
{
	memset(my_window, 0, sizeof(WINDOW));
	my_window->whandlem = 0;
	my_window->module = mod_id;
	my_window->wnum = 1;
	my_window->wx = 100;
	my_window->wy = 50;
	my_window->ww = main_form->ob_width;
	my_window->wh = main_form->ob_height;
	my_window->editob = 0;
	strncpy(my_window->wtitle, "Nohow's Bump Mapper", 40);
	my_window->resource_form = main_form;
	my_window->picture = NULL;
}



/*--------------------- Slider initialisieren -------------------*/
void f_init_sliders(void)
{
	red_slider.regler = M_RED_SLIDE;
	red_slider.schiene = M_RED_FHR;
	red_slider.rtree = main_form;
	red_slider.txt_obj = M_RED_EDIT;
	red_slider.min_val = 0;
	red_slider.max_val = 255;
	red_slider.window = my_window;

	green_slider.regler = M_GREEN_SLIDE;
	green_slider.schiene = M_GREEN_FHR;
	green_slider.rtree = main_form;
	green_slider.txt_obj = M_GREEN_EDIT;
	green_slider.min_val = 0;
	green_slider.max_val = 255;
	green_slider.window = my_window;

	blue_slider.regler = M_BLUE_SLIDE;
	blue_slider.schiene = M_BLUE_FHR;
	blue_slider.rtree = main_form;
	blue_slider.txt_obj = M_BLUE_EDIT;
	blue_slider.min_val = 0;
	blue_slider.max_val = 255;
	blue_slider.window = my_window;

	m_str_slider.regler = M_BRIGHTNESS_SLIDE;
	m_str_slider.schiene = M_BRIGHTNESS_FHR;
	m_str_slider.rtree = main_form;
	m_str_slider.txt_obj = M_BRIGHTNESS_EDIT;
	m_str_slider.min_val = 0;
	m_str_slider.max_val = 100;
	m_str_slider.window = my_window;

	m_hard_slider.regler = M_HARD_SLIDE;
	m_hard_slider.schiene = M_HARD_FHR;
	m_hard_slider.rtree = main_form;
	m_hard_slider.txt_obj = M_HARD_EDIT;
	m_hard_slider.min_val = 0;
	m_hard_slider.max_val = 100;
	m_hard_slider.window = my_window;

	bm_str_slider.regler = BM_STRENGTH_SLIDE;
	bm_str_slider.schiene = BM_STRENGTH_FHR;
	bm_str_slider.rtree = main_form;
	bm_str_slider.txt_obj = BM_STRENGTH_EDIT;
	bm_str_slider.min_val = 0;
	bm_str_slider.max_val = 999;
	bm_str_slider.window = my_window;
}


/*--------------------- Slider auf defaults setzen -------------------*/
void f_default_sliders(void)
{
	red = 255;
	green = 0;
	blue = 0;
	diffuse_strengh = 100;
	diffuse_hard = 80;

	gred = 255;
	ggreen = 255;
	gblue = 255;
	glanz_strengh = 90;
	glanz_hard = 80;

	bm_strengh = 200;

	set_slider(&red_slider, red);
	set_slider(&green_slider, green);
	set_slider(&blue_slider, blue);
	set_slider(&m_str_slider, diffuse_strengh);
	set_slider(&m_hard_slider, diffuse_hard);

	set_slider(&bm_str_slider, bm_strengh);
}




/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel gro�es Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview mu� in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* �bernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul mu� nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierf�r bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* f�r die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
