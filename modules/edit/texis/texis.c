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

/*              FÅr SMURF Bildkonverter, 19.10.95           */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "import.h"
#include "smurfine.h"

#include "texis.rsh"


#define Clip(a,b)   (a>b? 0 : a)
#define Abs(a)      (a<0? -a : a)


/*----- Typendefinitionen -------------*/
typedef struct
{
	uint8_t *map;
	short map_wid, map_hgt;
	short xscale, yscale;
	short turbulence;
	uint8_t turbulence_mode;
	uint8_t map_mode;
} TEXTURE_DEF;




/*----- Modulinterne Funktionen -------------*/

/*---------- Blinddeklarationen fÅr Smurf-Funktionen -----------------*/

static SERVICE_FUNCTIONS *services;



/*--------------------- globale Variablen ------------------------*/
static WINDOW mwindow;
static OBJECT *maintree;
static uint8_t *map;
static uint8_t *mapdata;
static short map_height, map_width;
static short scaled_wid, scaled_hgt;
static uint8_t redtab[256];
static uint8_t greentab[256];
static uint8_t bluetab[256];
static float fpar1;

static short inter_startup = 1;

/*-------------------- Strukturen --------------------------------*/
static SLIDER xscale_slider;
static SLIDER yscale_slider;
static SLIDER turb_slider;

static POP_UP turbulence_popup;


static TEXTURE_DEF texture_define;


/* Infostruktur fÅr Hauptmodul */
MOD_INFO module_info = {
	"ProTexiS",
	0x0030,
	"Olaf Piesche",
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
	0, 128,
	0, 128,
	0, 128,
	0, 128,
	0, 10,
	0, 10,
	0, 10,
	0, 10,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
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
	0
};

/* **************************************************************** */
/*          Window-Struktur fÅllen und Fenster anfordern            */
/* **************************************************************** */
static void open_window(GARGAMEL *smurf_struct)
{
	/* Window-Struktur fÅllen ------------------------------    */
	smurf_struct->wind_struct = &mwindow;
	strcpy(mwindow.wtitle, "Procedural Textures in Smurf V0.3");

	mwindow.whandlem = 0;				/* evtl. Handle lîschen */
	mwindow.module = smurf_struct->module_number;	/* Welche ID habe ich? */
	mwindow.wnum = 1;
	mwindow.wx = -1;
	mwindow.wy = -1;
	mwindow.ww = maintree->ob_width;
	mwindow.wh = maintree->ob_height;
	mwindow.resource_form = maintree;
	mwindow.picture = 0;
	mwindow.xoffset = 0;
	mwindow.yoffset = 0;
	mwindow.pic_xpos = 0;
	mwindow.pic_ypos = 0;
	mwindow.editob = 0;
	mwindow.nextedit = 0;
	mwindow.editx = 0;
}




/* **************************************************************** */
/*                          Mapfile laden                           */
/* **************************************************************** */
static short load_mapfile(void)
{
	/* FIXME: how to obtain Sys_info.standard_path here? */
	char mappath[] = "F:\\DALEMAN\\SMURF\\MOD_DEVL\\EDITMODS\\TEXTURE\\RANDOM.MAP";
	int handle;
	long f_len;

	handle = (int) Fopen(mappath, FO_READ);

	if (handle > 0)
	{
		f_len = Fseek(0, handle, 2);
		Fseek(0, handle, 0);
		map = (uint8_t *)Malloc(f_len);
		if (map == NULL)
			return M_MEMORY;
		Fread(handle, f_len, map);
		Fclose(handle);
	} else
	{
		form_alert(1, "[0][   Konnte Map   |   nicht laden!!   ][ Uups. ]");
		return M_EXIT;
	}

	texture_define.map_wid = map_width = *(map + 18);
	texture_define.map_hgt = map_height = *(map + 20);
	texture_define.map = mapdata = map + 24;
	return 0;
}





/* **************************************************************** */
/*                  Sliderstrukturen vorbereiten                    */
/* **************************************************************** */
static void make_sliders(void)
{
	xscale_slider.regler = XS_SLIDER;
	xscale_slider.schiene = XS_PAR;
	xscale_slider.rtree = maintree;
	xscale_slider.txt_obj = XS_A;
	xscale_slider.min_val = 1;
	xscale_slider.max_val = 1000;
	xscale_slider.window = &mwindow;

	yscale_slider.regler = YS_SLIDER;
	yscale_slider.schiene = YS_PAR;
	yscale_slider.rtree = maintree;
	yscale_slider.txt_obj = YS_A;
	yscale_slider.min_val = 1;
	yscale_slider.max_val = 1000;
	yscale_slider.window = &mwindow;

	turb_slider.regler = TURB_SLIDER;
	turb_slider.schiene = TS_PAR;
	turb_slider.rtree = maintree;
	turb_slider.txt_obj = TS_A;
	turb_slider.min_val = 0;
	turb_slider.max_val = 100;
	turb_slider.window = &mwindow;
}








