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
 * Olaf Piesche
 *
 * Contributor(s):
 *
 *
 * ***** END LICENSE BLOCK *****
 */

/* --------------------------------------------------------	*/
/*				F�r SMURF Bildkonverter, 25.02.98 			*/
/*															*/
/* Version 1.02  --  04.05.98								*/
/*	  work_in wird vor v_opnbm() f�r das Preview ausgenullt	*/
/* Version 1.03  --  17.05.98								*/
/*	  work_in wird vor v_opnbm() f�r das Bild ausgenullt	*/
/* Version 1.04  --  14.06.98								*/
/*	  Ab jetzt wird auch der MagiC-Fontselektor unterst�tzt	*/
/* Version 1.05  --  13.07.98								*/
/*	  Kleinen Fehler beim Aufruf des MagiC-Fontselektor		*/
/*	  behoben (Workstation wurde zu fr�h geschlossen).		*/
/* =========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "import.h"
#include "smurfine.h"
#include <math.h>
#include "fontsel.h"
#include "bindings.h"

#include "country.h"

#if COUNTRY==1
#include "de/text.rsh"
#elif COUNTRY==0
#include "en/text.rsh"
#elif COUNTRY==2
#include "en/text.rsh"					/* missing french resource */
#else
#error "Keine Sprache!"
#endif

#define Goto_pos(x,y)   ((void) Cconws("\33Y"),  Cconout(' ' + x), Cconout(' ' + y))

