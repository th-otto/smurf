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
 * The Initial Developer of the Original Code is
 * Bjoern Spruck
 *
 * This code is a module for the programm SMURF developed by
 * Olaf Piesche, Christian Eyrich, Dale Russell and Joerg Dittmer
 *         
 * Contributor(s):
 *         
 *
 * ***** END LICENSE BLOCK *****
 */

/*
		Smurf MagicPicture
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "import.h"
#include "smurfine.h"
#include "magicpic.h"

#include "country.h"

#if COUNTRY==1
#include "de/magic.rsh"
#define fehler1 "Die Streifenbreite ist gr”žer als die Texturbreite. Bitte Textur vergr”žern oder Spaltenbreite kleiner w„hlen."
#define fehler2 "Die Streifenbreite zu klein."
#define zielbild_str "Zielbild"
#define textur_str "Textur"
#define hoehen_str "H”henbild"

#elif COUNTRY==0

#include "en/magic.rsh"
#define fehler1 "The ..width ist bigger than the texturewidth. Please increase texturewidth or use a smaller ...width."
#define fehler2 "The ...width is to small."
#define zielbild_str "destination"
#define textur_str "texture"
#define hoehen_str "height"

#elif COUNTRY==2

#include "fr/magic.rsh"
#define fehler1 "Die Streifenbreite ist gr”žer als die Texturbreite. Bitte Textur vergr”žern oder Spaltenbreite kleiner w„hlen."
#define fehler2 "Die Streifenbreite zu klein."
#define zielbild_str "Zielbild"
#define textur_str "Textur"
#define hoehen_str "H”henbild"

#else

#error "Keine Sprache!"

#endif

enum { DEST_PIC = 0, TEXT_PIC, DEEP_PIC };

/* Infostruktur fr Hauptmodul */
MOD_INFO module_info = {
	"Magic Picture",
	0x0060,
	"Bj”rn Spruck",
	{ "", "", "", "", "", "", "", "", "", "" },
	/* Objekttitel */
	{ "", "", "", "" },
	{ "", "", "", "" },
	{ "Streifen", "", "", "" },
	/* Min/Max Slider */
	{ { 0, 128 }, { 0, 128 }, { 0, 128 }, { 0, 128 } },
	/* Min/Max Editfelder */
	{ { 10, 1000 }, { 0, 10 }, { 0, 10 }, { 0, 10 } },
	/* Slider-Defaultwerte */
	{ 0, 0, 0, 0 },
	{ 0, 0, 0, 0 },
	{ 200, 0, 0, 0 },
	/* Anzahl Bilder */
	3,
	/* Bildbeschreibung */
	{ zielbild_str, textur_str, hoehen_str, NULL, NULL, NULL }
};

MOD_ABILITY module_ability = {
	24, 16, 8, 0, 0, 0, 0, 0,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	FORM_PIXELPAK, FORM_PIXELPAK,
	0,
};

static SERVICE_FUNCTIONS *service;
short (*busybox)(short lft);
void (*reset_busybox)(short lft, const char *string);

static SMURF_PIC *picture;
static SMURF_PIC *text_pic;
static SMURF_PIC *deep_pic;

static OBJECT *my_tree;
static WINDOW my_window;


static short get_highest(SMURF_PIC *d)
{
	uint8_t *c = d->pic_data;
	uint8_t m = 0;
	uint8_t *cm;

	cm = c + (long) d->pic_width * d->pic_height;

	while (c < cm)
	{
		if (*c > m)
			m = *c;
		c++;
	}
	return m;
}