/* **************************************************************** */
/*                  Popupstrukturen vorbereiten                     */
/* **************************************************************** */
static void make_popups(void)
{
	turbulence_popup.popup_tree = TURB_POPUP;	/* Objektbaum-Index des Popups */
	turbulence_popup.item = TURB_NONE;	/* Item (=letzter Eintrag)     */
	turbulence_popup.display_tree = maintree;	/* Tree, in dems dargestellt werden soll */
	turbulence_popup.display_obj = TURB_PB;	/* Objekt in display_tree, bei dems dargestellt werden soll */
	turbulence_popup.Cyclebutton = TURB_CB;	/* Index des Cyclebuttons */
}




/* **************************************************************** */
/*                          Turbulenz                               */
/* **************************************************************** */
static void do_turbulence(uint8_t *pixel, short *xp, short *yp)
{
	short xpos, ypos;
	long mapoff;
	short owid, ohgt;
	short newpixel;
	uint8_t *map_data;
	double two_pi;

	two_pi = 2 * M_PI;

	xpos = *xp;
	ypos = *yp;
	newpixel = *pixel;

	owid = texture_define.map_wid;
	ohgt = texture_define.map_hgt;

	map_data = texture_define.map;

	if (texture_define.turbulence_mode == TURB_ZOOM)
	{
		xpos *= 2;
		ypos *= 2;

		if (xpos > owid)
			xpos -= owid;
		if (ypos > ohgt)
			ypos -= ohgt;

		mapoff = (long) ypos *(long) owid;

		newpixel += *(map_data + xpos + mapoff);
		newpixel >>= 1;
		*pixel = newpixel;
	} else if (texture_define.turbulence_mode == TURB_SIN)
	{
		xpos += sin(fpar1) * (owid * texture_define.turbulence / 100);
		ypos += sin(fpar1) * (ohgt * texture_define.turbulence / 100);

		fpar1 += 0.001;
		if (fpar1 > two_pi)
			fpar1 = 0;

		if (xpos > owid)
			xpos -= owid;
		if (ypos > ohgt)
			ypos -= ohgt;
		if (xpos < 0)
			xpos += owid;
		if (ypos < 0)
			ypos += ohgt;

		mapoff = (long) ypos *(long) owid;

		newpixel = *(map_data + xpos + mapoff);
		*pixel = newpixel;
	}
}



/* **************************************************************** */
/*                              Scaling                             */
/* **************************************************************** */
static void do_scaling(short *xp, short *yp)
{
	short xpos, ypos;
	short xsc, ysc;
	short owid, ohgt;

	xsc = texture_define.xscale;
	ysc = texture_define.yscale;

	owid = texture_define.map_wid;
	ohgt = texture_define.map_hgt;

	xpos = (*xp);
	ypos = (*yp);
	scaled_wid = (short) (((long) texture_define.map_wid * (long) xsc) / 100L);
	scaled_hgt = (short) (((long) texture_define.map_hgt * (long) ysc) / 100L);

	xpos = (short) ((xpos * 100L) / xsc);
	ypos = (short) ((ypos * 100L) / ysc);

	if (xpos > owid)
	{
		xpos -= owid;
		inter_startup = 1;
	}
	if (ypos > ohgt)
		ypos -= ohgt;

	*xp = xpos;
	*yp = ypos;
}