/* Infostruktur f�r Hauptmodul */
MOD_INFO module_info = { "Text",
	0x0105,
	"Olaf Piesche",
	{"", "", "", "", "", "", "", "", "", "" },
/* 4 Sliderueberschriften: max 8 */
	{ NULL, NULL, NULL, NULL },
/* 4 Checkboxueberschriften: */
	{ NULL, NULL, NULL, NULL },
/* 4 Edit-Objekt-�berschriften: */
	{ NULL, NULL, NULL, NULL },
/* min/max-Werte fuer Slider */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* min/max f�r Editobjekte */
	{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
/* Defaultwerte fuer Slider, Check und Edit */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
/* Anzahl der Bilder */
	1,
	COMPILER_ID,
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


MOD_ABILITY module_ability = {
	/* Farbtiefen, die vom Modul unterst�tzt werden:            */
	1, 2, 4, 6, 7, 8, 16, 24,
	/*  Dazugeh�rige Datenformate (FORM_PIXELPAK/FORM_STANDARD/FORM_BOTH) */
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_STANDARD,
	FORM_BOTH,
	FORM_PIXELPAK,
	FORM_PIXELPAK,
	0
};

WORD work_in[25];
WORD work_out[57];
OBJECT *alerts;
SERVICE_FUNCTIONS *services;


static OBJECT *maintree;
static WINDOW *mwindow;
static SMURF_PIC *picture;
static SMURF_PIC preview;

static MFDB offscreen;
static WORD oshandle;
static WORD char_height, cell_height, char_width, cell_width;

static XFNT_INFO font_info;


static BOOLEAN handle_aesmsg(GARGAMEL *smurf_struct, FONT_INFO *font)
{
	WORD *msg;
	char name[40];
	char str[10];

	msg = smurf_struct->event_par;

	if (msg[0] == 0x7A18)				/* FONT_CHANGED */
	{
		if (msg[4])
		{
			font->ID = msg[4];
			font->ID = vst_font(oshandle, font->ID);

			font_info.size = 890;
			vqt_xfntinfo(oshandle, 0xFF, font->ID, 0, &font_info);
			font->index = font_info.index;

			vqt_name(oshandle, font->index, name);
			strncpy(maintree[FONTNAME].ob_spec.tedinfo->te_ptext, name, 50);
		}

		if (msg[5])
		{
			font->size = msg[5];
			itoa(font->size, str, 10);
			strcpy(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext, str);
			vst_arbpt32(oshandle, (long) font->size * 65536L, &char_width, &char_height, &cell_width, &cell_height);
		}

		services->redraw_window(mwindow, NULL, FONTNAME, 0);
		services->redraw_window(mwindow, NULL, FONTSIZE, 0);
		return TRUE;
	}

	return FALSE;
}




static void f_doit(GARGAMEL *smurfstruct, SMURF_PIC *picture, FONT_INFO *font)
{
	WORD pxy[10];
	WORD extent[10];
	WORD doithandle;
	MFDB osbmp;
	long size;
	long byte_width;
	WORD dummy;
	SMURF_PIC *destination;

	/*
	 * MFDB vorbereiten und Offscreen-Bitmap anmelden 
	 */
	vqt_real_extent(oshandle, 0, 0, maintree[TEXT_OB].ob_spec.tedinfo->te_ptext, extent);

	byte_width = ((extent[2] + 15) / 16) * 2;
	size = byte_width * (long) extent[5];
	osbmp.fd_addr = smurfstruct->services->SMalloc(size);
	osbmp.fd_w = ((extent[2] + 15) / 16) * 16;
	osbmp.fd_h = extent[5];
	osbmp.fd_wdwidth = (extent[2] + 15) / 16;
	osbmp.fd_stand = 0;
	osbmp.fd_nplanes = 1;

	memset(work_in, 0, sizeof(work_in));
	memset(osbmp.fd_addr, 0, size);
	work_in[11] = osbmp.fd_w - 1;
	work_in[12] = osbmp.fd_h - 1;
	v_opnbm(work_in, &osbmp, &doithandle, work_out);

	/*
	 * Fonteinstellungen f�r die neue osbmp �bernehmen 
	 */
	vst_load_fonts(doithandle, 0);
	vst_font(doithandle, font->ID);
	vst_color(doithandle, 1);
	vst_rotation(doithandle, 0);		/* unrotiert... */
	vswr_mode(doithandle, MD_REPLACE);
	vst_effects(doithandle, 0);
	vst_skew(doithandle, 0);
	vst_kern(doithandle, 0, 1, &dummy, &dummy);	/* Pair-Kerning ein */
	vst_alignment(doithandle, 0, 5, &dummy, &dummy);

	vst_arbpt32(doithandle, (long) font->size * 65536L, &char_width, &char_height, &cell_width, &cell_height);

	/*
	 * Clipping ein und rein mit dem Text
	 */
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = extent[4] - 1;
	pxy[3] = extent[5] - 1;
	vs_clip(doithandle, TRUE, pxy);

	v_ftext(doithandle, 0, 0, maintree[TEXT_OB].ob_spec.tedinfo->te_ptext);

	if (picture->changed == 255)		/* �bergebenes Bild ist schon ein Block */
	{
		Mfree(picture->pic_data);
		destination = picture;
	} else if (picture->block != NULL)	/* kein Block aber einer im �bergebenen Bild */
	{
		Mfree(picture->block->pic_data);
		destination = picture->block;
	} else								/* Block mu� neu erstellt werden */
	{
		picture->block = smurfstruct->services->SMalloc(sizeof(SMURF_PIC));
		destination = picture->block;
	}

	destination->changed = 255;
	destination->pic_data = osbmp.fd_addr;
	destination->pic_width = osbmp.fd_w;
	destination->pic_height = osbmp.fd_h;
	destination->depth = 1;
	destination->block = NULL;
	destination->prev_picture = NULL;
	destination->next_picture = NULL;
	destination->screen_pic = NULL;
	destination->format_type = FORM_STANDARD;
	destination->col_format = RGB;
	destination->zoom = 0;

	destination->palette[0] = 255;
	destination->palette[1] = 255;
	destination->palette[2] = 255;
	destination->palette[3] = 0;
	destination->palette[4] = 0;
	destination->palette[5] = 0;

	vst_unload_fonts(doithandle, 0);
	v_clsbm(doithandle);
}



static void compute_preview(void)
{
	WORD pxy[10];

	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = 403;
	pxy[3] = 71;

	/*
	 * Preview l�schen und Text reinschreiben
	 */
	memset(preview.pic_data, 0, 416L * 72L / 8L);
	vs_clip(oshandle, TRUE, pxy);
	v_ftext(oshandle, 0, 0, maintree[TEXT_OB].ob_spec.tedinfo->te_ptext);
}

/*---------------------------  FUNCTION MAIN -----------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	WORD t;
	short back;
	short wind_num = 1;
	WORD Button;
	unsigned long dummyl;
	WORD dummy;
	char name[40];
	char str[6];
	static FONT_INFO font;


	/*
	 * Hier werden die Funktionen aus der GARGAMEL-Struktur geholt.
	 */
	services = smurf_struct->services;

	picture = smurf_struct->smurf_pic;

	switch (smurf_struct->module_mode)
	{
	/* 
	 * Wenn das Modul zum ersten Mal gestartet wurde 
	 */
	case MSTART:
		maintree = rs_trindex[TEXT_MAIN];	/* Resourcebaum holen */
		alerts = rs_trindex[ALERT_STRINGS];	/* Resourcebaum holen */

		if (get_cookie(0x45644449L, &dummyl) == 0) /* 'EdDI' */
		{
			services->f_alert(alerts[NO_NVDI].ob_spec.tedinfo->te_ptext, NULL, NULL, NULL, 1);
			smurf_struct->module_mode = M_MODERR;
			return;
		}


		/* Resource umbauen */
		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(rs_object, t);

		mwindow = (WINDOW *) smurf_struct->services->SMalloc(sizeof(WINDOW));
		memset(mwindow, 0, sizeof(WINDOW));
		mwindow->whandlem = 0;			/* evtl. Handle l�schen */
		mwindow->module = smurf_struct->module_number;		/* ID in die Fensterstruktur eintragen  */
		strcpy(mwindow->wtitle, "Text V0.1");	/* Titel reinkopieren   */
		mwindow->wnum = wind_num;		/* Fenster nummer 1...  */
		mwindow->wx = -1;				/* Fenster X-...    */
		mwindow->wy = -1;				/* ...und Y-Pos     */
		mwindow->ww = maintree[0].ob_width;	/* Fensterbreite    */
		mwindow->wh = maintree[0].ob_height;	/* Fensterh�he      */
		mwindow->resource_form = maintree;	/* Resource         */
		mwindow->picture = NULL;		/* Preview als Bild */
		mwindow->pic_xpos = maintree[PREVIEW].ob_x;
		mwindow->pic_ypos = maintree[PREVIEW].ob_y + 1;
		mwindow->clipwid = 404;
		mwindow->cliphgt = 72;
		mwindow->editob = TEXT_OB;
		mwindow->nextedit = FONTSIZE;
		mwindow->editx = 0;

		smurf_struct->wind_struct = mwindow;	/* und die Fensterstruktur in die Gargamel */

		/*
		 * Preview-Smurfpic-Struktur
		 */
		preview.pic_width = 416;
		preview.pic_height = 72;
		preview.depth = 1;
		preview.pic_data = smurf_struct->services->SMalloc(416L * 72L / 8L);
		preview.screen_pic = NULL;
		preview.changed = 0;
		preview.format_type = FORM_STANDARD;
		preview.col_format = RGB;
		preview.zoom = 0;
		preview.block = NULL;
		preview.local_nct = NULL;
		preview.prev_picture = NULL;
		preview.next_picture = NULL;
		memset(preview.pic_data, 0, 416L * 72L / 8L);

		/*
		 * MFDB vorbereiten und Offscreen-Bitmap anmelden 
		 */
		offscreen.fd_addr = preview.pic_data;
		offscreen.fd_w = 416;
		offscreen.fd_h = 72;
		offscreen.fd_wdwidth = (416 + 15) / 16;
		offscreen.fd_stand = 0;
		offscreen.fd_nplanes = 1;
		memset(work_in, 0, sizeof(work_in));
		work_in[11] = offscreen.fd_w - 1;
		work_in[12] = offscreen.fd_h - 1;
		v_opnbm(work_in, &offscreen, &oshandle, work_out);

		/*
		 * Font-Voreinstellungen
		 */
		Goto_pos(1, 0);
		font.size = 24;
		vst_load_fonts(oshandle, 0);

		vst_color(oshandle, 1);
		vst_rotation(oshandle, 0);		/* unrotiert... */
		vswr_mode(oshandle, MD_REPLACE);
		vst_effects(oshandle, 0);
		vst_skew(oshandle, 0);
		vst_kern(oshandle, 0, 1, &dummy, &dummy);	/* Pair-Kerning ein */

		vst_arbpt32(oshandle, (long) font.size * 65536L, &char_width, &char_height, &cell_width, &cell_height);

		vst_alignment(oshandle, 0, 5, &dummy, &dummy);

		font.ID = vst_font(oshandle, 0);

		font_info.size = sizeof(XFNT_INFO);
		vqt_xfntinfo(oshandle, 0xFF, font.ID, 0, &font_info);
		font.index = font_info.index;

		vqt_name(oshandle, font.index, name);
		strncpy(maintree[FONTNAME].ob_spec.tedinfo->te_ptext, name, 50);

		font.size = atoi(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext);
		vst_arbpt32(oshandle, (long) font.size * 65536L, &char_width, &char_height, &cell_width, &cell_height);

		strcpy(maintree[TEXT_OB].ob_spec.tedinfo->te_ptext, "The quick blue Smurf jumps over the lazy pic");
		strcpy(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext, "24");

		back = smurf_struct->services->f_module_window(mwindow);	/* Gib mir 'n Fenster! */
		if (back == -1)					/* keins mehr da? */
		{
			smurf_struct->services->SMfree(mwindow);
			v_clsbm(oshandle);
			smurf_struct->module_mode = M_EXIT;
		} else							/* doch? Ich warte... */
		{
			compute_preview();
			mwindow->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
		}
		break;

	/*
	 * Buttonevent im Modulfenster
	 */
	case MBEVT:
		Button = smurf_struct->event_par[0];

		switch (Button)
		{
		case FONTNAME:
			if (call_fontsel(oshandle, &font) > 0)
			{
				font.ID = vst_font(oshandle, font.ID);

				font_info.size = 890;
				vqt_xfntinfo(oshandle, 0xFF, font.ID, 0, &font_info);
				font.index = font_info.index;

				vqt_name(oshandle, font.index, name);
				strncpy(maintree[FONTNAME].ob_spec.tedinfo->te_ptext, name, 50);

				itoa(font.size, str, 10);
				strcpy(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext, str);
				vst_arbpt32(oshandle, (long) font.size * 65536L, &char_width, &char_height, &cell_width, &cell_height);

				compute_preview();
				mwindow->picture = &preview;
				smurf_struct->services->redraw_window(mwindow, NULL, FONTNAME, 0);
				smurf_struct->services->redraw_window(mwindow, NULL, FONTSIZE, 0);
				smurf_struct->module_mode = M_MODPIC;
			} else
			{
				smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
			}
			break;
		case TEXT_START:
			font.size = atoi(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext);
			smurf_struct->module_mode = M_STARTED;	/* Ich will loslegen!... */
			break;
		case PREVIEW:
		case TEXT_OB:
		case FONTSIZE:
			font.size = atoi(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext);
			vst_arbpt32(oshandle, (long) font.size * 65536L, &char_width, &char_height, &cell_width, &cell_height);

			compute_preview();
			mwindow->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;	/* Ich warte... */
			break;
		}
		break;

	/*
	 * Keyboardevent
	 */
	case MKEVT:
		if (smurf_struct->event_par[0] == TEXT_OB || smurf_struct->event_par[0] == FONTSIZE)
		{
			if (smurf_struct->event_par[0] == FONTSIZE)
			{
				font.size = atoi(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext);
				vst_arbpt32(oshandle, (long) font.size * 65536L, &char_width, &char_height, &cell_width, &cell_height);
			}

			compute_preview();
			mwindow->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
		} else if (smurf_struct->event_par[0] == TEXT_START)
		{
			font.size = atoi(maintree[FONTSIZE].ob_spec.tedinfo->te_ptext);
			smurf_struct->module_mode = M_STARTED;	/* Ich will loslegen!... */
		}
		break;

	/*
	 * Preview fertig?
	 */
	case MDITHER_READY:
		smurf_struct->services->redraw_window(mwindow, NULL, PREVIEW, 0);
		smurf_struct->module_mode = M_WAITING;
		break;

	/*
	 * Los gehts
	 */
	case MEXEC:
		f_doit(smurf_struct, picture, &font);
		smurf_struct->module_mode = M_PICDONE;	/* Fertig!... */
		break;

	/* 
	 * Mterm empfangen - Speicher freigeben und beenden
	 */
	case MTERM:
		smurf_struct->services->SMfree(mwindow);
		smurf_struct->services->SMfree(preview.pic_data);
		vst_unload_fonts(oshandle, 0);
		v_clsbm(oshandle);
		smurf_struct->module_mode = M_EXIT;
		break;
	
	case SMURF_AES_MESSAGE:
		if (handle_aesmsg(smurf_struct, &font) > 0)
		{
			compute_preview();
			mwindow->picture = &preview;
			smurf_struct->module_mode = M_MODPIC;
		} else
		{
			smurf_struct->module_mode = M_WAITING;
		}
		break;
	}
}
