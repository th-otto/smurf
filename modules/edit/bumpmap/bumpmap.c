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

/*  ------------- Bumpmap mit Texture - Modul ------------------   */
/*          FÅr SMURF Bildkonverter, 26.04.96           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"
#include "bumpmap.rsh"

#include "country.h"


#define Obj_Selected(a) (main_form[a].ob_state & OS_SELECTED)

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview);

#define BUMPMAP 0
#define TEXTURE 1

/*------ Infostruktur fÅr Hauptprogramm -----*/
MOD_INFO module_info = {
	"Bump It Up",						/* Name des Moduls */
	0x0121,
	"J. Dittmer/O. Piesche",			/* Autor */
	{ "", "", "", "", "", "", "", "", "", "" },	/* 10 Extensionen fÅr Importer */
/* 4 SliderÅberschriften: max 8 */
	{ "", "", "", "" },
/* 4 CheckboxÅberschriften: */
	{ "", "", "", "" },
/* 4 Edit-Objekt-öberschriften: */
	{ "", "", "", "" },
/* min/max-Werte fÅr Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max fÅr Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fÅr Slider, Check und Edit */
	{ 255, 255, 255, 100 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	2,
	COMPILER_ID,
/* Bezeichnung fÅr Bilder */
	{ "Bumpmap", "Texture", NULL, NULL, NULL, NULL }
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


typedef struct configuration
{
	long version;
	uint8_t red, green, blue;
	uint8_t gred, ggreen, gblue;
	uint8_t diff_strength, gl_strength;
	uint8_t diff_pow, gl_pow;
	short bump_str;
	uint8_t invert;
	uint8_t background;
	uint8_t parallel;
	uint8_t texture;
} BUMP_CONFIG;

static uint8_t red, green, blue;
static uint8_t gred, ggreen, gblue;
static uint8_t diffuse_strengh, glanz_strengh;
static uint8_t diffuse_hard, glanz_hard;
static short bm_strengh;
static short kreuz_x, kreuz_y;

static SMURF_PIC *input_pictures[3];

static OBJECT *main_form;
static WINDOW my_window;
static short module_id;

static SLIDER red_slider;
static SLIDER green_slider;
static SLIDER blue_slider;
static SLIDER m_str_slider;
static SLIDER m_hard_slider;
static SLIDER bm_str_slider;


static SERVICE_FUNCTIONS *services;



/*--------------------- Slider initialisieren -------------------*/
static void f_init_sliders(void)
{
	red_slider.regler = M_RED_SLIDE;
	red_slider.schiene = M_RED_FHR;
	red_slider.rtree = main_form;
	red_slider.txt_obj = M_RED_EDIT;
	red_slider.min_val = 0;
	red_slider.max_val = 255;
	red_slider.window = &my_window;

	green_slider.regler = M_GREEN_SLIDE;
	green_slider.schiene = M_GREEN_FHR;
	green_slider.rtree = main_form;
	green_slider.txt_obj = M_GREEN_EDIT;
	green_slider.min_val = 0;
	green_slider.max_val = 255;
	green_slider.window = &my_window;

	blue_slider.regler = M_BLUE_SLIDE;
	blue_slider.schiene = M_BLUE_FHR;
	blue_slider.rtree = main_form;
	blue_slider.txt_obj = M_BLUE_EDIT;
	blue_slider.min_val = 0;
	blue_slider.max_val = 255;
	blue_slider.window = &my_window;

	m_str_slider.regler = M_BRIGHTNESS_SLIDE;
	m_str_slider.schiene = M_BRIGHTNESS_FHR;
	m_str_slider.rtree = main_form;
	m_str_slider.txt_obj = M_BRIGHTNESS_EDIT;
	m_str_slider.min_val = 0;
	m_str_slider.max_val = 100;
	m_str_slider.window = &my_window;

	m_hard_slider.regler = M_HARD_SLIDE;
	m_hard_slider.schiene = M_HARD_FHR;
	m_hard_slider.rtree = main_form;
	m_hard_slider.txt_obj = M_HARD_EDIT;
	m_hard_slider.min_val = 0;
	m_hard_slider.max_val = 100;
	m_hard_slider.window = &my_window;

	bm_str_slider.regler = BM_STRENGTH_SLIDE;
	bm_str_slider.schiene = BM_STRENGTH_FHR;
	bm_str_slider.rtree = main_form;
	bm_str_slider.txt_obj = BM_STRENGTH_EDIT;
	bm_str_slider.min_val = 0;
	bm_str_slider.max_val = 999;
	bm_str_slider.window = &my_window;
}


/*--------------------- Slider auf defaults setzen -------------------*/
static void f_default_sliders(void)
{
	red = 255;
	green = 255;
	blue = 255;
	diffuse_strengh = 100;
	diffuse_hard = 70;

	gred = 255;
	ggreen = 255;
	gblue = 255;
	glanz_strengh = 100;
	glanz_hard = 80;

	bm_strengh = 200;

	services->set_slider(&red_slider, red);
	services->set_slider(&green_slider, green);
	services->set_slider(&blue_slider, blue);
	services->set_slider(&m_str_slider, diffuse_strengh);
	services->set_slider(&m_hard_slider, diffuse_hard);

	services->set_slider(&bm_str_slider, bm_strengh);
}


/*  ------------------- Fensterstruktur init -------------------    */
static void f_init_window(short mod_id)
{
	my_window.whandlem = 0;
	my_window.module = mod_id;
	my_window.wnum = 1;
	my_window.wx = -1;
	my_window.wy = -1;
	my_window.ww = main_form->ob_width;
	my_window.wh = main_form->ob_height;
	strcpy(my_window.wtitle, "Bump It Up");
	my_window.resource_form = main_form;
	my_window.picture = NULL;
	my_window.pflag = 0;
	my_window.editob = 0;
}




static void write_setting(BUMP_CONFIG *myConfig)
{
	myConfig->version = 0x0100;
	myConfig->red = red;
	myConfig->green = green;
	myConfig->blue = blue;
	myConfig->gred = gred;
	myConfig->ggreen = ggreen;
	myConfig->gblue = gblue;
	myConfig->diff_strength = diffuse_strengh;
	myConfig->gl_strength = glanz_strengh;
	myConfig->diff_pow = diffuse_hard;
	myConfig->gl_pow = glanz_hard;
	myConfig->bump_str = bm_strengh;

	if (main_form[BLACK_BAK].ob_state & OS_SELECTED)
		myConfig->background = BLACK_BAK;
	else if (main_form[WHITE_BAK].ob_state & OS_SELECTED)
		myConfig->background = WHITE_BAK;
	else if (main_form[NORM_BAK].ob_state & OS_SELECTED)
		myConfig->background = NORM_BAK;

	myConfig->parallel = main_form[BM_PARALEL].ob_state;
	myConfig->texture = main_form[BM_TEXTURE].ob_state;
	myConfig->invert = main_form[BM_INVERT].ob_state;
}


static void save_setting(void)
{
	BUMP_CONFIG myConfig;
	char name[33];

	write_setting(&myConfig);

	name[0] = '\0';
	services->mconfSave(&module_info, module_id, &myConfig, sizeof(BUMP_CONFIG), name);

	if (name[0] != '\0')
	{
		strcpy(main_form[LOAD_SET].ob_spec.tedinfo->te_ptext, name);
		services->redraw_window(&my_window, NULL, LOAD_SET, 0);
	}
}


static void apply_setting(BUMP_CONFIG *myConfig)
{
	red = myConfig->red;
	green = myConfig->green;
	blue = myConfig->blue;
	gred = myConfig->gred;
	ggreen = myConfig->ggreen;
	gblue = myConfig->gblue;
	diffuse_strengh = myConfig->diff_strength;
	glanz_strengh = myConfig->gl_strength;
	diffuse_hard = myConfig->diff_pow;
	glanz_hard = myConfig->gl_pow;
	bm_strengh = myConfig->bump_str;
	services->set_slider(&bm_str_slider, bm_strengh);

	main_form[BM_INVERT].ob_state = myConfig->invert;
	main_form[BM_TEXTURE].ob_state = myConfig->texture;
	main_form[BLACK_BAK].ob_state &= ~OS_SELECTED;
	main_form[WHITE_BAK].ob_state &= ~OS_SELECTED;
	main_form[NORM_BAK].ob_state &= ~OS_SELECTED;

	if (myConfig->background == BLACK_BAK)
		main_form[BLACK_BAK].ob_state |= OS_SELECTED;
	else if (myConfig->background == WHITE_BAK)
		main_form[WHITE_BAK].ob_state |= OS_SELECTED;
	else if (myConfig->background == NORM_BAK)
		main_form[NORM_BAK].ob_state |= OS_SELECTED;

	main_form[BM_PARALEL].ob_state = myConfig->parallel;

	services->redraw_window(&my_window, NULL, BLACK_BAK, 0);
	services->redraw_window(&my_window, NULL, WHITE_BAK, 0);
	services->redraw_window(&my_window, NULL, NORM_BAK, 0);
	services->redraw_window(&my_window, NULL, BM_PARALEL, 0);
	services->redraw_window(&my_window, NULL, BM_TEXTURE, 0);
	services->redraw_window(&my_window, NULL, BM_INVERT, 0);

	if (main_form[M_RADIO_DIFFUSE].ob_state & OS_SELECTED)
	{
		services->set_slider(&red_slider, red);
		services->set_slider(&green_slider, green);
		services->set_slider(&blue_slider, blue);
		services->set_slider(&m_str_slider, diffuse_strengh);
		services->set_slider(&m_hard_slider, diffuse_hard);
	} else
	{
		services->set_slider(&red_slider, gred);
		services->set_slider(&green_slider, ggreen);
		services->set_slider(&blue_slider, gblue);
		services->set_slider(&m_str_slider, glanz_strengh);
		services->set_slider(&m_hard_slider, glanz_hard);
	}
}


static void load_setting(void)
{
	BUMP_CONFIG *myConfig;
	char name[33];

	name[0] = '\0';
	myConfig = services->mconfLoad(&module_info, module_id, name);
	if (myConfig != NULL)
	{
		if (name[0] != '\0')
			strcpy(main_form[LOAD_SET].ob_spec.tedinfo->te_ptext, name);
		apply_setting(myConfig);
		services->redraw_window(&my_window, NULL, LOAD_SET, 0);
	
		services->SMfree(myConfig);
	}
}


/*------------------------- BambmÑbbing ---------------------*/
static short do_it(GARGAMEL *smurf_struct)
{
	SMURF_PIC *picture;
	SMURF_PIC *texture;
	short width, height;
	short twidth = 0;
	short theight = 0;
	short tex_x = 0;
	short tex_y = 0;
	uint8_t *mainpic;
	uint8_t *texpic = 0;
	uint8_t *greypic;
	uint8_t *coltab;
	uint8_t *glanztab;
	uint8_t *c_offset;
	uint8_t *offset;
	uint8_t *noffset;
	uint8_t *y_goffset;
	uint8_t *g_offset;
	uint8_t *t_offset = 0;
	short grey;
	short x, y;
	uint8_t red2, green2, blue2;
	uint8_t gred2, ggreen2, gblue2;
	short nred, ngreen, nblue;
	unsigned short tred, tgreen, tblue;
	long n;
	long t;
	long bpl;
	long gbpl;
	long tbpl = 0;

	long col_size;
	long coltab_size;
	long coltab_size2;
	long glanz_size;
	long glanztab_size;
	long glanz_x, glanz_y;
	long bump_fakt;

	short x_vekt, y_vekt;
	uint8_t o_height;
	long x_dif, y_dif;
	long abstand;

	long bs_diago;

	/*---Bilddaten auslesen----------------------*/
	picture = input_pictures[0];
	width = picture->pic_width;
	height = picture->pic_height;
	mainpic = picture->pic_data;

	bpl = width * 3L;
	gbpl = width;

	if (Obj_Selected(BM_TEXTURE))
	{
		texture = input_pictures[1];
		twidth = texture->pic_width;
		theight = texture->pic_height;
		texpic = texture->pic_data;
		tbpl = twidth * 3L;
	}

	bs_diago = sqrt((long) width * width + (long) height * height);

	/*---Slider umrechnen----------------------*/
	col_size = (long) ((100L - diffuse_hard) * 6.0 + 50);
	glanz_size = (long) ((100L - glanz_hard) / 2.04 + 1);

	red2 = (uint8_t) ((long) red * (long) diffuse_strengh / 100);
	green2 = (uint8_t) ((long) green * (long) diffuse_strengh / 100);
	blue2 = (uint8_t) ((long) blue * (long) diffuse_strengh / 100);

	gred2 = (uint8_t) ((long) gred * (long) glanz_strengh / 100);
	ggreen2 = (uint8_t) ((long) ggreen * (long) glanz_strengh / 100);
	gblue2 = (uint8_t) ((long) gblue * (long) glanz_strengh / 100);

	bump_fakt = (long) (bs_diago / 360.0F * (bm_strengh / 80.0F) * 1024.0F);


	/*-----------------------------------------------------*/

	coltab_size = (long) (bs_diago * col_size / 100L);
	coltab_size2 = coltab_size * coltab_size;
	glanztab_size = (long) (bs_diago * glanz_size / 100L);

	if (Obj_Selected(BM_PARALEL))
	{
		glanz_x = kreuz_x - width / 2;
		glanz_y = kreuz_y - height / 2;
	} else
	{
		glanz_x = kreuz_x;
		glanz_y = kreuz_y;
	}

	/*---Speicher anfordern-----------------------------------*/
	if ((coltab = (uint8_t *)Malloc((long) coltab_size * 3L)) == NULL)
		return M_MEMORY;
	if ((glanztab = (uint8_t *)Malloc((long) coltab_size * 3L)) == NULL)
	{
		Mfree(coltab);
		return (M_MEMORY);
	}
	if ((greypic = (uint8_t *)Malloc((long) width * (long) height)) == NULL)
	{
		Mfree(glanztab);
		Mfree(coltab);
		return M_MEMORY;
	}


	/*---Glanztab lîschen------------------------*/
	g_offset = glanztab;
	for (t = 0; t < (coltab_size * 3L); t++)
		*(g_offset++) = 0;

	/*---Graubild erzeugen------------------------------------*/
	offset = mainpic;
	noffset = greypic;
	if (Obj_Selected(HW_RADIO_GREY))
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				grey = *(offset++);
				grey += *(offset++);
				grey += *(offset++);
				if (Obj_Selected(BM_INVERT))
					*(noffset++) = 255 - (uint8_t) (grey / 3);
				else
					*(noffset++) = (uint8_t) (grey / 3);
			}
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

	if (Obj_Selected(BM_TEXTURE))
	{
		/*---Glanzpunkt-----------------------*/
		c_offset = glanztab;
		for (t = 0; t < glanztab_size; t++)
		{
			n = glanztab_size - t;
			*(c_offset++) = (long) ((long) gred2 * n / glanztab_size);
			*(c_offset++) = (long) ((long) ggreen2 * n / glanztab_size);
			*(c_offset++) = (long) ((long) gblue2 * n / glanztab_size);
		}
	} else
	{
		/*---Glanzpunkt-----------------------*/
		c_offset = coltab;
		for (t = 0; t < glanztab_size; t++)
		{
			n = glanztab_size - t;
			nred = (short) ((long) (*(c_offset + 0L)) + (long) ((long) gred2 * n / glanztab_size));
			ngreen = (short) ((long) (*(c_offset + 1L)) + (long) ((long) ggreen2 * n / glanztab_size));
			nblue = (short) ((long) (*(c_offset + 2L)) + (long) ((long) gblue2 * n / glanztab_size));

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
	}

	/*---Bump Mapping--------------------------------------------*/
	for (y = 0; y < height; y++)
	{
		if ((y & 7) == 0)
			smurf_struct->services->busybox((short)(((long) y << 7L) / (long) height));

		offset = mainpic + y * bpl;
		y_goffset = greypic + y * gbpl;

		if (Obj_Selected(BM_TEXTURE))
		{
			tex_y = y % (theight - 1);
			tex_x = 0;
			t_offset = texpic + tex_y * tbpl;
		}

		for (x = 0; x < width; x++)
		{
			g_offset = y_goffset + x;
			o_height = *(g_offset);

			/*  Soll der Hintergrund (Level 0) mit bearbeitet werden? */
			if (Obj_Selected(NORM_BAK) || o_height != 0)
			{
				x_vekt = 0;
				y_vekt = 0;

				if (x < (width - 1))
					x_vekt += o_height - *(g_offset + 1);
				if (y < (height - 1))
					y_vekt += o_height - *(g_offset + gbpl);
				/* if (x > 0) */
					x_vekt -= o_height - *(g_offset - 1);
				/* if (y > 0) */
					y_vekt -= o_height - *(g_offset - gbpl);

				x_vekt = (short)(((long) x_vekt * bump_fakt) >> 10);
				y_vekt = (short)(((long) y_vekt * bump_fakt) >> 10);

				if (Obj_Selected(BM_PARALEL))
				{
					x_dif = glanz_x - x_vekt;
					y_dif = glanz_y - y_vekt;
				} else
				{
					x_dif = glanz_x - (x_vekt + x);
					y_dif = glanz_y - (y_vekt + y);
				}
				abstand = sqrt(x_dif * x_dif + y_dif * y_dif);

				if (abstand >= coltab_size)
					abstand = coltab_size - 1;

				c_offset = coltab + abstand * 3L;

				if (Obj_Selected(BM_TEXTURE))
				{
					g_offset = glanztab + abstand * 3L;
					tred = (short)((((long) t_offset[0] * (long) c_offset[0]) >> 8) + (long) g_offset[0]);
					tgreen = (short)((((long) t_offset[1] * (long) c_offset[1]) >> 8) + (long) g_offset[1]);
					tblue = (short)((((long) t_offset[2] * (long) c_offset[2]) >> 8) + (long) g_offset[2]);

					if (tred > 255)
						tred = 255;
					if (tgreen > 255)
						tgreen = 255;
					if (tblue > 255)
						tblue = 255;

					*(offset++) = tred;
					*(offset++) = tgreen;
					*(offset++) = tblue;
				} else
				{
					*(offset++) = *(c_offset++);
					*(offset++) = *(c_offset++);
					*(offset++) = *(c_offset++);
				}
			} else
			{
				/*  Hintergrund einfÑrben? */
				if (Obj_Selected(WHITE_BAK))
				{
					*(offset++) = 255;
					*(offset++) = 255;
					*(offset++) = 255;
				} else
				{
					*(offset++) = 0;
					*(offset++) = 0;
					*(offset++) = 0;
				}
			}

			if (Obj_Selected(BM_TEXTURE))
			{
				tex_x += 1;
				t_offset += 3;
				c_offset += 3;
				g_offset += 3;
				if (tex_x >= twidth)
				{
					tex_x -= twidth;
					t_offset -= tbpl;
				}
			}
		}
	}


	Mfree(coltab);
	Mfree(glanztab);
	Mfree(greypic);

	smurf_struct->event_par[0] = 0;
	smurf_struct->event_par[1] = -1;

	return M_PICDONE;
}


/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short t;
	BUMP_CONFIG *Default;
	WORD object;
	BUMP_CONFIG **pp;
	
	services = smurf_struct->services;

	main_form = rs_trindex[BUMPMAIN];

	/* Wenn das Modul aufgerufen wurde, */
	switch (smurf_struct->module_mode)
	{
	case MSTART:
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		module_id = smurf_struct->module_number;

		f_init_sliders();
		f_init_window(module_id);

		smurf_struct->wind_struct = &my_window;
		smurf_struct->services->f_module_window(&my_window);

		f_default_sliders();
		strcpy(main_form[LOAD_SET].ob_spec.tedinfo->te_ptext, rs_frstr[LOAD_SET_STR]);
		
		smurf_struct->module_mode = M_WAITING;
		break;

	case MBEVT:
		object = smurf_struct->event_par[0];

		switch (object)
		{
		case DO_IT:
			smurf_struct->module_mode = M_STARTED;
			return;

		case M_RED_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				red = services->slider(&red_slider);
			else
				gred = services->slider(&red_slider);
			break;

		case M_GREEN_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				green = services->slider(&green_slider);
			else
				ggreen = services->slider(&green_slider);
			break;

		case M_BLUE_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				blue = services->slider(&blue_slider);
			else
				gblue = services->slider(&blue_slider);
			break;

		case M_BRIGHTNESS_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				diffuse_strengh = services->slider(&m_str_slider);
			else
				glanz_strengh = services->slider(&m_str_slider);
			break;

		case M_HARD_SLIDE:
			if (Obj_Selected(M_RADIO_DIFFUSE))
				diffuse_hard = services->slider(&m_hard_slider);
			else
				glanz_hard = services->slider(&m_hard_slider);
			break;

		case BM_STRENGTH_SLIDE:
			bm_strengh = services->slider(&bm_str_slider);
			break;

		case M_RADIO_DIFFUSE:
			services->set_slider(&red_slider, red);
			services->set_slider(&green_slider, green);
			services->set_slider(&blue_slider, blue);
			services->set_slider(&m_str_slider, diffuse_strengh);
			services->set_slider(&m_hard_slider, diffuse_hard);
			break;

		case M_RADIO_GLANZ:
			services->set_slider(&red_slider, gred);
			services->set_slider(&green_slider, ggreen);
			services->set_slider(&blue_slider, gblue);
			services->set_slider(&m_str_slider, glanz_strengh);
			services->set_slider(&m_hard_slider, glanz_hard);
			break;

		case SAVE_SET:
			save_setting();
			break;

		case LOAD_SET:
			load_setting();
			break;
		}

		smurf_struct->module_mode = M_WAITING;
		break;

	case MPICS:
		switch (smurf_struct->event_par[0])
		{
		case TEXTURE:
			if (Obj_Selected(BM_TEXTURE))
			{
				smurf_struct->event_par[0] = 24;
				smurf_struct->event_par[1] = FORM_PIXELPAK;
				smurf_struct->event_par[2] = RGB;
				smurf_struct->module_mode = M_PICTURE;
			} else
			{
				smurf_struct->module_mode = M_WAITING;
			}
			break;

		case BUMPMAP:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;
			smurf_struct->module_mode = M_PICTURE;
			break;

		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

	case MPICTURE:
		/* Bild holen */
		input_pictures[smurf_struct->event_par[0]] = smurf_struct->smurf_pic;

		/* und weiter warten */
		smurf_struct->module_mode = M_WAITING;
		break;

	case MCROSSHAIR:
		/* Smurf fragt: Fadenkreuz? -------------- */
		smurf_struct->event_par[0] = 1;
		smurf_struct->event_par[1] = 0;

		smurf_struct->module_mode = M_CROSSHAIR;
		break;

		/* Smurf Åbergibt mir das Fadenkreuz ----------- */
	case MCH_COORDS:
		kreuz_x = smurf_struct->event_par[0];
		kreuz_y = smurf_struct->event_par[1];
		smurf_struct->module_mode = M_CH_COORDS;
		break;

	case MEXEC:
		/* Wenn das Modul gestartet wurde */
		smurf_struct->module_mode = do_it(smurf_struct);
		break;

	case MTERM:
		/* Wenn das Modul sich verpissen soll */
		smurf_struct->module_mode = M_EXIT;
		break;

	case GETCONFIG:
		Default = smurf_struct->services->SMalloc(sizeof(BUMP_CONFIG));
		write_setting(Default);
		pp = (BUMP_CONFIG **)&smurf_struct->event_par[0];
		*pp = Default;
		smurf_struct->event_par[2] = (short) sizeof(BUMP_CONFIG);
		smurf_struct->module_mode = M_CONFIG;
		break;

	case CONFIG_TRANSMIT:
		pp = (BUMP_CONFIG **)&smurf_struct->event_par[0];
		Default = *pp;
		apply_setting(Default);
		smurf_struct->module_mode = M_WAITING;
		break;
	}
}




/*------ Previewfunktion - wird von Smurf bei Klick aufs Preview aufgerufen.------- */
/* Diese Funktion sollte ein 64*64 Pixel groûes Preview des Bildes nach             */
/* Abarbeitung der Modulfunktion erzeugen. Das Preview muû in der SMURF_PIC-        */
/* Struktur *preview abgelegt werden. Dithering und Darstellung werden von Smurf    */
/* Åbernommen.                                                                      */
/* In prev_struct->smurf_pic liegt das unbearbeitete Bild. Das Modul muû nun        */
/* ein Previewbild in *preview erzeugen. Speicher wurde hierfÅr bereits von Smurf   */
/* angefordert. Das Preview (im Smurf-Standardformat) wird dann vom Hauptprogramm   */
/* fÅr die Screen-Farbtiefe gedithert und im Einstellformular dargestellt.          */

void prev(SMURF_PIC *smurfpic, SMURF_PIC *preview)
{
	/* Ich mach' noch nix. */
	(void) smurfpic;
	(void) preview;
}