/* **************************************************************** */
/*                          Textur erzeugen                         */
/* **************************************************************** */
static void start_texture(GARGAMEL *smurf_struct)
{
	short map_xpos = 0;
	short map_ypos = 0;
	short cur_map_xpos = 0;
	short cur_map_ypos = 0;
	short x, y;
	short width, height;
	uint8_t mappixel;
	uint8_t *pic_data;
	uint8_t *mapcopy;
	long xoffset, yoffset;
	short stx = 0;
	short nx = 0;
	short xdist;
	short greyd;
	uint8_t pix1 = 0;
	uint8_t pix2;

	width = smurf_struct->smurf_pic->pic_width;
	height = smurf_struct->smurf_pic->pic_height;
	pic_data = smurf_struct->smurf_pic->pic_data;
	mapcopy = texture_define.map;
	scaled_wid = map_width;
	scaled_hgt = map_height;

	for (y = 0; y < height; y++)
	{
		if (!(y & 7))
			services->busybox((short) (((long) y << 7L) / (long) height));

		map_xpos = 0;
		nx = 0;
		inter_startup = 1;

		yoffset = (long) y *(long) width *3L;

		for (x = 0; x < width; x++)
		{
			cur_map_xpos = map_xpos;
			cur_map_ypos = map_ypos;

			do_scaling(&cur_map_xpos, &cur_map_ypos);

			mappixel = *(mapcopy + cur_map_xpos + (cur_map_ypos * map_width));


			if (cur_map_xpos == nx || inter_startup == 1)
			{
				inter_startup = 0;
				nx = cur_map_xpos + 1;

				pix1 = mappixel;
				pix2 = *(mapcopy + nx + (cur_map_ypos * map_width));
				greyd = pix2 - pix1;

				xdist = texture_define.xscale / 100;

				stx = greyd / xdist;
			}

			pix1 += stx;
			mappixel = pix1;


			do_turbulence(&mappixel, &cur_map_xpos, &cur_map_ypos);

			map_xpos++;
			if (map_xpos > scaled_wid)
			{
				map_xpos -= scaled_wid;
				inter_startup = 1;
			}

			xoffset = x + x + x;
			*(pic_data + xoffset + yoffset) = redtab[mappixel];
			*(pic_data + xoffset + yoffset + 1) = greentab[mappixel];
			*(pic_data + xoffset + yoffset + 2) = bluetab[mappixel];
		}


		map_ypos++;
		if (map_ypos > scaled_hgt)
			map_ypos -= scaled_hgt;
	}
}






/* **************************************************************** */
/*                      Farbtabelle fÅllen                          */
/* **************************************************************** */
static void fill_colourtable(void)
{
	short t;

	for (t = 0; t < 256; t++)
	{
		redtab[t] = t;
		greentab[t] = t;
		bluetab[t] = t;
	}
}



/* **************************************************************** */
/*                                                                  */
/*  -----------------------  FUNCTION MAIN -----------------------  */
/*                                                                  */
/* **************************************************************** */
void edit_module_main(GARGAMEL *smurf_struct)
{
/*---------- Anmeldevariablen ---------------------------------------*/

/*---------- Modulvariablen ---------------------------------------*/
	short Button;
	WORD t;
	short back;



/* Funktionen holen ----------------------------------------    */
	services = smurf_struct->services;

/* Resource holen ------------------------------------------    */
	maintree = rs_trindex[TEX_MAIN];



	switch (smurf_struct->module_mode)
	{
	/* -----------------------------------------------------------  */
	/* Wenn das Modul zum ersten Mal gestartet wurde,               */
	/* -----------------------------------------------------------  */
	case MSTART:
		/* Resource fix ----------------------------------------    */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		open_window(smurf_struct);
		make_sliders();
		make_popups();
		fill_colourtable();

		if ((back = load_mapfile()) == 0)
			back = smurf_struct->services->f_module_window(&mwindow);

		smurf_struct->services->set_slider(&xscale_slider, 100);
		smurf_struct->services->set_slider(&yscale_slider, 100);
		smurf_struct->services->set_slider(&turb_slider, 50);

		texture_define.xscale = 100;
		texture_define.yscale = 100;
		texture_define.turbulence = 50;


		if (back < 0)
			smurf_struct->module_mode = M_EXIT;
		else
			smurf_struct->module_mode = M_WAITING;

		break;



	/* -----------------------------------------------------------  */
	/*              Buttonevent im Modulfenster                     */
	/* -----------------------------------------------------------  */
	case MBEVT:
		Button = smurf_struct->event_par[0];

		switch (Button)
		{
		case XS_SLIDER:
			texture_define.xscale = smurf_struct->services->slider(&xscale_slider);
			break;
		case YS_SLIDER:
			texture_define.yscale = smurf_struct->services->slider(&yscale_slider);
			break;
		case TURB_SLIDER:
			texture_define.turbulence = smurf_struct->services->slider(&turb_slider);
			break;

		case TURB_PB:
		case TURB_CB:
			texture_define.turbulence_mode = smurf_struct->services->popup(&turbulence_popup, 0, Button, rs_trindex[TURB_POPUP]);
			smurf_struct->services->deselect_popup(&mwindow, TURB_PB, TURB_CB);
			break;

		case STARTIT:
			smurf_struct->module_mode = M_STARTED;
			return;
		}


		smurf_struct->module_mode = M_WAITING;
		break;



	/* -----------------------------------------------------------  */
	/*                      Modul kann loslegen                     */
	/* -----------------------------------------------------------  */
	case MEXEC:
		start_texture(smurf_struct);
		smurf_struct->module_mode = M_PICDONE;
		break;




	/* -----------------------------------------------------------  */
	/*      Mterm empfangen - Speicher freigeben und beenden        */
	/* -----------------------------------------------------------  */
	case MTERM:
		Mfree(map);
		smurf_struct->module_mode = M_EXIT;
		break;
	}
}