/* -------------------------------------------------*/
/* -------------------------------------------------*/
/*		MagicPicture (by Sage) 						*/
/* -------------------------------------------------*/
/* -------------------------------------------------*/
void edit_module_main(GARGAMEL *smurf_struct)
{
	short my_id;
	short t;
	short objct;
	short interpol = 0;
	short sparen = 0;

	service = smurf_struct->services;

	busybox = smurf_struct->services->busybox;
	reset_busybox = smurf_struct->services->reset_busybox;

	switch (smurf_struct->module_mode)
	{
	case MSTART:
		my_id = smurf_struct->module_number;

		my_tree = rs_trindex[SM_MAGIC];

		for (t = 0; t < NUM_OBS; t++)
			rsrc_obfix(&rs_object[t], 0);

		/* WINDOW - Struktur vorbereiten
		 */
		my_window.picture = NULL;
		my_window.clipwid = my_window.cliphgt = 0;
		my_window.resource_form = my_tree;
		my_window.module = my_id;
		my_window.wnum = 1;
		strcpy(my_window.wtitle, "Magic Picture");
		my_window.pic_xpos = my_window.pic_ypos = 0;
		my_window.editob = HOEHE_SW;
		my_window.pflag = 0;
		my_window.prev_window = NULL;
		my_window.next_window = NULL;
		my_window.wx = -1;
		my_window.wy = -1;
		my_window.ww = my_tree[0].ob_width;
		my_window.wh = my_tree[0].ob_height;
		smurf_struct->wind_struct = &my_window;

		if (service->f_module_window(&my_window) != -1)
			smurf_struct->module_mode = M_WAITING;
		else
			smurf_struct->module_mode = M_MODERR;
		break;

	case MPICS:
		if (my_tree[INTERPOL].ob_state & OS_SELECTED)
			interpol = 1;
		if (my_tree[SPEICHER].ob_state & OS_SELECTED)
			sparen = 1;
		/*
		   interpol 
		   alles in TC

		   !interpol && sparen
		   alles (aužer hoehe) belassen wie es ist

		   !interpol && !sparen
		   wenn hinter<=256 in PP wandeln, sonst lassen

		 */
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			/* destpic braucht nicht gewandelt zu werden */
			smurf_struct->module_mode = M_PICTURE;
			break;
		case TEXT_PIC:
			smurf_struct->event_par[0] = 24;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = RGB;

			(void) sparen;
			(void) interpol;
#if 0
			if (interpol)
			{
				smurf_struct->event_par[0] = 24;
				smurf_struct->event_par[1] = FORM_PIXELPAK;
				smurf_struct->event_par[2] = RGB;
			} else
			{
				if (!sparen && <=8)
				{
					smurf_struct->event_par[0] = 8;
					smurf_struct->event_par[1] = FORM_PIXELPAK;
					smurf_struct->event_par[2] = RGB;
				} else
				{						/* else keine aenderung ausser evtl 8Plane->PP */
					if ( == 8 && !=FORM_PIXELPAK)
					{
						smurf_struct->event_par[0] = 8;
						smurf_struct->event_par[1] = FORM_PIXELPAK;
						smurf_struct->event_par[2] = RGB;
					}
				}
			}
#endif
			smurf_struct->module_mode = M_PICTURE;
			break;
		case DEEP_PIC:
			smurf_struct->event_par[0] = 8;
			smurf_struct->event_par[1] = FORM_PIXELPAK;
			smurf_struct->event_par[2] = GREY;
			smurf_struct->module_mode = M_PICTURE;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

	case MPICTURE:
		switch (smurf_struct->event_par[0])
		{
		case DEST_PIC:
			picture = smurf_struct->smurf_pic;
			break;
		case TEXT_PIC:
			text_pic = smurf_struct->smurf_pic;
			break;
		case DEEP_PIC:
			deep_pic = smurf_struct->smurf_pic;
			break;
		default:
			smurf_struct->module_mode = M_WAITING;
			break;
		}
		break;

	case MEXEC:
		{
			short ART = 0;
			short SPALTE = 1;
			short sposch = 0;
			short i;

			if (my_tree[RI1].ob_state & OS_SELECTED)
				ART = 1;
			if (my_tree[RI2].ob_state & OS_SELECTED)
				ART = 2;
			if (my_tree[RI3].ob_state & OS_SELECTED)
				ART = 3;
			if (my_tree[RI4].ob_state & OS_SELECTED)
				ART = 4;
			if (my_tree[RI5].ob_state & OS_SELECTED)
				ART = 5;
			if (my_tree[RI6].ob_state & OS_SELECTED)
				ART = 6;
			if (my_tree[RI7].ob_state & OS_SELECTED)
				ART = 7;
			if (my_tree[RI8].ob_state & OS_SELECTED)
				ART = 8;
			if (my_tree[RI9].ob_state & OS_SELECTED)
				ART = 9;
			if (my_tree[RI10].ob_state & OS_SELECTED)
				ART = 10;

			if (my_tree[SPOSCH1].ob_state & OS_SELECTED)
				sposch = 1;
			if (my_tree[SPOSCH2].ob_state & OS_SELECTED)
				sposch = 0;

			if (my_tree[STRF1].ob_state & OS_SELECTED)
			{
				SPALTE = text_pic->pic_width;
			} else
			{
				SPALTE = atoi(my_tree[ANZSPALT].ob_spec.tedinfo->te_ptext);
				if (SPALTE > text_pic->pic_width)
				{
					service->f_alert(fehler1, "O.K.", 0, 0, 1);
					smurf_struct->module_mode = M_WAITING;
					return;
				}
			}
			if (SPALTE <= 5)
			{
				service->f_alert(fehler2, "O.K.", 0, 0, 1);
				smurf_struct->module_mode = M_WAITING;
				return;
			}

			if (my_tree[SHORTEN].ob_state & OS_SELECTED)
			{
				short MAXPRO = 30;
				short high;
				long l;

				MAXPRO = atoi(my_tree[HOEHE_SW].ob_spec.tedinfo->te_ptext);
				high = get_highest(deep_pic);

				for (i = 0; i < 256; i++)
				{
					l = ((long) i * SPALTE * MAXPRO) / ((long) high * 1000);
					if (l > 255)
						l = 255;
					fuck_tab[i] = (unsigned char) l;
				}
			} else
			{
				for (i = 0; i < 256; i++)
				{
					fuck_tab[i] = (unsigned char) i;
				}
			}

#if 0
			if (interpol)
			{
				if (berechne_inter(sposch, ART, SPALTE, picture, text_pic, deep_pic))
				{
					smurf_struct->module_mode = M_MEMORY;
				} else
				{
					smurf_struct->module_mode = M_PICDONE;
				}
			} else
#endif
			{
				if (berechne_no_inter(sposch, ART, SPALTE, picture, text_pic, deep_pic))
				{
					smurf_struct->module_mode = M_MEMORY;
				} else
				{
					smurf_struct->module_mode = M_PICDONE;
				}
			}
		}
		break;

	case MTERM:
		smurf_struct->module_mode = M_EXIT;
		break;

	case MBEVT:
		objct = smurf_struct->event_par[0];
		if (objct == GO)
			smurf_struct->module_mode = M_STARTED;
		break;
	case MKEVT:
		objct = smurf_struct->event_par[0];
		if (objct == GO)
			smurf_struct->module_mode = M_STARTED;
		break;
	}
}
